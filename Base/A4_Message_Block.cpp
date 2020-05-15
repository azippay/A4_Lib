/**
 * @brief   Simple message block passed between threads - replaces the ACE_Message_Block in non-ACE modules
 * @author  albert zippay * 2017..2020
 * @file A4_Message_Block.cpp
 * @note  * The MIT License
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
#ifdef A4_Lib_Windows
#include "Stdafx.h"
#endif

#include "A4_Message_Block.hh" 
#include "A4_Method_State_Block.hh"

using namespace A4_Lib;

namespace Message_Block_Constants
{ // begin
  static const Error_Offset Get_Uint8_Error_Offset  = 100;
  static const Error_Offset Get_Uint16_Error_Offset = 200;
  static const Error_Offset Get_Uint32_Error_Offset = 300;  
  static const Error_Offset Get_Uint64_Error_Offset = 400; 
  static const Error_Offset Get_Double_Error_Offset = 500; 
  static const Error_Offset Get_Long_Double_Error_Offset = 600;   
} // namespace Message_Block_Constants

/**
 * \brief Default destructor
 */
Message_Block::~Message_Block(void)
{ // begin
  A4_Cleanup_Begin
    for (std::size_t offset = 0; offset < this->data_vector.size(); offset++)
      this->data_vector [offset].reset();

    this->data_vector.clear();
    this->data_length_vector.clear();

    if (this->child != nullptr)
      this->child.reset();
  A4_End_Cleanup
} // destructor


/// @brief  Allocate a new Message_Block instance and initialize the maximum size.
/// @param  the_new_instance - IN - must equal nullptr - OUT - the new instance.
//
Error_Code Message_Block::Allocate (Message_Block::Pointer   &the_new_instance)
{ // begin
  Method_State_Block_Begin(2)
    State(1)
      if (the_new_instance != nullptr)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, A_Invalid_Parameter_State, "Invalid parameter state - the_new_instance != nullptr, indicating a possible memory leak.");
    End_State

    State(2)
      the_new_instance = std::make_shared<Message_Block>();
    
      if (the_new_instance == nullptr)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, A_Allocation_Error, "Memory allocation error - could not allocate a new Message_Block instance.");
    End_State
  End_Method_State_Block
 
  return the_method_error.Get_Error_Code();    
} // Allocate    

/// @brief  Set the child message block independent of whatever data needs to be set in this instance
/// @param  the_child - IN - must not be nullptr
//
Error_Code  Message_Block::Set_Child_Message_Block (Message_Block::Pointer   the_child)
{ // begin
  Method_State_Block_Begin(2)
    State(1) 
      if (the_child == nullptr)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SCMB_Invalid_Child, "Invalid parameter value - the_child == nullptr");
    End_State

    State(2)
      if (this->child != nullptr)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SCMB_Child_Already_Set, "The child message block has already been set. This method is not recursive!");
        else this->child = the_child;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();    
} // Set_Child_Message_Block


/// @brief  Get a previously Set child Message_Block
/// @param  the_child - IN - must == nullptr - OUT - the child Pointer.
//
Error_Code  Message_Block::Get_Child_Message_Block (Message_Block::Pointer   &the_child)
{ // begin
  Method_State_Block_Begin(2)
    State(1)
      if (the_child != nullptr)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GCMB_Invalid_Address, "Invalid parameter state - the_child must equal nullptr - check for memory issue.");
    End_State

    State(2)
      if (this->Child_Is_Set() != true)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GCMB_Child_Not_Set, "No child message block has been Set, so none to Get.");
      else the_child = this->child;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code(); 
} // Get_Child_Message_Block

/// @brief  Tests whether the child message block has been set
/// @return true if it has
//
bool        Message_Block::Child_Is_Set (void)
{ // begin
  return this->child != nullptr;
} // Child_Is_Set

/**
 * \brief Retrieve the data length
 * @param the_vector_offset - IN - the zero-based vector offset.
 * @return number of bytes allocated in the block - zero if: 1) the_vector_offset exceeds the vector size. 2) The 
 */
std::size_t    Message_Block::Data_Length(Vector_Offset  the_vector_offset) const
{ // begin
  std::size_t   the_result = 0;
  
  if (the_vector_offset < this->data_length_vector.size())
    the_result = this->data_length_vector[the_vector_offset];
  else (void) App_Log->Write (A4_Lib::Logging::Error, "Invalid parameter value - the_vector_offset (%ld) >= this->data_length_vector (%ld)", the_vector_offset, this->data_length_vector.size());
  
  return the_result;
} // Data_Length

/**
 * Template used to implement the various data flavors
 * @param the_data - OUT
 * @param the_vector_offset - IN
 * @param the_data_vector - IN
 * @param the_data_size_vector - IN
 * @param this_instance - IN
 * @return No_Error, GDT_Incorrect_Bytes_Retrieved
 */
  template <typename      The_Data_Type,
            Error_Offset  The_Error_Offset> 
  Error_Code Get_Data_T (The_Data_Type                     &the_data,
                         Message_Block::Vector_Offset      the_vector_offset,
                         Message_Block::Data_Vector        &the_data_vector,
                         Message_Block::Data_Size_Vector   &the_data_size_vector,
                         Message_Block                     &this_instance)
  { // begin
    std::size_t   the_number_of_bytes_retrieved = 0;

    Method_State_Block_Begin(2)
      State(1) 
        the_data = 0;

        the_method_error = this_instance.Get_Data(&the_data, the_vector_offset, sizeof (The_Data_Type), the_number_of_bytes_retrieved);
      End_State

      State(2)
        if (the_number_of_bytes_retrieved != sizeof (The_Data_Type))
          the_method_error = A4_Error (A4_Message_Block_Module_ID, Message_Block::GDT_Incorrect_Bytes_Retrieved, A4_Lib::Logging::Error,
                                       "An invalid number of bytes were retrieved (%lld) instead of the expected %lld bytes.", the_number_of_bytes_retrieved, sizeof (The_Data_Type));
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();  
  } // Get_Data_T
  
Error_Code Message_Block::Get_Data (std::uint8_t   &the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = Get_Data_T<std::uint8_t, Message_Block_Constants::Get_Uint8_Error_Offset>(the_data, the_vector_offset, this->data_vector, this->data_length_vector, *this);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Get_Data (uint8_t)

Error_Code Message_Block::Set_Data (std::uint8_t   the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = this->Set_Data (&the_data, the_vector_offset, sizeof (std::uint8_t));
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data (uint8_t)


Error_Code Message_Block::Get_Data (std::uint16_t  &the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(2)
    State(1) 
      the_method_error = Get_Data_T<std::uint16_t, Message_Block_Constants::Get_Uint16_Error_Offset>(the_data, the_vector_offset, this->data_vector, this->data_length_vector, *this);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Get_Data (uint16_t)

Error_Code Message_Block::Set_Data (std::uint16_t  the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = this->Set_Data (&the_data, the_vector_offset, sizeof (the_data));
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data (uint16_t)


Error_Code Message_Block::Get_Data (std::uint32_t  &the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = Get_Data_T<std::uint32_t, Message_Block_Constants::Get_Uint32_Error_Offset>(the_data, the_vector_offset, this->data_vector, this->data_length_vector, *this);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Get_Data (uint32_t)

Error_Code Message_Block:: Set_Data (std::uint32_t  the_data,
                                     Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = this->Set_Data (&the_data, the_vector_offset, sizeof (std::uint32_t));
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data (uint32_t)

/**
 * \brief Retrieve a std::uint64_t items of data from the given offset
 * @param the_data - OUT
 * @param the_vector_offset - IN
 * @return No_Error upon success.
 */
Error_Code Message_Block::Get_Data (std::uint64_t  &the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = Get_Data_T<std::uint64_t, Message_Block_Constants::Get_Uint64_Error_Offset>(the_data, the_vector_offset, this->data_vector, this->data_length_vector, *this);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Get_Data (std::uint64_t)

/**
 * \brief Set a std::uint64_t data items into the message block at the given offset
 * @param the_data - IN
 * @param the_vector_offset - IN
 * @return 
 */
Error_Code Message_Block::Set_Data (std::uint64_t  the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = this->Set_Data (&the_data, the_vector_offset, sizeof (std::uint64_t));
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data (uint64_t)

Error_Code Message_Block::Get_Data (double         &the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = Get_Data_T<double, Message_Block_Constants::Get_Double_Error_Offset>(the_data, the_vector_offset, this->data_vector, this->data_length_vector, *this);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Get_Data (double)

/**
 * Set \b double  at the_vector_offset
 * @param the_data - IN
 * @param the_vector_offset - IN
 * @return No_Error upon success.
 */
Error_Code Message_Block::Set_Data (double         the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = this->Set_Data (&the_data, the_vector_offset, sizeof (double));
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data (double)

Error_Code Message_Block::Get_Data (long double    &the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = Get_Data_T<long double, Message_Block_Constants::Get_Long_Double_Error_Offset>(the_data, the_vector_offset, this->data_vector, this->data_length_vector, *this);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Get_Data (long double)
    
/**
 * Set long double  at the_vector_offset
 * @param the_data - IN
 * @param the_vector_offset - IN
 * @return No_Error upon success.
 */
Error_Code Message_Block::Set_Data (long double    the_data,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_method_error = this->Set_Data (&the_data, the_vector_offset, sizeof (long double));
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data (long double)
    
    
/**
 * \brief  Insert an a-string into the message block at a specific offset
 * @param the_string - IN
 * @param the_vector_offset - IN
 * @return No_Error upon success
 */
Error_Code Message_Block::Set_Data (std::string    the_string,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1)  
      if (the_string.length() < 1)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SDAS_Empty_String, "Invalid parameter length - the_string is empty."); // do we need to support the empty string use case?
      else the_method_error = this->Set_Data ((void *) the_string.c_str(), the_vector_offset, sizeof (char) * the_string.length());
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data

/**
 * \brief Retrieve an a-string from the message block from a specific offset
 * @param the_string - OUT
 * @param the_vector_offset - IN
 * @return No_Error, GDAS_Invalid_Byte_Offset, GDAS_Allocation_Error, GDAS_Invalid_Bytes_Retrieved
 */
Error_Code Message_Block::Get_Data (std::string    &the_string,
                                    Vector_Offset  the_vector_offset)
{ // begin
  char          *the_buffer = NULL;

  std::size_t	the_data_length = 0;
  std::size_t   the_number_of_bytes_retrieved = 0;

  Method_State_Block_Begin(4)
    State(1)  
      the_string.clear();

      if (the_vector_offset >= this->data_length_vector.size())
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GDAS_Invalid_Byte_Offset, "Invalid parameter value - the_byte_offset is larger than this->data_length");
    End_State

    State(2)
      the_data_length = this->data_length_vector[the_vector_offset] + sizeof (char);

      the_buffer = new (std::nothrow) char[the_data_length];

      if (the_buffer == NULL)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GDAS_Allocation_Error, "Memory allocation error - could not allocate a new string buffer.");
      else memset (the_buffer, 0, the_data_length);
    End_State

    State(3)
      the_method_error = this->Get_Data (the_buffer, the_vector_offset, the_data_length, the_number_of_bytes_retrieved);
    End_State

    State(4)
      if (the_number_of_bytes_retrieved != (the_data_length - sizeof (char)))
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GDAS_Invalid_Bytes_Retrieved, A4_Lib::Logging::Error,
                                    "Invalid number of bytes retrieved (%lld) when %lld were expected.", the_number_of_bytes_retrieved, the_data_length);
      else the_string = the_buffer;

      delete[] the_buffer;
      the_buffer = NULL;
    End_State
  End_Method_State_Block
    
  A4_Cleanup_Begin
    if (the_buffer != NULL)
    { // free buffer
      delete[] the_buffer;
      the_buffer = NULL;      
    } // if then
  A4_End_Cleanup

  return the_method_error.Get_Error_Code();
} // Get_Data (string)

/**
 * 
 * @param the_string - IN -
 * @param the_vector_offset - IN - zero-based index
 * @return 
 */
Error_Code Message_Block::Set_Data (std::wstring   the_string,
                                    Vector_Offset  the_vector_offset)
{ // begin
  Method_State_Block_Begin(1)
    State(1)  
      if (the_string.length() < 1)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SD_Empty_String, "Invalid parameter length - the_string is empty."); // do we need to support the empty string use case?
      else the_method_error = this->Set_Data ((void *) the_string.c_str(), the_vector_offset, sizeof (wchar_t) * the_string.length());
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();
} // Set_Data (wstring)

/**
 * @brief Get the wstring from the_vector_offset 
 * @param the_string - OUT
 * @param the_vector_offset - IN
 * @return No_Error, GDS_Invalid_Byte_Offset, GDS_Invalid_Bytes_Retrieved
 */
Error_Code Message_Block::Get_Data (std::wstring   &the_string,
                                    Vector_Offset  the_vector_offset)
{ // begin
  wchar_t      *the_buffer = NULL;

  std::size_t	the_data_length = 0;
  std::size_t   the_number_of_bytes_retrieved = 0;

  Method_State_Block_Begin(4)
    State(1)  
      the_string.clear();

      if (the_vector_offset >= this->data_length_vector.size())
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GDS_Invalid_Byte_Offset, "Invalid parameter value - the_byte_offset is larger than this->data_length");
    End_State

    State(2)
      the_data_length = this->data_length_vector[the_vector_offset] + sizeof (wchar_t);

      the_buffer = new (std::nothrow) wchar_t[the_data_length];

      if (the_buffer == NULL)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GD_Allocation_Error, "Memory allocation error - could not allocate a new string buffer.");
      else memset (the_buffer, 0, the_data_length);
    End_State

    State(3)
      the_method_error = this->Get_Data (the_buffer, the_vector_offset, the_data_length, the_number_of_bytes_retrieved);
    End_State

    State(4)
      if (the_number_of_bytes_retrieved != (the_data_length - sizeof (wchar_t)))
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GDS_Invalid_Bytes_Retrieved, A4_Lib::Logging::Error,
                                    "Invalid number of bytes retrieved (%lld) when %lld were expected.", the_number_of_bytes_retrieved, the_data_length);
      else the_string = the_buffer;

      delete[] the_buffer;
      the_buffer = NULL;
    End_State
  End_Method_State_Block
    
  A4_Cleanup_Begin
    if (the_buffer != NULL)
    { // free buffer
      delete[] the_buffer;
      the_buffer = NULL;      
    } // if then
  A4_End_Cleanup

  return the_method_error.Get_Error_Code();
} // Get_Data (wstring)

/**
 * \brief Copy the_data buffer to this->data_vector (share_ptr)
 * @param the_data - IN - the input data address
 * @param the_vector_offset - IN - offset within this->data_vector
 * @param the_data_length - IN - the number of bytes to copy.
 *
 * @todo std::shared_ptr<unsigned char[]>(new unsigned char[the_max_data_length]); is the c++17 syntax...determine the minimum gcc version required for this.
 */
Error_Code  Message_Block::Set_Data (void           *the_data,
                                     Vector_Offset  the_vector_offset,
                                     std::size_t    the_data_length)
{ // begin
  std::shared_ptr<void>   the_new_ptr;
  
  Method_State_Block_Begin(4)
    State(1)
      if (the_data == NULL)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SD_Invalid_Data_Address, "Invalid parameter address - the_data is NULL");
    End_State
    
    State(2)
      if (the_data_length < 1)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SD_Invalid_Num_Bytes, "Invalid parameter value - the_max_data_length must be > zero.");
    End_State
      
    State(3)
      the_new_ptr = std::shared_ptr<void>(new (std::nothrow) std::uint8_t[the_data_length], std::default_delete<std::uint8_t[]>());
    
      if (the_new_ptr == nullptr)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SD_Allocation_Error, A4_Lib::Logging::Error,
                                     "Memory allocation error - could not allocate %ld bytes for the new shared pointer.", the_data_length);
      else memcpy (the_new_ptr.get(), the_data, the_data_length);
    End_State
      
    State(4)
      while (the_vector_offset >= this->data_vector.size())
      { // increase the storage vector sizes
        this->data_vector.push_back(nullptr);
        this->data_length_vector.push_back(0);
      } // while
           
      this->data_vector[the_vector_offset] = the_new_ptr;
      this->data_length_vector[the_vector_offset] = the_data_length;
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();  
} // Set_Data (c-style data address)
    
/**
 * \brief Retrieve the Message_Block data and copies it to the_data buffer. The Message_Block still owns a shared_copy until the destructor is called.
 * @param the_data - IN (pre allocated buffer) - OUT - the copied data
 * @param the_vector_offset - IN - this->data_vector offset
 * @param the_max_data_length - IN - max buffer size - OUT - number of bytes copied.
 * @return No_Error upon success
 */
Error_Code  Message_Block::Get_Data (void	    *the_data,
                                     Vector_Offset  the_vector_offset,
                                     std::size_t    the_max_data_length,
                                     std::size_t    &the_number_of_bytes_retrieved)
{// begin
  Method_State_Block_Begin(4)
    State(1) 
      the_number_of_bytes_retrieved = 0;
  
      if (the_data == NULL)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GD_Invalid_Data_Address, "Invalid parameter address - the_data is NULL");
    End_State
    
    State(2)
      if (the_vector_offset >= this->data_length_vector.size())
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GD_Invalid_Byte_Offset, "Invalid parameter value - the_byte_offset is larger than this->data_length");
    End_State
      
    State(3)
      if (the_max_data_length < this->data_length_vector[the_vector_offset])
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GD_Invalid_Num_Bytes, "Invalid parameter value - the_max_data_length is smaller than this->data_length - your buffer is too small.");
    End_State
      
    State(4)
      if (this->data_length_vector[the_vector_offset] == 0)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GD_Invalid_Data_Length, A4_Lib::Logging::Error,
                                     "The data length at vector offset %ld is zero. This means the data stored was passed as a std::shared_ptr and must be retrieved the same way.", the_vector_offset);
      else { // copy the data
        memcpy (the_data, this->data_vector[the_vector_offset].get(), this->data_length_vector[the_vector_offset]); 
        the_number_of_bytes_retrieved = this->data_length_vector[the_vector_offset];
      } // if else
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();   
} // Get_Data

/**
 * @brief Set the_data into this->data_vector.
 * @param the_data - IN
 * @param the_vector_offset - IN - this->data_vector will increase in size of the_vector_offset + 1
 * @return No_Error, SD_Invalid_Data_Address2
 */
Error_Code Message_Block::Set_Data (std::shared_ptr<void>  the_data,
                                    Vector_Offset          the_vector_offset)
{ // begin
  Method_State_Block_Begin(2)
    State(1)
      if (the_data == nullptr)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, SD_Invalid_Data_Address2, "Invalid parameter address - the_data == nullptr");
    End_State
    
    State(2)
      while (the_vector_offset >= this->data_vector.size())
      { // increase the storage vector sizes
        this->data_vector.push_back(nullptr);
        this->data_length_vector.push_back(0); // <-- data length for shared pointer objects remains zero.
      } // while
           
      this->data_vector[the_vector_offset] = the_data;      
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();     
} // Set_Data
   
/**
 * \brief Test whether there is any data present in the data_vector member
 * @return true if the data_vector size if > zero.
 */
bool    Message_Block::Has_Data(void)
{ // begin
  return this->data_vector.size() > 0;
} // Has_Data

/**
 * @brief Get data saved as a shared_ptr
 * @param the_data - OUT
 * @param the_vector_offset - IN - must be < this->data_vector.size()
 * @return No_Error, GD_No_Data, GD_Invalid_Offset
 */
Error_Code Message_Block::Get_Data (std::shared_ptr<void>  &the_data,
                                    Vector_Offset          the_vector_offset)
{ // begin
  Method_State_Block_Begin(2)
    State(1)
      the_data.reset();
  
      if (this->data_vector.size() < 1)
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GD_No_Data, "This instance does not have any data.");
    End_State

    State(2)
      if (the_vector_offset >= this->data_vector.size())
      { // bad offset
        the_data.reset();
     
        the_method_error = A4_Error (A4_Message_Block_Module_ID, GD_Invalid_Offset, A4_Lib::Logging::Error,
                                     "Invalid parameter value - the_vector_offset (%ld) must be less than %ld", the_vector_offset, this->data_vector.size());
      } // if then
      else the_data = this->data_vector[the_vector_offset];
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();   
} // Get_Data
    
/**
 * @brief Retrieve the size of the internal data_vector member
 * @return the size of the data_vector member - implies that there is data at every offset, but that may not be true.
 */
std::size_t   Message_Block::Data_Vector_Size(void)
{ // begin
  return this->data_vector.size();
} // Data_Vector_Size
