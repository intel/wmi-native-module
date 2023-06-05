/*
 * **************************************************************************
 * Copyright 2023 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * **************************************************************************
 */

'use strict';

const wmi = require('../build/Release/wmi_native_module');

function printResult(result) {
    if (result) {
        let values = Object.values(result);
        values?.forEach(v => {
            console.log(v);
        });
    }
}

function rootWmiNamespace() {
    const properties = ['InstanceName'];
    const query = `SELECT ${properties.join(',')} FROM WmiMonitorId`;
    try {
        let result = wmi.query('root/wmi', query, properties);
        console.log(query);
        printResult(result);
    } catch (error) {
        console.error(error);
    }
}

function batteryCapacity() {
    const properties = ['RemainingCapacity'];
    const query = `SELECT ${properties.join(',')} FROM BatteryStatus`;
    try {
        let result = wmi.query('root/wmi', query, properties);
        console.log(query);
        printResult(result);
    } catch (error) {
        console.error(error);
        assert.fail();
    }
}

function rootCimv2() {
    const properties = ['CurrentHorizontalResolution', 'CurrentVerticalResolution'];
    const query = `SELECT ${properties.join(',')} FROM Win32_VideoController`
    try {
        let result = wmi.query('root/cimv2', query, properties);
        console.log(query);
        printResult(result);
    } catch (error) {
        console.error(error);
        assert.fail();
    }
}

function processorSelectAll() {
    const query = 'SELECT * FROM Win32_Processor';
    try {
        let result = wmi.query('root/cimv2', query);
        console.log(query);
        printResult(result);
    } catch (error) {
        console.error(error);
        assert.fail();
    }
}

function processorSpecifyProperties() {
    const properties = ['Caption', 'DeviceID', 'Manufacturer', 'MaxClockSpeed', 'Name', 'SocketDesignation'];
    const query = `SELECT ${properties.join(',')} FROM Win32_Processor`;
    try {
        let result = wmi.query('root/cimv2', query, properties);
        console.log(query);
        printResult(result);
    } catch (error) {
        console.error(error);
        assert.fail();
    }
}

function osProperties() {
    const properties = ['SystemDirectory', 'Organization', 'BuildNumber', 'RegisteredUser', 'SerialNumber', 'Version'];
    const query = `SELECT ${properties.join(',')} FROM Win32_OperatingSystem`;
    try {
        let result = wmi.query('root/cimv2', query, properties);
        console.log(query);
        printResult(result);
    } catch (error) {
        console.error(error);
        assert.fail();
    }
}

rootWmiNamespace();
batteryCapacity();
rootCimv2();
processorSelectAll();
processorSpecifyProperties();
osProperties();