import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let dataBox = document.getElementById("dataBox");
let statusBox = document.getElementById("statusBox");
let status = document.getElementById("status");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";

        if (wsMessage.KLineStatus == false) {
            status.innerHTML = "Not Connected to the Vehicle.";
        } else {
            const liveData = wsMessage.LiveData;
            const voltageData = wsMessage.Voltage.toFixed(1);
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
            if (voltageData) {
                const voltageBox = document.createElement("div");
                voltageBox.classList.add("box");

                voltageBox.innerHTML = `
                    <h1><span>${voltageData}</span> V</h1>
                    <div class="name">
                        <h2>Battery Voltage</h2>
                    </div>
                `;

                dataBox.appendChild(voltageBox);
            }
        }

        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
