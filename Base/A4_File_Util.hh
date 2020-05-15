#ifndef __A4_File_Util_Defined__ 
#define __A4_File_Util_Defined__

/**
 * \brief  AZ Lib File Utility interface - wrapper around common file functions, normalizing error handling & memory management. 
 *
 * \author a. zippay * 2019, 2020
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


namespace A4_Lib
{ // begin
  Error_Code  Get_File_Last_Modified_Time (std::string    the_filespec,
					   std::time_t    &the_last_modified_time);

  Error_Code  Get_File_Creation_Time(std::string    the_filespec,
				     std::time_t    &the_last_modified_time);

  Error_Code Get_File_Size(std::string the_filespec,
                           std::size_t &the_file_size);
  
  bool File_Exists (std::string   the_filespec);
  bool File_Exists (std::wstring   the_filespec);
  
  Error_Code Get_File_Vector (std::string    the_search_string,
                              String_Vector  &the_filename_vector);
   
  Error_Code Delete_File (std::string  the_filespec);
  
  enum A4_File_Util_Errors      
  { // begin  
    GFLMT_Invalid_Filespec            = 0,
    GFLMT_No_File_Found               = 1,
    GFCT_Invalid_Filespec             = 2,
    GFCT_No_File_Found                = 3,
    GFS_Invalid_Filespec              = 4,
    GFS_File_Not_Found                = 5,
    GFV_Empty_Search_String           = 6,
    GLFV_Closedir_Error               = 7,
    GLFV_Empty_Vector1                = 8,
    GLFV_Invalid_Foldername_Length    = 9,
    GLFV_Folder_Not_Found             = 10,
    DF_Empty_Filespec                 = 11,
    DF_No_File_Exists                 = 12,
    DF_Remove_Failure                 = 13,
  }; // A4_File_Util_Errors
} // namespace A4_Lib

#endif // __A4_File_Util_Defined__
  