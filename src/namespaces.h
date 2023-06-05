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

#include <vector>
#include <string>

namespace namespaces
{
    std::vector<std::string> GetWhitelist()
    {
        std::vector<std::string> whitelist_lowercase{
            "root/cimv2",
            "root/cimv2/power",
            "root/wmi",
            "root/microsoft/windows/storage"};

        return whitelist_lowercase;
    }

    bool IsSupportedNamespace(Napi::String wmi_namespace)
    {
        std::string wmi_namespace_lowercase = wmi_namespace;
        std::transform(
            wmi_namespace_lowercase.begin(),
            wmi_namespace_lowercase.end(),
            wmi_namespace_lowercase.begin(),
            [](unsigned char c)
            { return std::tolower(c); });

        std::vector<std::string> whitelist = namespaces::GetWhitelist();

        return std::any_of(
            whitelist.begin(),
            whitelist.end(),
            [wmi_namespace_lowercase](const std::string &str)
            {
                return str == wmi_namespace_lowercase;
            });
    }
}