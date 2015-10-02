/*********************************************************************
 * PiCubes - Native libarry for Pi-Cubes Modules
 *
 * Copyright (c) 2015 Cube-Controls Inc.
 *
 * MIT License 
 ********************************************************************/

#include <nan.h>
#include "writedo.h"  // NOLINT(build/include)
#include <sys/ioctl.h>			
#include <fcntl.h>
#include <linux/i2c-dev.h>

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

class writeDOWorker : public AsyncWorker {
 public:
  writeDOWorker(Callback *callback, int module, int output, int value)
    : AsyncWorker(callback), module(module), output(output), value(value) {}
  ~writeDOWorker() {}

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

	if ((output < 1) || (output > 4))
	{
		SetErrorMessage("Output number out of range");
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
	buf[0] = (unsigned char)((module - 1) * 8 + (output - 1) * 2);
	buf[1] = (unsigned char)(value);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 2)) != 2) 
	{									
		SetErrorMessage("Error writing to Pi-Cubes I/O module.");
		return;  	 
	}
	
	// Close Port
	close(fd);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    HandleScope scope;

    Local<Value> argv[] = {
        Null()
    };

    callback->Call(1, argv);
  }

 private:
  int module;
  int output;
  int value;
};

// Asynchronous WriteDO
NAN_METHOD(writeDO) {
  int module = To<int>(info[0]).FromJust();
  int output = To<int>(info[1]).FromJust();
  int value = To<int>(info[2]).FromJust();
  Callback *callback = new Callback(info[3].As<Function>());
  
  if (info.Length() != 4)
  {
    return Nan::ThrowError("Incorect number of parameters passed to writeDOSync function !");
  }
  
  AsyncQueueWorker(new writeDOWorker(callback, module, output, value));
}

// Ssynchronous WriteDO
NAN_METHOD(writeDOSync) {
	// expect a number as the first argument
	int module = To<int>(info[0]).FromJust();
	int output = To<int>(info[1]).FromJust();
	int value = To<int>(info[2]).FromJust();
  
	unsigned char buf[10];
	int fd;
	
	if (info.Length() != 3)
	{
		return Nan::ThrowError("Incorect number of parameters passed to writeDO function !");
	}
	
	if ((module < 1) || (module > 6))
	{
		return Nan::ThrowRangeError("Module number out of range");
	}

	if ((output < 1) || (output > 4))
	{
		return Nan::ThrowRangeError("Output number out of range");
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
	buf[0] = (unsigned char)((module - 1) * 8 + (output - 1) * 2);
	buf[1] = (unsigned char)(value);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 2)) != 2) 
	{									
		return;
	}
	
	// Close Port
	close(fd);

	info.GetReturnValue().Set(0);
}
