#ifndef __A4_Base_Types_Defined__
#define __A4_Base_Types_Defined__

/**
* \brief  Define the base data types and functions of the A4_Lib framework
 * \author a. zippay - 1995-2020
\note  
*   These Base Library methods are used throughout the A4_Lib 
* Copyright (c) 1995...2020, Albert Zippay
 * The MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <stdint.h>
#include <cstddef>
#include <sstream>
#include <ctime>
#include <cstring>
#include <cwchar>
#include <vector>
#include <chrono>

#ifndef A4_DotNet
#include <thread>
#include <atomic>
#endif // A4_DotNet

#ifdef A4_Lib_Windows
#define A4_Unused_Arg(x) x
#define A4_TZ_Set _tzset
typedef std::int64_t  ssize_t;
#define A4_SetLocale setlocale
#define A4_mbstowcs mbstowcs
#define A4_isspace  isspace

#else // not windows
#define A4_Unused_Arg(x)(void)(x)
#define A4_TZ_Set tzset
#define A4_SetLocale std::setlocale
#define A4_mbstowcs std::mbstowcs
#define A4_isspace(x)  std::isspace<char>(x, std::locale::classic())

  #ifdef A4_Use_Wide_String
    #error  "A4_Use_Wide_String should only be used for Windows builds. "
  #endif
#endif  

typedef std::uint64_t  Error_Code; // top 6-bytes are reserved for Module, lower 2-bytes for the model-specific error offset. This definition will need changing if > 64K errors/module are needed.
typedef std::uint64_t  Module_ID;
typedef std::uint16_t  Error_Offset;
typedef std::uint16_t  Method_State;

static const Error_Code   No_Error = 0;  

typedef double Dot_Error_Code;
static const Dot_Error_Code   Dot_No_Error = 0.0; 

#define A4_Error_Code(module_id,error_value)((static_cast<Error_Code>(module_id) << 16) + static_cast<Error_Code>(error_value))

typedef unsigned int  A4_Key; // unsigned

const A4_Key	Invalid_A4_Key = 0;

#define SQL_C_A4_Key  SQL_C_LONG
#define SQL_A4_Key  SQL_C_LONG

#define SQL_C_A4_Big_Key  SQL_C_SBIGINT
#define SQL_A4_Big_Key  SQL_C_SBIGINT

#define Make_A4_Big_Key(az_key_1,az_key_2)((((A4_Big_Key)az_key_1) << 32) + (A4_Big_Key) az_key_2)


#ifdef __cplusplus
#include <string>
  #ifdef A4_Use_Wide_String
  typedef std::wstring   A4_String;
  typedef wchar_t A4_Char;
  #define A4_TEXT(s) L##s
  #define String_Compare wcscmp
  #define String_Copy wcscpy
  #else 
  typedef std::string   A4_String;
  typedef char A4_Char;
  #define A4_TEXT(s) s
  #define String_Compare strcmp
  #define String_Copy strcpy
  #endif // A4_Use_Wide_String
#endif // A4_Use_Wide_String

typedef std::basic_stringstream<A4_Char>   A4_String_Stream; 

// windows crap
#ifdef A4_DLL_EXPORTS
#define A4_Export __declspec (dllexport)
#elif A4_DLL_IMPORTS
#define A4_Export __declspec (dllimport)
#else
#define A4_Export
#endif
 
#ifdef A4_Lib_Windows
  #define This_Function_Name	__FUNCTION__
#else
  #define This_Function_Name __PRETTY_FUNCTION__
#endif

namespace A4_Lib
{ // begin
  typedef std::vector<std::string>  String_Vector;
  typedef std::vector<std::wstring> WString_Vector;

  typedef std::vector<std::uint8_t>  Byte_Vector;

  typedef std::chrono::time_point<std::chrono::system_clock>	Time_Point;
  typedef std::uint8_t    MB_Command;
  
  static const double Near_Zero = 0.000000001;

  typedef std::uint64_t Class_Instance_ID;

  static const Class_Instance_ID Invalid_Class_Instance_ID = 0;
  static const Class_Instance_ID Class_Instance_ID_Reset = static_cast<Class_Instance_ID>(-1);

  typedef std::uint64_t   Process_ID; // cross platform definition...should be large enough for a while
  static const Process_ID   Invalid_Process_ID = 0;

  typedef std::uint64_t   Protocol_ID;
  static const Protocol_ID  Invalid_Protocol_ID = 0;

  typedef std::uint16_t   Protocol_State;
  static const Protocol_State Invalid_Protocol_State = 0;
  const std::size_t     Max_Error_Message_Length = 2048;

  static const std::time_t One_Day_In_Seconds = 86400;
  typedef std::vector <std::uint8_t *>	  Byte_Ptr_Vector; /**< @todo see about changing this into a std::unique_ptr */
  
  namespace Logging
  { // begin
  // deliberately not an enumeration so that dotnet can use it as well
    typedef std::uint8_t  Detail; // moved from A4_Log.hh - removed cycle include with A4_Error
    static const Detail Private_Comment  = 0;  //- reserved for log headers / trailers
    static const Detail Off              = 1;  // nothing is logged    Default_Message
    static const Detail Error            = 2;  // log errors
    static const Detail Warning          = 3;  // + Errors
    static const Detail Info             = 4;  // + Warnings + Errors
    static const Detail Module_Specific  = 5;  // + Info + Warnings + Errors + module specific information
    static const Detail Debug            = 6;  // detailed programming sequence
    static const Detail Dump_Start       = 7;  // put a starting label on  the dump
    static const Detail Dump_End         = 8; // put an ending label on the dump
    static const Detail Content_Dump     = 9;  // details on what the program is processing... probably a nicer way of saying Barf
  } // namespace Logging

  #ifndef A4_DotNet
  typedef std::atomic<std::time_t>  atomic_time_t;
  #endif // A4_DotNet
} // A4_Lib
#endif // __A4_Base_Types_Defined__

