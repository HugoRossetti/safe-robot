const canvas = document.getElementById('joystick');
const ctx = canvas.getContext('2d');
const radius = canvas.width / 2;
let joystickPos = { x: radius, y: radius };

// Desenhar o joystick no centro
function drawJoystick() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Fundo do joystick
    ctx.beginPath();
    ctx.arc(radius, radius, radius - 10, 0, Math.PI * 2, true);
    ctx.fillStyle = '#444';
    ctx.fill();

    // Parte móvel do joystick
    ctx.beginPath();
    ctx.arc(joystickPos.x, joystickPos.y, 30, 0, Math.PI * 2, true);
    ctx.fillStyle = '#ffcc00';
    ctx.fill();
}

// Atualizar posição do joystick
function moveJoystick(event) {
    let rect = canvas.getBoundingClientRect();
    let x = event.clientX - rect.left;
    let y = event.clientY - rect.top;

    // Limitar movimento à área circular
    let dx = x - radius;
    let dy = y - radius;
    let distance = Math.sqrt(dx * dx + dy * dy);

    if (distance < radius - 10) {
        joystickPos.x = x;
        joystickPos.y = y;
    }

    drawJoystick();
}

canvas.addEventListener('mousemove', moveJoystick);
canvas.addEventListener('touchmove', moveJoystick);

drawJoystick();
