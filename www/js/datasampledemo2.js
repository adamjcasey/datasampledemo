$(document).ready(function() {
    var g1 = new JustGage({
        id: "gauge_voltage",
        value: 0,
        min: 0,
        max: 600,
        title: "",
        label: "",
        levelColors: [
            "#dd5f32"
        ],
        relativeGaugeSize: true,
        hideValue: true
    });

    var g2 = new JustGage({
        id: "gauge_current",
        value: 0,
        min: 0,
        max: 100,
        title: "",
        label: "",
        levelColors: [
            "#adc4cc"
        ],
        relativeGaugeSize: true,
        hideValue: true
    });

    // Graphs
    // var voltage1 = new TimeSeries();
    // var voltage2 = new TimeSeries();
    // var voltage3 = new TimeSeries();

    // var current1 = new TimeSeries();
    // var current2 = new TimeSeries();
    // var current3 = new TimeSeries();

    // // Add the charts
    // var smoothie_voltage = new SmoothieChart({ grid: { strokeStyle: 'rgb(63, 63, 63)', fillStyle: 'rgb(42, 42, 42)', lineWidth: 1, millisPerLine: 250, verticalSections: 6 } });
    // smoothie_voltage.addTimeSeries(voltage1, { strokeStyle: 'rgb(221, 95, 50)', lineWidth: 3 });
    // smoothie_voltage.addTimeSeries(voltage2, { strokeStyle: 'rgb(225, 157, 41)', lineWidth: 3 });
    // smoothie_voltage.addTimeSeries(voltage3, { strokeStyle: 'rgb(146, 176, 106)', lineWidth: 3 });
    // smoothie_voltage.addTimeSeries(current1, { strokeStyle: 'rgb(173, 196, 204)', lineWidth: 3 });
    // smoothie_voltage.addTimeSeries(current2, { strokeStyle: 'rgb(66, 152, 181)', lineWidth: 3 });
    // smoothie_voltage.addTimeSeries(current3, { strokeStyle: 'rgb(170, 155, 213)', lineWidth: 3 });
    // smoothie_voltage.streamTo(document.getElementById("current"), 1000);
var ctx = $("#graph_canvas");
var myChart = new Chart(ctx, {
  type: "line",
  data: {
    datasets: [{
      label: "V Phase 1",
      fill: "false",
      borderColor: "#dd5f32",
      yAxisID: "y-axis-0",
      data: []
    }, {
      label: "V Phase 2",
      fill: "false",
      borderColor: "#e19d29",
      yAxisID: "y-axis-0",
      data: []
    }, {
      label: "V Phase 3",
      fill: "false",
      borderColor: "#92b06a",
      yAxisID: "y-axis-0",
      data: []
    }, {
      label: "I Phase 1",
      fill: "false",
      borderColor: "#adc4cc",
      yAxisID: "y-axis-1",
      data: []
    }, {
      label: "I Phase 2",
      fill: "false",
      borderColor: "#4298b5",
      yAxisID: "y-axis-1",
      data: []
    }, {
      label: "I Phase 3",
      fill: "false",
      borderColor: "#aa9bd5",
      yAxisID: "y-axis-1",
      data: []
    }]
  },
  options: {
    scales: {
      yAxes: [{
        position: "left",
        id: "y-axis-0"
      }, {
        position: "right",
        id: "y-axis-1"
      }]
    }
  }
});
    // WebSockets
    if (!window.WebSocket) {
        alert("Your browser does not support the WebSocket API!");
        return;
    }

    // Set the websocket server URL
    var websocketurl = "ws://172.28.128.3:8088";

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
      //console.log("Message");
      var dataobject = JSON.parse(message.data);

      //console.log(dataobject);

      myChart.data.datasets[0].data = dataobject.fifteenmins.v1;
      myChart.data.datasets[1].data = dataobject.fifteenmins.v2;
      myChart.data.datasets[2].data = dataobject.fifteenmins.v3;
      myChart.data.datasets[3].data = dataobject.fifteenmins.i1;
      myChart.data.datasets[4].data = dataobject.fifteenmins.i2;
      myChart.data.datasets[5].data = dataobject.fifteenmins.i3;

      myChart.update();

      // Update the Voltage gauge with an average of three phases
      var v_avg = ((dataobject.live.v1 + dataobject.live.v2 + dataobject.live.v3) / 3.0).toFixed(1);
      $("#vavg").text(v_avg);
      g1.refresh(parseInt(v_avg));

      // Update the Current gauge with an average of three phases
      var i_avg = ((dataobject.live.i1 + dataobject.live.i2 + dataobject.live.i3) / 3.0).toFixed(1);
      $("#iavg").text(i_avg);
      g2.refresh(parseInt(i_avg));

      // Update the table
      // Live values
      $("#v1_1").text(dataobject.live.v1.toFixed(1));
      $("#v2_1").text(dataobject.live.v2.toFixed(1));
      $("#v3_1").text(dataobject.live.v3.toFixed(1));
      $("#i1_1").text(dataobject.live.i1.toFixed(1));
      $("#i2_1").text(dataobject.live.i2.toFixed(1));
      $("#i3_1").text(dataobject.live.i3.toFixed(1));

      // 15 minutes
      $("#v1_2").text(_.mean(dataobject.fifteenmins.v1).toFixed(1));
      $("#v2_2").text(_.mean(dataobject.fifteenmins.v2).toFixed(1));
      $("#v3_2").text(_.mean(dataobject.fifteenmins.v3).toFixed(1));
      $("#i1_2").text(_.mean(dataobject.fifteenmins.i1).toFixed(1));
      $("#i2_2").text(_.mean(dataobject.fifteenmins.i2).toFixed(1));
      $("#i3_2").text(_.mean(dataobject.fifteenmins.i3).toFixed(1));

      // 1 Hour
      $("#v1_3").text(_.mean(dataobject.onehour.v1).toFixed(1));
      $("#v2_3").text(_.mean(dataobject.onehour.v2).toFixed(1));
      $("#v3_3").text(_.mean(dataobject.onehour.v3).toFixed(1));
      $("#i1_3").text(_.mean(dataobject.onehour.i1).toFixed(1));
      $("#i2_3").text(_.mean(dataobject.onehour.i2).toFixed(1));
      $("#i3_3").text(_.mean(dataobject.onehour.i3).toFixed(1));

      // 10 hours
      $("#v1_4").text(_.mean(dataobject.tenhours.v1).toFixed(1));
      $("#v2_4").text(_.mean(dataobject.tenhours.v2).toFixed(1));
      $("#v3_4").text(_.mean(dataobject.tenhours.v3).toFixed(1));
      $("#i1_4").text(_.mean(dataobject.tenhours.i1).toFixed(1));
      $("#i2_4").text(_.mean(dataobject.tenhours.i2).toFixed(1));
      $("#i3_4").text(_.mean(dataobject.tenhours.i3).toFixed(1));

      // 24 hours
      $("#v1_5").text(_.mean(dataobject.twentyfourhours.v1).toFixed(1));
      $("#v2_5").text(_.mean(dataobject.twentyfourhours.v2).toFixed(1));
      $("#v3_5").text(_.mean(dataobject.twentyfourhours.v3).toFixed(1));
      $("#i1_5").text(_.mean(dataobject.twentyfourhours.i1).toFixed(1));
      $("#i2_5").text(_.mean(dataobject.twentyfourhours.i2).toFixed(1));
      $("#i3_5").text(_.mean(dataobject.twentyfourhours.i3).toFixed(1));


      // voltage1.append(new Date().getTime(), v1);
      // voltage2.append(new Date().getTime(), v2);
      // voltage3.append(new Date().getTime(), v3);
      // current1.append(new Date().getTime(), i1);
      // current2.append(new Date().getTime(), i2);
      // current3.append(new Date().getTime(), i3); 

    };

});









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