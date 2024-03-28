import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.color = "#00ff00";
        wsStatus.innerHTML = "Connected";
        document.getElementById("ErrorCodes").innerHTML = wsMessage.MAF;

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
