#ifndef __A4_Message_Queue_Defined__ 
#define __A4_Message_Queue_Defined__

#include "A4_Message_Block.hh"

#ifndef A4_DotNet
#include "A4_Recursive_Mutex.hh"

#include <condition_variable>
#include <deque>
#include <mutex>
#endif // A4_DotNet

/**
 * \brief Thread safe (blocking) FIFO message queue.
 * \author  a. zippay * 2017..2020
 * \file A4_Message_Queue.hh
 * 
 * \note  Deliberately uses the std::timed_mutex instead of the A4_Time_Mutex wrapper to avoid cyclical \b additional cyclical dependencies with the method block macros and App_Log.
 * 
 * \note  The MIT License
 *
 * Copyright 1995..2020 albert zippay
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
namespace A4_Lib
{ // begin
  typedef class Message_Queue
  { // being
  public: // construction
    Message_Queue(void);
    virtual ~Message_Queue(void);

  public: // types
    typedef std::shared_ptr<Message_Queue>  Pointer;

  public: // methods
    static Error_Code   Allocate (Message_Queue::Pointer    &the_new_queue);

    Error_Code    Initialize (size_t    the_maximum_queued_items);

    Error_Code    Enqueue (A4_Lib::Message_Block::Pointer   the_message_block,// by value
                           std::int64_t                     the_max_milli_seconds_to_wait = 0, // zero means wait forever
                           bool                             is_high_prio_prepend = false); // true means: 1) the limit is bypassed, 2) the message is pushed front

    Error_Code    Dequeue (A4_Lib::Message_Block::Pointer   &the_message_block, // caller becomes owner
                           std::int64_t                     the_max_milli_seconds_to_wait = 0);// zero means wait forever

    Error_Code    Set_Activation_State(bool    the_new_state);

    bool          Is_Empty(void);
    bool          Is_Activated(void) const;
    bool          Is_Initialized(void) const;

#ifndef A4_DotNet
  private: //data
    std::deque<A4_Lib::Message_Block::Pointer>  msg_queue; /**< queue used as a FIFO - with high priority messages enqueued to the front */

    std::condition_variable                     access_condition; /**< allows for a time-limited blocking of the Deque_Message method.*/
    std::mutex                                  condition_mutex; /**< used in conjunction with the access_condition */
    A4_Lib::Recursive_Mutex		        deque_mutex; /**< allows thread-safe en/dequing of Message_Blocks */

    std::size_t                                 max_queued_items; /**< zero means no fixed limit */

    bool                                        is_activated; /**< indicates that the queue is open for business */
    bool                                        is_initialized; /**< \b true if the instance is initialized */
#endif // A4_DotNet

  public: // errors
    enum Message_Queue_Errors /**< Message_Queue Error_Offsets */
    { // begin
      EQ_Invalid_Address              = 0, /**< \b Enqueue: Invalid parameter address - the_message_block is nullptr */
      A_Allocation_Error              = 1, /**< \b Allocate: Memory allocation error - could not allocate a new Message_Queue instance. */
      EQ_Negative_Time                = 2, /**< \b Enqueue: Invalid parameter value - the_max_milli_seconds_to_wait < 0 */
      EQ_Not_Activated2               = 3, /**< \b Enqueue: The message queue is no longer activated - message not inserted into the queue. */
      EQ_Timeout2                     = 4, /**< \b Enqueue: Could not Enqueue the message block within the allotted time. */
      EQ_Not_Activated                = 5, /**< \b Enqueue: Message queue is not in an Activated state - could not enqueue the message block. */
      DQ_Not_Activated                = 6, /**< \b Dequeue: Message queue is not in an Activated state - could not dequeue the message block. */
      DQ_Invalid_Input_Address        = 7, /**< \b Dequeue: Message queue is not in an Activated state - could not dequeue the message block. */
      DQ_Negative_Time                = 8, /**< \b Dequeue: Invalid parameter value - the_max_milli_seconds_to_wait < 0 */
      I_Already_Initialized           = 9, /**< \b Initialize: Instance is already initialized. */
      I_Invalid_Max_Value             = 10, /**< \b Initialize: Invalid parameter value - the_maximum_queued_items should be set to a non-zero value. */
      A_Invalid_Initial_State         = 11, /**< \b Allocate: Invalid parameter state - the_new_queue must equal nullptr - memory leak? */
    }; // Message_Queue_Errors
  }Message_Queue;
}// namespace A4_Lib
#endif // __A4_Message_Queue_Defined__

