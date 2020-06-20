#ifndef __A4_File_Logger_Defined__
#define __A4_File_Logger_Defined__
/**
* \author Albert Zippay * 1995..2020
* \copyright Albert Zippay - 1995 - 2020
* \file A4_File_Logger.hh
*
* \brief  Logging base class interface for all A4 Library objects.
* \note   The idea here is to allow multiple forms (File, ELK, etc) of logging via one common virtual interface.
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

#include "A4_Logger.hh"
#include "A4_Active_Object.hh"
#include "A4_Recursive_Mutex.hh"
#include "A4_Unordered_Map_T.hh"
#include <atomic>

#define A4_File_Log  A4_Lib::File_Logger::Instance()

namespace A4_Lib
{ // begin

  namespace File_Logger_Constants
  { // begin
    static const Error_Offset Timestamp_Map_Error_Offset = 100;
    
    const std::size_t     Max_Header_Text_Length = 256; /**< in characters - including null terminator */
    const std::int64_t    Max_Milli_Second_Lock_Wait = 500; // 
    const std::size_t     Min_Num_Threads = 2; /**< The minimum number of threads that should be running for this logger - 1 for writing, one for admin. */
    const std::size_t     Max_Error_Code_Length = 32; /**< conversion buffer length */
    const std::size_t     Min_Log_Filename_Length = 10; /**< A log filespec should include this many character, or it will be deemed incorrect */
    const std::uint16_t   Default_Log_Archive_Days = 3; /**< Number of days to keep older log files */
    const std::uint64_t   Default_Max_Log_Length = 10485760; /**< 10MB - can be larger */
    const std::uint64_t   Min_Max_Log_Length = 1048576; /**< why have the maximum size smaller than this value? */
    const std::size_t     Max_Filename_Length = 256; /**< Max length of the file-spec - this could be longer if the use case requires it. */
    const std::size_t     Max_Shutdown_Wait = 30; /**< if it takes longer than this number of seconds to stop, then data will be lost */
    const std::time_t     Buffer_Flush_Interval = 10; /**< The max. number of seconds a log message can be written, but not flushed to storage. */
    const std::time_t     Rollover_Check_Interval = 15; /**< The number of seconds between testing whether old log files need deleting */
  } // namespace File_Logger_Constants

  typedef class File_Logger : public A4_Lib::Logger,
                              protected A4_Lib::Active_Object
  { // begin
    public: // construction
      File_Logger(void); // 
      virtual ~File_Logger (void);

    public: // types
      typedef std::shared_ptr<File_Logger>  Pointer; 
      typedef A4_Lib::Unordered_Map_T <std::string, std::time_t, A4_File_Logger_Module_ID, File_Logger_Constants::Timestamp_Map_Error_Offset>  Map_Type;
      
    public: // required virtual methods
      static Error_Code Allocate_Singleton (void); // allocate a new singleton instance - 
      
      A4_Export virtual Error_Code  Write(std::string       the_log_text,
                                          std::string       the_calling_function_name,
                                          Error_Code        the_error,
                                          Method_State      the_state,
                                          Logging::Detail   the_message_detail_level) override;
      
      A4_Export virtual Error_Code Write (std::string       the_log_text,
                                          Logging::Detail   the_message_detail_level) override;
      
      A4_Export virtual Error_Code Write (A4_Lib::Logging::Detail    the_message_detail_level,
                                          std::string                the_log_text,
                                          ...) override;
      
      virtual Error_Code  Open (std::string        the_log_filespec,
                                Logging::Detail	   the_log_level = Logging::Debug,
                                std::uint16_t      max_log_archive_days = File_Logger_Constants::Default_Log_Archive_Days, // number of days the logs will be saved before deletion
                                std::uint64_t      the_max_log_file_size = File_Logger_Constants::Default_Max_Log_Length);
      
      virtual Error_Code  Close(void) override;
      
      static File_Logger::Pointer  Instance(void);
      
      bool Is_Closing(void) const;
      
    public: // file logging specific methods
      A4_Export Error_Code    Set_Log_Header_Text (std::string the_header_text);
      
    protected: // overrides
      virtual   Error_Code  Process_Message (A4_Lib::Message_Block::Pointer   &the_message_block) override;
      virtual   Error_Code  Handle_Timeout (void) override;
      
    private: // methods
      Error_Code  Format_and_Enque_Message (std::string       &the_log_text, 
                                            Logging::Detail   the_message_detail_level,
                                            bool              is_high_prio_prepend = false);
      
      Error_Code  Init_Log_Filespec (std::string  the_log_filespec);
      Error_Code  Init_Rollover_Sequence(void);
      
      Error_Code  Open_Log_File (void);
      Error_Code  Close_Log_File (void);
      Error_Code  Make_Closing_Entry (void);
      Error_Code  Make_Opening_Entry (void);
      
      Error_Code  Flush_File_Buffer(std::time_t  the_current_time); // log will be flushed at a regular interval - not after every write
      
      Error_Code  Rollover_Log_File (std::time_t  the_current_time);
      
      Error_Code  Delete_Expired_Log_Files(std::time_t  the_current_time);
      
      Error_Code  Internal_Write (std::string   &the_log_message);
      
    private: // data
      std::FILE                 *log_file; /**< The file instance that will be appended with new log entries - deliberately \b not a unique_ptr */
      A4_Lib::Recursive_Mutex   log_file_mutex; /**< Keeps this->log_file thread safe */
      
      A4_Lib::String_Vector	filespec_vector; /**< splits the filespec into folders / filename.ext */
      A4_Lib::String_Vector     filename_vector; /**< splits the filename into filename / .ext */
      
      Map_Type                  log_file_timestamps; /**< map containing the log filename and the creation date */
      
      std::atomic_int_fast64_t  sequence_number; /**< unique number for each log entry */
      std::atomic_uint16_t      rollover_sequence; /**< the sequence number appended to the log filename */
      std::string               header_text; /**< text that will appear at the beginning of each log file */
      std::string               computer_name; /**< name of the computer running this software */
      std::string               log_folder; /**< The folder where the log file is located */
      std::string               log_filename; /**< the log filename, without sequence number */
      std::string               log_extension; /**< The log file extension */
      
      std::uint64_t             max_log_file_size; /**< The maximum size the log file will be allowed to grow before it is closed and the next sequence opened */
      
      std::time_t               log_archive_days; /**< The number of days a log file will be allowed to stay in storage. A value of zero means forever */
      
      std::time_t               last_flush_time; /**< The last time (in seconds) the log file was flushed */
      std::time_t               file_creation_time; /**< The log file creation time */
      std::time_t               last_rollover_time; /**< The last time the logs were tested for rolling over */
      
      bool                      is_closing; /**< if \b true, this File_Logger instance is closing and no new log entries will be accepted. */
      
    public: // errors
      enum File_Logger_Errors /**< Errors unique to File_Logger */
      { // begin
        SLHT_Text_Too_Long                = 0, /**< \b Set_Log_Header_Text: Invalid parameter length - the_header_text is too long - must be < File_Logger_Constants::Max_Header_Text_Length */
        AS_Already_Allocated              = 1, /**< \b Allocate_Singleton: The logging singleton has already been allocated */
        AS_Allocation_Error               = 2, /**< \b Allocate_Singleton: Memory allocation error - could not allocate a new A4_Lib::File_Logger instance. */
        PM_Invalid_Message_Block          = 3, /**< \b Process_Message: Invalid parameter address - the_message_block == nullptr */
        O_Already_Open                    = 4, /**< \b Open: File_Logger singleton is already open. */
        C_Not_Open                        = 5, /**< \b Close: File_Logger is not open. */
        ILF_Invalid_Filespec_Length       = 6, /**< \b Init_Log_Filespec: Invalid parameter length - the_log_filespec is too short. */
        O_Invalid_Max_Log_File_Size       = 7, /**< \b Open: Invalid parameter value - the_max_log_file_size is too small. */
        ILF_Invalid_Filespec_Vector_Size  = 8, /**< \b Init_Log_Filespec: Parsing the_log_filespec resulted in a parsed vector that is too short. */
        ILF_Invalid_Filename_Vector_Size  = 9, /**< \b Init_Log_Filespec: Parsing the file name resulted in a parsed vector that is too short. */
        OLF_File_Already_Open             = 10, /**< \b Open_Log_File: Invalid member state - this->log_file is not NULL. */
        OLF_fopen_Error                   = 11, /**< \b Open_Log_File: Call to std::fopen resulted in error */
        CLF_fclose_Error                  = 12, /**< \b Close_Log_File: all to std::fclose resulted in error */
        IW_Log_File_Not_Open              = 13, /**< \b Internal_Write: Invalid state - the log file is not open. */
        IW_FWrite_Error                   = 14, /**< \b Internal_Write: Call to std::fwrite failed - enough storage space? */
        W2_Invalid_Text_Length            = 15, /**< \b Write (2-parameter): Invalid parameter length - the_log_text is empty. */
        W3_Invalid_Text_Length            = 16, /**< \b Write (variadic): Invalid parameter length - the_log_text is empty. */
      }; // File_Logger_Errors
  } File_Logger;
} // namespace A4_Lib

 #endif // __A4_File_Logger_Defined__
