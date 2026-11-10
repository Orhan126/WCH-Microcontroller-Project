# CH32V003 – ADC-Controlled RGB LED Color Blending System

## Project Overview

This project demonstrates an ADC-controlled RGB LED color blending system using the **WCH CH32V003F4P6 RISC-V microcontroller**.

A potentiometer provides an analog input to the microcontroller. The ADC reads this input and maps it to a hue value. The program then converts the HSV color representation into RGB values and uses PWM signals to control the LED's red, green, and blue channels.

## Hardware Used

- WCH CH32V003F4P6 microcontroller
- RGB LED
- Potentiometer
- Connecting wires
- WCH-Link programmer/debugger

## Software and Tools

- PlatformIO
- Visual Studio Code
- WCH NoneOS SDK
- C programming language

## How It Works

1. The potentiometer provides an analog voltage to pin PD4.
2. The ADC reads the voltage and produces a 10-bit value.
3. The ADC value is mapped to a hue range.
4. The HSV-to-RGB conversion function calculates the RGB values.
5. Timer 1 generates PWM signals to control the RGB LED channels.
6. The LED color changes as the potentiometer position changes.

## Pin Configuration

| Function | Microcontroller Pin | Peripheral |
|---|---|---|
| ADC Input | PD4 | ADC Channel 7 |
| Red LED | PD2 | TIM1 Channel 1 |
| Green LED | PC3 | TIM1 Channel 3 |
| Blue LED | PC4 | TIM1 Channel 4 |

## Main Features

- ADC-based analog input reading
- HSV-to-RGB color conversion
- PWM-based RGB LED control
- Real-time color adjustment
- Serial output for monitoring ADC and RGB values

## Project Structure

The main application is located in `ch32Test/src/main.c`.

```text
WCH-Microcontroller-Project/
├── ch32Test/
│   ├── include/
│   ├── lib/
│   ├── src/
│   │   └── main.c
│   ├── test/
│   ├── platformio.ini
│   └── .gitignore
├── include/
├── lib/
├── src/
│   └── main.c
├── test/
├── platformio.ini
└── README.md
```

## Learning Outcomes

- Working with ADC peripherals on a RISC-V microcontroller
- Configuring timer-based PWM outputs
- Understanding HSV and RGB color models
- Writing embedded C programs
- Using PlatformIO for microcontroller development

## Author

CSE Student | Embedded Systems and Microcontroller Projects