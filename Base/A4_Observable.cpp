
/**
* \brief  Observable base class. 
*
* \author a. zippay * 2013..2018
 *  *
 * Copyright 1995..2018 a. zippay
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

#include "A4_Observable.hh"
#include "A4_Method_State_Block.hh"

namespace A4_Lib
{ // begin


  /********************************   Observable_Entry Implementation   ********************************/
  /**
  \brief  Observable_Entry is a private class wrapping either a classic C pointer or a C++ share_ptr Observer
  */

  typedef class Observable_Entry
  { // begin
  public: // construction
    Observable_Entry(void);
    virtual ~Observable_Entry (void);

  public: // methods
    Error_Code  Set_Observer (A4_Lib::Observer  *the_observer_instance); // class c pointer
    Error_Code  Set_Observer (A4_Lib::Observer::Pointer  the_observer_instance); // c++ shared pointer

    static Error_Code Allocate (Observable::Observable_Entry_Pointer   &the_new_instance);

    Observer * Address (void);

  private: // data
    union
    { // begin
      A4_Lib::Observer            *c_observer_instance;
      A4_Lib::Observer::Pointer   cpp_observer_instance;

      bool   is_shared;
    }; // union data
  } Observable_Entry;


  Observable_Entry::Observable_Entry(void)
  { // begin
    this->is_shared = false;
    this->c_observer_instance = NULL;
  } // constructor


  Observable_Entry::~Observable_Entry(void)
  { // begin
    if ( this->is_shared == true)
      this->cpp_observer_instance.reset();
    else  this->c_observer_instance = NULL;

    this->is_shared = false;
  } // destructor


  Error_Code  Observable_Entry::Set_Observer(A4_Lib::Observer  *the_observer_instance)
  { // begin
    Method_State_Block_Begin(3)
      State(1)
        if (the_observer_instance == NULL)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::SO_NULL_Instance, "Invalid parameter address - the_observer_instance is NULL.");
      End_State

      State(2)
        if (this->is_shared == true)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::SO_Instance_Is_Shared, "Invalid state - this instance is marked as Shared.");
      End_State

      State(3)
        if (this->c_observer_instance != NULL)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::SO_Instance_Already_Set, "Instance state - this instance already has a non-NULL value for c_observer_instance.");
        else { // set the address
          this->c_observer_instance = the_observer_instance;
        } // if else
      End_State
    End_Method_State_Block    
    
    return the_method_error.Get_Error_Code();
  } // Set_Observer (C)


  Error_Code  Observable_Entry::Set_Observer(A4_Lib::Observer::Pointer  the_observer_instance)
  { // begin
    Method_State_Block_Begin(3)
      State(1)
        if (the_observer_instance == nullptr)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::SO_NullPtr_Instance, "Invalid parameter address - the_observer_instance is NULL.");
      End_State

      State(2)
        if (this->is_shared == true)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::SO_Instance_Is_Shared2, "Invalid state - this instance is marked as Shared.");
      End_State

      State(3)
        if (this->cpp_observer_instance != nullptr)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::SO_Instance_Already_Set2, "Instance state - this instance already has a non-NULL value for c_observer_instance.");
        else { // set the address
          this->is_shared = true;
          this->cpp_observer_instance = the_observer_instance;
        } // if else
      End_State
    End_Method_State_Block    
    
    return the_method_error.Get_Error_Code();
  } // Set_Observer (CPP)

  /**
  * \brief  Allocate a new share_ptr Observable_Entry instance.
  * \param  the_new_instance - IN - must be nuillptr, OUT - the new instance.
  */
  Error_Code Observable_Entry::Allocate(Observable::Observable_Entry_Pointer   &the_new_instance)
  { // begin
    Method_State_Block_Begin(2)
      State(1)
        if (the_new_instance != nullptr)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::A_Already_Allocated, "Invalid parameter address - the_new_instance is not nullptr");
      End_State

      State(2)
        the_new_instance = std::make_shared<Observable_Entry>();

        if (the_new_instance == nullptr)
          the_method_error = A4_Error (A4_Observable_Module_ID, Observable::A_Allocation_Error, "Memory allocation error - could not allocate a new Observable_Entry instance.");
      End_State
    End_Method_State_Block    
    
    return the_method_error.Get_Error_Code();
  } // Allocate



  Observer * Observable_Entry::Address(void)
  { // begin
    Observer *the_address = NULL;

    if (this->is_shared == true)
    { // get share_ptr
      if (this->cpp_observer_instance != nullptr)
        the_address = this->cpp_observer_instance.get();
    } // if then
    else the_address = this->c_observer_instance;

    return the_address;
  } // Address

  /********************************   Observable Implementation   ********************************/



  /**
   * \brief default constructor
   */
  Observable::Observable(void)
  { // begin
    this->is_initialized = false;
  } // constructor



  /**
   * \brief Default destructor
   */
  Observable::~Observable(void)
  { // begin
    this->is_initialized = false;
  
//    this->observer_map.clear();
  } // destructor



  /**
   * \brief   Register an Observer with the Observable
   * @param the_observer_instance - IN - must be a valid address and not NULL
   * @return No_Error, ROSP_Invalid_Observer_Address, ROSP_Duplicate_Observer
   */
  Error_Code	Observable::Register_Observer (A4_Lib::Observer::Pointer  the_observer_instance)
  { // begin
   Observable::Observable_Entry_Pointer   the_entry;
   
   bool                       the_mutex_is_acquired = false;
  
    Method_State_Block_Begin(5)
      State(1)
        if (the_observer_instance == nullptr)
	  the_method_error = A4_Error (A4_Observable_Module_ID, ROSP_Invalid_Observer_Address, "Invalid parameter address - the_observer_instance");
        else the_method_error = this->observer_map.Lock(the_mutex_is_acquired);
      End_State
    
      State(2)
        if (this->observer_map.Exists(reinterpret_cast<uint64_t>(the_observer_instance.get())) != false)
          the_method_error = A4_Error (A4_Observable_Module_ID, ROSP_Duplicate_Observer, "Duplicate address - the_observer_instance is already registered.");
        else the_method_error = Observable_Entry::Allocate (the_entry);
      End_State

      State(3)
        the_method_error = the_entry->Set_Observer (the_observer_instance);
      End_State

      State(4)
        the_method_error = this->observer_map.Insert(reinterpret_cast<uint64_t>(the_observer_instance.get()), the_entry); 
      End_State
      
      State(5)
        the_method_error = this->observer_map.Unlock(the_mutex_is_acquired);
      End_State
    End_Method_State_Block
    
    if (the_mutex_is_acquired == true)
      (void) this->observer_map.Unlock(the_mutex_is_acquired);
    
    return the_method_error.Get_Error_Code();
  } // Register_Observer (shared pointer)

  /**
   * \brief   Register an Observer with the Observable
   * @param the_observer_instance - IN - must be a valid address and not NULL
   * @return 
   */
  Error_Code	Observable::Register_Observer (A4_Lib::Observer	  *the_observer_instance)
  { // begin
    Observable::Observable_Entry_Pointer   the_entry;

    bool                       the_mutex_is_acquired = false;

    Method_State_Block_Begin(5)
      State(1)
        if (the_observer_instance == NULL)
	  the_method_error = A4_Error (A4_Observable_Module_ID, RO_Invalid_Observer_Address, "Invalid parameter address - the_observer_instance");
        else the_method_error = this->observer_map.Lock(the_mutex_is_acquired);
      End_State

      State(2)
        if (this->observer_map.Exists(reinterpret_cast<uint64_t>(the_observer_instance)) != false)
          the_method_error = A4_Error (A4_Observable_Module_ID, ROSP_Duplicate_Observer2, "Duplicate address - the_observer_instance is already registered.");
        else the_method_error = Observable_Entry::Allocate (the_entry);
      End_State

      State(3)
        the_method_error = the_entry->Set_Observer (the_observer_instance);
      End_State

      State(4)
        the_method_error = this->observer_map.Insert(reinterpret_cast<uint64_t>(the_observer_instance), the_entry); 
      End_State
      
      State(5)
        the_method_error = this->observer_map.Unlock(the_mutex_is_acquired);
      End_State
    End_Method_State_Block
    
    if (the_mutex_is_acquired == true)
      (void) this->observer_map.Unlock(the_mutex_is_acquired);
    
    return the_method_error.Get_Error_Code();
  } // Register_Observer (C pointer)


  /**
  * \brief  Unregister an Observer
  * \param  the_observer_instance - IN - must be a valid std::shared_ptr
  */
  Error_Code	Observable::Unregister_Observer (A4_Lib::Observer::Pointer  the_observer_instance)
  { // begin
    Observable::Observable_Entry_Pointer    the_entry;

    bool                        the_mutex_is_acquired = false; 
  
    Method_State_Block_Begin(4)
      State(1)
        the_method_error = this->observer_map.Lock(the_mutex_is_acquired);
      End_State
    
      State(2)   
        if(this->observer_map.Exists (reinterpret_cast<uint64_t>(the_observer_instance.get())) != true)
          the_method_error = A4_Error (A4_Observable_Module_ID, UOSP_Observer_Not_Found, "The observer instance was not found in the observer map");
//        else this->observer_map[reinterpret_cast<uint64_t>(the_observer_instance.get())].reset();
      End_State
      
      State(3)
        the_method_error = this->observer_map.Erase (reinterpret_cast<uint64_t>(the_observer_instance.get()));
      End_State

      State(4)
        the_method_error = this->observer_map.Unlock(the_mutex_is_acquired);
      End_State
    End_Method_State_Block

    if (the_mutex_is_acquired == true)
      (void)this->observer_map.Unlock(the_mutex_is_acquired);
  
    return the_method_error.Get_Error_Code();
  } // Unregister_Observer (shared pointer)

  /**
   * \brief   Un-register an Observer
   * @param the_observer_instance - IN - should be a registered observer address and is not NULL
   * @return No_Error upon success.
   */
  Error_Code	Observable::Unregister_Observer (A4_Lib::Observer	  *the_observer_instance)
  { // begin
    bool      the_mutex_is_acquired = false; 
  
    Method_State_Block_Begin(4)
      State(1)
        the_method_error = this->observer_map.Lock(the_mutex_is_acquired);
      End_State
    
      State(2)
        if(this->observer_map.Exists (reinterpret_cast<uint64_t>(the_observer_instance)) != true)
          the_method_error = A4_Error (A4_Observable_Module_ID, UO_Observer_Not_Found, "The observer instance was not found in the observer map");
//        else this->observer_map[reinterpret_cast<uint64_t>(the_observer_instance)].reset();
      End_State
      
      State(3)
        the_method_error = this->observer_map.Erase (reinterpret_cast<uint64_t>(the_observer_instance));
      End_State

      State(4)
        the_method_error = this->observer_map.Unlock(the_mutex_is_acquired);
      End_State
    End_Method_State_Block

    if (the_mutex_is_acquired == true)
      (void)this->observer_map.Unlock(the_mutex_is_acquired);
    
    the_observer_instance = NULL;
  
    return the_method_error.Get_Error_Code();
  } // Unregister_Observer


  /**
  * \brief  Iterate through the observer_map and call the Notify method of each Observer.
  *
  * \param  the_data - IN - data passed to the Observer...may be NULL
  * \param  the_hint - IN - another optional data bit
  */
  Error_Code	Observable::Notify_Observers (std::uint64_t           the_data,
					      A4_Lib::Observer::Hint  the_hint)
  { // begin
    Map_Type::Iterator    the_iterator;
  
    int     the_observer_loop = 0;
  
    bool    the_mutex_is_acquired = false;
  
  
    Method_State_Block_Begin(7)
      State(1)
        the_method_error = this->observer_map.Lock(the_mutex_is_acquired);
        //the_method_error = this->observer_map.Lock(the_mutex_is_acquired); 
      End_State

      State(2)
        the_method_error = this->observer_map.Begin (the_iterator);
      End_State
      
      State(3)
        Define_Target_State(the_observer_loop);
      End_State
    
      State(4)
        if (the_iterator == this->observer_map.End())
        { // finish up
          Terminate_The_Method_Block; // all done
          the_method_error = this->observer_map.Unlock(the_mutex_is_acquired);
        } // if then
      End_State
      
      State(5)
        if (the_iterator->second == NULL)
          the_method_error = A4_Error (A4_Observable_Module_ID, NCO_Invalid_Observer_Address, "Invalid Observer address retrieved from the observer map iterator.");
      End_State

      State(6)
        if (the_iterator->second->Address() == NULL)
          the_method_error = A4_Error (A4_Observable_Module_ID, NO_Invalid_Observer_Address, "An invalid Observer address was encountered.");
        else the_method_error = reinterpret_cast<A4_Lib::Observer *>(the_iterator->second->Address())->Notify (the_data, the_hint); // it's the responsibility of the Observer to handle this asynchronously...or not.
      End_State

      State(7)
        the_iterator++;
    
        Set_Target_State(the_observer_loop);
      End_State
    End_Method_State_Block

    if (the_mutex_is_acquired == true)
      (void) this->observer_map.Unlock(the_mutex_is_acquired);
    
    return the_method_error.Get_Error_Code();
  } // Notify_Observers


  /**
  * \brief  Notify all observers with the contents of the A4_Lib::Message_Block and A4_Lib::Observer::Hint
  * \param  the_msg_block - IN - must not be nullptr - OUT nullptr
  * \param  the_hint - IN
  */
  Error_Code	Observable::Notify_Observers (A4_Lib::Message_Block::Pointer   the_msg_block,
				              A4_Lib::Observer::Hint           the_hint)
  { // begin
    Map_Type::Iterator    the_iterator;

    int     the_observer_loop = 0;
   
    bool    the_mutex_is_acquired = false;
  
    Method_State_Block_Begin(8)
      State(1)
        if (the_msg_block == nullptr)
          the_method_error = A4_Error (A4_Observable_Module_ID, NCO_Invalid_Msg_Block, "Invalid parameter - the_msg_block == nullptr");
        else the_method_error = this->observer_map.Lock(the_mutex_is_acquired); 
      End_State

      State(3)
        the_method_error = this->observer_map.Begin (the_iterator);
      End_State
      
      State(4)
        Define_Target_State(the_observer_loop);
      End_State
    
      State(5)
        if (the_iterator == this->observer_map.End())
        { // finish up
          Terminate_The_Method_Block; // all done
          the_method_error = this->observer_map.Unlock(the_mutex_is_acquired);
        } // if then
      End_State

      State(6)
        if (the_iterator->second == NULL)
          the_method_error = A4_Error (A4_Observable_Module_ID, NO_Invalid_Entry_Address2, "Invalid Observer address retrieved from the observer map iterator.");
      End_State

      State(7)
        if (the_iterator->second->Address() == NULL)
          the_method_error = A4_Error (A4_Observable_Module_ID, NO_Invalid_Observer_Address2, "An invalid Observer address was encountered.");
        else the_method_error = reinterpret_cast<A4_Lib::Observer *>(the_iterator->second->Address())->Notify (the_msg_block, the_hint); // it's the responsibility of the Observer to handle this asynchronously...or not.
      End_State

      State(8)
        the_iterator++;
    
        Set_Target_State(the_observer_loop);
      End_State
    End_Method_State_Block

    the_msg_block.reset();

    if (the_mutex_is_acquired == true)
      (void) this->observer_map.Unlock(the_mutex_is_acquired);
    
    return the_method_error.Get_Error_Code();
  } // Notify_Observer
} // namespace A4_Lib