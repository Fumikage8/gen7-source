/**
* @brief  �t�B�[���h�̒n�`�u���b�N�֘A�̒�`
* @file   FieldTerrainBlockTypes.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINBLOCKTYPES_H_INCLUDED__ )
#define __FIELDTERRAINBLOCKTYPES_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );
GFL_NAMESPACE_BEGIN( Block );

/** @brief  �n�`�u���b�N�̃A�j���[�V�����X���b�g�p�r */
struct AnimationSlotNo
{
  enum Value
  {
    LOCAL = 0,  // ���[�J�����[�v�A�j���[�V����
    ONE_DAY,    // 1�������A�j���[�V����
    MAX
  };
};


GFL_NAMESPACE_END( Block );
GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINBLOCKTYPES_H_INCLUDED__