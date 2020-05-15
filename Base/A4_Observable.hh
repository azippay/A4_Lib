#ifndef __Observable_Defined__
#define __Observable_Defined__
/**
* \brief  Observable base class. 
*
* \author a. zippay * 2013, 2016, 2017
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

#include "A4_Observer.hh"

#ifndef A4_DotNet
#include "A4_Mutex.hh"
#include "A4_Unordered_Map_T.hh"
#endif // A4_DotNet

namespace A4_Lib
{ // begin

  namespace Observable_Constant
  {
    const Error_Offset   Observer_Map_Error_Offset = 100;
  } // namespace Observable_Constant
  
  class  Observable_Entry; //  forward declaration of a private implementation

  typedef class Observable
  { // begin
  public: // construction
    Observable(void);
    ~Observable(void);
    
  public: // methods
    Error_Code	Register_Observer (A4_Lib::Observer	      *the_observer_instance); // C pointers
    Error_Code	Register_Observer (A4_Lib::Observer::Pointer  the_observer_instance);

    Error_Code	Unregister_Observer (A4_Lib::Observer           *the_observer_instance); // C pointers
    Error_Code	Unregister_Observer (A4_Lib::Observer::Pointer  the_observer_instance);

    Error_Code	Notify_Observers (std::uint64_t           the_data,
                                    A4_Lib::Observer::Hint  the_hint = 0);

    Error_Code	Notify_Observers (A4_Lib::Message_Block::Pointer   the_msg_block,
                                    A4_Lib::Observer::Hint           the_hint = 0);

  public: // types
    typedef std::shared_ptr <Observable_Entry>  Observable_Entry_Pointer;
    typedef Unordered_Map_T<std::uint64_t, Observable_Entry_Pointer, A4_Observable_Module_ID, Observable_Constant::Observer_Map_Error_Offset>   Map_Type;
    
  #ifndef A4_DotNet
  private: // data
    //Mutex       notification_mutex; // protects the observer_map
    Map_Type    observer_map;

    bool        is_initialized;
  #endif // A4_DotNet  

  public: // errors
    enum Observable_Errors
    { // begin
      RO_Invalid_Observer_Address       = 0,
      SO_NULL_Instance                  = 1,
      UO_Observer_Not_Found             = 2,
      NCO_Invalid_Observer_Address      = 3,
      UO_Erase_Failed                   = 4,
      NCO_Invalid_Msg_Block             = 5,
      NO_Invalid_Observer_Address2      = 6,
      ROSP_Invalid_Observer_Address     = 7,
      ROSP_Duplicate_Observer           = 8,
      UOSP_Observer_Not_Found           = 9,
      NO_Invalid_Observer_Address       = 10,
      SO_Instance_Is_Shared             = 11,
      SO_Instance_Already_Set           = 12,
      SO_NullPtr_Instance               = 13,
      SO_Instance_Is_Shared2            = 14,
      SO_Instance_Already_Set2          = 15,
      A_Already_Allocated               = 16,
      A_Allocation_Error                = 17,
      ROSP_Duplicate_Observer2          = 18,
      NO_Invalid_Entry_Address2         = 19,
      //NO_Invalid_Observer_Address2      = 20,
    }; // end Observable_Errors
  } Observable;

} // namespace A4_Lib

#endif // __Observable_Defined__
