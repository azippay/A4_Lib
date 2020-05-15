#ifdef A4_Lib_Windows
#include "Stdafx.h"
#endif

#include "A4_Observer.hh"
#include "A4_Method_State_Block.hh"
#include "A4_Lib_Base.hh"
/**
 * \brief   Observer base class implementation. 
 * \file  A4_Observer.cpp
 * \author  a. zippay 
 * 
 * \note 
 * Copyright 1995..2019 a. zippay
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
using namespace A4_Lib;


/**
 * \brief   Empty Notify - Must be overridden if utilized
 * @param the_data - unused
 * @param the_hint - unused
 * @return A4_Observer_Module_ID.N_Not_Implemented
 */
Error_Code  Observer::Notify (std::uint64_t  the_data,
                              Observer::Hint the_hint)
{ // being
  A4_Unused_Arg(the_data);
  A4_Unused_Arg(the_hint); 
  
  Method_State_Block_Begin(1)
    State(1)   
      the_method_error = A4_Error (A4_Observer_Module_ID, N_Not_Implemented, "Method not implemented - need to provide an override");
    End_State
  End_Method_State_Block
 
  return the_method_error.Get_Error_Code();    
} // Notify


/**
 * \brief   Empty Notify - Must be overridden if utilized
 * @param the_msg_block - IN - override should call the_msg_block.reset() when finished
 * @param the_hint - unused
 * @return A4_Observer_Module_ID.N_Not_Implemented2
 */
Error_Code  Observer::Notify (A4_Lib::Message_Block::Pointer  the_msg_block,
			      Observer::Hint                  the_hint)
{ // begin
  A4_Unused_Arg(the_hint); 
  
  Method_State_Block_Begin(1)
    State(1)   
      the_msg_block.reset();
      the_method_error = A4_Error (A4_Observer_Module_ID, N_Not_Implemented2, "Method not implemented - need to provide an override");
    End_State
  End_Method_State_Block
 
  return the_method_error.Get_Error_Code(); 
} // Notify (2)

/**
 * \brief Convert a Hint into a Module_ID.Hint_Offset form.
 * \param the_hint - IN
 * \returns the Dot version of the Hint.
 */
double Observer::Make_Hint_Dot (Observer::Hint  the_hint)
{
  return static_cast<double>(the_hint >> 16) + (static_cast<double>(static_cast<Error_Offset>(the_hint) & 0xFFFF)) / 10000;
}