/**
* \brief  This is a collection or useful small functions.
* \author a. zippay * 2014...2018
 * 
 * \note  * The MIT License
 *
 * Copyright 2018 a. zippay
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

#include "A4_Utils.hh"
#include "A4_Method_State_Block.hh"
#include "A4_Lib_Base.hh"

#include <cstdarg>
#include <thread>
#include <algorithm>
#include <sys/stat.h>
#include <cstdio>
#include <regex>
#include <limits.h>

namespace A4_Lib
{ // begin
  /**
   * @brief Wrapper around the localtime_r/localtime_s method
   * @param the_time_t - IN
   * @param the_time_string - OUT
   * @return 
   */
  Error_Code  Time_T_To_String(std::time_t  the_time_t,
                               std::wstring &the_time_string)
  { // begin
    struct tm the_time_info;
    int    the_error = 0;

    Method_State_Block_Begin(1)
      State(1)
        the_time_string.clear();
        memset(&the_time_info, 0, sizeof (the_time_info));

  #ifdef A4_Lib_Windows      
        the_error = localtime_s (&the_time_info, &the_time_t);
  #else

        if (localtime_r(&the_time_t, &the_time_info) == NULL)
         the_error = 1; // 
  #endif
        if (the_error != 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, TTTS_localtime_s_Error, Logging::Error,
                                        "Call to localtime failed with error %d", the_error);
        else the_method_error = SNPrintf(the_time_string, L"%02d-%02d-%d %02d:%02d:%02d", Max_Error_Message_Length, the_time_info.tm_mday, the_time_info.tm_mon + 1, the_time_info.tm_year + 1900,
                                                 the_time_info.tm_hour, the_time_info.tm_min, the_time_info.tm_sec);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();
  } // Time_T_To_String

  /**
   * @brief Convert a time_t value into a struct tm
   * @param the_time_t - IN
   * @param the_time_info - OUT
   * @return No_Error, TTTST_localtime_s_Error
   */
  Error_Code  Time_T_To_Struct_TM (std::time_t  the_time_t,  
                                   struct tm    &the_time_info)  
  { // begin
    int    the_error = 0;
    
    Method_State_Block_Begin(1)
      State(1)
        memset(&the_time_info, 0, sizeof (the_time_info));

  #ifdef A4_Lib_Windows      
        the_error = localtime_s (&the_time_info, &the_time_t);
  #else

        if (localtime_r(&the_time_t, &the_time_info) == NULL)
          the_error = 1; // 
  #endif           
        if (the_error != 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, TTTST_localtime_s_Error, Logging::Error,
                                        "Call to localtime failed with error %d", the_error);       
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();    
  } // Time_T_To_Struct_TM
  
  /**
   * @brief Convert the contents of a struct tm into a std::time_t
   * @param the_time_info - IN
   * @param the_time_t - OUT
   * @todo Determine whether an additional parameter is required to determine whether a timezone needs to be included. There isn't a current use-case that requires this...
   */
  Error_Code  Struct_TM_To_Time_T (struct tm    &the_time_info, // in
                                   std::time_t  the_time_t)
  { // begin
     Method_State_Block_Begin(1)
      State(1)  
         the_time_t = 0;
     
          if ((the_time_info.tm_mday < 1) || (the_time_info.tm_mday > 31))
            the_method_error = A4_Error (A4_Utils_Module_ID, STTT_Invalid_Day, A4_Lib::Logging::Error,
                                         "Invalid parameter state - the_time_info.tm_mday == %d", the_time_info.tm_mday);
        the_time_t = std::mktime (&the_time_info);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();      
  } // Struct_TM_To_Time_T
  
  /**
   * \brief Translate the current high-resolution clock into a human readable string.
   * @param the_time_string - OUT - the human readable timestamp.
   * @return No_Error, HRTS_localtime_Error
   */
  Error_Code  High_Res_Timestamp_String (std::string &the_time_string)
  { // begin
    std::chrono::high_resolution_clock::time_point  the_time_point = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds  num_ms = std::chrono::duration_cast<std::chrono::milliseconds>(the_time_point.time_since_epoch());
    std::chrono::seconds       num_seconds = std::chrono::duration_cast<std::chrono::seconds>(num_ms);

    std::time_t                the_time_t = num_seconds.count ();

    struct tm the_time_info;
    int    the_error = 0;


    Method_State_Block_Begin(1)
      State(1)  
        the_time_string.clear();
        memset(&the_time_info, 0, sizeof (the_time_info));

  #ifdef A4_Lib_Windows      
        the_error = localtime_s (&the_time_info, &the_time_t);
  #else

        if (localtime_r(&the_time_t, &the_time_info) == NULL)
         the_error = 1; // 
  #endif
        if (the_error != 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, HRTS_localtime_Error, Logging::Error,
                                        "Call to localtime failed with error %d", the_error);          
        else the_method_error = SNPrintf(the_time_string, "%02d-%02d-%d %02d:%02d:%02d.%lld", Max_Error_Message_Length, the_time_info.tm_mday, the_time_info.tm_mon + 1, the_time_info.tm_year + 1900,
                                                 the_time_info.tm_hour, the_time_info.tm_min, the_time_info.tm_sec, num_ms.count() % 1000);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();  
  } // High_Res_Timestamp_String


  /**
   * \brief Convert a DATE to time_t
   * @param the_date - IN
   * @param the_time_t - OUT
   * @return 
   */
  static const DATE AZU_Time_t_Const = 2209168800.0; // 

  Error_Code  DATE_To_Time_T (DATE	the_date,     // in
                                      time_t	&the_time_t)  // out
  { // begin
    Method_State_Block_Begin(1)
      State(1)
        the_date *= 86400.0;

        if (the_date < AZU_Time_t_Const)
        { // bad DATE
          the_time_t = 0;

          the_method_error = A4_Error (A4_Utils_Module_ID, DTTT_Invalid_Date, "Invalid parameter value - the_date < min value required for a positive conversion to a time_t");
        } // end if then
        else the_time_t = static_cast<time_t>((the_date * 86400.0) - 2209168800.0);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();
  } // END DATE_To_Time_T



  /**
   * \brief Convert a time_t to DATE
   * @param the_time_t - IN
   * @param the_date - OUT
   * @return 
   */
  Error_Code  Time_T_To_DATE (time_t	the_time_t, // in
                                        DATE	&the_date)  // out
  { // begin
    the_date = (static_cast<DATE>(the_time_t) + 2209168800.0) / 86400.0;

    return No_Error;
  } // END DATE_To_Time_T



  /// @brief   Retrieve the string left of an Equal Sign
  /// @param the_input_string - IN
  /// @param the_output_string - OUT
  //
  Error_Code Left_Value(std::string the_input_string,
                                std::string &the_output_string)
  { // begin
    auto the_offset = the_input_string.find_first_of ("=");

    Method_State_Block_Begin(1)
      State(1)    
        if (the_offset != std::string::npos)
        { // get & trim
          the_output_string = the_input_string.substr(0, the_offset);
          the_output_string = Right_Trim (the_output_string);
        } // if then
        else the_method_error = A4_Error (A4_Utils_Module_ID, LV_Missing_Equal_Sign, "Invalid parameter contents: the_input_string does not contain an Equal Sign");
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();   
  } // Left_Value


  /// @brief Retrieve the Value Right of the Equal sign
  /// @param the_input_string - IN
  /// @param the_output_string - OUT
  //
  Error_Code Right_Value(std::string the_input_string,
                                 std::string &the_output_string)
    { // begin
    auto the_offset = the_input_string.find_first_of ("=");

    Method_State_Block_Begin(2)
      State(1)  
        if (the_offset != std::string::npos)
        { // extract the substring
          the_output_string = the_input_string.substr(the_offset + 1);
          the_output_string = Right_Trim (the_output_string);
        } // if then
        else the_method_error = A4_Error (A4_Utils_Module_ID, RV_Missing_Equal_Sign, "Invalid parameter contents: the_input_string does not contain an Equal Sign");
      End_State

      State(2)
        the_output_string = Left_Trim (the_output_string);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();   
  } // Right_Value


  /// @brief Remove leading white space from the_input_string
  /// @param the_input_string - IN/OUT
  //
  std::string & Left_Trim(std::string &the_input_string)
  { // begin
    auto the_iterator =  std::find_if (the_input_string.begin(), the_input_string.end(), [](char ch){ return !A4_isspace(ch); });
    the_input_string.erase( the_input_string.begin() , the_iterator);

    return the_input_string;   
  } // Left_Trim


  /// @brief (wstring) Remove leading white space from the_input_string
  /// @param the_input_string - IN/OUT
  //
  std::wstring & Left_Trim(std::wstring &the_input_string)
  { // begin
    auto the_iterator =  std::find_if (the_input_string.begin(), the_input_string.end(), [](wchar_t ch){ return !A4_isspace(ch); });
    the_input_string.erase( the_input_string.begin() , the_iterator);

    return the_input_string;   
  } // Left_Trim

  /// @brief Remove trailing white space from the_input_string
  /// @param the_input_string - IN/OUT
  //
  std::string & Right_Trim(std::string &the_input_string)
  { // begin
    auto the_iterator =  std::find_if (the_input_string.rbegin(), the_input_string.rend(), [](char ch){ return !A4_isspace(ch); });
    the_input_string.erase( the_iterator.base() , the_input_string.end());

    return the_input_string;   
  } // Right_Trim



  /// @brief Remove trailing white space from the_input_string
  /// @param the_input_string - IN/OUT
  //
  std::wstring & Right_Trim(std::wstring &the_input_string)
  { // begin
    auto the_iterator =  std::find_if (the_input_string.rbegin(), the_input_string.rend(), [](wchar_t ch){ return !A4_isspace(ch); });
    the_input_string.erase( the_iterator.base() , the_input_string.end());

    return the_input_string;   
  } // Right_Trim

  /// @brief  Make the_string upper case
  /// \param the_string - in / OUT
  //
  Error_Code  Make_Upper_Case (std::wstring    &the_string)
  { //begin
    std::transform(the_string.begin(), the_string.end(), the_string.begin(), ::toupper);

    return No_Error;
  } // Make_Upper_Case


  /**
  * \brief  Translate * and ? wildcard string into Regex expressions.
  * \param  the_wildcard_string - IN - the search string with wildcards
  * \param  the_regex_string - OUT - the search string in Regex form
  */
  Error_Code  Wildcard_To_Regex (std::string   &the_wildcard_string, // in - input a dos style wildcard string
                                 std::string   &the_regex_string)   // out - translated to regex string
  { // begin
      Method_State_Block_Begin(2)
        State(1)
          the_regex_string.clear();

          if (the_wildcard_string.length() < 1)
            the_method_error = A4_Error (A4_Utils_Module_ID, WTRA_Empty_Wildcard_String, "Invalid parameter length - the_wildcard_string is empty.");
        End_State

        State(2)  
          the_regex_string = "^";

          for (std::size_t the_offset = 0; the_offset < the_wildcard_string.length(); the_offset++)
            if (the_wildcard_string[the_offset] == '*')
              the_regex_string += ".*";
            else if (the_wildcard_string[the_offset] == '?')
                  the_regex_string += ".";
                 else the_regex_string += the_wildcard_string[the_offset];

           the_regex_string += "$";
        End_State
      End_Method_State_Block

      return the_method_error.Get_Error_Code();
  } // Wildcard_To_Regex

  /**
  * \brief  Translate (wstring) * and ? wildcard string into Regex expressions.
  * \param  the_wildcard_string - IN - the search string with wildcards
  * \param  the_regex_string - OUT - the search string in Regex form
  */
  Error_Code  Wildcard_To_Regex (std::wstring   &the_wildcard_string, // in - input a dos style wildcard string
                                 std::wstring   &the_regex_string)   // out - translated to regex string
  { // begin
      Method_State_Block_Begin(2)
        State(1)
          the_regex_string.clear();

          if (the_wildcard_string.length() < 1)
            the_method_error = A4_Error (A4_Utils_Module_ID, WTRW_Empty_Wildcard_String, "Invalid parameter length - the_wildcard_string is empty.");
        End_State

        State(2)  
          the_regex_string = L"^";

          for (std::size_t the_offset = 0; the_offset < the_wildcard_string.length(); the_offset++)
            if (the_wildcard_string[the_offset] == static_cast<wchar_t>('*'))
              the_regex_string += L".*";
            else if (the_wildcard_string[the_offset] == static_cast<wchar_t>('?'))
                  the_regex_string += L".";
                 else the_regex_string += the_wildcard_string[the_offset];

           the_regex_string += L"$";
        End_State
      End_Method_State_Block

      return the_method_error.Get_Error_Code();
  } // Wildcard_To_Regex

  /**
  * \brief  test whether a string matches a dos-style wildcard string. The test is case-insensitive
  * \param  the_wildcard_string - IN - must have some length > zero
  * \param  the_string - IN - must also have some length > zero
  * \param  they_match - OUT
  */
    Error_Code  String_Matches_Wildcard (std::wstring   &the_wildcard_string,
                                         std::wstring   the_string,
                                         bool           &they_match)
    { // begin
      std::wstring  the_regex_string;

      std::wsmatch  the_search_result;

      Method_State_Block_Begin(3)
        State(1)
          they_match = false;

          if (the_wildcard_string.length() < 1)
            the_method_error = A4_Error (A4_Utils_Module_ID, SMWW_Invalid_Wildcard_String_Len, "Invalid parameter length - the_wildcard_string is empty.");
        End_State

        State(2)
          if (the_string.length() < 1)
            the_method_error = A4_Error (A4_Utils_Module_ID, SMWW_Invalid_String_Len, "Invalid parameter length - the_string is empty.");
          else the_method_error = Wildcard_To_Regex (the_wildcard_string, the_regex_string);
        End_State

        State(3)
          std::wregex      the_regex(the_regex_string, std::regex_constants::ECMAScript | std::regex_constants::icase); // todo find another way of assigning filters and flags without the constructor

          they_match = std::regex_search (the_string, the_search_result, the_regex);
        End_State
      End_Method_State_Block

      return the_method_error.Get_Error_Code();
    } // Name_Matches_Wildcard (wstring)

    /**
  * \brief  test whether a string matches a dos-style wildcard string. The test is case-insensitive
  * \param  the_wildcard_string - IN - must have some length > zero
  * \param  the_string - IN - must also have some length > zero
  * \param  they_match - OUT
  */
    Error_Code  String_Matches_Wildcard (std::string   &the_wildcard_string,
                                         std::string   the_string,
                                         bool          &they_match)
    { // begin
      std::string  the_regex_string;

      std::smatch  the_search_result;

      Method_State_Block_Begin(3)
        State(1)
          they_match = false;

          if (the_wildcard_string.length() < 1)
            the_method_error = A4_Error (A4_Utils_Module_ID, SMWWA_Invalid_Wildcard_String_Len, "Invalid parameter length - the_wildcard_string is empty.");
        End_State

        State(2)
          if (the_string.length() < 1)
            the_method_error = A4_Error (A4_Utils_Module_ID, SMWWA_Invalid_String_Len, "Invalid parameter length - the_string is empty.");
          else the_method_error = Wildcard_To_Regex (the_wildcard_string, the_regex_string);
        End_State

        State(3)
          std::regex      the_regex(the_regex_string, std::regex_constants::ECMAScript | std::regex_constants::icase); 

          they_match = std::regex_search (the_string, the_search_result, the_regex);
        End_State
      End_Method_State_Block

      return the_method_error.Get_Error_Code();
    } // Name_Matches_Wildcard (string)

  /**
   * \brief Transform the_string to upper-case 
   * @param the_string - IN - mixed case string - OUT - Upper Case string
   * @return No_Error
   */
  Error_Code  Make_Upper_Case (std::string    &the_string)
  { //begin

    std::transform(the_string.begin(), the_string.end(),the_string.begin(), ::toupper);

    return No_Error;
  } // Make_Upper_Case

  Error_Code  Make_Lower_Case (std::wstring    &the_string)
  { //begin

    std::transform(the_string.begin(), the_string.end(),the_string.begin(), ::tolower);

    return No_Error;
  } // Make_Lower_Case

  /**
   * \brief Transform the_string to lower-case 
   * @param the_string - IN - mixed case string - OUT - Lower Case string
   * @return No_Error
   */
  Error_Code  Make_Lower_Case (std::string    &the_string)
  { //begin

    std::transform(the_string.begin(), the_string.end(),the_string.begin(), ::tolower);

    return No_Error;
  } // Make_Lower_Case

  /// @brief A simple hash function for short strings that have a tendency to collide with other hash functions.
  /// @param the_string - in
  /// @return the hash
  //
  std::uint64_t   Short_String_Hash(std::string     the_string)
  { // begin
    std::uint64_t    the_hash = 0;
    std::size_t      the_offset = 0;

    Method_State_Block_Begin(1)
      State(1)  
        for (the_offset = 0; the_offset < the_string.length(); the_offset++)
          the_hash += ((the_hash * 100) + static_cast<std::uint64_t>(the_string [the_offset]));
      End_State
    End_Method_State_Block

    return the_hash;
  } // Short_String_Hash

  /// @brief A simple hash function for short strings that have a tendency to collide with other hash functions.
  /// @param the_string - in
  /// @return the hash
  //
  std::uint64_t   Short_String_Hash (std::wstring     the_string)
  { // begin
    std::string	  the_narrow_string;

    std::uint64_t   the_hash = 0;

    Method_State_Block_Begin(2)
      State (1)
        the_method_error = WChar_to_Char (the_string, the_narrow_string);
      End_State

      State(2)
        the_hash = Short_String_Hash(the_narrow_string);
      End_State
    End_Method_State_Block

    return the_hash;
  } // Short_String_Hash (wstring)

  /// @brief   std__snprintf wrapper - 
  /// @param the_buffer - IN / OUT - must be large enough to contain the formatted char string
  /// @param the_maxlen - IN
  /// @param the_format - IN
  /// @param ... - IN - variable length list of arguments
  /// @return No_Error should all go well
  //
  Error_Code  SNPrintf (char          *the_buffer, 
                        std::size_t   the_maxlen, 
                        const char    *the_format, 
                        ...)
  { // begin
    va_list the_va_list;

    Method_State_Block_Begin(4)
      State(1)
        if (the_buffer == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNP_Invalid_Buffer_Address, "Invalid parameter value - the_buffer is NULL");
      End_State

      State(2)
        if (the_maxlen < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNP_Invalid_Maxlen, "Invalid parameter value - the_maxlen");
      End_State

      State(3)
        if (the_format == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNP_Invalid_Format_Address, "Invalid parameter value - the_format is NULL");
      End_State

      State(4)  
        va_start (the_va_list, the_format);

        if (std::vsnprintf (the_buffer, the_maxlen, the_format, the_va_list) < 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNP_Format_Error, "The call to std::vsnprintf failed");

        va_end (the_va_list);    
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();  
  } // SNPrintf - char

  /// @brief   std__snprintf wrapper - 
  /// @param the_buffer - IN / OUT - must be large enough to contain the formatted char string
  /// @param the_maxlen - IN
  /// @param the_format - IN
  /// @param ... - IN - variable number of arguments for the_format_string 
  /// @return No_Error should all go well
  //
  Error_Code  SNPrintf (wchar_t       *the_buffer, 
                                std::size_t   the_maxlen, 
                                const wchar_t *the_format, 
                                ...)
  { // begin
    va_list the_va_list;

    Method_State_Block_Begin(4)
      State(1)
        if (the_buffer == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Invalid_Buffer_Address, "Invalid parameter value - the_buffer is NULL");
      End_State

      State(2)
        if (the_maxlen < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Invalid_Maxlen, "Invalid parameter value - the_maxlen");
      End_State

      State(3)
        if (the_format == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Invalid_Format_Address, "Invalid parameter value - the_format is NULL");
      End_State

      State(4)  
        va_start (the_va_list, the_format);

        if (std::vswprintf (the_buffer, the_maxlen, the_format, the_va_list) < 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Format_Error, "The call to std::vswprintf failed");

        va_end (the_va_list);    
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();  
  } // SNPrintf -

  /**
   * 
   * @param the_output_string - OUT
   * @param the_format_string - IN
   * @param the_maximum_length - IN
   * @param ... - IN - variable number of arguments for the_format_string 
   * @return No_Error upon success
   */
  Error_Code  SNPrintf (std::wstring  &the_output_string,
                                std::wstring  the_format_string, 
                                std::size_t   the_maximum_length, ...)
  { // begin
    va_list the_va_list;

    Method_State_Block_Begin(1)
      State(1)
        va_start (the_va_list, the_maximum_length);

        the_method_error = SNPrintf (the_output_string, the_format_string, the_maximum_length, the_va_list);

        va_end (the_va_list);  
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();  
  } // SNPrintf (wstring, ...)

  /**
   * @brief format a std::wstring using a variable argument list
   * @param the_output_string - OUT
   * @param the_format_string - IN
   * @param the_maximum_length - IN
   * @param the_parameter_list - IN
   * @return No_Error, SNPW_Invalid_FormatLength, SNPW_Invalid_Maximum_Length
   */
  Error_Code  SNPrintf (std::wstring  &the_output_string,
                        std::wstring  the_format_string, 
                        std::size_t   the_maximum_length,
                        va_list       &the_parameter_list)

  { // begin
    wchar_t   *the_buffer = NULL;

    Method_State_Block_Begin(4)
      State(1)
        if (the_format_string.length() < 3)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Invalid_FormatLength, "Invalid parameter value - the_format is too short");
      End_State

      State(2)
        if (the_maximum_length < the_format_string.length())
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Invalid_Maximum_Length, "Invalid parameter value - the_maximum_length is to short");
      End_State

      State(3)
        the_buffer = new (std::nothrow) wchar_t[the_maximum_length];

        if (the_buffer == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Memory_Allocation_Error, Logging::Error,
                                        "Memory allocation error - could not allocate %lld bytes for the local buffer.", the_maximum_length * sizeof (wchar_t));
      End_State

      State(4)
        if (std::vswprintf (the_buffer, the_maximum_length, the_format_string.c_str(), the_parameter_list) < 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPW_Format_Error2, "The call to std::vswprintf failed");
        else { // assign & cleanup
          the_output_string = the_buffer;

          delete[] the_buffer;
          the_buffer = NULL;
        } // if then
      End_State
    End_Method_State_Block

    A4_Cleanup_Begin
      if (the_buffer != NULL)
      { // begin
        delete[] the_buffer;
        the_buffer = NULL;   
      } // if then
    A4_End_Cleanup

    return the_method_error.Get_Error_Code();  
  } // SNPrintf (wstring)

  /**
   * @brief format a std::string using a variable argument list
   * @param the_output_string - OUT
   * @param the_format_string - IN
   * @param the_maximum_length - IN
   * @param ... - IN - parameter list
   * @return No_Error, SNPW_Invalid_FormatLength, SNPW_Invalid_Maximum_Length
   */
  Error_Code  SNPrintf (std::string  &the_output_string,
                        std::string  the_format_string, 
                        std::size_t  the_maximum_length, 
                        ...)
  { // begin
    va_list the_va_list;

    Method_State_Block_Begin(1)
      State(1)
        va_start (the_va_list, the_maximum_length);

        the_method_error = SNPrintf (the_output_string, the_format_string, the_maximum_length, the_va_list);

        va_end (the_va_list);  
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();  
  } // SNPrintf (string, ...)

  /**
   * @brief Format a std::string using a variable argument list.
   * @param the_output_string - OUT
   * @param the_format_string - IN
   * @param the_maximum_length - IN
   * @param the_parameter_list - IN
   * @return No_Error, SNPA_Invalid_FormatLength, SNPA_Invalid_Maximum_Length, SNPA_Memory_Allocation_Error, SNPA_Format_Error2
   */
  Error_Code  SNPrintf (std::string  &the_output_string,
                        std::string  the_format_string, 
                        std::size_t  the_maximum_length,
                        va_list      &the_parameter_list)

  { // begin
    char   *the_buffer = NULL;

    Method_State_Block_Begin(4)
      State(1)
        if (the_format_string.length() < 3)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPA_Invalid_FormatLength, "Invalid parameter value - the_format is too short");
      End_State

      State(2)
        if (the_maximum_length < the_format_string.length())
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPA_Invalid_Maximum_Length, "Invalid parameter value - the_maximum_length is to short");
      End_State

      State(3)
        the_buffer = new (std::nothrow) char[the_maximum_length];

        if (the_buffer == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPA_Memory_Allocation_Error, Logging::Error,
                                        "Memory allocation error - could not allocate %lld bytes for the local buffer.", the_maximum_length * sizeof (char));
      End_State

      State(4)
        if (std::vsnprintf (the_buffer, the_maximum_length, the_format_string.c_str(), the_parameter_list) < 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, SNPA_Format_Error2, "The call to std::vsnprintf failed");
        else { // assign & cleanup
          the_output_string = the_buffer;

          delete[] the_buffer;
          the_buffer = NULL;
        } // if then
      End_State
    End_Method_State_Block

    A4_Cleanup_Begin
      if (the_buffer != NULL)
      { // begin
        delete[] the_buffer;
        the_buffer = NULL;   
      } // if then
    A4_End_Cleanup

    return the_method_error.Get_Error_Code();  
  } // SNPrintf (string)

  /// @brief Parse the individual (delimited) values from (std::string) the_input_string.
  /// @param the_input_string - IN
  /// @param the_value_vector - OUT
  /// @param the_delimiting_character - IN - the value separator character (default is a comma).
  //
  Error_Code  Parse_CSV_Values (std::string   the_input_string,
                                String_Vector &the_value_vector,
                                const char    the_delimiting_character)
  { // begin
    Method_State         the_value_loop = 0;

    std::size_t the_input_offset = 0;

    std::string the_parse_string;

    bool        in_quote = false;

    Method_State_Block_Begin(6)
      State(1) 
        the_value_vector.clear();

        if (the_delimiting_character == '\0')
          the_method_error = A4_Error (A4_Utils_Module_ID, PCV_Invalid_Delimiter_Char, "Invalid parameter value - the_delimiting_character is zero.");
      End_State

      State(2)
        if (the_input_string.length() < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, PCV_Empty_Input_String, "Invalid parameter contents - the_input_string is empty.");
      End_State

      State(3)
        Define_Target_State(the_value_loop);
      End_State

      State_NoTry(4)
        if (the_input_offset >= the_input_string.length())
        { // all done
          if (the_parse_string.length() > 0)
            the_value_vector.push_back(the_parse_string);

          Terminate_The_Method_Block; 
        } // if then
      End_State_NoTry

      State(5)
        switch (the_input_string[the_input_offset])
        { // begin
          case '\n': // end of the line
          case '\r':
            if (the_parse_string.length() > 0)
              the_value_vector.push_back(the_parse_string);

            Terminate_The_Method_Block; //all done
          break;

          case '\"': // double quotes are not included in the value vector...is there a use case to include them?
            in_quote = !in_quote; // toggle the state
            the_input_offset += 1;
          break;

          default: // handle the rest
            if (the_input_string[the_input_offset] == the_delimiting_character)
            { // handle the delimiter
              if (in_quote == true)
              { // include the delimiter in the value string
                the_parse_string += the_input_string[the_input_offset];
              } // if then
              else { // hit the delimiter
                if (the_parse_string.length() > 0)
                { // append the string to the value vector
                  the_value_vector.push_back(the_parse_string);

                  the_parse_string.clear(); 
                } // if then
              } // if else            
            } // if then
            else if ((A4_isspace (the_input_string[the_input_offset]) == 0) || (in_quote == true)) // <-- white space is only copied when inside of double-quotes
                    the_parse_string += the_input_string[the_input_offset];

            the_input_offset += 1;
          break;
        } // switch      
      End_State

      State(6)
        Set_Target_State(the_value_loop);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();    
  } // Parse_CSV_Values

  /// @brief Parse the individual (delimited) values from (wstring) the_input_string.
  /// @param the_input_string - IN
  /// @param the_value_vector - OUT
  /// @param the_delimiting_character - IN - the value separator character (default is a comma).
  //
  Error_Code  Parse_CSV_Values (std::wstring   the_input_string,
                                WString_Vector &the_value_vector,
                                wchar_t        the_delimiting_character)
  { // begin
    Method_State    the_value_loop = 0;

    std::size_t     the_input_offset = 0;

    std::wstring    the_parse_string;

    bool            in_quote = false;

    Method_State_Block_Begin(6)
      State(1) 
        the_value_vector.clear();

        if (the_delimiting_character == '\0')
          the_method_error = A4_Error (A4_Utils_Module_ID, PCV2_Invalid_Delimiter_Char, "Invalid parameter value - the_delimiting_character is zero.");
      End_State

      State(2)
        if (the_input_string.length() < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, PCV2_Empty_Input_String, "Invalid parameter contents - the_input_string is empty.");
      End_State

      State(3)
        Define_Target_State(the_value_loop);
      End_State

      State_NoTry(4)
        if (the_input_offset >= the_input_string.length())
        { // all done
          if (the_parse_string.length() > 0)
            the_value_vector.push_back(the_parse_string);

          Terminate_The_Method_Block; 
        } // if then
      End_State_NoTry

      State(5)
        switch (the_input_string[the_input_offset])
        { // begin
          case '\n': // end of the line
          case '\r':
            if (the_parse_string.length() > 0)
              the_value_vector.push_back(the_parse_string);

            Terminate_The_Method_Block; //all done
          break;

          case '\"': // double quotes are not included in the value vector...is there a use case to include them?
            in_quote = !in_quote; // toggle the state
            the_input_offset += 1;
          break;

          default: // handle the rest
            if (the_input_string[the_input_offset] == the_delimiting_character)
            { // handle the delimiter
              if (in_quote == true)
              { // include the delimiter in the value string
                the_parse_string += the_input_string[the_input_offset];
              } // if then
              else { // hit the delimiter
                if (the_parse_string.length() > 0)
                { // append the string to the value vector
                  the_value_vector.push_back(the_parse_string);

                  the_parse_string.clear(); 
                } // if then
              } // if else            
            } // if then
            else if ((A4_isspace (the_input_string[the_input_offset]) == 0) || (in_quote == true)) // <-- white space is only copied when inside of double-quotes
                    the_parse_string += the_input_string[the_input_offset];

            the_input_offset += 1;
          break;
        } // switch      
      End_State

      State(6)
        Set_Target_State(the_value_loop);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();    
  } // Parse_CSV_Values (wide)

  /// @brief Wrap the std::chrono methods to produce an old-fashioned time_t.
  //
  std::time_t   Now(void)
  { // begin
    A4_TZ_Set();
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  } // end now

  /**
   * @brief Retrieve the computer name from a Linux system.
   * @param the_computer_name - OUT
   * @return No_Error, GLCN_FOpen_Error, GLCN_FGets_Error, GLCN_FClose_Error
   */
  Error_Code  Get_Linux_Computer_Name (std::string  &the_computer_name)
  { // begin
    std::FILE   *the_file = NULL;

    char        the_buffer [HOST_NAME_MAX + 1];

    Method_State_Block_Begin(3)  
      State(1)
        the_computer_name.clear();
        memset (the_buffer, 0, sizeof (the_buffer));

        the_file = std::fopen ("/proc/sys/kernel/hostname", "r");

        if (the_file == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, GLCN_FOpen_Error, "Call to std::fopen failed - Is this a Linux system?");
      End_State

      State(2)
        if (std::fgets (the_buffer, sizeof (the_buffer), the_file) == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, GLCN_FGets_Error, "Call to std::fgets failed.");
        else { // set the value
          the_computer_name = the_buffer;
          the_computer_name.pop_back(); // remove the trailing newline character
       } // if then
      End_State

      State(3)
        if (std::fclose(the_file) != 0)
          the_method_error = A4_Error (A4_Utils_Module_ID, GLCN_FClose_Error, "Call to std::fclose failed.");

        the_file = NULL;
      End_State
    End_Method_State_Block

    A4_Cleanup_Begin
      if (the_file != NULL)
      { // cleanup
        (void) std::fclose(the_file);
        the_file = NULL;
      } // if then      
    A4_End_Cleanup            

    return the_method_error.Get_Error_Code();   
  } // Get_Linux_Computer_Name

  /// @brief Convert a std::string to std::wstring
  /// @param the_input_string - in - std::string
  /// @param the_output_string - OUT - std::wstring
  /// @note Only performs an actual conversion is compiled with A4_Use_Wide_String defined, 
  //
  Error_Code  AString_To_WString(std::string   the_input_string,
                                 std::wstring     &the_output_string)
  { // begin
    wchar_t  *the_wchar_string = NULL;

    Method_State_Block_Begin(2)
      State_NoTry(1)  
        the_output_string.clear();

        if (the_input_string.length() < 1)
          Terminate_The_Method_Block; // return an empty string
      End_State_NoTry

      State(2) // conditional compilation
          the_method_error = Char_to_WChar (the_input_string.c_str(), the_input_string.length(), the_wchar_string);

        if (the_method_error == No_Error)
          the_output_string = the_wchar_string;
      End_State
    End_Method_State_Block

    if (the_wchar_string != NULL)
    { // garbage collect
      delete[] the_wchar_string;
      the_wchar_string = NULL;
    } // if then

    return the_method_error.Get_Error_Code();   
  } // AString_To_WString

  /// @brief Convert from an std::wstring to a std::string
  /// @param the_input_string
  /// @param the_output_string
  /// @note The Windows method appears to be unreliable
  //
  Error_Code  WString_To_AString(std::wstring  the_input_string,
                                 std::string   &the_output_string)
  { // begin
    std::size_t   the_offset = 0;

    std::uint8_t  *the_byte_array = ((std::uint8_t *) the_input_string.c_str());

    Method_State_Block_Begin(1)
      State(1)  
        the_output_string.clear();

        while ((the_offset < (the_input_string.length() * sizeof (wchar_t))) && (the_output_string.length() < the_input_string.length()))
        { // begin
          if ((the_byte_array[the_offset] > 0) && (the_byte_array[the_offset] < 128))
            the_output_string += static_cast<char>(the_byte_array[the_offset]);

          the_offset += 1;
        } // while
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();   
  } // AString_To_WString


  /// @brief Convert a char string to a wchar_t string
  /// @param the_input_string - IN - must not be NULL
  /// @param the_input_length - IN - must be > 0
  /// @param the_output_string - IN - must be NULL, OUT - the converted string
  //
  Error_Code  Char_to_WChar (const char   *the_input_string,
                             std::size_t  the_input_length,
                             wchar_t      *&the_output_string)
  { // begin
    Method_State_Block_Begin(5)
      State(1)  
        if (the_output_string != NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, CtWC_Invalid_Output_State, "Invalid parameter value - the_output_string is not initialized to NULL - memory leak?");
      End_State

      State(2)
        if (the_input_string == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, CtWC_Invalid_Input_Address, "Invalid parameter value - the_input_string is NULL.");
      End_State

      State(3)
        if (the_input_length < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, CtWC_Invalid_Input_Length, "Invalid parameter value - the_input_length is zero.");
      End_State

      State(4)
        the_output_string = new (std::nothrow) wchar_t[the_input_length + 1];

        if (the_output_string == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, CtWC_Memory_Allocation_Error, "Memory allocation error - could not allocate the_output_string.");
        else memset (the_output_string, 0, sizeof (wchar_t) * (the_input_length + 1));
      End_State

      State(5)
        if (A4_SetLocale(LC_ALL, "") == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, CtWC_Setlocale_Error, "Failed to set Locale to user defaults");
        else{ // convert
          if (static_cast <ssize_t>(A4_mbstowcs(the_output_string, the_input_string, the_input_length)) < 0)
            the_method_error = A4_Error (A4_Utils_Module_ID, CtWC_Conversion_Error, "Call to std::mbstowcs failed");
        } // if else
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();    
  } // Char_to_WChar

  /// @brief Convert a char string to a wchar_t string
  /// @param the_input_string - IN - must not be NULL
  /// @param the_input_length - IN - must be > 0
  /// @param the_output_string - IN - must be NULL, OUT - the converted string
  /// @deprecated - will soon be removed as it appears not to be reliable now, for some reason...used to work.
  //
  Error_Code  WChar_to_Char (const wchar_t  *the_input_string,
                             std::size_t    the_input_length,
                             char           *&the_output_string)
  { // begin
    Method_State_Block_Begin(5)
      State(1)  
        if (the_output_string != NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, WCtC_Invalid_Output_State, "Invalid parameter value - the_output_string is not initialized to NULL - memory leak?");
      End_State

      State(2)
        if (the_input_string == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, WCtC_Invalid_Input_Address, "Invalid parameter value - the_input_string is NULL.");
      End_State

      State(3)
        if (the_input_length < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, WCtC_Invalid_Input_Length, "Invalid parameter value - the_input_length is zero.");
      End_State

      State(4)
        the_output_string = new (std::nothrow) char[the_input_length + 1];

        if (the_output_string == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, WCtC_Memory_Allocation_Error, "Memory allocation error - could not allocate the_output_string.");
        else memset (the_output_string, 0, the_input_length + 1);
      End_State

      State(5)
        if (A4_SetLocale(LC_ALL, "") == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, WCtC_Setlocale_Error, "Failed to set Locale to user defaults");
        else{ // convert
          if (static_cast <ssize_t>(std::wcstombs(the_output_string, the_input_string, the_input_length)) < 0)
            the_method_error = A4_Error (A4_Utils_Module_ID, WCtC_Conversion_Error, "Call to std::wcstombs failed");
        } // if else
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();    
  } // WChar_to_Char

  /// @brief Convert a wide string to a char string
  /// @param the_input_string - IN
  /// @param the_output_string - OUT
  //
  Error_Code  WChar_to_Char (const std::wstring   the_input_string,
                             std::string          &the_output_string)
  { // begin 
    char     *the_output_char_string = NULL;

    Method_State_Block_Begin(2)
      State(1)  
        the_output_string.clear();

        if (the_input_string.length() < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, WCtC2_Empty_Input_String, "Invalid parameter length - the_input_string is empty.");
        else the_method_error = WChar_to_Char(the_input_string.c_str(), the_input_string.length(), the_output_char_string);
      End_State

      State(2)
        if (the_output_char_string == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, WCtC2_No_Output_String, "No output string was created.");
        else { // xfer the results
          the_output_string = the_output_char_string;
          delete[] the_output_char_string;
          the_output_char_string = NULL;
        } // if else
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();   
  } // WChar_to_Char

  /// @brief Sleep for a specified number of milli-seconds
  /// @param num_milli_seconds - IN
  //
  void  Sleep_MS(std::uint64_t   num_milli_seconds)
  { // begin
    if (num_milli_seconds > 0)
      std::this_thread::sleep_for(std::chrono::duration<std::int64_t, std::milli>(num_milli_seconds));
  } // Sleep_MS

/**
 * @brief Replace a substring in  the_string with a replacement substring
 * @param the_string - IN / OUT
 * @param the_string_to_replace - IN
 * @param the_replacement_string - IN
 * @return No_error, R_Empty_Input_String, R_Empty_String_To_Replace, R_Empty_Replacement_String
 */
  Error_Code Replace (std::string         &the_string, 
                      const std::string   the_string_to_replace, 
                      const std::string   the_replacement_string) 
  { // begin
    std::size_t the_start_pos = 0;

    Method_State_Block_Begin(4)
      State(1)
        if (the_string.length() < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, R_Empty_Input_String, "Invalid parameter length - the_string is empty.");
      End_State

      State(2)
        if (the_string_to_replace.length() < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, R_Empty_String_To_Replace, "Invalid parameter length - the_string_to_replace is empty.");
      End_State

      State(3)
        if (the_replacement_string.length() < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, R_Empty_Replacement_String, "Invalid parameter length - the_replacement_string is empty.");
      End_State

      State(4)
        the_start_pos = the_string.find(the_string_to_replace);

        if(the_start_pos != std::string::npos)
           the_string.replace(the_start_pos, the_string_to_replace.length(), the_replacement_string);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code(); 
  } // Replace

  /// @brief  Determine whether the num_milli_seconds_to_wait has passed from the_start_point
  /// @param the_start_point - IN - the starting time point to compare with.
  /// @param num_milli_seconds_to_wait
  /// @return \b true if (the_start_point + num_milli_seconds_to_wait) < now
  //
  bool  Time_Has_Elapsed(Time_Point     &the_start_point,
                         std::uint64_t	num_milli_seconds_to_wait)
  { // begin
    std::chrono::time_point<std::chrono::system_clock>  the_end_time = std::chrono::system_clock::now();

    std::chrono::duration<double>	  the_difference = the_end_time - the_start_point;

    return (the_difference.count() > (static_cast<double>(num_milli_seconds_to_wait) / 1000.0));
  } // Time_Has_Elapsed

  /**
  * \brief  Overwrite a strings contents before releasing the memory
  * \param  the_string - IN / OUT
  */
    void Burn(std::wstring &the_string)
    { // begin
      for (std::size_t i = 0; i < the_string.length(); i++)
        the_string [i] = static_cast<wchar_t>('\0');

      the_string.clear();
      the_string.shrink_to_fit();
    } // Burn

  /**
  * \brief  Overwrite a strings contents before releasing the memory
  * \param  the_string - IN / OUT
  */
    void Burn(std::string &the_string)
    { // begin
      for (std::size_t i = 0; i < the_string.length(); i++)
        the_string [i] = static_cast<char>('\0');

      the_string.clear();
      the_string.shrink_to_fit();
    } // Burn

  /**
  * \brief  Burn & shrink the string vector
  * \param  the_vector - IN / OUT
  */  
  void Burn(String_Vector &the_vector)
  { // begin
    for (std::size_t i = 0; i < the_vector.size(); i++)
      Burn(the_vector[i]);

    the_vector.clear();
    the_vector.shrink_to_fit();
  } // Burn (String_Vector)

  /// Simple (but slow) method to convert a byte array to the hex string representation of the array
  /// @param  the_byte_array - IN - must not be NULL
  /// @param  the_byte_array_length - IN - the length of the_byte_array.
  /// @param  the_hex_string - OUT
  //
  Error_Code  Binary_To_Hex_String(std::uint8_t   *the_byte_array,
                                   std::size_t    the_byte_array_length,
                                   std::string    &the_hex_string)
  { // begin
    std::size_t	the_buffer_length = (the_byte_array_length + 1) * 2;
    std::size_t	the_offset = 0;

    char	the_buffer[3];

    Method_State_Block_Begin(3)
      State(1)
        if (the_byte_array == NULL)
          the_method_error = A4_Error (A4_Utils_Module_ID, BTHS_Invalid_Byte_Array_Address, "Invalid parameter address - the_byte_array is NULL.");
      End_State

      State(2)
        if (the_byte_array_length < 1)
          the_method_error = A4_Error (A4_Utils_Module_ID, BTHS_Invalid_Byte_Array_Length, "Invalid parameter value - the_byte_array_length is zero.");
      End_State

      State(3) // this is definitely not the fastest method and should not be heavily used
        the_hex_string.reserve(the_buffer_length); // allocate enough room for the finished product

        for (the_offset = 0; the_offset < the_byte_array_length; the_offset++)
        { // begin
          std::snprintf (the_buffer, sizeof (the_buffer), "%02X", static_cast<int>(the_byte_array[the_offset]));
          the_hex_string += the_buffer;
        } // for
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();   
  } // Binary_To_Hex_String
} // namespace A4_Lib
