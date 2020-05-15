#ifndef __A4_Utils_Defined__
#define __A4_Utils_Defined__

/**
* \brief  AZ Lib Utility class interface - provides useful utility methods
*
* \author a. zippay * 2013 - 2020 
 * 
 * \note  * The MIT License
 *
 * Copyright 2020 a. zippay
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


#include "A4_Error.hh"

#ifndef DATE
typedef double DATE;
#endif

namespace A4_Lib
{ // begin
  A4_Export Error_Code  SNPrintf (char *buf, std::size_t maxlen, const char *format, ...);
  A4_Export Error_Code  SNPrintf (wchar_t *buf, std::size_t maxlen, const wchar_t *format, ...);

  A4_Export Error_Code  SNPrintf (std::wstring  &the_output_string,
                                  std::wstring  the_format_string, 
                                  std::size_t   the_maximum_length, ...);

  A4_Export  Error_Code  SNPrintf (std::wstring  &the_output_string,
                                   std::wstring  the_format_string, 
                                   std::size_t   the_maximum_length,
                                   va_list       &the_parameter_list);

  A4_Export Error_Code  SNPrintf (std::string  &the_output_string,
                                  std::string  the_format_string, 
                                  std::size_t  the_maximum_length, ...);

  A4_Export  Error_Code  SNPrintf (std::string  &the_output_string,
                                   std::string  the_format_string, 
                                   std::size_t  the_maximum_length,
                                   va_list      &the_parameter_list);
  
  std::uint64_t   Short_String_Hash(std::string     the_string);
  std::uint64_t   Short_String_Hash (std::wstring     the_string);

  Error_Code  Time_T_To_String(std::time_t  the_time_t,
                               std::wstring &the_time_string);

  Error_Code  High_Res_Timestamp_String (std::string &the_time_string);
  
  Error_Code  DATE_To_Time_T (DATE      the_date,	    // in
			      time_t    &the_time_t); // out

  Error_Code  Time_T_To_DATE (time_t  the_time_t,	  // in
			      DATE    &the_date);	  // out

  Error_Code  Parse_CSV_Values (std::string   the_input_string,
				String_Vector &the_value_vector,
				char          the_delimiting_character = ',');

  Error_Code  Parse_CSV_Values (std::wstring   the_input_string,
				WString_Vector &the_value_vector,
				wchar_t        the_delimiting_character = ',');

  Error_Code  Left_Value(std::string the_input_string,
			 std::string &the_output_string);

  Error_Code  Right_Value(std::string the_input_string,
			  std::string &the_output_string);

  std::string & Left_Trim(std::string &the_input_string); // remove leading white space
  std::string & Right_Trim(std::string &the_input_string); // remove trailing white space

  std::wstring & Left_Trim(std::wstring &the_input_string); // remove leading white space
  std::wstring & Right_Trim(std::wstring &the_input_string); // remove trailing white space

  Error_Code  Char_to_WChar (const char   *the_input_string,
			     std::size_t  the_input_length,
			     wchar_t      *&the_output_string);

  Error_Code  WChar_to_Char (const wchar_t  *the_input_string,
			     std::size_t    the_input_length,
			     char           *&the_output_string);

  Error_Code  WChar_to_Char (const std::wstring   the_input_string,
			     std::string          &the_output_string);
                           
  Error_Code  AString_To_WString(std::string   the_input_string,
				 std::wstring  &the_output_string);

  Error_Code  WString_To_AString(std::wstring  the_input_string,
				 std::string   &the_output_string);

  Error_Code  Make_Upper_Case (std::wstring   &the_string);
  Error_Code  Make_Upper_Case (std::string    &the_string);
  Error_Code  Make_Lower_Case (std::wstring   &the_string);
  Error_Code  Make_Lower_Case (std::string    &the_string);

  Error_Code  Wildcard_To_Regex (std::string   &the_wildcard_string, // in - input a dos style wildcard string
                                 std::string   &the_regex_string);   // out - translated to regex string

  Error_Code  Wildcard_To_Regex (std::wstring   &the_wildcard_string, // in - input a dos style wildcard string
                                 std::wstring   &the_regex_string);   // out - translated to regex string

  Error_Code  String_Matches_Wildcard (std::wstring   &the_wildcard_string,
                                       std::wstring   the_string,
                                       bool           &they_match);

  Error_Code  String_Matches_Wildcard (std::string   &the_wildcard_string,
                                       std::string   the_string,
                                       bool          &they_match); 
  
  A4_Export std::time_t   Now(void);

  A4_Export void  Sleep_MS(std::uint64_t   num_milli_seconds);

  bool  Time_Has_Elapsed (A4_Lib::Time_Point  &the_start_point,
			  std::uint64_t	      num_milli_seconds_to_wait);


  Error_Code  Binary_To_Hex_String (std::uint8_t   *the_byte_array,
				    std::size_t    the_byte_array_length,
				    std::string    &the_hex_string);

  Error_Code  Replace (std::string         &the_string, 
                       const std::string   the_string_to_replace, 
                       const std::string   the_replacement_string);

  void Burn(std::wstring &the_string);
  void Burn(std::string &the_string);
  void Burn(A4_Lib::String_Vector &the_vector);
  
  Error_Code  Get_Linux_Computer_Name (std::string  &the_computer_name);
  
//
///////////////////////////////////////////////////////////////   Errors  ///////////////////////////////////////////////////////////////
//
  enum A4_Lib_Errors      
  { // begin
    DTTT_Invalid_Date                   = 0, 
    SNP_Invalid_Maxlen                  = 1,
    SNP_Invalid_Buffer_Address          = 2,
    SNP_Invalid_Format_Address          = 3,
    SNP_Format_Error                    = 4,
    SNPW_Invalid_Maxlen                 = 5,
    SNPW_Invalid_Buffer_Address         = 6,
    SNPW_Invalid_Format_Address         = 7,
    SNPW_Format_Error                   = 8,   
    PCV_Invalid_Delimiter_Char          = 9,
    PCV_Empty_Input_String              = 10,
    LV_Missing_Equal_Sign               = 11,
    RV_Missing_Equal_Sign               = 12,
    CtWC_Invalid_Output_State           = 13,
    CtWC_Invalid_Input_Address          = 14,
    CtWC_Invalid_Input_Length           = 15,
    CtWC_Memory_Allocation_Error        = 16,
    CtWC_Setlocale_Error                = 17,
    CtWC_Conversion_Error               = 18,
    WCtC_Invalid_Output_State           = 19,
    WCtC_Invalid_Input_Address          = 20,
    WCtC_Invalid_Input_Length           = 21,
    WCtC_Memory_Allocation_Error        = 22,
    WCtC_Setlocale_Error                = 23,
    WCtC_Conversion_Error               = 24,  
    WCtC2_Empty_Input_String            = 25,
    WCtC2_No_Output_String              = 26,
    SNPA_Invalid_FormatLength           = 27,
    SNPA_Invalid_Maximum_Length         = 28,
    BTHS_Invalid_Byte_Array_Address	= 29,
    BTHS_Invalid_Byte_Array_Length	= 30,
    BTHS_Allocation_Error		= 31,
    SNPA_Memory_Allocation_Error        = 32,
    SNPA_Format_Error2                  = 33,
    HRTS_localtime_Error		= 34,
    GLCN_FOpen_Error			= 35,
    R_Empty_Input_String                = 36,
    R_Empty_String_To_Replace           = 37,
    R_Empty_Replacement_String          = 38,
    PCV2_Invalid_Delimiter_Char         = 39,
    PCV2_Empty_Input_String             = 40,
    WTRW_Empty_Wildcard_String          = 41,
    WTRA_Empty_Wildcard_String          = 42,
    SMWW_Invalid_Wildcard_String_Len    = 43,
    SMWW_Invalid_String_Len             = 44,
    SNPW_Invalid_FormatLength           = 45,
    SNPW_Invalid_Maximum_Length         = 46,
    SNPW_Memory_Allocation_Error        = 47,
    SNPW_Format_Error2                  = 48,
    TTTS_localtime_s_Error              = 49,
    GLCN_FGets_Error                    = 50,
    GLCN_FClose_Error                   = 51,
    SMWWA_Invalid_Wildcard_String_Len   = 52,
    SMWWA_Invalid_String_Len            = 53,
  }; // A4_Lib_Errors
} // namespace A4_Lib

#endif //__A4_Utils_Defined__
