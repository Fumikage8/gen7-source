/**
* @brief  �t�B�[���h�̉��i1���N���X
* @file   TerrainDistantView.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINDISTANTVIEW_H_INCLUDED__ )
#define __FIELDTERRAINDISTANTVIEW_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>


// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

class TerrainFactory;


/**
* @brief �t�B�[���h�̉��i1���N���X
* @note  �J�����O���邽�߂ɁA���i��C�ӂ̃T�C�Y�ŕ������Ă���
*/
class TerrainDistantView
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainDistantView );

public:

  /** @brief  �������ݒ� */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    u32                           m_DistantViewNo;             // ���i�ԍ�(�G���A���[�J���ԍ�)
    TerrainFactory*               m_pTerrainFactory;           // �n�`�����@�\
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  TerrainDistantView( void );

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~TerrainDistantView( void );

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


private:


  //
  // ����
  //
  u32                           m_DistantViewNo;      // ���i�ԍ�(�G���A���[�J���ԍ�)

  //
  // �I�u�W�F�N�g
  //
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModel;

  //
  // ���\�[�X�f�[�^
  //

  //
  // �O���I�u�W�F�N�g
  //

};  // class TerrainDistantView


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINDISTANTVIEW_H_INCLUDED__