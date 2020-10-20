/* 
 * File:   util.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#ifndef __UTIL_H__
#define	__UTIL_H__


namespace util
{
	int CalcFrameToPosX(const int frame , const int width );

	int CalcPosxToFrame(const int posX, const int width);
}


#endif  __UTIL_H__