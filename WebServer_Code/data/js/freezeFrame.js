import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let dtcBox = document.getElementById("dtcBox");
let errors = document.getElementById("errors");
let dataBox = document.getElementById("dataBox");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";

        let DTCs = wsMessage.DTCs;
        if (wsMessage.KLineStatus == false) {
            errors.innerHTML = "Not Connected to the Vehicle.";
        } else if (DTCs) {
            dataBox.innerHTML = "";
            dtcBox.style.display = "none";
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
            dtcBox.style.display = "block";
            dataBox.style.display = "none";
            errors.innerHTML = "No errors detected.";
        }

        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
