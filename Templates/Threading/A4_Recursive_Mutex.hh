#ifndef __A4_Recursive_Mutex_Defined__
#define __A4_Recursive_Mutex_Defined__

/**
 * \brief Wrapper around STL mutex
 * \author a. zippay * 2017..2019
 * \file  A4_Mutex.hh
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
#include "A4_Lib_Module_ID.hh"
#include <mutex>
#include <thread>

namespace A4_Lib
{ // begin
  typedef class Recursive_Mutex : protected std::recursive_mutex
  { // begin
  public: // construction
    Recursive_Mutex(void);
    virtual ~Recursive_Mutex(void) = default;

  public: // methods

    Error_Code  Lock (bool    &the_mutex_is_acquired,
                      bool    just_try = false);

    Error_Code  Unlock (bool    &the_mutex_is_acquired);


    bool  Is_Locked(void);
    
    bool  Is_My_Lock (void); // thread id's must match and be locked

  private: // data
    std::recursive_mutex  mutex;
    std::thread::id       holder_thread_id; 
    std::size_t           recursion_level;
    
  public: // errors
    enum Mutex_Errors
    { // begin
      L_Invalid_Acquired_State  = 0, /**< Invalid parameter value - the_mutex_is_acquired is not false. Is the mutex already acquired for the parameter not initialized? */
      U_Invalid_Parameter_Value = 1, /**< Invalid parameter value - the_mutex_is_acquired is not true. */
      U_Already_Unlocked        = 2, /**< Invalid mutex state - this->recursion_level == 0, meaning that the last lock has already been released. */
      U_Not_Locked              = 3, /**< Invalid thread id - attempting to unlock this Mutex that is currently locked by another thread. */
    }; // Mutex_Errors
  } Recursive_Mutex;
} // namespace A4_Lib
#endif // __A4_Recursive_Mutex_Defined__

