var Socket;
var host = "192.168.100.69";
var lastId;
function init() {
    Socket = new WebSocket("ws://" + host + ":80/ws");
    Socket.onmessage = function (event) {
        var data = JSON.parse(event.data);
        if (data.sensorData) {
            updateTable(data.sensorData);
        }

        if (data.temperatureF >= 92) {
            document.getElementById("tempF").style.color = "red";
            document.getElementById("tempC").style.color = "red";
        } else {
            document.getElementById("tempF").style.color = "#0f377a";
            document.getElementById("tempC").style.color = "#165249";
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
            displayStorage([
                100 / (res.totalSpace / res.usedBytes).toFixed(1),
                (
                    ((res.totalSpace - res.usedBytes) / res.totalSpace) *
                    100
                ).toFixed(1),
            ]);
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
    var td1 = "<td>" + (parseInt(lastId) + 1) + "</td>";
    var td2 =
        "<td>" +
        new Date(
            new Date(0).setUTCSeconds(sensorData["date"])
        ).toLocaleString() +
        "</td>";
    var td3 = "<td>" + sensorData["tempC"] + "</td>";
    var td4 = "<td>" + sensorData["tempF"] + "</td>";
    lastId = parseInt(lastId) + 1;
    $("#people").text(parseInt($("#people").text()) + 1);

    $("#tableBody").append(tr + td1 + td2 + td3 + td4);
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
            lastId = res[res.length - 1]["tid"];
            for (var i = 0; i < res.length; i++) {
                var tr = `<tr class=${colorSwitch(res[i]["tempF"])}>`;
                var td1 = "<td>" + res[i]["tid"] + "</td>";
                var td2 =
                    "<td>" +
                    new Date(
                        new Date(0).setUTCSeconds(res[i]["date"])
                    ).toLocaleString() +
                    "</td>";
                var td3 = "<td>" + res[i]["tempC"] + "</td>";
                var td4 = "<td>" + res[i]["tempF"] + "</td>";

                $("#tableBody").append(tr + td1 + td2 + td3 + td4);
            }
        });
}

function displayStorage(showData) {
    const pieData = {
        labels: ["Used", "Available Space"],
        datasets: [
            {
                label: "Storage",
                data: showData,
                backgroundColor: ["rgb(54, 162, 235)", "rgb(255, 99, 132)"],
                hoverOffset: 2,
            },
        ],
    };

    let ctx = document.getElementById("countries").getContext("2d");
    new Chart(ctx, {
        type: "pie",
        data: pieData,
        options: {
            responsive: false,
            maintainAspectRatio: true,
            plugins: {
                datalabels: {
                    borderWidth: 5,
                    borderColor: "white",
                    borderRadius: 8,
                    color: 0,
                    font: {
                        weight: "bold",
                    },
                    backgroundColor: "lightgray",
                },
            },
        },
    });
}
