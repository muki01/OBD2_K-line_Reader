let ws = null;
let messageHandler = null;

export function InitWebSocket() {
    ws = new WebSocket("ws://" + window.location.hostname + ":81/");
    ws.onopen = function () {
        console.log("WebSocket connected !");
    };
    ws.onmessage = function (event) {
        let JSONobj = JSON.parse(event.data);
        // console.log("Websocket message:", JSONobj);
        if (messageHandler) {
            messageHandler(JSONobj);
        }
    };
    ws.onclose = function () {
        console.log("WebSocket closed.");
        InitWebSocket();
    };
    ws.onerror = function (error) {
        console.error("WebSocket error:", error);
    };
}

export function setMessageHandler(handler) {
    messageHandler = handler;
}
