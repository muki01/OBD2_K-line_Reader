import { InitWebSocket, setMessageHandler } from "./webSocket.js";

let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");

function handleWebSocketMessage(wsMessage) {
    if (wsMessage) {
        wsStatus.style.fill = "#00ff00";
        document.getElementById("Speed").innerHTML = wsMessage.Speed;
        document.getElementById("RPM").innerHTML = wsMessage.RPM;
        document.getElementById("CoolantTemp").innerHTML = wsMessage.CoolantTemp;
        document.getElementById("IntakeTemp").innerHTML = wsMessage.IntakeTemp;
        document.getElementById("Throttle").innerHTML = wsMessage.Throttle;
        document.getElementById("TimingAdvance").innerHTML = wsMessage.TimingAdvance;
        document.getElementById("EngineLoad").innerHTML = wsMessage.EngineLoad;
        document.getElementById("MAF").innerHTML = wsMessage.MAF;

        if (wsMessage.KLineStatus == true) {
            klStatus.style.fill = "#00ff00";
        } else {
            klStatus.style.fill = "red";
        }
    } else {
        wsStatus.style.fill = "red";
    }
}

setMessageHandler(handleWebSocketMessage);
InitWebSocket();
