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








function showOverlay() {
    document.getElementById('overlay').style.display = 'flex';
}

function hideOverlay() {
    document.getElementById('overlay').style.display = 'none';
}

function submitForm(event, formId, url) {
    event.preventDefault();
    const formData = new FormData(document.getElementById(formId));
    const xhr = new XMLHttpRequest();
    xhr.open("POST", url, true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            showOverlay();
        }
    };
    xhr.send(formData);
}

document.getElementById('hideOverlayBtn').addEventListener('click', function (event) {
    hideOverlay();
});

document.getElementById('wifiForm').addEventListener('submit', function (event) {
    submitForm(event, 'wifiForm', '/wifiOptions');
});

document.getElementById('protocolForm').addEventListener('submit', function (event) {
    submitForm(event, 'protocolForm', '/protocolOptions');
});

document.getElementById('firmwareUpdate').addEventListener('submit', function (event) {
    submitForm(event, 'firmwareUpdate', '/firmwareUpdate');
});
document.getElementById('fileSystemUpdate').addEventListener('submit', function (event) {
    submitForm(event, 'fileSystemUpdate', '/fileSystemUpdate');
});








let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        document.getElementById("selectedProtocol").innerHTML = wsMessage.selectedProtocol;
        document.getElementById("connectedProtocol").innerHTML = wsMessage.connectedProtocol;
        
        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();

