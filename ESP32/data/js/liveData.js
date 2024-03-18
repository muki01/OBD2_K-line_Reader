import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.color = "#00ff00";
        wsStatus.innerHTML = "Connected";
        document.getElementById("Speed").innerHTML = wsMessage.Speed;
        document.getElementById("RPM").innerHTML = wsMessage.RPM;
        document.getElementById("CoolantTemp").innerHTML = wsMessage.CoolantTemp;
        document.getElementById("IntakeTemp").innerHTML = wsMessage.IntakeTemp;
        document.getElementById("Throttle").innerHTML = wsMessage.Throttle;

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
