import { InitWebSocket, setMessageHandler } from "./webSocket.js";


let staticIpRadio = document.getElementById("staticIp");
let dynamicIpRadio = document.getElementById("dynamicIp");
let staticIpMenu = document.querySelector(".staticIP-menu");
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








function showOverlay(showParagraph) {
    document.getElementById('overlay').style.display = 'flex';
    if (showParagraph) {
        document.getElementById('overlayParagraph').style.display = 'block';
    } else {
        document.getElementById('overlayParagraph').style.display = 'none';
    }
}

function hideOverlay() {
    document.getElementById('overlay').style.display = 'none';
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

document.getElementById('hideOverlayBtn').addEventListener('click', function (event) {
    hideOverlay();
});

document.getElementById('wifiForm').addEventListener('submit', function (event) {
    submitForm(event, 'wifiForm', '/wifiOptions', true);
});

document.getElementById('protocolForm').addEventListener('submit', function (event) {
    submitForm(event, 'protocolForm', '/protocolOptions', false);
});

document.getElementById('firmwareUpdate').addEventListener('submit', function (event) {
    submitForm(event, 'firmwareUpdate', '/firmwareUpdate', true);
});
document.getElementById('fileSystemUpdate').addEventListener('submit', function (event) {
    submitForm(event, 'fileSystemUpdate', '/fileSystemUpdate', false);
});

document.getElementById('selectPID').addEventListener('submit', function (event) {
    submitForm(event, 'selectPID', '/pidSelect', false);
});








let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

let protocol = document.getElementById("protocol");
let selectedProtocol = document.getElementById("selectedProtocol");
let connectedProtocol = document.getElementById("connectedProtocol");
let selectArea = document.getElementById("selectArea");

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
            connectedProtocol.innerHTML = selectArea.innerHTML = "Not Connected to the Vehicle.";
        } else {
            if (!dataReceived2) {
                connectedProtocol.innerHTML = wsMessage.connectedProtocol;
                const supportedLiveData = wsMessage.SupportedLiveData;
                for (let key in supportedLiveData) {
                    if (supportedLiveData.hasOwnProperty(key)) {
                        const data = supportedLiveData[key];

                        const box = document.createElement("div");
                        box.classList.add("box");

                        box.innerHTML = `
                            <label for='${key}'>${key}: </label>
                            <input type='checkbox' name='${key}' value='${data.pid}'>
                        `;

                        selectArea.appendChild(box);
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

