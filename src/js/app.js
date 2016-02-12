Pebble.addEventListener("ready", function(e) {});

Pebble.addEventListener("showConfiguration",
  function(e) {
    Pebble.openURL("http://192.168.1.42");
  }
);
Pebble.addEventListener("webviewclosed",
  function(e) {
    var configuration = JSON.parse(e.response);
    var backgroundColor = configuration.background_color;
    
    if (!configuration.leading_zero) {
      configuration.leading_zero = "off";
    }
    var dict = {};
        dict.KEY_COLOR_RED = parseInt(backgroundColor.substring(2, 4), 16);
        dict.KEY_COLOR_GREEN = parseInt(backgroundColor.substring(4, 6), 16);
        dict.KEY_COLOR_BLUE = parseInt(backgroundColor.substring(6), 16);
        //dict.KEY_LEADING_ZERO = configuration.leading_zero;
        dict.KEY_DATE_FORMAT = configuration.dateformat;
        dict.KEY_LOCALE = configuration.locale;
    
    console.log(e.response);
    //Pebble.sendAppMessage(configuration);
    
    // Send to watchapp
    Pebble.sendAppMessage(dict, function() {
        console.log('Send successful: ' + JSON.stringify(dict));
    }, function() {
        console.log('Send failed!');
    });
  }
);

