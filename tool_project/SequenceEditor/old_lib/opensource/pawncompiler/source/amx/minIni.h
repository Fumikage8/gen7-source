/*  minIni - Multi-Platform INI file parser, suitable for embedded systems
 *
 *  Copyright (c) ITB CompuPhase, 2008
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
 *  Version: $Id: minIni.h 4008 2008-09-09 08:14:39Z thiadmer $
 */
#ifndef MININI_H
#define MININI_H

#if defined _UNICODE || defined __UNICODE__ || defined UNICODE
  #include <tchar.h>
#elif !defined __T
  typedef char TCHAR;
#endif

#if defined __cplusplus
  extern "C" {
#endif

long ini_getl(const TCHAR *Section, const TCHAR *Key, long DefValue, const TCHAR *Filename);
int  ini_gets(const TCHAR *Section, const TCHAR *Key, const TCHAR *DefValue, TCHAR *Buffer, int BufferSize, const TCHAR *Filename);
int  ini_putl(const TCHAR *Section, const TCHAR *Key, long Value, const TCHAR *Filename);
int  ini_puts(const TCHAR *Section, const TCHAR *Key, const TCHAR *Value, const TCHAR *Filename);

#if defined __cplusplus
  }
#endif

#endif /* MININI_H */
