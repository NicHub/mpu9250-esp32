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

        var layout = {
            yaxis: {
                range: [-200, 200],
                tickmode: "linear",
                tick0: 0,
                dtick: 20
            }
        }

        Plotly.plot('divgraph1', [{
            y: [],
            mode: 'lines+markers',
            marker: { color: '#FF0018', size: 8 },
            name: 'Angle A',
            line: { width: 4 }
        }, {
            y: [],
            mode: 'lines+markers',
            marker: { color: '#18FF00', size: 8 },
            name: 'Angle B',
            line: { width: 4 }
        }, {
            y: [],
            mode: 'lines+markers',
            marker: { color: '#008BFF', size: 8 },
            name: 'Angle C',
            line: { width: 4 }
        }
        ], layout);

        ws.onmessage = function (evt) {
            var data = JSON.parse(evt.data);
            if (!data.hasOwnProperty("quaternions") || !data.hasOwnProperty("angles")) {
                return;
            }
            Plotly.extendTraces('divgraph1', {
                y: [[data.angles.A], [data.angles.B], [data.angles.C]]
            }, [0, 1, 2], 10)
        };
    }
}
