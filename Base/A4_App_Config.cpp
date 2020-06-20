/**
 * \brief Application configuration file. 
 * \author a. zippay * 2017..2020
 * \file A4_App_Config.cpp
 * \note  This Active Object will periodically reload the configuration file. Any Observers will be Notified.
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

#ifdef A4_Lib_Windows
#include "Stdafx.h"
#endif

#define A4_App_Config_Build
#include "A4_App_Config.hh"
#undef A4_App_Config_Build

#include "A4_Lib_Base.hh"
#include "A4_Method_State_Block.hh" 
#include "A4_Utils.hh"
#include "A4_File_Util.hh"

using namespace A4_Lib;
  
namespace App_Config_Private
{ // begin
static const char *Config_Root_Section = "__root__section__";

  std::size_t   Num_Threads = 2;
////////////////////////////////////////////////  private App_Config_Node class implementation  ////////////////////////////////////////////////

/**
 * \brief Application Configuration Node.
 * \note  A simple recursive tree.  
 */
typedef class App_Config_Node
{ // begin
public: // construction
  App_Config_Node(void);
  virtual ~App_Config_Node(void);
  
public: // enumerations   
  enum Node_Type
  { // begin
    Undefined = 0,
    Comment   = 1,
    Section   = 2,
    Pair      = 3,
  }; // Node_Type
  
public: // methods 
  static Error_Code Read_Line (std::ifstream      &the_stream,
                               App_Config_Node    *&the_new_node,
                               bool               &is_eof);
  
public: // data  
  Node_Type     node_type;
  
  std::string   node_lable; // left side of equal sign
  std::string   node_value; // right side of equal sign 
} App_Config_Node; 

/**
 * \brief Default constructor
 */
App_Config_Node::App_Config_Node(void)
{ // begin
  this->node_type = App_Config_Node::Node_Type::Undefined;
} // constructor

/**
 * \brief Default destructor
 */
App_Config_Node::~App_Config_Node(void)
{ // begin
  this->node_type = App_Config_Node::Node_Type::Undefined;
  this->node_lable.clear();
  this->node_value.clear();
} // destructor

/**
 * \brief Read a single line from the configuration file
 * @param the_stream - IN/OUT
 * @param the_new_node - OUT - not NULL if an App_Config_Node was successfully extracted
 * @param is_eof - OUT - true if the end-of-file was reached
 * @return No_Error, RL_Invalid_Section_Length, RL_Invalid_Pair_Length, RL_Allocation_Error, RL_Unsupported_Node_Type
 */
Error_Code App_Config_Node::Read_Line (std::ifstream      &the_stream,
                                       App_Config_Node    *&the_new_node,
                                       bool               &is_eof)
{ // begin
  std::string   the_line;
  
  int           the_retry_loop = 0;
  
  App_Config_Private::App_Config_Node::Node_Type    the_node_type = App_Config_Private::App_Config_Node::Node_Type::Undefined;
  
  Method_State_Block_Begin(8)
    State(1) 
      is_eof = false;
      the_new_node = NULL;
      
      the_new_node = NULL;
  
      if (the_stream.eof() == true)
      { // end of file
        is_eof = true;
        
        Terminate_The_Method_Block;
      } // if then     
    End_State
    
    State(2)
      Define_Target_State(the_retry_loop);
    End_State
      
    State(3)
      std::getline(the_stream, the_line);
    
      if (the_line.length() < 1)
      { // nothing from file - eof
        if (the_stream.eof() == true)
        { // it really is the end
          is_eof = true;
        
          Terminate_The_Method_Block;
        } // if then
        else{ // probably just a blank line
          Set_Target_State(the_retry_loop);
        } // if elseApp_Configuration::Min_Pair_Length
      } // if then
    End_State
      
    State(4)
      the_line = A4_Lib::Left_Trim (A4_Lib::Right_Trim (the_line)); // remove leading / trailing white space
    
      if (the_line.length() < 1)
        Terminate_The_Method_Block; // nothing left
      else (void) App_Log->Write (A4_Lib::Logging::Content_Dump, "Config File found: %s", the_line.c_str());
    End_State
      
    State(5)
      switch (the_line.front())
      { // begin
        case ';': // leading comment characters
        case '#':
          the_node_type = App_Config_Private::App_Config_Node::Node_Type::Comment;
        break;
        
        case '[': //Section
          the_node_type = App_Config_Private::App_Config_Node::Node_Type::Section;
          
          if (the_line.length() < App_Configuration::Min_Section_Length)
            the_method_error = A4_Error (A4_Config_File_Module_ID, App_Configuration::RL_Invalid_Section_Length, "An Invalid Section Length was encountered");
        break;
        
        default: // the rest will need to be a pair
          the_node_type = App_Config_Private::App_Config_Node::Node_Type::Pair;
          
          if (the_line.length() < App_Configuration::Min_Pair_Length)
            the_method_error = A4_Error (A4_Config_File_Module_ID, App_Configuration::RL_Invalid_Pair_Length, "An invalid Value Name / Value was encountered");
        break;
      } // switch
    End_State
        
    State(6)
      the_new_node = new (std::nothrow) App_Config_Node;
    
      if (the_new_node == NULL)
        the_method_error = A4_Error (A4_Config_File_Module_ID, App_Configuration::RL_Allocation_Error, "Memory allocation error - could not allocate a new App_Config_Node");
      else the_new_node->node_type = the_node_type;
    End_State
      
    State(7)
      switch(the_new_node->node_type)
      { // begin
        case App_Config_Private::App_Config_Node::Node_Type::Comment:
          the_new_node->node_value = the_line;
          Terminate_The_Method_Block;
        break;
        
        case App_Config_Private::App_Config_Node::Node_Type::Section:
          the_new_node->node_lable = the_line.substr(1, the_line.length() - 2); // strip off the brackets
          Terminate_The_Method_Block;
        break;
        
        case App_Config_Private::App_Config_Node::Node_Type::Pair:
          the_method_error = A4_Lib::Left_Value (the_line, the_new_node->node_lable);
        break;
        
        default:
          the_method_error = A4_Error (A4_Config_File_Module_ID, App_Configuration::RL_Unsupported_Node_Type, "An invalid Node Type was encountered");
        break;
      } // switch
    End_State
        
    State(8)
      the_method_error = A4_Lib::Right_Value (the_line, the_new_node->node_value);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();    
} // Read_Line
} // namespace App_Config_Private

////////////////////////////////////////////////    App_Configuration Implementation  ////////////////////////////////////////////////


/**
 * \brief Default constructor
 */
App_Configuration::App_Configuration(void) 
{ // begin
  this->is_open = false;
  this->file_last_modified_time = 0;
  this->next_reload_time = 0;
  this->auto_reload_enabled = App_Configuration::No_Auto_Reload;
} // constructor

/**
 * \brief Default destructor
 */
App_Configuration::~App_Configuration(void)
{ // begin
  if (this->Is_Open() == true)
    (void) this->Close();
  
  this->filespec.clear();
  this->auto_reload_enabled = App_Configuration::No_Auto_Reload;
} // destructor

/**
 * \brief   Simple linear search.
 * @param the_section_name - IN
 * @param the_section_offset - OUT
 * @param is_found - OUT
 * 
 * \note  If performance becomes an issue, a section map may need creating...
 * @return 
 */
Error_Code  App_Configuration::Find_Section (std::string   &the_section_name,
                                             std::size_t   &the_section_offset,
                                             bool          &is_found)
{ // begin
  std::size_t    the_offset = 0;
  
  std::string  the_string;

  Method_State_Block_Begin(1)
    State(1)   
      is_found = false;
      the_section_offset = 0;
      
      while ((the_offset < this->config_vector.size()) && (is_found == false))
      { // begin
        the_string = this->config_vector[the_offset]->node_lable;

        if ((this->config_vector[the_offset]->node_type == App_Config_Private::App_Config_Node::Node_Type::Section) && (this->config_vector[the_offset]->node_lable == the_section_name))
        { // found it
          is_found = true;
          the_section_offset = the_offset;
        } // if then
        else the_offset += 1;
      } // while
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();    
} // Find_Section


Error_Code  App_Configuration::Find_Section (std::wstring   &the_section_name,
                                             std::size_t    &the_section_offset,
                                             bool           &is_found)
{ // begin
std::string   the_a_section_name;

  Method_State_Block_Begin(2)
    State(1)
      the_method_error = A4_Lib::WString_To_AString(the_section_name, the_a_section_name);
    End_State

    State(2)
      the_method_error = this->Find_Section (the_a_section_name, the_section_offset, is_found);
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code(); 
} // Find_Section


/**
* \brief  Test whether a specific Secion within the configuration file exists.
* \param  the_section_name - IN
*/
bool   App_Configuration::Section_Exists(std::wstring the_section_name)
{
  std::size_t    the_section_offset = 0; // not used
  bool           is_found = false;

  Method_State_Block_Begin(2)
    State(1)
      the_method_error = this->Find_Section (the_section_name, the_section_offset, is_found);
    End_State

    State(2)
      if ((is_found != true) && (the_section_name[0] != '['))
      { // build a proper section string
        the_section_name = (L"[" + the_section_name) + L"]";

        the_method_error = this->Find_Section (the_section_name, the_section_offset, is_found);
      } // if then
    End_State
  End_Method_State_Block
    
  return is_found; 
} // Section_Exists


/// @brief  Insert a new App_Config_Node into the config_vector
/// \param the_new_node - IN
/// \param the_current_section - IN
//
Error_Code  App_Configuration::Insert_New_Node(App_Config_Private::App_Config_Node  *&the_new_node, 
                                               std::string                          &the_current_section)
{ // begin
  std::string   the_test_value;
  
  std::size_t   the_offset = 0;
  
  bool          is_found = false;
  
  Method_State_Block_Begin(3)
    State(1)   
      the_method_error = this->Find_Section (the_current_section, the_offset, is_found);
    End_State
    
    State(2)
      switch (the_new_node->node_type)
      { // begin
        case App_Config_Private::App_Config_Node::Node_Type::Undefined:
          the_method_error = A4_Error (A4_Config_File_Module_ID, INN_Undefined_Node_Type, "An Undefined node type was encountered.");
        break;
          
        case App_Config_Private::App_Config_Node::Node_Type::Section:
          if ((is_found == true) && (the_current_section == the_new_node->node_lable))
            the_method_error = A4_Error (A4_Config_File_Module_ID, INN_Duplicate_Section, A4_Lib::Logging::Error,
                                         "A duplicate Section [%s] was detected. These must be unique.", the_current_section.c_str());
          else { // append the new section
            this->config_vector.push_back(the_new_node);
            the_current_section = the_new_node->node_lable;
            the_new_node = NULL;
            Terminate_The_Method_Block;
          } // if else
        break;
        
        case App_Config_Private::App_Config_Node::Node_Type::Pair:
          if (is_found != true)
            the_method_error = A4_Error (A4_Config_File_Module_ID, INN_Missing_Section, A4_Lib::Logging::Error,
                                         "The required section [%s] is missing.", the_current_section.c_str());
          else the_method_error = this->Get_String (the_current_section, the_new_node->node_lable, the_test_value, is_found);
        break;
          
        case App_Config_Private::App_Config_Node::Node_Type::Comment:
          if (is_found != true)
            the_method_error = A4_Error (A4_Config_File_Module_ID, INN_Missing_Section2, A4_Lib::Logging::Error,
                                         "The required section [%s] is missing.", the_current_section.c_str());
          else { // 
            this->config_vector.push_back(the_new_node);
            the_new_node = NULL;
            Terminate_The_Method_Block;
          } // if else
        break;
        
        default:
          the_method_error = A4_Error (A4_Config_File_Module_ID, INN_Unsupported_Node_Type, A4_Lib::Logging::Error, 
                                       "An unsupported node type (%d) was encountered.", static_cast<int>(the_new_node->node_type));
        break;
      } //switch
    End_State
        
    State(3)
      if (is_found == true)
        the_method_error = A4_Error (A4_Config_File_Module_ID, INN_Duplicate_Pair_Found, A4_Lib::Logging::Error, 
                                     "A duplicate Pair was detected (Section [%s] and Key %s).", the_current_section.c_str(), the_new_node->node_lable.c_str());
      else{ // append a new pair
        this->config_vector.push_back(the_new_node);
        the_new_node = NULL;
      } // if else
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();    
} //Insert_New_Node


/**
 * \brief Read the contents of a configuration file.
 * @param the_file - IN - The ifstream should already be open
 */
Error_Code  App_Configuration::Read_Configuration (std::ifstream   &the_stream)
{ // begin
  App_Config_Private::App_Config_Node     *the_new_node = NULL;
  
  std::string             the_current_section = App_Config_Private::Config_Root_Section;
  
  int                     the_line_loop = 0;

  bool                    is_eof = false;
  
  Method_State_Block_Begin(5)
    State(1)
      the_new_node = new (std::nothrow) App_Config_Private::App_Config_Node;
  
      if (the_new_node == NULL)
        the_method_error = A4_Error(A4_Config_File_Module_ID, RC_Root_Allocation_Error, "Memory allocation error - could not allocate the root node.");
      else { // initialize the root
        the_new_node->node_type = App_Config_Private::App_Config_Node::Node_Type::Section;
        the_new_node->node_lable = the_current_section;
        this->config_vector.push_back(the_new_node);
        the_new_node = NULL;
      } // if else
    End_State
    
    State(2) 
      Define_Target_State(the_line_loop);
    End_State
    
    State(3)
      the_method_error = App_Config_Private::App_Config_Node::Read_Line(the_stream, the_new_node, is_eof);
    End_State
      
    State(4)
      if (the_new_node != NULL)
        the_method_error = this->Insert_New_Node(the_new_node, the_current_section);
      else Terminate_The_Method_Block;
    End_State
      
    State(5)
      if (is_eof == false)
      { // try again
        Set_Target_State(the_line_loop);
      } // if then
    End_State
  End_Method_State_Block
    
  return the_method_error.Get_Error_Code();   
} // Read_Configuration


/**
* \brief  Retrieve the last modified time of the configuration file. It is only meaningful if the instance Is_Open.
*/
std::time_t App_Configuration::Get_Last_Modified_Time(void) const
{ // begin
  if (this->Is_Open() != true)
    (void) App_Log->Write("Call to App_Configuration::Get_Last_Modified_Time is invalid because the object is not open - data is useless.", A4_Lib::Logging::Error);

  return this->file_last_modified_time;
} // Get_Last_Modified_Time


/**
 * \brief Reloads the configuration file if modifications are detected
 */
Error_Code  App_Configuration::Reload_Config_Job (void)
{ // begin
  std::ifstream   the_stream;
  
  std::time_t     the_last_modified_time = 0;
  
  bool            the_job_mutex_is_locked = false;
  bool            the_data_mutex_is_acquired = false;

//  std::time_t test_time = A4_Lib::Now() ;

  Method_State_Block_Begin(9)
    State(1) 
      if (this->next_reload_time >= A4_Lib::Now())
        Terminate_The_Method_Block;
      else the_method_error = this->reload_config_job_mutex.Lock(the_job_mutex_is_locked, true);
    End_State
    
    State(2)    
      if (the_job_mutex_is_locked != true)
        Terminate_The_Method_Block; // another thread is busy with it
      else { // set next_reload_time to prevent repeat calls
        
        this->next_reload_time = A4_Lib::Now() + App_Configuration::Config_Reload_Check_Period;
        
        the_method_error = A4_Lib::Get_File_Last_Modified_Time(this->filespec, the_last_modified_time);
      } // if else
    End_State
      
    State(3)
      if (the_last_modified_time <= this->file_last_modified_time)
      { // nothing changed
        Terminate_The_Method_Block; // nothing changed     
        this->reload_config_job_mutex.Unlock(the_job_mutex_is_locked);
      } // if then
    End_State
      
    State(4)
      (void) App_Log->Write("Configuration File change detected. Cached items will be revalidated.", A4_Lib::Logging::Info);
      the_stream.open(this->filespec, std::ifstream::in);
        
      if (the_stream.is_open() != true)
        the_method_error = A4_Error (A4_Config_File_Module_ID, RCJ_File_Open_Failure, "Failed to open the configuration file.");
      else { // opened
        this->is_open = true;    
        this->next_reload_time = A4_Lib::Now() + App_Configuration::Config_Reload_Check_Period;
        
        the_method_error = this->data_mutex.Lock(the_data_mutex_is_acquired);
      } // if else        
    End_State     
      
    State(5) 
      the_method_error = this->Free_Config_Vector();
    End_State   
      
    State(6)
      the_method_error = this->Read_Configuration(the_stream);
    End_State
      
    State(7)
      this->file_last_modified_time = the_last_modified_time;

      the_method_error = this->data_mutex.Unlock(the_data_mutex_is_acquired);
    End_State
      
    State(8)
      the_method_error = this->Notify_Observers(0, A4_Lib::Observer_Hint::App_Config_Modified); // 
    End_State
      
    State(9)
      the_method_error = this->reload_config_job_mutex.Unlock(the_job_mutex_is_locked);
    End_State
  End_Method_State_Block
    
  if (the_job_mutex_is_locked == true)
    (void) this->reload_config_job_mutex.Unlock(the_job_mutex_is_locked);
    
  if (the_job_mutex_is_locked == true)
    (void) this->reload_config_job_mutex.Unlock(the_job_mutex_is_locked);
    
  if (the_stream.is_open() == true)
  { // cleanup
    the_stream.close();
  } // if then
          
  return the_method_error.Get_Error_Code();   
} // Reload_Config_Job


/**
 * \brief Open and load a configuration file. The file is closed when the load completes.
 * \param the_filespec - IN - std::string
 * \param enable_auto_reload - IN - \b Enable_Auto_Reload will check the config file for changes, reload it if the file is modified and notify any Observers of the change(s) 
 */
Error_Code  App_Configuration::Open (std::string   the_filespec,
                                     bool          enable_auto_reload)
{ // begin
  std::ifstream   the_stream;
    
  bool            the_mutex_is_locked = false;
  
  Method_State_Block_Begin(8)
    State(1)  
      if (this->Is_Open() == true)
        the_method_error = A4_Error (A4_Config_File_Module_ID, O_Already_Open, "Instance is already open.");
      else (void) App_Log->Write (A4_Lib::Logging::Info, "Opening Configuration File %s", the_filespec.c_str());
    End_State
    
    State(2)
      if (the_filespec.length() < 1)
        the_method_error = A4_Error (A4_Config_File_Module_ID, O_Empty_Fiename, "Invalid parameter value - the_filespec is empty");
    End_State
      
    State(3)
      if (A4_Lib::File_Exists (the_filespec) != true)
        the_method_error = A4_Error (A4_Config_File_Module_ID, O_No_File, A4_Lib::Logging::Error, "Missing file- %s does not exist.", the_filespec.c_str());
      else the_method_error = A4_Lib::Get_File_Last_Modified_Time (the_filespec, this->file_last_modified_time);
    End_State
      
    State(4)
      if (this->Is_Initialized() != true)
        the_method_error = this->Initialize(App_Config_Private::Num_Threads); // 
    End_State
    
    State(5)
      this->auto_reload_enabled = enable_auto_reload;
      the_stream.open(the_filespec, std::ifstream::in);
        
      if (the_stream.is_open() != true)
        the_method_error = A4_Error (A4_Config_File_Module_ID, O_File_Open_Failure, A4_Lib::Logging::Error, "Failed to open the configuration file %s.", the_filespec.c_str());
      else { // opened
	(void) App_Log->Write (A4_Lib::Logging::Info, "Opened configuration file %s", the_filespec.c_str());
        this->is_open = true;    
        this->next_reload_time = A4_Lib::Now() + App_Configuration::Config_Reload_Check_Period;
        this->filespec = the_filespec;
        
        the_method_error = this->data_mutex.Lock(the_mutex_is_locked);
      } // if else        
    End_State
    
    State(6)
      if ((this->Is_Started() == false) && (this->auto_reload_enabled == App_Configuration::Enable_Auto_Reload))
        the_method_error = this->Start();
    End_State
        
    State(7)        
      the_method_error = this->Read_Configuration (the_stream);
      
      if (the_method_error != No_Error)
        this->is_open = false;
    End_State
      
    State(8)
      the_method_error = this->data_mutex.Unlock(the_mutex_is_locked);
    End_State
  End_Method_State_Block

  if (the_mutex_is_locked == true)
    (void) this->data_mutex.Unlock(the_mutex_is_locked);
    
  if (the_stream.is_open() == true)
  { // cleanup
    the_stream.close();
  } // if then
      
  return the_method_error.Get_Error_Code();   
} // Open
 

/**
 * \brief Open and load a configuration file. The file is closed when the load completes.
 * \param the_filespec - IN - std::wstring
 * \param enable_auto_reload - IN - \b Enable_Auto_Reload will check the config file for changes, reload it if the file is modified and notify any Observers of the change(s) 
 */
Error_Code  App_Configuration::Open (std::wstring    the_filespec,
                                     bool            enable_auto_reload)
{ // begin
  std::string   the_a_string;

  Method_State_Block_Begin(2)
    State(1)  
      the_method_error = A4_Lib::WString_To_AString (the_filespec, the_a_string);
    End_State

    State(2)
      the_method_error = this->Open(the_a_string, enable_auto_reload);
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} // Open (wstring)

/**
 * \brief Retrieve the current status.
 * @return 
 */
bool  App_Configuration::Is_Open(void) const
{ // begin
  return this->is_open;
} // Is_Open


/**
 * \brief Close the configuration.
 */

Error_Code  App_Configuration::Close(void)
{ // begin
  bool	the_mutex_is_locked = false;

  Method_State_Block_Begin(5)
    State(1)
      the_method_error = this->data_mutex.Lock(the_mutex_is_locked);
    End_State

    State(2)	
      if (this->Is_Open() != true)
	the_method_error = A4_Error (A4_Config_File_Module_ID, C_Not_Open, "This instance is not open.");
    End_State

    State(3)
      this->is_open = false;

      if (this->Active_Object::Is_Started() == true)
        the_method_error = this->Active_Object::Stop(); 
    End_State

    State(4)
      this->auto_reload_enabled = false;
      this->next_reload_time = 0;
      this->filespec.clear();

      the_method_error = this->Free_Config_Vector();
    End_State

    State(5)
      the_method_error = this->data_mutex.Unlock(the_mutex_is_locked);
    End_State
  End_Method_State_Block

  if (the_mutex_is_locked == true)
    (void) this->data_mutex.Unlock(the_mutex_is_locked);

  return the_method_error.Get_Error_Code();   
} // Close

  
/**
 * \brief Retrieve a string Value associated with a Section and Key
 * @param the_section_name - IN
 * @param the_key_name - IN
 * @param the_string - OUT
 * @param is_found - OUT
 */
Error_Code  App_Configuration::Get_String (std::string   the_section_name, 
                                           std::string   the_key_name,    
                                           std::string   &the_string, 
                                           bool          &is_found)
{ // begin
  std::size_t    the_offset = 0;
  
  bool      the_section_is_found = false;
  
  Method_State_Block_Begin(5)
    State(1)  
      is_found = false;
      the_string.clear();
      
      if (this->Is_Open() != true)
        the_method_error = A4_Error (A4_Config_File_Module_ID, GS_Not_Open, "Instance is not open. Not possible to read anything.");
    End_State
    
    State(2)
      if (the_section_name.length() < 1)
        the_method_error = A4_Error (A4_Config_File_Module_ID, GS_No_Section, "Invalid parameter value - the_section_name is empty");
    End_State
      
    State(3)
      if (the_key_name.length() < 1)
        the_method_error = A4_Error (A4_Config_File_Module_ID, GS_No_Key, "Invalid parameter value - the_key_name is empty");
      else the_method_error = this->Find_Section(the_section_name, the_offset, the_section_is_found);
    End_State
      
    State_NoTry(4)
      if (the_section_is_found != true)
        Terminate_The_Method_Block;
      else the_offset += 1; // move past the section label
    End_State_NoTry
      
    State(5) // search the current Section for the Key
      while ((is_found == false) && (the_offset < this->config_vector.size()) && (this->config_vector[the_offset]->node_type != App_Config_Private::App_Config_Node::Node_Type::Section))
        if (this->config_vector[the_offset]->node_lable == the_key_name)
        { // found it
          is_found = true;
          the_string = this->config_vector[the_offset]->node_value;
        } // if then
        else the_offset += 1;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} //Get_String

/**
 * \brief Retrieve a wstring Value associated with a Section and Key
 * @param the_section_name - IN
 * @param the_key_name - IN
 * @param the_string - OUT
 * @param is_found - OUT
 */
Error_Code  App_Configuration::Get_String (std::wstring   the_section_name, // in
                                           std::wstring   the_key_name,     // in
                                           std::wstring   &the_string,      // out
                                           bool           &is_found)
{ // begin
  std::string   the_a_section_name;
  std::string   the_a_key_name;
  std::string   the_a_string;

  bool          the_a_string_is_found = false;

  Method_State_Block_Begin(5)
    State(1)  
      is_found = false;

      the_method_error = A4_Lib::WString_To_AString (the_section_name, the_a_section_name);
    End_State

    State(2)
      the_method_error = A4_Lib::WString_To_AString (the_key_name, the_a_key_name);
    End_State

    State(3)
      the_method_error = this->Get_String(the_a_section_name, the_a_key_name, the_a_string, the_a_string_is_found);
    End_State

    State(4)
      if (the_a_string_is_found != true)
        Terminate_The_Method_Block;
      else the_method_error = A4_Lib::AString_To_WString (the_a_string, the_string);
    End_State

    State(5)
      is_found = true;
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} // Get_String (wstring)

/**
 * \brief   Retrieve a Pair from the configuration file and interpret the value as a uint32_t
 * @param the_section_name - IN
 * @param the_key - IN
 * @param the_value - OUT - will be
 * @param the_default_value - IN - 
 * @return 
 */
Error_Code  App_Configuration::Get_UINT32 (std::string      the_section_name, 
                                           std::string      the_key, 
                                           std::uint32_t    &the_value, 
                                           std::uint32_t    the_default_value)
{ // begin
  std::string   the_string_value;
  
  char          *the_pointer_result = NULL;

  bool          is_found = false;
  
  
  Method_State_Block_Begin(3)
    State(1)   
      the_method_error = this->Get_String (the_section_name, the_key, the_string_value, is_found);
    End_State
    
    State_NoTry(2)
      if (is_found != true)
      { // not found - use the default value
        the_value = the_default_value;
        Terminate_The_Method_Block;
      } // if then
    End_State_NoTry
      
    State_NoTry(3) // try & validate the retrieved value - it may or may not be a number      
      the_value = static_cast <std::uint32_t>(strtol (the_string_value.c_str(), &the_pointer_result, 10));

      if ((the_pointer_result == NULL) || (*the_pointer_result != 0))
      { // not a number - use the default value parameter
        the_value = the_default_value;
      } // if then
    End_State_NoTry
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} // Get_UINT32

 /**
  * \brief   Retrieve a Pair from the configuration file and interpret the value as a std::size_t
  * @param the_section_name - IN
  * @param the_key - IN
  * @param the_value - OUT 
  * @param the_default_value - IN -
 */
Error_Code  App_Configuration::Get_Size_T(std::string     the_section_name,
					  std::string     the_key,
					  std::size_t	  &the_value,
					  std::size_t    the_default_value)
{ // begin
  std::string   the_string_value;

  char          *the_pointer_result = NULL;

  bool          is_found = false;


  Method_State_Block_Begin(3)
    State(1)
      the_method_error = this->Get_String(the_section_name, the_key, the_string_value, is_found);
    End_State

    State_NoTry(2)
      if (is_found != true)
      { // not found - use the default value
	the_value = the_default_value;
	Terminate_The_Method_Block;
      } // if then
    End_State_NoTry

    State_NoTry(3) // try & validate the retrieved value - it may or may not be a number      
      the_value = static_cast <std::size_t>(strtoll(the_string_value.c_str(), &the_pointer_result, 10));

      if ((the_pointer_result == NULL) || (*the_pointer_result != 0))
      { // not a number - use the default value parameter
	the_value = the_default_value;
      } // if then
    End_State_NoTry
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Get_Size_T


 /**
  * \brief   Retrieve a Pair from the configuration file and interpret the value as a std::size_t
  * @param the_section_name - IN
  * @param the_key - IN
  * @param the_value - OUT -
  * @param the_default_value - IN -
 */
Error_Code  App_Configuration::Get_Double (std::string  the_section_name,
			                   std::string  the_key,
			                   double       &the_value,
			                   double       the_default_value)
{ // begin
  std::string   the_string_value;

  char          *the_pointer_result = NULL;

  bool          is_found = false;


  Method_State_Block_Begin(3)
    State(1)
      the_method_error = this->Get_String(the_section_name, the_key, the_string_value, is_found);
    End_State

    State_NoTry(2)
      if (is_found != true)
      { // not found - use the default value
	the_value = the_default_value;
	Terminate_The_Method_Block;
      } // if then
    End_State_NoTry

    State_NoTry(3) // try & validate the retrieved value - it may or may not be a number      
      the_value = strtod(the_string_value.c_str(), &the_pointer_result);

      if ((the_pointer_result == NULL) || (*the_pointer_result != 0))
      { // not a number - use the default value parameter
	the_value = the_default_value;
      } // if then
    End_State_NoTry
  End_Method_State_Block

  return the_method_error.Get_Error_Code();
} // Get_Double



/**
 * \brief   Called when the thread times out waiting for a message. 
 * @return  No_Error upon success
 */
Error_Code  App_Configuration::Handle_Timeout(void)
{ // begin
  Method_State_Block_Begin(2)
    State(1) 
      if (this->auto_reload_enabled == App_Configuration::Enable_Auto_Reload)
        the_method_error = this->Reload_Config_Job();
    End_State
    
    State(2)
      the_method_error = this->Active_Object::Handle_Timeout();
    End_State
  End_Method_State_Block

  return the_method_error.Get_Error_Code();   
} // Handle_Timeout


/**
* \brief  Delete the contents of the config_vector member
*/
Error_Code  App_Configuration::Free_Config_Vector(void)
{ // begin
  std::size_t    the_offset = 0;
  
  Method_State_Block_Begin(1)
    State(1) 
      for (the_offset = 0; the_offset < this->config_vector.size(); the_offset++)
      {// begin
        delete this->config_vector[the_offset];
        this->config_vector[the_offset] = NULL;
      } // for 
  
      this->config_vector.clear();
      this->config_vector.shrink_to_fit();
    End_State
  End_Method_State_Block 
      
  return the_method_error.Get_Error_Code(); 
} // Free_Config_Vector

/**
* \brief  Retrieve all Section Names from the cached configuration vector
* \param  the_list - OUT
*/
Error_Code  App_Configuration::Get_Section_Deque(App_Configuration::String_Deque   &the_list)
{ // begin
  Method_State_Block_Begin(1)
    State(1) 
      the_list.clear();

      if (this->Is_Open() != true)
        the_method_error = A4_Error (A4_Config_File_Module_ID, GSD_Not_Open, "Invalid state - the instance needs to be Open.");
      else for (std::size_t  the_offset = 0; the_offset < this->config_vector.size(); the_offset++)
              if (this->config_vector[the_offset]->node_type == App_Config_Private::App_Config_Node::Section)
                the_list.push_back(this->config_vector[the_offset]->node_lable);
    End_State
  End_Method_State_Block 
      
  return the_method_error.Get_Error_Code(); 
} // Get_Section_Deque


/**
* \brief  Retrieve all Name/Value pairs for a given Section of the configuration file
* \param  the_section_name - IN - case sensitive - the section must exist
* \param  the_value_name_list - OUT
* \param  the_value_list - OUT
* \note   An alternative to explore would be exporting a Deque of std::pair 
*/
Error_Code  App_Configuration::Get_Values_By_Section (std::string                      the_section_name, 
                                                      App_Configuration::String_Deque  &the_value_name_list, // out
                                                      App_Configuration::String_Deque  &the_value_list)
{ // begin
  std::size_t   the_offset = 0;

  bool          the_section_is_found = false;

  Method_State_Block_Begin(4)
    State(1) 
      the_value_name_list.clear();
      the_value_list.clear();

      if (the_section_name.length() < 1)
        the_method_error = A4_Error (A4_Config_File_Module_ID,  GVBS_No_Section_Name, "Invalid parameter length - the_section_name is empty.");
    End_State

    State(2)
      if (this->Is_Open() != true)
        the_method_error = A4_Error (A4_Config_File_Module_ID,  GVBS_Not_Open, "Invalid state - the instance needs to be Open.");
    End_State

    State(3)
      while ((the_offset < this->config_vector.size()) && (the_section_is_found != true))
        if ((the_section_name == this->config_vector[the_offset]->node_lable) && (this->config_vector[the_offset]->node_type == App_Config_Private::App_Config_Node::Section))
          the_section_is_found = true;
        else the_offset += 1;

      if (the_section_is_found != true)
        the_method_error = A4_Error (A4_Config_File_Module_ID,  GVBS_Section_Not_Found, A4_Lib::Logging::Error,
                                      "Section %s was not found in the configuration file", the_section_name.c_str());
      else the_offset += 1; // move past the Section node
    End_State

    State(4)
      while ((the_offset < this->config_vector.size()) && (this->config_vector[the_offset]->node_type != App_Config_Private::App_Config_Node::Section))
      { // step through
        if (this->config_vector[the_offset]->node_type == App_Config_Private::App_Config_Node::Pair)
        { // 
          the_value_name_list.push_back(this->config_vector[the_offset]->node_lable);
          the_value_list.push_back(this->config_vector[the_offset]->node_value);
        } // if then

        the_offset += 1;
      } // while
    End_State
  End_Method_State_Block 
      
  return the_method_error.Get_Error_Code(); 
} // Get_Values_By_Section