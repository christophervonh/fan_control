var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener("load", onload);

function onload(event) {
  initWebSocket();
}

function getValues() {
  websocket.send("getValues");
}

function initWebSocket() {
  console.log("Trying to open a WebSocket connection…");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
  console.log("Connection opened");
  getValues();
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
  console.log(event.data);
  var myObj = JSON.parse(event.data);
  var keys = Object.keys(myObj);

  for (var i = 0; i < keys.length; i++) {
    var key = keys[i];
    document.getElementById(key).innerHTML = myObj[key];
    document.getElementById("slider" + (i + 1).toString()).value = myObj[key];
  }
}

let colorPicker;
let LEDControlStatus = true;
let fanControlStatus = true;
const defaultColor = "#0000ff";

window.addEventListener("load", startup, false);

function startup() {
  fanControlStatus = this.getFanControlStatus();
  LEDControlStatus = this.getLEDControlStatus();
  colorPicker = document.querySelector("#color-picker");
  colorPicker.value = defaultColor;
  colorPicker.addEventListener("input", updateFirst, false);
  colorPicker.addEventListener("change", updateAll, false);
  colorPicker.select();
}

function updateFirst(event) {
  const p = document.querySelector("div");
  if (p) {
    p.style.color = event.target.value;
  }
}

function updateAll(event) {
  document.querySelectorAll("div.TEST").forEach((p) => {
    p.style.color = event.target.value;
    console.log("Test: " + event.target.value);
  });
}

// get from C++ Code
function getFanControlStatus() {
  return fanControlStatus;
}

function toggleFanControlStatus(event) {
  fanControlStatus = this.getFanControlStatus();
  if (fanControlStatus === true) {
    disableFanControl();
  } else if (fanControlStatus === false) {
    enableFanControl();
  }
}

function enableFanControl() {
  const p = document.querySelector("#fanControlStatus");
  const fan = document.querySelectorAll(".form-range");
  if (p) {
    p.textContent = "On";
    p.className = "btn btn-success";
    for (var i = 0; i < fan.length; i++) {
      fan[i].disabled = false;
    }
  }
  fanControlStatus = true;
  return fanControlStatus;
}

function disableFanControl() {
  const p = document.querySelector("#fanControlStatus");
  const fan = document.querySelectorAll(".form-range");
  if (p) {
    p.textContent = "Off";
    p.className = "btn btn-danger";
    for (var i = 0; i < fan.length; i++) {
      fan[i].disabled = true;
    }
  }
  fanControlStatus = false;
  return fanControlStatus;
}

//setInterval(getFanRPM, 1000);

// Get Fan RPM speed from C++ Code in automatic mode
function getFanRPM() {
  //let strang1RPM = document.querySelector("#fanControlStatus");
  //let strang2RPM = document.querySelector("#fanControlStatus");
  let strang1RPM = 300,
    strang2RPM = 50;
  return {
    strang1RPM: strang1RPM,
    strang2RPM: strang2RPM,
  };
}

// Update Fan RPM via Slider Element
function updateFanRPM(element) {
  var sliderNumber = element.id.charAt(element.id.length - 1);
  var sliderValue = document.getElementById(element.id).value;
  document.getElementById("fanRange" + sliderNumber).innerHTML = sliderValue;
  console.log(sliderValue);
  websocket.send(sliderNumber + "s" + sliderValue.toString());
}

// Update Fan RPM via Slider Element
// function initializeFanSlider(event) {
//   strang1RPM = this.getFanRPM().strang1RPM;
//   strang2RPM = this.getFanRPM().strang2RPM;
//   document.querySelector("#fanRange1").value = strang1RPM;
//   document.querySelector("#amountFan1").value = strang1RPM;
//   document.querySelector("#fanRange2").value = strang2RPM;
//   document.querySelector("#amountFan2").value = strang2RPM;

//   //websocket.send(sliderNumber + "s" + sliderValue.toString());
// }

// get from C++ Code
function getLEDControlStatus() {
  return LEDControlStatus;
}

function toggleLEDControlStatus(event) {
  fanControlStatus = this.getLEDControlStatus();
  if (LEDControlStatus === true) {
    disableLEDControl();
  } else if (LEDControlStatus === false) {
    enableLEDControl();
  }
}

function enableLEDControl() {
  const p = document.querySelector("#LEDControlStatus");
  const fan = document.querySelectorAll(".LED-Control");
  if (p) {
    p.textContent = "On";
    p.className = "btn btn-success";
    for (var i = 0; i < fan.length; i++) {
      fan[i].disabled = false;
    }
  }
  LEDControlStatus = true;
  return LEDControlStatus;
}

function disableLEDControl() {
  const p = document.querySelector("#LEDControlStatus");
  const fan = document.querySelectorAll(".LED-Control");
  if (p) {
    p.textContent = "Off";
    p.className = "btn btn-danger";
    for (var i = 0; i < fan.length; i++) {
      fan[i].disabled = true;
    }
  }
  LEDControlStatus = false;
  return LEDControlStatus;
}

function onMessage(event) {
  console.log(event.data);
  var myObj = JSON.parse(event.data);
  var keys = Object.keys(myObj);

  for (var i = 0; i < keys.length; i++) {
    var key = keys[i];
    console.log("AAA: " + keys);
    document.getElementById(key).innerHTML = myObj[key];
    document.getElementById("fanRange" + (i + 1).toString()).value = myObj[key];
    document.getElementById("slider" + (i + 1).toString()).value = myObj[key];
  }
}
