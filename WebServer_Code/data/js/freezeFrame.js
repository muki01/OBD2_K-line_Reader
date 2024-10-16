import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        let dtcArray = wsMessage.DTCs;
        if (dtcArray.length > 0) {
            document.querySelector(".dtcBox").style.display = "none";
            document.querySelector(".data").style.display = "grid";
            document.getElementById("Speed").innerHTML = wsMessage.Speed;
            document.getElementById("RPM").innerHTML = wsMessage.RPM;
            document.getElementById("CoolantTemp").innerHTML = wsMessage.CoolantTemp;
            document.getElementById("EngineLoad").innerHTML = wsMessage.EngineLoad;
        } else {
            document.querySelector(".dtcBox").style.display = "block";
            document.querySelector(".data").style.display = "none";
            document.getElementById("errors").innerHTML = "No errors detected.";
        }

        klStatus.style.fill = wsMessage.KLineStatus ? "#00ff00" : "red";
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
