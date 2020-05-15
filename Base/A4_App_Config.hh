#ifndef __A4_Application_Config_Defined__
#define __A4_Application_Config_Defined__

/**
 * \brief Application configuration file. 
 * \author a. zippay * 2013..2018
 * 
 * \todo Implement periodic reloads & observer notifications.
 * \note  This Active Object will (eventually) periodically reload the configuration file. Any Observers will be Notified when changes occur.
 * 
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
 *  
 */
#include "A4_Singleton.hh"
#include "A4_Active_Object.hh"
#include "A4_Observable.hh"
#include "A4_Recursive_Mutex.hh"
#include <fstream> 

namespace App_Config_Private
{ // begin
class App_Config_Node; // private implementation
} // App_Config_Private

#define App_Config  A4_Lib::App_Configuration::Instance()

namespace A4_Lib
{ // begin
// base template error offsets
static const Error_Offset AC_Singleton_Error_Offset = 1000;
static const Error_Offset AC_Active_Object_Error_Offset = 2000;

  namespace Observer_Hint
  { // begin
    static const  A4_Lib::Observer::Hint App_Config_Modified = Observer::Make_Hint(A4_Config_File_Module_ID, 0);
  } // namespace Observer_Hint

typedef class App_Configuration : public Active_Object, public Observable
{ // begin
public: // construction - yes it's a singleton, but can also be used as a normal class instance (think runtime config) - just don't try using the singleton stuff
  A4_Export App_Configuration(void);
  A4_Export virtual ~App_Configuration(void);
  
  A4_Declare_Singleton(App_Configuration);
  
public: // types
  typedef std::deque<std::string>   String_Deque;

public: // constants
  static const std::size_t Min_Pair_Length = 3; // x=y
  static const std::size_t Min_Section_Length = 3; // [x]
  static const std::time_t Config_Reload_Check_Period = 2; // seconds - check the file last modified time, reload if it's been changed.
  
  static const bool Enable_Auto_Reload = true;
  static const bool No_Auto_Reload = false;

public: // 
  A4_Export  Error_Code  Open (std::string   the_filespec,
                               bool          enable_auto_reload = No_Auto_Reload);  
      
  A4_Export Error_Code  Open (std::wstring    the_filespec,
                              bool            enable_auto_reload = No_Auto_Reload);  
                    
  A4_Export Error_Code  Close(void);
  
  A4_Export bool        Is_Open(void) const;
  
  bool                  Section_Exists (std::wstring the_section_name);

  A4_Export Error_Code  Get_String (std::string   the_section_name, // in
                                    std::string   the_key_name,     // in
                                    std::string   &the_string,      // out
                                    bool          &is_found);       // out  
  
  A4_Export Error_Code  Get_String (std::wstring   the_section_name, // in
                                    std::wstring   the_key_name,     // in
                                    std::wstring   &the_string,      // out
                                    bool           &is_found);       // out  

  A4_Export Error_Code  Get_UINT32(std::string      the_section_name, 
                                   std::string      the_key, 
                                   std::uint32_t    &the_value, 
                                   std::uint32_t    the_default_value = 0);
                         
  A4_Export Error_Code  Get_Size_T(std::string   the_section_name,
			           std::string   the_key,
			           std::size_t   &the_value,
			           std::size_t   the_default_value);

  A4_Export Error_Code  Get_Double (std::string  the_section_name,
 			            std::string  the_key,
			            double       &the_value,
			            double       the_default_value);

  A4_Export Error_Code  Get_Section_Deque (App_Configuration::String_Deque   &the_list); // retrieve all the Sections from the file

  A4_Export Error_Code  Get_Values_By_Section (std::string                      the_section_name, // in
                                               App_Configuration::String_Deque  &the_value_name_list, // out
                                               App_Configuration::String_Deque  &the_value_list); // out

  A4_Export std::time_t Get_Last_Modified_Time (void) const;

  virtual Error_Code  Handle_Timeout(void) override;
  
private: // methods
  Error_Code  Reload_Config_Job (void);    
  
  Error_Code  Read_Configuration (std::ifstream     &the_stream);
  
  Error_Code  Insert_New_Node(App_Config_Private::App_Config_Node   *&the_new_node, 
                              std::string       &the_current_section);

  Error_Code  Find_Section (std::string   &the_section_name,
                            std::size_t   &the_section_offset,
                            bool          &is_found);
  
  Error_Code  Find_Section (std::wstring   &the_section_name,
                            std::size_t    &the_section_offset,
                            bool           &is_found);

  Error_Code  Free_Config_Vector(void);
  
private: // data
  std::vector<App_Config_Private::App_Config_Node *>  config_vector;
  
  std::string               filespec;
  
  std::time_t               file_last_modified_time; // kept here to detect changes since the last load
  std::time_t               next_reload_time; // time when the file will be checked & reloaded
  
  A4_Lib::Recursive_Mutex   data_mutex;
  A4_Lib::Mutex             reload_config_job_mutex; 
  
  bool                      is_open;
  bool                      auto_reload_enabled;
  
public: // errors
  enum App_Config_Errors : Error_Offset
  { // begin
    RC_Root_Allocation_Error          = 0, /**< Memory allocation error - could not allocate the root node. */
    INN_Duplicate_Section             = 1, /**< A duplicate Section was detected. These must be unique - see log for the name */
    INN_Missing_Section               = 2, /**< The required section is missing - see log for the name */
    INN_Missing_Section2              = 3, /**< The required section is missing - see log for the name */
    INN_Unsupported_Node_Type         = 4, /**< An unsupported node type was encounterd - see the log for the enumeration value. */
    INN_Undefined_Node_Type           = 5, /**< An Undefined node type was encountered. */
    INN_Duplicate_Pair_Found          = 6, /**< A duplicate Pair was detected - see log for details */
    O_Already_Open                    = 7, /**< Instance is already open. */
    O_Empty_Fiename                   = 8, /**< Invalid parameter value - the_filespec is empty */
    O_No_File                         = 9, /**< Missing configuration file - besure to verify the filespec */ 
    O_File_Open_Failure               = 10, /**< Failed to open the configuration file */
    GS_Not_Open                       = 11, /**< Instance is not open. Not possible to read anything. */
    GS_No_Section                     = 12, /**< Invalid parameter value - the_section_name is empty */
    GS_No_Key                         = 13, /**< Invalid parameter value - the_key_name is empty */
    RL_Invalid_Section_Length         = 14, /**< n Invalid Section Length was encountered */
    RL_Invalid_Pair_Length            = 15, /**< An invalid Value Name / Value was encountered */
    RL_Allocation_Error               = 16, /**< Memory allocation error - could not allocate a new App_Config_Node */
    RL_Unsupported_Node_Type          = 17, /**< An invalid Node Type was encountered */
    RCJ_File_Open_Failure             = 18, /**< Failed to open the configuration file. */
    C_Not_Open                        = 19, /**< This instance is not open. */
    GSD_Not_Open                      = 20, /**< Invalid state - the instance needs to be Open. */
    GVBS_No_Section_Name              = 21, /**< Invalid parameter length - the_section_name is empty. */
    GVBS_Not_Open                     = 22, /**< Invalid state - the instance needs to be Open. */
    GVBS_Section_Not_Found            = 23, /**< Section %s was not found in the configuration file */
  }; // App_Config_Errors
} App_Configuration;
} // namespace A4_Lib
#endif // __A4_Application_Config_Defined__


