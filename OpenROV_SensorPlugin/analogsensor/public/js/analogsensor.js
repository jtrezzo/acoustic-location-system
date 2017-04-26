(function(window) 
{
    'use strict';
    class AnalogSensor 
    {
        constructor( cockpit )
        {
            console.log('AnalogSensor Plugin running');

            var self = this;
            self.cockpit = cockpit;

            self.settings = null;     // These get sent by the local model

            //Set up actions associated with this plugin
            this.actions = 
            {
                "plugin.analogsensor.sayHello":
                {
                    description: "Say Hello!",
                    controls:
                    {
                        button:
                        {
                            down: function() {
                                cockpit.emit('plugin.analogsensor.sayHello');
                            }
                        }
                    }
                }
            };

            // Setup input handlers
            this.inputDefaults =
            {
                keyboard:
                {
                    "alt+0": { type: "button",
                               action: "plugin.analogsensor.sayHello"}
                }
            };
        };

        sayHello()
        {
          // Send the sayHello command to the node plugin
          this.cockpit.rov.emit( 'plugin.analogsensor.sayHello' );
        }

        getTelemetryDefinitions()
        {
            return [{
                name: 'analogsensor.message',
                description: 'The message sent from the analogsensor module in the MCU'
            }]
        };

        // This pattern will hook events in the cockpit and pull them all back
        // so that the reference to this instance is available for further processing
        listen() 
        {
            var self = this;

            // Listen for settings from the node plugin
            this.cockpit.rov.withHistory.on('plugin.analogsensor.settingsChange', function(settings)
            {
                // Copy settings
                self.settings = settings;

                // Re-emit on cockpit
                self.cockpit.emit( 'plugin.analogsensor.settingsChange', settings );
            });

            // Listen for response messages from the Node plugin
            this.cockpit.rov.withHistory.on('plugin.analogsensor.message', function( message )
            {
                // Log the message!
                console.log( "AnalogSensor Plugin says: " + message );

                // Rebroadcast for other plugins and widgets in the browser
                self.cockpit.emit( 'plugin.analogsensor.message', message );
            });

            // Listen for sayHello requests from other plugins and widgets
            this.cockpit.on('plugin.analogsensor.sayHello', function()
            {
                self.sayHello();
            });
        };
    };

    // Add plugin to the window object and add it to the plugins list
    var plugins = namespace('plugins');
    plugins.AnalogSensor = AnalogSensor;
    window.Cockpit.plugins.push( plugins.AnalogSensor );

}(window));