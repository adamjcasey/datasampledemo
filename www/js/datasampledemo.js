
// Graphs
var voltage1 = new TimeSeries();
var voltage2 = new TimeSeries();
var voltage3 = new TimeSeries();

var current1 = new TimeSeries();
var current2 = new TimeSeries();
var current3 = new TimeSeries();

// Add the charts
var smoothie_voltage = new SmoothieChart({ grid: { strokeStyle: 'rgb(63, 63, 63)', fillStyle: 'rgb(42, 42, 42)', lineWidth: 1, millisPerLine: 250, verticalSections: 6 } });
smoothie_voltage.addTimeSeries(voltage1, { strokeStyle: 'rgb(228, 82, 79)', lineWidth: 3 });
smoothie_voltage.addTimeSeries(voltage2, { strokeStyle: 'rgb(144, 197, 100)', lineWidth: 3 });
smoothie_voltage.addTimeSeries(voltage3, { strokeStyle: 'rgb(60, 170, 235)', lineWidth: 3 });
smoothie_voltage.streamTo(document.getElementById("voltage"), 1000);

var smoothie_current = new SmoothieChart({ grid: { strokeStyle: 'rgb(63, 63, 63)', fillStyle: 'rgb(42, 42, 42)', lineWidth: 1, millisPerLine: 250, verticalSections: 6 } });
smoothie_current.addTimeSeries(current1, { strokeStyle: 'rgb(228, 82, 79)', lineWidth: 3 });
smoothie_current.addTimeSeries(current2, { strokeStyle: 'rgb(144, 197, 100)', lineWidth: 3 });
smoothie_current.addTimeSeries(current3, { strokeStyle: 'rgb(60, 170, 235)', lineWidth: 3 });
smoothie_current.streamTo(document.getElementById("current"), 1000);

// Datapoint storage
var v1, v2, v3, i1, i2, i3;
var v1total, v2total, v3total, i1total, i2total, i3total;
var v1max, v2max, v3max, i1max, i2max, i3max;
var v1min, v2min, v3min, i1min, i2min, i3min;
var numsamples;

v1total = v2total = v3total = i1total = i2total = i3total = 0;
v1max = v2max = v3max = 0;
i1max = i2max = i3max = 0;
v1min = v2min = v3min = 1000;
i1min = i2min = i3min = 125;
numsamples = 0;

// -------------------------------------------------------------------------------------------------------
// First test for the browsers support for WebSockets
if (!window.WebSocket) {
    // If the user's browser does not support WebSockets give an alert message
    alert("Your browser does not support the WebSocket API!");
} else {
    // Set the websocket server URL
    var websocketurl = "ws://172.28.128.3:8080";

    //  Create the WebSocket object
    socket = new WebSocket(websocketurl);

    // This function is called when the websocket connection is opened
    socket.onopen = function() {
      console.log("Open");
    };

    // This function is called when the websocket connection is closed
    socket.onclose = function() {
       console.log("Close");
    };

    // This function is called when the websocket receives a message. It is passed the message object as its only parameter
    socket.onmessage = function(message) {
      console.log(message.data);
      var dataobject = JSON.parse(message.data);

      v1 = parseFloat(dataobject.v1);
      v2 = parseFloat(dataobject.v2);
      v3 = parseFloat(dataobject.v3);
      i1 = parseFloat(dataobject.i1);
      i2 = parseFloat(dataobject.i2);
      i3 = parseFloat(dataobject.i3);

      UpdateGraphs();

    };
}

// This is to test without the websocket
// setInterval(function() {
//       v1 = Math.random() * 1000;
//       v2 = Math.random() * 1000;
//       v3 = Math.random() * 1000;
//       i1 = Math.random() * 125;
//       i2 = Math.random() * 125;
//       i3 = Math.random() * 125;

//       UpdateGraphs();

//       }, 1000);

function UpdateGraphs() {
  voltage1.append(new Date().getTime(), v1);
  voltage2.append(new Date().getTime(), v2);
  voltage3.append(new Date().getTime(), v3);
  current1.append(new Date().getTime(), i1);
  current2.append(new Date().getTime(), i2);
  current3.append(new Date().getTime(), i3);  

  numsamples++;

  v1total += v1;
  v2total += v2;
  v3total += v3;
  i1total += i1;
  i2total += i2;
  i3total += i3;

  if (v1 > v1max)
    v1max = v1;
  if (v1 < v1min)
    v1min = v1;
  
  if (v2 > v2max)
    v2max = v2;
  if (v2 < v2min)
    v2min = v2;
  
  if (v3 > v3max)
    v3max = v3;
  if (v3 < v3min)
    v3min = v3;
  
  if (i1 > i1max)
    i1max = i1;
  if (i1 < i1min)
    i1min = i1;
  
  if (i2 > i2max)
    i2max = i2;
  if (i2 < i2min)
    i2min = i2;
  
  if (i3 > i3max)
    i3max = i3;
  if (i3 < i3min)
    i3min = i3;

  document.getElementById("v1").innerHTML = v1.toFixed(1);
  document.getElementById("v2").innerHTML = v2.toFixed(1);
  document.getElementById("v3").innerHTML = v3.toFixed(1);
  document.getElementById("i1").innerHTML = i1.toFixed(1);
  document.getElementById("i2").innerHTML = i2.toFixed(1);
  document.getElementById("i3").innerHTML = i3.toFixed(1);

  document.getElementById("v1avg").innerHTML = (v1total / numsamples).toFixed(1);
  document.getElementById("v2avg").innerHTML = (v2total / numsamples).toFixed(1);
  document.getElementById("v3avg").innerHTML = (v3total / numsamples).toFixed(1);
  document.getElementById("i1avg").innerHTML = (i1total / numsamples).toFixed(1);
  document.getElementById("i2avg").innerHTML = (i2total / numsamples).toFixed(1);
  document.getElementById("i3avg").innerHTML = (i3total / numsamples).toFixed(1);

  document.getElementById("v1max").innerHTML = v1max.toFixed(1);
  document.getElementById("v2max").innerHTML = v2max.toFixed(1);
  document.getElementById("v3max").innerHTML = v3max.toFixed(1);
  document.getElementById("i1max").innerHTML = i1max.toFixed(1);
  document.getElementById("i2max").innerHTML = i2max.toFixed(1);
  document.getElementById("i3max").innerHTML = i3max.toFixed(1);

  document.getElementById("v1min").innerHTML = v1min.toFixed(1);
  document.getElementById("v2min").innerHTML = v2min.toFixed(1);
  document.getElementById("v3min").innerHTML = v3min.toFixed(1);
  document.getElementById("i1min").innerHTML = i1min.toFixed(1);
  document.getElementById("i2min").innerHTML = i2min.toFixed(1);
  document.getElementById("i3min").innerHTML = i3min.toFixed(1);

}
