# Sistema de Remediação de Cyberataques em Robôs Industriais com MQTT

## Sumário

- [Descrição do Projeto](#descrição-do-projeto)
- [Tecnologias Utilizadas](#tecnologias-utilizadas)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Configuração do Ambiente](#configuração-do-ambiente)
  - [Pré-requisitos](#pré-requisitos)
  - [Configurando o Mosquitto Broker](#configurando-o-mosquitto-broker)
  - [Compilação do Código](#compilação-do-código)
- [Funcionamento do Sistema](#funcionamento-do-sistema)
- [Desenvolvimento](#desenvolvimento)
- [Como Contribuir](#como-contribuir)
- [Licença](#licença)

---

## Descrição do Projeto

Este projeto consiste em um **robô protótipo** que utiliza a placa de desenvolvimento **VESPA** e é programado em **C++**. O foco principal é a criação de um **sistema de remediação** para mitigar cyberataques em robôs que operam utilizando o protocolo **MQTT**. Para isso, estamos utilizando o **Mosquitto Broker** executado em um container **Docker**, permitindo a execução de todas as aplicações localmente.

A proposta do sistema é desenvolver um **webapp** que disponibiliza diversas funcionalidades para robôs industriais e implementar um mecanismo de **contramedida** para responder a cyberataques. O robô é equipado com um **sensor ultrassônico**, que será utilizado em modo autônomo para realizar a detecção e prevenção de ameaças.

## Tecnologias Utilizadas

- **Linguagem de Programação:** C++
- **Placa de Desenvolvimento:** VESPA - ROBOCORE
- **Protocolo de Comunicação:** MQTT
- **Broker MQTT:** Mosquitto
- **Containerização:** Docker
- **Web Framework:** [Express.js,]

## Estrutura do Projeto

```plaintext
safe-robot/
│
├── code-robot-c++/       # Código-fonte do robô
│   └── code-robot-c++.ino # Arquivo principal do robô
│
└── webapp-mqtt/          # Código-fonte do WebApp
    ├── frontend/         # Código do front-end
    │   ├── index.html    # Página inicial do front-end
    │   ├── menu.html     # Página do menu do front-end
    │   ├── manual.html   # Página do modo manual do front-end
    │   ├── manutencao.html  # Página de manutenção do front-end
    │   └── styles.css    # Estilos do front-end
    │
    └── backend/          # Código do back-end
        ├── server.js     # Lógica do servidor
        └── package.json    # Conexão com o banco de dados
        └── package-lock..json    # Conexão com o banco de dados

```

## Configuração do Ambiente

### Pré-requisitos

Certifique-se de ter as seguintes ferramentas instaladas:

- [Docker](https://www.docker.com/get-started)
- [C++ Compiler](https://gcc.gnu.org/) (ex: GCC)
- [MQTT Client](https://mosquitto.org/)

### Configurando o Mosquitto Broker

1. Clone o repositório:
   ```bash
   git clone <URL do repositório>
   cd projeto-robo
   ```

2. Inicie o Docker e execute o Mosquitto Broker:
   ```bash
   docker-compose up -d
   ```

3. Acesse o Mosquitto Broker em `localhost:1883`.

### Compilação do Código

Para compilar o código C++, use o seguinte comando dentro da pasta `code-robot-c++`:

```bash
arduino-cli compile --fqbn esp8266:esp8266:vespa code-robot-c++.ino
```

## Funcionamento do Sistema

O sistema opera da seguinte maneira:

1. **Comunicação MQTT:** O robô se conecta ao broker MQTT para enviar e receber mensagens.
2. **Detecção de Ameaças:** O sensor ultrassônico realiza medições e, em caso de detecção de anomalias, o sistema entra em modo autônomo.
3. **Contramedidas:** O robô executa ações predefinidas para contrarrestar o cyberataque, enviando relatórios e atualizações ao webapp.
4. **Webapp:** O webapp fornece uma interface gráfica para monitoramento e controle das funções do robô.

## Desenvolvimento

### Como Contribuir

1. Fork este repositório.
2. Crie uma nova branch para suas alterações:
   ```bash
   git checkout -b feature/nome-da-sua-feature
   ```
3. Commit suas alterações:
   ```bash
   git commit -m "Descrição das alterações"
   ```
4. Envie para o seu repositório:
   ```bash
   git push origin feature/nome-da-sua-feature
   ```
5. Crie um Pull Request.

## Licença

Este projeto está sob a licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.

---
