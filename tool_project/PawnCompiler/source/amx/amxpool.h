/*  Simple allocation from a memory pool, with automatic release of
 *  least-recently used blocks (LRU blocks).
 *
 *  Copyright (c) ITB CompuPhase, 2007-2009
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
 *  Version: $Id: amxpool.h 4057 2009-01-15 08:21:31Z thiadmer $
 */
#ifndef AMXPOOL_H_INCLUDED
#define AMXPOOL_H_INCLUDED

void  amx_poolinit(void *pool, unsigned size);
void *amx_poolalloc(unsigned size, int index);
void  amx_poolfree(void *block);
void *amx_poolfind(int index);
int   amx_poolprotect(int index);


#endif /* AMXPOOL_H_INCLUDED */
