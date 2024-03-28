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
