import { InitWebSocket, setMessageHandler } from "./webSocket.js";


const staticIpRadio = document.getElementById("staticIp");
const dynamicIpRadio = document.getElementById("dynamicIp");
const staticIpMenu = document.querySelector(".staticIP-menu");
let menu = `
            <h1>Static IP Options</h1>
            <div class="row">
                <label class="label" for="ipAddr">IP Address:</label>
                <input type="text" class="input" id="ipAddr" name="ipAddr" required minlength="7" maxlength="15" placeholder="Enter IP Address">
            </div>
            <div class="row">
                <label class="label" for="subnetMask">Subnet Mask:</label>
                <input type="text" class="input" id="subnetMask" required minlength="7" maxlength="15" name="subnetMask"placeholder="Enter Subnet Mask">
            </div>
            <div class="row">
                <label class="label" for="gateway">Gateway:</label>
                <input type="text" class="input" id="gateway" name="gateway" required minlength="7" maxlength="15" placeholder="Enter Gateway">
            </div>`;

staticIpRadio.addEventListener("click", function () {
    if (staticIpRadio.checked) {
        staticIpMenu.classList.add("show");
        staticIpMenu.innerHTML = menu;
    }
});
dynamicIpRadio.addEventListener("click", function () {
    if (dynamicIpRadio.checked) {
        staticIpMenu.classList.remove("show");
        setTimeout(function () {
            staticIpMenu.innerHTML = " ";
        }, 400);
    }
});





const overlay = document.getElementById("overlay");
const overlayParagraph = document.getElementById("overlayParagraph");


function showOverlay(showParagraph) {
    overlay.style.display = 'flex';
    if (showParagraph) {
        overlayParagraph.style.display = 'block';
    } else {
        overlayParagraph.style.display = 'none';
    }
}

function hideOverlay() {
    overlay.style.display = 'none';
}

function submitForm(event, formId, url, showParagraph) {
    event.preventDefault();
    const formData = new FormData(document.getElementById(formId));
    const xhr = new XMLHttpRequest();
    xhr.open("POST", url, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                showOverlay(showParagraph);
                console.log("Successfully");
            } else {
                console.error(`Error: ${xhr.status} - ${xhr.statusText}`);
            }
        }
    };
    xhr.onerror = function () {
        console.error("A network error occurred.");
    };
    xhr.send(formData);
}

const hideOverlayBtn = document.getElementById("hideOverlayBtn");
const wifiChange_Form = document.getElementById("wifiChange_Form");
const protocolChange_Form = document.getElementById("protocolChange_Form");
const firmwareUpdate_Form = document.getElementById("firmwareUpdate_Form");
const fileSystemUpdate_Form = document.getElementById("fileSystemUpdate_Form");
const selectPID_Form = document.getElementById("selectPID_Form");

hideOverlayBtn.addEventListener('click', function (event) {
    hideOverlay();
});

wifiChange_Form.addEventListener('submit', function (event) {
    submitForm(event, 'wifiChange_Form', '/wifiOptions', true);
});

protocolChange_Form.addEventListener('submit', function (event) {
    submitForm(event, 'protocolChange_Form', '/protocolOptions', false);
});

firmwareUpdate_Form.addEventListener('submit', function (event) {
    submitForm(event, 'firmwareUpdate_Form', '/firmwareUpdate', true);
});
fileSystemUpdate_Form.addEventListener('submit', function (event) {
    submitForm(event, 'fileSystemUpdate_Form', '/fileSystemUpdate', false);
});

selectPID_Form.addEventListener('submit', function (event) {
    submitForm(event, 'selectPID_Form', '/pidSelect', false);
});








const wsStatus = document.getElementById("ws");
const klStatus = document.getElementById("kl");

const protocol = document.getElementById("protocol");
const selectedProtocol = document.getElementById("selectedProtocol");
const connectedProtocol = document.getElementById("connectedProtocol");
const selectPID_Boxes = document.getElementById("selectPID_Boxes");
const selectPID_Status = document.getElementById("selectPID_Status");

let dataReceived1 = false;
let dataReceived2 = false;

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";

        if (!dataReceived1) {
            selectedProtocol.innerHTML = protocol.value = wsMessage.selectedProtocol;
            dataReceived1 = true;
        }

        if (wsMessage.KLineStatus == false) {
            connectedProtocol.innerHTML = selectPID_Status.innerHTML = "Not Connected to the Vehicle.";
        } else {
            if (!dataReceived2) {
                selectPID_Status.style.display = "none";
                selectPID_Form.style.display = "block";
                connectedProtocol.innerHTML = wsMessage.connectedProtocol;
                const supportedLiveData = wsMessage.SupportedLiveData;
                const desiredLiveData = wsMessage.DesiredLiveData;
                for (let key in supportedLiveData) {
                    if (supportedLiveData.hasOwnProperty(key)) {
                        const data = supportedLiveData[key];
                        const isChecked = desiredLiveData.includes(data.pid) ? "checked" : "";

                        const box = document.createElement("div");
                        box.classList.add("box");

                        box.innerHTML = `
                            <label for='${key}'>${key}: </label>
                            <input type='checkbox' name='${key}' value='${data.pid}' ${isChecked}>
                        `;

                        selectPID_Boxes.appendChild(box);
                    }
                }
                dataReceived2 = true;
            }
        }
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();

