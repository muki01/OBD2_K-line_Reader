const char webpageCont[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>OBD2 Master</title>
</head>
<style>
    * {
        margin: 0;
        padding: 0;
        user-select: none;
        text-decoration: none;
        font-size: 16px;
    }
    body {
        font-family: sans-serif;
        min-height: 100vh;
        width: 100%;
        display: flex;
        flex-direction: column;
        background-color: #dddddd;
    }
    header {
        width: 100%;
        background-color: #0395d8;
        padding: 10px 0;
        display: flex;
        justify-content: center;
        box-shadow: 0 1px 10px 0 rgba(0, 0, 0, 0.4);
    }
    header h1 {
        font-size: 40px;
        color: #fff;
        text-shadow: 0 3px 5px rgba(0, 0, 0, 0.4);
        text-align: center;
    }
    main {
        margin: 20px auto;
        width: 100%;
        max-width: 1280px;
        color: #fff;
    }
    main .container{
        width: 90%;
        margin: auto;
    }
    main .status {
        display: flex;
        justify-content: space-around;
        flex-wrap: wrap;
    }
    main .status .box {
        width: 400px;
        background-color: #393939;
        border-radius: 8px;
        text-align: center;
        margin-bottom: 10px;
    }
    main .status .box h1 {
        margin: 10px 0;
        font-size: 16px;
    }
    main .data {
        display: flex;
        justify-content: space-around;
        flex-wrap: wrap;
    }
    main .data .box {
        width: 400px;
        margin-bottom: 10px;
        background-color: #393939;
        border-radius: 10px;
        text-align: center;

    }
    main .data .box h1 {
        margin: 10px 0;
        font-size: 26px;
    }
    main .data .box h1 span {
        font-size: 26px;
    }
    main .data .box .name {
        padding: 5px;
        background-color: #2d2d2d;
        border-radius: 0 0 10px 10px;
    }
    footer {
        width: 100%;
        height: auto;
        background-color: #1f2024;
        margin-top: auto;
    }
    footer .textArea {
        display: flex;
        justify-content: center;
        text-align: center;
        padding: 15px 0 20px;
    }
    footer .textArea h1 {
        font-size: 18px;
        color: white;
        margin: 5px 15px;
    }
    footer .textArea .red {
        color: red;
    }
    footer .textArea .colored {
        color: #0395d8;
    }
</style>
<body>
    <header>
        <h1>OBD2 Master</h1>
    </header>
    <main>
        <div class="container">
            <div class="status">
                <div class="box">
                    <h1>Websocket Connection: <span style="color:red" id="ws">Not Connected</span></h1>
                </div>
                <div class="box">
                    <h1>K-Line Connection: <span style="color:red" id="kl">Not Connected</span></h1>
                </div>
            </div>
            <div class="data">
                <div class="box">
                    <h1><span style="color:#0395d8" id="Speed">0</span> KM/h</h1>
                    <div class="name">
                        <h2>Speed</h2>
                    </div>
                </div>
                <div class="box">
                    <h1><span style="color:#0395d8" id="RPM">0</span> RPM</h1>
                    <div class="name">
                        <h2>RPM</h2>
                    </div>
                </div>
                <div class="box">
                    <h1><span style="color:#0395d8" id="CoolantTemp">0</span> °C</h1>
                    <div class="name">
                        <h2>Coolant Temp</h2>
                    </div>
                </div>
                <div class="box">
                    <h1><span style="color:#0395d8" id="IntakeTemp">0</span> °C</h1>
                    <div class="name">
                        <h2>Intake Temp</h2>
                    </div>
                </div>
                <div class="box">
                    <h1><span style="color:#d80303" id="Throttle">0</span> %</h1>
                    <div class="name">
                        <h2>Throttle</h2>
                    </div>
                </div>
            </div>
        </div>
    </main>
    <footer>
        <div class="container">
            <div class="textArea">
                <h1>
                    OBD2 Master <span class="red">|</span> Distributed by
                    <a href="github.com/muki01"><span class="colored">Muksin</span></a>
                </h1>
            </div>
        </div>
    </footer>
</body>
<script>
    let wsStatus = document.getElementById('ws')
    let klStatus = document.getElementById('kl')
    InitWebSocket()
    function InitWebSocket() {
        websock = new WebSocket('ws://' + window.location.hostname + ':81/');
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
            document.getElementById('Speed').innerHTML = JSONobj.Speed;
            document.getElementById('RPM').innerHTML = JSONobj.RPM;
            document.getElementById('CoolantTemp').innerHTML = JSONobj.CoolantTemp;
            document.getElementById('IntakeTemp').innerHTML = JSONobj.IntakeTemp;
            document.getElementById('Throttle').innerHTML = JSONobj.Throttle;
        }
    }
</script>
</html>
)=====";