#ifndef __A4_Message_Block_Defined__
#define __A4_Message_Block_Defined__
/**
 * @brief   Simple message block allows data sharing via Message_Queue, Observable Notifications, etc.
 * @author  a. zippay * 2017..2020
 * @file A4_Message_Block.hh
 * @note  * The MIT License
 *
 * Copyright 2020 albert zippay
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

#include "A4_Lib_Module_ID.hh"
#include <memory>
#include <deque>

namespace A4_Lib
{ // begin
  typedef class Message_Block
  { // begin
  public: // construction
    Message_Block(void) = default;
    virtual ~Message_Block(void);
    
  public: // types
    typedef std::shared_ptr<A4_Lib::Message_Block>   Pointer;
    typedef std::size_t                              Vector_Offset;
    typedef std::deque<Message_Block::Pointer>       Deque;
    typedef std::vector<std::shared_ptr<void>>       Data_Vector;
    typedef std::vector<std::size_t>                 Data_Size_Vector;
    
  public: // methods

    static Error_Code  Allocate (Message_Block::Pointer   &the_new_instance);    
    
   // c++11 share_ptr's are passed here  
    Error_Code Set_Data (std::shared_ptr<void>  the_data,
                         Vector_Offset          the_vector_offset = 0);
    
    Error_Code Get_Data (std::shared_ptr<void>  &the_data,
                         Vector_Offset          the_vector_offset = 0);

    Error_Code Set_Data (std::wstring   the_string,
                         Vector_Offset  the_vector_offset = 0);
    
    Error_Code Get_Data (std::wstring   &the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Set_Data (std::string   the_string,
                         Vector_Offset  the_vector_offset = 0);
    
    Error_Code Get_Data (std::string    &the_data,
                         Vector_Offset  the_vector_offset = 0); 
    
    Error_Code Get_Data (std::uint8_t   &the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Set_Data (std::uint8_t   the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Get_Data (std::uint16_t  &the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Set_Data (std::uint16_t  the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Get_Data (std::uint32_t  &the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Set_Data (std::uint32_t  the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Get_Data (std::uint64_t  &the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Set_Data (std::uint64_t  the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Get_Data (double         &the_data,
                         Vector_Offset  the_vector_offset = 0);
    
    Error_Code Set_Data (double         the_data,
                         Vector_Offset  the_vector_offset = 0);

    Error_Code Get_Data (long double    &the_data,
                         Vector_Offset  the_vector_offset = 0);
    
    Error_Code Set_Data (long double    the_data,
                         Vector_Offset  the_vector_offset = 0);  
    
    Error_Code Get_Data (std::time_t    &the_data,
                         Vector_Offset  the_vector_offset = 0);
    
    Error_Code Set_Data (std::time_t    the_data,
                         Vector_Offset  the_vector_offset = 0);
    
// C style pointers passed here...    
    Error_Code  Set_Data (void	         *the_data,
                          Vector_Offset  the_vector_offset,
                          std::size_t    the_data_length);
    
    Error_Code  Get_Data (void		*the_data,
                          Vector_Offset	the_vector_offset,
                          std::size_t	the_max_data_length,
                          std::size_t   &the_number_of_bytes_retrieved);   
    
    bool    Has_Data(void);

    Error_Code  Set_Child_Message_Block (Message_Block::Pointer   the_child);
    Error_Code  Get_Child_Message_Block (Message_Block::Pointer   &the_child);
    
    bool        Child_Is_Set (void);

    std::size_t   Data_Vector_Size (void);

    std::size_t    Data_Length(Vector_Offset  the_vector_offset) const;

  private: // data
    Message_Block::Data_Vector          data_vector; /**< container of shared data pointers */
    
    Message_Block::Data_Size_Vector     data_length_vector; /**< the data length in bytes for each element of this->data_vector */

    Message_Block::Pointer              child; /**< nested message block - for use cases involving aggregated classes */

  public: // errors
    enum Message_Block_Errors
    { // begin
      SD_Invalid_Num_Bytes            = 0,  /**< \b Set_Data: Invalid parameter value - the_max_data_length must be > zero. */
      SD_Invalid_Data_Address         = 1,  /**< \b Set_Data: Invalid parameter address - the_data is NULL */
      GD_Invalid_Data_Address         = 2,  /**< \b Get_Data: Invalid parameter address - the_data is NULL */
      GD_Invalid_Byte_Offset          = 3,  /**< \b Get_Data: Invalid parameter value - the_byte_offset is larger than this->data_length. */
      GD_Invalid_Num_Bytes            = 4,  /**< \b Get_Data: Invalid parameter value - the_max_data_length is smaller than this->data_length - your buffer is too small. */
      GD_Invalid_Data_Length          = 5,  /**< \b Get_Data: The data length at vector offset X is zero. This means the data stored was passed as a std::shared_ptr and must be retrieved the same way. */
      SD_Invalid_Data_Address2        = 6,  /**< \b Set_Data (share_ptr): Invalid parameter address - the_data == nullptr */
      SD_Allocation_Error             = 7,  /**< \b Set_Data (c-ptr):Memory allocation error - could not allocate X bytes for the new shared pointer. */
      GD_Invalid_Offset               = 8,  /**< \b Get_Data: Invalid parameter value - the_vector_offset X must be less than Y */
      GCMB_Child_Not_Set              = 9,  /**< \b Get_Child_Message_Block: No child message block has been Set, so none to Get. */
      GCMB_Invalid_Address            = 10, /**< \b Get_Child_Message_Block: Invalid parameter state - the_child must equal nullptr - check for memory issue. */
      A_Allocation_Error              = 11, /**< \b Allocate: Memory allocation error - could not allocate a new Message_Block instance. */
      SCMB_Invalid_Child              = 12, /**< \b Set_Child_Message_Block: Invalid parameter value - the_child == nullptr */
      SCMB_Child_Already_Set          = 13, /**< \b Set_Child_Message_Block: The child message block has already been set. This method is not recursive! */
      A_Invalid_Parameter_State       = 14, /**< \b Allocate: Invalid parameter state - the_new_instance != nullptr, indicating a possible memory leak. */
      GD_No_Data                      = 15, /**< \b Get_Data: This instance does not have any data. */
      SD_Empty_String                 = 16, /**< \b Set_Data (wstring): Invalid parameter length - the_string is empty. */
      GDS_Invalid_Byte_Offset         = 17, /**< \b Get_Data (wstring): Invalid parameter value - the_byte_offset is larger than this->data_length */
      GD_Allocation_Error             = 18, /**< \b Get_Data (wstring): Memory allocation error - could not allocate a new string buffer. */
      GDS_Invalid_Bytes_Retrieved     = 19, /**< \b Get_Data (wstring): Invalid number of bytes retrieved X when Y were expected. */
      GDT_Incorrect_Bytes_Retrieved   = 20, /**< \b Get_Data_T: An invalid number of bytes were retrieved X instead of the expected Y bytes. */
      GDAS_Invalid_Bytes_Retrieved    = 21, /**< \b Get_Data (string): Invalid number of bytes retrieved (%lld) when %lld were expected. */
      GDAS_Allocation_Error           = 22, /**< \b Get_Data (string): Memory allocation error - could not allocate a new string buffer. */
      SDAS_Empty_String               = 23, /**< \b Set_Data (string): Invalid parameter length - the_string is empty. */
      GDAS_Invalid_Byte_Offset        = 24, /**< \b Get_Data (string): Invalid parameter value - the_byte_offset is larger than this->data_length */
    }; // Message_Block_Errors
  }Message_Block;
  
} // end namespace A4_Lib

#endif // __A4_Message_Block_Defined__
  
