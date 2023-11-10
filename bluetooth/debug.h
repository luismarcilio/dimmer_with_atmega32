/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01-06-2009 20:17:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luis Marcilio Braga de Oliveira (Eng. Elet.), luismarcilio@gmail.com
 *        Company:  Universidade Estácio de Sá
 *
 * =====================================================================================
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <stdio.h>

#ifdef DEBUG
#define Log(A,B,C) do_log(A,B,C,__LINE__,__FILE__,__func__)
#define DBG(...) {printf("%s:%d(%s):", __FILE__, __LINE__,__func__);\
                      printf(__VA_ARGS__);}  
#endif

extern const char *event_descriptions[0x21]; 
extern char *error_descriptions[0x29];

#endif //__DEBUG_H__
