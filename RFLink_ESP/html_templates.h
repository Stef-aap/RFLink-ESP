
#ifndef html_templates_h
#define html_templates_h



// ***********************************************************************************
const char HTML_Sensor_StartPagina_Begin [] PROGMEM = R"(<!doctype html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    table { border-collapse: collapse; }
    table, td, th { border: 1px solid black; }
    tr:nth-child(even) {background-color: #f2f2f2;}
  </style>
</head>
<body>
<h2><img src="logo.gif" style="width:24px;height:24px;" /> MiRa Sensors Data</h2>)" ; 


const char HTML_Sensor_StartPagina_Begin_Settings  [] PROGMEM = R"(<a href="Settings.html" target="_self" >Settings</a><br>)" ; 
const char HTML_Sensor_StartPagina_Begin_BuildInfo [] PROGMEM = R"(<a href="Build_Info.html" target="_self" >Simpel Build Info</a><br>)" ;
const char HTML_Sensor_StartPagina_Begin_Values    [] PROGMEM = R"(<a href="Values.html" target="_self" >Current Values</a><br>)" ;


// ***********************************************************************************
const char HTML_Sensor_StartPagina_End [] PROGMEM = R"(<br><br>
<a href="Stop_Recording.html" target="_self" >Stop Recording</a><br><br>
<a href="Factory_Settings.html" target="_self" >Factory Settings</a><br><br>
<a href="Format_SPIFFS.html" target="_self" >Format SPIFFS  (werkt nog niet)</a>
</body>
</html>)" ;


// ***********************************************************************************
const char HTML_Build_Info_Pagina_Begin [] PROGMEM = R"(<!doctype html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
<body>
<h2><img src="logo.gif" style="width:24px;height:24px;" /> MiRa Build Info (only for experts)</h2>
<a href="index.html" target="_self" >StartPage</a><br><br>
<form action="/Build_Page.php">)" ;



// ***********************************************************************************
const char HTML_Stop_Recording [] PROGMEM = "<html><body><h2> Trying to stop Recording </h2></body></html>" ;



// ***********************************************************************************
const char HTML_Values_Pagina_Begin [] PROGMEM = R"(<!doctype html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <script type="text/javascript" src="http://code.jquery.com/jquery-1.7.1.min.js"></script>
  <style>
    table { border-collapse: collapse; }
    table, td, th { border: 1px solid black; }
    tr:nth-child(even) {background-color: #f2f2f2;}
  </style>
</head>

<body>
<script>
  //var myVar = setInterval(myTimer, 5000);
  function myTimer() {
    var d = new Date();
    var t = d.toLocaleTimeString();
    document.getElementById("field_id").innerHTML = t;
  } 
  
  $(function() {
  // request data every 5 seconds
  setInterval ( requestData, 5000 ) ;

  function requestData() {
    // ajax request for latest sensor data
    $.get( "/Xsensors" )
      .done ( function ( data ) {
        //console.log(data);  // debugging - remove when satisfied
        if ( data ) { 
          $.each(data, function(key, value) {
            console.log(key, value);
            $("#"+key).text(value);
          });
        }
      }).fail(function() {
        console.log("The was a problem retrieving the data.");
      });
  }
  requestData();
  });
  
</script>
<h2><img src="logo.gif" style="width:24px;height:24px;" /> MiRa Actual Values</h2>
<a href="index.html" target="_self" >StartPage</a><br><br>)" ;

// ***********************************************************************************
const char HTML_Values_Pagina_End [] PROGMEM = R"(<br>
</body>
</html>)" ;



// https://pimylifeup.com/arduino-web-server/
// https://stackoverflow.com/questions/44809589/web-server-on-esp32-how-to-update-and-display-sensor-values-from-the-server-aut



// ***********************************************************************************
const char HTML_Sensor_Settings_Begin [] PROGMEM = R"(<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
<body>
<h2><img src="logo.gif" style="width:24px;height:24px;" /> MiRa Sensors Settings</h2>
<a href="index.html" target="_self" >StartPage</a><br><br>
<form action="/Settings_Page.php">)" ;

// ***********************************************************************************
const char HTML_Settings_Done [] PROGMEM = R"(<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
<body>
<h2><img src="logo.gif" style="width:24px;height:24px;" /> Gewijzigde Instellingen zijn opgeslagen. de ESP wordt opnieuw gestart.</h2>
<a href="index.html" target="_self" >StartPage</a><br><br>
</body>
</html>)" ;




// ***********************************************************************************
const char HTML_Graph_Page_Begin [] PROGMEM = R"(<!doctype html>
<html>
<head>
  <title>MiRa ESP</title>
  <script src = "https://cdnjs.cloudflare.com/ajax/libs/dygraph/1.0.1/dygraph-combined.js"></script>
</head>
<body>
<a href="index.html" target="_self" >StartPage</a><br><br>)" ;


// ***********************************************************************************
// LET OP deze is heel gevoelig en bevat bovendien het eindteken   )"  
//   hiervoor is een extra spatie ingevoegd
// ***********************************************************************************
const char HTML_Dygraph_Signal_Select [] PROGMEM = R"(<input type="checkbox" id="XXX" onclick="change_0(this) " checked>  <label for="XXX"> XXX_Label</label><br>)" ;

// ***********************************************************************************
const char HTML_Dygraph [] PROGMEM = R"(<div id="dygraph_XXX" style="width:100%; height:320px;"></div>
<p>CBXs</p>
<script type="text/javascript">
  graph_XXX = new Dygraph (
    document.getElementById ( "dygraph_XXX" ),
    "/FFF",
    {
      visibility: [ true, true, true, true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true ],
      legend  : 'always',
      title   : 'FFF',
      showRangeSelector            : true,
      rangeSelectorHeight          : 30,
    }
  );
  setStatus_XXX();
  function setStatus_XXX() {
    document.getElementById("visibility").innerHTML = graph_XXX.visibility().toString();
  }
  function change_XXX(el) {
    graph_XXX.setVisibility(parseInt(el.id), el.checked);
    setStatus_XXX();
  }
</script>)" ;


#endif