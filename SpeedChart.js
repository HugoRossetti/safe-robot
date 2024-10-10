const ctx = document.getElementById('speedChart').getContext('2d');
const speedChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [], // Os tempos ou momentos em que as medições são feitas
        datasets: [{
            label: 'Velocidade dos Motores',
            data: [], // Os dados de velocidade que vêm do robô
            borderColor: '#00ff00',
            borderWidth: 2,
            fill: false
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: true,
                max: 100
            }
        }
    }
});

// Função para atualizar o gráfico com os dados do robô
function updateSpeedChart(speedData) {
    const currentTime = new Date().toLocaleTimeString();
    speedChart.data.labels.push(currentTime);
    speedChart.data.datasets[0].data.push(speedData);

    // Mantém o gráfico com no máximo 10 pontos
    if (speedChart.data.labels.length > 10) {
        speedChart.data.labels.shift();
        speedChart.data.datasets[0].data.shift();
    }

    speedChart.update();
}

// Simulação de atualização do gráfico com valores dos motores
setInterval(() => {
    const simulatedSpeed = Math.floor(Math.random() * 100);  // Substitua com os valores reais
    updateSpeedChart(simulatedSpeed);
}, 2000);
