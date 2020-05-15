#ifndef __A4_Error_Defined__
#define __A4_Error_Defined__
/**
* \author Albert Zippay * 1995..2020
* \copyright Albert Zippay - 1995 - 2020
* \file A4_Error.hh
*
* \brief  Binds a distinct Error_Code with a human readable description of the error that will help the developer debug the application.
*
* \note
*   The A4_Error is used throughout the A4_Lib (aka Albert Zippay Library of reusable objects)
* Copyright (c) 1995...2019, Albert Zippay
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
 * 
 * \note This iteration now uses a 64-bit unsigned integer. 6-bytes are reserved for a Module base and 2-bytes are used for the module-specific error condition. 
 * 
*/

#include "A4_Lib_Base.hh"

typedef class A4_Error
{ // begin
public: // construction
// char version
  A4_Export A4_Error (Module_ID                   the_module_id, // top 6-bytes  
                      Error_Offset                the_error_offset,// bottom 2-bytes...should be sufficient for human writers
	              std::string		  the_error_message = "Problem returned from called method",  // users should pass something useful to his parameter
	              A4_Lib::Logging::Detail	  the_error_detail_level = A4_Lib::Logging::Error, // the detail level of the error (info, warning, error, etc
	              A4_Lib::Logging::Detail	  the_log_detail_override = A4_Lib::Logging::Off); // override the setting of the file logger
  
  // wchar_t version
  A4_Export A4_Error (Module_ID                   the_module_id, // top 6-bytes  
                      Error_Offset                the_error_offset,// bottom 2-bytes...should be sufficient for human writers
	              std::wstring	          the_error_message = L"Error from called method",  // users should pass something useful to his parameter ** do not use A4_TEXT
	              A4_Lib::Logging::Detail	  the_error_detail_level = A4_Lib::Logging::Error, // the detail level of the error (info, warning, error, etc
	              A4_Lib::Logging::Detail	  the_log_detail_override = A4_Lib::Logging::Off); // override the setting of the file logger
  
  A4_Export A4_Error(Error_Code the_error_code = No_Error);
  
// variadic
  A4_Export A4_Error (Module_ID                 the_module_id, // top 6-bytes  
                      Error_Offset              the_error_offset,// bottom 2-bytes...should be sufficient for human writers
	              A4_Lib::Logging::Detail   the_error_detail_level, // the detail level of the error (info, warning, error, etc
                      std::string               the_error_msg_format, // error message text and formatting
                      ...); // items to format in the error message
 
  A4_Export A4_Error (Module_ID                 the_module_id, // top 6-bytes  
                      Error_Offset              the_error_offset,// bottom 2-bytes...should be sufficient for human writers
	              A4_Lib::Logging::Detail   the_error_detail_level, // the detail level of the error (info, warning, error, etc
                      std::wstring              the_error_msg_format, // error message text and formatting
                      ...); // items to format in the error message  

  A4_Export virtual ~A4_Error(void);

public: // types
  typedef double  Dot_Error_Code;
  
public: // methods
  A4_Export const char *  Get_Error_Message (void) noexcept;
  
  A4_Export Error_Code  Get_Error_Code (void) const; // <-- use this for new development
  A4_Export Error_Code  Get_Error_Offset (void); // <-- use this when you want a module-specific error offset

  A4_Export Dot_Error_Code  Get_Dot_Error_Code (void) const;
  A4_Export static Dot_Error_Code  Get_Dot_Error_Code (Error_Code  the_error_code);

  A4_Export A4_Lib::Logging::Detail	Get_Error_Detail_Level (void) const;

  A4_Export A4_Lib::Logging::Detail	Get_Logging_Detail_Level (void) const;

  A4_Export void Set (Error_Code  the_error_code);
  
  A4_Export static Dot_Error_Code Make_Dot_Code (Module_ID    the_module_id,
                                       Error_Offset the_error_offset);

 A4_Export  static Error_Code  Make_Error_Code (Module_ID    the_module_id,
                                      Error_Offset the_error_offset);

public: // operators
  A4_Export A4_Error &  operator = (const Error_Code  the_error_code); 
  
  A4_Export A4_Error &  operator = (const A4_Error &the_instance);

  A4_Export bool  operator == (Error_Code  the_error_code); // <-- use this for new development
  
  A4_Export bool  operator != (Error_Code  the_error_code);// <-- use this for new development

private: // data
  Error_Code            error_code;
  
  std::string		error_message;

  A4_Lib::Logging::Detail	error_detail_level;
  A4_Lib::Logging::Detail	log_detail_override;
}A4_Error;

#endif // __A4_Error_Defined__
