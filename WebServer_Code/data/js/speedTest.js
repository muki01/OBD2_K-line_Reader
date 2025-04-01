import { InitWebSocket, setMessageHandler, sendData } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const klStatus = document.getElementById("kl");
const timeRef = document.getElementById("timerDisplay");
const startBtn = document.getElementById("start-timer");
const resetBtn = document.getElementById("reset-timer");
const statusBox = document.getElementById("statusBox");
const status = document.getElementById("status");
const speedBox = document.getElementById("speedBox");
const speed = document.getElementById("Speed");

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
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";

        if (wsMessage.KLineStatus != false) {
            statusBox.style.display = "none";
            speedBox.style.display = "block";
            speed.innerHTML = wsMessage.Speed;

            if (counterEnabled == true && wsMessage.Speed > 0) {
                counterEnabled = false;
                startTimer();
                sendData("beep");
            } else if (stoperEnabled == false && wsMessage.Speed >= 100) {
                stoperEnabled = true;
                pauseTimer();
                sendData("beep");
            }
        }else{
            statusBox.style.display = "block";
            status.innerHTML = "Not Connected to the Vehicle.";
            speedBox.style.display = "none";
        }

        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();