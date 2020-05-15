 #ifndef __A4_Method_State_Block_Defined__
#define __A4_Method_State_Block_Defined__

/**
* \author Albert Zippay * 1995..2020
* \copyright Albert Zippay - 1995 - 2020
* \file A4_Method_State_Block.hh
*
* \brief  Implements the Process Block Idiom
*
* \note  
*   The Method Block Idiom is used throughout the A4_Lib 
* Copyright (c) 1995...2020 Albert Zippay
* 
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

/**
 * \note  The Log & Error classes are necessary for the idiom to function. This creates a cyclical dependency with them and this header.
*/
#include "A4_Lib_Module_ID.hh"
#include "A4_Error.hh"
#include "A4_Logger.hh"

#include <memory.h>

/**
 * \brief process block error conditions
 */
enum A4_Process_Block_Errors
{ // begin
  STS_Undefined_Target_State1 = 0, /**< Invalid Target State value which must be between 0..A4_Max_Target_States-1 */
  DTS_Invalid_Target_State    = 1, /**< Invalid Target State - The max state number must be less than A4_Max_Target_States */
  STS_Undefined_Target_State2 = 2, /**< Invalid Target State - Set_Target_State must point to a defined target state */
  EMSB_Undefined_State        = 3, /**< Undefined method state was encountered - probably skipped a \b State. */
  ES_Exception_Caught	      = 4, /**< An exception was thrown and caught at a specific \b State. */
}; // end A4_Process_Block_Errors


static const int A4_Max_Target_States = 5; // an arbitrary number - perhaps this should be a macro parameter? perhaps array should be replaced by a vector?
static const int A4_Max_Method_States = 0x7530; // 30000 states is probably enough for humans...but machine generated code?  

/**
 * \brief The head of the Method State Block - must be closed with End_Method_State_Block
 */
#define Method_State_Block_Begin(max_states)\
  Method_State		the_method_state = 0;\
  A4_Error  		the_method_error(No_Error);\
  Method_State		the_target_state[A4_Max_Target_States];\
  memset(the_target_state, 0, sizeof(the_target_state));\
while((the_method_state < max_states) && (the_method_error == No_Error)){ the_method_state += 1;\
  switch(the_method_state){ 
// Method_State_Block_Begin

/**
 * \brief The Method State Block termination - will log any errors 
 */
#define End_Method_State_Block  default:\
if (the_method_state < A4_Max_Method_States){\
  the_method_error = A4_Error(A4_Method_State_Block_Module_ID, EMSB_Undefined_State, A4_Lib::Logging::Error, "Undefined method state was encountered in %s at line=%d in the_method_state=%d",\
This_Function_Name, __LINE__, the_method_state);\
};}}\
/* log errors */\
if (the_method_error != No_Error){\
  if (App_Log->Is_Open() == true) (void) App_Log->Write(the_method_error.Get_Error_Message(), This_Function_Name, the_method_error.Get_Error_Code(), the_method_state, A4_Lib::Logging::Error);};
// End_Method_State_Block 

/**
 * \brief Start of a single state
 * \param x - IN - the state number - must be unique within the Method State Block and must be closed with End_State
 * 
 * \note  Since C++ implements a zero-work try, there's no performance hit for trying...only for catching
 */
#define State(x)case x: try { 

#define State_NoTry(x)case x: /**< When there's no need to try or (more likely) when a compiler complains about Unreachable Code */
#define End_State_NoTry break; /**< must be used together with State_NoTry */

/**
 * \brief  The State(x) termination - will log any exceptions caught.
 */
#define End_State }\
catch (...) {\
  the_method_error = A4_Error(A4_Method_State_Block_Module_ID, ES_Exception_Caught, A4_Lib::Logging::Error, "Exception thrown in %s at line=%d in the_method_state=%d",\
This_Function_Name, __LINE__, the_method_state);\
}\
break; // End_State 

/**
 * \brief Allows control to exit the Method State Block
 */
#define Terminate_The_Method_Block	(the_method_state += A4_Max_Method_States) // set to base-10 number 30000 - this allows the current method state to be preserved


/**
 * \brief Define a State where control may be directed to - this can be seen as the start of a Loop - the advantage being that control remains in the Method State Block
 * \param x - IN - defines the offset within the_target_state array which holds the_method_state 
 */					
#define Define_Target_State(x)if((x < A4_Max_Target_States)&&(x >= 0))\
				the_target_state[x]= the_method_state;\
			      else the_method_error = A4_Error(A4_Method_State_Block_Module_ID, DTS_Invalid_Target_State, "Invalid Target State - The max state number must be less than A4_Max_Target_States")

/**
 * \brief Sets the_method_state to the_target_state[x]
 * \param x - IN - defines the offset within the_target_state array which holds the_method_state 
 */
#define Set_Target_State(x)if((x < A4_Max_Target_States)&&(x >= 0)){\
			     if(the_target_state[x]==0) the_method_error = A4_Error(A4_Method_State_Block_Module_ID, STS_Undefined_Target_State1, "Invalid Target State value which must be between 0..A4_Max_Target_States-1");\
			     else the_method_state = the_target_state[x];}\
			   else the_method_error = A4_Error(A4_Method_State_Block_Module_ID, STS_Undefined_Target_State2, "Invalid Target State - Set_Target_State must point to a defined target state")

#define A4_Cleanup_Begin  try{

#define A4_End_Cleanup }\
catch(...){\
  (void) App_Log->Write (A4_Lib::Logging::Error, "Exception thrown in %s at line=%d in method cleanup.",\
This_Function_Name, __LINE__);\
}
#endif // __A4_Method_State_Block_Defined__
