/**
 * @brief    Active Object base class implementation- 
 * @author  a. zippay * 2017..2020
 * @copyright (c) 1995...2020, Albert Zippay
 * @file A4_Active_Object.cpp
 *  * @note The Active Object subclass should pass its own Module_ID and an appropriate, non-zero Error_Offset into the Initialize method.
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
#ifdef A4_Lib_Windows
#include "Stdafx.h"
#endif
 
#include "A4_Active_Object.hh"
#include "A4_Method_State_Block.hh"
#include "A4_Utils.hh"

using namespace A4_Lib;

/**
 * @brief Default constructor
 */
Active_Object::Active_Object(void)
{ // begin
  this->is_initialized = false;
  this->is_started = false;
  this->min_num_worker_threads = 0;
  this->next_check_thread_time = 0;
  this->message_queue_wait = 0;
  this->num_active_threads = 0;
} // constructor

/**
* \brief Default destructor
*/
Active_Object::~Active_Object(void)
{ // begin
  if (this->Is_Started() == true)
    (void) this->Stop();
  
  this->is_initialized = false;
  this->is_started = false;
  this->min_num_worker_threads = 0;
  this->next_check_thread_time = 0;
  this->message_queue_wait = 0;
  this->num_active_threads = 0;
} // destructor

/**
* \brief  Increment the thread count limit by one.
* \param  the_count_was_incremented - OUT - flag indicating the action was successful - used by the caller as an indication that the count should be decremented when exiting the method.
*/
Error_Code  Active_Object::Increment_Thread_Count(bool   &the_count_was_incremented)
{ // begin
  Method_State_Block_Begin(2)
    State(1)
      the_count_was_incremented = false;

      the_method_error = this->Set_Active (true);
    End_State

    State(2)
      the_count_was_incremented = true;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();  
} // Increment_Thread_Count

/**
* \brief  Decrement the thread count limit by one. The thread is not terminated but remains in a thread pool.
*/
Error_Code  Active_Object::Decrement_Thread_Count(void)
{ // begin
  Method_State_Block_Begin(1)
    State(1)
      the_method_error = this->Set_Active (false);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();  
} // Decrement_Thread_Count

/**
* \brief  Increment / Decrement the number of active threads that should be running.
* \param  the_active_state - IN - when true, the number of active threads is incremented, false decrements the count
* \note  The actual number of threads may or may not change.. However, if the number of threads is < this->num_active_threads, one will be created. 
*/
Error_Code  Active_Object::Set_Active(bool  the_active_state)
{ // begin
  bool the_mutex_is_locked = false;

  Method_State_Block_Begin(3)
    State(1)
      the_method_error = this->set_active_mutex.Lock(the_mutex_is_locked);
    End_State

    State(2)
      if (the_active_state == true)
      { // add a thread 
        this->num_active_threads += 1;
        this->next_check_thread_time = 0;        
      } // if then
      else { // allow a thread to end
        if (this->num_active_threads > 0)
          this->num_active_threads -= 1;

        Terminate_The_Method_Block;
      } // if else

      the_method_error = this->set_active_mutex.Unlock(the_mutex_is_locked);
    End_State

    State(3)
      the_method_error = this->Check_Threads(); // create a new thread
    End_State
  End_Method_State_Block
    
  if (the_mutex_is_locked == true)
    (void) this->set_active_mutex.Unlock(the_mutex_is_locked);

  return the_method_error.Get_Error_Code();  
} // Set_Active

/**
 * @brief Enqueue a \b Message_Block
 * @param the_message_block - IN
 * @param is_high_prio_prepend - IN - if \b true, then the message block will be placed in the front of the queue.
 * @return 
 */
Error_Code  Active_Object::Enqueue_Message(A4_Lib::Message_Block::Pointer   &the_message_block,
                                         bool                             is_high_prio_prepend)
{ // begin
  Method_State_Block_Begin(1)
    State(1)  
      if (this->Is_Started() != true)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, EM_Not_Started, "The instance is not started - no new messages may be Enqueued.");
      else the_method_error = this->message_queue.Enqueue(the_message_block, this->message_queue_wait, is_high_prio_prepend);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();   
} // Enqueue_Message

/**
*  @brief Initialize this instance
*  @param the_number_of_worker_threads - IN - must be >= Active_Object_Constant::Min_Num_Threads
*  @param the_message_queue_wait - IN - a non-zero value indicating the maximum number of milli-seconds to wait for a message to appear in the queue. Must be >=  Min_Message_Queue_Wait_MS.
*  @param the_maximum_queued_items - IN - The maximum number of messages allowed in the message queue before it blocks. Must be >= Active_Object_Constant::Min_Queued_Messages
*/
Error_Code  Active_Object::Initialize(std::size_t    the_number_of_worker_threads,
                                      std::uint64_t  the_message_queue_wait,
                                      std::size_t    the_maximum_queued_items)
{ // begin
  Method_State_Block_Begin(5)
    State(1)
      if (this->Is_Initialized() == true)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, I_Already_Initialized, "Instance is already initialized.");
    End_State
    
    State(2)  
      if (the_number_of_worker_threads < Active_Object_Constant::Min_Num_Threads)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, I_Insufficient_Threads, "Invalid parameter value - the_number_of_worker_threads is less than the minimum.");
    End_State
    
    State(3)
      if (the_message_queue_wait < Active_Object_Constant::Min_Message_Queue_Wait_MS)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, I_Invalid_MQ_Wait, "Invalid parameter value - the_message_queue_wait < Min_Message_Queue_Wait_MS.");
    End_State
      
    State(4)
      if (the_maximum_queued_items < Active_Object_Constant::Min_Queued_Messages)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, I_Invalid_Max_Queued_Items, "Invalid parameter value - the_maximum_queued_items is too small.");
      else the_method_error = this->message_queue.Initialize(the_maximum_queued_items);
    End_State
      
    State(5)
      this->min_num_worker_threads = the_number_of_worker_threads;
      this->message_queue_wait = the_message_queue_wait;
      
      this->is_initialized = true;
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();   
} // Initialize

/**
* @brief   Retrieve the current initialization status.
* @return \b true if the instance is initialized.
*/
bool Active_Object::Is_Initialized(void)
{ // begin
  return this->is_initialized;
} // Is_Initialized

/** 
* @brief Retrieve the current started state of the instance.
* @return \b true if the instance is started.
*/
bool Active_Object::Is_Started(void) const
{ // begin
  return this->is_started;
} // Is_Started

/**  
 * @brief Retrieves the Empty state of the internal message queue.
 * @return \b true if the internal message queue is empty.
 */
bool Active_Object::Message_Queue_Is_Empty(void)
{ // begin
  return this->message_queue.Is_Empty();
} // Message_Queue_Is_Empty

/**
*  @brief Start the worker thread(s) of the instance. Must be initialized.
*/
Error_Code  Active_Object::Start(void)
{ // begin
  std::size_t   num_threads_started = 0;
  
  bool    the_mutex_is_acquired = false;
  
  Method_State_Block_Begin(5)
    State(1)
      the_method_error = this->start_stop_mutex.Lock(the_mutex_is_acquired);
    End_State
    
    State(2)  
      if (this->Is_Started() == true)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, S_Already_Started, "The instance is already started.");
    End_State
    
    State(3)
      if (this->Is_Initialized() != true)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, S_Not_Initialized, "The instance is not initialized.");
    End_State
      
    State(4)
      this->is_started = true; 
    
      the_method_error = this->Check_Threads();
      
      A4_Lib::Sleep_MS(5); // wait for the thread(s) to start
      
      if (the_method_error != No_Error)
        this->is_started = false;
      else the_method_error = this->start_stop_mutex.Unlock(the_mutex_is_acquired);
    End_State
      
    State(5)
      num_threads_started = this->Num_Threads();
    
      if (this->min_num_worker_threads != num_threads_started)
      { // oops - something is wrong...
        the_method_error = A4_Error (A4_Active_Object_Module_ID, S_Bad_Thread_Count, A4_Lib::Logging::Error, 
                                     "Failed to start the required %lld threads, instead %lld were created.", this->min_num_worker_threads, num_threads_started);
        
        if (this->Is_Started() == true)
          (void) this->Stop();
      } // if then
    End_State
  End_Method_State_Block
    
  if (the_mutex_is_acquired == true)
    (void) this->start_stop_mutex.Unlock(the_mutex_is_acquired);
    
  return the_method_error.Get_Error_Code();  
} // Start

/**
*  @brief Stop the worker thread(s) of the instance. 
*/
Error_Code  Active_Object::Stop(void)
{ // begin
  bool    the_mutex_is_acquired = false;
  
  Method_State_Block_Begin(2)
    State(1)
      the_method_error = this->start_stop_mutex.Lock(the_mutex_is_acquired);
    End_State
    
    State(2)  
      if (this->Is_Started() != true)
        the_method_error = A4_Error (A4_Active_Object_Module_ID, S_Not_Started, "The instance is already stopped.");
      else { // set status
        this->is_started = false; // this should stop the thread(s)
        
        while (this->Num_Threads() > 0)
          A4_Lib::Sleep_MS(10);
        
        the_method_error = this->start_stop_mutex.Unlock(the_mutex_is_acquired);
      } // if else
    End_State
  End_Method_State_Block
    
  if (the_mutex_is_acquired == true)
    (void) this->start_stop_mutex.Unlock(the_mutex_is_acquired);
    
  return the_method_error.Get_Error_Code();    
} // Stop

/**
*  @brief  Retrieve the current number of \b active worker threads.
*/
std::size_t   Active_Object::Num_Threads (void)
{ // begin
  std::size_t   the_offset = 0;  
  std::size_t   the_number_of_threads = 0;
  
  bool          the_mutex_is_acquired = false;
  
  Method_State_Block_Begin(2)
    State(1) 
      the_method_error = this->check_thread_mutex.Lock(the_mutex_is_acquired);
    End_State
    
    State(2)
      for (the_offset = 0; the_offset < this->thread_future_vector.size(); the_offset++)
        if (this->thread_future_vector [the_offset].wait_for(std::chrono::seconds(0)) != std::future_status::ready)
          the_number_of_threads += 1;
      
      the_method_error = this->check_thread_mutex.Unlock(the_mutex_is_acquired);
    End_State
  End_Method_State_Block

  if (the_mutex_is_acquired == true)
    (void) this->check_thread_mutex.Unlock(the_mutex_is_acquired);
    
  return the_number_of_threads;  
} // Num_Threads

/**
*  @brief Active worker threads at startup, Restart crashed threads if necessary
*/
Error_Code  Active_Object::Check_Threads (void)
{ // begin
  std::size_t   the_offset = 0;
  
  Error_Code    the_error = No_Error;
  
  std::time_t   the_current_time = A4_Lib::Now();
  
  bool          the_mutex_is_acquired = false;
  
  Method_State_Block_Begin(5)
    State(1) 
      if ((this->Is_Started() != true) || (this->next_check_thread_time > the_current_time))
         Terminate_The_Method_Block; // some other thread is busy, or it's not quite time
      else the_method_error = this->check_thread_mutex.Lock(the_mutex_is_acquired, A4_Lib::Mutex::Just_Try);
    End_State
    
    State(2)
      if (the_mutex_is_acquired != true)
        Terminate_The_Method_Block; // another thread is busy here
    End_State
      
    State(3)
      this->next_check_thread_time = the_current_time + Active_Object_Constant::Check_Thread_Interval;
    
      for (the_offset = 0; the_offset < this->thread_future_vector.size(); the_offset++)
        if (this->thread_future_vector [the_offset].wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        { // log the error and remove the old future
          the_error = this->thread_future_vector[the_offset].get();
          
          App_Log->Write (A4_Lib::Logging::Error, "Worker thread terminated with error %1.5f and will be restarted.", A4_Error::Get_Dot_Error_Code(the_error)); // 
          this->thread_future_vector.erase(this->thread_future_vector.begin() + the_offset);
        } // for if then
    End_State
      
    State(4) // start thread(s)
      while (this->thread_future_vector.size() < (this->min_num_worker_threads + this->num_active_threads))
      { // begin
        this->thread_future_vector.push_back (std::async(std::launch::async, &Active_Object::Worker_Thread_Method, this));
	(void) App_Log->Write (A4_Lib::Logging::Content_Dump, "Another worker thread has been activated. Expected thread count %lld", (this->min_num_worker_threads + this->num_active_threads));
      } // while
    End_State
      
    State(5)
      the_method_error = this->check_thread_mutex.Unlock(the_mutex_is_acquired);
    End_State
  End_Method_State_Block
    
  if (the_mutex_is_acquired == true)
    (void) this->check_thread_mutex.Unlock(the_mutex_is_acquired);
    
  return the_method_error.Get_Error_Code();   
} // Check_Threads
  
/**
*  @brief Handle administrative tasks  
*  @note  This base-class instance should be called \b first from the override method \b before performing its own processes..
*/
Error_Code  Active_Object::Handle_Timeout (void)
{ // begin
  Method_State_Block_Begin(1)
    State(1)   
      the_method_error = this->Check_Threads();
    End_State
  End_Method_State_Block  
    
  return the_method_error.Get_Error_Code();  
} // Handle_Timeout

/**
*  @brief Does nothing but delete the message block
*  @param the_message_block - IN - data to be processed - OUT - nullptr
*  @note The method should \b not be called from the overridden subclass implementation.
*  @returns No_Error (Success), PM_Message_Not_Handled 
*/
Error_Code  Active_Object::Process_Message (A4_Lib::Message_Block::Pointer   &the_message_block)
{ // begin
  Method_State_Block_Begin(1)
    State(1)
      if (the_message_block != nullptr)
      { // delete the message block since
        the_message_block.reset();
   
        the_method_error = A4_Error (A4_Active_Object_Module_ID, PM_Message_Not_Handled, "This method should not be called but overridden by the subclass. The message was not processed.");
      } // if then
    End_State
  End_Method_State_Block  
    
  return the_method_error.Get_Error_Code();  
} // Process_Message

#ifdef A4_Lib_Windows

  /**
  * \brief  SEH exception handler - captures the SEH exception and re-throws it as a c++ exception. This allows method state block to preserve call stack.
  * \note In a production environment, it's rare for this to be called and something unusual has happend to the system.
  */
  void   Active_Object::SEH_Exception_Handler (unsigned int         the_code,
                                                    EXCEPTION_POINTERS   *the_pointers)
    { // begin 
      A4_Unused_Arg (the_pointers);
   
      App_Log->Write (A4_Lib::Logging::Error, "SEH Exception %d Thrown...re-throwing a c++ generic exception to preserve the call stack...", the_code); 
 
      throw 1;// re-throw as a c++ exception

    } // end SEH_Exception_Handler

#endif // A4_Lib_Windows

/**
 * @brief Main worker thread method. Waits for a \b Message_Block to be retrieved from the internal \b Message_Queue and call \b Process_Message. 
 * If no message appears, \b Handle_Timeout will be called.
 * @return No_Error (success)
 */
Error_Code  Active_Object::Worker_Thread_Method (void)
{ // begin
  A4_Lib::Message_Block::Pointer  the_message_block;
  
  int                             the_main_loop = 0;
  
#ifdef A4_Lib_Windows
  (void) _set_se_translator(this->SEH_Exception_Handler);
#endif // #ifdef A4_Lib_Windows

  Method_State_Block_Begin(5)
    State(1)   
      Define_Target_State(the_main_loop);
    End_State
    
    State(2) 
      if ((this->Is_Started() != true) && ((this->message_queue.Is_Empty() == true) || (this->message_queue.Is_Activated() == false)))
        Terminate_The_Method_Block; // shut down - 
    End_State
      
    State(3)
      the_method_error = this->message_queue.Dequeue(the_message_block, this->message_queue_wait);
    End_State
      
    State(4)
      if (the_message_block == nullptr)
        the_method_error = this->Handle_Timeout(); // no message with no error means timeout
      else the_method_error = this->Process_Message(the_message_block);
    End_State
        
    State(5)
      if (the_message_block != nullptr)
      { // garbage collect
        the_message_block.reset();
      } // if then
      
      if (this->next_check_thread_time < A4_Lib::Now()) // <-- test to be sure the thread count is still being checked, there may not be any idle time
        the_method_error = this->Check_Threads(); 
      
      Set_Target_State(the_main_loop);
    End_State
  End_Method_State_Block
    
  if (the_message_block != nullptr)
  { // garbage collect
    the_message_block.reset();
  } // if then
    
  if ((this->Is_Started() == true) && (the_method_error != No_Error))
    (void) this->Check_Threads(); // see if the thread needs restarting
    
  return the_method_error.Get_Error_Code();    
} // Worker_Thread_Method
