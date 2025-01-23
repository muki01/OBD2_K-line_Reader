import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let VIN = document.getElementById("VIN");
let ID = document.getElementById("ID");
let IDNum = document.getElementById("IDNum");
let liveDataField = document.getElementById("liveData");
let freezeFrameField = document.getElementById("freezeFrame");
let vehicleInfoField = document.getElementById("vehicleInfo");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        if (wsMessage.KLineStatus == false) {
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

        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
