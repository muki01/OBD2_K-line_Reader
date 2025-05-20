import { InitWebSocket, setMessageHandler, sendData } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const vehicleStatus = document.getElementById("vehicleStatus");

const btnArea = document.getElementById("btnArea");
const clearDTC = document.getElementById("clearDTC");

clearDTC.addEventListener("click", function () {
    sendData("clear_dtc");
});

const errorsField = document.getElementById("errorCodes");
const mil = document.getElementById("DistanceTraveledWithMIL");

function handleWebSocketMessage(wsMessage) {
    if (!wsMessage) {
        wsStatus.style.fill = "red";
        return;
    }

    wsStatus.style.fill = "#00ff00";
    vehicleStatus.style.fill = wsMessage.vehicleStatus ? "#00ff00" : "red";

    let DTCs = wsMessage.DTCs;
    if (wsMessage.vehicleStatus == false) {
        errorsField.innerHTML = "Not Connected to the Vehicle.";
        btnArea.style.display = "none";
    } else if (DTCs) {
        errorsField.innerHTML = DTCs;
        btnArea.style.display = "flex";
    } else {
        errorsField.innerHTML = "No errors detected.";
        btnArea.style.display = "none";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();