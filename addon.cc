/*********************************************************************
 * PiCubes - Native libarry for Pi-Cubes Modules
 *
 * Copyright (c) 2015 Cube-Controls Inc.
 *
 * MIT License 
 ********************************************************************/

#include <nan.h>
#include "writedo.h"   // NOLINT(build/include)
#include "writeuo.h"   // NOLINT(build/include)
#include "readui.h"    // NOLINT(build/include)
#include "readcta.h"   // NOLINT(build/include)
#include "writecta.h"  // NOLINT(build/include)

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

// Expose synchronous and asynchronous access to our
// Estimate() function
NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("writeDO").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(writeDO)).ToLocalChecked());

  Set(target, New<String>("writeDOSync").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(writeDOSync)).ToLocalChecked());
	
  Set(target, New<String>("writeUO").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(writeUO)).ToLocalChecked());
	
  Set(target, New<String>("writeUOSync").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(writeUOSync)).ToLocalChecked());	
	
  Set(target, New<String>("readUI").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(readUI)).ToLocalChecked());	
	
  Set(target, New<String>("readUISync").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(readUISync)).ToLocalChecked());	
	
  Set(target, New<String>("readCTA").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(readCTA)).ToLocalChecked());	

  Set(target, New<String>("readCTASync").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(readCTASync)).ToLocalChecked());	
	
  Set(target, New<String>("writeCTA").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(writeCTA)).ToLocalChecked());		
	
  Set(target, New<String>("writeCTASync").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(writeCTASync)).ToLocalChecked());		
}



NODE_MODULE(addon, InitAll)
