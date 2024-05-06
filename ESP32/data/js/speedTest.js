import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");
let timeRef = document.querySelector(".timerDisplay");
let startBtn = document.getElementById("start-timer");
let pauseBtn = document.getElementById("pause-timer");
let resetBtn = document.getElementById("reset-timer");

let [milliseconds, seconds, minutes] = [0, 0, 0];
let interval = null;
let starter = false;

startBtn.addEventListener("click", () => {
    starter = true;
});

pauseBtn.addEventListener("click", pauseTimer);
resetBtn.addEventListener("click", resetTimer);

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
    timeRef.innerHTML = "00 : 00 : 000 ";
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
        wsStatus.style.color = "#00ff00";
        wsStatus.innerHTML = "Connected";
        document.getElementById("Speed").innerHTML = wsMessage.Speed;

        if (starter == true && wsMessage.Speed > 0) {
            starter = false;
            startTimer();
        } else if (wsMessage.Speed >= 100) {
            pauseTimer();
        }

        if (wsMessage.KLineStatus == true) {
            klStatus.style.color = "#00ff00";
            klStatus.innerHTML = "Connected";
        } else {
            klStatus.style.color = "red";
            klStatus.innerHTML = "Not Connected";
        }
    } else {
        wsStatus.style.color = "red";
        wsStatus.innerHTML = "Not Connected";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();