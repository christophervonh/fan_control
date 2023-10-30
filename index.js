let colorPicker;
const defaultColor = "#0000ff";

window.addEventListener("load", startup, false);

function startup() {
  fanControlStatus = this.getFanControlStatus();
  LEDControlStatus = ths.getLEDControlStatus();
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

let fanControlStatus = true;

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

setInterval(getFanRPM, 1000);

// Get Fan RPM speed from C++ Code in automatic mode
function getFanRPM() {
  const strang1RPM = document.querySelector("#fanControlStatus");
  const strang2RPM = document.querySelector("#fanControlStatus");
}
// Update Fan RPM via Slider Element
function updateFanRPM(element) {
  var sliderNumber = element.id.charAt(element.id.length - 1);
  var sliderValue = document.getElementById(element.id).value;
  document.getElementById("fanRange" + sliderNumber).innerHTML = sliderValue;
  console.log(sliderValue);
  //websocket.send(sliderNumber + "s" + sliderValue.toString());
}

let LEDControlStatus = true;

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
