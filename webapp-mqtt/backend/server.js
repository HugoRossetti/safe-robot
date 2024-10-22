const mqtt = require('mqtt');
const express = require('express');
const http = require('http');
const path = require('path');
const WebSocket = require('ws');

// Conectando ao broker MQTT
const mqttClient = mqtt.connect('mqtt://192.168.151.58:1883'); // Substitua pelo IP correto do broker

mqttClient.on('connect', () => {
    console.log('Conectado ao broker MQTT');
    mqttClient.subscribe('robo/bateria', (err) => {
        if (err) {
            console.error('Erro ao se inscrever no tópico robo/bateria:', err);
        } else {
            console.log('Inscrito no tópico robo/bateria');
        }
    });

    // Subscribing to MQTT topics to receive feedback from the robot
    mqttClient.subscribe('robo/bateria'); // Tópico para receber informações de status do robô
    mqttClient.subscribe('robo/status'); // Tópico para receber status geral do robô
    mqttClient.subscribe("robo/velocidade");
});

// Configurando o servidor express
const app = express();
const server = http.createServer(app);

// Servindo arquivos estáticos da pasta frontend
const frontendPath = path.join(__dirname, '../frontend');
app.use(express.static(frontendPath));

// Rotas para diferentes páginas
app.get('/', (req, res) => {
    res.sendFile(path.join(frontendPath, 'index.html'));  // Página de login
});

app.get('/menu', (req, res) => {
    res.sendFile(path.join(frontendPath, 'menu.html'));  // Menu de controle
});

app.get('/controle', (req, res) => {
    res.sendFile(path.join(frontendPath, 'manual.html'));  // Página de controle do robô
});

// WebSocket para comunicação em tempo real com o frontend
const wss = new WebSocket.Server({ server });

wss.on('connection', (ws) => {
    console.log('Novo cliente conectado via WebSocket');

    // Quando o cliente envia uma mensagem (comandos do joystick)
    ws.on('message', (message) => {
        console.log('Mensagem recebida do frontend:', message);

        // Parse da mensagem recebida do WebSocket (velocidade e ângulo)
        const data = JSON.parse(message);
        const velocidade = data.velocidade;
        const angulo = data.angulo;

        // Publicar comando no broker MQTT
        const comando = JSON.stringify({
            velocidade: velocidade,
            angulo: angulo
        });
        mqttClient.publish('robo/comandos', comando);

        console.log('Comando enviado ao MQTT:', comando);
    });

    mqttClient.on('message', (topic, message) => {
        console.log(`Tópico recebido: ${topic}`);
        console.log('Mensagem recebida:', message.toString());
    
        if (topic === 'robo/status') {
            console.log('Mensagem de status recebida no tópico robo/status:', message.toString());
    
            const status = JSON.parse(message.toString());
            wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify(status));
                    console.log('Dados enviados via WebSocket:', status);
                }
            });
        }
    
        // Verifique o tópico de bateria
        if (topic === 'robo/bateria') {
            console.log('Mensagem de bateria recebida no tópico robo/bateria:', message.toString());
            // Enviar dados de bateria para os clientes via WebSocket
            const bateriaData = JSON.parse(message.toString()); // Supondo que a mensagem seja JSON
            wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify(bateriaData));
                    console.log('Dados de bateria enviados via WebSocket:', bateriaData);
                }
            });
        }
        if (topic === 'robo/velocidade') {
            console.log('Mensagem de velocidade recebida no tópico robo/velocidade:', message.toString());
            const velocidadeData = JSON.parse(message.toString());
            wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify({ velocidade: velocidadeData.velocidade }));
                    console.log('Dados de velocidade enviados via WebSocket:', velocidadeData.velocidade);
                }
            });
        }
    });
});

// Inicia o servidor na porta 3000
server.listen(3000, () => {
    console.log('Servidor rodando em http://localhost:3000');
});