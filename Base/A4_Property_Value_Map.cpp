 /**
* \author Albert Zippay * 2020
* \copyright Albert Zippay - 2020
* \file A4_Property_Value_Map.cpp
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
#ifdef A4_Lib_Windows
  #include "Stdafx.h"
#endif

#include "A4_Property_Value_Map.hh"

namespace A4_Lib
{ // begin
  
  namespace Property_Value_Map_Constants
  {
    static const Message_Block::Vector_Offset  Property_Value_Offset = 0;
    static const Message_Block::Vector_Offset  Property_Type_Offset = 1;
    
    static const Error_Offset   Add_UInt8_Error_Offset        = 1000;
    static const Error_Offset   Add_UInt16_Error_Offset       = 2000;
    static const Error_Offset   Add_UInt32_Error_Offset       = 3000;    
    static const Error_Offset   Add_UInt64_Error_Offset       = 4000;   
    static const Error_Offset   Add_Double_Error_Offset       = 5000;  
    static const Error_Offset   Add_Long_Double_Error_Offset  = 6000; 
    static const Error_Offset   Add_AString_Error_Offset      = 7000;
    static const Error_Offset   Add_WString_Error_Offset      = 8000;   
    
    static const Error_Offset   Get_UInt8_Error_Offset        = 9000;
    static const Error_Offset   Get_UInt16_Error_Offset       = 10000;
    static const Error_Offset   Get_UInt32_Error_Offset       = 11000;    
    static const Error_Offset   Get_UInt64_Error_Offset       = 12000;   
    static const Error_Offset   Get_Double_Error_Offset       = 13000;  
    static const Error_Offset   Get_Long_Double_Error_Offset  = 14000; 
    static const Error_Offset   Get_AString_Error_Offset      = 15000;
    static const Error_Offset   Get_WString_Error_Offset      = 16000;      
  } // namespace Property_Value_Map_Constants

  
  /**
   * \brief default destructor
   */
  Property_Value_Map::~Property_Value_Map(void)
  { // begin
    A4_Cleanup_Begin
      (void) this->property_map.Reset_All();
    A4_End_Cleanup 
  } // destructor
  
/**
 * \brief Template of the Add_Property method - allows all supported data types to be instantiated.
 * @tparam the_property_name - IN
 * @tparam the_property_value - IN
 * @tparam the_data_type - IN
 * @tparam the_class_instance - IN
 * @tparam the_pv_map - IN/OUT - an additional pair will be inserted when successful.
 * @return No_Error, AP_Invalid_Property_Name_Length + The_Error_Offset, AP_Already_Exists + The_Error_Offset
 */
  template <typename      The_Class_Name,
            typename      The_Property_Value_Type,
            Error_Offset  The_Error_Offset> 
  Error_Code  Add_Property_T (std::string                   the_property_name,
                              The_Property_Value_Type       the_property_value,
                              Property_Value_Map::Data_Type the_data_type,
                              The_Class_Name                &the_class_instance,
                              Property_Value_Map::PV_Map    &the_pv_map)
  { // begin
    A4_Lib::Message_Block::Pointer  the_msg_block;
    
    Method_State_Block_Begin(5)    
      State(1)
        if (the_property_name.length() < 1)
          the_method_error = A4_Error (A4_Property_Value_Map_Module_ID, Property_Value_Map::AP_Invalid_Property_Name_Length + The_Error_Offset, "Invalid parameter length - the_property_name is empty.");
      End_State
            
      State(2)
        if (the_class_instance.Exists(the_property_name) == true)
          the_method_error = A4_Error (A4_Property_Value_Map_Module_ID, Property_Value_Map::AP_Already_Exists + The_Error_Offset, A4_Lib::Logging::Error,
                                       "Property '%s' already exists.", the_property_name.c_str());
        else the_method_error = A4_Lib::Message_Block::Allocate (the_msg_block);
      End_State
            
      State(3)
        the_method_error = the_msg_block->Set_Data (the_property_value, Property_Value_Map_Constants::Property_Value_Offset);          
      End_State
              
      State(4)
        the_method_error = the_msg_block->Set_Data (static_cast<std::uint8_t>(the_data_type), Property_Value_Map_Constants::Property_Type_Offset); 
      End_State
            
      State(5)
        the_method_error = the_pv_map.Insert(the_property_name, the_msg_block);
      End_State
    End_Method_State_Block

    A4_Cleanup_Begin
      if (the_msg_block != nullptr)
        the_msg_block.reset();       
    A4_End_Cleanup
              
    return the_method_error.Get_Error_Code();            
  } // Add_Property_T

  /**
   * \brief Add a std::uint8_t property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  std::uint8_t the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, std::uint8_t, Property_Value_Map_Constants::Add_UInt8_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt8, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property
    
  /**
   * \brief Add a std::uint16_t property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  std::uint16_t the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, std::uint16_t, Property_Value_Map_Constants::Add_UInt16_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt16, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property
  
  /**
   * \brief Add a std::uint32_t property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */  
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  std::uint32_t the_property_value)    
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, std::uint32_t, Property_Value_Map_Constants::Add_UInt32_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt32, *this, this->property_map);            
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property
  
  /**
   * \brief Add a std::uint64_t property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */   
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  std::uint64_t the_property_value)  
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, std::uint64_t, Property_Value_Map_Constants::Add_UInt64_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt64, *this, this->property_map);            
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property
  
  /**
   * \brief Add a double property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  double        the_property_value) 
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, double, Property_Value_Map_Constants::Add_Double_Error_Offset>(the_property_name, the_property_value, Data_Type::Double, *this, this->property_map);            
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property
  
   /**
   * \brief Add a long double property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */ 
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  long double   the_property_value) 
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, long double, Property_Value_Map_Constants::Add_Long_Double_Error_Offset>(the_property_name, the_property_value, Data_Type::Long_Double, *this, this->property_map);            
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property      
  
  /**
   * \brief Add a std::string property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  std::string   the_property_value)     
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, std::string, Property_Value_Map_Constants::Add_AString_Error_Offset>(the_property_name, the_property_value, Data_Type::A_String, *this, this->property_map);            
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property
  
  /**
   * \brief Add a std::wstring property value
   * @param the_property_name - IN
   * @param the_property_value - IN
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Add_Property (std::string   the_property_name,
                                                  std::wstring  the_property_value) 
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Add_Property_T<Property_Value_Map, std::wstring, Property_Value_Map_Constants::Add_WString_Error_Offset>(the_property_name, the_property_value, Data_Type::W_String, *this, this->property_map);            
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Add_Property
  
  /**
   * \brief 
   * @param the_property_name - IN - it must exist in the property map
   * @return No_Error, RP_Invalid_Property_Name_Len
   */
  Error_Code    Property_Value_Map::Remove_Property (std::string  the_property_name)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        if (the_property_name.length() < 1)
          the_method_error = A4_Error (A4_Property_Value_Map_Module_ID, RP_Invalid_Property_Name_Len, "Invalid parameter length - the_property_name is empty.");
        else the_method_error = this->property_map.Erase(the_property_name);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } // Remove_Property      

/**
 * \brief test whether the_property_name exists in  this->property_map
 * @param the_property_name - IN
 * @return true if the_property_name is found
 */      
  bool Property_Value_Map::Exists (std::string  the_property_name)
  { // begin
    return this->property_map.Exists (the_property_name);
  } // Exists
  
  
  template <typename      The_Class_Name,
            typename      The_Property_Value_Type,
            Error_Offset  The_Error_Offset> 
  Error_Code  Get_Value_T (std::string                   the_property_name,
                           The_Property_Value_Type       &the_property_value,
                           Property_Value_Map::Data_Type the_data_type,
                           The_Class_Name                &the_class_instance,
                           Property_Value_Map::PV_Map    &the_pv_map)
  { // begin
    A4_Lib::Message_Block::Pointer  the_msg_block;
    
    std::uint8_t                    the_stored_data_type = 0;
    
    bool                            is_found = false;
    
    Method_State_Block_Begin(3)    
      State(1)  
        if (the_property_name.length() < 1) 
          the_method_error = A4_Error (A4_Property_Value_Map_Module_ID, Property_Value_Map::GVT_Invalid_Name_Length + The_Error_Offset, "Invalid parameter length - the_property_name is empty.");
        else the_method_error = the_pv_map.Find(the_property_name, the_msg_block, is_found);
      End_State
            
      State(2)
        if (is_found != true)
          the_method_error = A4_Error (A4_Property_Value_Map_Module_ID, Property_Value_Map::GVT_Not_Found + The_Error_Offset, A4_Lib::Logging::Error,
                                       "Property '%s' was not found.", the_property_name.c_str());
        else the_method_error = the_msg_block->Get_Data (the_stored_data_type, Property_Value_Map_Constants::Property_Type_Offset);
      End_State
              
      State(3)
        if (the_stored_data_type == static_cast <std::uint8_t>(the_data_type))
          the_method_error = the_msg_block->Get_Data (the_property_value, Property_Value_Map_Constants::Property_Value_Offset);
        else the_method_error = A4_Error (A4_Property_Value_Map_Module_ID, Property_Value_Map::GVT_Data_Type_Mismatch + The_Error_Offset, A4_Lib::Logging::Error,
                                          "Data Type mismatch - was expecting %d but was passed the_data_type = %d", static_cast<int>(the_stored_data_type), static_cast<int>(the_data_type));
      End_State
    End_Method_State_Block

    A4_Cleanup_Begin
      if (the_msg_block != nullptr)
        the_msg_block.reset();       
    A4_End_Cleanup
              
    return the_method_error.Get_Error_Code();            
  } //  Get_Value_T
  
  /**
   * \brief Get std::uint8_t property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                                std::uint8_t  &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, std::uint8_t, Property_Value_Map_Constants::Get_UInt8_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt8, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (std::uint8_t)
   
  /**
   * \brief Get std::uint16_t property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                                std::uint16_t  &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, std::uint16_t, Property_Value_Map_Constants::Get_UInt16_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt16, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (std::uint16_t)
   
  /**
   * \brief Get std::uint32_t property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                                std::uint32_t  &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, std::uint32_t, Property_Value_Map_Constants::Get_UInt32_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt32, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (std::uint32_t)  
  
  /**
   * \brief Get std::uint64_t property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                                std::uint64_t  &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, std::uint64_t, Property_Value_Map_Constants::Get_UInt64_Error_Offset>(the_property_name, the_property_value, Data_Type::UInt64, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (std::uint64_t)   
  
  /**
   * \brief Get double property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                               double        &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, double, Property_Value_Map_Constants::Get_Double_Error_Offset>(the_property_name, the_property_value, Data_Type::Double, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (double)   
  
  /**
   * \brief Get long double property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                               long double   &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, long double, Property_Value_Map_Constants::Get_Long_Double_Error_Offset>(the_property_name, the_property_value, Data_Type::Long_Double, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (long double)   
  
  /**
   * \brief Get std::string property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                               std::string   &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, std::string, Property_Value_Map_Constants::Get_AString_Error_Offset>(the_property_name, the_property_value, Data_Type::A_String, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (std::string)   
  
  /**
   * \brief Get std::wstring  property value
   * @param the_property_name - IN
   * @param the_property_value - OUT
   * @return No_Error when successful
   */
  Error_Code    Property_Value_Map::Get_Value (std::string   the_property_name,
                                               std::wstring  &the_property_value)
  { // begin
    Method_State_Block_Begin(1)    
      State(1)
        the_method_error = Get_Value_T<Property_Value_Map, std::wstring, Property_Value_Map_Constants::Get_WString_Error_Offset>(the_property_name, the_property_value, Data_Type::W_String, *this, this->property_map);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();            
  } //  Get_Value (std::wstring)   
} // namespace A4_Lib