#ifndef __A4_Logger_Defined__
#define __A4_Logger_Defined__
/**
* \author Albert Zippay * 1995..2020
* \copyright Albert Zippay - 1995 - 2020
* \file A4_Logger.hh
*
* \brief  Logging Singleton interface. 
* \note   The idea here is to allow multiple forms (File, ELK, etc) of logging via one common virtual interface.
*
 * \note  * The MIT License
 *
 * Copyright 1995..2020 a. zippay
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

#include "A4_Lib_Base.hh"
#include <memory>


#define App_Log A4_Lib::Logger::Instance() /**< App_Log->Write versus A4_Lib::Logger::Instance()->Write */

namespace A4_Lib
{ // begin
  typedef class Logger
  { // begin
    protected: // construction
      Logger(void);

      A4_Export virtual ~Logger(void);

    public: // types
      typedef std::shared_ptr<Logger>  Pointer;

    public: // interface
      A4_Export virtual bool Is_Open(void); 

      static Logger::Pointer    Instance (void); // returns the singleton pointer -

      A4_Export virtual Error_Code    Write(std::string        the_log_text,
                                            std::string        the_calling_function_name,  
                                            Error_Code         the_error,
                                            Method_State       the_state,
                                            Logging::Detail    the_message_detail_level);  

     A4_Export Error_Code    Write (Logging::Detail    the_message_detail_level,
                                    std::string        the_log_text,
                                    ...);
     
     A4_Export virtual Error_Code Write (std::string       the_log_text,
                                         Logging::Detail   the_message_detail_level);
     
     A4_Export Logging::Detail  Get_Detail_Level (void) const;
     
      virtual Error_Code  Open (void);
      virtual Error_Code  Close(void);

      Error_Code    Set_Instance_Pointer (Logger::Pointer   the_instance_pointer); // called from subclass after allocation

      Error_Code  Get_Detail_Level_String (Logging::Detail  the_detail_level, // in
                                           std::string      &the_string); // out
      
    private: // data
      Logging::Detail   log_detail_level; /**< The maximum logging detail level that will be written. */

      bool              is_open; /**< if \b true, the log is open for business */

    public: // errors
      enum Logger_Errors
      { // begin
        SIP_Invalid_Pointer_Address             = 0, /**< Invalid parameter address - the_instance_pointer == nullptr */
        W_Not_Implemented                       = 1, /**< method Write must be overridden in the subclass, not called here. */
        O_Already_Open                          = 2, /**< Open has already been called and the log is open for business */
        C_Already_Closed                        = 3, /**< Instance is already closed or was never open. */
        W_Not_Implemented2                      = 4, /**< method Write must be overridden in the subclass, not called here. */
        W_Not_Implemented3                      = 5, /**< method Write must be overridden in the subclass, not called here. */
      }; // Logger_Errors
  } Logger;
} // namespace A4_Lib
#endif // __A4_Logger_Defined__
