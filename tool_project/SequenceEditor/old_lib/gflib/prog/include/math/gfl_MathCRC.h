#ifndef __GFL_MATHCRC_H__
#define __GFL_MATHCRC_H__
//============================================================================================
/**
 *	GAME FREAK inc.
 *	@file		gfl_MathCRC.h
 *	@brief  CRC計算
 *	@author	k.ohno
 *	@date		2011.05.25
 */
//============================================================================================
#pragma once

namespace gfl{
namespace math{

  class Crc {
    GFL_FORBID_COPY_AND_ASSIGN(Crc); //コピーコンストラクタ＋代入禁止

  public:
    //------------------------------------------------------------------
    /**
    * @name     Calc
    * @brief    Crc16Ccittの計算 結果を返す 
    * @param    input   CRCを計算するためのバッファ
    * @param    length  長さ
    * @return   CRCの値
    */
    //------------------------------------------------------------------
    static int Calc(const void * input,size_t length){ return nn::util::Crc16Ccitt::Calculate(input,length); };
    

  };

}
}

#endif // __GFL_MATHCRC_H__
