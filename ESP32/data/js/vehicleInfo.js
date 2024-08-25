import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let liveDataField = document.getElementById("liveData");
let freezeFrameField = document.getElementById("freezeFrame");
let vehicleInfoField = document.getElementById("vehicleInfo");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        document.getElementById("VIN").innerHTML = wsMessage.VIN;
        document.getElementById("ID").innerHTML = wsMessage.ID;
        document.getElementById("IDNum").innerHTML = wsMessage.IDNum;

        if (wsMessage.KLineStatus == true) {
            klStatus.style.fill = "#00ff00";
        } else {
            klStatus.style.fill = "red";
        function updateField(field, data) {
            field.innerHTML = data.length > 0 ? data.join(", ") : "";
        }

        updateField(liveDataField, wsMessage.supportedLiveData);
        updateField(freezeFrameField, wsMessage.supportedFreezeFrame);
        updateField(vehicleInfoField, wsMessage.supportedVehicleInfo);

    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
