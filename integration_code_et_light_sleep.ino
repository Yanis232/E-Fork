#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>
#include "esp_pm.h"

// ======================
// Configuration BLE
// ======================
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *pCharacteristic;
BLEServer         *pServer;
bool deviceConnected = false;

// ======================
// Configuration MPU6050
// ======================
Adafruit_MPU6050 mpu;

// ======================
// Configuration Analog
// ======================
#define ANALOG_PIN 34 // Broche d'entrée analogique

// ======================
// Callbacks pour gérer la connexion/déconnexion
// ======================
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("Appareil BLE connecté !");
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("Appareil BLE déconnecté !");
    // Relancer la publicité BLE pour autoriser une nouvelle connexion
    pServer->getAdvertising()->start();
  }
};

// ======================
// Configuration de la gestion d'énergie
// ======================
void configurePowerManagement() {
  Serial.println("Configuration Power Management (Light Sleep auto)...");

  // Paramètres pour autoriser Light Sleep automatique
  esp_pm_config_esp32_t pm_config;
  pm_config.max_freq_mhz = 80;        // Limite la fréquence CPU à 80 MHz
  pm_config.min_freq_mhz = 10;        // Peut descendre à 10 MHz
  pm_config.light_sleep_enable = true; // Active Light Sleep automatique

  esp_pm_configure(&pm_config);

  Serial.println("-> CPU réglé à max 80 MHz (Light Sleep auto activé)");
}

// ======================
// Initialisation du BLE
// ======================
void initializeBLE() {
  BLEDevice::init("ESP32_Sensor");  // Nom du périphérique BLE

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Création d'un service BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Caractéristique en mode NOTIFY pour envoyer des données vers le smartphone
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());

  // Démarre le service
  pService->start();

  // Lance la publicité BLE
  pServer->getAdvertising()->start();

  Serial.println("Publicité BLE activée !");
}

// ======================
// Initialisation du MPU6050
// ======================
void initializeMPU6050() {
  if (!mpu.begin()) {
    Serial.println("MPU6050 introuvable, vérifiez le câblage !");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 initialisé avec succès.");

  // Configuration des plages de mesure
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

// ======================
// Lecture des capteurs + Envoi BLE
// ======================
void sendBLEData() {
  // Buffers de texte pour chaque mesure
  char accelBuffer[64], gyroBuffer[64], analogBuffer[32];

  // Lecture MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Accéléromètre
  snprintf(accelBuffer, sizeof(accelBuffer),
           "Accel;X:%.2f;Y:%.2f;Z:%.2f",
           a.acceleration.x, a.acceleration.y, a.acceleration.z);

  // Gyroscope
  snprintf(gyroBuffer, sizeof(gyroBuffer),
           "Gyro;X:%.2f;Y:%.2f;Z:%.2f",
           g.gyro.x, g.gyro.y, g.gyro.z);

  // Capteur analogique (ex: Force sensor)
  int sensorValue = analogRead(ANALOG_PIN);
  float voltage = (sensorValue / 4095.0) * 3.3; // conversion en Volts
  snprintf(analogBuffer, sizeof(analogBuffer),
           "ForceSensor;%.2fV", voltage);

  // Fusion dans un seul message
  char notificationBuffer[160];
  snprintf(notificationBuffer, sizeof(notificationBuffer),
           "%s;%s;%s",
           accelBuffer, gyroBuffer, analogBuffer);

  // Envoi via BLE
  pCharacteristic->setValue(notificationBuffer);
  pCharacteristic->notify();

  // Affichage dans le moniteur série
  Serial.println(notificationBuffer);
}

// ======================
// Setup
// ======================
void setup() {
  Serial.begin(115200);
  Serial.println("Démarrage...");

  configurePowerManagement(); // Active la gestion d’énergie (Light Sleep auto)
  initializeMPU6050();        // Initialise le MPU6050
  initializeBLE();            // Initialise le Bluetooth Low Energy

  Serial.println("Système prêt !");
}

// ======================
// Boucle principale
// ======================
void loop() {
  if (!deviceConnected) {
    // Personne n'est connecté : on attend
    Serial.println("En attente d'une connexion BLE...");
    delay(100);
  } else {
    // Quelqu'un est connecté : on envoie nos mesures
    sendBLEData();
    delay(100);  // on envoie toutes les 1 seconde
  }
}
