// Simulação inicial para testes
let batteryLevel = 100;  // 100% inicial
let wifiStrength = 100;  // Sinal Wi-Fi forte inicial

// Função para atualizar status da bateria e Wi-Fi baseado em valores do robô
function updateStatus(batteryLevel, wifiStrength) {
    let batteryIcon = document.getElementById('battery-icon');
    let batteryText = document.getElementById('battery-level');
    let wifiIcon = document.getElementById('wifi-icon');
    let wifiText = document.getElementById('wifi-level');

    // Atualizar nível da bateria e alterar cor conforme nível
    if (batteryLevel > 75) {
        batteryIcon.className = 'fa fa-battery-full';
        batteryIcon.style.color = '#00ff00';  // Verde
    } else if (batteryLevel > 50) {
        batteryIcon.className = 'fa fa-battery-three-quarters';
        batteryIcon.style.color = '#ffff00';  // Amarelo
    } else if (batteryLevel > 25) {
        batteryIcon.className = 'fa fa-battery-half';
        batteryIcon.style.color = '#ff9900';  // Laranja
    } else {
        batteryIcon.className = 'fa fa-battery-quarter';
        batteryIcon.style.color = '#ff0000';  // Vermelho
    }
    batteryText.textContent = Math.floor(batteryLevel) + '%';

    // Atualizar nível de Wi-Fi e alterar cor conforme sinal
    wifiIcon.style.color = wifiStrength > 50 ? '#00ff00' : '#ff0000';
    wifiText.textContent = Math.floor(wifiStrength) + '%';
}

// Simular atualização a cada 2 segundos
setInterval(() => {
    // Aqui você pode substituir pelos valores reais vindos do robô
    updateStatus(batteryLevel, wifiStrength);

    // Detecção de ciberataque com popup
    if (wifiStrength < 50) {
        document.getElementById('cyberattack-popup').classList.add('show');
        // Comando para mudar o robô para o modo autônomo
        console.log("Robô mudando para o modo autônomo devido ao ataque.");
        // Coloque aqui o comando que será enviado ao robô
    } else {
        document.getElementById('cyberattack-popup').classList.remove('show');
    }
}, 2000);

