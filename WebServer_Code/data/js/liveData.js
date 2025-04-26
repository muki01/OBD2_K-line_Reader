import { InitWebSocket, setMessageHandler } from "./webSocket.js";

const wsStatus = document.getElementById("ws");
const vehicleStatus = document.getElementById("vehicleStatus");

const dataBox = document.getElementById("dataBox");
const statusBox = document.getElementById("statusBox");
const status = document.getElementById("status");


function handleWebSocketMessage(wsMessage) {
    if (!wsMessage) {
        wsStatus.style.fill = "red";
        return;
    }

    wsStatus.style.fill = "#00ff00";
    vehicleStatus.style.fill = wsMessage.vehicleStatus ? "#00ff00" : "red";


    if (wsMessage.vehicleStatus == false) {
        statusBox.style.display = "block";
        status.innerHTML = "Not Connected to the Vehicle.";
        dataBox.innerHTML = "";
    } else {
        const liveData = wsMessage.LiveData;
        let batteryVoltage = wsMessage.Voltage.toFixed(1);
        statusBox.style.display = "none";
        dataBox.innerHTML = "";
        for (let key in liveData) {
            if (liveData.hasOwnProperty(key)) {
                const data = liveData[key];

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
        if (batteryVoltage) {
            const voltageBox = document.createElement("div");
            voltageBox.classList.add("box");

            voltageBox.innerHTML = `
                    <h1><span>${batteryVoltage}</span> V</h1>
                    <div class="name">
                        <h2>Battery Voltage</h2>
                    </div>
                `;

            dataBox.appendChild(voltageBox);
        }
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
