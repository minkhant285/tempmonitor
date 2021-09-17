var Socket;
var host = window.location.host;
function init() {
    Socket = new WebSocket("ws://" + host + ":80/ws");
    Socket.onmessage = function (event) {
        var data = JSON.parse(event.data);
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

function getDBData() {
    var x = document.createElement("TABLE");

    x.setAttribute("id", "myTable");
    x.setAttribute("border", "1");
    document.body.appendChild(x);

    fetch("http://" + host + "/select", {
        headers: {
            "Content-Type": "application/json",
            Accept: "application/json",
        },
    })
        .then((response) => response.json())
        .then((res) => {
            var people = res.length;
            $("#people").text(people);
            var tbl = $(
                '<table class="table table-hover"/><thead><tr><th>Date</th><th>Temp &#8451;</th><th>Temp &#8457;</th></tr></thead><tbody>'
            ).attr("id", "mytable");
            $("#div1").append(tbl);
            for (var i = 0; i < res.length; i++) {
                var tr = `<tr class=${colorSwitch(res[i]["tempF"])}>`;
                var td1 = "<td>" + res[i]["tid"] + "</td>";
                var td2 = "<td>" + res[i]["date"] + "</td>";
                var td3 = "<td>" + res[i]["tempC"] + "</td>";
                var td4 = "<td>" + res[i]["tempF"] + "</td></tr></tbody>";

                $("#mytable").append(tr + td2 + td3 + td4);
            }
        });
}
