/**
 * \brief Wrapper around std::timed_mutex to 1) normalize errors, flag common errors
 * \author a. zippay * 2017..2020
 * \file  A4_Timed_Mutex.cpp
 * 
 * 
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
 * 
 */

#ifdef A4_Lib_Windows
#include "Stdafx.h"
#endif

#include "A4_Timed_Mutex.hh"
#include "A4_Method_State_Block.hh"
#include <chrono>

using namespace A4_Lib;

/**
 * \brief Default constructor
 */
Timed_Mutex::Timed_Mutex(void)
{
  this->holder_thread_id = std::thread::id();
  this->is_locked = false;  
} // constructor

/**
 * \brief   Lock the mutex
 * @param the_max_milli_seconds_to_wait - IN - zero means wait forever.
 * @param the_mutex_is_locked - IN - must be false, OUT - true if the mutex is locked. false indicates a timeout
 * \note  Is_Locked must be tested with the_max_milli_seconds_to_wait set to a non-zero value.
 * @return No_Error upon success.
 */
Error_Code  Timed_Mutex::Lock (bool             &the_mutex_is_locked,
                               std::int64_t     the_max_milli_seconds_to_wait)
{ // begin
  Method_State_Block_Begin(4)
    State(1)
      if (the_mutex_is_locked != false)
        the_method_error = A4_Error (A4_Timed_Mutex_Module_ID, L_Invalid_Acquired_State, "Invalid parameter value - the_mutex_is_locked is not false. Is the mutex already acquired for the parameter not initialized?");
    End_State
    
    State(2)  
      if (the_max_milli_seconds_to_wait < 0)
        the_method_error = A4_Error (A4_Timed_Mutex_Module_ID, L_Negative_Time, "Invalid parameter value - the_max_milli_seconds_to_wait < 0");
    End_State
      
    State(3)
      if ((this->is_locked == true) && (std::this_thread::get_id() == this->holder_thread_id))
        the_method_error = A4_Error (A4_Timed_Mutex_Module_ID, L_Recursive_Call, "The current thread already owns this mutex - fix the error or consider using the A4_Lib::Recursive_Mutex.");            
    End_State
            
    State(4)
     if (the_max_milli_seconds_to_wait == 0)
      { // wait forever
        this->mutex_member.lock(); // will throw on failure
        the_mutex_is_locked = true;
      } // if then
      else { // try for a period of time
        the_mutex_is_locked = this->mutex_member.try_lock_for(std::chrono::duration<std::int64_t, std::milli>(the_max_milli_seconds_to_wait));
      } // if else        
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();     
} // Lock

/**
 * \brief Unlock the mutex - must be in a locked state
 * \param the_mutex_is_locked - IN - should be  true, OUT - false
 * @return No_Error upon success.
 */  
Error_Code  Timed_Mutex::Unlock (bool     &the_mutex_is_locked)
{
  Method_State_Block_Begin(2)
    State(1)  
      if (the_mutex_is_locked != true)
        the_method_error = A4_Error (A4_Timed_Mutex_Module_ID, U_Invalid_Param, "Invalid parameter value - the_mutex_is_locked should be true if the mutex has already been locked.");
    End_State
    
    State(2)
      if (std::this_thread::get_id() != this->holder_thread_id) // test prevents incorrect thread unlocking or double-unlock.
        the_method_error = A4_Error (A4_Timed_Mutex_Module_ID, U_Not_The_Owner, "The calling thread is not the owner of this mutex instance.");
      else { // we own it, so we can unlock it
        this->mutex_member.unlock();
        this->is_locked = false;
        the_mutex_is_locked = false;
      } // if else 
    End_State 
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();    
} // Unlock

/**
 * @brief Retrieve the current lock state.
*/
bool  Timed_Mutex::Is_Locked(void) const
{ // begin
  return this->is_locked;
} // Is_Locked

/**
 * @return \b true if the instance is locked and the thread id's match
 */
bool  Timed_Mutex::Is_My_Lock (void)
{
  return this->Is_Locked () && (std::this_thread::get_id() == this->holder_thread_id);
} // Is_My_Lock
