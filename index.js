"use strict"

var async = require('async');
var i2c = require('i2c-bus');

module.exports.writeDO = function(module_no, output_no, output_value, callback)
{
  var i2c1;
    
  async.series([
    // first open i2c
    function (callback) 
	{
		i2c1 = i2c.open(1, callback);
    },
    // then send data via I2C
    function (callback) 
	{
	   // check is it module parameter in range
	   if ((module_no < 1) || (module_no > 6)) 
	   {
			return callback( new Error('Module number out of the range !'),null );
	   }
	   // check is it output parameter in range
	   if ((output_no < 1) || (output_no > 4)) 
	   {
			return callback( new Error('Output number out of the range !'),null );
	   }
	   // calculate address
	   var address = (module_no - 1) * 8 + (output_no - 1) * 2;
	   // send command
	   i2c1.writeByte(0x1C,address,output_value, callback);
    },
    // then close I2C
    function (callback) 
	{
		i2c1.close(callback);
    }],
    function(err) 
    {
       if (err) callback(err);
       callback(null);
    }
    );
}

module.exports.writeUO = function(module_no, output_no, output_type, output_value, callback)
{
  var i2c1;
    
  async.series([
    // first open i2c
    function (callback) 
	{
		i2c1 = i2c.open(1, callback);
    },
    // then send data via I2C
    function (callback) 
	{
       var buf = new Buffer(2);
      
       // check is it module parameter in range
	   if ((module_no < 1) || (module_no > 6)) 
	   {
			return callback( new Error('Module number out of the range !'),null );
	   }
	   // check is it output parameter in range
	   if ((output_no < 1) || (output_no > 4)) 
	   {
			return callback( new Error('Output number out of the range !'),null );
	   }
	   // calculate address
	   var address = (module_no - 1) * 8 + (output_no - 1) * 2;
	   // send command
       buf[0] = output_type;
       buf[1] = output_value;  
	   i2c1.writeI2cBlock(0x1C,address,2,buf,callback);
    },
    // then close I2C
    function (callback) 
	{
		i2c1.close(callback);
    }],
    function(err) 
    {
       if (err) callback(err);
       callback(null);
    }      
    );
}

module.exports.readUI = function(module_no, input_no, input_type, callback)
{
  var data;
  var i2c1;

  async.series([
    // first open i2c
    function (callback) 
	{
		i2c1 = i2c.open(1, callback);
    },
    // them send  type data via I2C
    function (callback) 
	{
       // check is it module parameter in range
	   if ((module_no < 1) || (module_no > 6)) 
	   {
			return callback( new Error('Module number out of the range !'),null );
	   }
	   // check is it output parameter in range
	   if ((input_no < 1) || (input_no > 4)) 
	   {
			return callback( new Error('Input number out of the range !'),null );
	   }
	   // calculate address
	   var address = (module_no - 1) * 8 + (input_no - 1) * 2;
	   // send command
	   i2c1.writeByte(0x1C,address,input_type,callback);
    },
    // then read data
    function (callback)
    {
        var buf = new Buffer(4);
        
        // calculate address
	    var reg = 48 + (module_no - 1) * 16 + (input_no -1 ) * 4;
        
        // read command
        i2c1.readI2cBlock(0x1C,reg,4,buf, function(err, bytesRead, buffer)
                                          {
                                             if (err) return callback(err,null);
                                             if (bytesRead != 4) return callback( new Error('Error in I2C Block reading !'),null );
                                             data = buffer[0] | buffer[1]<<8 | buffer[2]<<16 | buffer[3]<<24;
                                             callback(null);
                                          }
                         );
    },
    // then close I2C
    function (callback) 
	{
		i2c1.close(callback);
    }],
    function(err) 
    {
       if (err) return callback(err);
       callback(null, data);
    }
      
    );
}

