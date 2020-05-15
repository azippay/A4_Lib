#ifndef __Shared_Timed_Mutex_Defined__
#define __Shared_Timed_Mutex_Defined__
/**
 * \brief Wrapper around std::shared_timed_mutex in order to normalize error handling, interface simplification and integration with the A4_Lib.
 * \author a. zippay * 2017
 * \file  A4_Shared_Timed_Mutex.hh
 * @note  The unordered_map is used to define the undefined behavior of the lock method when (accidentally) called recursively by a given thread. An error is created and logged, showing the developer where the
 *        code needs to be fixed.
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
#include "A4_Lib_Base.hh"
#include <shared_mutex>
#include <unordered_map>
#include <thread>

namespace A4_Lib
{ // begin

typedef class Shared_Timed_Mutex : public std::shared_timed_mutex
{ // begin
public: // construction
  Shared_Timed_Mutex(void);
  virtual ~Shared_Timed_Mutex(void);
  
public: // constants  
  static const bool   Just_Try = true;
  static const bool   Block_Until_Locked = false;
  
  static const bool   Lock_Shared = true;
  static const bool   Lock_Exclusive = false;
  
public: // methods
  Error_Code    Lock (bool            &the_mutex_is_locked,
                      bool            just_try = Block_Until_Locked,
                      std::uint64_t   num_milliseconds_to_wait = 0, // forever
                      bool            lock_is_shared = Lock_Exclusive);
  
  Error_Code    Unlock (bool  &the_mutex_is_locked);
  

private: // data  
  std::unordered_map <std::thread::id, bool> thread_id_map; // define the undefined behavior
  
  bool  is_shared;
  
public: // errors
  enum Shared_Timed_Mutex_Errors
  { // begin
    L_Already_Locked1         = 0,
    L_Already_Locked2         = 1,
    U_Not_Locked1             = 2,
    U_Not_Locked2             = 3,
    U_ID_Not_Erased           = 4,
    L_Insert_Failure          = 5,
  }; // Shared_Timed_Mutex_Errors
} Shared_Timed_Mutex;
} // namespace A4_Lib

#endif // __Shared_Timed_Mutex_Defined__
