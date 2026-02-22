import { InitWebSocket, setMessageHandler, sendData } from "./webSocket.js";

let page = 0;

const wsStatusSVG = document.getElementById("wsStatusSVG");
const vehicleStatusSVG = document.getElementById("vehicleStatusSVG");

// Main Menu Page
const voltage = document.getElementById("voltage");
const liveDataBtn = document.getElementById("liveDataBtn");
const errorCodesBtn = document.getElementById("errorCodesBtn");
const freezeFrameBtn = document.getElementById("freezeFrameBtn");
const speedTestBtn = document.getElementById("speedTestBtn");
const vehicleInfoBtn = document.getElementById("vehicleInfoBtn");
const settingsBtn = document.getElementById("settingsBtn");
const backBtn = document.getElementById("backBtn");
const header = document.getElementById("header");
const menu1 = document.getElementById("menu1");
const menu2 = document.getElementById("menu2");
const menu3 = document.getElementById("menu3");
const menu4 = document.getElementById("menu4");
const menu5 = document.getElementById("menu5");
const menu6 = document.getElementById("menu6");
const mainMenu = document.getElementById("mainMenu");

const orangeText = document.getElementById("orangeText");
const normalText = document.getElementById("normalText");

const allMenus = [menu1, menu2, menu3, menu4, menu5, menu6];

let isSwitchingPage = false;

async function switchPage(targetPage, orangeTitle, normalTitle, targetMenuElement) {
    if (page === targetPage || isSwitchingPage) return;
    isSwitchingPage = true;

    // Fade out current elements
    const currentElements = [];
    if (page === 0) {
        currentElements.push(mainMenu);
    } else {
        const activeMenu = allMenus[page - 1];
        if (activeMenu) currentElements.push(activeMenu);
    }

    currentElements.forEach(el => {
        el.style.transition = "opacity 0.4s ease-in-out";
        el.style.opacity = "0";
    });

    await new Promise(resolve => setTimeout(resolve, 200));

    currentElements.forEach(el => el.style.display = "none");

    window.scrollTo({ top: 0, behavior: 'instant' });
    page = targetPage;
    sendData("page" + targetPage);

    if (orangeTitle) orangeText.textContent = orangeTitle;
    if (normalTitle) normalText.textContent = normalTitle;

    // Prepare new elements
    const newElements = [];
    if (targetPage === 0) {
        mainMenu.style.display = "block";
        newElements.push(mainMenu);
        backBtn.style.opacity = "0";
        backBtn.style.pointerEvents = "none";
    } else {
        if (targetMenuElement) {
            targetMenuElement.style.display = "block";
            newElements.push(targetMenuElement);
        }
        backBtn.style.opacity = "1";
        backBtn.style.pointerEvents = "auto";
    }

    newElements.forEach(el => {
        el.style.transition = "none";
        el.style.opacity = "0";
    });

    // Trigger reflow
    void document.body.offsetHeight;

    // Fade in new elements
    newElements.forEach(el => {
        el.style.transition = "opacity 0.2s ease-in-out";
        el.style.opacity = "1";
    });

    await new Promise(resolve => setTimeout(resolve, 200));
    isSwitchingPage = false;
}

liveDataBtn.addEventListener("click", () => {
    switchPage(1, "Live", "Data", menu1);
});

errorCodesBtn.addEventListener("click", () => {
    switchPage(2, "Error", "Codes", menu2);
});

freezeFrameBtn.addEventListener("click", () => {
    switchPage(3, "Freeze", "Frame", menu3);
});

speedTestBtn.addEventListener("click", () => {
    switchPage(4, "Speed", "Test", menu4);
});

vehicleInfoBtn.addEventListener("click", () => {
    switchPage(5, "Vehicle", "Info", menu5);
});

settingsBtn.addEventListener("click", () => {
    switchPage(6, "Settings", " ", menu6);
});

backBtn.addEventListener("click", () => {
    switchPage(0, "OBD2", " Master", null);
});


// Live Data Page
const statusBoxLiveData = document.getElementById("statusBoxLiveData");
const statusLiveData = document.getElementById("statusLiveData");
const dataBoxLiveData = document.getElementById("dataBoxLiveData");


// Error Codes Page
const statusBoxErrorCodes = document.getElementById("statusBoxErrorCodes");
const statusErrorCodes = document.getElementById("statusErrorCodes");
const dataBoxErrorCodes = document.getElementById("dataBoxErrorCodes");
const errorCodesList = document.getElementById("errorCodesList");
const btnArea = document.getElementById("btnArea");
const clearDTC_Btn = document.getElementById("clearDTC_Btn");

clearDTC_Btn.addEventListener("click", function () {
    sendData("clear_dtc");
});



// Freeze Frame Page
const statusBoxFreezeFrame = document.getElementById("statusBoxFreezeFrame");
const statusFreezeFrame = document.getElementById("statusFreezeFrame");
const dataBoxFreezeFrame = document.getElementById("dataBoxFreezeFrame");



// Speed Test Page
const statusBoxSpeedTest = document.getElementById("statusBoxSpeedTest");
const statusSpeedTest = document.getElementById("statusSpeedTest");
const dataBoxSpeedTest = document.getElementById("dataBoxSpeedTest");

const Speed = document.getElementById("Speed");
const timeRef = document.getElementById("timerDisplay");
const startBtn = document.getElementById("start-timer");
const resetBtn = document.getElementById("reset-timer");

let [milliseconds, seconds, minutes] = [0, 0, 0];
let interval = null;
let counterEnabled = false;
let stoperEnabled = false;

startBtn.addEventListener("click", () => {
    sendData("beep");
    counterEnabled = true;
    stoperEnabled = false;
});

resetBtn.addEventListener("click", () => {
    sendData("beep");
    resetTimer();
});

function startTimer() {
    if (interval !== null) {
        clearInterval(interval);
    }
    interval = setInterval(displayTimer, 10);
}
function pauseTimer() {
    clearInterval(interval);
}
function resetTimer() {
    clearInterval(interval);
    [milliseconds, seconds, minutes] = [0, 0, 0];
    timeRef.innerHTML = "00 : 00 : 000";
}

function displayTimer() {
    milliseconds += 10;
    if (milliseconds == 1000) {
        milliseconds = 0;
        seconds++;
        if (seconds == 60) {
            seconds = 0;
            minutes++;

        }
    }

    let m = minutes < 10 ? "0" + minutes : minutes;
    let s = seconds < 10 ? "0" + seconds : seconds;
    let ms =
        milliseconds < 10
            ? "00" + milliseconds
            : milliseconds < 100
                ? "0" + milliseconds
                : milliseconds;

    timeRef.innerHTML = `${m} : ${s} : ${ms}`;
}




// Vehicle Info Page
const VIN = document.getElementById("VIN");
const ID = document.getElementById("ID");
const IDNum = document.getElementById("IDNum");
const liveDataField = document.getElementById("liveData");
const freezeFrameField = document.getElementById("freezeFrame");
const vehicleInfoField = document.getElementById("vehicleInfo");


// Settings Page
const staticIpRadio = document.getElementById("staticIp");
const dynamicIpRadio = document.getElementById("dynamicIp");
const staticIpMenu = document.querySelector(".staticIP-menu");
const staticIpInputs = staticIpMenu.querySelectorAll("input");

// Live filtering: Only allow valid IP segments (0-255, max 3 digits)
staticIpInputs.forEach(input => {
    input.addEventListener("input", function () {
        let parts = this.value.replace(/[^0-9.]/g, '').split('.');

        // Limit to 4 segments
        if (parts.length > 4) parts = parts.slice(0, 4);

        const filteredParts = parts.map(part => {
            // Max 3 digits per segment
            if (part.length > 3) part = part.substring(0, 3);
            // Max value 255
            if (part !== "" && parseInt(part) > 255) part = "255";
            return part;
        });

        this.value = filteredParts.join('.');
    });
});

staticIpRadio.addEventListener("click", function () {
    if (staticIpRadio.checked) {
        staticIpMenu.classList.add("show");
        staticIpInputs.forEach(input => {
            input.required = true;
            // Restore pattern if it was removed
            input.setAttribute("pattern", "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        });
    }
});
dynamicIpRadio.addEventListener("click", function () {
    if (dynamicIpRadio.checked) {
        staticIpMenu.classList.remove("show");
        staticIpInputs.forEach(input => {
            input.required = false;
            input.value = ""; // Clear values so invalid data doesn't block submission
            input.removeAttribute("pattern"); // Remove pattern to avoid validation errors on empty/hidden fields
        });
    }
});





const overlay = document.getElementById("overlay");
const overlayParagraph = document.getElementById("overlayParagraph");


function showOverlay(showParagraph) {
    overlay.classList.remove('closing');
    overlay.style.display = 'flex';
    overlayParagraph.style.display = showParagraph ? 'block' : 'none';
}

function hideOverlay() {
    overlay.classList.add('closing');
    setTimeout(() => {
        overlay.style.display = 'none';
        overlay.classList.remove('closing');
    }, 250); // Matches the 0.25s animation duration in CSS
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

function updateConnectionStatus(isWsConnected, isVehicleConnected) {
    const colorConnected = "#00ff00";
    const colorDisconnected = "#ff0000";

    wsStatusSVG.style.fill = isWsConnected ? colorConnected : colorDisconnected;
    if (isWsConnected) {
        vehicleStatusSVG.style.fill = isVehicleConnected ? colorConnected : colorDisconnected;
    } else {
        vehicleStatusSVG.style.fill = colorDisconnected;
    }
}


const protocol = document.getElementById("protocol");
const selectedProtocol = document.getElementById("selectedProtocol");
const connectedProtocol = document.getElementById("connectedProtocol");
const selectPID_Boxes = document.getElementById("selectPID_Boxes");
const selectPID_Status = document.getElementById("selectPID_Status");

let dataReceived1 = false;
let dataReceived2 = false;



function handleWebSocketMessage(wsMessage) {
    if (!wsMessage) {
        updateConnectionStatus(false, false);
        return;
    }
    const isVehicleConnected = wsMessage.vehicleStatus;
    updateConnectionStatus(true, isVehicleConnected);

    if (page == 0) {
        let batteryVoltage = wsMessage.Voltage.toFixed(1);
        voltage.innerHTML = `${batteryVoltage} V`;


        if (batteryVoltage >= 12.0 && batteryVoltage <= 12.6) {
            voltage.style.color = "#ffe100";
        } else if (batteryVoltage < 12.0) {
            voltage.style.color = "red";
        } else if (batteryVoltage > 12.6) {
            voltage.style.color = "#00ff00";
        }
    }
    else if (page == 1) {
        if (wsMessage.vehicleStatus == false) {
            statusBoxLiveData.style.display = "block";
            statusLiveData.innerHTML = "Not Connected to the Vehicle.";
            dataBoxLiveData.innerHTML = "";
        } else {
            const liveData = wsMessage.LiveData;
            let batteryVoltage = wsMessage.Voltage.toFixed(1);
            statusBoxLiveData.style.display = "none";
            dataBoxLiveData.innerHTML = "";
            for (let key in liveData) {
                if (liveData.hasOwnProperty(key)) {
                    const data = liveData[key];

                    const box = document.createElement("div");
                    box.classList.add("live-card");

                    box.innerHTML = `
                        <div class="label">${key}</div>
                        <div class="value-area">
                            <span class="value">${data.value}</span>
                            <span class="unit">${data.unit}</span>
                        </div>
                    `;

                    dataBoxLiveData.appendChild(box);
                }
            }
            if (batteryVoltage) {
                const voltageBox = document.createElement("div");
                voltageBox.classList.add("live-card");

                voltageBox.innerHTML = `
                    <div class="label">Battery Voltage</div>
                    <div class="value-area">
                        <span class="value">${batteryVoltage}</span>
                        <span class="unit">V</span>
                    </div>
                `;

                dataBoxLiveData.appendChild(voltageBox);
            }
        }
    } else if (page == 2) {
        let DTCs = wsMessage.DTCs;
        if (wsMessage.vehicleStatus == false) {
            dataBoxErrorCodes.style.display = "none";
            statusErrorCodes.innerHTML = "Not Connected to the Vehicle.";
            statusBoxErrorCodes.style.display = "block";
            btnArea.style.display = "none";
        } else if (DTCs) {
            statusBoxErrorCodes.style.display = "none";
            dataBoxErrorCodes.style.display = "grid";
            errorCodesList.innerHTML = "";
            let dtcArray = String(DTCs).split(",").map(code => code.trim()).filter(code => code);
            dtcArray.forEach(code => {
                const box = document.createElement("div");
                box.classList.add("error-card");
                box.innerHTML = `
                    <div class="icon">
                        <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <path d="M12 9V14M12 17.5V17.51M21.1 14.5L14.7 3.4C13.5 1.3 10.5 1.3 9.3 3.4L2.9 14.5C1.7 16.6 3.2 19.3 5.6 19.3H18.4C20.8 19.3 22.3 16.6 21.1 14.5Z" stroke="#ff3b30" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
                        </svg>
                    </div>
                    <div class="details">
                        <h1 class="errorCode">${code}</h1>
                        <p class="errorMeaning">Example Meaning for ${code}</p>
                    </div>
                `;
                errorCodesList.appendChild(box);
            });
            btnArea.style.display = "flex";
        } else {
            statusBoxErrorCodes.style.display = "none";
            dataBoxErrorCodes.style.display = "grid";
            errorCodesList.innerHTML = `
                <div class="error-card success-card">
                    <div class="icon">
                        <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <path d="M20 6L9 17L4 12" stroke="#34c759" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
                        </svg>
                    </div>
                    <div class="details">
                        <h1 class="errorCode">No DTC Found</h1>
                        <p class="errorMeaning">No errors detected.</p>
                    </div>
                </div>
            `;
            btnArea.style.display = "none";
        }
    } else if (page == 3) {
        let DTCs = wsMessage.DTCs;
        if (wsMessage.vehicleStatus == false) {
            dataBoxFreezeFrame.innerHTML = "";
            dataBoxFreezeFrame.style.display = "none";
            statusFreezeFrame.innerHTML = "Not Connected to the Vehicle.";
            statusBoxFreezeFrame.style.display = "block";
        } else if (DTCs) {
            dataBoxFreezeFrame.innerHTML = "";
            dataBoxFreezeFrame.style.display = "grid";
            statusBoxFreezeFrame.style.display = "none";
            const freezeFrame = wsMessage.FreezeFrame;

            for (let key in freezeFrame) {
                if (freezeFrame.hasOwnProperty(key)) {
                    const data = freezeFrame[key];

                    const box = document.createElement("div");
                    box.classList.add("live-card");

                    box.innerHTML = `
                        <div class="label">${key}</div>
                        <div class="value-area">
                            <span class="value">${data.value}</span>
                            <span class="unit">${data.unit}</span>
                        </div>
                    `;

                    dataBoxFreezeFrame.appendChild(box);
                }
            }
        } else {
            dataBoxFreezeFrame.style.display = "none";
            statusFreezeFrame.innerHTML = "No errors detected.";
            statusBoxFreezeFrame.style.display = "block";
        }
    } else if (page == 4) {
        if (wsMessage.vehicleStatus == true) {
            statusBoxSpeedTest.style.display = "none";
            dataBoxSpeedTest.style.display = "grid";
            Speed.innerHTML = wsMessage.Speed;

            if (counterEnabled == true && wsMessage.Speed > 0) {
                counterEnabled = false;
                startTimer();
                sendData("beep");
            } else if (stoperEnabled == false && wsMessage.Speed >= 100) {
                stoperEnabled = true;
                pauseTimer();
                sendData("beep");
            }
        } else {
            statusBoxSpeedTest.style.display = "block";
            statusSpeedTest.innerHTML = "Not Connected to the Vehicle.";
            dataBoxSpeedTest.style.display = "none";
        }
    } else if (page == 5) {
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
    } else if (page == 6) {
        const protocolMap = {
            "ISO9141": "ISO9141",
            "ISO14230_Slow": "ISO14230 Slow",
            "ISO14230_Fast": "ISO14230 Fast",
            "11b250": "CAN 11Bit 250KBPS",
            "11b500": "CAN 11Bit 500KBPS",
            "29b250": "CAN 29Bit 250KBPS",
            "29b500": "CAN 29Bit 500KBPS",
            "Automatic": "Automatic"
        };

        selectedProtocol.innerHTML = protocolMap[wsMessage.selectedProtocol] || "Unknown";

        if (!dataReceived1) {
            protocol.value = wsMessage.selectedProtocol;
            dataReceived1 = true;
        }

        if (!wsMessage.vehicleStatus) {
            selectPID_Status.style.display = "block";
            selectPID_Form.style.display = "none";
            connectedProtocol.innerHTML = selectPID_Status.innerHTML = "Not Connected to the Vehicle.";
            dataReceived2 = false;
        }

        if (wsMessage.vehicleStatus && !dataReceived2) {
            connectedProtocol.innerHTML = protocolMap[wsMessage.connectedProtocol];
            selectPID_Status.innerHTML = "Waiting...";

            if (wsMessage.SupportedLiveData) {
                selectPID_Boxes.innerHTML = "";
                selectPID_Status.style.display = "none";
                selectPID_Form.style.display = "block";

                const supportedLiveData = wsMessage.SupportedLiveData;
                const desiredLiveData = wsMessage.DesiredLiveData;

                for (let key in supportedLiveData) {
                    if (!supportedLiveData.hasOwnProperty(key)) continue;

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

                dataReceived2 = true;
            }
        }
    }

}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();