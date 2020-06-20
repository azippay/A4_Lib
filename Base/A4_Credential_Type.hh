#ifndef __A4_Credential_Type_Defined__
#define __A4_Credential_Type_Defined__
/**
 * \brief Credential Types
 * \author  a. zippay * 2020
 * \file A4_Credential_Type.hh
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
 * 
 */ 
#include "A4_Lib_Base.hh"

namespace A4_Lib
{ // begin
  typedef enum Credential_Type
  { // begin
    Undefined_Credential_Type = 0, 
    ODBC_Credential_Type      = 1,       
  } Credential_Type;
} // namespace A4_Lib
#endif // __A4_Credential_Type_Defined__
