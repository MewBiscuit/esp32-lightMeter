# esp32-lightMeter

This is an example project for interfacing with an SSD1306 OLED display using the ESP-IDF framework. The project reads an analog input value from a photoresistor and displays it on the OLED screen along with a set of LED indicators and a sun icon to represent light levels. The LED indicators represent different light level thresholds.

## Dependencies

This project depends on the [nopnop2002/esp-idf-ssd1306](https://github.com/nopnop2002/esp-idf-ssd1306) library. Make sure to install this component in your ESP-IDF development environment before compiling and running this code. You can find installation instructions in the provided link.

## Hardware Setup

Connect your SSD1306 OLED display and LEDs to the GPIO pins as specified in the code. Ensure that the power supply and communication lines are correctly connected.

## How to Build and Flash

Follow these steps to build and flash the code to your ESP32 device:

1. Clone this repository to your local machine.

2. Open a terminal and navigate to the project directory.

3. Build the project by running the following command:
```
idf.py build
```
4. Flash the firmware to your ESP32 device:
```
idf.py -p [PORT] flash
```
Replace `[PORT]` with the correct serial port for your ESP32 device.

5. Monitor the output:
```
idf.py -p [PORT] monitor
```
This will open a serial monitor that displays the output of your ESP32.

## Functionality

The code reads an analog input from `ADC_CHANNEL_4` and determines the light level based on thresholds. It then displays the analog value on the OLED screen and controls LEDs according to the light level. Additionally, a sun icon is displayed on the OLED screen when the light level exceeds a certain threshold.

## Customization

You can customize the code to use different GPIO pins for the OLED display, LEDs, and the analog input pin by modifying the corresponding definitions in the code. Additionally, you can adjust the light level thresholds and icon images to suit your application.

Feel free to explore and modify the code as needed for your project.

Enjoy working with your SSD1306 OLED display and ESP32!

![Project_In_Action](Example.jpg)
