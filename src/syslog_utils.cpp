#include <WiFiUdp.h>
#include <WiFi.h>
#include "configuration.h"
#include "syslog_utils.h"
#include "gps_utils.h"

extern Configuration    Config;
extern int              stationMode;

WiFiUDP udpClient;

namespace SYSLOG_Utils {

    void log(String type, String packet, int rssi, float snr, int freqError) {
        String syslogPacket = "<165>1 - " + Config.callsign + " LoRa" + " - - - "; //RFC5424 The Syslog Protocol
        if (Config.syslog.active && (stationMode==1 || stationMode==2 || (stationMode==5 && WiFi.status()==WL_CONNECTED))) {
            if (type == "APRSIS Tx") {
                if (packet.indexOf(":>") > 10) {
                    syslogPacket += type + " / StartUp STATUS / " + packet.substring(packet.indexOf(":>")+2);
                }
            } else if (type == "LoRa Rx") {
                if (packet.indexOf("::") > 10) {
                    syslogPacket += type + " / MESSAGE / " + packet.substring(3,packet.indexOf(">")) + " ---> " + packet.substring(packet.indexOf("::")+2);
                    syslogPacket += " / " + String(rssi) + "dBm / " + String(snr) + "dB / " + String(freqError) + "Hz";
                } else if (packet.indexOf(":!") > 10 || packet.indexOf(":=") > 10) {
                    syslogPacket += type + " / GPS / " + packet.substring(3,packet.indexOf(">")) + " / ";
                    if (packet.indexOf("WIDE1-1") > 10) {
                        syslogPacket += packet.substring(packet.indexOf(">")+1,packet.indexOf(",")) + " / WIDE1-1 / ";
                    } else {
                        syslogPacket += packet.substring(packet.indexOf(">")+1,packet.indexOf(":")) + " / _ / ";
                    }
                    syslogPacket += String(rssi) + "dBm / " + String(snr) + "dB / " + String(freqError) + "Hz / " +  GPS_Utils::getDistance(packet);
                } else if (packet.indexOf(":>") > 10) {
                    syslogPacket += type + " / STATUS / " + packet.substring(3,packet.indexOf(">")) + " ---> " + packet.substring(packet.indexOf(":>")+2);
                    syslogPacket += " / " + String(rssi) + "dBm / " + String(snr) + "dB / " + String(freqError) + "Hz";
                } else if (packet.indexOf(":`") > 10) {
                    syslogPacket += type + " / MIC-E / " + packet.substring(3,packet.indexOf(">")) + " ---> " + packet.substring(packet.indexOf(":`")+2);
                    syslogPacket += " / " + String(rssi) + "dBm / " + String(snr) + "dB / " + String(freqError) + "Hz";
                } else if (packet.indexOf(":T#") >= 10 && packet.indexOf(":=/") == -1) {
                    syslogPacket += type + " / TELEMETRY / " + packet.substring(3,packet.indexOf(">")) + " ---> " + packet.substring(packet.indexOf(":T#")+3);
                    syslogPacket += " / " + String(rssi) + "dBm / " + String(snr) + "dB / " + String(freqError) + "Hz";
                } else if (packet.indexOf(":;") > 10) {
                    syslogPacket += type + " / OBJECT / " + packet.substring(3,packet.indexOf(">")) + " ---> " + packet.substring(packet.indexOf(":;")+2);
                    syslogPacket += " / " + String(rssi) + "dBm / " + String(snr) + "dB / " + String(freqError) + "Hz";
                } else {
                    syslogPacket += type + " / " + packet;
                    syslogPacket += " / " + String(rssi) + "dBm / " + String(snr) + "dB / " + String(freqError) + "Hz";
                }
            } else if (type == "LoRa Tx") {
                if (packet.indexOf("RFONLY") > 10) {
                    syslogPacket += type + " / RFONLY / " + packet.substring(packet.indexOf("::")+2);
                } else if (packet.indexOf("::") > 10) {
                    syslogPacket += type + " / MESSAGE / " + packet.substring(0,packet.indexOf(">")) + " ---> " + packet.substring(packet.indexOf("::")+2);
                } else {
                    syslogPacket += type + " / " + packet;
                }
            } else {
                syslogPacket = "<165>1 - ERROR LoRa - - - ERROR / Error in Syslog Packet"; //RFC5424 The Syslog Protocol
            }
            udpClient.beginPacket(Config.syslog.server.c_str(), Config.syslog.port);
            udpClient.write((const uint8_t*)syslogPacket.c_str(), syslogPacket.length());
            udpClient.endPacket();
        }
    }

    void setup() {
        if (Config.syslog.active && (stationMode==1 || stationMode==2 || (stationMode==5 && WiFi.status()==WL_CONNECTED))) {
            udpClient.begin(WiFi.localIP(), 0);
            Serial.println("init : Syslog Server  ...     done!    (at " + Config.syslog.server + ")");
        }
    }

}