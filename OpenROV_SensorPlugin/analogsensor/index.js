(function() {
  const Listener = require('Listener');
  const fs = require('fs');
  const path = require('path');
  
  const SerialPort = require('serialport'); // modified code from SerialBridge.js
  var EventEmitter = require('events').EventEmitter;
  var logger = require('AppFramework.js').logger;



  var reader = new StatusReader();

  class AnalogSensor {
    constructor(name, deps) {
      deps.logger.debug('AnalogSensor plugin loaded');
      console.log('AnalogSensor plugin loaded');

      this.globalBus = deps.globalEventLoop; // This is the server-side messaging bus. The MCU sends messages to server plugins over this
      this.cockpitBus = deps.cockpit; // This is the server<->client messaging bus. This is how the server talks to the browser

      this.hasSaidHello = false;
      
      //By defaulting, we will get zero values in the file saves if we are not actually getting these messages
      this.depth = 0;
      this.temp = 0;

      var self = this;

      // Pre-define all of the event listeners here. We defer enabling them until later.
      // Look at src/libs/Listener.js to see how these work.
      this.listeners = {
        // Listener for Settings updates
        settings: new Listener(self.globalBus, 'settings-change.analogsensor', true, function(settings) {
          // Apply settings
          self.settings = settings.analogsensor;

          // Emit settings update to cockpit
          self.cockpitBus.emit('plugin.analogsensor.settingsChange', self.settings);
        }),

        // Listener for MCU status messages
        mcuStatus: new Listener(self.globalBus, 'mcu.status', false, function(data) {
          // Check for the analogsensor field name in the MCU's status update
          if ('analogsensor' in data) {
            // Get the message that the MCU sent to us
            var message = data.analogsensor;

            // Re-emit the message on the cockpit messaging bus (talks to the browser)
            self.cockpitBus.emit('plugin.analogsensor.message', message);
          }
          
          if ('temp' in data){ //Assuming "temp" is right, it might be all caps.
            self.temp = data.temp;
          }
        }),
        
        depthStatus: new Listener(self.globalBus, 'plugin.diveProfile.depth', false, function(value){
          self.depth = value;
        }),

        sayHello: new Listener(self.cockpitBus, 'plugin.analogsensor.sayHello', false, function(powerIn) {
          var command;

          // Create a command in the format "command( parameters )"
          if (self.hasSaidHello) {
            command = 'ex_hello(' + 0 + ')';
            self.hasSaidHello = false;
          } else {
            command = 'ex_hello(' + 1 + ')';
            self.hasSaidHello = true;
          }

          // Send command to mcu
          self.globalBus.emit('mcu.SendCommand', command);
        })
      }
    }

    // This is automatically called when cockpit loads all of the plugins, and when a plugin is enabled
    start() {
      // Enable the listeners!
      this.listeners.settings.enable();
      this.listeners.mcuStatus.enable();
      this.listeners.sayHello.enable();
      this.listeners.depthStatus.enable();
      var self = this;
      var ticks = 0;

      SerialPort.list(function(err, ports) {
        ports.forEach(function(port) {
          console.log(port.comName);
        });
      });

      var uartPath = "/dev/ttyACM0" // from SerialBridge.js
      var uartBaud = 115200;

      const serialPort = new SerialPort(uartPath, {
        baudRate: uartBaud,
        //            parser: SerialPort.parsers.ReadLine('\r\n'),  // dosen't work on serialPort 5.x
        autoOpen: false
      });

      serialPort.open(function(err) {
        if (err) {
          return console.log('Error opening port: ', err.message);
        }
      });

      serialPort.flush(function(err) {
        //  setTimeout(function () {
        //    sp.close(function (err) {
        //    });
        //  }, 10);
        console.log('Serial port flushed');
      });


      var Readline;
      var parser;

      if (SerialPort.parsers.Readline) {
        Readline= SerialPort.parsers.Readline; 
        parser=serialPort.pipe(Readline({delimiter: '\r\n'}));
        console.log('Readline');
      }
      if (SerialPort.parsers.ReadLine) {        // This one for serialPort 5.x
        Readline = SerialPort.parsers.ReadLine;
        parser = serialPort.pipe(Readline({
          delimiter: '\r\n'
        }));
        console.log('ReadLine');
      }
        if (SerialPort.parsers.readline) {
        Readline= SerialPort.parsers.readline; 
        parser=serialPort.pipe(Readline({delimiter: '\r\n'}));
        console.log('readline');
      }


      serialPort.on('open', function() {
        // logger.debug('Serial port opened');
        console.log('Serial port opened');
        serialPort.write('b');              //  signal microcontroller to start "b" for begin
        console.log('wrote b');
      });

      parser.on('data', function(jsondata) {
        console.log('Got data');
        var data
        try{
          data = JSON.parse(jsondata);
          
          if (data.Group){
            var filename = "sd_" + Date.now() + ".json"
            var sampleSet = {}
            sampleSet.group = data;
            sampleSet.depth = self.depth;
            sampleSet.temp = self.temp;
            fs.appendFile(path.join("/usr/share/cockpit/analogsensor",filename),JSON.stringify(sampleSet));
          }
          
          console.dir(data);
          console.log("Group=",data.Group)
          if (data.Group == 50){
            serialPort.write('b');              //  signal microcontroller to start "b" for begin
            console.log('wrote b');            
          }
        }catch(e){
          //ignore data, not valid json
          
          console.warn("Bad json found from serial port", jsondata);
        }
        //console.log(data.ToString())
        
/*        //parseStatus turns data in to an object of name,value pairs {name:"value",name2,"value2"}
        var status = reader.parseStatus(data);
        console.log(status);
        //var status = {name:"value",name2,"value2"};
        self.globalBus.emit('mcu.status',status);
*/
//          console.log(data);     //  seems to overflow things, get beeps from ROV and no logging
      });

      serialPort.on('error', function(err) {
        // logger.debug('Serial error',err)
        console.log('Serial error', err);
      })

      serialPort.on('close', function(data) {
        // logger.debug('Serial port closed');
        console.log('Serial port closed');
      });


      /*      bridge.emit('status', status);
            if (emitRawSerial) {
              bridge.emit('serial-recieved', data + '\n');
            }

                });
       */

      var self = this;
      setInterval(function() {

        self.globalBus.emit('mcu.status', {
          jimt: ticks++
        })
      }, 1000);

    }

    // This is called when the plugin is disabled
    stop() {
      // Disable listeners
      this.listeners.settings.disable();
      this.listeners.mcuStatus.disable();
      this.listeners.sayHello.disable();
    }

    // This is used to define user settings for the plugin. We populated some analogsensor properties below.
    // The UI for changing the settings is automatically generated in the Settings applet.
    getSettingSchema() {
      //from http://json-schema.org/analogsensors.html
      return [{
        'title': 'AnalogSensor Plugin',
        'type': 'object',
        'id': 'analogsensor',
        'properties': {
          'firstName': {
            'type': 'string',
            'default': 'Open'
          },
          'lastName': {
            'type': 'string',
            'default': 'Rov'
          },
          'age': {
            'description': 'Age in years',
            'type': 'integer',
            'minimum': 0
          }
        },
        'required': [
          'firstName',
          'lastName'
        ]
      }];
    }
  }


  module.exports = function(name, deps) {
    return new AnalogSensor(name, deps);
  };

  // Helper class for parsing status messages
  function StatusReader() {
    var reader = new EventEmitter();
    var currTemp = 20;
    var currDepth = 0;
    var processSettings = function processSettings(parts) {
      var setparts = parts.split(',');
      var settingsCollection = {};
      for (var s = 0; s < setparts.length; s++) {
        var lastParts = setparts[s].split('|');
        settingsCollection[lastParts[0]] = lastParts[1];
      }
      reader.emit('firmwareSettingsReported', settingsCollection);
      return settingsCollection;
    };
    var processItemsInStatus = function processItemsInStatus(status) {
      if ('iout' in status) {
        status.iout = parseFloat(status.iout);
      }
      if ('btti' in status) {
        status.btti = parseFloat(status.btti);
      }
      if ('vout' in status) {
        status.vout = parseFloat(status.vout);
      }
    };
    reader.parseStatus = function parseStatus(rawStatus) {
      var parts = rawStatus.split(';');
      var status = {};
      for (var i = 0; i < parts.length; i++) {
        var subParts = parts[i].split(':');
        switch (subParts[0]) {
          case '*settings':
            status.settings = processSettings(subParts[1]);
            break;
          default:
            status[subParts[0]] = subParts[1];
        }
      }
      processItemsInStatus(status);
      return status;
    };
    return reader;
  };

}());
