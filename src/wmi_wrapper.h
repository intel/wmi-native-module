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

#pragma once

#include <napi.h>
#include <Windows.h>
#include <Wbemidl.h>

namespace wmi_wrapper
{

    typedef std::vector<std::pair<std::wstring, std::wstring>> WmiQueryResult;
    typedef std::pair<std::wstring, std::vector<std::wstring>> WmiQueryParams;

    std::wstring GetPropertyValue(const std::wstring &property, IWbemClassObject *class_object);
    HRESULT GetAllValues(const std::wstring &query, std::vector<std::wstring> properties, std::vector<WmiQueryResult> *results, IWbemServices *service);
    HRESULT GetPropertyValues(std::vector<std::wstring> properties, WmiQueryResult *results, IWbemClassObject *class_object);
    HRESULT GetAllPropertyValues(IWbemClassObject *class_object, WmiQueryResult *results);
    HRESULT Query(const char *wmi_namespace, WmiQueryParams query, std::vector<WmiQueryResult> *results);

    WmiQueryParams GetWstrParams(Napi::String query, Napi::Array keys, Napi::Env env);
    Napi::Object ConvertResultsObject(std::vector<WmiQueryResult> results, Napi::Env env);

    /**
     * Queries WMI on the local system and returns an object with the requested values
     *
     * @param info[0] String containing the Namespace (example: 'root\wmi' or 'root\cimv2')
     * @param info[1] String containing the WQL query (example: "SELECT * FROM Win32_OperatingSystem")
     * @param info[2] Optional: Array of strings containing the desired data (example: ['Version','BuildNumber']).
     *                If no value is passed, all properties will be returned from the object.
     * @return An object containing objects with the requested data as strings (example: {'0': {'Version': '10.0.19044', 'BuildNumber': '19044'}})
     */
    Napi::Value WmiQuery(const Napi::CallbackInfo &info);

    Napi::Object Init(Napi::Env env, Napi::Object exports);

};