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
            if (!data.hasOwnProperty("quaternions") || !data.hasOwnProperty("angles")) {
                return;
            }
            dialA.setAttribute("transform", transfA + data.angles.A + ")");
            dialB.setAttribute("transform", transfB + data.angles.B + ")");
            dialC.setAttribute("transform", transfC + data.angles.C + ")");
            dialAangle.textContent = Number(data.angles.A).toFixed(2) + "°";
            dialBangle.textContent = Number(data.angles.B).toFixed(2) + "°";
            dialCangle.textContent = Number(data.angles.C).toFixed(2) + "°";
        };
    }
}
