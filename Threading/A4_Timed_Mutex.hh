#ifndef __A4_Timed_Mutex_Defined__
#define __A4_Timed_Mutex_Defined__

/**
 * \brief Wrapper around STL Timed Mutex
 * \author a. zippay * 2017
 * \file  A4_Timed_Mutex.hh
 * 
 * \note  STL mutexes come in various flavours with "nice-to-have features missing. They also throw, that must to be handled (caught).
 * \note  This implementation requires GCC 4.9 or higher or MSVC version (? - need to determine) or higher. 
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

namespace A4_Lib
{ // begin
  typedef class Timed_Mutex
  { // begin
  public: // construction
    Timed_Mutex(void);
    virtual ~Timed_Mutex(void) = default;

  public: // methods

    Error_Code  Lock (bool            &the_mutex_is_locked,
                      std::int64_t    the_max_milli_seconds_to_wait = 0);

    Error_Code  Unlock (bool   &the_mutex_is_locked);

    bool  Is_Locked(void) const;

    bool  Is_My_Lock (void); // thread id's must match and be locked

    private: // data
      std::timed_mutex  mutex_member;

      std::thread::id   holder_thread_id;
      bool              is_locked;    

  public: // errors
    enum Timed_Mutex_Errors
    { // begin
      L_Negative_Time             = 0,
      L_Invalid_Acquired_State    = 1,
      U_Invalid_Param             = 2,
      L_Recursive_Call            = 3,
      U_Not_The_Owner             = 4,
    }; // Timed_Mutex_Errors
  } Timed_Mutex;

} // namespace A4_Lib
#endif // __A4_Timed_Mutex_Defined__

