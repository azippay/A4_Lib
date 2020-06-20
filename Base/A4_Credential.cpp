/**
 * \brief Credential base class interface
 * \author  a. zippay * 2020
 * \file A4_Credential.cpp
 * \note  The MIT License
 *
 * Copyright 1995..2020 albert zippay
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

#include "A4_Credential.hh"
#include "A4_Utils.hh"
#include "A4_Method_State_Block.hh"

using namespace A4_Lib;

/**
 * @brief default constructor
 */
Credential::Credential(void)
{ // begin
  this->credential_type = Credential_Type::Undefined_Credential_Type;
  this->is_initialized = false;
} // constructor

/**
 * @brief Default destructor
 */
Credential::~Credential(void)
{ // begin
  this->credential_type = Credential_Type::Undefined_Credential_Type;
  this->is_initialized = false;
} // destructor
    
/**
 * Initialize this base class with the_credential_type
 * @param the_credential_type - IN
 * @return No_Error, I_Already_Initialized, I_Invalid_Credential_Type
 */
Error_Code  Credential::Initialize (Credential_Type   the_credential_type)
{ // begin
  Method_State_Block_Begin(2)
    State(1)
      if (this->Is_Initialized() == true)
        the_method_error = A4_Error (A4_Credential_Module_ID, I_Already_Initialized, "Instance is already initialized.");
    End_State
          
    State(2)
      if (the_credential_type == Credential_Type::Undefined_Credential_Type)
        the_method_error = A4_Error (A4_Credential_Module_ID, I_Undefined_Credential_Type, "Invalid parameter value - the_credential_type");
      else { // begin
        this->credential_type = the_credential_type;
        this->is_initialized = true;
      } // if else
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();           
} // Initialize

/**
 * @return the initialization state
 */
bool  Credential::Is_Initialized(void) const
{ // begin
  return this->is_initialized;
} // Is_Initialized
      
/**
 * @brief Retrieve the Credential_Type of this instance. Must be initialized.
 * @param the_credential_type - OUT
 * @return No_Error, GCT_Not_Initialized
 */
Error_Code  Credential::Get_Credential_Type (Credential_Type   &the_credential_type)
{ // begin
  Method_State_Block_Begin(2)
    State(1)  
      if (this->Is_Initialized() != true)
        the_method_error = A4_Error (A4_Credential_Module_ID, GCT_Not_Initialized, "Invalid instance state - not initialized.");
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();           
} // Get_Credential_Type
  