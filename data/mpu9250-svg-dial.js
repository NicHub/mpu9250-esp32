"use strict"
{

    {
        console.log("### START ###");
        getIP(webSocketHandle);
    }

    function getIP(callback) {
        // Check if the HTML file is served from the ESP
        // or from a development server. This is done by
        // checking if the file `info.json` is present on
        // the server. If it is present, then we are on
        // the ESP. If we are on a development server, the
        // IP address of the ESP must be set manually below.
        var file_url = location.origin + "/info.json";
        var xhr = new XMLHttpRequest();
        xhr.open("GET", file_url, true);
        xhr.send();
        xhr.onload = function () {
            var ip;
            var ans = xhr.status;
            if (ans === 200) {
                // We are on the ESP.
                ip = location.hostname;
                document.title = "ESP32—" + document.title;
            }
            else {
                // We are on the development server.
                // Manually set ESP IP address.
                ip = "192.168.2.16";
                document.title = "DEV[" + ans + "]—" + document.title;
            }
            console.log("ip = " + ip);
            callback(ip);
        }
    }

    function webSocketHandle(ip) {
        if (! "WebSocket" in window) {
            alert("WebSocket is not supported by your browser!");
            return;
        }

        var ws = new WebSocket("ws://" + ip + "/ws", ["arduino"]);

        var dialA = document.getElementById("dialA");
        var dialB = document.getElementById("dialB");
        var dialC = document.getElementById("dialC");
        var dialAangle = document.getElementById("dialAangle");
        var dialBangle = document.getElementById("dialBangle");
        var dialCangle = document.getElementById("dialCangle");

        var transfA = "rotate(";
        var transfB = "rotate(";
        var transfC = "rotate(";

        ws.onmessage = function (evt) {
            var data = JSON.parse(evt.data);
            if (!data.hasOwnProperty("quat") || !data.hasOwnProperty("euler")) {
                return;
            }

            var ALGO = "from_esp";
            if (ALGO === "threejs") {
                var quat1 = new THREE.Quaternion(
                    data.quat.qx, data.quat.qy,
                    data.quat.qz, data.quat.qw);
                // var eulerOrder = "XYZ";    // x  OK, y NOK, z  OK
                // var eulerOrder = "ZXY";    // x NOK, y  OK, z  OK
                // var eulerOrder = "YZX";    // x  OK, y  OK, z NOK
                var eulerOrder = "YXZ";    // x NOK, y  OK, z  OK
                var eViaQ1 = new THREE.Euler().setFromQuaternion(quat1, eulerOrder);
                var mViaQ1 = new THREE.Matrix4().makeRotationFromQuaternion(quat1);
                var eViaMViaQ1 = new THREE.Euler().setFromRotationMatrix(mViaQ1, eulerOrder);

                // console.log(eViaQ1);
                // console.log(eViaMViaQ1);
                // console.log(eViaQ1);
                console.log(quat1);
                var RAD_TO_DEG = 57.295779513082320876798154814105;
                data.euler.eA = eViaMViaQ1._x * RAD_TO_DEG;
                data.euler.eB = eViaMViaQ1._y * RAD_TO_DEG;
                data.euler.eC = eViaMViaQ1._z * RAD_TO_DEG;

            } else if (ALGO === "from_esp") {
            }

            dialA.setAttribute("transform", transfA + data.euler.eA + ")");
            dialB.setAttribute("transform", transfB + data.euler.eB + ")");
            dialC.setAttribute("transform", transfC + data.euler.eC + ")");
            dialAangle.textContent = Number(data.euler.eA).toFixed(2) + "°";
            dialBangle.textContent = Number(data.euler.eB).toFixed(2) + "°";
            dialCangle.textContent = Number(data.euler.eC).toFixed(2) + "°";
        };
    }
}
