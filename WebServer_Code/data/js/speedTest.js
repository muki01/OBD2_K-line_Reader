import { InitWebSocket, setMessageHandler, sendData } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const vehicleStatus = document.getElementById("vehicleStatus");
const statusBox = document.getElementById("statusBox");
const status = document.getElementById("status");
const dataBox = document.getElementById("dataBox");
const Speed = document.getElementById("Speed");
const timeRef = document.getElementById("timerDisplay");
const startBtn = document.getElementById("start-timer");
const resetBtn = document.getElementById("reset-timer");

let [milliseconds, seconds, minutes] = [0, 0, 0];
let interval = null;
let counterEnabled = false;
let stoperEnabled = false;

startBtn.addEventListener("click", () => {
    sendData("beep");
    counterEnabled = true;
    stoperEnabled = false;
});

resetBtn.addEventListener("click", () => {
    sendData("beep");
    resetTimer();
});

function startTimer() {
    if (interval !== null) {
        clearInterval(interval);
    }
    interval = setInterval(displayTimer, 10);
}
function pauseTimer() {
    clearInterval(interval);
}
function resetTimer() {
    clearInterval(interval);
    [milliseconds, seconds, minutes] = [0, 0, 0];
    timeRef.innerHTML = "00 : 00 : 000";
}

function displayTimer() {
    milliseconds += 10;
    if (milliseconds == 1000) {
        milliseconds = 0;
        seconds++;
        if (seconds == 60) {
            seconds = 0;
            minutes++;

        }
    }

    let m = minutes < 10 ? "0" + minutes : minutes;
    let s = seconds < 10 ? "0" + seconds : seconds;
    let ms =
        milliseconds < 10
            ? "00" + milliseconds
            : milliseconds < 100
                ? "0" + milliseconds
                : milliseconds;

    timeRef.innerHTML = `${m} : ${s} : ${ms}`;
}

function handleWebSocketMessage(wsMessage) {
    if (!wsMessage) {
        wsStatus.style.fill = "red";
        return;
    }

    wsStatus.style.fill = "#00ff00";
    vehicleStatus.style.fill = wsMessage.vehicleStatus ? "#00ff00" : "red";

    if (wsMessage.vehicleStatus == true) {
        statusBox.style.display = "none";
        dataBox.style.display = "grid";
        Speed.innerHTML = wsMessage.Speed;

        if (counterEnabled == true && wsMessage.Speed > 0) {
            counterEnabled = false;
            startTimer();
            sendData("beep");
        } else if (stoperEnabled == false && wsMessage.Speed >= 100) {
            stoperEnabled = true;
            pauseTimer();
            sendData("beep");
        }
    } else {
        statusBox.style.display = "block";
        status.innerHTML = "Not Connected to the Vehicle.";
        dataBox.style.display = "none";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();