/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2004, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id: escape.c,v 1.36 2004/07/01 08:10:21 bagder Exp $
 ***************************************************************************/

/* Escape and unescape URL encoding in strings. The functions return a new
 * allocated string or NULL if an error occurred.  */
 
/***************************************************************************
 * Modified version for Allmusic Hotkey Winamp Plugin.
 * Original source can be found at http://curl.haxx.se/download.html
 ***************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *curl_escape(const char *string, int inlength)
{
  size_t alloc = (inlength?(size_t)inlength:strlen(string))+1;
  char *ns;
  char *testing_ptr = NULL;
  unsigned char in;
  size_t newlen = alloc;
  int strindex=0;
  size_t length;

  ns = (char *)malloc(alloc);
  if(!ns)
    return NULL;

  length = alloc-1;
  while(length--) {
    in = *string;
    if(in == ' '){
      /* ' '->'+' */
      ns[strindex++]='+';
    }
    else if(!(in >= 'a' && in <= 'z') &&
       !(in >= 'A' && in <= 'Z') &&
       !(in >= '0' && in <= '9')) {
      /* encode it */
      newlen += 2; /* the size grows with two, since this'll become a %XX */
      if(newlen > alloc) {
        alloc *= 2;
        testing_ptr = (char *)realloc(ns, alloc);
        if(!testing_ptr) {
          free( ns );
          return NULL;
        }
        else {
          ns = testing_ptr;
        }
      }
      snprintf(&ns[strindex], 4, "%%%02X", in);

      strindex+=3;
    }
    else {
      /* just copy this */
      ns[strindex++]=in;
    }
    string++;
  }
  ns[strindex]=0; /* terminate it */
  return ns;
}

/* For operating systems/environments that use different malloc/free
   ssystems for the app and for this library, we provide a free that uses
   the library's memory system */
void curl_free(void *p)
{
  if(p)
    free(p);
}
