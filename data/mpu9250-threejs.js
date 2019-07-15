"use strict"
var scene;
var camera;
var renderer;
var cube;
var scene3dcontainer = document.getElementById("scene3dcontainer");
var infoPanel = document.getElementById("infoPanel");

{
    console.log("### START ###");
    CubeBegin();
    getIP(webSocketHandle);
}

function render() {
    requestAnimationFrame(render);
    renderer.render(scene, camera);
}

window.onresize = function () {
    CubeBegin()
};

function CubeBegin() {
    scene3dcontainer.innerHTML = "";

    scene = new THREE.Scene();

    camera = new THREE.PerspectiveCamera(
        650,
        scene3dcontainer.clientWidth / scene3dcontainer.clientHeight,
        0.1,
        1000
    );

    renderer = new THREE.WebGLRenderer();
    renderer.setSize(scene3dcontainer.clientWidth, scene3dcontainer.clientHeight);
    scene3dcontainer.appendChild(renderer.domElement);

    var geometry = new THREE.BoxGeometry(100, 50, 20);
    var cubeMaterials = [
        new THREE.MeshBasicMaterial({ color: 0xfe4365 }),
        new THREE.MeshBasicMaterial({ color: 0xfc9d9a }),
        new THREE.MeshBasicMaterial({ color: 0xf9cdad }),
        new THREE.MeshBasicMaterial({ color: 0xc8cba9 }),
        new THREE.MeshBasicMaterial({ color: 0x83af98 }),
        new THREE.MeshBasicMaterial({ color: 0xe5fcc2 })
    ];
    cube = new THREE.Mesh(geometry, cubeMaterials);
    scene.add(cube);
    camera.position.z = 200;
    render();
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
    var T1 = +new Date();

    ws.onmessage = function (evt) {
        var T2 = +new Date();
        var deltaTms = T2 - T1;
        var refreshRate = 1000 / deltaTms;
        T1 = T2;

        infoPanel.innerHTML = "<p>" + evt.data + "</p><p>Refresh rate = " + refreshRate.toFixed() + " Hz (ΔT = " + deltaTms.toFixed() + " ms)</p>";

        var data = JSON.parse(evt.data);
        if (!data.hasOwnProperty("quaternions") || !data.hasOwnProperty("angles")) {
            return;
        }
        var q0 = data.quaternions.q0;
        var q1 = data.quaternions.q1;
        var q2 = data.quaternions.q2;
        var q3 = data.quaternions.q3;
        var quat1 = new THREE.Quaternion(q1, q2, q3, q0);
        var quat2 = new THREE.Quaternion(1, 0, 0, 0);

        cube.quaternion.multiplyQuaternions(quat1, quat2);
    };

    ws.onclose = function () {
        infoPanel.innerHTML = "<p>WebSocket connection is closed</p>";
    };
}
