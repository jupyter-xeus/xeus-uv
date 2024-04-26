/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_UV_EXPORT_HPP
#define XEUS_UV_EXPORT_HPP

#ifdef _WIN32
    #ifdef XEUS_UV_STATIC_LIB
        #define XEUS_UV_API
    #else
        #ifdef XEUS_UV_EXPORTS
            #define XEUS_UV_API __declspec(dllexport)
        #else
            #define XEUS_UV_API __declspec(dllimport)
        #endif
    #endif
#else
    #define XEUS_UV_API __attribute__((visibility("default")))
#endif

// Project version
#define XEUS_UV_VERSION_MAJOR 0
#define XEUS_UV_VERSION_MINOR 1
#define XEUS_UV_VERSION_PATCH 0

// Binary version
#define XEUS_UV_BINARY_CURRENT 0
#define XEUS_UV_BINARY_REVISION 1
#define XEUS_UV_BINARY_AGE 0

#endif

