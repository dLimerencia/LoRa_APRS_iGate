#include <WiFi.h>
#include "configuration.h"
#include "station_utils.h"
#include "battery_utils.h"
#include "aprs_is_utils.h"
#include "syslog_utils.h"
#include "pins_config.h"
#include "A7670_utils.h"
#include "wifi_utils.h"
#include "gps_utils.h"
#include "bme_utils.h"
#include "display.h"
#include "utils.h"

extern WiFiClient           espClient;
extern Configuration        Config;
extern String               versionDate;
extern bool                 statusAfterBoot;
extern String               firstLine;
extern String               secondLine;
extern String               thirdLine;
extern String               fourthLine;
extern String               fifthLine;
extern String               sixthLine;
extern String               seventhLine;
extern uint32_t             lastBeaconTx;
extern uint32_t             lastScreenOn;
extern bool                 displayState;
extern bool                 beaconUpdate;
extern String               iGateBeaconPacket;
extern String               iGateLoRaBeaconPacket;
extern std::vector<String>  lastHeardStation;
extern int                  rssi;
extern float                snr;
extern int                  freqError;
extern String               distance;
extern uint32_t             lastWiFiCheck;
extern bool                 WiFiConnect;
extern bool                 WiFiConnected;


namespace Utils {

    void processStatus() {
        String status = Config.callsign + ">APLRG1," + Config.beacon.path;
        
        if (WiFi.status() == WL_CONNECTED && Config.aprs_is.active && Config.beacon.sendViaAPRSIS) {
            delay(1000);
            status += ",qAC:>https://github.com/richonguzman/LoRa_APRS_iGate " + versionDate;
            APRS_IS_Utils::upload(status);
            SYSLOG_Utils::log("APRSIS Tx", status,0,0,0);
            statusAfterBoot = false;
        }
        if (statusAfterBoot && !Config.beacon.sendViaAPRSIS && Config.beacon.sendViaRF) {
            delay(2000);
            status += ":>https://github.com/richonguzman/LoRa_APRS_iGate " + versionDate;
            STATION_Utils::addToOutputPacketBuffer(status);
            statusAfterBoot = false;
        }
    }

    String getLocalIP() {
        if (!WiFiConnected) {
            return "IP :  192.168.4.1";
        } else {
            return "IP :  " + String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
        }        
    }

    void setupDisplay() {
        setup_display();
        #ifdef INTERNAL_LED_PIN
        digitalWrite(INTERNAL_LED_PIN,HIGH);
        #endif
        Serial.println("\nStarting Station: " + Config.callsign + "   Version: " + versionDate);
        show_display(" LoRa APRS", "", "   ( iGATE & DIGI )", "", "", "Richonguzman / CA2RXU", "      " + versionDate, 4000);
        #ifdef INTERNAL_LED_PIN
        digitalWrite(INTERNAL_LED_PIN,LOW);
        #endif
        firstLine   = Config.callsign;
        seventhLine = "     listening...";
    }

    void activeStations() {
        fourthLine = "Stations (" + String(Config.rememberStationTime) + "min) = ";
        if (lastHeardStation.size() < 10) {
            fourthLine += " ";
        }
        fourthLine += String(lastHeardStation.size());            
    }

    void checkBeaconInterval() {
        uint32_t lastTx = millis() - lastBeaconTx;
        String beaconPacket             = iGateBeaconPacket;
        String secondaryBeaconPacket    = iGateLoRaBeaconPacket;

        if (lastBeaconTx == 0 || lastTx >= Config.beacon.interval * 60 * 1000) {
            beaconUpdate = true;    
        }

        if (beaconUpdate) {
            display_toggle(true);

            Utils::println("-- Sending Beacon to APRSIS --");

            STATION_Utils::deleteNotHeard();

            activeStations();

            if (Config.bme.active) {
                String sensorData = BME_Utils::readDataSensor();
                beaconPacket += sensorData;
                secondaryBeaconPacket += sensorData;
            }
            beaconPacket += Config.beacon.comment;
            secondaryBeaconPacket += Config.beacon.comment;

            #if defined(TTGO_T_LORA32_V2_1) || defined(HELTEC_V2) || defined(HELTEC_HTCT62) || defined(HELTEC_V3) || defined(ESP32_DIY_LoRa_A7670) || defined(TTGO_T_LORA32_V2_1_915)
            if (Config.sendBatteryVoltage) {
                beaconPacket += " Batt=" + String(BATTERY_Utils::checkBattery(),2) + "V";
                secondaryBeaconPacket += " Batt=" + String(BATTERY_Utils::checkBattery(),2) + "V";
                sixthLine = "     (Batt=" + String(BATTERY_Utils::checkBattery(),2) + "V)";
            }
            #endif

            if (Config.externalVoltageMeasurement) { 
                beaconPacket += " Ext=" + String(BATTERY_Utils::checkExternalVoltage(),2) + "V";
                secondaryBeaconPacket += " Ext=" + String(BATTERY_Utils::checkExternalVoltage(),2) + "V";
                sixthLine = "    (Ext V=" + String(BATTERY_Utils::checkExternalVoltage(),2) + "V)";
            }

            if (Config.aprs_is.active && Config.beacon.sendViaAPRSIS) {
                show_display(firstLine, secondLine, thirdLine, fourthLine, fifthLine, sixthLine, "SENDING IGATE BEACON", 0); 
                seventhLine = "     listening...";
                #ifdef ESP32_DIY_LoRa_A7670
                A7670_Utils::uploadToAPRSIS(beaconPacket);
                #else
                APRS_IS_Utils::upload(beaconPacket);
                #endif
            }

            if (Config.beacon.sendViaRF) {
                show_display(firstLine, secondLine, thirdLine, fourthLine, fifthLine, sixthLine, "SENDING DIGI BEACON", 0);
                seventhLine = "     listening...";
                STATION_Utils::addToOutputPacketBuffer(secondaryBeaconPacket);
            }

            lastBeaconTx = millis();
            if (Config.display.alwaysOn) {
                lastScreenOn = millis();
            }
            beaconUpdate = false;
        }

        if (statusAfterBoot) {
            processStatus();
        }
    }

    void checkDisplayInterval() {
        uint32_t lastDisplayTime = millis() - (lastScreenOn + (Config.display.timeout * 1000));
    
        if (!Config.display.alwaysOn && millis() > (Config.display.timeout * 1000)) {
            if (lastDisplayTime <= millis() && displayState) {
                displayState = false;
                display_toggle(false);
            }
        }
    }

    void checkWiFiInterval() {
        uint32_t WiFiCheck = millis() - lastWiFiCheck;
        if (WiFi.status() != WL_CONNECTED && WiFiCheck >= 15*60*1000) {
        WiFiConnect = true;
        }
        if (WiFiConnect) {
        Serial.println("\nConnecting to WiFi ...");
        WIFI_Utils::startWiFi();
        lastWiFiCheck = millis();
        WiFiConnect = false;
        }
    }

    void validateFreqs() {
        if (Config.loramodule.txFreq != Config.loramodule.rxFreq && abs(Config.loramodule.txFreq - Config.loramodule.rxFreq) < 125000) {
            Serial.println("Tx Freq less than 125kHz from Rx Freq ---> NOT VALID");
            show_display("Tx Freq is less than ", "125kHz from Rx Freq", "device will autofix", "and then reboot", 1000);
            Config.loramodule.txFreq = Config.loramodule.rxFreq; // Inform about that but then change the TX QRG to RX QRG and reset the device
            Config.writeFile();
            ESP.restart();
        }
    }

    void typeOfPacket(String packet, String packetType) {
        String sender;
        if (packetType == "LoRa-APRS") {
            fifthLine = "LoRa Rx ----> APRS-IS";
            sender = packet.substring(0,packet.indexOf(">"));
        } else if (packetType == "APRS-LoRa") {
            fifthLine = "APRS-IS ----> LoRa Tx";
            sender = packet.substring(0,packet.indexOf(">"));
        } else if (packetType == "Digi") {
            fifthLine = "LoRa Rx ----> LoRa Tx";
            sender = packet.substring(0,packet.indexOf(">"));
        }
        for (int i = sender.length(); i < 9; i++) {
            sender += " ";
        }
        if (packet.indexOf("::") >= 10) {
            sixthLine = sender + "> MESSAGE";
            seventhLine = "RSSI:" + String(rssi) + "dBm SNR: " + String(snr) + "dBm";
        } else if (packet.indexOf(":>") >= 10) {
            sixthLine = sender + "> NEW STATUS";
            seventhLine = "RSSI:" + String(rssi) + "dBm SNR: " + String(snr) + "dBm";
        } else if (packet.indexOf(":!") >= 10 || packet.indexOf(":=") >= 10) {
            sixthLine = sender + "> GPS BEACON";
            GPS_Utils::getDistance(packet);
            seventhLine = "RSSI:" + String(rssi) + "dBm";
            if (rssi <= -100) {
                seventhLine += " ";
            } else {
                seventhLine += "  ";
            }
            if (distance.indexOf(".") == 1) {
                seventhLine += " ";
            }
            seventhLine += "D:" + distance + "km";
        } else if (packet.indexOf(":T#") >= 10 && packet.indexOf(":=/") == -1) {
            sixthLine = sender + "> TELEMETRY";
            seventhLine = "RSSI:" + String(rssi) + "dBm SNR: " + String(snr) + "dBm";
        } else if (packet.indexOf(":`") >= 10) {
            sixthLine = sender + ">  MIC-E";
            seventhLine = "RSSI:" + String(rssi) + "dBm SNR: " + String(snr) + "dBm";
        } else if (packet.indexOf(":;") >= 10) {
            sixthLine = sender + ">  OBJECT";
            seventhLine = "RSSI:" + String(rssi) + "dBm SNR: " + String(snr) + "dBm";
        } else {
            sixthLine = sender + "> ??????????";
            seventhLine = "RSSI:" + String(rssi) + "dBm SNR: " + String(snr) + "dBm";
        }
    }

    void print(String text) {
        if (!Config.tnc.enableSerial) {
            Serial.print(text);
        }
    }

    void println(String text) {
        if (!Config.tnc.enableSerial) {
            Serial.println(text);
        }
    }

}
