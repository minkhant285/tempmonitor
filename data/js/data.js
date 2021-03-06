var Socket;
var host = "192.168.100.69";
var lastId;
var displaySotragePercent;
function init() {
    Socket = new WebSocket("ws://" + host + ":80/ws");
    Socket.onmessage = function (event) {
        var data = JSON.parse(event.data);
        let ramPercent = (100 / (data.totalHeap / data.freeHeap)).toFixed(0);

        $("#ramBar").text(ramPercent + "%");
        $("#ramBar").width(ramPercent + "%");
        if (ramPercent >= 90) {
            $("#ramBar").css({ "background-color": "#f00" });
        } else {
            $("#ramBar").css({ "background-color": "#0483aa" });
        }
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
    if (tempValue >= 90) {
        return "red";
    }
    return "";
}

$(document).ready(function () {
    let selector = $("#dpicker");
    selector.val(new Date().toISOString().split("T")[0]);
    selector.change(() => {
        var dt = new Date(selector.val());
        let ye = new Intl.DateTimeFormat("en", { year: "numeric" }).format(dt);
        let mo = new Intl.DateTimeFormat("en", { month: "2-digit" }).format(dt);
        let da = new Intl.DateTimeFormat("en", { day: "2-digit" }).format(dt);

        getDBData(`${ye}-${mo}-${da}`, `${ye}-${mo}-${da}`);
    });
});

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
            displaySotragePercent = sotragePercent.toFixed(0);
            $("#storage").text(sotragePercent.toFixed(0) + "%");
        });
});

$(function () {
    $("#myForm").submit(function (e) {
        e.preventDefault();

        let ssid = $("#ssid").val();
        let pass = $("#password").val();

        updateWifiData(ssid, pass);
    });
});

function updateWifiData(ssid, pass) {
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

function getDBData(date1, date2) {
    fetch(`http://${host}/selectdrange?d1=${date1}&d2=${date2}`, {
        headers: {
            "Content-Type": "application/json",
            Accept: "application/json",
        },
    })
        .then((response) => response.json())
        .then((res) => {
            if (res.length == 0) {
                $("#tableBody").empty();
            } else {
                $("#tableBody").empty();
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
            }
            move("storageBar");
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

var i = 0;
function move(type) {
    if (i == 0) {
        i = 1;
        var elem = document.getElementById("storageBar");
        var width = 0;
        var id = setInterval(frame, 10);
        function frame() {
            if (width >= displaySotragePercent) {
                clearInterval(id);
                i = 0;
            } else {
                width++;
                elem.style.width = width + "%";
                elem.innerHTML = width + "%";
            }
        }
    }
}

// Get the modal
var modal = document.getElementById("myModal");

// Get the button that opens the modal
var btn = document.getElementById("myBtn");

// Get the <span> element that closes the modal
var span = document.getElementsByClassName("close")[0];

// When the user clicks the button, open the modal
btn.onclick = function () {
    modal.style.display = "block";
};

// When the user clicks on <span> (x), close the modal
span.onclick = function () {
    modal.style.display = "none";
};

// When the user clicks anywhere outside of the modal, close it
window.onclick = function (event) {
    if (event.target == modal) {
        modal.style.display = "none";
    }
};
