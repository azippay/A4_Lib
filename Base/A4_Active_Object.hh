#ifndef __A4_Active_Object_Defined__
#define __A4_Active_Object_Defined__
/**
 * @brief    Active Object base class - 
 * @author  a. zippay * 2017..2020
 * @copyright (c) 1995...2020, Albert Zippay
 * @file A4_Active_Object.hh
 * @note The Active Object subclass should pass its own Module_ID and an appropriate, non-zero Error_Offset into the Initialize method.
* The MIT License
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

#include "A4_Message_Queue.hh"

#ifndef A4_DotNet
#include "A4_Mutex.hh"
#include <future>
#include <vector>
#endif // A4_DotNet

namespace A4_Lib
{ // begin
  namespace Active_Object_Constant
  { // begin
    static const std::size_t    Min_Num_Threads = 2; /**< without threads, it's just an object - one for processing messages, the other for administration.*/
    static const std::time_t    Check_Thread_Interval = 2; /**< seconds */
    static const std::uint64_t  Min_Message_Queue_Wait_MS = 100; /**< cpu utilization will increase if the message queue times out with shorter periods */
    static const std::uint64_t  Default_Message_Queue_Wait_MS = 250; /**< waiting for a non-empty message queue */
    static const std::size_t    Default_Max_Queued_Messages = 1000; /**< allow a default message queue backlog - if this limit is reached, consider adding more threads */
    static const std::size_t    Min_Queued_Messages = 10; /**< There needs to be some wiggle room - not recommened setting the queue backlog to less than this amount */
  } // namespace Active_Object_Constant

  /**
   * @class Active_Object
   * @brief Active Object - Implements a bare-bones, multi-threaded, general purpose Active Object interface.
   * @param The_Module_ID - The \b Module_ID of the subclass - this should follow the A4 Idiom rules of being system-wide unique.
   * @param The_Error_Offset - This value must be \b greater \b than \b zero and The_Error_Offset \b mod 100 == \b zero.
   */
  typedef class Active_Object
  { // begin definition
  public: // construction         
    Active_Object(void); 
    virtual ~Active_Object(void);

  public: // methods
    virtual Error_Code  Initialize(std::size_t    the_number_of_worker_threads = Active_Object_Constant::Min_Num_Threads,
                                   std::uint64_t  the_message_queue_wait = Active_Object_Constant::Default_Message_Queue_Wait_MS,
                                   std::size_t    the_maximum_queued_items = Active_Object_Constant::Default_Max_Queued_Messages);

    virtual bool Is_Initialized(void);

    virtual Error_Code  Start(void);
    virtual Error_Code  Stop(void);

    bool  Is_Started(void) const;

    std::size_t   Num_Threads (void);

    Error_Code  Enqueue_Message (A4_Lib::Message_Block::Pointer   &the_message_block,
                                 bool                             is_high_prio_prepend = false); 

    bool    Message_Queue_Is_Empty(void);

    Error_Code  Increment_Thread_Count(bool   &the_count_was_incremented);
    Error_Code  Decrement_Thread_Count(void);

  #ifndef A4_DotNet
  protected: // overridables
    virtual   Error_Code  Process_Message (A4_Lib::Message_Block::Pointer   &the_message_block); // <-- Must be overridden to process implementation-specific messages.

    virtual   Error_Code  Handle_Timeout (void);  // <-- called when no messages need to be processed

  private:
    Error_Code  Worker_Thread_Method (void); // performs default message queue handling - the number of active threads is configurable

    Error_Code  Check_Threads (void); // check & start missing threads (or all of them on start up )

    Error_Code  Set_Active (bool  the_active_state); // increments a usage count & allows another thread to be created in the pool

  #ifdef A4_Lib_Windows
    static void   SEH_Exception_Handler (unsigned int         the_code,
                                         EXCEPTION_POINTERS   *the_pointers);
  #endif // A4_Lib_Windows

  protected: // data  
    std::uint64_t   message_queue_wait; /**< The maximum number of milli-seconds to wait for the Message_Queue.Enqueue_Message method to return. */
    std::time_t	    next_check_thread_time; /**< The number of \b seconds to wait before each run of Check_Threads. */

  private: //  data
    A4_Lib::Message_Queue	    message_queue; /**< The blocking message queue - called exclusively by the \b Worker_Thread_Method method. */

    std::vector<std::future<Error_Code>> thread_future_vector; /**< Contains the future Error_Code of a terminating thread. */

    std::size_t       min_num_worker_threads;  /**< the minimum number of active threads required for this active object */
    std::size_t       num_active_threads; /**< can be thought of as the number of processes that require a dedicated thread */

    A4_Lib::Mutex     check_thread_mutex; /**< used by the private \b Check_Threads method - allowing only one thread at-a-time to enable threads. */
    A4_Lib::Mutex     start_stop_mutex; /**< used to prevent overlapping calls to Start & Stop */
    A4_Lib::Mutex     set_active_mutex; /**< Used by the \b Set_Active method to increment/decrement the number of worker threads. */

   
    bool              is_initialized; /**< indicates whether the instance has been Initialized. */
    bool              is_started; /**< indicates whether the instance has been Started. */
  #endif  // A4_DotNet 

  public: // errors
    /**
    * \brief Error conditions that mostly arise from subclass implementation mistakes. 
    */
    enum Active_Object_Errors 
    { // begin
      I_Insufficient_Threads      = 0, /**< Invalid parameter value - the_number_of_worker_threads is less than the minimum. */
      I_Already_Initialized       = 1, /**< Instance is already initialized. */
      S_Already_Started           = 2, /**< The instance is already started. */
      S_Not_Initialized           = 3, /**< The instance is not initialized. */
      S_Not_Started               = 4, /**< The instance is already stopped. */
      I_Invalid_MQ_Wait           = 5, /**<Invalid parameter value - the_message_queue_wait < Min_Message_Queue_Wait_MS */
      I_Invalid_Max_Queued_Items  = 6, /**< Invalid parameter value - the_maximum_queued_items is too small. */
      EM_Not_Started              = 7, /**< The instance is not started - no new messages may be Enqueued. */
      PM_Message_Not_Handled      = 8, /**< This method should not be called but overridden by the subclass. The message was not processed. */
      S_Bad_Thread_Count          = 9, /**< Failed to start a required thread - could be a system resource issue, but most probably a coding error. */
    }; // Active_Object_Errors
  } Active_Object;
} // namespace A4_Lib

#endif // __A4_Active_Object_Defined__
