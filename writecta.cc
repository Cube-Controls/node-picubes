/*********************************************************************
 * PiCubes - Native libarry for Pi-Cubes Modules
 *
 * Copyright (c) 2015 Cube-Controls Inc.
 *
 * MIT License 
 ********************************************************************/

#include <nan.h>
#include "modbus.h"   // NOLINT(build/include)
#include "writecta.h"  // NOLINT(build/include)

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

class writeCTAWorker : public AsyncWorker {
 public:
  writeCTAWorker(Callback *callback, int slaveaddr, int regaddr, int value )
    : AsyncWorker(callback), slaveaddr(slaveaddr), regaddr(regaddr), value(value) {}
  ~writeCTAWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute () {
	
    // Open port for reading and writing
    if (openModbus() < 0) 
    {				
		SetErrorMessage("Failed to open modbus serial port.");
		return;  
    }
	
	// read value from modbus
	if (writeHoldingRegister(slaveaddr, regaddr, value, 1000) < 0)  
    {				
		SetErrorMessage("Failed to write register to modbus device !");
		return;  
    }
	
	// Close Port
	closeModbus();
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
  int slaveaddr;
  int regaddr;
  int value;
};

// Asynchronous writeCTA
NAN_METHOD(writeCTA) {
  int slaveaddr = To<int>(info[0]).FromJust();
  int regaddr = To<int>(info[1]).FromJust();
  int value = To<int>(info[2]).FromJust();
  Callback *callback = new Callback(info[3].As<Function>());
  
  if (info.Length() != 4)
  {
    return Nan::ThrowError("Incorect number of parameters passed to writeCTA function !");
  }
  
  AsyncQueueWorker(new writeCTAWorker(callback, slaveaddr, regaddr, value));
}

// Synchronous writeCTA
NAN_METHOD(writeCTASync) {
	int slaveaddr = To<int>(info[0]).FromJust();
	int regaddr = To<int>(info[1]).FromJust();
	int value = To<int>(info[2]).FromJust();
  
	if (info.Length() != 3)
	{
		return Nan::ThrowError("Incorect number of parameters passed to writeCTASync function !");
	}
  
    // Open port for reading and writing
    if (openModbus() < 0) 
    {				
		return;
    }
	
	// read value from modbus
	if (writeHoldingRegister(slaveaddr, regaddr, value, 1000) < 0)  
    {				
		return;
	}
	// Close Port
	closeModbus();  
	
	info.GetReturnValue().Set(0);
  
}
