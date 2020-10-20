/**
* @brief  �t�B�[���h�n�`�u���b�N��LOD�؂�ւ��̒ʒm���󂯂���N���X( �����z�u�ȍ~�A�����Ȃ��I�u�W�F�N�g�p )
* @file   FieldTerrainBlockLodChangeStaticListener.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINLODCHANGESTATICLISTENER_H_INCLUDED__ )
#define __FIELDTERRAINLODCHANGESTATICLISTENER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>

// field
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodType.h"

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );
GFL_NAMESPACE_BEGIN( Block );

/**
* @brief �t�B�[���h�n�`�u���b�N��LOD�؂�ւ��̒ʒm���󂯂���N���X
* @note  �����z�u�ȍ~�A�����Ȃ��I�u�W�F�N�g�p
*/
class LodChangeStaticListener
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( LodChangeStaticListener );

public:

  /*
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  LodChangeStaticListener( void ){}

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~LodChangeStaticListener( void ){}

  /**
  * @brief �n�`�u���b�N�ɓo�^���ꂽ�u�ԂɎ��s�����֐�
  *
  * @param  lodType �o�^���̒n�`��LOD���
  *
  * @return ����
  */
  virtual void RegistAfter( LodType::Value lodType ) = 0;
  
  /**
  * @brief �n�`�u���b�N��LOD���؂�ւ�����u�ԂɎ��s�����֐�
  *
  * @param  lodType �؂�ւ����LOD���
  *
  * @return ����
  */
  virtual void ChangeLod( LodType::Value lodType ) = 0;

  /**
  * @brief ��������n�`�u���b�N�����肷����W���擾����
  *
  * @param  pOut�@���W�i�[��  
  *
  * @return ����
  */
  virtual b32 GetControlPosition( gfl2::math::Vector3* pOut ) const = 0; 

private:


};  // class LodChangeStaticListener


GFL_NAMESPACE_END( Block );
GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINLODCHANGESTATICLISTENER_H_INCLUDED__