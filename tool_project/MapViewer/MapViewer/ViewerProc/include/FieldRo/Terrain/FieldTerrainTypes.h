/**
* @brief  �t�B�[���h�̒n�`�֘A�̒�`
* @file   FieldTerrainTypes.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINTYPES_H_INCLUDED__ )
#define __FIELDTERRAINTYPES_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

// �G���A�f�[�^�����̃f�[�^�\����`
struct AreaResourceDataIndex
{
  enum Value
  {
    TEXTURE,                // �e�N�X�`��  ��high���f�����܂ށA�S�Ă̒n�`�u���b�N�Ɖ��i���f�����g�p����e�N�X�`�����p�b�N����Ă���
    SHADER,                 // �V�F�[�_�[  ��high���f�����܂ށA�S�Ă̒n�`�u���b�N�Ɖ��i���f�����g�p����V�F�[�_�[���p�b�N����Ă���
    MODEL_MIDDLE,           // �n�`�u���b�N�Amiddle���f���f�[�^
    MODEL_LOW,              // �n�`�u���b�N�Alow���f���f�[�^
    MODEL_DISTANT_VIEW,     // ���i���f���f�[�^
    ANIME_LOCAL,            // ���[�J�����[�v�A�j���[�V����
    ANIME_ONE_DAY,          // 1�������A�j���[�V���� 
    MAX
  };
};

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINTYPES_H_INCLUDED__