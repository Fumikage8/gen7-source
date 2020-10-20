/* 
 * File:   util.cpp
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/04
 */

#include "util.h"
#include "localDef.h"


namespace util
{
	int CalcFrameToPosX(const int frame , const int width )
	{
		return frame * width;
	}

	int CalcPosxToFrame(const int posX, const int width)
	{
		return posX / width;
	}
}
