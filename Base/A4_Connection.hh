#ifndef __A4_Connection_Defined__ 
#define __A4_Connection_Defined__
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

#include "A4_Lib_Base.hh"
#include "A4_Network_Base.hh"

#include <memory>

namespace A4_Lib
{ // begin
  namespace Network
  { // begin
    /**
     * @brief  Base class for all Connection types (Data, Neural, Emotional, etc)
     */
    typedef class Connection
    { // begin
    public: // construction
      Connection(void);
      virtual ~Connection(void);
      
    public: // types
      typedef std::shared_ptr<Connection>  Pointer;
      
    public: // methods
      Connection_ID   Get_Connection_ID (void) const;

      Error_Code  Set_Connection_ID (Connection_ID  the_connection_id);  
      
      Error_Code  Initialize(void);
      
      virtual bool Is_Initialized(void) const;
      
    private: // data      
      Connection_ID     connection_id;
      
      bool              is_initialized;
      
    public: // Errors
      enum Connection_Errors
      { // begin
        SCI_Invalid_Connection_ID     = 0, /**< Invalid parameter value - the_connection_id */
        I_Already_Initialized         = 1, /**< This instance is already initialized. */
      }; // Connection_Errors
    } Connection;
  } // namespace Network
} // namespace A4_Lib

#endif // __A4_Connection_Defined__
