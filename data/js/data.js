var Socket;
var host = "192.168.100.69";
function init() {
    Socket = new WebSocket("ws://" + host + ":80/ws");
    Socket.onmessage = function (event) {
        var data = JSON.parse(event.data);
        console.log(event.data);
        if (data.sensorData) {
            updateTable(data.sensorData);
        }
        document.getElementById("tempC").innerHTML =
            data.temperatureC + "&#8451;";
        document.getElementById("tempF").innerHTML =
            data.temperatureF + "&#8457;";
    };
}

function colorSwitch(tempValue) {
    if (tempValue >= 32) {
        return "bg-warning";
    }
    return "";
}

function getStorage() {
    fetch("http://" + host + "/fs", {
        headers: {
            "Content-Type": "application/json",
            Accept: "application/json",
        },
    })
        .then((response) => response.json())
        .then((res) => {
            let sotragePercent = 100 / (res.totalSpace / res.usedBytes);
            $("#storage").text(sotragePercent.toFixed(1) + "%");
        });
}

$(function () {
    $("#myform").submit(function (e) {
        e.preventDefault();

        let ssid = $("#ssid").val();
        let pass = $("#password").val();

        gg(ssid, pass);
    });
});

function gg(ssid, pass) {
    fetch("http://" + host + "/wconfig", {
        method: "PUT",
        body: JSON.stringify({ ssid: ssid, pass: pass }), // string or object
        headers: {
            "Content-Type": "application/json",
        },
    })
        .then((res) => res.json())
        .then((result) => console.log(result));
}

function updateTable(sensorData) {
    var tr = `<tr class=${colorSwitch(sensorData["tempF"])}>`;
    // var td1 = "<td>" + sensorData["tid"] + "</td>";
    var td2 = "<td>" + sensorData["date"] + "</td>";
    var td3 = "<td>" + sensorData["tempC"] + "</td>";
    var td4 = "<td>" + sensorData["tempF"] + "</td>";
    $("#people").text(parseInt($("#people").text()) + 1);

    $("#tableBody").append(tr + td2 + td3 + td4);
}

function getDBData() {
    fetch("http://" + host + "/select", {
        headers: {
            "Content-Type": "application/json",
            Accept: "application/json",
        },
    })
        .then((response) => response.json())
        .then((res) => {
            $("#people").text(res.length);
            for (var i = 0; i < res.length; i++) {
                var tr = `<tr class=${colorSwitch(res[i]["tempF"])}>`;
                var td1 = "<td>" + res[i]["tid"] + "</td>";
                var td2 = "<td>" + res[i]["date"] + "</td>";
                var td3 = "<td>" + res[i]["tempC"] + "</td>";
                var td4 = "<td>" + res[i]["tempF"] + "</td>";

                $("#tableBody").append(tr + td2 + td3 + td4);
            }
        });
}
