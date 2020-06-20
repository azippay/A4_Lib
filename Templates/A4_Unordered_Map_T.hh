#ifndef __A4_Unordered_Map_T
#define __A4_Unordered_Map_T
/**
* \brief    Template wrapper for the std::unordered_map 
*
* \author   a. zippay * 2017..2020
* 
* \note The intention with this wrapper is to: 1) marshal access to the map with a Mutex. 2) Validate parameters and log common errors.
* 
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
*/
#ifdef A4_Lib_Windows
#include "Stdafx.h"
#endif

#include "A4_Method_State_Block.hh"
#include "A4_Recursive_Mutex.hh"
#include <unordered_map>

namespace A4_Lib
{ // begin
  /**
   * @brief Unordered_Map_T template wrapper around the std::unordered_map template. 
   * @param The_Key_Class - typename of the key class
   * @param The_Data_Class - the data paired with the key
   * @param The_Module_ID - The Module_ID from the class using this template.
   * @param The_Error_Offset - An error offset that allows all Unordered_Map_T to be unique.
   */
  template <typename      The_Key_Class,
            typename      The_Data_Class,
            Module_ID     The_Module_ID,
            Error_Offset  The_Error_Offset> class Unordered_Map_T
  { // begin
    public: // construction
      Unordered_Map_T(void) = default;     
      virtual ~Unordered_Map_T(void) = default;
    
      typedef typename std::unordered_map <The_Key_Class, The_Data_Class> Map_Type; 
      typedef typename std::unordered_map <The_Key_Class, The_Data_Class>::iterator    Iterator; /**< type returned from Begin and End methods */
      
    public: // methods     
/**
 * @brief Update the_data for an \b existing key 
 * @param the_key - IN - must exist in the map
 * @param the_data - IN
 * @return 
 */      
      Error_Code  Update (The_Key_Class   the_key,    
                          The_Data_Class  the_data)
      { // begin
        bool    the_mutex_is_locked = false;

        Method_State_Block_Begin(2)
          State(1) 
           the_method_error = this->map_mutex.Lock(the_mutex_is_locked);
          End_State

          State(2)
            this->at(the_key) = the_data; // throws if the_key does not exist
          End_State
         End_Method_State_Block

         if (the_mutex_is_locked == true)
           (void) this->map_mutex.Unlock(the_mutex_is_locked);

         return the_method_error.Get_Error_Code();
      } // Update
/**
 * @brief Test whether the_key exists.
 * @param the_key - IN
 * @return true if the_key exists in the map.
 */      
      bool  Exists (The_Key_Class   the_key)
      { // begin
        auto the_iterator = this->map.find(the_key);
        return  (the_iterator != this->map.end());
      } // Exists      

/**
 * \brief Insert The_Data_Class into the map.
 * @param the_key - IN
 * @param the_data - IN
 */      
      Error_Code  Insert (The_Key_Class   the_key,    
                          The_Data_Class  the_data)
{ // begin
  bool    the_mutex_is_locked = false;
  
  Method_State_Block_Begin(2)
    State(1) 
      the_method_error = this->map_mutex.Lock(the_mutex_is_locked);
    End_State  
    
    State(2)
      auto the_pair = this->map.insert(std::pair<The_Key_Class, The_Data_Class>(the_key, the_data));
 
       if (the_pair.second != true)
         the_method_error = A4_Error (The_Module_ID, I_Insert_Failure, "The new info instance was not inserted into this unordered map. Did the key already exist?"); 
       else the_method_error = this->map_mutex.Unlock(the_mutex_is_locked);
     End_State
   End_Method_State_Block

   if (the_mutex_is_locked == true)
     (void) this->map_mutex.Unlock(the_mutex_is_locked);
     
   return the_method_error.Get_Error_Code();
} // Insert
      
/**
 * \brief Find an entry in the map.
 * @param the_key - IN
 * @param the_data - OUT - caller responsible for the initial state
 * @param is_found - OUT - set to \b true if the_data is found.
 * @return No_Error upon success.
 */      
      Error_Code  Find (The_Key_Class   the_key,    
                        The_Data_Class  &the_data,  
                        bool            &is_found)
{ // begin
  bool    the_mutex_is_locked = false;
  
  Method_State_Block_Begin(2)
    State(1) 
      is_found = false;
  
      the_method_error = this->map_mutex.Lock(the_mutex_is_locked);
    End_State  
     
    State(2)
      auto the_iterator = this->map.find(the_key);
        
      if (the_iterator != this->map.end())
      { // found it
        the_data = the_iterator->second;
        is_found = true;
      } // if then
    
      the_method_error = this->map_mutex.Unlock(the_mutex_is_locked);
    End_State
  End_Method_State_Block

  if (the_mutex_is_locked == true)
    (void) this->map_mutex.Unlock(the_mutex_is_locked);
     
  return the_method_error.Get_Error_Code();
} // Find
      
/** 
 * \brief Remove and existing items from the map 
 * @param the_key - IN - Must exist in the map, or an error is generated.
 * @return No_Error when successful
 */      
      Error_Code  Erase (The_Key_Class   the_key)
      { // begin
        bool    the_mutex_is_locked = false;

        Method_State_Block_Begin(2)
          State(1)   
            the_method_error = this->map_mutex.Lock(the_mutex_is_locked);
          End_State

          State(2)
            if (this->map.erase(the_key) < 1)
              the_method_error = A4_Error (The_Module_ID, E_Erase_Unsuccessful, "The key was not removed from the map.");
          End_State
        End_Method_State_Block

        if (the_mutex_is_locked == true)
          (void) this->map_mutex.Unlock(the_mutex_is_locked);

        return the_method_error.Get_Error_Code();  
      } // Erase    
      
  /**
   * @brief Reset all the shared memory data items in the map, then clear the pairs.
   * @return No_Error when successful
   */      
      Error_Code  Reset_All (void)
      { // begin
        bool    the_mutex_is_locked = false;

        Method_State_Block_Begin(2)
          State(1) 
            the_method_error = this->map_mutex.Lock(the_mutex_is_locked);
          End_State 

          State(2)
            auto it = this->map.begin();

            while (it != this->map.end())
            { // begin
              if (it->second != nullptr)
                it->second.reset();

              it++;
            } // while

            this->map.clear();

            the_method_error = this->map_mutex.Unlock(the_mutex_is_locked);
          End_State
         End_Method_State_Block

         if (the_mutex_is_locked == true)
           (void) this->map_mutex.Unlock(the_mutex_is_locked);

         return the_method_error.Get_Error_Code();
      } // Reset_All              
      
/**
 * \brief Lock the map_mutex member before using an iterator
 * @param the_mutex_is_acquired - IN / OUT
 * @param just_try - IN
 * @note  The mutex should be locked when iterators are used in a MT environment. 
 */      
      Error_Code  Lock (bool    &the_mutex_is_acquired,
                        bool    just_try = false)
      { // begin
        Method_State_Block_Begin(1)
          State(1)  
            the_method_error = this->map_mutex.Lock(the_mutex_is_acquired, just_try);
           End_State
        End_Method_State_Block

        return the_method_error.Get_Error_Code();    
      } // Lock              
  
/**
 * \brief Unlock the map_mutex member
 * @param the_mutex_is_acquired - IN / OUT
 */       
      Error_Code  Unlock (bool    &the_mutex_is_acquired)
      { // begin
        Method_State_Block_Begin(1)
          State(1)  
            the_method_error = this->map_mutex.Unlock(the_mutex_is_acquired);    
           End_State
        End_Method_State_Block

        return the_method_error.Get_Error_Code();    
      } // Unlock   
      
/**
 * @brief Retrieve the Iterator \b begin position.
 * @param the_iterator - OUT - if No_Error, he Iterator begin position, otherwise the iterator \b end.
 * @return No_Error, B_Not_Locked
 * @note The instance \b Mutex must be acquired with Lock \b before calling this method.
 */     
      Error_Code  Begin(Iterator  &the_iterator)
      { // begin
        the_iterator = this->map.end();
        
        Method_State_Block_Begin(1)
          State(1)
            if (this->map_mutex.Is_My_Lock() != true)
              the_method_error = A4_Error (The_Module_ID, B_Not_Locked, "Thread safety issue - Cannot Begin this map iterator because no lock is held.");        
            else the_iterator = this->map.begin();
          End_State
        End_Method_State_Block  
                  
        return the_method_error.Get_Error_Code(); 
      } // Begin 
      
/**
 * @brief Return the map end() iterator - no mutex testing is done here.
 * @return end()
 */      
      Iterator  End(void)
      { // begin
        return this->map.end();
      } // End
      
    private: // data
      A4_Lib::Recursive_Mutex map_mutex; /**< recursive mutex allowing thread-safe access to the map instance */

      Map_Type  map; /**< private unordered map instance */
      
    public: // errors 
      enum Unordered_Map_Errors
      { // begin
        I_Insert_Failure        = The_Error_Offset + 0, /**< The new info instance was not inserted into this unordered map. Did the key already exist? */
        E_Erase_Unsuccessful    = The_Error_Offset + 1, /**< The key was not removed from the map. */
        B_Not_Locked            = The_Error_Offset + 2, /**< Thread safety issue - Cannot Begin this map iterator because no lock is held. */
      }; // Unordered_Map_Errors
  }; // Unordered_Map_T (declaration)
} // namespace A4_Lib
#endif // __A4_Unordered_Map_T

