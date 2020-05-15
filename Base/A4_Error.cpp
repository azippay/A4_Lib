/**
* \author Albert Zippay * 1995..2018
* \copyright Albert Zippay - 1995 - 2018
* \file A4_Error.cpp
*
* \brief  Binds a distinct Error_Code with a human readable description of the error that will help the developer debug the application.
*
* \note
*   The A4_Error is used throughout the A4_Lib (aka Albert Zippay Library of reusable objects)
* Copyright (c) 1995...2017, Albert Zippay
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

#ifdef A4_Lib_Windows
#include "Stdafx.h"
#pragma warning(disable: 4840) // non-portable use...Linux builds don't bitch about it...
#endif

#include "A4_Error.hh"
#include "A4_Utils.hh" // <-- warning - cyclical dependency

#include <cstdarg>

///////////////////////////////   implementation   ////////////////////////////
  
/**
 * \brief Default char constructor as of 2016-01
 * @param the_module_id - IN - 6-byte value identifying the Module
 * @param the_error_offset - IN - 2-byte value representing a distinct error condition
 * @param the_error_message - IN
 * @param the_error_detail_level - IN
 * @param the_log_detail_override - IN - example: if the Application logging level is  Warning, then setting this value to Info would allow an Info level log entry as an exception.
 */
A4_Error::A4_Error (Module_ID                 the_module_id, 
                    Error_Offset              the_error_offset,
                    std::string               the_error_message,  
                    A4_Lib::Logging::Detail   the_error_detail_level ,	
                    A4_Lib::Logging::Detail   the_log_detail_override) 
  { // begin
    this->error_code = this->Make_Error_Code(the_module_id, the_error_offset);
    this->error_message = the_error_message;
    this->error_detail_level = the_error_detail_level;
    this->log_detail_override = the_log_detail_override;    
  } // end default constructor

/**
 * \brief Default wchar_t constructor as of 2016-01
 * @param the_module_id - IN - 6-byte value identifying the Module
 * @param the_error_offset - IN - 2-byte value representing a distinct error condition
 * @param the_error_message - IN
 * @param the_error_detail_level - IN
 * @param the_log_detail_override - IN - example: if the Application logging level is  Warning, then setting this value to Info would allow an Info level log entry as an exception.
 */
A4_Error::A4_Error (Module_ID                 the_module_id, 
                    Error_Offset              the_error_offset,
                    std::wstring	      the_error_message,  
                    A4_Lib::Logging::Detail   the_error_detail_level ,	
                    A4_Lib::Logging::Detail   the_log_detail_override)
  { // begin
    this->error_code = this->Make_Error_Code(the_module_id, the_error_offset);  
    this->error_message.clear();
    (void) A4_Lib::WChar_to_Char(the_error_message, this->error_message); // <-- this breaks an A4 rule - no initialization in a constructor - but errors should still be logged

    this->error_detail_level = the_error_detail_level;
    this->log_detail_override = the_log_detail_override;    
  } // end default wchar_t constructor


/**
 * \brief Simple constructor with default initialization
 * @param the_error_code
 */
A4_Error::A4_Error(Error_Code the_error_code)
{ // begin
  this->error_code = the_error_code;
  this->error_detail_level = A4_Lib::Logging::Error;
  this->log_detail_override = A4_Lib::Logging::Off;
} // end constructor (2)


/**
 * \brief  std::string Variadic constructor
 * @param the_module_id - IN
 * @param the_error_offset - IN
 * @param the_error_detail_level - IN
 * @param the_error_msg_format - IN
 * @param ... - IN - must match the formated string or the error text will reflect new problem while the error code reamins.
 */
A4_Error::A4_Error (Module_ID                 the_module_id, // top 6-bytes  
                    Error_Offset              the_error_offset,// bottom 2-bytes...should be sufficient for human writers
                    A4_Lib::Logging::Detail   the_error_detail_level, // the detail level of the error (info, warning, error, etc
                    std::string               the_error_msg_format, // error message text and formatting
                    ...) // items to format in the error message
{ // begin
  char    the_buffer[A4_Lib::Max_Error_Message_Length];
  
  va_list the_va_list;
  va_start (the_va_list, the_error_msg_format);
  
  if (std::vsnprintf (the_buffer, A4_Lib::Max_Error_Message_Length, the_error_msg_format.c_str(), the_va_list) < 0)
    this->error_message = "Variadic error message was not formatted correctly";
  else this->error_message = the_buffer;
  
  this->error_code = this->Make_Error_Code(the_module_id, the_error_offset);

  this->error_detail_level = the_error_detail_level;  
  
  va_end (the_va_list);  
} // variadic char error constructor


/**
 * \brief  std::wstring Variadic constructor
 * @param the_module_id - IN
 * @param the_error_offset - IN
 * @param the_error_detail_level - IN
 * @param the_error_msg_format - IN
 * @param ... - IN - must match the formated string or the error text will reflect new problem while the error code remains.
 */
A4_Error::A4_Error (Module_ID                 the_module_id, // top 6-bytes  
                    Error_Offset              the_error_offset,// bottom 2-bytes...should be sufficient for human writers
                    A4_Lib::Logging::Detail   the_error_detail_level, // the detail level of the error (info, warning, error, etc
                    std::wstring              the_error_msg_format, // error message text and formatting
                                              ...) // items to format in the error message
{ // begin
  wchar_t    the_buffer [A4_Lib::Max_Error_Message_Length];
  char       *the_char_buf = NULL;

  memset (the_buffer, 0, sizeof (the_buffer));
  
  va_list the_va_list;
  va_start (the_va_list, the_error_msg_format);
  
  if (std::vswprintf (the_buffer, A4_Lib::Max_Error_Message_Length, the_error_msg_format.c_str(), the_va_list) < 0)
    this->error_message = "Variadic error message was not formatted correctly";
  else { // begin
    (void) A4_Lib::WChar_to_Char(the_buffer, std::wcslen(the_buffer), (char *&) the_char_buf);
  
    if (the_char_buf != NULL)
    {
      this->error_message = the_char_buf;
      delete[] the_char_buf;
    } // if then
    else this->error_message = "A4_Lib::WChar_to_Char did not return a char string.";
  } // if else
  
  this->error_code = this->Make_Error_Code(the_module_id, the_error_offset);

  this->error_detail_level = the_error_detail_level;  
  
  va_end (the_va_list);  
} // variadic wchar_t error constructor


/**
 * \brief Default destructor
 */
A4_Error::~A4_Error(void)
{ // begin
  this->error_message.clear();
  this->error_code = No_Error;
  this->error_detail_level = A4_Lib::Logging::Off;
  this->log_detail_override = A4_Lib::Logging::Off;
} // end destructor


/**
 * \brief Create a single "dot" error code value from the Module_ID and Error_Offset
 * @param the_module_id - IN
 * @param the_error_offset - IN
 * @return An error code in the form: Module_ID.Error_Offset - more readable than a really large integer value
 */
Dot_Error_Code A4_Error::Make_Dot_Code (Module_ID      the_module_id,
                                        Error_Offset   the_error_offset)
{ // begin
  return static_cast <Dot_Error_Code>(the_module_id) + (static_cast <Dot_Error_Code>(the_error_offset) / 100000.0);
} // end Make_Dot_Code

/**
 * \brief Create a single Error_Code value from the Module_ID and Error_Offset
 * @param the_module_id - IN
 * @param the_error_offset - IN
 * @return An Error_Code containing the Module_ID and Error_Offset
 */
Error_Code  A4_Error::Make_Error_Code (Module_ID    the_module_id,
                                       Error_Offset the_error_offset)
{ // begin
  return (the_module_id << 16) + the_error_offset;
} // Make_Error_Code

/**
 * \brief Retrieve the error text.
 */
const char  * A4_Error::Get_Error_Message (void) noexcept
{ // begin
  return this->error_message.c_str();
} // end Get_Error_Message



/**
 * \brief Retrieve the 64-bit error code value.
 * @return 
 */
Error_Code  A4_Error::Get_Error_Code (void) const
{
  return this->error_code;
} // end Get_Error_Code


/// Retrieve the error offset
//
Error_Code  A4_Error::Get_Error_Offset (void)
{ // begin
return this->error_code & 0x000000000000FF;
} // Get_Error_Offset

/**
 * \
 * @return the error code in the form mm.eeeee where mm == the module-specific number, eeeee is the specific error within the module
 */
Dot_Error_Code  A4_Error::Get_Dot_Error_Code (void) const
{
  return static_cast <Dot_Error_Code>(this->error_code >> 16) + (static_cast <Dot_Error_Code>(this->error_code & 0x000000000000FFFF) / 100000);
} // end Get_Dot_Error_Code


/// @brief  static version - convert an interger 
Dot_Error_Code  A4_Error::Get_Dot_Error_Code(Error_Code  the_error_code)
{
  return static_cast <Dot_Error_Code>(the_error_code >> 16) + (static_cast <Dot_Error_Code>(the_error_code & 0x000000000000FFFF) / 100000);
} // Get_Dot_Error_Code (static)


/**
 * \brief Retrieve the current error detail level.
 * @return 
 */
A4_Lib::Logging::Detail	A4_Error::Get_Error_Detail_Level (void) const
{ // begin
  return this->error_detail_level;
} // end Get_Error_Detail_Level


A4_Lib::Logging::Detail	A4_Error::Get_Logging_Detail_Level (void) const
{ // begin
  return this->log_detail_override;
} // end Get_Logging_Detail_Level

/**
 * \brief A Set method when an operator won't do.
 * @param the_error_code
 */
void A4_Error::Set (Error_Code  the_error_code)
{ // begin
  this->error_code = the_error_code;
} // end Set


/**
 * \brief Error_Code Assignment operator
 */
A4_Error &  A4_Error::operator = (const Error_Code  the_error_code) // <-- use this for new development
{
  this->error_code = the_error_code;
  this->error_message = "Error from called method";
  
  return *this;  
} // end assignment operator

/**
 * \brief (deprecated) instance copy operator
 * @param the_instance
 * @return 
 */
A4_Error &  A4_Error::operator = (const A4_Error &the_instance)
{ // BEGIN
  this->error_code = the_instance.error_code;
  this->error_detail_level = the_instance.error_detail_level;
  this->error_message = the_instance.error_message;
  this->log_detail_override = the_instance.log_detail_override;
  return *this;
} // end operator = (3)


bool  A4_Error::operator == (Error_Code  the_error_code)
{ // begin
  return (this->error_code == the_error_code);
}// end operator ==


bool  A4_Error::operator != (Error_Code  the_error_code)
{ // BEGIN
  return (this->error_code != the_error_code);
} // end operator !=