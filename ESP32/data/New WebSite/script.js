let wsStatus = document.getElementById("ws");
let klStatus = document.getElementById("kl");
InitWebSocket();
function InitWebSocket() {
    websock = new WebSocket("ws://" + window.location.hostname + ":81/");
    websock.onmessage = function (evt) {
        JSONobj = JSON.parse(evt.data);
        wsStatus.style.color = "#00ff00";
        wsStatus.innerHTML = "Connected";
        if (JSONobj.KLineStatus == true) {
            klStatus.style.color = "#00ff00";
            klStatus.innerHTML = "Connected";
        } else {
            klStatus.style.color = "red";
            klStatus.innerHTML = "Not Connected";
        }
        document.getElementById("Speed").innerHTML = JSONobj.Speed;
        document.getElementById("RPM").innerHTML = JSONobj.RPM;
        document.getElementById("CoolantTemp").innerHTML = JSONobj.CoolantTemp;
        document.getElementById("IntakeTemp").innerHTML = JSONobj.IntakeTemp;
        document.getElementById("Throttle").innerHTML = JSONobj.Throttle;
    };
}