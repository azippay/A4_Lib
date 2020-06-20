/**
* \author Albert Zippay * 2020
* \copyright Albert Zippay - 2020
* \file A4_Connection.hh
*
* \brief  Base class for all Connections (Data, Neural, Emotional, etc)
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

#ifdef A4_Lib_Windows
  #include "Stdafx.h"
#endif

#include "A4_Connection.hh"
#include "A4_Method_State_Block.hh"

using namespace A4_Lib;
       
/**
 * \brief default constructor
 */
Connection::Connection(void)
{ // begin
  this->connection_id = Invalid_Connection_ID;
  this->is_initialized = false;
} // constructor

/**
 * \brief Default destructor.
 */
Connection::~Connection(void)
{ // begin
  this->connection_id = Invalid_Connection_ID;
  this->is_initialized = false;
} // destructor

/**
 * \brief Set the member Connection_ID
 * @param the_connection_id
 * @return No_Error, SCI_Invalid_Connection_ID
 */
Error_Code  Connection::Set_Connection_ID (Connection_ID  the_connection_id)
{ // begin
  Method_State_Block_Begin(1)
    State(1)
      if (the_connection_id == Invalid_Connection_ID)
        the_method_error = A4_Error (A4_Connection_Module_ID, SCI_Invalid_Connection_ID, "Invalid parameter value - the_connection_id");
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();              
} // Set_Connection_ID

/**
 * \brief Retrieve the current Connection_ID
 * @return this->connection_id value
 */
Connection::Connection_ID   Connection::Get_Connection_ID (void) const
{ // begin
  return this->connection_id;
} // Get_Connection_ID

/**
 * @brief Initialize this base instance
 * @return No_Error, I_Already_Initialized
 */
Error_Code  Connection::Initialize(void)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      if (this->Is_Initialized() == true)
        the_method_error = A4_Error (A4_Connection_Module_ID, I_Already_Initialized, "This instance is already initialized.");
      else this->is_initialized = true;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();              
} // Initialize

/**
 * 
 * @return the current initialization state
 */
bool Connection::Is_Initialized(void) const
{
  return this->is_initialized;
}