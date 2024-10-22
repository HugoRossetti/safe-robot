#include <WiFi.h>
#include <PubSubClient.h>
#include <RoboCore_Vespa.h>
#include <ArduinoJson.h>

// LED e sensores
const uint8_t PINO_LED = 15;
const uint8_t PINO_HCSR04_ECHO = 26;
const uint8_t PINO_HCSR04_TRIGGER = 25;

// Aliases JSON
const char* ALIAS_ANGULO = "angulo";
const char* ALIAS_DISTANCIA = "distancia";
const char* ALIAS_VELOCIDADE = "velocidade";
const char* ALIAS_VBAT = "vbat";

// Configurações da rede Wi-Fi
const char* ssid = "teste";
const char* password = "12345678";

// Configurações do broker MQTT
const char* mqtt_server = "192.168.151.58"; // IP ou URL do servidor MQTT
const char* mqtt_topic_comandos = "robo/comandos";
const char* mqtt_topic_bateria = "robo/bateria";
const char* mqtt_topic_velocidade = "robo/velocidade"; // Novo tópico para a velocidade

// Cliente Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis do motor e bateria
VespaMotors motores;
VespaBattery vbat;
const uint32_t TEMPO_ATUALIZACAO_VBAT = 5000; // [ms]
uint32_t timeout_vbat;
const uint32_t TEMPO_ATUALIZACAO_VELOCIDADE = 1000; // Atualizar velocidade a cada 1 segundo
uint32_t timeout_velocidade;

// Controle do robô
int16_t velocidade = 0;
int16_t angulo = 0;

// Variáveis de controle de conexão
unsigned long ultimaTentativaConexao = 0; // Guarda o tempo da última tentativa de reconexão
const unsigned long TEMPO_MAXIMO_RECONEXAO = 10000; // 10 segundos
bool modoAutonomo = false; // Controle de ativação do modo autônomo

// Função de controle de distância mínima
const int DISTANCIA_MINIMA = 20; // Distância mínima em cm para evitar colisão

// Funções e variáveis do modo autônomo (anticolisão)
const int VELOCIDADE_AUTONOMA = 80;         // Velocidade do robô em modo autônomo
const int DISTANCIA_OBSTACULO = 25;         // Distância mínima para evitar obstáculos
const int ESPERA_MOVIMENTO = 250;           // Tempo de movimento ao desviar de obstáculos
int distancia;                              // Variável para armazenar a distância medida pelo sensor

// Conectar ao Wi-Fi
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Conectando ao Wi-Fi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando...");
    }

    Serial.println("Wi-Fi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

// Função de callback para o MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    if (modoAutonomo) return; // Ignorar comandos MQTT se estiver no modo autônomo
    
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.print("Mensagem recebida no tópico: ");
    Serial.println(topic);
    Serial.print("Mensagem: ");
    Serial.println(message);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.println("Erro ao fazer parse do JSON");
        return;
    }

    // Processar comandos do joystick
    if (strcmp(topic, mqtt_topic_comandos) == 0) {
        velocidade = doc["velocidade"];
        angulo = doc["angulo"];

        // Ajustar ângulo negativo para o intervalo 0 a 360
        if (angulo < 0) {
            angulo = 360 + angulo;
        }

        // Verificar os valores recebidos
        Serial.print("Velocidade: ");
        Serial.println(velocidade);
        Serial.print("Ângulo ajustado: ");
        Serial.println(angulo);

        // Controle de movimento

        // Curva frente para a esquerda (ajuste para curva correta)
        if ((angulo >= 90) && (angulo <= 180)) {
            motores.turn(-velocidade, -velocidade * (135 - angulo) / 45);

        // Curva frente para a direita (ajuste para curva correta)
        } else if ((angulo >= 0) && (angulo < 90)) {
            motores.turn(-velocidade * (angulo - 45) / 45, -velocidade);

        // Curva ré para a esquerda (ajuste para curva correta)
        } else if ((angulo > 180) && (angulo <= 270)) {
            motores.turn(velocidade * (angulo - 225) / 45, velocidade);

        // Curva ré para a direita (ajuste para curva correta)
        } else if (angulo > 270) {
            motores.turn(velocidade, velocidade * (315 - angulo) / 45);

        } else {
            motores.stop();
        }

        // Publicar a velocidade atual calculada no tópico MQTT
        publicarVelocidade(velocidade);
    } else {
        // Se o ângulo estiver fora do esperado, parar o robô
        motores.stop();
    }
}

// Conectar ao broker MQTT
void reconnect() {
    if (!client.connected()) {
        unsigned long agora = millis();
        if (agora - ultimaTentativaConexao > 5000) { // Tentar a cada 5 segundos
            Serial.print("Tentando conectar ao broker MQTT...");
            if (client.connect("ESP32Client")) {
                Serial.println("Conectado");
                client.subscribe(mqtt_topic_comandos); // Inscrever no tópico de comandos
                
                // Se o modo autônomo estava ativo, desativá-lo e restaurar o controle remoto
                if (modoAutonomo) {  
                    Serial.println("Reconectado ao broker MQTT. Desativando modo autônomo...");
                    modoAutonomo = false;
                    motores.stop();  // Para garantir que os motores parem ao sair do modo autônomo
                }
                
                // Atualizar o tempo da última tentativa de conexão
                ultimaTentativaConexao = millis();
            } else {
                Serial.print("Falha, rc=");
                Serial.print(client.state());
                Serial.println(" Tentando novamente...");
            }
        }

        // Se o tempo de reconexão exceder 10 segundos, ativar modo autônomo
        if (millis() - ultimaTentativaConexao >= TEMPO_MAXIMO_RECONEXAO && !modoAutonomo) {
            Serial.println("Não foi possível reconectar ao broker. Entrando no modo autônomo...");
            modoAutonomo = true;
        }
    }
}

// Enviar tensão da bateria
void enviarTensaoBateria() {
    if (millis() > timeout_vbat) {
        uint32_t tensao = vbat.readVoltage();
        StaticJsonDocument<128> doc;
        doc["vbat"] = tensao;
        char mensagem[128];
        serializeJson(doc, mensagem);
        client.publish(mqtt_topic_bateria, mensagem);  // Publicar no tópico de bateria
        Serial.printf("Tensão da bateria enviada: %u mV\n", tensao);
        timeout_vbat = millis() + TEMPO_ATUALIZACAO_VBAT;
    }
}

// Publicar velocidade no MQTT
void publicarVelocidade(int16_t velocidadeAtual) {
    if (millis() > timeout_velocidade) {
        StaticJsonDocument<128> doc;
        doc["velocidade"] = velocidadeAtual;
        char mensagem[128];
        serializeJson(doc, mensagem);
        client.publish(mqtt_topic_velocidade, mensagem); // Publicar no tópico de velocidade
        Serial.printf("Velocidade enviada: %d\n", velocidadeAtual);
        timeout_velocidade = millis() + TEMPO_ATUALIZACAO_VELOCIDADE;
    }
}

// Função para leitura do sensor ultrassônico
int sensor_ultrassonico(int pinotrigger, int pinoecho) {
    // Realiza o pulso de 10 microsegundos no trigger do sensor
    digitalWrite(pinotrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinotrigger, LOW);

    // Mede o pulso em microsegundos retornado para o echo do sensor e converte o tempo para distância dividindo por 58
    return pulseIn(pinoecho, HIGH) / 58;
}

// Função para modo autônomo com anticolisão
void modoAutonomoAnticolisao() {
    Serial.println("Modo autônomo ativado. Evitando obstáculos...");

    distancia = sensor_ultrassonico(PINO_HCSR04_TRIGGER, PINO_HCSR04_ECHO);

    if (distancia <= DISTANCIA_OBSTACULO) {
        motores.stop();
        motores.backward(VELOCIDADE_AUTONOMA);
        delay(ESPERA_MOVIMENTO);
        motores.stop();

        // Fazer o robô girar para a esquerda ou direita
        if (millis() % 2 == 0) {
            motores.turn(-VELOCIDADE_AUTONOMA, VELOCIDADE_AUTONOMA);  // Girar para a esquerda
        } else {
            motores.turn(VELOCIDADE_AUTONOMA, -VELOCIDADE_AUTONOMA);  // Girar para a direita
        }

        delay(ESPERA_MOVIMENTO);
        motores.stop();
    } else {
        motores.forward(VELOCIDADE_AUTONOMA);
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(PINO_LED, OUTPUT);
    pinMode(PINO_HCSR04_TRIGGER, OUTPUT);
    pinMode(PINO_HCSR04_ECHO, INPUT);

    timeout_vbat = millis();
    timeout_velocidade = millis();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    if (modoAutonomo) {
        modoAutonomoAnticolisao();
    } else {
        enviarTensaoBateria();
    }
}












