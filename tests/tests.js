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

const assert = require("assert");

const wmi = require('../build/Release/wmi_native_module');

function selectAllWmiTest() {
    try {
        let result = wmi.query('root/cimv2', 'SELECT * FROM Win32_Processor');
        let keys = Object.keys(result);
        if (keys.length === 0) {
            assert.fail();
        }
        console.log('Win32_Processor: ' + result);
    } catch (error) {
        console.error(error);
        assert.fail();
    }
}

function badInputWmiTests_Exceptions() {
    let goodnamespace = 'root/cimv2';
    let badnamespace_wrongType = 123;
    let badnamespace_string = 'invalid';

    let goodQuery = 'SELECT * FROM Win32_Processor';
    let badQuery_wrongType = 123;

    let goodValues = ['DeviceID', 'Caption'];
    let badValues_wrongTypeArray = [123];
    let badValues_wrongType = 123;

    assert.doesNotThrow(() => wmi.query(goodnamespace, goodQuery, goodValues));
    assert.throws(() => wmi.query(badnamespace_wrongType, goodQuery, goodValues), Error);
    assert.throws(() => wmi.query(goodnamespace, badQuery_wrongType, goodValues), Error);
    assert.throws(() => wmi.query(goodnamespace, goodQuery, badValues_wrongTypeArray), Error);
    assert.throws(() => wmi.query(goodnamespace, goodQuery, badValues_wrongType), Error);

    assert.throws(() => wmi.query(badnamespace_string, goodQuery, goodValues), Error);
    console.log("badInputWmiTests_Exceptions() complete, all functions threw exceptions as expected. ");
}

function badInputWmiTests_NoExceptions() {
    let goodnamespaceLower = 'root/cimv2';
    let goodnamespaceUpper = 'ROOT/CIMV2';
    let goodnamespaceMixed = 'Root/Cimv2';

    let goodQuery = 'SELECT * FROM Win32_Processor';
    let badQuery_string = 'invalid';

    let goodValues = ['DeviceID', 'Caption'];
    let badValues_str = ['invalid'];

    let result = wmi.query(goodnamespaceLower, goodQuery, goodValues);
    console.log("good namespace (" + goodnamespaceLower + "): ");
    console.log(result);

    result = wmi.query(goodnamespaceMixed, goodQuery, goodValues);
    console.log("good namespace (" + goodnamespaceMixed + "): ");
    console.log(result);

    result = wmi.query(goodnamespaceUpper, goodQuery, goodValues);
    console.log("good namespace (" + goodnamespaceUpper + "): ");
    console.log(result);

    result = wmi.query(goodnamespaceLower, goodQuery, goodValues);
    console.log("good values: ");
    console.log(result);

    result = wmi.query(goodnamespaceLower, badQuery_string, goodValues);
    console.log("bad query: ");
    console.log(result);

    result = wmi.query(goodnamespaceLower, goodQuery, badValues_str);
    console.log("bad values: ");
    console.log(result);
}

selectAllWmiTest();
badInputWmiTests_Exceptions();
badInputWmiTests_NoExceptions();