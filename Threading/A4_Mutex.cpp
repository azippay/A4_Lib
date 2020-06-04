/**
 * \brief Wrapper around std::mutex
 * \author a. zippay * 2017, 2018
 * \file  A4_Mutex.cpp
 * 
 * \note  STL mutexes come in various flavours with "nice-to-have features missing. They also throw, that must to be handled (caught). 
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

#ifdef A4_Lib_Windows
#include "Stdafx.h"
#endif

#include "A4_Mutex.hh"
#include "A4_Lib_Module_ID.hh"
#include "A4_Method_State_Block.hh"


using namespace A4_Lib;

/**
 * \brief default constructor
 */
Mutex::Mutex(void)
{ // begin
  this->is_locked = false;
} // constructor

/**
 * \brief   Lock the mutex
 * @param the_mutex_is_locked - IN - must be \b false. - OUT - \b true if the lock was \b successful.
 * @param just_try - IN - if \b true, then a non-blocking lock is attempted. 
 * @return 
 */
Error_Code  Mutex::Lock (bool   &the_mutex_is_locked,
                         bool   just_try)
{ // begin
  Method_State_Block_Begin(3)    
    State(1)  
      if (the_mutex_is_locked != false)
        the_method_error = A4_Error (A4_Mutex_Module_ID, L_Invalid_Acquired_State, "Invalid parameter value - the_mutex_is_locked is not false. Is the mutex already acquired  is or the parameter not initialized?");
    End_State  
    
    State(2)
      if ((this->is_locked == true) && (std::this_thread::get_id() == this->holder_thread_id))
        the_method_error = A4_Error (A4_Mutex_Module_ID, L_Recursive_Call, "The current thread already owns this mutex - fix the error or consider using the A4_Lib::Recursive_Mutex.");
    End_State
      
    State(3)
      if (just_try != true)
      { // wait forever
        this->mutex_member.lock(); // will throw on failure, otherwise wait forever for a lock
        the_mutex_is_locked = true;
        this->is_locked = true;
        this->holder_thread_id = std::this_thread::get_id();
        Terminate_The_Method_Block;
      } // if then
      else { // just try the lock
        the_mutex_is_locked = this->mutex_member.try_lock(); 
    
        if (the_mutex_is_locked == true)
        { // we own the lock
          the_mutex_is_locked = true;
          this->is_locked = true;
          this->holder_thread_id = std::this_thread::get_id();
        } // if then
      } // if else
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();     
} // Lock

/**
 * \brief Unlock the mutex - must be in a locked state
 * @param the_mutex_is_locked - IN - must be \b true, indicating the Lock is aquired. - OUT - \b false if the lock was successfully released. 
 * @return No_Error, U_Invalid_Parameter_Value, U_Not_The_Owner, U_Not_Locked
 */  
Error_Code  Mutex::Unlock (bool    &the_mutex_is_locked)
{
  Method_State_Block_Begin(3)
    State(1)
      if (the_mutex_is_locked != true)
        the_method_error = A4_Error (A4_Mutex_Module_ID, U_Invalid_Parameter_Value, "Invalid parameter value - the_mutex_is_locked is not \b true.");
    End_State
    
    State(2)
      if (std::this_thread::get_id() != this->holder_thread_id) // test prevents incorrect thread unlocking or double-unlock.
        the_method_error = A4_Error (A4_Mutex_Module_ID, U_Not_The_Owner, "The calling thread is not the owner of this mutex instance.");
    End_State
      
    State(3)
    if (this->is_locked == true)
    { // unlock
      the_mutex_is_locked = false;
      this->mutex_member.unlock();
      this->is_locked = false;
    } // if then
    else the_method_error = A4_Error (A4_Mutex_Module_ID, U_Not_Locked, "Invalid mutex state - attempting to unlock a Mutex that is not currently locked by this thread.");
    End_State 
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();    
} // Unlock

/**
 * @brief Retrieve the current lock state.
*/
bool  Mutex::Is_Locked(void) const
{ // begin
  return this->is_locked;
} // Is_Locked

/**
 * @return \b true if the instance is locked and the thread id's match
 */
bool  Mutex::Is_My_Lock (void)
{ // begin
  return this->Is_Locked () && (std::this_thread::get_id() == this->holder_thread_id);
} // Is_My_Lock
