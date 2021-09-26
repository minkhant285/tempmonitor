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

$(document).ready(function () {
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
});

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
            console.log(res);
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

$(document).ready(function () {
    let dataArray = [
        {
            icon: `<svg
                    xmlns="http://www.w3.org/2000/svg"
                    width="35"
                    height="35"
                    fill="currentColor"
                    style="color: #1056b7"
                    viewBox="0 0 16 16"
                >
                    <path d="M9.5 12.5a1.5 1.5 0 1 1-2-1.415V2.5a.5.5 0 0 1 1 0v8.585a1.5 1.5 0 0 1 1 1.415z" />
                    <path d="M5.5 2.5a2.5 2.5 0 0 1 5 0v7.55a3.5 3.5 0 1 1-5 0V2.5zM8 1a1.5 1.5 0 0 0-1.5 1.5v7.987l-.167.15a2.5 2.5 0 1 0 3.333 0l-.166-.15V2.5A1.5 1.5 0 0 0 8 1z" />
                </svg>`,
            text: "Not Detect!",
            descriptionText: "Temperature",
            descriptionTextColor: "#0f377a",
            backgroundColor: "#d0f1fd",
            id: "tempF",
        },
        {
            icon: `<svg
                    xmlns="http://www.w3.org/2000/svg"
                    width="35"
                    height="35"
                    fill="currentColor"
                    style="color: #1056b7"
                    viewBox="0 0 16 16"
                >
                    <path d="M9.5 12.5a1.5 1.5 0 1 1-2-1.415V2.5a.5.5 0 0 1 1 0v8.585a1.5 1.5 0 0 1 1 1.415z" />
                    <path d="M5.5 2.5a2.5 2.5 0 0 1 5 0v7.55a3.5 3.5 0 1 1-5 0V2.5zM8 1a1.5 1.5 0 0 0-1.5 1.5v7.987l-.167.15a2.5 2.5 0 1 0 3.333 0l-.166-.15V2.5A1.5 1.5 0 0 0 8 1z" />
                </svg>`,
            text: "Not Detect!",
            descriptionText: "Temperature",
            descriptionTextColor: "#165249",
            backgroundColor: "#c6f4cd",
            roundCardBgColor: "#afeabe",
            id: "tempC",
        },
        {
            icon: `<svg
            xmlns="http://www.w3.org/2000/svg"
            width="35"
            height="35"
            style="color: #c79a2f"
            fill="currentColor"
            class="bi bi-person-fill"
            viewBox="0 0 16 16"
        >
            <path
                d="M3 14s-1 0-1-1 1-4 6-4 6 3 6 4-1 1-1 1H3zm5-6a3 3 0 1 0 0-6 3 3 0 0 0 0 6z"
            />
        </svg>`,
            text: "Not Detect!",
            descriptionText: "People",
            descriptionTextColor: "#7b5217",
            backgroundColor: "#fff6cd",
            roundCardBgColor: "#f4e5af",
            id: "people",
        },
        {
            icon: ` <svg
            style="color: #b72f36"
            width="35"
            height="35"
            fill="currentColor"
            viewBox="0 0 16 16"
        >
            <path
                d="M15.985 8.5H8.207l-5.5 5.5a8 8 0 0 0 13.277-5.5zM2 13.292A8 8 0 0 1 7.5.015v7.778l-5.5 5.5zM8.5.015V7.5h7.485A8.001 8.001 0 0 0 8.5.015z"
            />
        </svg>`,
            text: "Not Detect!",
            descriptionText: "Storage",
            descriptionTextColor: "#7b202e",
            backgroundColor: "#f3c5bd",
            roundCardBgColor: "#f3c5bd",
            id: "storage",
        },
    ];

    dataArray.map((uiData) => {
        $("#cardRen").append(`<div class="column">
        <div class="top-card" style="background-color: ${uiData.backgroundColor}">
            <div
                class="top-card-round"
                style="background-color: ${uiData.roundCardBgColor}"
            >
            ${uiData.icon}
            </div>
            <div class="top-card-bottom">
                <span id="${uiData.id}" class="header-content-text">
                ${uiData.text}
                </span>
                <span style="color: ${uiData.descriptionTextColor}"> ${uiData.descriptionText} </span>
            </div>
        </div>
    </div>`);
    });
});
