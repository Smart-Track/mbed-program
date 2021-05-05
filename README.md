# Smart Track - Mbed Program

Smart Track is a modular race track controlled a master PC GUI and a slave ARM Mbed. This repo contains the Mbed code. For the GUI code, see [the Smart Track GUI repo](https://github.com/Smart-Track/GUI-program).

Smart Track has three checkpoints, including one starting line checkpoint. These detect passing objects using sonar and can be placed anywhere to define the shape of the track. All hardware is controlled by the Mbed, which must be connected to a device running the GUI program. The GUI starts the race and resets the Mbed when the race is over. It also runs the race timer and keeps a record of past races.

On startup, the Mbed waits to be started by the GUI before begining a starting countdown. The race begins after the countdown finishes and the player first crosses the starting line. During the race, the player must cross each checkpoint in order. When they cross a checkpoint, its LEDs will turn on and a sound will play to indicate that the checkpoint has been crossed. The Mbed will then send this information to the GUI for timing. The race is finished after completing three laps, at which point the player will be prompted to enter their initials into the GUI for placement on a leaderboard. The Mbed can then be reset by the GUI to start another race.

# Setup

These instructions will walk you through hardware assembly and Mbed setup. For GUI setup, see [the Smart Track GUI repo](https://github.com/Smart-Track/GUI-program).

## Parts List

| Component     | Quantity |
|:-------------:|:--------:|
| [HC-SR04](https://os.mbed.com/platforms/mbed-LPC1768) | 1 |
| 5 mm LED (red) | 6 |
| RGB LED | 3 |
| 100 Ω resistor | 12
| [HC-SR04](https://os.mbed.com/users/4180_1/notebook/using-the-hc-sr04-sonar-sensor) | 3 |
| 2N3904 NPN BJT | 3 |
| 7-segment display | 1 |
| Speaker | 1 |
| [TPA2005D1](https://os.mbed.com/users/4180_1/notebook/tpa2005d1-class-d-audio-amp) | 1 |
| 5V power supply | 1 |
| 2.1 mm barrel jack | 1 |

Additionally, wood (or any other material) is required to form the physical structure of the checkpoints.

## Asembly

![block_diagram](https://user-images.githubusercontent.com/68122426/117152689-42a1f580-ad88-11eb-8b8b-d0689d2a9451.png)


### LED Circuits

First, assemble the RGB LED circuit for the starting line. The blue pin for the RGB LEDs are unused, so leave it unconnected.

![RGB_schematic](https://user-images.githubusercontent.com/68122426/117109109-25523480-ad52-11eb-9fd8-748ab086ec09.png)

Next, assemble the checkpoint LED circuits. You will need two of these in total.

![LED_schematic](https://user-images.githubusercontent.com/68122426/117110742-98f54100-ad54-11eb-8d1d-967ae51c9b38.png)

### Checkpoints

Now, assemble the checkpoints. You will need three in total. An example is shown below that includes hinges for foldability, but this is optional. The only things actually necessary for a checkpoint are two vertical posts. Each checkpoint does **not** have to be the same width, but for best results, the posts should be at least a foot apart. The height of the each checkpoint does not matter.

![20210420_115959](https://user-images.githubusercontent.com/68122426/117111606-e0300180-ad55-11eb-874d-566d390d71e2.jpg)

With the checkpoints assembled, mount one HC-SR04 sonar unit to each one of the vertical posts. Mount them as close to the ground as possible, but be careful that the sonar is level and pointed directly at the other post. Next mount the LED circuits somewhere on each of the checkpoints. They are all mounted in a plastic housing in the example below, but this is not necessary. 

![20210430_113106](https://user-images.githubusercontent.com/68122426/117148433-35830780-ad84-11eb-851b-cf86ac5a03ca.jpg)

For the starting line checkpoint, also mount the 7-segment display and speaker, along with the TPA2005D1 class D amp.

![20210430_113053](https://user-images.githubusercontent.com/68122426/117148536-4e8bb880-ad84-11eb-937e-a692eaf5ea06.jpg)

## Wiring

The USB cable is unable to supply enough power for all of the components, so an external 5V power supply is necessary. Be careful to connect the components **only** to the power supply and **not** to the Mbed output.

### Starting Line

#### HC-SR04 Sonar

| Mbed          | HC-SR04  | Power Supply |
|:-------------:|:--------:|:--------:|
|     | Vcc | + 
| GND | Gnd | -
| p15 | Trig |
| p16 | Echo |

#### RGB LED Circuit

| Mbed          | LED Circuit |
|:-------------:|:--------:|
| GND | GND |
| p8 | CTRL1 |
| p9 | CTRL2 |
| p10 | CTRL3 |
| p23 | RED |
| p24 | GRN |

#### Speaker

| Mbed          | TPA2005D1 | Speaker | Power Supply |
|:-------------:|:--------:|:---:|:--------:|
|  | PWR + | | +
|  | S | | +
| GND | PWR - | | -
| GND | IN - | | -
| p25 | IN + | |
| | OUT + | +
| | OUT - | -

Optional: For volume control, add a potentiometer on the VOL pins, with the potentiometer wiper pin on the center VOL pin. The remaining potentiometer legs go to the other VOL pins. Orientation does not matter; it will only change which way the potentiometer needs to be turned to increase/decrease volume.

#### 7-Segment Display

| Mbed          | 7-Segment | Power Supply |
|:-------------:|:--------:|:--------:|
|  | VCC | + 
| GND | GND | -
| p28 | RX |
| | TX |

The TX pin on the 7-segment display is a no connect.

---

### Checkpoint 1

#### HC-SR04 Sonar

| Mbed          | HC-SR04  | Power Supply |
|:-------------:|:--------:|:--------:|
|  | Vcc | +
| GND | Gnd | -
| p17 | Trig |
| p18 | Echo |

#### LED Circuit

| Mbed          | LED Circuit | Power Supply |
|:-------------:|:--------:|:--------:|
| GND | GND | -
| p6 | VCC |

---

### Checkpoint 2

#### HC-SR04 Sonar

| Mbed          | HC-SR04  | Power Supply |
|:-------------:|:--------:|:--------:|
|  | Vcc | +
| GND | Gnd | -
| p21 | Trig |
| p22 | Echo |

#### LED Circuit

| Mbed          | LED Circuit | Power Supply |
|:-------------:|:--------:|:--------:|
| GND | GND | -
| p7 | VCC |

## Mbed Software

Either compile the contents of `/Smart-Track/mbed-program/mbed_code/` or download `Smart_Track.LPC1768.bin` to the Mbed.
