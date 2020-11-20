/*
 *  PrintMotorDiagram.cpp
 *  Prints PWM, distance and speed diagram of an encoder motor.
 *
 *
 *  Copyright (C) 2020  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of Arduino-RobotCar https://github.com/ArminJo/PWMMotorControl.
 *
 *  PWMMotorControl is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>
#include "CarMotorControl.h"

#if !defined(USE_ENCODER_MOTOR_CONTROL)
//#error For this example to run, USE_ENCODER_MOTOR_CONTROL must be commented out / defined in PWMDCMotor.h line 42
#endif

#if ! defined(USE_ADAFRUIT_MOTOR_SHIELD) // enable / disable it in PWMDCMotor.h
/*
 * Pins for direct motor control with PWM and a dual full bridge e.g. TB6612 or L298.
 * 2 + 3 are reserved for encoder input
 */
#define PIN_RIGHT_MOTOR_FORWARD     4 // IN4 <- Label on the L298N board
#define PIN_RIGHT_MOTOR_BACKWARD    7 // IN3
#define PIN_RIGHT_MOTOR_PWM         5 // ENB - Must be PWM capable

#define PIN_LEFT_MOTOR_FORWARD      9 // IN1
#define PIN_LEFT_MOTOR_BACKWARD     8 // IN2
#define PIN_LEFT_MOTOR_PWM          6 // ENA - Must be PWM capable
#endif

#define LEFT_MOTOR_INTERRUPT     INT1 // Pin 3

EncoderMotor MotorUnderTest;

void setup() {
// initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL)
    delay(2000); // To be able to connect Serial monitor after reset and before first printout
#endif
    // Just to know which program is running on my Arduino
//    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_PWMMOTORCONTROL));

    // Print caption
    Serial.println(F("PWM Speed Average Encoder"));

#ifdef USE_ADAFRUIT_MOTOR_SHIELD
    // For Adafruit Motor Shield v2
    MotorUnderTest.init(1, LEFT_MOTOR_INTERRUPT);
#else
    MotorUnderTest.init(PIN_LEFT_MOTOR_FORWARD, PIN_LEFT_MOTOR_BACKWARD, PIN_LEFT_MOTOR_PWM, LEFT_MOTOR_INTERRUPT);
#endif

}

void loop() {

    static uint8_t sMotorDirection = DIRECTION_FORWARD;

    MotorUnderTest.resetEncoderControlValues();
    uint8_t tStartPWM;
    int tLastSpeed;
    int tStartSpeed = 0;
    int tMaxSpeed = 0;
    uint8_t tStopPWM;
    int tStopSpeed = 0;
    /*
     * Print value after each encoder count change and increase PWM every 40 ms
     */
//    for (uint8_t tSpeed = 0; tSpeed < 250; ++tSpeed) {
//        MotorUnderTest.setSpeed(tSpeed, sMotorDirection);
//        uint32_t tStartMillis = millis();
//        do {
//            if (MotorUnderTest.EncoderCountHasChanged) {
//                MotorUnderTest.EncoderCountHasChanged = false;
//                Serial.print(tSpeed * PWM_PRINT_FACTOR);
//                Serial.print(' ');
//                Serial.print(MotorUnderTest.EncoderCount);
//                Serial.print(' ');
//                Serial.print(MotorUnderTest.getSpeed());
//                Serial.print(' ');
//                Serial.println(MotorUnderTest.getAverageSpeed());
//            }
//        } while (millis() - tStartMillis < 50);
//    }
    /*
     * Increase PWM and print values every 40 ms
     */
    for (uint8_t tPWM = 0; tPWM < 249; ++tPWM) {
        MotorUnderTest.setSpeed(tPWM, sMotorDirection);
        delay(50);
        Serial.print(tPWM);

        Serial.print(' ');
        int tSpeed = MotorUnderTest.getSpeed();
        if (tStartSpeed == 0 && tSpeed != 0) {
            tStartSpeed = tSpeed;
            tStartPWM = tPWM;
        }
        Serial.print(tSpeed);

        Serial.print(' ');
        Serial.print(MotorUnderTest.getAverageSpeed());

        Serial.print(' ');
        Serial.print(MotorUnderTest.EncoderCount);
        Serial.println();
    }

    // wait 1 second at max speed and print nothing
    int16_t tMaxEncoderCount = MotorUnderTest.EncoderCount;
    delay(1000);
    tMaxSpeed = MotorUnderTest.getAverageSpeed();
    MotorUnderTest.LastRideEncoderCount = 0;
    // and decrease
    for (uint8_t tPWM = 248; tPWM != 0; tPWM--) {
        MotorUnderTest.setSpeed(tPWM, sMotorDirection);
        delay(50);
        Serial.print(tPWM);

        Serial.print(' ');
        int tSpeed = MotorUnderTest.getSpeed();
        if (tStopSpeed == 0 && tSpeed == 0) {
            tStopSpeed = tLastSpeed;
            tStopPWM = tPWM -1;
        }
        Serial.print(tSpeed);
        tLastSpeed = tSpeed;

        Serial.print(' ');
        Serial.print(MotorUnderTest.getAverageSpeed());

        Serial.print(' ');
        Serial.print(tMaxEncoderCount - (int16_t) MotorUnderTest.LastRideEncoderCount);
        Serial.println();
    }

    /*
     * Print extended caption
     */
    Serial.print(F("PWM Speed Average Encoder "));
    Serial.print(F(" Start_PWM_Speed="));
    Serial.print(tStartPWM);
    Serial.print('_');
    Serial.print(tStartSpeed);
    Serial.print(F("|Max_Speed="));
    Serial.print(tMaxSpeed);
    Serial.print(F("|Stop="));
    Serial.print(tStopPWM);
    Serial.print('_');
    Serial.print(tStopSpeed);
    Serial.println();

    /*
     * switch direction
     */
    delay(10000);
    sMotorDirection = oppositeDIRECTION(sMotorDirection);
}
