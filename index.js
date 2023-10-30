let colorPicker;
const defaultColor = "#0000ff";

window.addEventListener("load", startup, false);

function startup() {
  fanControlStatus = this.getFanControlStatus();
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
  });
}

let fanControlStatus = true;

function getFanControlStatus() {
  return fanControlStatus;
} // get from C++ Code

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
function getFanRPM() {}
function setFanRPM() {}
