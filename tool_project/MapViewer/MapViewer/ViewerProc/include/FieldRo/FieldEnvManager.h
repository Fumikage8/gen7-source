//===================================================================
/**
 * @file    FieldEnvManager.h
 * @brief   �t�B�[���hEnv�Ǘ��N���X
 * @author  saita_kazuki
 * @date    2015.05.12
 */
//===================================================================

///#if !defined( FIELD_ENV_MANAGER_H_INCLUDED )
///#define FIELD_ENV_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include "System/include/EnvAccessor.h"

// �O���Q��
namespace System {
  class nijiAllocator;
///  namespace Skybox {
///    class Skybox;
///  }
}
namespace Field {
class MyRenderingPipeLine;
}
namespace gfl2 {
  namespace fs {
    class BinLinkerAccessor;
  }
  namespace renderingengine { namespace scenegraph {
    namespace resource {
      class ResourceNode;
    }
    namespace instance {
      class DrawEnvNode;
    }
  }}
}


GFL_NAMESPACE_BEGIN( Field )

/**
 * @class �t�B�[���hEnv�Ǘ��N���X
 */
class EnvManager
{
  /**
   * @brief�R�s�[�R���X�g���N�^�{����֎~
   */
  GFL_FORBID_COPY_AND_ASSIGN( EnvManager );

public:

  /**
   * @brief �R���X�g���N�^
   */
  EnvManager();

  /**
   * @brief �f�X�g���N�^
   */
  virtual ~EnvManager();

  /**
   * @brief ������
   * @param �V���C���X�^���X�̃|�C���^
   * @todo �V��A�X�N���v�g�ȂǕK�v�Ȃ��̂�n��
   */
  void Initialize( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pGLAllocator, MyRenderingPipeLine* pRenderingPipeLine, void* pEnvPackData/*, System::Skybox::Skybox* pSkybox*/);//@todo_saito

  /**
   * @brief �I��
   */
  void Terminate();

  /**
   * @brief �X�V
   * @param �}�b�v�Ǘ����W
   * @param hour ����
   * @param minute ��
   * @param second �b
   */
  void Update( const gfl2::math::Vector3& controlPos, s32 hour, s32 minute, s32 second, gfl2::math::Matrix44& projMatrix,MyRenderingPipeLine* pRenderingPipeLine);

  /**
   * @brief DrawEnvNode�擾
   */
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetDrawEnvNode() const ;

  /**
   * @brief �t�H�O�f�[�^�̓K��
   * @param pModelInstanceNode �t�H�O��K�����������f���C���X�^���X
   */
  void ApplyFogParam(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode);

private:

  /**
   * @brief ���\�[�X�p�b�N�C���f�b�N�X
   */
  enum PackIndex
  {
    PACK_INDEX_ENV,             ///< gfbenv
    PACK_INDEX_MOTION_LIGHT,    ///< ���C�g���[�V����
    PACK_INDEX_MOTION_FOG,      ///< �t�H�O���[�V����
    PACK_INDEX_MOTION_BLOOM,    ///< �u���[�����[�V����
    PACK_INDEX_TEXTURE_BLOOM,   ///< �u���[���p�e�N�X�`�� 
    PACK_INDEX_MAX
  };

  /**
   * @brief �A�j���[�V�����C���f�b�N�X
   */
  enum AnimationIndex
  {
    ANIMATION_INDEX_LIGHT,
    ANIMATION_INDEX_FOG,
    ANIMATION_INDEX_BLOOM,

    ANIMATION_INDEX_MAX,
  };

private:

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* CreateDrawEnvNode( void* pData);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* CreateDrawEnvResourceNode( void* pData);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* CreateDrawEnvMotionResourceNode( void* pData);

private:

  gfl2::heap::HeapBase*       m_pHeap;
  System::nijiAllocator*      m_pGLAllocator;

  System::EnvAccessor         m_EnvAccessor;    ///< DrawEnv���ʏ����A�N�Z�T�[

  ///System::Skybox::Skybox*     m_pSkybox;        ///< �V��

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;                                         ///< Env�m�[�h
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvResourceRootNode;                             ///< Env���\�[�X�m�[�h
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_MAX];  ///< Env���[�V�������\�[�X�m�[�h
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvResourceAlphaMask;

};

GFL_NAMESPACE_END( Field )

///#endif // FIELD_ENV_MANAGER_H_INCLUDED

