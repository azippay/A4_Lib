#ifndef __A4_Template_Macros_Defined__
#define __A4_Template_Macros_Defined__
/**
* \brief  Template helpers - not all data classes have all required methods, members, constants, etc. These macros substitute the call, log errors if necessary
*
* \author a. zippay * 2016..2018
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


#include "A4_Lib_Module_ID.hh"
#include "A4_Log.hh"


#ifndef _Macro_Param_to_String
  #define _Macro_Param_to_String(s) _macro_str(s)
#endif

#ifndef _macro_str
  #define _macro_str(s) #s
#endif


/**
* \brief  Supports the use case where a method return type \b bool may not exist - if it does not exist, then the Substitute will return false. There are no parameters.
*
* \note A4_Unused_Arg(the_class_instance); is placed before the method call because MSVC (incorrectly) compiles with warning C4100: 'the_class_instance' : unreferenced formal parameter.
*       This is obviously not true and the proper class implemented method instance is called using the_class_instance. 
*
* \todo Compile this using gcc to see what happens.
*/
#define bool_Return_SFINAE(THE_CLASS_NAME, THE_METHOD_NAME) \
template <typename THE_CLASS_NAME> struct Method_##THE_METHOD_NAME \
{ \
public: \
  template <typename THE_CLASS_NAME> \
    static auto bRS (THE_CLASS_NAME  &the_class_instance)->decltype (std::declval<THE_CLASS_NAME>().THE_METHOD_NAME()){A4_Unused_Arg(the_class_instance);return the_class_instance.THE_METHOD_NAME();};\
\
  template <typename>\
    static bool bRS(...){return false;}; \
\
  static bool THE_METHOD_NAME (THE_CLASS_NAME &the_class_instance){return bRS<THE_CLASS_NAME>(the_class_instance);}; \
} // END bool_Return_SFINAE


/**
* \brief Support substitution for one-parameter temporal methods
*
* \param  THE_CLASS_NAME - the class name - treated as a Reference 
* \param  THE_METHOD_NAME - The Name of the method needing substituted.
* \param  PARAM1_TYPE - The parameter type declaration. 
*
* \note  Help compile time Substitution - If THE_METHOD_NAME exists in THE_CLASS_NAME, then it is used, otherwise it is substituted for the variadic version which returns an error if called.
*/


#define One_Parameter_SFINAE(THE_CLASS_NAME, THE_METHOD_NAME, PARAM1_TYPE) \
template <typename THE_CLASS_NAME> struct Method_##THE_METHOD_NAME                \
{ \
public: \
  template <typename THE_CLASS_NAME, typename P1> \
    static auto OPS (THE_CLASS_NAME  &the_class_instance, P1 param1)->decltype (std::declval<THE_CLASS_NAME>().THE_METHOD_NAME(param1)){return the_class_instance.THE_METHOD_NAME(param1);};\
\
  template <typename, typename>\
    static Error_Code OPS(...){A4_Char the_msg[Default_Log_Message_Length];\
    (void) A4_Lib::SNPrintf (the_msg, sizeof (the_msg), A4_TEXT("Incorrect call to %s.%s from method %s.\n"), _Macro_Param_to_String(THE_CLASS_NAME), _Macro_Param_to_String(THE_METHOD_NAME),ASL_This_Function_Name);\
    (void) AZLog->Write(the_msg, A4_Log::Error);\
    return Template_Macros_Module_ID + 0;}; \
\
  static Error_Code THE_METHOD_NAME (THE_CLASS_NAME &the_class_instance, PARAM1_TYPE  param1){return OPS<THE_CLASS_NAME, PARAM1_TYPE>(the_class_instance, param1);}; \
} // END One_Parameter_SFINAE


/**
* \brief Support substitution for four-parameter temporal methods
*
* \param  THE_CLASS_NAME - the class name - treated as a Reference 
* \param  THE_METHOD_NAME - The Name of the method needing substituted.
* \param  PARAM1_TYPE...PARAM4_TYPE - The parameter type declaration. 
*
* \note  Help compile time Substitution - Temporal classes have Timestamp methods missing from non-temporal classes. Using SFINAE, the template compiles non-temporal classes.
*/
#define Four_Parameter_SFINAE(THE_CLASS_NAME, THE_METHOD_NAME, PARAM1_TYPE, PARAM2_TYPE, PARAM3_TYPE, PARAM4_TYPE) \
template <typename THE_CLASS_NAME> struct Method_##THE_METHOD_NAME \
{ \
public: \
  template <typename THE_CLASS_NAME, typename P1, typename P2, typename P3, typename P4> \
    static auto F4PS (THE_CLASS_NAME  &the_class_instance, P1 param1, P2 param2, P3 param3, P4 param4)->decltype (std::declval<THE_CLASS_NAME>().THE_METHOD_NAME(param1,param2,param3,param4)){return the_class_instance.THE_METHOD_NAME(param1,param2,param3,param4);}; \
\
  template <typename, typename, typename,typename,typename> \
    static Error_Code F4PS(...){A4_Char the_msg[Default_Log_Message_Length];\
    (void) A4_Lib::SNPrintf (the_msg, sizeof (the_msg), A4_TEXT("Incorrect call to %s.%s from method %s.\n"), _Macro_Param_to_String(THE_CLASS_NAME), _Macro_Param_to_String(THE_METHOD_NAME),ASL_This_Function_Name);\
    (void) AZLog->Write(the_msg, A4_Log::Error);\
    return Template_Macros_Module_ID + 4;}; \
\
  static Error_Code THE_METHOD_NAME (THE_CLASS_NAME &the_class_instance, PARAM1_TYPE  param1, PARAM2_TYPE  param2, PARAM3_TYPE  param3, PARAM4_TYPE  param4){return F4PS<THE_CLASS_NAME, PARAM1_TYPE, PARAM2_TYPE, PARAM3_TYPE, PARAM4_TYPE>(the_class_instance,param1,param2,param3,param4);}; \
} // END Four_Parameter_SFINAE


/**
* \brief Support substitution for five-parameter temporal methods
*
* \param  THE_CLASS_NAME - the class name - treated as a Reference 
* \param  THE_METHOD_NAME - The Name of the method needing substituted.
* \param  PARAM1_TYPE...PARAM5_TYPE - The parameter type declaration. 
*
* \note  Help compile time Substitution - Temporal classes have Timestamp methods missing from non-temporal classes. Using SFINAE, the template compiles non-temporal classes.
*/
#define Five_Parameter_SFINAE(THE_CLASS_NAME, THE_METHOD_NAME, PARAM_TYPE_1, PARAM_TYPE_2, PARAM_TYPE_3, PARAM_TYPE_4, PARAM_TYPE_5) \
template <typename THE_CLASS_NAME> struct Method_##THE_METHOD_NAME \
{ \
public: \
  template <typename THE_CLASS_NAME, typename P1, typename P2, typename P3, typename P4, typename P5> \
    static auto F5PS (THE_CLASS_NAME  &c, P1 param1, P2 param2, P3 param3, P4 param4, P5 param5)->decltype (std::declval<THE_CLASS_NAME>().THE_METHOD_NAME(param1, param2, param3, param4, param5)){return c.THE_METHOD_NAME(param1, param2, param3, param4, param5);};\
\
  template <typename, typename,typename,typename,typename,typename>\
    static Error_Code F5PS(...){A4_Char the_msg[Default_Log_Message_Length];\
    (void) A4_Lib::SNPrintf (the_msg, sizeof (the_msg), A4_TEXT("Incorrect call to %s.%s from method %s.\n"), _Macro_Param_to_String(THE_CLASS_NAME), _Macro_Param_to_String(THE_METHOD_NAME),ASL_This_Function_Name);\
    (void) AZLog->Write(the_msg, A4_Log::Error);\
    return Template_Macros_Module_ID + 5;}; \
\
  static Error_Code THE_METHOD_NAME (THE_CLASS_NAME &the_class_instance, PARAM_TYPE_1  param1, PARAM_TYPE_2 param2, PARAM_TYPE_3 param3, PARAM_TYPE_4 param4, PARAM_TYPE_5 param5) {return F5PS<THE_CLASS_NAME, PARAM_TYPE_1, PARAM_TYPE_2, PARAM_TYPE_3, PARAM_TYPE_4, PARAM_TYPE_5>(the_class_instance, param1, param2, param3, param4, param5);}; \
} // end Five_Parameter_SFINAE

#endif // __A4_Template_Macros_Defined__
