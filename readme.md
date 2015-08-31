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
            0 = Off
            1 = On
- callback - The callback gets one argument (err).

