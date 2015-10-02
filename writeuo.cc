/*********************************************************************
 * PiCubes - Native libarry for Pi-Cubes Modules
 *
 * Copyright (c) 2015 Cube-Controls Inc.
 *
 * MIT License 
 ********************************************************************/

#include <nan.h>
#include "writeuo.h"  // NOLINT(build/include)
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

class writeUOWorker : public AsyncWorker {
 public:
  writeUOWorker(Callback *callback, int module, int output, int typeuo, int value)
    : AsyncWorker(callback), module(module), output(output), typeuo(typeuo), value(value) {}
  ~writeUOWorker() {}

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
	buf[1] = (unsigned char)(typeuo);
	buf[2] = (unsigned char)(value);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 3)) != 3) 
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
  int typeuo;
  int value;
};

// Asynchronous WriteUO
NAN_METHOD(writeUO) {
  int module = To<int>(info[0]).FromJust();
  int output = To<int>(info[1]).FromJust();
  int typeuo = To<int>(info[2]).FromJust();
  int value = To<int>(info[3]).FromJust();
  Callback *callback = new Callback(info[4].As<Function>());
  
  if (info.Length() != 5)
  {
    return Nan::ThrowError("Incorect number of parameters passed to writeUO function !");
  }
  
  AsyncQueueWorker(new writeUOWorker(callback, module, output, typeuo, value));
}


// Synchronous WriteUO
NAN_METHOD(writeUOSync) {
	int module = To<int>(info[0]).FromJust();
	int output = To<int>(info[1]).FromJust();
	int typeuo = To<int>(info[2]).FromJust();
	int value = To<int>(info[3]).FromJust();
  
	if (info.Length() != 4)
	{
		return Nan::ThrowError("Incorect number of parameters passed to writeUOSync function !");
	}
  
    unsigned char buf[10];
    int fd;
	
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
	buf[1] = (unsigned char)(typeuo);
	buf[2] = (unsigned char)(value);
	
	// Send address and value to I/O module
	if ((write(fd, buf, 3)) != 3) 
	{									
		return;
	}
	
	// Close Port
	close(fd);  
	
	info.GetReturnValue().Set(0);

}