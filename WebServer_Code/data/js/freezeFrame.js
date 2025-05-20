import { InitWebSocket, setMessageHandler } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const vehicleStatus = document.getElementById("vehicleStatus");

const statusBox = document.getElementById("statusBox");
const status = document.getElementById("status");
const dataBox = document.getElementById("dataBox");

function handleWebSocketMessage(wsMessage) {
    if (!wsMessage) {
        wsStatus.style.fill = "red";
        return;
    }

    wsStatus.style.fill = "#00ff00";
    vehicleStatus.style.fill = wsMessage.vehicleStatus ? "#00ff00" : "red";

    let DTCs = wsMessage.DTCs;
    if (wsMessage.vehicleStatus == false) {
        dataBox.innerHTML = "";
        dataBox.style.display = "none";
        status.innerHTML = "Not Connected to the Vehicle.";
        statusBox.style.display = "block";
    } else if (DTCs) {
        dataBox.innerHTML = "";
        statusBox.style.display = "none";
        dataBox.style.display = "grid";
        const freezeFrame = wsMessage.FreezeFrame;

        for (let key in freezeFrame) {
            if (freezeFrame.hasOwnProperty(key)) {
                const data = freezeFrame[key];

                const box = document.createElement("div");
                box.classList.add("box");

                box.innerHTML = `
                        <h1><span>${data.value}</span> ${data.unit}</h1>
                        <div class="name">
                            <h2>${key}</h2>
                        </div>
                    `;

                dataBox.appendChild(box);
            }
        }
    } else {
        dataBox.style.display = "none";
        status.innerHTML = "No errors detected.";
        statusBox.style.display = "block";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();