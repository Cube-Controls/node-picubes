"use strict"

var addon = require('./build/Release/addon');

module.exports.writeDO = function(module_no, output_no, output_value, callback)
{
	return addon.writeDO(module_no, output_no, output_value, callback);
}

module.exports.writeDOSync = function(module_no, output_no, output_value)
{
	return addon.writeDOSync(module_no, output_no, output_value);
}

module.exports.writeUO = function(module_no, output_no, output_type, output_value, callback)
{
    return addon.writeUO(module_no, output_no, output_type, output_value, callback);
}

module.exports.writeUOSync = function(module_no, output_no, output_type, output_value)
{
    return addon.writeUOSync(module_no, output_no, output_type, output_value);
}

module.exports.readUI = function(module_no, input_no, input_type, callback)
{
	return addon.readUI(module_no, input_no, input_type, callback)
}

module.exports.readUISync = function(module_no, input_no, input_type)
{
	return addon.readUISync(module_no, input_no, input_type)
}

module.exports.readCTA = function(slave_addr, reg_addr, callback)
{
	return addon.readCTA(slave_addr, reg_addr, callback)
}

module.exports.readCTASync = function(slave_addr, reg_addr)
{
	return addon.readCTASync(slave_addr, reg_addr)
}

module.exports.writeCTA = function(slave_addr, reg_addr, value, callback)
{
	return addon.writeCTA(slave_addr, reg_addr, value, callback)
}

module.exports.writeCTASync = function(slave_addr, reg_addr, value)
{
	return addon.writeCTASync(slave_addr, reg_addr, value)
}


