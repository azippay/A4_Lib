/**
 * \brief Thread safe message queue. The intention is to use this within an Active Object where asynchronous data processing is required.
 * \author  a. zippay * 2017..2020
 * \file A4_Message_Queue.cpp
 * \note  * The MIT License
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

#include "A4_Method_State_Block.hh"
#include "A4_Message_Queue.hh"
#include "A4_Utils.hh"

#include <mutex>
#include <chrono>

using namespace A4_Lib;

/**
 * \brief Default constructor
 */
Message_Queue::Message_Queue(void)
{ // begin
  this->max_queued_items = 0; 
  this->is_activated = false; 
  this->is_initialized = false;
} // constructor


/**
 * \brief default destructor
 */
Message_Queue::~Message_Queue(void)
{ // begin
  this->is_activated = false;
  this->max_queued_items = 0;
  this->is_initialized = false;
} // destructor

/**
 * \brief Retrieve the message queue empty state
 * @return 
 */
bool  Message_Queue::Is_Empty(void)
{ // begin
  return this->msg_queue.empty();
} // Is_Empty


/// @brief  Allocate a new Message_Queue::Pointer
/// \param the_new_queue - IN - nullptr - OUT - the new instance
//
Error_Code   Message_Queue::Allocate (Message_Queue::Pointer    &the_new_queue)
{ // begin
   Method_State_Block_Begin(2)
    State(1)  
      if (the_new_queue != nullptr)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, A_Invalid_Initial_State, "Invalid parameter state - the_new_queue must equal nullptr - memory leak?");
    End_State
     
    State(2)
      the_new_queue = std::make_shared<Message_Queue>();
    
      if (the_new_queue == nullptr)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, A_Allocation_Error, "Memory allocation error - could not allocate a new Message_Queue instance.");
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();     
} // Allocate

/**
 * \brief   Initialize the instance
 * @param the_maximum_queued_items - IN - must be non-zero. The value should be chosen with care. Too small can cause blockage, too high could cause data loss or wasted memory. 
 */
Error_Code    Message_Queue::Initialize (std::size_t    the_maximum_queued_items)
{ // begin
 
  Method_State_Block_Begin(3)
    State(1)  
      if (this->Is_Initialized() == true)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, I_Already_Initialized, "Instance is already initialized.");
    End_State
    
    State(2)
      if (the_maximum_queued_items < 1)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, I_Invalid_Max_Value, "Invalid parameter value - the_maximum_queued_items should be set to a non-zero value.");
      else this->max_queued_items = the_maximum_queued_items;
    End_State
      
    State_NoTry(3)
      this->is_initialized = true;
      this->is_activated = true;
    End_State_NoTry
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();   
} // Initialize


/**
 * \brief Retrieve the current initialization state.
 * @return 
 */
bool   Message_Queue::Is_Initialized(void) const
{ // begin
  return this->is_initialized;
} // Is_Initialized


/**
 * \brief Set the message queue activation state
 * @param the_new_state - in
 */
Error_Code    Message_Queue::Set_Activation_State(bool    the_new_state)
{ // begin
  if (the_new_state == true) // <-- why test? don't want un-initialized values being used.
    this->is_activated = true;
  else this->is_activated = false;
  
  return No_Error; // perhaps we should return an error is an un-initialized value is detected...
} // Set_Activation_State

/**
 * \brief   Retrieve the current activated state
 * @return true if this instance is activated
 */
bool   Message_Queue::Is_Activated(void) const
{ // begin
  return this->is_activated;
} // Is_Activated


/**
 * \brief Insert a message block into the queue
 * @param the_message_block - IN - message queue becomes owner, OUT - nullptr
 * @param the_max_milli_seconds_to_wait - IN 
 * @param is_high_prio_prepend - IN - if true, the message is considered to be high-priority and will be pushed to the from of the queue without checking message limit.
 */
Error_Code    Message_Queue::Enqueue (A4_Lib::Message_Block::Pointer    the_message_block,
                                      std::int64_t                      the_max_milli_seconds_to_wait,
                                      bool                              is_high_prio_prepend) 
{ // begin
  std::chrono::time_point<std::chrono::system_clock> the_current_time = std::chrono::system_clock::now();
  std::chrono::time_point<std::chrono::system_clock> the_stop_time = the_current_time + std::chrono::duration<std::int64_t, std::milli>(the_max_milli_seconds_to_wait);
  
  bool	  the_mutex_is_locked = false;

  Method_State_Block_Begin(5)
    State(1)
      if (this->is_activated != true)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, EQ_Not_Activated, "Message queue is not in an Activated state - could not enqueue the message block.");
    End_State
    
    State(2) 
      if ((the_message_block == nullptr) || (the_message_block.use_count() < 1))
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, EQ_Invalid_Address, "Invalid parameter address - the_message_block is nullptr");
    End_State
    
    State(3)
      if (the_max_milli_seconds_to_wait < 0)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, EQ_Negative_Time, "Invalid parameter value - the_max_milli_seconds_to_wait < 0");
    End_State
      
    State(4)
      the_current_time = std::chrono::system_clock::now();
    
    // it's possible that the message queue is full. We can wait for a until the timeout for room to appear.
      while ((this->msg_queue.size() >= this->max_queued_items) && (the_current_time < the_stop_time) && (this->is_activated == true) && (is_high_prio_prepend == false))
      { // wait until the number of items drops below the max or the timeout is exceeded
        A4_Lib::Sleep_MS(50); // perhaps this is too long?
        the_current_time = std::chrono::system_clock::now();
      } // while
    
      if ((msg_queue.size() >= this->max_queued_items) && (is_high_prio_prepend == false))
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, EQ_Timeout2, "Could not Enqueue the message block within the allotted time.");
      else the_method_error = this->deque_mutex.Lock(the_mutex_is_locked);
    End_State
      
    State(5)
      if (this->is_activated == true)
      { // insert the message
        std::lock_guard<std::mutex>(this->condition_mutex);
        
        if (is_high_prio_prepend == false)
          this->msg_queue.push_back(the_message_block); // will throw on failure      
        else this->msg_queue.push_front(the_message_block); // high priority message
        
        this->access_condition.notify_one(); 

	the_message_block.reset(); // this instance now owns the message block

	the_method_error = this->deque_mutex.Unlock(the_mutex_is_locked);
      } // if then
      else the_method_error = A4_Error (A4_Message_Queue_Module_ID, EQ_Not_Activated2, "The message queue is no longer activated - message not inserted into the queue.");
    End_State
  End_Method_State_Block

  if (the_mutex_is_locked == true)
    (void) this->deque_mutex.Unlock(the_mutex_is_locked);

  return the_method_error.Get_Error_Code();    
} // Enqueue
  


/**
 * \brief   Remove a message from the queue
 * @param the_message_block - IN - must be nullptr, OUT - the address of a Message_Block
 * @param the_max_milli_seconds_to_wait - IN - zero means wait forever, otherwise a positive number of milli-seconds
 */
Error_Code    Message_Queue::Dequeue (A4_Lib::Message_Block::Pointer  &the_message_block, // caller becomes owner
                                      std::int64_t                    the_max_milli_seconds_to_wait)// zero means wait forever
{ // begin
  bool	the_mutex_is_locked = false;

  Method_State_Block_Begin(6)
    State(1)
      if (this->is_activated != true)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, DQ_Not_Activated, "Message queue is not in an Activated state - could not dequeue the message block.");
    End_State
    
    State(2) 
      if (the_message_block != nullptr)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, DQ_Invalid_Input_Address, "Invalid parameter address - the_message_block is not nullptr, indicating a memory leak?");
    End_State
      
    State(3)
      if (the_max_milli_seconds_to_wait < 0)
        the_method_error = A4_Error (A4_Message_Queue_Module_ID, DQ_Negative_Time, "Invalid parameter value - the_max_milli_seconds_to_wait < 0");
    End_State     
      
    State(4)
      std::unique_lock<std::mutex>  the_lock(this->condition_mutex); // wait until it's really required
    
      if((this->msg_queue.empty() == true) && (this->access_condition.wait_for(the_lock, std::chrono::duration<std::int64_t, std::milli>(the_max_milli_seconds_to_wait)) == std::cv_status::timeout))
          Terminate_The_Method_Block; // timeout - lock not acquired   
      else if (this->msg_queue.empty() == true)
              Terminate_The_Method_Block; // the lock just "spuriously" woke up
    End_State

    State(5)
      the_method_error = this->deque_mutex.Lock(the_mutex_is_locked);
    End_State

    State(6)
      if ((this->msg_queue.size() > 0) && (this->msg_queue.front() != nullptr))
      { // get the available message
	the_message_block = this->msg_queue.front();  
        this->msg_queue.pop_front();
      } // if then
      // else another thread grabbed the message 
    
      the_method_error = this->deque_mutex.Unlock(the_mutex_is_locked);
    End_State
  End_Method_State_Block
    
  if (the_mutex_is_locked == true)
    (void) this->deque_mutex.Unlock(the_mutex_is_locked);

  return the_method_error.Get_Error_Code(); 
} // Dequeue
