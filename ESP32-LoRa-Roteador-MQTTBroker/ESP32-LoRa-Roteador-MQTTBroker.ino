/******************************************************************************************************************************************
  IoT - Automação Residencial
  Autor : Robson Brasil

  Dispositivos : ESP32 WROOM32
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  Roteador AP
  Broker MQTT
  Versão : 1.1
  Última Modificação : 19/07/2025
******************************************************************************************************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include <sMQTTBroker.h>
#include <Preferences.h>
#include "config_login.h"
#include "config_ip.h"
#include "config_wifi.h"
#include "config_mqtt.h"

// Variáveis globais para monitoramento
uint8_t numClientesWiFi = 0;
uint8_t numClientesMQTT = 0;

Preferences preferences;

// Classe que herda de sMQTTBroker e lida com os eventos de clientes MQTT
class MyBroker : public sMQTTBroker
{
public:
    // Função chamada quando um evento MQTT ocorre
    bool onEvent(sMQTTEvent *event) override
    {
        // Tratamento de tipos diferentes de eventos MQTT
        switch (event->Type())
        {
        case NewClient_sMQTTEventType:
        {
            sMQTTNewClientEvent *e = (sMQTTNewClientEvent *)event;
            // Verificar se o nome de usuário e a senha estão corretos para a nova conexão
            if ((e->Login() != LOGIN_MQTT) || (e->Password() != SENHA_MQTT))
            {
                Serial.println("Login ou Senha inválido(s)");
                return false;  // Rejeitar a conexão se as credenciais forem inválidas
            }
            numClientesMQTT++;
            Serial.printf("Novo cliente MQTT conectado. Total: %d\n", numClientesMQTT);
        };
        break;

        case LostConnect_sMQTTEventType:
            if (numClientesMQTT > 0) {
                numClientesMQTT--;
                Serial.printf("Cliente MQTT desconectado. Total: %d\n", numClientesMQTT);
            }
            break;

        // Eventos de subscribe e unsubscribe
        case UnSubscribe_sMQTTEventType:
        case Subscribe_sMQTTEventType:
        {
            sMQTTSubUnSubClientEvent *e = (sMQTTSubUnSubClientEvent *)event;
            // Aqui você pode adicionar código para gerenciar assinaturas de tópicos MQTT
        }
        break;
        }
        return true; // Continuar normalmente após tratar o evento
    }
};

// Instância da classe MyBroker
MyBroker broker;

// Callback para quando clientes se conectam/desconectam do AP
void onWiFiEvent(WiFiEvent_t event) {
    if (event == WIFI_EVENT_AP_STACONNECTED) {
        numClientesWiFi++;
        Serial.print("Cliente Wi-Fi conectado. Total: ");
        Serial.println(numClientesWiFi);
    } else if (event == WIFI_EVENT_AP_STADISCONNECTED) {
        if (numClientesWiFi > 0) numClientesWiFi--;
        Serial.print("Cliente Wi-Fi desconectado. Total: ");
        Serial.println(numClientesWiFi);
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\nIniciando ESP32 Router + MQTT Broker...");

    preferences.begin("esp32-ap", false);

    WiFi.onEvent(onWiFiEvent);
    setupWiFiAP();
    Serial.println("Access Point iniciado com sucesso!");
    Serial.print("SSID: ");
    Serial.println(SSID_AP);
    Serial.print("Endereço IP do AP: ");
    Serial.println(WiFi.softAPIP());

    if (!broker.init(MQTT_PORT)) {
        Serial.println("Falha ao iniciar o Broker MQTT!");
        return;
    }
    Serial.println("Broker MQTT iniciado com sucesso!");
}

void loop()
{
    broker.update();
    static unsigned long ultimoStatus = 0;
    if (millis() - ultimoStatus >= 30000) {
        Serial.println("\n=== Status do Sistema ===");
        Serial.print("Clientes Wi-Fi conectados: ");
        Serial.println(numClientesWiFi);
        Serial.print("Clientes MQTT conectados: ");
        Serial.println(numClientesMQTT);
        Serial.print("Memória livre: ");
        Serial.println(ESP.getFreeHeap());
        Serial.println("=======================");
        ultimoStatus = millis();
    }
}