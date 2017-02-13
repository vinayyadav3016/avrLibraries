/*! \file cpluscplus_use.hpp
 */
#ifndef __CPLUSPLUS_USE_HPP__
#define __CPLUSPLUS_USE_HPP__
#include <stdlib.h>
#include <inttypes.h>
///
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
/*!
 */
void * operator new(size_t size) {
	return malloc(size);
}
/*!
 */
void operator delete(void * ptr) {
	free(ptr);
}
/*!
 */
void * operator new[](size_t size) {
	return malloc(size);
}
/*!
 */
void operator delete[](void * ptr)  {
	free(ptr);
}
/*!
 */
__extension__ typedef int __guard __attribute__((mode (__DI__)));
extern "C" int  __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release(__guard *);
extern "C" void __cxa_guard_abort(__guard *);
/*!
 */
int  __cxa_guard_acquire(__guard *g) {return !*(char *)(g);}
void __cxa_guard_release(__guard *g) {*(char *)g = 1;}
void __cxa_guard_abort(__guard *) {}
/*!
 */
extern "C" void __cxa_pure_virtual(void);
/*!
 */
void __cxa_pure_virtual(void) {}
#endif//__CPLUSPLUS_USE_HPP__
