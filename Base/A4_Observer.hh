#ifndef __Observer_Defined__
#define __Observer_Defined__
/**
 * \brief   Observer base class interface. 
 * 
 * \file  A4_Observer.hh
 * \author  a. zippay * 2014, 2017
 * 
 *
 * Copyright 1995..2017 a. zippay
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
#include "A4_Message_Block.hh"

namespace A4_Lib
{ // begin
typedef class Observer
{ // begin
public: // construction
  Observer(void) = default;
  virtual ~Observer(void) = default;  

public: // types
  typedef std::uint64_t Hint;
  typedef std::uint16_t Hint_Offset;
 
  typedef std::shared_ptr<Observer>   Pointer;

public: // methods
  static inline constexpr Hint Make_Hint(const Module_ID   the_module_id,
                                         const Hint_Offset the_hint_offset) {return (static_cast<const A4_Lib::Observer::Hint>(the_module_id) << 16) + static_cast<const A4_Lib::Observer::Hint>(the_hint_offset);};
    
  static double Make_Hint_Dot (Observer::Hint  the_hint);
  
  // deprecated - all new development should use the message block overload
  virtual Error_Code  Notify (std::uint64_t   the_data,
			      Observer::Hint  the_hint);// Notify (1)

  virtual Error_Code  Notify (Message_Block::Pointer  the_msg_block, // passed by value - can be shared with multiple registered Observers
			      Observer::Hint          the_hint); // Notify(2)
  public: // errors
    enum Observer_Errors
    { // begin
      N_Not_Implemented   = 0,
      N_Not_Implemented2  = 1,
    }; // end Observer_Errors
} Observer;

} // namespace A4_Lib

#endif // __Observer_Defined__
