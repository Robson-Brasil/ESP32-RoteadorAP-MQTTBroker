#ifndef CONFIG_WIFI_H
#define CONFIG_WIFI_H

#include <WiFi.h>
#include "config_login.h"
#include "config_ip.h"

inline void setupWiFiAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(LOCAL_IP, GATEWAY_IP, SUBNET_IP);
    WiFi.softAP(SSID_AP, SENHA_AP);
}

#endif
