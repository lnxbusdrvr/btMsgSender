/*
 * 20240209 nopeusnaytto.cc lbd
 *
 * @author : lnxbusdrvr
 * @created : 2024-01-29
 *
 * No Copyright
 *
 * The person who associated a work with this deed
 * has dedicated the work to the public domain b
 * ywaiving all of his or her rights to the work
 * worldwide under copyright law,
 * including all related and neighboring rights,
 * to the extent allowed by law.
 * You can copy, modify, distribute and perform the work,
 * even for commercial purposes,
 * all without asking permission.
 * See Other Information below.
 *
 * Other Information
 *
 * In no way are the patent or trademark rights
 * of any person affected by CC0, nor are the right
 * sthat other persons may have in the work or in
 * how the work is used, such as publicity or privacy rights.
 * Unless expressly stated otherwise,
 * the person who associated a work with this deed makes
 * no warranties about the work, and disclaims liability
 * for all uses of the work, to the fullest extent
 * permitted by applicable law.
 * When using or citing the work,
 * you should not imply endorsement
 * by the author or the affirmer.
 *
 *
 * Original ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram),
 * visit https://esp32io.com/tutorials/esp32-gps
 *
 *
 *
 * Original Blootooth Code:
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 * By Evandro Copercini - 2018
 *
 * This example creates a bridge between Serial and Classical Bluetooth (SPP)
 * and also demonstrate that SerialBT have the same functionalities of a normal Serial
 *
 */

#include <TinyGPS++.h>
#include "BluetoothSerial.h"

/*
 * Check, if Bluetooth is enabled
 * on ESP32-micro-controller.
 */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth-toiminto ei ole käytössä ESP32-mikro-ohjaimessa.
#endif

TinyGPSPlus gps;
BluetoothSerial SerialBT;


void setup() {
  static const int BAUDRATE = 9600;  // The baudrate of ESP32 and NEO-6M is 9600
  Serial.begin(BAUDRATE);
  Serial2.begin(BAUDRATE);  // Serial from NEO-6M
  SerialBT.begin("Nopeuden kertoja");  // BT device name
}

void loop() {
  receiveProcessesedGPSData();
  checkGPSSignalReception();
}

void receiveProcessesedGPSData() {
  if (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      sendChangedSpeed();
    } else {
      Serial.println("GPS-tietojen koodaus epäonnistui.");
    }
  } else {
    Serial.println("GPS-lähetys ei toimi oikein.");
  }
}

void sendChangedSpeed() {
  static int oldSpeed = 0;
  static int currentSpeed = 0;

  if (gps.speed.isValid()) {
    currentSpeed = gps.speed.kmph();
    if (oldSpeed != currentSpeed) {
      SerialBT.println(currentSpeed);
      oldSpeed = currentSpeed;
    }
  } else {
    Serial.println("GPS-nopeustiedot eivät ole kelvollisia.");
  }
}

void checkGPSSignalReception() {
  static const int DELAY = 5000;
  static const byte RECIEVED_CHARACTERS = 10;

  /*
   * If no signal had recieved for awhile from GPS.
   * GPS should frequently send signal while running normally.
   */
  if (millis() > DELAY && gps.charsProcessed() < RECIEVED_CHARACTERS)
    Serial.println("GPS ei vastaanota signaalia: Tarkista kaapelit.");
}

