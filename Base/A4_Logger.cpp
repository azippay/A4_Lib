/**
* \brief  Logging base class implementation.
* \note   The idea here is to allow multiple forms (File, ELK, etc) of logging via one common virtual interface.
* \author Albert Zippay * 1995..2020
 * \file A4_Logger.cpp
* \copyright Albert Zippay - 1995 - 2020
 * \note  * The MIT License
 *
 * Copyright 2018 a. zippay
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
#endif 

#include "A4_Logger.hh"
#include "A4_Method_State_Block.hh" // <-- cyclical dependency

namespace A4_Lib_Logger_Singleton
{
  A4_Lib::Logger::Pointer       logging_instance_pointer;
} // A4_Lib_Logger_Singleton

/**
 * \brief Default constructor
 */
A4_Lib::Logger::Logger(void)
{ // begin
  this->is_open = false;  
  this->log_detail_level = A4_Lib::Logging::Debug;
} // constructor

/**
 * \brief Default destructor
 */
A4_Lib::Logger::~Logger(void)
{ // begin
  if (this->Is_Open() == true)
    (void) this->Close();
  
  if (A4_Lib_Logger_Singleton::logging_instance_pointer != nullptr)
    A4_Lib_Logger_Singleton::logging_instance_pointer.reset();
  
  this->log_detail_level = A4_Lib::Logging::Off;
} // destructor

/**
 * \brief retrieve the log_detail_level member.
 */
A4_Lib::Logging::Detail  A4_Lib::Logger::Get_Detail_Level (void) const
{ // begin
  return this->log_detail_level;
} // Get_Detail_Level

/**
 * \brief Retrieve this instance address - allocated by the subclass
 * \returns a shared pointer for this instance
 * @return 
 */
A4_Lib::Logger::Pointer    A4_Lib::Logger::Instance (void)
{ // begin
  return A4_Lib_Logger_Singleton::logging_instance_pointer;
} // Instance

/**
 * \brief Set the shared instance pointer - called from the subclass after allocation.
 * @param the_instance_pointer - IN - must be a valid address, not nullptr
 * @return No_Error upon success.
 */
Error_Code    A4_Lib::Logger::Set_Instance_Pointer (A4_Lib::Logger::Pointer   the_instance_pointer)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      if (the_instance_pointer == nullptr)
        the_method_error = A4_Error (A4_Logger_Base_Module_ID, SIP_Invalid_Pointer_Address, "Invalid parameter address - the_instance_pointer == nullptr");
      else A4_Lib_Logger_Singleton::logging_instance_pointer = the_instance_pointer;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();           
} // Set_Instance_Pointer


bool  A4_Lib::Logger::Is_Open()
{ // begin
  return this->is_open;
} // Is_Open


/**
 * \brief This method must be overridden in the subclass as it cannot be implemented here.
 * @param the_log_text - IN
 * @param the_calling_function_name - IN
 * @param the_error - IN
 * @param the_state - IN
 * @param the_message_detail_level - IN
 * @return 
 */
Error_Code    A4_Lib::Logger::Write(std::string        the_log_text,
                                    std::string        the_calling_function_name,  
                                    Error_Code         the_error,
                                    Method_State       the_state,
                                    A4_Lib::Logging::Detail    the_message_detail_level)
{ // begin
  return A4_Error_Code (A4_Logger_Base_Module_ID, W_Not_Implemented); 
} // Write        

Error_Code A4_Lib::Logger::Write (std::string       the_log_text,
                                  Logging::Detail   the_message_detail_level)
{ // begin
  return A4_Error_Code (A4_Logger_Base_Module_ID, W_Not_Implemented3); 
} // Write  
/**
 * \brief This method must be overridden in the subclass as it cannot be implemented here.
 * @param the_message_detail_level - IN
 * @param the_log_text - IN - 
 * @param ... - variables to include into the formatted log text
 * @return 
 */
Error_Code    A4_Lib::Logger::Write (A4_Lib::Logging::Detail    the_message_detail_level,
                                     std::string                the_log_text,
                                     ...)
{ // begin
  return A4_Error_Code (A4_Logger_Base_Module_ID, W_Not_Implemented2); 
} // Write 

/**
 * \brief Open the instance by setting the is_open member to true. Must be called from the subclass from the overridden Open method.
 * @return No_Error upon success.
 */
Error_Code    A4_Lib::Logger::Open (void)
{ // begin
  Method_State_Block_Begin(1)
    State(1)
      if (this->Is_Open() == true)
        the_method_error = A4_Error (A4_Logger_Base_Module_ID, O_Already_Open, "Instance is already open.");
      else this->is_open =  true;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();           
} // Open

/**
 * \brief Close the instance by setting the member is_open to false.
 */         
Error_Code    A4_Lib::Logger::Close(void)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      if (this->Is_Open() != true)
        the_method_error = A4_Error (A4_Logger_Base_Module_ID, C_Already_Closed, "Instance is already closed.");
      else this->is_open =  false;          
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();           
} // Close      

/**
 * \brief Retrieve the string description matching the_detail_level
 * @param the_detail_level - IN - if it's not defined, then "Undefined Level" is 
 * @param the_string - OUT - if the_detail_level is not defined, then "Undefined Level" is returned.
 * @return No_Error
 */
Error_Code  A4_Lib::Logger::Get_Detail_Level_String (A4_Lib::Logging::Detail  the_detail_level, // in
                                                     std::string              &the_string) 
{ // begin
  Method_State_Block_Begin(1)
    State(1)  
      the_string.clear();
  
      switch (the_detail_level)
      { // begin
        case A4_Lib::Logging::Private_Comment:
          the_string = "Comment";
        break;
        
        case A4_Lib::Logging::Off:
          the_string = "Off";
        break;

        case A4_Lib::Logging::Error:
          the_string = "ERROR";
        break;

        case A4_Lib::Logging::Warning:
          the_string = "WARNING";
        break;

        case A4_Lib::Logging::Info:
          the_string = "INFO";
        break;

        case A4_Lib::Logging::Module_Specific:
          the_string = "Module-Specific Info";
        break;

        case A4_Lib::Logging::Debug:
          the_string = "DEBUG";
        break;

        case A4_Lib::Logging::Dump_Start:
          the_string = "CONTENT DUMP BEGIN";
        break;

        case A4_Lib::Logging::Dump_End:
          the_string = "CONTENT DUMP END";
        break;

        case A4_Lib::Logging::Content_Dump:
         the_string = "CONTENT DUMP";
        break;

        default:
          the_string = "Undefined Level";
        break;        
      } // switch
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} // Get_Detail_Level_String
