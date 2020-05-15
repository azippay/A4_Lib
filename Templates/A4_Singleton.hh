#ifndef __A4_Singleton_Defined__
#define __A4_Singleton_Defined__
/**
* \author Albert Zippay 
* \copyright Albert Zippay - 1995 - 2019
* \file A4_Singleton.hh
*
* \brief  Simple Singleton pattern...with A4_Lib idiom naming. The idea here is to have a light-weight singleton depending upon the STL. First and final usage should occur from the main thread.
*
* \note  
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


/// Basic Singleton - no managed memory, no early destruction or re-initialization. Accounts for 99% of all use cases.

#define A4_Declare_Singleton(T)\
A4_Export static T & Instance(void){\
static T  instance_of_##T;\
return instance_of_##T;}


#endif

