#ifndef __GFL_MATHCRC_H__
#define __GFL_MATHCRC_H__
//============================================================================================
/**
 *	GAME FREAK inc.
 *	@file		gfl_MathCRC.h
 *	@brief  CRC�v�Z
 *	@author	k.ohno
 *	@date		2011.05.25
 */
//============================================================================================
#pragma once

namespace gfl{
namespace math{

  class Crc {
    GFL_FORBID_COPY_AND_ASSIGN(Crc); //�R�s�[�R���X�g���N�^�{����֎~

  public:
    //------------------------------------------------------------------
    /**
    * @name     Calc
    * @brief    Crc16Ccitt�̌v�Z ���ʂ�Ԃ� 
    * @param    input   CRC���v�Z���邽�߂̃o�b�t�@
    * @param    length  ����
    * @return   CRC�̒l
    */
    //------------------------------------------------------------------
    static int Calc(const void * input,size_t length){ return nn::util::Crc16Ccitt::Calculate(input,length); };
    

  };

}
}

#endif // __GFL_MATHCRC_H__
