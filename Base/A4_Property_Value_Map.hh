#ifndef __A4_Property_Value_Map_Defined__
#define __A4_Property_Value_Map_Defined__
/**
* \author Albert Zippay * 2020
* \copyright Albert Zippay - 2020
* \file A4_Property_Value_Map.hh
*
* \brief  Binds a Property Name (string) with a Value and value-type.
* Copyright (c) 1995...2020, Albert Zippay
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

#include "A4_Unordered_Map_T.hh"
#include "A4_Message_Block.hh"

namespace A4_Lib
{ // begin

  //class Property_Value_Map_Internal;
  
  typedef class Property_Value_Map
  { // begin
    public: // construction
      Property_Value_Map(void) = default; 
      virtual ~Property_Value_Map(void);
      
    public: // types & constants
      static const  Error_Offset  Property_Map_Error_Offset = 100;
      
      enum Data_Type
      { // begin
        Undefined     = 0,
        UInt8         = 1,
        UInt16        = 2,
        UInt32        = 3,
        UInt64        = 4,
        Double        = 5,
        Long_Double   = 6,
        A_String      = 7,
        W_String      = 8
      }; // Data_Type
           
      typedef Unordered_Map_T<std::string, A4_Lib::Message_Block::Pointer, A4_Property_Value_Map_Module_ID, Property_Map_Error_Offset>  PV_Map;
      
    public: // methods
      Error_Code    Add_Property (std::string  the_property_name,
                                  std::uint8_t the_property_value);

      Error_Code    Add_Property (std::string   the_property_name,
                                  std::uint16_t the_property_value);

      Error_Code    Add_Property (std::string   the_property_name,
                                  std::uint32_t the_property_value);    
  
      Error_Code    Add_Property (std::string   the_property_name,
                                  std::uint64_t the_property_value);  
      
      Error_Code    Add_Property (std::string   the_property_name,
                                  double        the_property_value); 
      
      Error_Code    Add_Property (std::string   the_property_name,
                                  long double   the_property_value); 
      
      Error_Code    Add_Property (std::string   the_property_name,
                                  std::string   the_property_value);     
      
      Error_Code    Add_Property (std::string   the_property_name,
                                  std::wstring  the_property_value);  
      
      Error_Code    Get_Value (std::string  the_property_name,
                               std::uint8_t &the_property_value);

      Error_Code    Get_Value (std::string   the_property_name,
                               std::uint16_t &the_property_value);

      Error_Code    Get_Value (std::string   the_property_name,
                               std::uint32_t &the_property_value);    
  
      Error_Code    Get_Value (std::string   the_property_name,
                               std::uint64_t &the_property_value);  
      
      Error_Code    Get_Value (std::string   the_property_name,
                               double        &the_property_value); 
      
      Error_Code    Get_Value (std::string   the_property_name,
                               long double   &the_property_value); 
      
      Error_Code    Get_Value (std::string   the_property_name,
                               std::string   &the_property_value);     
      
      Error_Code    Get_Value (std::string   the_property_name,
                               std::wstring  &the_property_value);        
      
      Error_Code    Remove_Property (std::string  the_property_name);
     
      
      bool          Exists (std::string  the_property_name);
      
    private: // data
      PV_Map  property_map;
      
    public: // errors
      enum Property_Value_Map_Errors
      { // begin
        AP_Invalid_Property_Name_Length     = 0,
        AP_Already_Exists                   = 1,
        RP_Invalid_Property_Name_Len        = 2,
        GVT_Invalid_Name_Length             = 3,
        GVT_Not_Found                       = 4,
        GVT_Data_Type_Mismatch              = 5,
      }; // Property_Value_Map_Errors
  } Property_Value_Map;
  
} // namespace A4_Lib
#endif // __A4_Property_Value_Map_Defined__

