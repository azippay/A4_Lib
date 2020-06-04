/**
 * \brief Wrapper around std::recursive_mutex
 * \author a. zippay * 2017..2020
 * \file  A4_Mutex.cpp
 * 
 * \note  STL mutexes come in various flavors with "nice-to-have features missing. They also throw, that must to be handled (caught).
 * 
 *
 * Copyright 1995..2020a. zippay
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

#include "A4_Recursive_Mutex.hh"
#include "A4_Method_State_Block.hh"


using namespace A4_Lib;

/**
 * \@breif Default constructor
 */
Recursive_Mutex::Recursive_Mutex(void)
{ // begin
  this->recursion_level = 0;
} // default constructor

/**
 * \brief   Lock the mutex
 * @param the_mutex_is_acquired - IN - must be false, OUT - true if the mutex is locked by the caller.
 * @param just_try - IN - if true, the mutex will not block until a lock is acquired.
 * 
 * \note  Is_Locked must be tested with the_max_milli_seconds_to_wait set to a non-zero value.
 * @return 
 */
Error_Code  Recursive_Mutex::Lock (bool   &the_mutex_is_acquired,
                                   bool   just_try)
{ // begin
  Method_State_Block_Begin(2)    
    State(1)  
      if (the_mutex_is_acquired != false)
        the_method_error = A4_Error (A4_Recursive_Mutex_Module_ID, L_Invalid_Acquired_State, "Invalid parameter value - the_mutex_is_acquired is not false. Is the mutex already acquired for the parameter not initialized?");
    End_State  
    
    State(2)
      if (just_try != true)
      { // wait forever
        this->lock(); // will throw on failure
        the_mutex_is_acquired = true;
      } // if then
      else { // try for a period of time
        the_mutex_is_acquired = this->try_lock();
      } // if else   
    
      if (the_mutex_is_acquired == true)
      { // lock is acquired, 
        this->recursion_level += 1;
        this->holder_thread_id = std::this_thread::get_id();
      } // if then
    End_State    
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();     
} // Lock

/**
 * \brief Unlock the mutex - must be in a locked state
 * @param the_mutex_is_acquired - IN - should be true - OUT - false after the lock has been released.
 * @return 
 */  
Error_Code  Recursive_Mutex::Unlock (bool    &the_mutex_is_acquired)
{
  Method_State_Block_Begin(2)
    State(1)
      if (the_mutex_is_acquired != true)
        the_method_error = A4_Error (A4_Recursive_Mutex_Module_ID, U_Invalid_Parameter_Value, "Invalid parameter value - the_mutex_is_acquired is not true.");
    End_State
    
    State(2)  
      if (this->holder_thread_id == std::this_thread::get_id())
      { // begin
        if (this->recursion_level == 0)
          the_method_error = A4_Error (A4_Recursive_Mutex_Module_ID, U_Already_Unlocked, "Invalid mutex state - this->recursion_level == 0, meaning that the last lock has already been released.");
        else { // release this lock
          this->unlock();
          the_mutex_is_acquired = false;
          this->recursion_level -= 1;
        } // if else
      } // if then
      else the_method_error = A4_Error (A4_Recursive_Mutex_Module_ID, U_Not_Locked, "Invalid thread id - attempting to unlock this Mutex that is currently locked by another thread.");
    End_State 
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();    
} // Unlock

/**
 * @brief Retrieve the current lock state.
*/
bool  Recursive_Mutex::Is_Locked(void)
{ // begin
  return this->recursion_level > 0; 
} // Is_Locked
    
/**
 * @return \b true if the instance is locked and the thread id's match
 */
bool  Recursive_Mutex::Is_My_Lock (void)
{ // begin
  return this->Is_Locked () && (std::this_thread::get_id() == this->holder_thread_id);
} // Is_My_Lock