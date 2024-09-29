import { InitWebSocket, setMessageHandler, sendData } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let clearDTC = document.getElementById("clearDTC");

clearDTC.addEventListener("click", function () {
    sendData("clear_dtc");
})

let errorsField = document.getElementById("errorCodes");
let mil = document.getElementById("DistanceTraveledWithMIL");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
    
        let dtcArray = wsMessage.DTCs;
        if (dtcArray.length > 0) {
            mil.innerHTML = wsMessage.DistanceTraveledWithMIL;
            document.querySelector(".MIL_Box").style.display = "flex";
            
            errorsField.innerHTML = dtcArray.join(", ");
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
