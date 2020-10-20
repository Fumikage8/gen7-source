/**
* @brief  �|�P�����ӂꂠ���p�f�[�^�A�N�Z�b�T�[
* @file   FieldDataAccessor.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDPOKEMONFUREAI_H_INCLUDED__ )
#define __FIELDPOKEMONFUREAI_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#if 0
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#endif

// field
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTypes.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( PokemonFureai );

/**
* @brief �|�P�����ӂꂠ���p�f�[�^�A�N�Z�b�T�[
*/
class DataAccessor
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( DataAccessor );

public:

  struct Header
  {
    s32 version;
    s32 data_num;
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  DataAccessor( void* pData );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~DataAccessor( void );
  
#if 0
  /**
  * @brief ����������
  *
  * @param  rInitDesc   �������ݒ�
  *
  * @return ����
  */
  void Initialize( const InitializeDescription& rInitDesc );
  /**
  * @brief �I������
  *
  * @return ����
  */
  void Terminate( void );
#endif

  /**
  * @brief �ݒ�f�[�^���擾
  *
  * @param modelId �擾����|�P�����̃��f��ID
  *
  * @return �ݒ�f�[�^�ւ̃|�C���^
  */
  const POKEMON_SETTING_DATA* GetSettingData( s32 modelId );

  /**
  * @brief �������x�����擾
  */
  u32 GetVoiceID( s32 modelId, s32 type );

#if PM_DEBUG
  void Dump( void );
#endif

private:


  //
  // ����
  //
  void*                       m_pData;
  Header*                     m_pHeaderData;
  POKEMON_SETTING_DATA*       m_pBodyData;
  //
  // �����I�u�W�F�N�g
  //

  //
  // �O���I�u�W�F�N�g
  //

  //
  // �f�o�b�O�p
  //
#if PM_DEBUG
#endif // PM_DEBUG

};  // class FieldDataAccessor

GFL_NAMESPACE_END( PokemonFureai );
GFL_NAMESPACE_END( Field );

#endif // __FIELDPOKEMONFUREAI_H_INCLUDED__
