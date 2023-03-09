//begin adder.h
#ifndef MATHSLIB_ADDER_H
#define MATHSLIB_ADDER_H
#if defined(WIN32)|defined(_WIN32)
/** @file
*This file contains the declarations of all exported functions in the maths library.
*/
/** Adding function
* This function adds two numbers [Brief description]
* @param a is the first number  [More detail]
* @param b  is the second number [Parameter definition]
* @return sum of a and b [Return value description]
*/
#ifdef maths_STATIC
#define MATHSLIB_API
#else
#ifdef maths_EXPORTS
#define MATHSLIB_API __declspec( dllexport )
#else
#define MATHSLIB_API __declspec( dllimport )
#endif
#endif
#else
#define MATHSLIB_API
#endif
MATHSLIB_API int add( int a, int b);
#endif
//end - adder.h-------

