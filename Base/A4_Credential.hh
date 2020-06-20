#ifndef __A4_Credential_Defined__
#define __A4_Credential_Defined__

/**
 * \brief Credential base class interface
 * \author  a. zippay * 2020
 * \file A4_Credential.hh
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
  
#include "A4_Credential_Type.hh"
#include <memory>

namespace A4_Lib
{ // begin
  typedef class Credential
  { // begin
    public: // construction
      Credential (void);
      virtual ~Credential(void);
      
    public: // types
      typedef std::shared_ptr<Credential> Pointer;
      
    public: // methods
      virtual Error_Code  Initialize (Credential_Type   the_credential_type);
      
      bool  Is_Initialized(void) const;
      
      Error_Code  Get_Credential_Type (Credential_Type   &the_credential_type);
      
    private: // data
      Credential_Type   credential_type;
      
      bool              is_initialized;
      
    public: // errors
      enum Credential_Errors
      { // begin
        I_Already_Initialized       = 0, /**< \b Initialize: Instance is already initialized. */
        I_Undefined_Credential_Type = 1, /**< \b Initialize: Invalid parameter value - the_credential_type */
        GCT_Not_Initialized         = 2, /**< \b Get_Credential_Type: Invalid instance state - not initialized. */
      }; // Credential_Errors
  } Credential;  
} // namespace A4_Lib
#endif // __A4_Credential_Defined__
