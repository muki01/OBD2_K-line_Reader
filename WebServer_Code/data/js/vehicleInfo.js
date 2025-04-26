import { InitWebSocket, setMessageHandler } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const vehicleStatus = document.getElementById("vehicleStatus");

const VIN = document.getElementById("VIN");
const ID = document.getElementById("ID");
const IDNum = document.getElementById("IDNum");
const liveDataField = document.getElementById("liveData");
const freezeFrameField = document.getElementById("freezeFrame");
const vehicleInfoField = document.getElementById("vehicleInfo");

function handleWebSocketMessage(wsMessage) {
    if (!wsMessage) {
        wsStatus.style.fill = "red";
        return;
    }

    wsStatus.style.fill = "#00ff00";
    vehicleStatus.style.fill = wsMessage.vehicleStatus ? "#00ff00" : "red";

    if (wsMessage.vehicleStatus == false) {
        VIN.innerHTML = "Not Connected to the Vehicle.";
        ID.innerHTML = "Not Connected to the Vehicle.";
        IDNum.innerHTML = "Not Connected to the Vehicle.";
        liveDataField.innerHTML = "Not Connected to the Vehicle.";
        freezeFrameField.innerHTML = "Not Connected to the Vehicle.";
        vehicleInfoField.innerHTML = "Not Connected to the Vehicle.";
    } else {
        VIN.innerHTML = wsMessage.VIN;
        ID.innerHTML = wsMessage.ID;
        IDNum.innerHTML = wsMessage.IDNum;
        liveDataField.innerHTML = wsMessage.supportedLiveData;
        freezeFrameField.innerHTML = wsMessage.supportedFreezeFrame;
        vehicleInfoField.innerHTML = wsMessage.supportedVehicleInfo;
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
