/**
* \author Albert Zippay * 1995..2020
* \copyright Albert Zippay - 1995 - 2020
* \file A4_File_Logger.cpp
*
* \brief  File Logging base class implementation. Creates one or more log files.
*
 * \note  * The MIT License
 *
 * Copyright 1995..2020 a. zippay
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

#include "A4_File_Logger.hh"
#include "A4_Method_State_Block.hh"
#include "A4_Utils.hh"
#include "A4_File_Util.hh"

#include <iostream>
#include <cstdio>

#ifdef A4_Lib_Windows
  #include "A4_Win_Helper.hh"
#endif
/**
 * \brief default constructor
 */
A4_Lib::File_Logger::File_Logger(void) 
{ // begin
  this->log_file = NULL;
  this->sequence_number = 1; // start the count at one
  this->max_log_file_size = 0;
  this->rollover_sequence = 0;
  this->is_closing = false;
  this->last_flush_time = A4_Lib::Now();
  this->file_creation_time = 0;
} // constructor

/**
 * \brief default destructor
 */
A4_Lib::File_Logger::~File_Logger(void)
{ // begin
  this->rollover_sequence = 0;
  this->sequence_number = 0;
  this->max_log_file_size = 0;
  this->file_creation_time = 0;
  
  if (this->Is_Open () == true)
    (void) this->Close ();
  
  A4_Lib::Burn(this->header_text);
  A4_Lib::Burn(this->computer_name);  
  A4_Lib::Burn(this->log_folder); 
  A4_Lib::Burn(this->log_filename); 
  A4_Lib::Burn(this->log_extension);   
  
  A4_Lib::Burn(this->filespec_vector);  
  A4_Lib::Burn(this->filename_vector);    
} // destructor

/**
 * \brief Initialize internal members based upon the_log_filespec 
 * @param the_log_filespec - IN
 * @return ILF_Invalid_Filespec_Length when the_log_filespec is too short
 */
Error_Code  A4_Lib::File_Logger::Init_Log_Filespec (std::string  the_log_filespec)
{ // begin
  char  the_delimiter = '\0';
#ifdef A4_Lib_Windows
  the_delimiter = '\\';
#else
  the_delimiter = '/';
#endif
  
  Method_State_Block_Begin(4)
    State(1)
      if (the_log_filespec.length() < File_Logger_Constants::Min_Log_Filename_Length)
        the_method_error = A4_Error (A4_Log_Module_ID, File_Logger::ILF_Invalid_Filespec_Length, "Invalid parameter length - the_log_filespec is too short.");
      else the_method_error = A4_Lib::Parse_CSV_Values (the_log_filespec, this->filespec_vector, the_delimiter);           
    End_State
          
    State(2)
      if (this->filespec_vector.size() < 2)
        the_method_error = A4_Error (A4_Log_Module_ID, ILF_Invalid_Filespec_Vector_Size, "Parsing the_log_filespec resulted in a parsed vector that is too short.");
      else the_method_error = A4_Lib::Parse_CSV_Values (this->filespec_vector.back(), this->filename_vector, '.');  
    End_State
          
    State(3)
      if (this->filename_vector.size () < 1)
        the_method_error = A4_Error (A4_Log_Module_ID, ILF_Invalid_Filename_Vector_Size, "Parsing the file name resulted in a parsed vector that is too short.");
    End_State
            
    State(4)
#ifndef A4_Lib_Windows  
      this->log_folder = the_delimiter;
#endif
    
      for (std::size_t i = 0; i < this->filespec_vector.size() - 1; i++)
        this->log_folder += (this->filespec_vector[i] + the_delimiter);
    
      this->log_filename = this->filename_vector[0];
    
      if (this->filename_vector.size() > 1)
      { // fill in the log extension, if it exists
        this->log_extension = ".";
        this->log_extension += this->filename_vector.back();
      } // if then
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Init_Log_Filespec

/**
 * \brief Scan the log folder for pre-existing log files - scan the rollover sequence numbers and set the rollover_sequence member to the max+1
 * \note assumes the name format is name-nnnnn.ext where the numeric number is added to the initial log filename.
 */
Error_Code  A4_Lib::File_Logger::Init_Rollover_Sequence(void)
{ // begin
  String_Vector  the_filename_vector;
  String_Vector  the_filename_parts;
          
  std::string    the_search_string;
  
  Method_State   the_file_loop = 0;
  
  std::size_t    the_offset = 0;
  std::int32_t   the_max_sequence = -1;
  
  std::time_t    the_creation_time = 0;
  std::time_t    the_current_time = A4_Lib::Now();
  
  int            the_int_value = 0;
  
  Method_State_Block_Begin(7)
    State(1) 
      the_search_string = this->log_folder + this->log_filename + "*";
  
      the_method_error = A4_Lib::Get_File_Vector (the_search_string, the_filename_vector);
    End_State
              
    State(2)
      Define_Target_State(the_file_loop);
    End_State
    
    State(3)
      if (the_offset < the_filename_vector.size())
        the_method_error = A4_Lib::Parse_CSV_Values (the_filename_vector[the_offset], the_filename_parts, '.');
      else { // finish
        this->rollover_sequence = static_cast<std::uint16_t>(the_max_sequence + 1);
        
        Terminate_The_Method_Block;
      } // if else
    End_State
            
    State(4)
      if (the_filename_parts.size() == 2)
        the_method_error = A4_Lib::Parse_CSV_Values (the_filename_parts[0], the_filename_parts, '-');
      else { // not a file we're looking for
        the_offset += 1;
        Set_Target_State(the_file_loop);         
      } // if else
    End_State
            
    State(5)
      if (the_filename_parts.size() == 2)
      { // fits the patter, now compare the sequece number
        the_int_value = std::atoi(the_filename_parts[1].c_str());
    
        if (the_int_value > the_max_sequence)
          the_max_sequence = the_int_value;
      } // if then
    
      if (this->log_archive_days != 0)  
        the_method_error = A4_Lib::Get_File_Creation_Time (this->log_folder + the_filename_vector[the_offset], the_creation_time);
      else {
        the_offset += 1;
        Set_Target_State(the_file_loop);       
      } // if else
    End_State
              
    State(6)
      if ((the_current_time - the_creation_time) > (A4_Lib::One_Day_In_Seconds * this->log_archive_days))
        the_method_error = A4_Lib::Delete_File(this->log_folder + the_filename_vector[the_offset]);
      else the_method_error = this->log_file_timestamps.Insert(the_filename_vector[the_offset], the_creation_time); // save the name & timestamp for later deletion
    End_State

    State(7)
      the_offset += 1;
      Set_Target_State(the_file_loop);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();  
} // Init_Rollover_Sequence

/**
 * \brief Initialize the underlying active obejct, open the initial log file and wait for log messages to appears
 * @param the_log_filespec - IN - the entire filespec
 * @param the_log_level - IN
 * @param max_log_archive_days - IN - zero means never deleting the old files.
 * @param the_max_log_file_size - IN - must e greater than File_Logger_Constants::Min_Max_Log_Length
 * @return 
 */
Error_Code A4_Lib::File_Logger::Open (std::string      the_log_filespec,
                                      Logging::Detail  the_log_level,
                                      std::uint16_t    max_log_archive_days, // number of days the logs will be saved before deletion
                                      std::uint64_t    the_max_log_file_size)
{ // begin
  
  Method_State_Block_Begin(9)
    State(1)
      if (this->Is_Open () == true)
        the_method_error = A4_Error (A4_Log_Module_ID, File_Logger::O_Already_Open, "File_Logger singleton is already open.");
    End_State
          
    State(2)
      the_method_error = this->Init_Log_Filespec (the_log_filespec);         
    End_State
          
    State(3)
      if (the_max_log_file_size < File_Logger_Constants::Min_Max_Log_Length)
        the_method_error = A4_Error (A4_Log_Module_ID, File_Logger::O_Invalid_Max_Log_File_Size, "Invalid parameter value - the_max_log_file_size is too small.");
      else this->max_log_file_size = the_max_log_file_size;
    End_State
            
    State(4)
#ifdef A4_Lib_Windows
      the_method_error = A4_Lib::Windows::Get_Computer_Name (this->computer_name);
#else
    the_method_error = A4_Lib::Get_Linux_Computer_Name (this->computer_name);
#endif            
    End_State
            
    State(5)
      the_method_error = this->Initialize(File_Logger_Constants::Min_Num_Threads);
    End_State
        
    State(6)
      this->log_archive_days = max_log_archive_days; // does this value need testing?
    
      the_method_error = this->Init_Rollover_Sequence ();
    End_State
            
    State(7)
      the_method_error = this->Start(); // active object threads will be created
    End_State            

    State(8)
      the_method_error = this->Open_Log_File();
    End_State
            
    State(9)
      the_method_error = this->A4_Lib::Logger::Open();
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // open

/**
 * \brief Close the instance by setting the member is_open to false.
 */  
Error_Code A4_Lib::File_Logger::Close(void)
{ // begin
  std::size_t  the_count = 0;
  
  Method_State_Block_Begin(4)
    State(1)
      if (this->Is_Started () != true)
        the_method_error = A4_Error (A4_Log_Module_ID, File_Logger::C_Not_Open, "File_Logger is not open.");
    End_State
          
    State(2)
      A4_Lib::Sleep_MS(100); // allow time to write the closing entry
      this->is_closing = true; 
      
      while ((this->Message_Queue_Is_Empty() != true) && (the_count < (File_Logger_Constants::Max_Shutdown_Wait * 10)))
      {
	A4_Lib::Sleep_MS (100);
        the_count += 1; // if the messages are still there, then the thread probably crashed...
      } // while
      
      the_method_error = this->Stop();
    End_State
              
    State(3)
      if (this->log_file != NULL)
        the_method_error = Close_Log_File();
    End_State
            
    State(4)
      the_method_error = this->A4_Lib::Logger::Close();
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Close

/**
 * \brief Open the log file with the assembled filespec peices.
 * @return No_Error, OLF_File_Already_Open, OLF_fopen_Error
 * \note Assumes that this->log_file_mutex is held by the calling method or is not required to be held.
 */
Error_Code  A4_Lib::File_Logger::Open_Log_File (void)
{ // begin
  std::string   the_filespec;
  std::string   the_format_mask;
  
#ifdef A4_Lib_Windows
  the_format_mask = "%s\\%s-%05d%s";
#else
  the_format_mask = "%s%s-%05d%s";  
#endif
  
  Method_State_Block_Begin(3)
    State(1)  
      if (this->log_file != NULL)
        the_method_error = A4_Error (A4_Log_Module_ID, File_Logger::OLF_File_Already_Open, "Invalid member state - this->log_file is not NULL.");
      else the_method_error = A4_Lib::SNPrintf(the_filespec, the_format_mask, A4_Lib::File_Logger_Constants::Max_Filename_Length, 
                                               this->log_folder.c_str(), this->log_filename.c_str(), static_cast<int>(this->rollover_sequence.fetch_add(1)), log_extension.c_str());
    End_State
          
    State(2)
      this->log_file = std::fopen (the_filespec.c_str(), "a");
    
      if (this->log_file == NULL)
        the_method_error = A4_Error (A4_Log_Module_ID, File_Logger::OLF_fopen_Error, A4_Lib::Logging::Error,
                                     "Call to std::fopen resulted in error %d for filespec %s", errno, the_filespec.c_str());
      else the_method_error = this->Make_Opening_Entry();
    End_State
            
    State(3)
      this->file_creation_time = A4_Lib::Now();
      this->last_rollover_time = this->file_creation_time;
      
      the_method_error = this->log_file_timestamps.Insert (the_filespec, this->file_creation_time);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();  
} // Open_Log_File

/**
 * \brief Close this->log_file - Assumes that this->log_file_mutex is held by the calling method or is not required to be held.
 * @return No_Error, 
 */
Error_Code  A4_Lib::File_Logger::Close_Log_File (void)
{
  Method_State_Block_Begin(2)
    State(1) 
      if (this->log_file == NULL)
      { // file is not open - not fatal, but not good
        std::cout << "A4_Lib::File_Logger::Close_Log_File called for a file that is not open.";
        Terminate_The_Method_Block;
      } // if then
      else this->Make_Closing_Entry ();
    End_State
          
    State(2)
      (void) std::fflush(this->log_file); 
    
      if (std::fclose (this->log_file) != 0)
        the_method_error = A4_Error (A4_Log_Module_ID, File_Logger::CLF_fclose_Error, A4_Lib::Logging::Error,
                                     "Call to std::fclose resulted in error %d ", errno);
      this->log_file = NULL; // even when fclose fails
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} // Close_Log_File

/**
 * \brief Retrieves the current singleton shared pointer.
 * @return the type-cast shared pointer to this file logging singleton.
 */
A4_Lib::File_Logger::Pointer  A4_Lib::File_Logger::Instance(void)
{ // begin
  return std::static_pointer_cast<A4_Lib::File_Logger>(A4_Lib::Logger::Instance());
} // Instance

/**
 * \brief Make & enqueue the opening header log message.
 * @return 
 */
Error_Code    A4_Lib::File_Logger::Make_Opening_Entry (void)
{ // begin
  std::string   the_log_message;
  
  Method_State_Block_Begin(2)
    State(1)
      the_method_error = A4_Lib::SNPrintf (the_log_message, "Log %s opened on Computer %s\r\n", Max_Error_Message_Length, this->log_filename.c_str(), this->computer_name.c_str());
    End_State

    State(2)
      if (this->header_text.length() > 0)
        the_log_message += (this->header_text + "\r\n");
      else the_log_message += "\r\n";
  
      the_method_error = this->Format_and_Enque_Message (the_log_message, A4_Lib::Logging::Private_Comment, true);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} //end Make_Opening_Entry

/**
* \brief  Create a text entry used as the last line of the log file. 
*/
Error_Code    A4_Lib::File_Logger::Make_Closing_Entry (void)
{ // begin
  std::string     the_log_message = "** Log Closed **\r\n=====================================================\r\n\r\n";
  
  Method_State_Block_Begin(1)
    State(1)
      the_method_error = this->Format_and_Enque_Message (the_log_message, A4_Lib::Logging::Private_Comment, true);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();;
} //end Make_Closing_Entry

      
/**
 * \brief Allocate a new singleton instance and assign the address to the member pointer.
 * @return 
 */
Error_Code A4_Lib::File_Logger::Allocate_Singleton (void)
{ // begin
  std::shared_ptr<A4_Lib::File_Logger>  the_new_instance;
  
  Method_State_Block_Begin(2)
    State(1)  
      if (A4_Lib::Logger::Instance() != nullptr)
        the_method_error = A4_Error (A4_Log_Module_ID, AS_Already_Allocated, "The logging singleton has already been allocated.");
    End_State
                
    State(2)
      the_new_instance = std::make_shared<A4_Lib::File_Logger>();
  
      if (the_new_instance == nullptr)
        the_method_error = A4_Error (A4_Log_Module_ID, AS_Allocation_Error, "Memory allocation error - could not allocate a new A4_Lib::File_Logger instance.");
      else the_method_error = the_new_instance->Set_Instance_Pointer(the_new_instance);
    End_State
  End_Method_State_Block

  A4_Cleanup_Begin
    if (the_new_instance != nullptr)
      the_new_instance.reset();
  A4_End_Cleanup
            
  return the_method_error.Get_Error_Code();  
} // Allocate_Singleton

/**
 * \brief set the text used with opening a log file.
 * \param the_header_text - IN - must be shorter than A4_Lib::File_Logger_Constants::Max_Header_Text_Length)
 */
Error_Code  A4_Lib::File_Logger::Set_Log_Header_Text (std::string the_header_text)
{ // begin
  Method_State_Block_Begin(1)
    State(1)
      if (the_header_text.length() < A4_Lib::File_Logger_Constants::Max_Header_Text_Length)
        this->header_text = the_header_text;
      else the_method_error = A4_Error (A4_Log_Module_ID, SLHT_Text_Too_Long, "Invalid parameter length - the_header_text is too long.");
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Set_Log_Header_Text

/**
 * \brief Write the contents of the_message_block to the open log file
 * @param the_message_block - IN - contains a log message to write / OUT - nullptr
 * @return No_Error upon success.
 */
Error_Code  A4_Lib::File_Logger::Process_Message (A4_Lib::Message_Block::Pointer   &the_message_block) 
{ // begin
  std::string  the_log_message;
  
  bool  the_mutex_is_locked = false;
  
  Method_State_Block_Begin(5)
    State(1)
      if (this->Is_Started () != true)
      { // begin
        std::cerr << "Logging will be lost because the logger is not started, but A4_Lib::File_Logger::Process_Message has been called.";
        Terminate_The_Method_Block; // all done, although logging is lost
      } // if then
    End_State
          
    State(2)
      if (the_message_block == nullptr)
        the_method_error = A4_Error (A4_Log_Module_ID, PM_Invalid_Message_Block, "Invalid parameter address - the_message_block == nullptr");
      else the_method_error = the_message_block->Get_Data(the_log_message);
    End_State
          
    State(3)
      if (the_log_message.length() > 0)
        the_method_error = this->log_file_mutex.Lock (the_mutex_is_locked);
      else { // no message to write
        std::cerr << "An empty log message string was retrieved from the_message_block";
        Terminate_The_Method_Block;
      } // if else
    End_State
            
    State(4)
      if (std::fwrite(the_log_message.c_str(), 1, the_log_message.length(), this->log_file) != the_log_message.length())
      { // failure
        std::cerr << "fwrite did not write the entire message length - out of storage space?";
      } // if then
      else { // test whether the file has grown too much
        if (std::ftell (this->log_file) > static_cast<long int>(this->max_log_file_size))
          the_method_error = Rollover_Log_File (A4_Lib::Now ()); // perhaps the logs volume is high enough where Handle_Timeout method is not called.
        } // if else
    End_State
            
    State(5)
      the_method_error = this->log_file_mutex.Unlock (the_mutex_is_locked);
    End_State
  End_Method_State_Block

  A4_Cleanup_Begin
    if (the_mutex_is_locked == true)
      (void) this->log_file_mutex.Unlock (the_mutex_is_locked);

    if (the_message_block != nullptr)
      the_message_block.reset();
  A4_End_Cleanup
            
  return the_method_error.Get_Error_Code();
} // Process_Message

/**
 * Perform necessary admin (log rollover) during idle periods - 
 * @return No_Error upon success.
 */
Error_Code  A4_Lib::File_Logger::Handle_Timeout (void)
{ // begin
  std::time_t  the_time = 0;
  
  Method_State_Block_Begin(1)
    State(1) // first address any base class admin.
      the_method_error = this->Active_Object::Handle_Timeout();
    End_State
          
    State(2)
      the_time = A4_Lib::Now();
    
      if ((the_time - this->last_flush_time) > File_Logger_Constants::Buffer_Flush_Interval)
        the_method_error = this->Flush_File_Buffer (the_time);
    End_State
            
    State(3)
      if ((the_time - this->last_rollover_time) > File_Logger_Constants::Rollover_Check_Interval)
        the_method_error = this->Rollover_Log_File (the_time);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Handle_Timeout

/**
 * \brief Flush the file buffer periodically to ensure everything is committed to the output file.
 * @param the_current_time - IN
 * @return 
 */
Error_Code  A4_Lib::File_Logger::Flush_File_Buffer(std::time_t  the_current_time)
{ // begin
  bool  the_mutex_is_locked = false;
  
  Method_State_Block_Begin(2)
    State(1)  
      the_method_error = this->log_file_mutex.Lock(the_mutex_is_locked);
    End_State
          
    State(2)
      if (the_mutex_is_locked != true)
        Terminate_The_Method_Block; // maybe an issue, maybe not
      else {
        this->last_flush_time = the_current_time;
        (void) std::fflush(this->log_file);               
      } // if else
    
      the_method_error = this->log_file_mutex.Unlock (the_mutex_is_locked);
    End_State
  End_Method_State_Block

  if (the_mutex_is_locked == true)
    (void) this->log_file_mutex.Unlock (the_mutex_is_locked);
    
  return the_method_error.Get_Error_Code();  
} // Flush_File_Buffer

/**
 * \brief Close the current log file and open the next file with incremented sequence number
 * @param the_current_time - IN
 * @return 
 */
Error_Code  A4_Lib::File_Logger::Rollover_Log_File (std::time_t  the_current_time)
{ // begin
  bool  the_mutex_is_locked = false;
  
  Method_State_Block_Begin(6)
    State(1)  
      the_method_error = this->log_file_mutex.Lock(the_mutex_is_locked);
    End_State
          
    State(2) 
      if (the_mutex_is_locked != true)
        Terminate_The_Method_Block; // maybe an issue, maybe not
      else this->last_rollover_time = the_current_time;
    End_State
            
    State(3)
      if (((the_current_time - this->file_creation_time) > A4_Lib::One_Day_In_Seconds) || (static_cast<std::size_t>(std::ftell (this->log_file)) > this->max_log_file_size))
        the_method_error = this->Close_Log_File (); // time to rollover the log
      else { // not yet time to rollover the log file
        the_method_error = this->log_file_mutex.Unlock (the_mutex_is_locked);
        Terminate_The_Method_Block;
      } // if else
    End_State
            
    State(4)
      the_method_error = this->Open_Log_File (); 
    End_State
            
    State(5)
      the_method_error = this->Delete_Expired_Log_Files (the_current_time);
    End_State
            
    State(6)
      the_method_error = this->log_file_mutex.Unlock (the_mutex_is_locked);
    End_State
  End_Method_State_Block

  if (the_mutex_is_locked == true)
    (void) this->log_file_mutex.Unlock (the_mutex_is_locked);
    
  return the_method_error.Get_Error_Code();    
} // Rollover_Log_File

/**
 * \brief Iterate through the log_file_timestamps member map and delete any log files older than the retention time.
 * @param the_current_time - IN
 * @return 
 */
Error_Code  A4_Lib::File_Logger::Delete_Expired_Log_Files(std::time_t  the_current_time)
{ // begin
  Map_Type::Iterator it;// = this->log_file_timestamps.Begin();
    
  std::time_t  the_timeout = this->log_archive_days * A4_Lib::One_Day_In_Seconds;
  
  Method_State the_map_loop = 0;
  
 
  Method_State_Block_Begin(6)
    State(1)
      the_method_error = this->log_file_timestamps.Begin(it);
    End_State
          
    State(2)
      Define_Target_State (the_map_loop);
    End_State
          
    State(3)
      if (it == this->log_file_timestamps.End())
        Terminate_The_Method_Block;
    End_State
            
    State(4)
      if ((the_current_time - it->second ) > the_timeout)
      { // begin
        the_method_error = A4_Lib::Delete_File (this->log_folder + it->first);
      } // if then
      else { // move to the next map item
        it++;
        Set_Target_State (the_map_loop);
      } // if else
    End_State
            
    State(5)
      the_method_error = this->log_file_timestamps.Erase (it->first);
    End_State
            
    State(6)
      //the_method_error = this->log_file_timestamps.Begin (it);
       the_method_error = this->log_file_timestamps.Begin(it); // iterator is now invalid - start again
       Set_Target_State (the_map_loop);
    End_State
  End_Method_State_Block  
  
  return the_method_error.Get_Error_Code();   
} // Delete_Expired_Log_Files

/**
 * 
 * @return true if the logger is closing - only messages currently in the queue will be processed, new Write requests will be ignored.
 */
bool A4_Lib::File_Logger::Is_Closing(void) const
{ // begin
  return this->is_closing;
} // Is_Closing

/**
 * \brief Generic write method - used in the End_Method_State_Block macro
 * @param the_log_text - IN
 * @param the_calling_function_name - IN
 * @param the_error - IN
 * @param the_state - IN
 * @param the_message_detail_level - IN
 * @return No_Error upon success.
 */
Error_Code  A4_Lib::File_Logger::Write(std::string       the_log_text,
                                       std::string       the_calling_function_name,
                                       Error_Code        the_error,
                                       Method_State      the_state,
                                       Logging::Detail   the_message_detail_level)
{ // begin
  std::string  the_formatted_log_text;
  
  Method_State_Block_Begin(2)
    State(1) 
      if (((the_message_detail_level > this->Get_Detail_Level()) && (the_message_detail_level != A4_Lib::Logging::Module_Specific)) || (this->Is_Closing() == true)) // always want to see the module-specific stuff
        Terminate_The_Method_Block; // message doesn't need logging
      else the_method_error = A4_Lib::SNPrintf (the_formatted_log_text,  "%s - Error Code %1.5f returned from %s at machine state %d", 
                                                Max_Error_Message_Length, 
                                                the_log_text.c_str(), A4_Error::Get_Dot_Error_Code(the_error), the_calling_function_name.c_str(), static_cast<int>(the_state));
    End_State
          
    State(2)
      the_method_error = this->Format_and_Enque_Message (the_formatted_log_text, the_message_detail_level);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Write

/**
 * \brief Write a log entry without formatting it - these are mostly used for Info messages.
 * @param the_log_text - IN
 * @param the_message_detail_level - IN
 */
Error_Code A4_Lib::File_Logger::Write (std::string       the_log_text,
                                       Logging::Detail   the_message_detail_level)
{
  Method_State_Block_Begin(2)
    State(1)   
      if (the_log_text.length() < 1)
      { // no text to write
        std::cout << "Invalid parameter calling A4_Lib::File_Logger::Write(string, Detail) - the_log_text was emoty.";
        Terminate_The_Method_Block;
      } // if then
    End_State
          
    State(2)
      if (((the_message_detail_level > this->Get_Detail_Level()) && (the_message_detail_level != A4_Lib::Logging::Module_Specific)) || (this->Is_Closing() == true))  // always want to see the module-specific stuff
        Terminate_The_Method_Block; // message doesn't need logging   
      else the_method_error = this->Format_and_Enque_Message (the_log_text, the_message_detail_level);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();  
} // Write

/**
 * \brief synchronousy write this->log_file
 * @param the_log_message - IN
 * @return No_Error, IW_Log_File_Not_Open, IW_FWrite_Error
 */
Error_Code  A4_Lib::File_Logger::Internal_Write (std::string   &the_log_message)
{
  Method_State_Block_Begin(2)
    State(1) 
      if (this->log_file == NULL)
        the_method_error = A4_Error (A4_Log_Module_ID, IW_Log_File_Not_Open, "Invalid state - the log file is not open.");
    End_State
          
    State(2)
      if (std::fwrite(the_log_message.c_str(), 1, the_log_message.length(), this->log_file) != the_log_message.length())
        the_method_error = A4_Error (A4_Log_Module_ID, IW_FWrite_Error, "Call to std::fwrite failed - enough storage space?");
    End_State
  End_Method_State_Block
            
  return the_method_error.Get_Error_Code();   
} // Internal_Write

/**
 * \brief Format the_log_text and insert it into the active object message queue - the log text twill be written by one of the worker threads.
 * @param the_log_text - IN
 * @param the_message_detail_level - IN
 * @param is_high_prio_prepend - IN
 * @return 
 */
Error_Code  A4_Lib::File_Logger::Format_and_Enque_Message (std::string       &the_log_text, 
                                                           Logging::Detail   the_message_detail_level,
                                                           bool              is_high_prio_prepend)
{ // begin
  A4_Lib::Message_Block::Pointer  the_msg_block;
  
  std::string  the_detail_level_string;
  std::string  the_formatted_text;
  std::string  the_timestamp;
  
  std::int64_t the_sequence = 0;
  
  Method_State_Block_Begin(7)
    State_NoTry(1) 
      if ((the_message_detail_level > this->Get_Detail_Level())  && (the_message_detail_level != A4_Lib::Logging::Module_Specific)) // always want to see the module-specific stuff
        Terminate_The_Method_Block; // message doesn't need logging
    End_State_NoTry
          
    State(2) 
      the_method_error = this->Get_Detail_Level_String (the_message_detail_level, the_detail_level_string);
    End_State
          
    State(3)
      the_method_error = A4_Lib::High_Res_Timestamp_String (the_timestamp);
    End_State
            
    State(4)
      the_sequence = this->sequence_number.fetch_add(1); // atomic increment the log sequence
    
      if (the_message_detail_level == A4_Lib::Logging::Content_Dump)
        the_method_error = A4_Lib::SNPrintf (the_formatted_text, "%lld: %s", A4_Lib::Max_Error_Message_Length, the_sequence, the_log_text.c_str());
      else the_method_error = A4_Lib::SNPrintf (the_formatted_text, "%s (%06jd) %s - %s\r\n", A4_Lib::Max_Error_Message_Length, 
                                                the_detail_level_string.c_str(), the_sequence, the_timestamp.c_str(), the_log_text.c_str());
    End_State
            
    State(5)
      if (is_high_prio_prepend == false)
        the_method_error = A4_Lib::Message_Block::Allocate(the_msg_block);
      else { // write synchronously to the log file
        the_method_error = this->Internal_Write (the_formatted_text);
        Terminate_The_Method_Block;
      } // if else
    End_State
            
    State(6)
      the_method_error = the_msg_block->Set_Data(the_formatted_text, 0);
    End_State
            
    State(7)
      the_method_error = this->Enqueue_Message(the_msg_block);
    End_State
  End_Method_State_Block

  A4_Cleanup_Begin
    if (the_msg_block != nullptr)
      the_msg_block.reset();
  A4_End_Cleanup
            
  return the_method_error.Get_Error_Code();  
} // Format_and_Enque_Message       