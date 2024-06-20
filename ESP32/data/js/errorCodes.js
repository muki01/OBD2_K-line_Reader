import { InitWebSocket, setMessageHandler, sendData } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let clearDTC = document.getElementById("clearDTC");

clearDTC.addEventListener("click", function () {
    sendData("clear_dtc");
})

let errorsField = document.getElementById("errorCodes");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        let dtcArray = wsMessage.DTCs;
        if (dtcArray.length > 0) {
            let errorsString = "";
            for (let i = 0; i < dtcArray.length; i++) {
                if (i === dtcArray.length - 1) {
                    errorsString += dtcArray[i];
                } else {
                    errorsString += dtcArray[i] + ", ";
                }
            }
            errorsField.innerHTML = errorsString;
        } else {
            errorsField.innerHTML = "Not Errors Found";
        }

        if (wsMessage.KLineStatus == true) {
            klStatus.style.fill = "#00ff00";
        } else {
            klStatus.style.fill = "red";
        }
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
