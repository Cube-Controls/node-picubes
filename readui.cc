/*********************************************************************
 * PiCubes - Native libarry for Pi-Cubes Modules
 *
 * Copyright (c) 2015 Cube-Controls Inc.
 *
 * MIT License 
 ********************************************************************/

#include <nan.h>
#include "readui.h"  // NOLINT(build/include)
#include <sys/ioctl.h>			
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;

class readUIWorker : public AsyncWorker {
 public:
  readUIWorker(Callback *callback, int module, int input, int typeui)
    : AsyncWorker(callback), module(module), input(input), typeui(typeui) {}
  ~readUIWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute () {
    unsigned char buf[10];
    int fd;
	
	if ((module < 1) || (module > 6))
	{
		SetErrorMessage("Module number out of range");
		return;
	}

	if ((input < 1) || (input > 4))
	{
		SetErrorMessage("Input number out of range");
		return;
	} 
	
    // Open port for reading and writing
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) 
    {				
		SetErrorMessage("Failed to open i2c-1 port.");
		return;  
    }
	
	 // Set the port options and set the address of the device we wish to speak to
	if (ioctl(fd, I2C_SLAVE, 0x1C) < 0) 
	{					
		SetErrorMessage("Unable to get bus access to talk to Pi-Cubes I/O module.");
		return;  
	}
	
	 // calculate address
	buf[0] = (unsigned char)((module - 1) * 8 + (input - 1) * 2);
	buf[1] = (unsigned char)(typeui);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 2)) != 2) 
	{									
		SetErrorMessage("Error writing to Pi-Cubes I/O module.");
		return;  	 
	}
	
	 // calculate address
	buf[0] = (unsigned char)(48 + (module - 1) * 16 + (input - 1) * 4);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 1)) != 1) 
	{									
		SetErrorMessage("Error writing to Pi-Cubes I/O module.");
		return;  	 
	}
	
	// Send address and read value from I/O module
	if ((read(fd, buf, 4)) != 4) 
	{									
		SetErrorMessage("Error reading from Pi-Cubes I/O module.");
		return;  	 
	}	
	
	value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	
	// Close Port
	close(fd);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    HandleScope scope;

    Local<Value> argv[] = {
        Null(),
		New<Number>(value)
    };

    callback->Call(2, argv);
  }

 private:
  int module;
  int input;
  int typeui;
  int32_t value;
};

// Asynchronous readUI
NAN_METHOD(readUI) {
  int module = To<int>(info[0]).FromJust();
  int input = To<int>(info[1]).FromJust();
  int typeui = To<int>(info[2]).FromJust();
  Callback *callback = new Callback(info[3].As<Function>());
  
  if (info.Length() != 4)
  {
    return Nan::ThrowError("Incorect number of parameters passed to readUI function !");
  }
  
  AsyncQueueWorker(new readUIWorker(callback, module, input, typeui));
}

// Synchronous readUI
NAN_METHOD(readUISync) {
	int module = To<int>(info[0]).FromJust();
	int input = To<int>(info[1]).FromJust();
	int typeui = To<int>(info[2]).FromJust();
    int32_t value;
	unsigned char buf[10];
    int fd;
    
	if (info.Length() != 3)
	{
		return Nan::ThrowError("Incorect number of parameters passed to readUISync function !");
	}
	if ((module < 1) || (module > 6))
	{
		return Nan::ThrowRangeError("Module number out of range");
	}

	if ((input < 1) || (input > 4))
	{
		return Nan::ThrowRangeError("Input number out of range");
	} 
	
    // Open port for reading and writing
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) 
    {				
		return;
    }
	
	 // Set the port options and set the address of the device we wish to speak to
	if (ioctl(fd, I2C_SLAVE, 0x1C) < 0) 
	{					
		return;
	}
	
	 // calculate address
	buf[0] = (unsigned char)((module - 1) * 8 + (input - 1) * 2);
	buf[1] = (unsigned char)(typeui);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 2)) != 2) 
	{									
		return;
	}
	
	 // calculate address
	buf[0] = (unsigned char)(48 + (module - 1) * 16 + (input - 1) * 4);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 1)) != 1) 
	{									
		return;
	}
	
	// Send address and read value from I/O module
	if ((read(fd, buf, 4)) != 4) 
	{									
		return;
	}	
	
	value = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	
	info.GetReturnValue().Set(value);
  

}
