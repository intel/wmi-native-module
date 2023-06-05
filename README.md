# WMI Native Module
## Overview
The `wmi-native-module` is a native module for Node.js applications to query WMI on a local Windows system using native C++. 

## Install
`npm install @intelcorp/wmi-native-module`

## Usage
Example queries can be found in `tests\exampleQueries.js`

## Methods
`function query(namespace: string, query: string, properties?: string[]): object;` 

### Arguments
- `namespace`: Namespace of the class to query. Examples: `'root\wmi'` or `'root\cimv2'`
- `query`: WQL query string. Examples: `'SELECT * FROM Win32_Processor'` or `'SELECT Caption,DeviceID FROM Win32_Processor`
- `properties`: Optional parameter to limit the properties returned. Example: `query('root\wmi', 'SELECT * FROM Win32_Processor', ['Caption','DeviceID'])`

#### Namespace Whitelist
There is a whitelist for supported namespaces defined in `namespaces.h`. 
- To allow the module to query any namespace, the `IsSupportedNamespace()` method can be modified to always return true.
- To add additional namespaces to the whitelist, add the new namespace to the vector returned by `GetWhitelist()`. Namespaces should always be added to the whitelist as all lowercase values.

### Return Value
- Object containing the results found by the query. 
- If the query fails or does not return any results an empty object will be returned: `{}`

### Examples
```
const wmi = require('wmi_native_module');
let result = wmi.query('root/cimv2', 'SELECT * FROM Win32_Processor');
```
```
const wmi = require('wmi_native_module');
const properties = ['Caption', 'DeviceID', 'Manufacturer', 'MaxClockSpeed', 'Name', 'SocketDesignation'];
const query = `SELECT ${properties.join(',')} FROM Win32_Processor`;
let result = wmi.query('root/cimv2', query, properties);
```