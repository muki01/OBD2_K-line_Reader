let gateway = `ws://${window.location.hostname}/ws`;
let ws = null;
let messageHandler = null;

export function InitWebSocket() {
    ws = new WebSocket(gateway);
    ws.onopen = onOpen;
    ws.onmessage = onMessage;
    ws.onclose = onClose;
    ws.onerror = onError;
}

function onOpen(event) {
    console.log("WebSocket connected !");
}

function onMessage(event) {
    let JSONobj = JSON.parse(event.data);
    // console.log("Websocket message:", JSONobj);
    if (messageHandler) {
        messageHandler(JSONobj);
    }
}

function onClose(event) {
    console.log("WebSocket closed.");
    InitWebSocket();
}

function onError(error) {
    console.log("WebSocket error:", error);
}




export function sendData(data) {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(data);
        //console.log("Sending Data: ", data);
    } else {
        console.log("WebSocket is not open. Data not sent.");
    }
}

export function setMessageHandler(handler) {
    messageHandler = handler;
}