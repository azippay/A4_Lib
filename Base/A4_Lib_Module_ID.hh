#ifndef __A4_Lib_Module_Defined__
#define __A4_Lib_Module_Defined__

/// @file A4_Lib_Module_ID.hh
/// @brief  Module_ID's defined for the AZ Library - each distinct module in the library is assigned a unique ID.
/// @author a. zippay * 2013..2018


#include "A4_Lib_Base.hh"

/// @defgroup A4_Lib_Module_ID_Group AZ Lib Module ID's
///@{


const Module_ID  A4_Lib_Max_Module_ID = 499; /**< Must be changed should the number of modules exceed this value */

// modules work in Linux & Windows, unless otherwise indicated
const Module_ID A4_Method_State_Block_Module_ID       = 0;
const Module_ID A4_Utils_Module_ID                    = 1;
const Module_ID A4_Active_Object_Module_ID            = 2;
const Module_ID A4_Config_File_Module_ID              = 3;
const Module_ID A4_Log_Module_ID                      = 4; 
const Module_ID A4_Observer_Module_ID                 = 5;
const Module_ID A4_Observable_Module_ID               = 6;
const Module_ID A4_Network_Service_Handler_Module_ID  = 7; 
const Module_ID A4_Message_Block_Module_ID            = 8;
const Module_ID A4_Message_Queue_Module_ID            = 9;
const Module_ID A4_Timed_Mutex_Module_ID              = 10;
const Module_ID A4_Mutex_Module_ID                    = 11;
const Module_ID A4_Recursive_Mutex_Module_ID          = 12;
const Module_ID A4_Win_Helper_Module_ID               = 13; /**< MS WIndows Only */
const Module_ID A4_WMI_Module_ID                      = 14; /**< MS WIndows Only */
const Module_ID A4_WMI_Singleton_Module_ID            = 15; /**< MS WIndows Only */
const Module_ID A4Qt_Combo_Module_ID                  = 16;
const Module_ID A4_Socket_Address_Module_ID           = 17;
const Module_ID A4_Socket_Module_ID                   = 18;
const Module_ID A4_Shared_Timed_Mutex_Module_ID       = 19;
const Module_ID A4_Buffer_Page_Module_ID              = 20;
const Module_ID A4_Active_Buffer_Module_ID            = 21;
const Module_ID A4_Protocol_Handler_Module_ID         = 22;
const Module_ID A4_Win_Sec_Module_ID                  = 23; /**< MS Windows Only */
const Module_ID A4_Win_Registry_Module_ID             = 24; /**< MS Windows Only */
const Module_ID A4_Credential_Cache_Module_ID         = 25; /**< MS Windows Only */
const Module_ID A4_File_Util_Module_ID                = 26;
const Module_ID A4_File_Logger_Module_ID              = 27;
const Module_ID A4_Logger_Base_Module_ID              = 28;
const Module_ID A4_Connector_Module_ID                = 29;
const Module_ID A4_Acceptor_Module_ID                 = 30;
const Module_ID A4_Pipe_Instance_Entry_Module_ID      = 31;
const Module_ID A4_Property_Value_Map_Module_ID       = 32;
const Module_ID A4_Connection_Module_ID               = 33;
const Module_ID A4_ODBC_Environment_Module_ID         = 34;
const Module_ID A4_Credential_Module_ID               = 35;
const Module_ID A4_ODBC_User_Credential_Module_ID     = 36;
const Module_ID A4_ODBC_Connection_Module_ID          = 37;
const Module_ID A4_ODBC_Statement_Module_ID           = 38;
const Module_ID A4_ODBC_Column_Info_Base_Module_ID    = 39;
const Module_ID A4_ODBC_Column_Info_Module_ID         = 40;
const Module_ID A4_ODBC_Table_Module_ID               = 41;
const Module_ID A4_ODBC_Base_Module_ID                = 42;
const Module_ID A4_RDBMS_Table_Row_Module_ID          = 43;
const Module_ID A4_RDBMS_Trigger_Module_ID            = 44;
const Module_ID A4_RDBMS_Table_Module_ID              = 45;
const Module_ID A4_Network_Data_Connection_Module_ID  = 46;
const Module_ID A4_RDBMS_Common_Base_Module_ID        = 47;
const Module_ID A4_Trading_Exchange_Info_Module_ID    = 48;
/** @}*/ // A4_Lib_Module_ID_Group
#endif // __A4_Lib_Module_Defined__
