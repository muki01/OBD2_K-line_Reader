import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let PIDsField = document.getElementById("PIDs");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        document.getElementById("VIN").innerHTML = wsMessage.VIN;
        document.getElementById("ID").innerHTML = wsMessage.ID;
        document.getElementById("IDNum").innerHTML = wsMessage.IDNum;
        let PIDsArray = wsMessage.SuportedPIDs;
        if (PIDsArray.length > 0) {
            let PIDsString = "";
            for (let i = 0; i < PIDsArray.length; i++) {
                if (i === PIDsArray.length - 1) {
                    PIDsString += PIDsArray[i];
                } else {
                    PIDsString += PIDsArray[i] + ", ";
                }
            }
            PIDsField.innerHTML = PIDsString;
        } else {
            PIDsField.innerHTML = "Error";
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
