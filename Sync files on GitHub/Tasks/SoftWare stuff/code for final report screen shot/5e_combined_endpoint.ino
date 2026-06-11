// --- Rover side: pack EVERY sensor reading into ONE response, separated by '|' ---
// One request instead of five -> much less traffic competing with the drive commands.
void handleSensors() {
  String out = irStatus() + " - " + String((int)IRpulseRate) + "/s";
  out += "|" + magStatus();
  out += "|" + usStatus();
  out += "|" + ageBest();          // age, e.g. "4.44"
  out += "|" + rockType();         // rock type
  replyAPI(out);                   // e.g. "312 - 290/s|None|Detected|4.44|Gravion"
}

// --- Browser side: poll that one endpoint, split it, fill in the dashboard ---
setInterval(function () {
  var sx = new XMLHttpRequest();
  sx.open('GET', '/sensors', true);
  sx.onload = function () {
    var p = sx.responseText.split('|');          // [ir, mag, us, age, rock]
    document.getElementById('ir-val').innerHTML  = p[0];
    document.getElementById('mag-val').innerHTML = p[1];
    document.getElementById('us-val').innerHTML  = p[2];
    document.getElementById('age-val').innerHTML = p[3];
    document.getElementById('rock-id').innerHTML = p[4];
  };
  sx.send();
}, 250);                                           // every 250ms
