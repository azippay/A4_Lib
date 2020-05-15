/**
 * \brief Wrapper around std::shared_timed_mutex in order to normalize error handling, interface simplification and integration with the A4_Lib.
 * \author a. zippay * 2017, 2018
 * \file  A4_Shared_Timed_Mutex.hh
 * 
 *
 * Copyright 2017 a. zippay
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

#include "A4_Shared_Timed_Mutex.hh"
#include "A4_Method_State_Block.hh"

/// default constructor
//
A4_Lib::Shared_Timed_Mutex::Shared_Timed_Mutex(void)
{ // begin
  this->is_shared = false;
} // constructor


/// Default destructor
//
A4_Lib::Shared_Timed_Mutex::~Shared_Timed_Mutex(void)
{ // begin
  this->thread_id_map.clear();
  this->is_shared = false;
} // destructor


/// Attempt locking the mutext.
/// \param the_mutex_is_locked - IN (must be false), OUT - true if the mutex is locked.
/// \param just_try - IN - 
/// \param num_milliseconds_to_wait - IN - zero == forever
/// \param lock_is_shared - IN - true == shared, false == exclusive
//
Error_Code    A4_Lib::Shared_Timed_Mutex::Lock (bool            &the_mutex_is_locked,
                                                bool            just_try,
                                                std::uint64_t   num_milliseconds_to_wait, // forever
                                                bool            lock_is_shared)
{ // begin
  std::thread::id the_thread_id =  std::this_thread::get_id();
  
  Method_State_Block_Begin(4)    
    State(1)    
      if (the_mutex_is_locked != false)
        the_method_error = A4_Error (A4_Shared_Timed_Mutex_Module_ID, L_Already_Locked1, "Invalid parameter value - the_mutex_is_locked - needs to be false or you need to use a Recursive mutex.");
    End_State
    
    State(2)
      auto the_iterator = this->thread_id_map.find(the_thread_id);
        
      if (the_iterator != this->thread_id_map.end())
      {
        if (just_try == false)
          the_method_error = A4_Error (A4_Shared_Timed_Mutex_Module_ID, L_Already_Locked2, A4_Lib::Logging::Error, "Thread already owns a lock.");
        else Terminate_The_Method_Block; // just trying, after all
      } // if then
    End_State
      
    State(3)
      if (lock_is_shared == A4_Lib::Shared_Timed_Mutex::Lock_Exclusive)
      { // lock exclusive
        if (just_try == true)
          the_mutex_is_locked = try_lock_for (std::chrono::milliseconds(num_milliseconds_to_wait));
        else { // begin
          this->lock(); // will throw on failure
          the_mutex_is_locked = true;
        } // if else
      } // if then
      else { // lock shared
        if (just_try == true)
          the_mutex_is_locked = try_lock_shared_for (std::chrono::milliseconds(num_milliseconds_to_wait));
        else { // begin
          this->lock_shared(); // will throw on failure
          the_mutex_is_locked = true;
        } // if else          
      } // if else
    End_State
      
    State(4)
      auto the_pair = this->thread_id_map.insert(std::pair<std::thread::id, bool>(the_thread_id, true));
    
      if (the_pair.second != true)
      { // insertion failed for some reason...
        the_method_error = A4_Error (A4_Shared_Timed_Mutex_Module_ID, L_Insert_Failure, A4_Lib::Logging::Error, "Could not insert Thread ID %ld into this->thread_id_map.", the_thread_id); 
         
        // don't leave the mutex locked...
        if (this->is_shared == true)
          this->unlock_shared();
        else this->unlock();
        
        the_mutex_is_locked = false;
      } // if then
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();   
} // Lock

 


/// Unlock the mutex
/// \param the_mutex_is_locked - IN (should be true indicating a prior call to Lock, OUT - false indicating a successful release.)
//
Error_Code    A4_Lib::Shared_Timed_Mutex::Unlock (bool  &the_mutex_is_locked)
{ // begin
  std::thread::id the_thread_id = std::this_thread::get_id();
  
   Method_State_Block_Begin(3)    
    State(1)    
      if (the_mutex_is_locked != true)
        the_method_error = A4_Error (A4_Shared_Timed_Mutex_Module_ID, U_Not_Locked1, "Invalid parameter value - the_mutex_is_locked - needs to be true.");
    End_State
    
    State(2)
      auto the_iterator = this->thread_id_map.find(the_thread_id);
        
      if (the_iterator == this->thread_id_map.end())
        the_method_error = A4_Error (A4_Shared_Timed_Mutex_Module_ID, U_Not_Locked2, A4_Lib::Logging::Error, "Thread %ld does not own a mutex lock.", std::this_thread::get_id());
    End_State
      
    State(3)
      if (this->is_shared == true)
        this->unlock_shared();
      else this->unlock();
    
      the_mutex_is_locked = false;

      if (this->thread_id_map.erase(the_thread_id) != 1)
        the_method_error = A4_Error (A4_Shared_Timed_Mutex_Module_ID, U_ID_Not_Erased, A4_Lib::Logging::Error, "Thread ID %ld was not erased from this->thread_id_map", std::this_thread::get_id());
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();       
} // Unlock
