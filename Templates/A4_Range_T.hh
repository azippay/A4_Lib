#ifndef __Range_Template_Defined_
#define __Range_Template_Defined_

#include "A4_Method_State_Block.hh"
#include "A4_Log.hh"

#include <ace/Basic_Types.h>

/*
* \template A4_Range
* \brief    The purpose of this analogous to the BETWEEN statement in SQL.  The Equality operator == returns True if a test value lies between the low & high value.
*
* \author   a. zippay * 2009, 2013, 2014
*/
//////////////////////  template definition  ///////////////////////////

template <class THE_TYPE> class A4_Range
{  // begin
public: // construction
  A4_Range(void);
  ~A4_Range(void);

public: // methods
  ERROR_CODE	Get_Range (THE_TYPE   &the_range_low_value,
			   THE_TYPE   &the_range_high_value);

  ERROR_CODE	Set_Range (THE_TYPE   the_range_low_value,
			   THE_TYPE   the_range_high_value);

  void		Set_Range_Precedence(void);

  void		Set_Range_Null(void);

  bool		Range_Is_NULL(void) const;

  ERROR_CODE	Merge_Range (A4_Range<THE_TYPE>	  &the_range1,
			     A4_Range<THE_TYPE>	  &the_range2);

  ERROR_CODE	Set_Range_Low_Value (THE_TYPE   the_range_low_value);

  ERROR_CODE	Set_Range_High_Value (THE_TYPE   the_range_high_value);

  bool	  Moment_in_Range (THE_TYPE	the_moment_in_time);

public: // operators
  bool operator == (THE_TYPE  the_value);
  bool operator != (THE_TYPE  the_value);
  bool operator < (THE_TYPE  the_value);
  bool operator > (THE_TYPE  the_value);

  bool operator == (A4_Range  &the_value);
  bool operator != (A4_Range  &the_value);
  bool operator < (A4_Range  &the_value);
  bool operator > (A4_Range  &the_value);

  A4_Range<THE_TYPE> & operator = (A4_Range  &the_value);
  A4_Range<THE_TYPE> & operator = (THE_TYPE  the_value);

protected: // data
  THE_TYPE    range_low_value;
  THE_TYPE    range_high_value;

  bool	      range_is_null;
  bool	      range_has_precedence;

public: // errors
  enum Range_Errors
  { // begin
    TRSR_LOW_VALUE_TOO_HIGH	    = A4_Range_Error_Base + 0,
    TRSLV_LOW_VALUE_TOO_HIGH	    = A4_Range_Error_Base + 1,
    TRSHV_HIGH_VALUE_TOO_LOW	    = A4_Range_Error_Base + 2,
    TRM_RANGE1_NOT_SET		    = A4_Range_Error_Base + 3,
    TRM_RANGE2_NOT_SET		    = A4_Range_Error_Base + 4,
    GR_Range_Is_NULL		    = A4_Range_Error_Base + 5,
  } Range_Errors;
}; // end A4_Range template






/////////////////////////////////////////////////////////
// template implementation
/////////////////////////////////////////////////////////

/**
* \brief default constructor
*/
template <class THE_TYPE>
A4_Range<THE_TYPE>::A4_Range(void)
{ //begin
  this->Set_Range_Null();
}  // end constructor


/**
* \brief  Default destructor
*/
template <class THE_TYPE>
A4_Range<THE_TYPE>::~A4_Range(void)
{  // begin
  this->Set_Range_Null();
} // end destructor




/**
* \brief  Reset the contents to NULL
*/

template <class THE_TYPE>
void	A4_Range<THE_TYPE>::Set_Range_Null(void)
{ // begin
  this->range_low_value = 0;
  this->range_high_value = 0;
  this->range_is_null = true;
  this->range_has_precedence = false;
} // END Set_Range_Null



/**
* \brief  Test the range - return \b true if it is NULL (not set)
*/

template <class THE_TYPE>
bool	A4_Range<THE_TYPE>::Range_Is_NULL(void) const
{ // begin
  return this->range_is_null;
} // end Range_Is_NULL





/**
* \brief  Reuturn the upper & lower range bounds. If the range is NULL, an error is returned
*/

template <class THE_TYPE>
ERROR_CODE	A4_Range<THE_TYPE>::Get_Range (THE_TYPE   &the_range_low_value,
					    THE_TYPE   &the_range_high_value)
{ // begin
  const ACE_TCHAR   *the_error_msg = ASL_Default_Message;

  METHOD_STATE_BLOCK_BEGIN(1)
    STATE 1 :
      if (this->Range_Is_NULL() == true)
      { // begin
	the_method_error = GR_Range_Is_NULL;
	the_error_msg = ACE_TEXT ("Invalid state - cannot get a range that is NULL");
      } // end if then
      else { // begin
	the_range_low_value = this->range_low_value;
	the_range_high_value = this->range_high_value;
      } // end if else
    END_STATE
  END_METHOD_STATE_BLOCK

  if (the_method_error != NO_ERROR)
    (void) AZLog->Write(the_error_msg, This_Function_Name, the_method_error, the_method_state, A4_Log::Error);

  return  the_method_error.Get_Error_Code();
} // end Get_Range





/**
* \brief  Set the Upper & Lower range bounds. the_range_high_value must be > the_range_low_value
*/

template <class THE_TYPE>
ERROR_CODE	A4_Range<THE_TYPE>::Set_Range (THE_TYPE   the_range_low_value,
					    THE_TYPE   the_range_high_value)
{ // begin
  const ACE_TCHAR   *the_error_msg = ASL_Default_Message;

  METHOD_STATE_BLOCK_BEGIN(1)
    STATE(1)
      if (the_range_low_value > the_range_high_value)
      { // begin
         the_method_error = TRSR_LOW_VALUE_TOO_HIGH;
	 the_error_msg = ACE_TEXT ("Invalid range parameters - the_range_low_value > the_range_high_value");
      } // end if then
      else { // begin
	this->range_low_value = the_range_low_value;
	this->range_high_value = the_range_high_value;
	this->range_is_null = false;
      } // end if else
    END_STATE
  END_METHOD_STATE_BLOCK

  if (the_method_error != NO_ERROR)
    (void) AZLog->Write(the_error_msg, This_Function_Name, the_method_error, the_method_state, A4_Log::Error);

  return the_method_error.Get_Error_Code();
} // end  Set_Range



/**
* \brief  Set just the Lower bound - the_range_low_value  must be  less than this->range_high_value
*/

template <class THE_TYPE>
ERROR_CODE	A4_Range<THE_TYPE>::Set_Range_Low_Value (THE_TYPE   the_range_low_value)
{ // Begin
  const ACE_TCHAR   *the_error_msg = ASL_Default_Message;

  METHOD_STATE_BLOCK_BEGIN(1)
    STATE(1)
      if (the_range_low_value > this->range_high_value)
      { // begin
        the_method_error = TRSLV_LOW_VALUE_TOO_HIGH;

	the_error_msg = ACE_TEXT ("Invalid parameter value - the_range_low_value > this->range_high_value");
      } // end if then
      else this->range_low_value = the_range_low_value;
    END_STATE
  END_METHOD_STATE_BLOCK

  if (the_method_error != NO_ERROR)
    (void) AZLog->Write(the_error_msg, This_Function_Name, the_method_error, the_method_state, A4_Log::Error);

  return the_method_error.Get_Error_Code();
} // end Set_Range_Low_Value




/**
* \brief  Merge 2 non-null Ranges into one. Neither parameter can be NULL.
*/

template <class THE_TYPE>
ERROR_CODE	A4_Range<THE_TYPE>::Merge_Range (A4_Range<THE_TYPE>	  &the_range1,
					      A4_Range<THE_TYPE>	  &the_range2)
{ // begin
  const ACE_TCHAR   *the_error_msg = ASL_Default_Message;



  METHOD_STATE_BLOCK_BEGIN(3)
    STATE(1)
      if (the_range1.range_is_null == true)
      { // begin    
	the_method_error = TRM_RANGE1_NOT_SET;
	the_error_msg = ACE_TEXT ("Invalid parameter state - the_range1 is NULL");
      } // end if then
    END_STATE

    STATE(2)
      if (the_range2.range_is_null == true)
      { // begin
	the_method_error = TRM_RANGE2_NOT_SET;
	the_error_msg = ACE_TEXT ("Invalid parameter state - the_range2 is NULL");
      } // end if then
    END_STATE

    STATE(3) 
      this->range_low_value = min(the_range1.range_low_value, the_range2.range_low_value);
      this->range_high_value = max(the_range1.range_high_value, the_range2.range_high_value);
      this->range_is_null = false;
    END_STATE
  END_METHOD_STATE_BLOCK

  if (the_method_error != NO_ERROR)
    (void) AZLog->Write(the_error_msg, This_Function_Name, the_method_error, the_method_state, A4_Log::Error);

  return the_method_error.Get_Error_Code();
} // end Merge_Range





/**
* \brief  Set just the range High value - It must be > the current low range value
*/
template <class THE_TYPE>
ERROR_CODE	A4_Range<THE_TYPE>::Set_Range_High_Value (THE_TYPE   the_range_high_value)
{ // begin
  const ACE_TCHAR   *the_error_msg = ASL_Default_Message;


  METHOD_STATE_BLOCK_BEGIN(1)
    STATE(1)
      if (the_range_high_value < this->range_low_value)
      { // begin
        the_method_error = TRSHV_HIGH_VALUE_TOO_LOW;
	the_error_msg = ACE_TEXT ("Invalid parameter value - the_range_high_value is lessthan the current range_low_value");
      } // end if then
      else this->range_high_value = the_range_high_value;
    END_STATE
  END_METHOD_STATE_BLOCK

  if (the_method_error != NO_ERROR)
    (void) AZLog->Write(the_error_msg, This_Function_Name, the_method_error, the_method_state, A4_Log::Error);

  return the_method_error.Get_Error_Code();
}// end Set_Range_High_Value



/**
* \brief this operator acts like a Union set operator
*/

template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator == (A4_Range  &the_value)
{ // BEGIN
  bool	  is_in_the_set = false;

  if ((the_value.range_low_value <= this->range_high_value) && (the_value.range_high_value >= this->range_low_value))
    is_in_the_set = true;

  return is_in_the_set;
} // end operator ==



/**
* \brief this operator is more like a "not identical" instead of a not union operator
*/

template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator != (A4_Range  &the_value)
{ // begin
  bool	  not_identical = false;

  if ((the_value.range_low_value != this->range_low_value) || (the_value.range_high_value != this->range_high_value))
    not_identical = true;

  return not_identical;
} // end operator !=




/**
* \brief  Less than operator
*/

template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator < (A4_Range  &the_value)
{ // begin
  bool is_less_than = false;

  if (this->range_high_value < the_value.range_low_value)
    is_less_than = true;

  return is_less_than;
} // end operator <




/**
* \brief  Greater than operator
*/

template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator > (A4_Range  &the_value)
{ // begin
  bool	  is_greater_than = false;

  if (this->range_low_value > the_value.range_high_value)
    is_greater_than = true;

  return is_greater_than;
} // end operator >





template <class THE_TYPE>
bool A4_Range<THE_TYPE>::Moment_in_Range (THE_TYPE	the_moment_in_time)
{ // begin
  bool	  is_in = false;

  if ((this->Range_Is_NULL() == false) && (the_moment_in_time >= this->range_low_value) && (the_moment_in_time <= this->range_high_value))
    is_in = true;

  return is_in;
} // end Moment_in_Range




/**
* \brief  Returns true if the_value is between the low & high ranges inclusive (think about a SQL Between statement)
*/
template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator == (THE_TYPE  the_value)
{ // begin
  bool	  is_in = false;

  if ((the_value >= this->range_low_value) && (the_value <= this->range_high_value))
    is_in = true;

  return is_in;
} // end operator ==



/**
* \brief  Return \b true if the_value lies outside of this range
*/
template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator != (THE_TYPE  the_value)
{ // begin
  bool	  is_out = true;

  if ((the_value >= this->range_low_value) && (the_value <= this->range_high_value))
    is_out = false;

  return is_out;
} // end operator !=



/**
* \brief  Return \b true if the_value if less than this->range_low_value
*/
template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator < (THE_TYPE  the_value)
{ // begin
  bool	  is_less_than = false;


  if (the_value < this->range_low_value)
    is_less_than = true;

  return is_less_than;
} // end  operator <





template <class THE_TYPE>
bool A4_Range<THE_TYPE>::operator > (THE_TYPE  the_value)
{ // begin
  bool	  is_greater_than = false;


  if (the_value > this->range_high_value)
    is_greater_than = true;

  return is_greater_than;
} // end operator >


/**
* \brief Assign a new value - both high & low are assigned this data point
*/
template <class THE_TYPE>
A4_Range<THE_TYPE> & A4_Range<THE_TYPE>::operator = (THE_TYPE  the_value)
{ // begin
    this->range_low_value = the_value;
    this->range_high_value = the_value;
    this->range_is_null = false;

  return *this;
} // end operator =



/**
* \brief  Set precedence level - the idea being that not all values are created equal. Once record will take priority over another
*/
template <class THE_TYPE>
void A4_Range<THE_TYPE>::Set_Range_Precedence(void)
{ // begin
  this->range_has_precedence = true;
} // end Set_Range_Precedence




/**
* \brief  Assign a new range
*/

template <class THE_TYPE>
A4_Range<THE_TYPE> & A4_Range<THE_TYPE>::operator = (A4_Range  &the_value)
{ // begin
  switch (the_value.range_has_precedence)
  { // begin
    case false: if ((the_value.range_low_value == the_value.range_high_value) && (this->range_is_null == false))
	        { // begin - merge with existing range
		   if (the_value.range_low_value < this->range_low_value)
		     this->range_low_value = the_value.range_low_value;

		   if (the_value.range_high_value > this->range_high_value)
		     this->range_high_value = the_value.range_high_value;
		 } // end if
		 else { //begin  - overwrite local contents
		   this->range_low_value = the_value.range_low_value;
		   this->range_high_value = the_value.range_high_value;
		   this->range_is_null = false;
		 } // end if else
    break;

    case true:  // the new value has precedence and overwrites local values
	       this->range_low_value = the_value.range_low_value;
	       this->range_high_value = the_value.range_high_value;
	       this->range_is_null = false;
    break;
  } // end switch

  return *this;
} // end operator =




#endif // __Range_Template_Defined_
