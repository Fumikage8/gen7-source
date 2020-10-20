/**
* @brief  �t�B�[���h�J�����A�J�����G���A�f�[�^�Ǘ�
* @file   FieldCameraAreaDataManager.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__ )
#define __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Easing.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );


struct AreaNo
{
  enum Value
  {
    DEFAULT = 0xFFFF,       // �f�t�H���g�ݒ�f�[�^ 
    INVALID = 0xFFFE,       // �����ݒ�f�[�^ 
  };
};


struct BIN_DATA;

struct DataType
{
  enum Value
  {
    FOLLOW = 0,               // �ʏ�Ǐ]�^
    FOLLOW_TRANS,             // 2�̃p�����[�^�Ԃ�J�ڂ���Ǐ]�^
    HOLD,
    INHERIT,
    PATH,
    MAX,
    INVALID = 255,
  };
};


struct POINT_DATA
{
  gfl2::math::Vector3 offset;
  gfl2::math::Vector3 rotation;
  f32                 fovY;
  f32                 distance;
};


// @todo ��
struct DATA
{
  DataType::Value               dataType;
  u32                           interpolationFrame;
  gfl2::math::Easing::EaseFunc  easeFunc;

  POINT_DATA          pointA;
  POINT_DATA          pointB;
};

/**
* @brief �t�B�[���h�J�����̑���A�J�����G���A�p
*/
class AreaDataManager
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( AreaDataManager );

public:

  /** @brief  �������ݒ� */
  class InitializeDescription
  {
  public:
    void* m_pData;
    // const gfl2::fs::BinLinkerAccessor* m_BinData;
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @param rInitDesc  �������ݒ�
  *
  * @return ����
  */ 
  AreaDataManager( const InitializeDescription& rInitDesc );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~AreaDataManager( void );
  
  /**
  * @brief �G���A�̃f�t�H���g�ݒ���擾
  *
  * @return ����(false�͂��肦�Ȃ�)
  */
  b32 GetDefaultData( DATA* pOut ) const;


  // @todo ��
  b32 GetData( DATA* pOut, s32 areaId, s32 prevAreaId = 1 ) const;


  s32 GetPriority( s32 areaId ) const;

  void Lerp( POINT_DATA* pOut, const POINT_DATA& r1, const POINT_DATA& r2, f32 t ) const;


private:

  //
  // ��������
  //

  POINT_DATA                        m_DefaultData;


  //
  // �O���I�u�W�F�N�g
  //
  BIN_DATA*                         m_pBinData;

};  // class AreaDataManager


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAAREADATAMANAGER_H_INCLUDED__

  