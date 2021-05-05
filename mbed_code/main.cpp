//************************//************************//
//       Mbed to PC       //       PC to Mbed       //
//************************//************************//
// 0 - past start, lap 1  // S - start race         //
// 1 - past cp1, lap 1    // R - reset              //
// 2 - past cp2, lap 1    //                        //
// 3 - past start, lap 2  //                        //
// ...                    //                        //
// 9 - end race           //                        //
//************************//************************//

#include "mbed.h"
#include "ultrasonic.h"
#include "rtos.h"
#include "SevenSegmentSerial.h"

Thread      thread1;
Thread      thread2;
Thread      thread3;
Serial      pc(USBTX,USBRX);
DigitalOut  led1(p6);
DigitalOut  led2(p7);
PwmOut      speaker(p25);
DigitalOut  rgbRed(p23);
DigitalOut  rgbGreen(p24);
DigitalOut  rgbCtrl1(p8);
DigitalOut  rgbCtrl2(p9);
DigitalOut  rgbCtrl3(p10);
SevenSegmentSerial ssd(UART_MODE, p28);

#define     B4NOTE  500 //Hz
#define     E5NOTE  659 //Hz

int width[3];           //width of each checkpoint, mm
int threshold[3];       //how much distance has to change, mm
float sensitivity[3] = {0.1,0.1,0.1};
int total_laps = 3;
int current_lap = 0;
int i = 500;

bool early_start = 0;
bool start_done = 0;

int setup[3] = {1,1,1};

//state machine, tracks current position in track
enum state_type {start,past_start,past_cp1,past_cp2};
state_type track_state = start;

//Mbed reset
extern "C" void mbed_reset();

//starting countdown
void start_race()
{
    // 3
    ssd.setDigit(3,0);
    rgbRed = 1;
    speaker.period(1.0/B4NOTE);
    speaker =0.5;
    rgbCtrl1 = 1;
    Thread::wait(500);
    speaker = 0;
    Thread::wait(500);
    // 2
    ssd.setDigit(2,0);
    speaker =0.5;
    rgbCtrl2 = 1;
    Thread::wait(500);
    speaker = 0;
    Thread::wait(500);
    // 1
    ssd.setDigit(1,0);
    speaker =0.5;
    rgbCtrl3 = 1;
    Thread::wait(500);
    speaker = 0;
    Thread::wait(500);
    // GO!
    ssd.write("GOOO");
    rgbRed = 0;
    rgbGreen = 1;
    start_done = 1;
    speaker.period(1.0/E5NOTE);
    speaker =0.5;
    Thread::wait(1000);
    speaker = 0;
    rgbCtrl1 = 0;
    rgbCtrl2 = 0;
    rgbCtrl3 = 0;
}

//false start speaker
void false_speaker()
{
    speaker.period(1.0/B4NOTE);
    while(1) {
        speaker = rgbRed ? 0.5 : 0.0;   //sync with flashing LEDs
        Thread::wait(10);
    }
}

//plays bleep for crossing finish line
void play_audio()
{
    speaker.period(1.0/B4NOTE);
    speaker =0.5;
    Thread::wait(150);
    speaker=0.0;
}

//executed when crossing starting line
void start_line(int distance)
{
    //calibration
    if (setup[0]) {
        width[0] = distance;
        threshold[0] = distance*sensitivity[0];
        setup[0] = !setup[0];
    }

    if (width[0] - distance >= threshold[0]) {
        led1 = 0;
        led2 = 0;
        //RGB LED lap counter
        switch (current_lap) {
            case 1:
                rgbRed = 1;
                rgbGreen = 0;
                rgbCtrl1 = 1;
                rgbCtrl2 = 0;
                rgbCtrl3 = 0;
                break;
            case 2:
                rgbCtrl2 = 1;
                break;
            case 3: 
                rgbCtrl3 = 1;
                break;
            default:
                break;
        }
        current_lap++;
        track_state = past_start;
        pc.putc('0' + (current_lap*total_laps) - 3);    //send lap char to PC
        thread1.start(&play_audio);
    }
}

//executed when passing checkpoint 1
void checkpoint1(int distance)
{
    //calibration
    if (setup[1]) {
        width[1] = distance;
        threshold[1] = distance*sensitivity[1];
        setup[1] = !setup[1];
    }

    if (width[1] - distance >= threshold[1]) {
        led1 = 1;
        track_state = past_cp1;
        pc.putc('0' + (current_lap*total_laps) - 2);   //send lap char to PC 
        thread1.start(&play_audio);
    }
}

//executed when passing checkpoint 2
void checkpoint2(int distance)
{
    //calibration
    if (setup[2]) {
        width[2] = distance;
        threshold[2] = distance*sensitivity[2];
        setup[2] = !setup[2];
    }

    if (width[2] - distance >= threshold[2]) {
        led2 = 1;
        track_state = past_cp2;
        pc.putc('0' + (current_lap*total_laps) - 1);   //send lap char to PC 
        thread1.start(&play_audio);
    }
}

ultrasonic  sonar0(p15,p16,0.1,0.1,&start_line);      //starting line
ultrasonic  sonar1(p17,p18,0.1,0.1,&checkpoint1);     //checkpoint 1
ultrasonic  sonar2(p21,p22,0.1,0.1,&checkpoint2);     //checkpoint 2

int main()
{
    //clear SSD
    ssd.clear();

    //sonar initialization
    sonar0.startUpdates();
    sonar1.startUpdates();
    sonar2.startUpdates();

    //sonar calibration
    sonar0.checkDistance();
    sonar1.checkDistance();
    sonar2.checkDistance();

    //wait for start char from PC
    while(1) {
        if (pc.getc() == 'S') {
            break;
        }
        Thread::wait(50);
    }

    //starting countdown sequence
    thread2.start(&start_race);

    //false start detect
    while(!start_done) {
        sonar0.checkDistance();
        if (current_lap) {
            thread2.terminate();
            thread3.start(&false_speaker);
            rgbRed = 0;
            rgbGreen = 0;
            rgbCtrl1 = rgbCtrl2 = rgbCtrl3 = 1;
            led1 = led2 = 0;
            //flash all LEDs
            while(1) {
                rgbRed = !rgbRed;
                led1 = !led1;
                led2 = !led2;
                Thread::wait(200);
            }
        }
    }

    while(1) {
        switch (track_state) {
            case start:
                sonar0.checkDistance();
                break;
            case past_start:
                sonar1.checkDistance();
                break;
            case past_cp1:
                sonar2.checkDistance();
                break;
            case past_cp2:
                sonar0.checkDistance();
                break;
        }
        if (current_lap > total_laps) {
            pc.putc('9');
            //wait for reset char from PC
            while(1) {
                if (pc.getc() == 'R') {
                    mbed_reset();
                }
                Thread::wait(50);
            }
        }
    }
}
