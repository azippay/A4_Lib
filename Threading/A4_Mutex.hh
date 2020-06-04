#ifndef __A4_Mutex_Defined__
#define __A4_Mutex_Defined__

/**
 * \brief Wrapper around STL mutex
 * \author a. zippay * 2017..2020
 * \file  A4_Mutex.hh
 * 
 * \note  STL mutexes come in various flavours with "nice-to-have features missing. They also throw, so exceptions must to be handled (caught).
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

#include "A4_Lib_Base.hh"
#include <mutex>
#include <thread>

namespace A4_Lib
{ // begin
  /**
   * @class Mutex
   * @brief Wrapper class around the \b std::mutex for the purposes of:
   * 1) capture all exceptions
   * 2) prevent common un-happy flow scenarios.
   */
  typedef class Mutex
  { // begin
  public: // construction
    Mutex(void);
    Mutex(Mutex &) = delete;

    virtual ~Mutex(void) = default; 

    Mutex & operator = (Mutex &) = delete;

  public: // constants
    static const bool   Just_Try = true; /**< something to pass to the \b just_try parameter of the \b Lock method.*/

  public: // methods

    Error_Code  Lock (bool    &the_mutex_is_locked,
                      bool    just_try = false);

    Error_Code  Unlock (bool    &the_mutex_is_locked);

    bool  Is_Locked(void) const;

    bool  Is_My_Lock (void); // thread id's must match and be locked
    
  private: // data
    std::mutex      mutex_member; /**< \b private: the main wrapping target. */
    std::thread::id holder_thread_id; /**< \b private: used to verfiy that the calling thread actually owns the lock */
    bool            is_locked; /**< \b private: current lock state of this instance */

  public: // errors
    enum Mutex_Errors
    { // begin
      L_Invalid_Acquired_State  = 0, /**< the_mutex_is_locked is not false. Is the mutex already acquired or is the parameter not initialized? */
      U_Invalid_Parameter_Value = 1, /**< Invalid parameter value - the_mutex_is_locked is not \b true */
      L_Recursive_Call          = 2, /**< The current thread already owns this mutex - fix the error or consider using the A4_Lib::Recursive_Mutex. */
      U_Not_The_Owner           = 3, /**< The calling thread is not the owner of this mutex instance. */
      U_Not_Locked              = 4, /**< Invalid mutex state - attempting to unlock a Mutex that is not currently locked by this thread. */
    }; // Mutex_Errors
  } Mutex;

} // namespace A4_Lib
#endif // __A4_Mutex_Defined__

