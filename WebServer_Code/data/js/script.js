import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");
let voltage = document.getElementById("voltage");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        let batteryVoltage = wsMessage.Voltage;
        voltage.innerHTML = `${batteryVoltage} V`;
        wsStatus.style.color = "#00ff00";
        wsStatus.innerHTML = "Connected";
        if (wsMessage.KLineStatus == true) {
            klStatus.style.color = "#00ff00";
            klStatus.innerHTML = "Connected";
        } else {
            klStatus.style.color = "red";
            klStatus.innerHTML = "Not Connected";
        }

        if (batteryVoltage > 11.8 && batteryVoltage <= 12.6) {
            voltage.style.color = "#ffe100";
        } else if (batteryVoltage <= 11.8) {
            voltage.style.color = "red";
        } else if (batteryVoltage > 12.6) {
            voltage.style.color = "#00ff00";
        }
    } else {
        wsStatus.style.color = "red";
        wsStatus.innerHTML = "Not Connected";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
