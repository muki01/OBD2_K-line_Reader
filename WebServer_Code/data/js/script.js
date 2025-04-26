import { InitWebSocket, setMessageHandler } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const vehicleStatus = document.getElementById("vehicleStatus");
const voltage = document.getElementById("voltage");


function handleWebSocketMessage(wsMessage) {
    if (!wsMessage) {
        wsStatus.style.color = "red";
        wsStatus.innerHTML = "Not Connected";
        return;
    }

    wsStatus.style.color = "#00ff00";
    wsStatus.innerHTML = "Connected";

    let batteryVoltage = wsMessage.Voltage.toFixed(1);
    voltage.innerHTML = `${batteryVoltage} V`;


    if (wsMessage.vehicleStatus == true) {
        vehicleStatus.style.color = "#00ff00";
        vehicleStatus.innerHTML = "Connected";
    } else {
        vehicleStatus.style.color = "red";
        vehicleStatus.innerHTML = "Not Connected";
    }

    if (batteryVoltage >= 12.0 && batteryVoltage <= 12.6) {
        voltage.style.color = "#ffe100";
    } else if (batteryVoltage < 12.0) {
        voltage.style.color = "red";
    } else if (batteryVoltage > 12.6) {
        voltage.style.color = "#00ff00";
    }

}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
