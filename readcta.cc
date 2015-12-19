/*********************************************************************
 * PiCubes - Native libarry for Pi-Cubes Modules
 *
 * Copyright (c) 2015 Cube-Controls Inc.
 *
 * MIT License 
 ********************************************************************/

#include <nan.h>
#include "modbus.h"   // NOLINT(build/include)
#include "readcta.h"  // NOLINT(build/include)

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

class readCTAWorker : public AsyncWorker {
 public:
  readCTAWorker(Callback *callback, int slaveaddr, int regaddr )
    : AsyncWorker(callback), slaveaddr(slaveaddr), regaddr(regaddr) {}
  ~readCTAWorker() {}

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
	if (readHoldingRegister(slaveaddr, regaddr, &value, 2000) < 0)  
    {				
		SetErrorMessage("Failed to read register from modbus device !");
		// Close Port
		closeModbus();
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
        Null(),
		New<Number>(value)
    };

    callback->Call(2, argv);
  }

 private:
  int slaveaddr;
  int regaddr;
  unsigned int value;
};

// Asynchronous readCTA
NAN_METHOD(readCTA) {
  int slaveaddr = To<int>(info[0]).FromJust();
  int regaddr = To<int>(info[1]).FromJust();
  Callback *callback = new Callback(info[2].As<Function>());
  
  if (info.Length() != 3)
  {
    return Nan::ThrowError("Incorect number of parameters passed to readCTA function !");
  }
  
  AsyncQueueWorker(new readCTAWorker(callback, slaveaddr, regaddr));
}


// Synchronous readCTA
NAN_METHOD(readCTASync) {
	int slaveaddr = To<int>(info[0]).FromJust();
	int regaddr = To<int>(info[1]).FromJust();
	unsigned int value;
  
	if (info.Length() != 2)
	{
		return Nan::ThrowError("Incorect number of parameters passed to readCTA function !");
	}
	
    // Open port for reading and writing
    if (openModbus() < 0) 
    {				
		return;
    }
	
	// read value from modbus
	if (readHoldingRegister(slaveaddr, regaddr, &value, 2000) < 0)  
    {				
		// Close Port
		closeModbus();	
		return;
    }
	
	info.GetReturnValue().Set(value);
	
	// Close Port
	closeModbus();	

}
