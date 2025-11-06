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

#include "wmi_wrapper.h"

#include <comdef.h>
#include <propvarutil.h>
#include <Wbemidl.h>
#include <Windows.h>

#include <napi.h>

#include "namespaces.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "propsys.lib")

namespace wmi_wrapper
{

    std::string ConvertWstringToString(const std::wstring &wstring)
    {
        if (wstring.empty())
            return std::string();

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), NULL, 0, NULL, NULL);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), &str[0], size_needed, NULL, NULL);
        return str;
    }

    std::wstring ConvertStringToWstring(const std::string &string)
    {
        if (string.empty())
        {
            return std::wstring();
        }

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &string[0], (int)string.size(), NULL, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &string[0], (int)string.size(), &wstr[0], size_needed);
        return wstr;
    }

    std::wstring GetPropertyValue(
        const std::wstring &property,
        IWbemClassObject *class_object)
    {
        HRESULT hres;
        VARIANT variant;
        std::wstring value = L"";
        VariantInit(&variant);

        hres = class_object->Get(
            property.c_str(), // property name
            0,                // reserved, must be 0
            &variant,         // when successfull, this will hold the requested value
            NULL,             // Optional CIM type of the property
            NULL              // If specified receives information about the origin of the property
        );

        if (!FAILED(hres))
        {

            const UINT kMaxStrLength = 1024;
            wchar_t result[kMaxStrLength];
            VariantToString(variant, result, kMaxStrLength);
            value = std::wstring(result);
        }
        VariantClear(&variant);

        return value;
    }

    HRESULT GetPropertyValues(
        std::vector<std::wstring> properties,
        WmiQueryResult *results,
        IWbemClassObject *class_object)
    {
        HRESULT hres = ERROR_SUCCESS;
        for (size_t i = 0; i < properties.size(); ++i)
        {
            std::wstring value = GetPropertyValue(properties[i], class_object);
            (*results).push_back(make_pair(properties[i], std::wstring(std::move(value))));
        }
        return hres;
    }

    HRESULT GetAllPropertyValues(
        IWbemClassObject *class_object,
        WmiQueryResult *results)
    {
        HRESULT hres;
        SAFEARRAY *names_array;
        LONG start;
        LONG end;

        hres = class_object->GetNames(
            0,
            WBEM_FLAG_ALWAYS,
            0,
            &names_array);

        if (FAILED(hres))
        {
            return hres;
        }
        hres = SafeArrayGetLBound(names_array, 1, &start);
        if (FAILED(hres))
        {
            return hres;
        }
        hres = SafeArrayGetUBound(names_array, 1, &end);
        if (FAILED(hres))
        {
            return hres;
        }
        BSTR *names;
        hres = SafeArrayAccessData(names_array, (void HUGEP **)&names);
        for (int i = start; i <= end; ++i)
        {
            std::wstring value = GetPropertyValue(names[i], class_object);
            (*results).push_back(make_pair(names[i], std::wstring(std::move(value))));
        }
        hres = SafeArrayUnaccessData(names_array);
        return hres;
    }

    HRESULT GetAllValues(
        const std::wstring &query,
        std::vector<std::wstring> properties,
        std::vector<WmiQueryResult> *results,
        IWbemServices *service)
    {
        HRESULT hres;
        IEnumWbemClassObject *enumerator = NULL;

        hres = service->ExecQuery(
            bstr_t("WQL"),                                         // Query language, must be "WQL" for WMI
            bstr_t(query.c_str()),                                 // Query text
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, // These flags suggested for best performance
            NULL,                                                  // Typically NULL
            &enumerator                                            // Enumerator to get the instances in the results
        );

        if (FAILED(hres))
        {
            return hres; // Query failed
        }

        const u_int kMaxNumObjects = 10;
        IWbemClassObject *class_objects[kMaxNumObjects];
        ULONG num_objs_returned = 0;

        HRESULT enum_next_result = WBEM_S_NO_ERROR;

        while (WBEM_S_NO_ERROR == enum_next_result)
        {
            enum_next_result = enumerator->Next(
                WBEM_INFINITE,     // Timeout: maximum amount of time the call blocks before returning
                kMaxNumObjects,    // Number of requested IWbemClassObjects
                class_objects,     // Pointer to location with space to hold pointers to the number of objects specified
                &num_objs_returned // number of objects returned (can be less than number requested, but not NULL)
            );
            if (SUCCEEDED(enum_next_result) && num_objs_returned > 0)
            {
                for (ULONG i = 0; i < num_objs_returned; ++i)
                {
                    HRESULT object_result;
                    WmiQueryResult result;
                    if (properties.size() > 0)
                    {
                        object_result = GetPropertyValues(properties, &result, class_objects[i]);
                    }
                    else
                    {
                        object_result = GetAllPropertyValues(class_objects[i], &result);
                    }
                    results->push_back(std::move(result));

                    class_objects[i]->Release();
                }
            }
        }

        if (enumerator != NULL)
        {
            enumerator->Release();
        }

        return hres;
    }

    HRESULT Query(
        const char *wmi_namespace,
        WmiQueryParams query,
        std::vector<WmiQueryResult> *results)
    {

        HRESULT hres;

        // Initialize COM.
        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        {
            if (FAILED(hres) && hres == RPC_E_CHANGED_MODE)
            {
                // Was already initialized in a different mode, switch
                hres = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
            }
            if (FAILED(hres))
            {
                // Failed to initialize COM library
                return hres;
            }

            // Initialize security
            hres = CoInitializeSecurity(
                NULL,
                -1,                          // COM authentication
                NULL,                        // Authentication services
                NULL,                        // Reserved
                RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
                RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
                NULL,                        // Authentication info
                EOAC_NONE,                   // Additional capabilities
                NULL                         // Reserved
            );

            // RCP_E_TOO_LATE = CoInitializeSecurity has already been called by
            // the same process, in which case will continue to create instance.
            if (FAILED(hres) && hres != RPC_E_TOO_LATE)
            {
                // Failed to initialize security
                CoUninitialize();
                return hres;
            }

            // Obtain the initial locator to WMI
            IWbemLocator *locator = NULL;

            hres = CoCreateInstance(
                CLSID_WbemLocator,
                0,
                CLSCTX_INPROC_SERVER,
                IID_IWbemLocator,
                (LPVOID *)&locator);

            if (FAILED(hres))
            {
                // Failed to create IWbemLocator object.
                CoUninitialize();
                return hres;
            }

            // Connect to WMI through the IWbemLocator::ConnectServer method
            IWbemServices *service = NULL;

            // Connect to the namespace with the current user and obtain pointer
            // to make IWbemServices calls.
            hres = locator->ConnectServer(
                _bstr_t(wmi_namespace), // WMI namespace (root/wmi, root/cimv2, etc)
                NULL,                   // User name. NULL = current user
                NULL,                   // User password. NULL = current
                0,                      // Locale. NULL indicates current
                NULL,                   // Security flags.
                0,                      // Authority (for example, Kerberos)
                0,                      // Context object
                &service                // pointer to IWbemServices proxy
            );

            if (FAILED(hres))
            {
                // Could not connect.
                locator->Release();
                CoUninitialize();
            }

            // Set security levels on the proxy
            hres = CoSetProxyBlanket(
                service,                     // Indicates the proxy to set
                RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
                RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
                NULL,                        // Server principal name
                RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
                RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                NULL,                        // client identity
                EOAC_NONE                    // proxy capabilities
            );

            if (FAILED(hres))
            {
                // Could not set proxy blanket.
                service->Release();
                locator->Release();
                CoUninitialize();
            }

            // Use the IWbemServices pointer to make requests of WMI
            GetAllValues(query.first, query.second, results, service);

            // Cleanup
            service->Release();
            locator->Release();
        }

        CoUninitialize();

        // return status;
        return ERROR_SUCCESS;
    }

    WmiQueryParams wmi_wrapper::GetWstrParams(
        Napi::String query,
        Napi::Array properties,
        Napi::Env env)
    {
        WmiQueryParams wstr_params;
        std::wstring wstr_query = ConvertStringToWstring(query);

        std::vector<std::wstring> wstr_properties;

        for (uint32_t i = 0; i < properties.Length(); ++i)
        {
            Napi::Value param_value = properties[i];
            if (param_value.IsString())
            {
                std::string value = param_value.ToString().Utf8Value();
                std::wstring wstr_value = ConvertStringToWstring(value);
                wstr_properties.push_back(std::move(wstr_value));
            }
            else
            {
                Napi::Error::New(env, "Invalid Parameter").ThrowAsJavaScriptException();
                return wstr_params;
            }
        }

        wstr_params = make_pair(wstr_query, wstr_properties);
        return wstr_params;
    }

    Napi::Object ConvertResultsObject(
        std::vector<WmiQueryResult> results,
        Napi::Env env)
    {
        Napi::Object return_values = Napi::Object::New(env);

        size_t results_length = results.size();
        for (size_t i = 0; i < results_length; ++i)
        {
            Napi::Object return_obj = Napi::Object::New(env);
            for (size_t j = 0; j < results[i].size(); ++j)
            {
                std::wstring wst_key = results[i][j].first;
                std::wstring wst_value = results[i][j].second;

                std::string key = ConvertWstringToString(wst_key);
                std::string value = ConvertWstringToString(wst_value);

                return_obj.Set(key, Napi::String::New(env, value));
            }
            return_values.Set(i, return_obj);
        }
        return return_values;
    }

    Napi::Value WmiQuery(
        const Napi::CallbackInfo &info)
    {
        const int kNamespaceParam = 0;
        const int kQueryParam = 1;
        const int kPropertiesParam = 2; // optional

        const int kMinRequiredParamCount = 2;
        const int kMaxAllowedParams = 3;

        Napi::Env env = info.Env();
        if (info.Length() < kMinRequiredParamCount || info.Length() > kMaxAllowedParams)
        {
            // Too few or too many parameters passed
            Napi::Error::New(env, "Invalid Parameters").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[kNamespaceParam].IsString() || !info[kQueryParam].IsString())
        {
            Napi::Error::New(env, "Invalid Parameter").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::String wmi_namespace = info[kNamespaceParam].As<Napi::String>();
        if (!namespaces::IsSupportedNamespace(wmi_namespace))
        {
            Napi::Error::New(env, "Unsupported Namespace").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::String query = info[kQueryParam].As<Napi::String>();

        Napi::Array properties = Napi::Array::New(env);

        // Properties param is optional
        if (info.Length() == kMaxAllowedParams)
        {
            // If specific properties are requested, they must be passed as an array
            if (info[kPropertiesParam].IsArray())
            {
                properties = info[kPropertiesParam].As<Napi::Array>();
            }
            else
            {
                Napi::Error::New(env, "Invalid Parameter").ThrowAsJavaScriptException();
                return env.Null();
            }
        }

        WmiQueryParams wstr_params = GetWstrParams(query, properties, env);

        std::vector<WmiQueryResult> results;
        HRESULT hres = wmi_wrapper::Query(wmi_namespace.Utf8Value().c_str(), std::move(wstr_params), &results);
        if (FAILED(hres))
        {
            std::string hresStr = std::to_string(hres);
            Napi::Error::New(env, "Query failed with error code: " + hresStr).ThrowAsJavaScriptException();
        }

        return ConvertResultsObject(std::move(results), env);
    }

    Napi::Object Init(
        Napi::Env env,
        Napi::Object exports)
    {
        exports.Set("query", Napi::Function::New(env, wmi_wrapper::WmiQuery));
        return exports;
    }

}
