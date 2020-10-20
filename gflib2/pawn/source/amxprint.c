/* Console output module (terminal I/O) for the Pawn AMX
 *
 *  Since some of these routines go further than those of standard C, they
 *  cannot always be implemented with portable C functions. In other words,
 *  these routines must be ported to other environments.
 *
 *  Copyright (c) ITB CompuPhase, 1997-2006
 *
 *  This software is provided "as-is", without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1.  The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *  2.  Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *  3.  This notice may not be removed or altered from any source distribution.
 *
 *  Version: $Id: amxcons.c 3649 2006-10-12 13:13:57Z thiadmer $
 */

#define FLOATPOINT

#if defined _UNICODE || defined __UNICODE__ || defined UNICODE
# if !defined UNICODE   /* for Windows */
#   define UNICODE
# endif
# if !defined _UNICODE  /* for C library */
#   define _UNICODE
# endif
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#if defined __WIN32__ || defined _WIN32 || defined WIN32 || defined __MSDOS__
  #include <conio.h>
  #include <malloc.h>
#endif
#if defined USE_CURSES
  #include <curses.h>
#endif
#include "../include/amx.h"
#if defined __WIN32__ || defined _WIN32 || defined WIN32
#include <windows.h>
#include <winnls.h>
#endif

#if defined _UNICODE
# include <tchar.h>
#elif !defined __T
  typedef char          TCHAR;
# define __T(string)    string
# define _tcschr        strchr
# define _tcscpy        strcpy
# define _tcsdup        strdup
# define _tcslen        strlen
# define _stprintf      sprintf
#endif
#include <pawn/include/amxcons.h>

#if defined __MSDOS__
  #define EOL_CHAR       '\r'
#endif
#if defined __WIN32__ || defined _WIN32 || defined WIN32
  #define EOL_CHAR       '\r'
#endif
#if !defined EOL_CHAR
  /* if not a "known" operating system, assume Linux/Unix */
  #define EOL_CHAR     '\n'
#endif


#if !defined AMX_STRING_LIB


#if defined AMX_TERMINAL
  /* required functions are implemented elsewhere */
  int amx_putstr(const TCHAR *);
  int amx_putchar(int);
  int amx_fflush(void);
#elif defined VT100 || defined LINUX || defined ANSITERM
  /* ANSI/VT100 terminal, or shell emulating "xterm" */
  #define amx_putstr(s)   printf("%s",(s))
  #define amx_putchar(c)  putchar(c)
  #define amx_fflush()    fflush(stdout)
#elif defined __WIN32__ || defined _WIN32 || defined WIN32
/*
-------------------------------------------------------------
これは、Pawn version 3.3.4097に
株式会社ゲームフリークが改変を加えたものです。
-------------------------------------------------------------
*/
/* Win32 console */ // GameFreak
#if 0
  #define amx_putstr(s)     OutputDebugString(s)
  #define amx_putchar(c) \
{ \
    char s[2]; \
    s[0] = c; \
    s[1] = 0x00; \
    OutputDebugString(s); \
  }
#else
#include <locale.h>

#define TEMPSTR_MAX (256)
static int   s_ii = 0;
static char s_pTempStr[ TEMPSTR_MAX ];

// niji特
void  amx_putstr( const TCHAR *s ){
  int ii;
  for(ii=0 ; (s_ii<TEMPSTR_MAX-1 && 0 != *(s+ii))  ; ii++)  s_pTempStr[ s_ii++ ] = *(s+ii);
}
void  amx_putchar(TCHAR c){
  if( s_ii < TEMPSTR_MAX-1 ) s_pTempStr[ s_ii++ ] = c;
}
void  amx_putstring_debug()
{
  WCHAR  utf16_buff[256];
  char  sjis_buff[256];
  char tbuff[256];
  int nSize;

  memset(tbuff,0,sizeof(tbuff));
  memset(utf16_buff,0,sizeof(utf16_buff));
  memset(sjis_buff,0,sizeof(sjis_buff));

  nSize = MultiByteToWideChar(CP_UTF8, 0, tbuff, -1, NULL, 0);
  MultiByteToWideChar(CP_UTF8,0,s_pTempStr,s_ii,utf16_buff,256);
  OutputDebugString( utf16_buff );
  OutputDebugString( L"\n"); //DebugView対策　改行を入れないと出力が飛ぶことがある
  //    WideCharToMultiByte(CP_ACP,0,utf16_buff,s_ii,sjis_buff,256,NULL,NULL);
  //    OutputDebugStringA( sjis_buff );
}
#endif

  #define amx_fflush()
#elif defined USE_CURSES
  /* Use the "curses" library to implement the console */
  #define amx_putstr(s)       printw("%s",(s))
  #define amx_putchar(c)      addch(c)
  #define amx_fflush()        (0)
/*
-------------------------------------------------------------
これは、Pawn version 3.3.4097に
株式会社ゲームフリークが改変を加えたものです。
-------------------------------------------------------------
*/
#elif defined GF_PLATFORM_CTR // GameFreak

#if GFL_PAWN_DEBUG

#include <locale.h>
#include <wchar.h>
#include <nn/assert.h>
#pragma import(__use_utf8_ctype)
//#pragma import(__use_sjis_ctype)

#define TEMPSTR_MAX (256)
static int   s_ii = 0;
static char s_pTempStr[ TEMPSTR_MAX ];

//#define amx_putstr(s)       nndbgDetailPrintf(s)
//#define amx_putchar(c)      nndbgDetailPrintf("%c",c)
// niji特
void  amx_putstr( const TCHAR *s ){
  int ii;
  for(ii=0 ; (s_ii<TEMPSTR_MAX-1 && 0 != *(s+ii))  ; ii++)  s_pTempStr[ s_ii++ ] = *(s+ii);
}
void  amx_putchar(TCHAR c){
  if( s_ii < TEMPSTR_MAX-1 ) s_pTempStr[ s_ii++ ] = c;
}
void  amx_putstring_debug(){
  nndbgDetailPrintf(s_pTempStr);

#if 0
  wchar_t utf16_buff[256];
  char  sjis_buff[256];
  int nSize;
  char* tmpLocale = NULL;

  memset(utf16_buff,0,sizeof(utf16_buff));
  memset(sjis_buff,0,sizeof(sjis_buff));
  tmpLocale = setlocale(LC_CTYPE,NULL);
  tmpLocale = setlocale(LC_CTYPE,"UTF-8");
  tmpLocale = setlocale(LC_CTYPE,"SJIS");
  nSize = mbstowcs( utf16_buff, s_pTempStr,  strlen( s_pTempStr )+1 );
  nSize = wcstombs( sjis_buff, utf16_buff, nSize );
  nndbgDetailPrintf(sjis_buff);

  setlocale(LC_CTYPE,"ja_JP.UTF-8");
  setlocale(LC_CTYPE,"SJIS");
  nSize = mbstowcs( utf16_buff, s_pTempStr,  strlen( s_pTempStr )+1 );
  setlocale(LC_CTYPE,"__use_utf8_ctype");
  nSize = mbstowcs( utf16_buff, s_pTempStr,  strlen( s_pTempStr )+1 );
  setlocale(LC_CTYPE,"SJIS");
  nSize = wcstombs( sjis_buff, utf16_buff, nSize );
  nndbgDetailPrintf(sjis_buff);
  setlocale(LC_CTYPE,tmpLocale);
#endif
}

#else
#define amx_putstr(s)
#define amx_putchar(c)
#endif

#define amx_fflush()
#else
  /* assume a streaming terminal; limited features (no colour, no cursor
   * control)
   */
  #define amx_putstr(s)       printf("%s",(s))
  #define amx_putchar(c)      putchar(c)
  #define amx_fflush()        fflush(stdout)
#endif

#include <macro/include/gfl2_Macros.h>
#if defined GF_PLATFORM_WIN32 // GameFreak
GFL_WARNING_WIN32_IGNORE(4133)
#elif defined GF_PLATFORM_CTR // GameFreak
GFL_WARNING_CTR_IGNORE(177)
#endif


static int cons_putstr(void *dest,const TCHAR *str)
{
  (void)dest;
  amx_putstr(str);
  return 0;
}

static int cons_putchar(void *dest,TCHAR ch)
{
  (void)dest;
  amx_putchar(ch);
  return 0;
}

#endif /* AMX_STRING_LIB */

enum {
  SV_DECIMAL,
  SV_HEX
};

static TCHAR *reverse(TCHAR *string,int stop)
{
  int start=0;
  TCHAR temp;

  /* swap the string */
  stop--;       /* avoid swapping the '\0' byte to the first position */
  while (stop - start > 0) {
    temp = string[start];
    string[start] = string[stop];
    string[stop] = temp;
    start++;
    stop--;
  } /* while */
  return string;
}

/* Converts an integral value to a string, with optional padding with spaces or
 * zeros.
 * The "format" must be decimal or hexadecimal
 * The number is right-aligned in the field with the size of the absolute value
 * of the "width" parameter.
 * If the width value is positive, the string is padded with spaces; if it is
 * negative, it is padded with zeros.
 */
static TCHAR *amx_strval(TCHAR buffer[], long value, int format, int width)
{
  int start, stop;
  TCHAR temp;

  start = stop = 0;
  if (format == SV_DECIMAL) {
    if (value < 0) {
      buffer[0] = __T('-');
      start = stop = 1;
      value = -value;
    } /* if */
    do {
      buffer[stop++] = (TCHAR)((value % 10) + __T('0'));
      value /= 10;
    } while (value > 0);
  } else {
    /* hexadecimal */
    unsigned long v = (unsigned long)value; /* copy to unsigned value for shifting */
    do {
      buffer[stop] = (TCHAR)((v & 0x0f) + __T('0'));
      if (buffer[stop] > __T('9'))
        buffer[stop] += (TCHAR)(__T('A') - __T('0') - 10);
      v >>= 4;
      stop++;
    } while (v != 0);
  } /* if */

  /* pad to given width */
  if (width < 0) {
    temp = __T('0');
    width = -width;
  } else {
    temp = __T(' ');
  } /* if */
  while (stop < width)
    buffer[stop++] = temp;

  buffer[stop] = __T('\0');

  /* swap the string, and we are done */
  reverse(buffer+start,stop-start);
  return buffer;
}

#if defined FIXEDPOINT
  #define FIXEDMULT     1000
  #define FIXEDDIGITS   3

static TCHAR *formatfixed(TCHAR *string,cell value,TCHAR align,int width,TCHAR decpoint,int digits,TCHAR filler)
{
  int i, len;
  cell ipart,v;
  TCHAR vsign=__T('\0');

  /* make the value positive (but keep the sign) */
  if (value<0) {
    value=-value;
    vsign=__T('-');
  } /* if */

  /* "prepare" the value so that when it is truncated to the requested
   * number of digits, the result is rounded towards the dropped digits
   */
  assert(digits<INT_MAX);
  v=FIXEDMULT/2;
  for (i=0; i<digits; i++)
    v/=10;
  value+=v;

  /* get the integer part and remove it from the value */
  ipart=value/FIXEDMULT;
  value-=FIXEDMULT*ipart;
  assert(ipart>=0);
  assert(value>=0);

  /* truncate the fractional part to the requested number of digits */
  for (i=FIXEDDIGITS; i>digits; i--)
    value/=10;

  string[0]=__T('\0');

  /* add sign */
  i=_tcslen(string);
  string[i]=vsign;
  string[i+1]=__T('\0');

  /* add integer part */
  amx_strval(string+_tcslen(string),(long)ipart,SV_DECIMAL,0);

  /* add fractional part */
  if (digits>0) {
    i=_tcslen(string);
    string[i]=decpoint;
    amx_strval(string+i+1,(long)value,SV_DECIMAL,-digits);
  } /* if */

  len=_tcslen(string);
  if (len<width) {
    /* pad to the requested width */
    for (i=len; i<width; i++)
      string[i]=filler;
    string[i]=__T('\0');
    /* optionally move the padding to the beginning of the string, using the handwaving algorithm */
    if (align!=__T('-')) {
      assert(i==(int)_tcslen(string));
      assert(i>=len);
      reverse(string,len);
      reverse(string+len,i-len);
      reverse(string,i);
    } /* if */
  } /* if */

  return string;
}
#endif


static int dochar(AMX *amx,TCHAR ch,cell param,TCHAR sign,TCHAR decpoint,int width,int digits,TCHAR filler,
                  int (*f_putstr)(void*,const TCHAR *),int (*f_putchar)(void*,TCHAR),void *user)
{
  cell *cptr;
  TCHAR buffer[40];
  #if defined FLOATPOINT
    TCHAR formatstring[40];
  #endif

//  #if !defined FIXEDPOINT && !defined FLOATPOINT
    (void)decpoint;
//  #endif
  assert(f_putstr!=NULL);
  assert(f_putchar!=NULL);

  switch (ch) {
  case __T('%'):
    f_putchar(user,ch);
    return 0;

  case __T('c'):
    amx_GetAddr(amx,param,&cptr);
    width--;            /* single character itself has a with of 1 */
    if (sign!=__T('-'))
      while (width-->0)
        f_putchar(user,filler);
    f_putchar(user,(TCHAR)*cptr);
    while (width-->0)
      f_putchar(user,filler);
    return 1;

  case __T('d'): {
    cell value;
    int length=1;
    amx_GetAddr(amx,param,&cptr);
    value=*cptr;
    if (value<0 || sign==__T('+'))
      length++;
    if (value<0)
      value=-value;
    while (value>=10) {
      length++;
      value/=10;
    } /* while */
    width-=length;
    if (sign!=__T('-'))
      while (width-->0)
        f_putchar(user,filler);
    amx_strval(buffer,*cptr,SV_DECIMAL,0);
    if (sign==__T('+') && *cptr>=0)
      f_putchar(user,sign);
    f_putstr(user,buffer);
    while (width-->0)
      f_putchar(user,filler);
    return 1;
  } /* case */

#if defined FLOATPOINT
  case __T('f'): /* 32-bit floating point number */
  case __T('r'): /* if floating point is enabled, %r == %f */
    /* build a format string */
    if (digits==INT_MAX)
      digits=5;
    else if (digits>25)
      digits=25;
    _tcscpy(formatstring,__T("%"));
    if (sign!=__T('\0'))
      _stprintf(formatstring+_tcslen(formatstring),__T("%c"),sign);
    if (width>0)
      _stprintf(formatstring+_tcslen(formatstring),__T("%d"),width);
    _stprintf(formatstring+_tcslen(formatstring),__T(".%df"),digits);
    /* ??? decimal comma? */
    amx_GetAddr(amx,param,&cptr);
    _stprintf(buffer,formatstring,*(float*)cptr);
    f_putstr(user,buffer);
    return 1;
#endif

#if defined FIXEDPOINT
  #define FIXEDMULT 1000
  case __T('q'): /* 32-bit fixed point number */
#if !defined FLOATPOINT
  case __T('r'): /* if fixed point is enabled, and floating point is not, %r == %q */
#endif
    amx_GetAddr(amx,param,&cptr);
    /* format the number */
    if (digits==INT_MAX)
      digits=3;
    else if (digits>25)
      digits=25;
    formatfixed(buffer,*cptr,sign,width,decpoint,digits,filler);
    assert(_tcslen(buffer)<sizeof buffer);
    f_putstr(user,buffer);
    return 1;
#endif

#if !defined FLOATPOINT && !defined FIXEDPOINT
  case __T('f'):
  case __T('q'):
  case __T('r'):
    f_putstr(user,__T("(no rational number support)"));
    return 0; /* flag this as an error */
#endif

  case __T('s'): {
    AMX_FMTINFO info;
    memset(&info,0,sizeof info);
    info.length=digits;
    info.f_putstr=f_putstr;
    info.f_putchar=f_putchar;
    info.user=user;
    amx_GetAddr(amx,param,&cptr);
    amx_printstring(amx,cptr,&info);
    return 1;
  } /* case */

  case __T('x'): {
    ucell value;
    int length=1;
    amx_GetAddr(amx,param,&cptr);
    value=*(ucell*)cptr;
    while (value>=0x10) {
      length++;
      value>>=4;
    } /* while */
    width-=length;
    if (sign!=__T('-'))
      while (width-->0)
        f_putchar(user,filler);
    amx_strval(buffer,(long)*cptr,SV_HEX,0);
    f_putstr(user,buffer);
    while (width-->0)
      f_putchar(user,filler);
    return 1;
  } /* case */

  } /* switch */
  /* error in the string format, try to repair */
  f_putchar(user,ch);
  return 0;
}

enum {
  FMT_NONE,   /* not in format state; accept '%' */
  FMT_START,  /* found '%', accept '+', '-' (START), '0' (filler; START), digit (WIDTH), '.' (DECIM) or format letter (done) */
  FMT_WIDTH,  /* found digit after '%' or sign, accept digit (WIDTH), '.' (DECIM) or format letter (done) */
  FMT_DECIM,  /* found digit after '.', accept accept digit (DECIM) or format letter (done) */
};

static int formatstate(TCHAR c,int *state,TCHAR *sign,TCHAR *decpoint,int *width,int *digits,TCHAR *filler)
{
  assert(state!=NULL && sign!=NULL && decpoint!=NULL && width!=NULL && digits!=NULL && filler!=NULL);
  switch (*state) {
  case FMT_NONE:
    if (c==__T('%')) {
      *state=FMT_START;
      *sign=__T('\0');
      *decpoint=__T('.');
      *width=0;
      *digits=INT_MAX;
      *filler=__T(' ');
    } else {
      return -1;  /* print a single character */
    } /* if */
    break;
  case FMT_START:
    if (c==__T('+') || c==__T('-')) {
      *sign=c;
    } else if (c==__T('0')) {
      *filler=c;
    } else if (c>=__T('1') && c<=__T('9')) {
      *width=(int)(c-__T('0'));
      *state=FMT_WIDTH;
    } else if (c==__T('.') || c==__T(',')) {
      *decpoint=c;
      *digits=0;
      *state=FMT_DECIM;
    } else {
      return 1; /* print formatted character */
    } /* if */
    break;
  case FMT_WIDTH:
    if (c>=__T('0') && c<=__T('9')) {
      *width=*width*10+(int)(c-__T('0'));
    } else if (c==__T('.') || c==__T(',')) {
      *decpoint=c;
      *digits=0;
      *state=FMT_DECIM;
    } else {
      return 1; /* print formatted character */
    } /* if */
    break;
  case FMT_DECIM:
    if (c>=__T('0') && c<=__T('9')) {
      *digits=*digits*10+(int)(c-__T('0'));
    } else {
      return 1; /* print formatted character */
    } /* if */
    break;
  } /* switch */

  return 0;
}

int amx_printstring(AMX *amx,cell *cstr,AMX_FMTINFO *info)
{
  int i,paramidx=0;
  int fmtstate=FMT_NONE,width,digits;
  TCHAR sign,decpoint,filler;
  int (*f_putstr)(void*,const TCHAR *);
  int (*f_putchar)(void*,TCHAR);
  void *user;
  int skip,length;

#if GFL_PAWN_DEBUG
  s_ii = 0;
#endif

  if (info!=NULL) {
    f_putstr=info->f_putstr;
    f_putchar=info->f_putchar;
    user=info->user;
    skip=info->skip;
    length=info->length;
  } else {
    f_putstr=NULL;
    f_putchar=NULL;
    user=NULL;
    skip=0;
    length=INT_MAX;
  } /* if */
  #if !defined AMX_STRING_LIB
    if (f_putstr==NULL)
      f_putstr=cons_putstr;
    if (f_putchar==NULL)
      f_putchar=cons_putchar;
  #else
    assert(f_putstr!=NULL && f_putchar!=NULL);
  #endif

  /* if no placeholders appear, we can use a quicker routine */
  if (info==NULL || info->params==NULL) {

    TCHAR cache[100];
    int idx=0;

    if ((ucell)*cstr>UNPACKEDMAX) {
      int j=sizeof(cell)-sizeof(char);
      char c;
      /* the string is packed */
      i=0;
      for ( ; ; ) {
        c=(char)((ucell)cstr[i] >> 8*j);
        if (c==0)
          break;
        if (skip>0) {
          skip--;               /* skip a number of characters */
        } else {
          if (length--<=0)
            break;              /* print up to a certain length */
          assert(idx<(int)sizeof cache);
          cache[idx++]=c;
          if (idx==sizeof cache - 1) {
            cache[idx]=__T('\0');
            f_putstr(user,cache);
            idx=0;
          } /* if */
        } /* if */
        if (j==0)
          i++;
        j=(j+sizeof(cell)-sizeof(char)) % sizeof(cell);
      } /* for */
    } else {
      /* unpacked string */
      for (i=0; cstr[i]!=0; i++) {
        if (skip-->0)
          continue;
        assert(idx<(int)sizeof cache);
        cache[idx++]=(TCHAR)cstr[i];
        if (idx==sizeof cache - 1) {
          cache[idx]=__T('\0');
          f_putstr(user,cache);
          idx=0;
        } /* if */
      } /* for */
    } /* if */
    if (idx>0) {
      cache[idx]=__T('\0');
      f_putstr(user,cache);
    } /* if */

  } else {

    /* check whether this is a packed string */
    if ((ucell)*cstr>UNPACKEDMAX) {
      int j=sizeof(cell)-sizeof(char);
      char c;
      /* the string is packed */
      i=0;
      for ( ; ; ) {
        c=(char)((ucell)cstr[i] >> 8*j);
        if (c==0)
          break;
        switch (formatstate(c,&fmtstate,&sign,&decpoint,&width,&digits,&filler)) {
        case -1:
          f_putchar(user,c);
          break;
        case 1:
          assert(info!=NULL && info->params!=NULL);
          paramidx+=dochar(amx,c,info->params[paramidx],sign,decpoint,width,digits,filler,
                           f_putstr,f_putchar,user);
          fmtstate=FMT_NONE;
          break;
        case 0:
          assert(info!=NULL && info->params!=NULL);
          if (paramidx>=info->numparams)  /* insufficient parameters passed */
            amx_RaiseError(amx, AMX_ERR_NATIVE);
          break;
        } /* switch */
        if (j==0)
          i++;
        j=(j+sizeof(cell)-sizeof(char)) % sizeof(cell);
      } /* for */
    } else {
      /* the string is unpacked */
      for (i=0; cstr[i]!=0; i++) {
        switch (formatstate((TCHAR)cstr[i],&fmtstate,&sign,&decpoint,&width,&digits,&filler)) {
        case -1:
          f_putchar(user,(TCHAR)cstr[i]);
          break;
        case 1:
          assert(info!=NULL && info->params!=NULL);
          paramidx+=dochar(amx,(TCHAR)cstr[i],info->params[paramidx],sign,decpoint,width,digits,filler,
                           f_putstr,f_putchar,user);
          fmtstate=FMT_NONE;
          break;
        case 0:
          assert(info!=NULL && info->params!=NULL);
          if (paramidx>=info->numparams)  /* insufficient parameters passed */
            amx_RaiseError(amx, AMX_ERR_NATIVE);
          break;
        } /* switch */
      } /* for */
    } /* if */

  } /* if (info==NULL || info->params==NULL) */

#if GFL_PAWN_DEBUG
  s_pTempStr[ s_ii ] = '\0';
  amx_putstring_debug();
#endif

  return paramidx;
}

static cell AMX_NATIVE_CALL n_printf(AMX *amx,const cell *params)
{
  cell *cstr;
  AMX_FMTINFO info;

  memset(&info,0,sizeof info);
  info.params=params+2;
  info.numparams=(int)(params[0]/sizeof(cell))-1;
  info.skip=0;
  info.length=INT_MAX;

  amx_GetAddr(amx,params[1],&cstr);
  amx_printstring(amx,cstr,&info);
  amx_fflush();
  return 0;
}

AMX_NATIVE_INFO print_natives[] = {
  { "printf",n_printf },
  {0,0}
};

int AMXEXPORT AMXAPI amx_PrintInit(AMX *amx)
{
    return amx_Register(amx, print_natives, -1);
}

