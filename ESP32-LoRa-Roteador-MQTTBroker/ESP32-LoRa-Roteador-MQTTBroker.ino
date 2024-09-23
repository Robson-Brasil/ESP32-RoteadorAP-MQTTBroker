/******************************************************************************************************************************************
  IoT - Automação Residencial
  Autor : Robson Brasil

  Dispositivos : ESP32 LoRa Heltec V.2
  Preferences--> URLs adicionais do Gerenciador de placas:
                                    ESP32  : https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  Download Board ESP32 (x.x.x):
  Roteador AP
  Broker MQTT
  Versão : 1.0 - Release Candidate
  Última Modificação : 12/09/2024
******************************************************************************************************************************************/

#include <sMQTTBroker.h> // Biblioteca para criar um broker MQTT no ESP32
#include <WiFi.h>        // Biblioteca para funções de Wi-Fi no ESP32

// Credenciais de acesso para o MQTT Broker
const char* LoginMQTT = "Colocar Aqui o Seu Login"; // Altere aqui para o seu Login do MQTT Broker.
const char* SenhaMQTT = "Colocar Aqui a Sua Senha";  // Altere aqui para a sua senha do MQTT Broker.

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
            if ((e->Login() != LoginMQTT) || (e->Password() != SenhaMQTT))
            {
                Serial.println("Login ou Senha inválido(s)");
                return false;  // Rejeitar a conexão se as credenciais forem inválidas
            }
        };
        break;

        case LostConnect_sMQTTEventType:
            // Se a conexão for perdida, tentar reconectar o Wi-Fi
            WiFi.reconnect();
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

void setup()
{
    Serial.begin(115200); // Inicializa a comunicação serial para depuração

    // Configurações de IP estático no modo Access Point (AP)
    IPAddress local_IP(192,168,10,1);    // Define o IP desejado para o AP
    IPAddress gateway(192,168,10,1);     // O Gateway, normalmente o mesmo do IP do AP
    IPAddress subnet(255,255,255,0);     // Máscara de sub-rede

    // Tenta configurar o IP do AP
    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        Serial.println("Falha na configuração do IP do AP!"); // Mensagem de erro se falhar
    }

    // Configura o ESP32 como Access Point com nome de rede (SSID) e senha
    WiFi.softAP("Trocar aqui para o nome da rede WiFi", "Colocar Aqui a Senha do WiFi");  // Substitua pelo seu SSID e senha

    // Exibe o endereço IP do Access Point na serial
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Endereço IP do AP: ");
    Serial.println(IP); // Exibe o IP atribuído ao AP

    // Inicializa o Broker MQTT na porta 1883 (padrão do MQTT)
    const unsigned short mqttPort = 1883;
    broker.init(mqttPort); // Inicializa o Broker MQTT
}

void loop()
{
    broker.update(); // Mantém o broker MQTT funcionando e atualizado
}