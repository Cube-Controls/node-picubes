## node-picubes   

Node.js module for Pi-Cubes.
Pi Cubes is Modular DIY/Hobby HVAC Automation system based on the Raspberry Pi. 
Supports up to 24 I/Os and 4 Thermostats.

More information: [www.cube-controls.com/pi-cubes.html](http://www.cube-controls.com/pi-cubes.html)

## Installation

    $ npm install node-picubes

I2C has to be configured on Raspberry Pi :

[www.cube-controls.com/blog/how-to-enable-i2c-on-raspberry-pi](http://www.cube-controls.com/blog/how-to-enable-i2c-on-raspberry-pi)

## Control Pi-Cubes DO4 Module

```js
var pic = require('picubes');

pic.writeDO(1,1,value,function(err){
	            if (err) console.log(err);
   	          });
```

### picubes.writeDO(module, output, value, callback)

- module - Module address(1 - 6)
- output - Output number(1 - 4)
- value  - Digital output value :
  - 0 = Off
  - 1 = On
- callback - The callback gets one argument (err).

## Control Pi-Cubes UO4 Module

```js
var pic = require('picubes');

pic.writeUO(1,1,0,value,function(err){
	              if (err) console.log(err);
   	             });
```

### picubes.writeUO(module, output, type, value, callback)

- module - Module address(1 - 6)
- output - Output number(1 - 4)
- type   - Universal output type :
  - 0 = Digital Output(0 / 12VDC)
  - 1 = Modulation(0 - 10VDC)
  - 2 - 255 = Digital otput PWM with this period in sec.
- value  - Universal output value :
  - 0 / 1 = For digital type
  - 0 - 100 % = for Modulation / PWM
- callback - The callback gets one argument (err).

## Read Pi-Cubes UI4 Module

```js
var pic = require('picubes');

pic.readUI(3,1,1,function(err,data)
                 {
	                if (err) console.log(err);
                    console.log('Data:'+data);
   	             }
          );
```

### picubes.readUI(module, input, type, callback)

- module - Module address(1 - 6)
- output - Output number(1 - 4)
- type   - Input type
0 = Resistance(Return value 0 - 350000)
  - 1 = Digital(Return value 0 - 1)
  - 2 = Voltage / Current(Return value 0 - 100 % , represent 0 - 10V / 0 - 20mA)
  - 3 = 10K Type II Thermistor(Return value - 4000 to 14000, scale 0.01)
  - 4 = Pulse Input(Return value is counter value)
- callback - The callback gets two argument (err,data).

