/**
* @brief  �t�B�[���h�̒n�`�u���b�N�N���X
* @file   TerrainBlock.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINBLOCK_H_INCLUDED__ )
#define __FIELDTERRAINBLOCK_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Collision/include/gfl2_CollisionModel.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>

// field
///#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodType.h"

class BaseCollisionScene;   /// �R���W�����Ǘ��V�[��
class StaticActor;

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
/**
GFL_NAMESPACE_BEGIN(KusaModel);
class FieldKusaModel;
GFL_NAMESPACE_END(KusaModel);
*/ //@saito_del
GFL_NAMESPACE_BEGIN( Terrain );

class TerrainFactory;
/**
GFL_NAMESPACE_BEGIN( Block );
class LodChangeStaticListener;   // �n�`�u���b�N��LOD�ύX�ʒm���󂯎��I�u�W�F�N�g�̊��N���X
GFL_NAMESPACE_END( Block );
*/ //@saito_del

/**
* @brief �t�B�[���h�̒n�`�Ǘ�
*/
class TerrainBlock
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainBlock );

public:

  struct LodLevel
  {
    enum Value
    {
      HIGH = 0,
      MIDDLE,
      MAX
    };
  };

  /** @brief  �������ݒ� */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::math::Vector3           m_Pos;
    u32                           m_X;
    u32                           m_Z;
    f32                           m_Size;
    u32                           m_LodChangeStaticListenerMax;

    TerrainFactory*               pTerrainFactory;             // �n�`�����@�\
    BaseCollisionScene*           pCollisionScene;             // �����R���W�����Ǘ��V�[��
    BaseCollisionScene*           m_pWallCollisionScene;         // �ǃR���W�����Ǘ��V�[��
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  TerrainBlock( void );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~TerrainBlock( void );

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

  /**
  * @brief �X�V����
  *
  * @return ����
  */
  void Update( void );

  /**
  * @brief LOD�ύX����
  *
  * @param  lodLevel �ύX���������
  *
  * @return ����
  */
  ///void ChangeLod( Block::LodType::Value lodLevel );

  /**
  * @brief LOD�ύX�ʒm���󂯎�郊�X�i�[��o�^
  * @note  ���X�i�[�̎����͓o�^���ŊǗ����Ă�������
  *
  * @param  pListener  �o�^���郊�X�i�[
  *
  * @return ����
  */
  ///b32 RegistListener( Block::LodChangeStaticListener* pListener );

  /**
  * @brief LOD�ύX�ʒm���󂯎�郊�X�i�[��o�^�폜
  * @note  ���X�i�[�̎����͓o�^���ŊǗ����Ă�������
  *
  * @param  pListener  �o�^�폜���郊�X�i�[
  *
  * @return ����
  */
  ///b32 UnRegistListener( Block::LodChangeStaticListener* pListener );

private:

  /**
  * @brief LOD�ύX�����X�i�[�ɒʒm
  *
  * @param  lodType �ύX��̏��
  *
  * @return ����
  */
  ///void ChangeLodNotificationListener( Block::LodType::Value lodType );

  /**
  * @brief �n�`���f���̕\���E��\���؂�ւ�
  *
  * @param lodType  �ΏۂƂȂ�LOD���f��
  * @param visible  true �\�� false ��\��
  *
  * @return ����
  */
  ///void UpdateVisibleImpl( Block::LodType::Value lodType, b32 visible );

  //
  // ����
  //
  u32                           m_X;
  u32                           m_Z;
  f32                           m_Size;               // �u���b�N�̑傫��(�����`)
  gfl2::math::Vector3           m_Pos;                // �u���b�N�̒��S���W
  b32                           m_IsLoading;
  ///b32                           m_IsHighModel;

  ///Block::LodType::Value         m_RequestLodType;    // �o���������f��
  ///Block::LodType::Value         m_LodType;           // ���ۂɕ\�����Ă��郂�f��


  u8                            m_InvisibleBit;      // 0�Ȃ�\���A����ȊO�Ȃ��\��

  //
  // �I�u�W�F�N�g
  //
  // gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelNodeTable[ Block::LodType::MAX ];
  poke_3d::model::BaseModel*                                      m_pModelTable;//[ Block::LodType::MAX ];@saito_change
  ///KusaModel::FieldKusaModel**                                     m_ppEncountGrassModelTable[ Block::LodType::MAX ];
  gfl2::collision::CollisionModel*                                m_pCollisionModel;
  StaticActor*                                                    m_pStaticActor;
  gfl2::collision::CollisionModel*                                m_pWallCollisionModel;
  StaticActor*                                                    m_pWallStaticActor;
  ///Block::LodChangeStaticListener**                                m_ppLodChangeStaticListenerTable;
  u32                                                             m_LodChangeStaticListenerMax;

  //
  // ���\�[�X�f�[�^
  //

  //
  // �O���I�u�W�F�N�g
  //
  TerrainFactory*               m_pTerrainFactory;             // �n�`�����@
  BaseCollisionScene*           m_pCollisionScene;             // �R���W�����Ǘ��V�[��
  BaseCollisionScene*           m_pWallCollisionScene;         // �ǃR���W�����Ǘ��V�[��

};  // class TerrainBlock


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINBLOCK_H_INCLUDED__