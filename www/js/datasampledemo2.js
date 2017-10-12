//-----------------------------------------------------------------------------------------
// Globals
var dataobject = null;
var graph_display = "live";
var live = {
    v1: Array.from(Array(60), () => 0),
    v2: Array.from(Array(60), () => 0),
    v3: Array.from(Array(60), () => 0),
    i1: Array.from(Array(60), () => 0),
    i2: Array.from(Array(60), () => 0),
    i3: Array.from(Array(60), () => 0)
};

//-----------------------------------------------------------------------------------------
// Gauge objects
var gauge_voltage = new JustGage({
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

var gauge_current = new JustGage({
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


//-----------------------------------------------------------------------------------------
// Graph Object
var ctx = $("#graph_canvas");
var voltage_current_chart = new Chart(ctx, {
    type: "line",
    data: {
        labels: Array.from(Array(60), () => ""),
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


$(document).ready(function() {

    //-----------------------------------------------------------------------------------------
    // Button click handlers
    $("#button_live").click(function(){
        graph_display = "live";
        if (dataobject)
            UpdateGraph();
    });
    
    $("#button_fifteenmins").click(function(){
        graph_display = "fifteenmins";        
        if (dataobject)
            UpdateGraph();
    });
    
    $("#button_onehour").click(function(){
        graph_display = "onehour";
        if (dataobject)
            UpdateGraph();
    });
    
    $("#button_tenhours").click(function(){
        graph_display = "tenhours";        
        if (dataobject)
            UpdateGraph();
    });
    
    $("#button_twentyfourhours").click(function(){
        graph_display = "twentyfourhours";
        if (dataobject)
            UpdateGraph();
    });

    //-----------------------------------------------------------------------------------------
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
        dataobject = JSON.parse(message.data);

        UpdateLiveData();
        UpdateGraph();
        UpdateGauges();
        UpdateTable();

    };

});

function UpdateLiveData()
{
    if (!dataobject)
        return;

    // Update the live data
    live.v1.unshift(dataobject.live.v1.toFixed(1));
    live.v1.length = 60;
    live.v2.unshift(dataobject.live.v2.toFixed(1));
    live.v2.length = 60;
    live.v3.unshift(dataobject.live.v3.toFixed(1));
    live.v3.length = 60;
    live.i1.unshift(dataobject.live.i1.toFixed(1));
    live.i1.length = 60;
    live.i2.unshift(dataobject.live.i2.toFixed(1));
    live.i2.length = 60;
    live.i3.unshift(dataobject.live.i3.toFixed(1));
    live.i3.length = 60;
}

function UpdateGraph()
{
    var dataset;

    if (!dataobject)
        return;

    // Get the correct dataset
    switch (graph_display)
    {
        case "live":
            dataset = live;
            break;
        case "fifteenmins":
            dataset = dataobject.fifteenmins;
            break;
        case "onehour":
            dataset = dataobject.onehour;
            break;
        case "tenhours":
            dataset = dataobject.tenhours;
            break;
        case "twentyfourhours":
            dataset = dataobject.twentyfourhours;
            break;
    }

    // Fill the dataset with zeros
    var v1graph = dataset.v1.slice();
    var v2graph = dataset.v2.slice();
    var v3graph = dataset.v3.slice();
    var i1graph = dataset.i1.slice();
    var i2graph = dataset.i2.slice();
    var i3graph = dataset.i3.slice();

    while (v1graph.length < 60)
        v1graph.push(0);
    while (v2graph.length < 60)
        v2graph.push(0);
    while (v3graph.length < 60)
        v3graph.push(0);
    while (i1graph.length < 60)
        i1graph.push(0);
    while (i2graph.length < 60)
        i2graph.push(0);
    while (i3graph.length < 60)
        i3graph.push(0);

    // Get the min max for all voltage.  Excluding zeros. Then add 1x to the top and 2x to the bottom for proper display
    var vmax = 0;
    var vmin = 1000;
    v1graph.forEach(function(v) {
        if ((v > 0) && (v > vmax))
            vmax = v;
        if ((v > 0) && (v < vmin))
            vmin = v;
    });
    v2graph.forEach(function(v) {
        if ((v > 0) && (v > vmax))
            vmax = v;
        if ((v > 0) && (v < vmin))
            vmin = v;
    });
    v3graph.forEach(function(v) {
        if ((v > 0) && (v > vmax))
            vmax = v;
        if ((v > 0) && (v < vmin))
            vmin = v;
    });

    var range = vmax - vmin;
    var graph_vmax = Math.ceil(parseFloat(vmax + range));
    var graph_vmin = Math.floor(parseFloat(vmin - (2.0 * range)));

    voltage_current_chart.options.scales.yAxes[0].ticks.max = graph_vmax;
    voltage_current_chart.options.scales.yAxes[0].ticks.min = graph_vmin - 2;
    console.log("graph_vmax: " + graph_vmax + "graph_vmin: " + graph_vmin);

    // Get the min max for all current.  Excluding zeros. Then add 2x to the top and 1x to the bottom for proper display
    var imax = 0;
    var imin = 1000;
    i1graph.forEach(function(i) {
        if ((i > 0) && (i > imax))
            imax = i;
        if ((i > 0) && (i < imin))
            imin = i;
    });
    i2graph.forEach(function(i) {
        if ((i > 0) && (i > imax))
            imax = i;
        if ((i > 0) && (i < imin))
            imin = i;
    });
    i3graph.forEach(function(i) {
        if ((i > 0) && (i > imax))
            imax = i;
        if ((i > 0) && (i < imin))
            imin = i;
    });

    range = imax - imin;
    var graph_imax = Math.ceil(parseFloat(imax + (2.0 * range)));
    var graph_imin = Math.floor(parseFloat(imin -  range));

    voltage_current_chart.options.scales.yAxes[1].ticks.max = graph_imax + 2;
    voltage_current_chart.options.scales.yAxes[1].ticks.min = graph_imin;

    voltage_current_chart.data.datasets[0].data = _.reverse(v1graph);
    voltage_current_chart.data.datasets[1].data = _.reverse(v2graph);
    voltage_current_chart.data.datasets[2].data = _.reverse(v3graph);
    voltage_current_chart.data.datasets[3].data = _.reverse(i1graph);
    voltage_current_chart.data.datasets[4].data = _.reverse(i2graph);
    voltage_current_chart.data.datasets[5].data = _.reverse(i3graph);

    voltage_current_chart.update();
}

function UpdateTable()
{
    if (!dataobject)
        return;

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
}

function UpdateGauges()
{
    if (!dataobject)
        return;
    
    // Update the Voltage gauge with an average of three phases
    var v_avg = ((dataobject.live.v1 + dataobject.live.v2 + dataobject.live.v3) / 3.0).toFixed(1);
    $("#vavg").text(v_avg);
    gauge_voltage.refresh(parseInt(v_avg));

    // Update the Current gauge with an average of three phases
    var i_avg = ((dataobject.live.i1 + dataobject.live.i2 + dataobject.live.i3) / 3.0).toFixed(1);
    $("#iavg").text(i_avg);
    gauge_current.refresh(parseInt(i_avg));    
}