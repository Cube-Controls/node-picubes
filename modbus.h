/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2015 NAN contributors
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 ********************************************************************/

#ifndef MODBUS_H
#define MODBUS_H

#include <nan.h>

int openModbus(void);
int writeHoldingRegister(int SlaveAddress, int RegisterAddress, unsigned int Value, int TimeOut);
int readHoldingRegister(int SlaveAddress, int RegisterAddress, unsigned int *Value, int TimeOut);
void closeModbus(void);



#endif  // MODBUS_H
