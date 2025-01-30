import { InitWebSocket, setMessageHandler, sendData } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const klStatus = document.getElementById("kl");

const btnArea = document.getElementById("btnArea");
const clearDTC = document.getElementById("clearDTC");

clearDTC.addEventListener("click", function () {
    sendData("clear_dtc");
});

const errorsField = document.getElementById("errorCodes");
const mil = document.getElementById("DistanceTraveledWithMIL");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";

        let DTCs = wsMessage.DTCs;
        if (wsMessage.KLineStatus == false) {
            errorsField.innerHTML = "Not Connected to the Vehicle.";
        } else if (DTCs) {
            errorsField.innerHTML = DTCs;
            btnArea.style.display = "flex";
        } else {
            errorsField.innerHTML = "No errors detected.";
        }

        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
