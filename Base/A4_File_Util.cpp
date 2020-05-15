/**
 * \brief  AZ Lib File Utility interface - wrapper around common file functions, normalizing error handling & memory management. 
 *
 * \author a. zippay * 2019 
 * 
 * \note  * The MIT License
 *
 * Copyright 2019 a. zippay
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

#include "A4_File_Util.hh"
#include "A4_Utils.hh"
#include "A4_Method_State_Block.hh"
#include "A4_Lib_Base.hh"

#ifndef A4_Lib_Windows
  #include <dirent.h>
#endif
#include <sys/stat.h>

/// @brief Detect whether the a given file currently exists.
/// @param the_filespec
/// @return \b true if the file exists.
//
bool A4_Lib::File_Exists (std::string   the_filespec)
{ // begin
  struct stat the_buffer; 
  
  int the_result = stat (the_filespec.c_str(), &the_buffer);

  return (the_result== 0);
} // File_Exists 


/// @brief Detect whether the a given file currently exists.
/// @param the_filespec
/// @return \b true if the file exists.
//
bool A4_Lib::File_Exists (std::wstring   the_filespec)
{ // begin
  std::string	the_a_filename;

  bool		the_result = false;

  Method_State_Block_Begin(2)
    State(1) 
      the_method_error = A4_Lib::WString_To_AString (the_filespec, the_a_filename);
    End_State

    State(2)
      the_result = A4_Lib::File_Exists (the_a_filename);
    End_State
  End_Method_State_Block

  return the_result;
} // File_Exists



/// @brief   Retrieve the time the file was last modified.
/// @param the_filespec  - IN
/// @param the_last_modified_time - OUT
/// @return No_Error upon success.
//
Error_Code  A4_Lib::Get_File_Last_Modified_Time (std::string    the_filespec,
                                                 std::time_t    &the_last_modified_time)
{ // begin
  struct stat the_buffer; 
  
  Method_State_Block_Begin(2)
    State(1)
      the_last_modified_time = 0;

      if (the_filespec.length() < 1)
        the_method_error = A4_Error (A4_File_Util_Module_ID, A4_File_Util_Errors::GFLMT_Invalid_Filespec, "Invalid parameter value - the_filespec is empty.");
    End_State
    
    State(2)    
      if  (stat (the_filespec.c_str(), &the_buffer) != 0)
        the_method_error = A4_Error (A4_File_Util_Module_ID, A4_File_Util_Errors::GFLMT_No_File_Found, A4_Lib::Logging::Error, "The file %s does not exist", the_filespec.c_str());
      else { // found it
        the_last_modified_time = the_buffer.st_mtime;// <-- use legacy define since Windows doesn't support the new nano-second version.
      } // if else
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} // Get_File_Last_Modified_Time


  /// @brief   Retrieve the time the file was created.
  /// @param the_filespec  - IN
  /// @param the_last_modified_time - OUT
  /// @return No_Error upon success.
  //
Error_Code  A4_Lib::Get_File_Creation_Time(std::string    the_filespec,
					   std::time_t    &the_creation_time)
{ // begin
  struct stat the_buffer;

  Method_State_Block_Begin(2)
    State(1)
    the_creation_time = 0;

    if (the_filespec.length() < 1)
      the_method_error = A4_Error(A4_File_Util_Module_ID, A4_File_Util_Errors::GFCT_Invalid_Filespec, "Invalid parameter value - the_filespec is empty.");
  End_State

    State(2)
      if (stat(the_filespec.c_str(), &the_buffer) != 0)
	the_method_error = A4_Error(A4_File_Util_Module_ID, A4_File_Util_Errors::GFCT_No_File_Found, A4_Lib::Logging::Error, "The file %s does not exist", the_filespec.c_str());
      else { // found it
	the_creation_time = the_buffer.st_ctime;// <-- use legacy define since Windows doesn't support the new nano-second version.
      } // if else
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Get_File_Last_Modified_Time


/**
* \brief  Retrieve the file size in bytes.
* \param  the_filespec - IN
* \param  the_file_size - OUT
* \note stat64 should probably be used, but Windows will need mapping given their desire for unique naming.
*/
  Error_Code A4_Lib::Get_File_Size(std::string the_filespec,
                                   std::size_t &the_file_size)
  { // begin
    struct stat the_stat_buffer;

    Method_State_Block_Begin(2)
      State(1)  
        the_file_size = 0;

        if (the_filespec.length() < 1)
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_File_Util_Errors::GFS_Invalid_Filespec, "Invalid parameter length - the_filespec is empty.");
      End_State

      State(2)
        if (stat(the_filespec.c_str(), &the_stat_buffer) == 0)
          the_file_size = static_cast<std::size_t>(the_stat_buffer.st_size);
        else the_method_error = A4_Error (A4_File_Util_Module_ID, A4_File_Util_Errors::GFS_File_Not_Found, A4_Lib::Logging::Error,
                                          "The file %s was not found.", the_filespec.c_str());
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();
  } // Get_File_Size
  
  
  #ifdef A4_Lib_Windows
  Error_Code Get_Windows_File_Vector (std::wstring            the_search_string,
                                      A4_Lib::WString_Vector  &the_filename_vector)
  { // begin
    Method_State_Block_Begin(0)
      State(1)    
        the_filename_vector.clear();
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code(); 
  }   // Get_Windows_File_Vector
#else
  Error_Code Get_Linux_File_Vector (std::string            the_search_string,
                                    A4_Lib::String_Vector  &the_filename_vector)
  { // begin
    A4_Lib::String_Vector   the_search_string_vector;
    
    std::string             the_foldername;
    std::string             the_stat_string;
    
    DIR                     *the_dir = NULL;
  
    struct dirent           *the_dir_entry = NULL; 
    
    Method_State            the_file_loop = 0;
    
    bool                    is_a_match = false;
    
    Method_State_Block_Begin(8)
      State(1)  
        the_filename_vector.clear();
    
        the_method_error = A4_Lib::Parse_CSV_Values (the_search_string, the_search_string_vector, '/');
      End_State
            
      State(2)
        if (the_search_string_vector.size() < 1)  
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::GLFV_Empty_Vector1, "the_search_string did not parse into a searchable vector.");
      End_State
              
      State(3)
        for (std::size_t i = 0; i < the_search_string_vector.size() -1; i++)
          the_foldername += ("/" + the_search_string_vector[i]);
      
        if (the_foldername.length() < 1)
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::GLFV_Invalid_Foldername_Length, A4_Lib::Logging::Error,
                                       "Could not create a valid foldername from the_search_string '%s'", the_search_string.c_str());
      End_State
              
      State(4)
        the_dir = opendir (the_foldername.c_str());
      
        if (the_dir == NULL)
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::GLFV_Folder_Not_Found, A4_Lib::Logging::Error,
                                       "Folder '%s' was not found", the_foldername.c_str());
        else the_dir_entry = readdir (the_dir);
      End_State
              
      State(5)
        Define_Target_State(the_file_loop);
      End_State
      
      State(6)
        if (the_dir_entry == NULL)
        { // all done
          Terminate_The_Method_Block;
          
          if (closedir (the_dir) != 0)
            the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::GLFV_Closedir_Error, "Call to closedir failed with error %d", errno);
        } // if then
      End_State
              
      State(7)
        if (the_dir_entry->d_type == DT_REG)
          the_method_error = A4_Lib::String_Matches_Wildcard (the_search_string_vector.back(), the_dir_entry->d_name, is_a_match);
        else {
          the_dir_entry = readdir (the_dir);
        
          Set_Target_State (the_file_loop);         
        } // if else
      End_State
              
      State(8)
        if (is_a_match == true)
        { // file found
          the_filename_vector.push_back (the_dir_entry->d_name);
        } // if then
        
        the_dir_entry = readdir (the_dir);
        
        Set_Target_State (the_file_loop);
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code(); 
  } // Get_Linux_File_Vector
#endif // Get_File_Vector os-specific implementations
  
  
  Error_Code A4_Lib::Get_File_Vector (std::string            the_search_string,
                                      A4_Lib::String_Vector  &the_filename_vector)
  { // begin       
    Method_State_Block_Begin(2)
      State(1)    
        the_filename_vector.clear();
    
        if (the_search_string.length() < 1)
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::GFV_Empty_Search_String, "Invalid parameter length - the_search_string is empty");
      End_State
            
      State(2)
  #ifdef A4_Lib_Windows
        the_method_error = Get_Windows_File_Vector (the_search_string, the_filename_vector);
  #else
        the_method_error = Get_Linux_File_Vector (the_search_string, the_filename_vector);
  #endif 
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code(); 
  } // Get_File_Vector
  
  /**
   * \brief Delete a file from storage with the given file-spec
   * @param the_filespec - IN - the file must exist
   * @return No_Error, DF_Empty_Filespec, DF_No_File_Exists, DF_Remove_Failure
   */
  Error_Code A4_Lib::Delete_File (std::string  the_filespec)
  { // begin
    Method_State_Block_Begin(3)
      State(1)
        if (the_filespec.length() < 1)
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::DF_Empty_Filespec, "Invalid parameter length - the_filespec is empty.");
      End_State
            
      State(2)
        if (A4_Lib::File_Exists (the_filespec) != true)
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::DF_No_File_Exists, A4_Lib::Logging::Error,
                                       "No file exists with name %s", the_filespec.c_str());
      End_State
              
      State(3)
        if (std::remove (the_filespec.c_str()) != 0)
          the_method_error = A4_Error (A4_File_Util_Module_ID, A4_Lib::A4_File_Util_Errors::DF_Remove_Failure, A4_Lib::Logging::Error,
                                       "Call to std::remove failed delete file %s", the_filespec.c_str());
      End_State
    End_Method_State_Block

    return the_method_error.Get_Error_Code();   
  } // Delete_File
  