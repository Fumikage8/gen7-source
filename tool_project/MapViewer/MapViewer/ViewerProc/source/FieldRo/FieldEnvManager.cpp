//===================================================================
/**
 * @file    FieldEnvManager.cpp
 * @brief   �t�B�[���hEnv�Ǘ��N���X
 * @author  saita_kazuki
 * @date    2015.05.12
 */
//===================================================================

#include "ViewerProc/include/FieldRo/FieldEnvManager.h"
#include "ViewerProc/include/FieldRo/MyRenderingPipeLine.h"

#include "System/include/nijiAllocator.h"
///#include "System/include/Skybox/Skybox.h"

#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

#define ENABLE_ENV 1

GFL_NAMESPACE_BEGIN( Field )

/**
 * @brief �R���X�g���N�^
 */
EnvManager::EnvManager()
{
}

/**
 * @brief �f�X�g���N�^
 */
EnvManager::~EnvManager()
{
}

/**
 * @brief ������
 * @param �V���C���X�^���X�̃|�C���^
 */
void EnvManager::Initialize( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pGLAllocator, MyRenderingPipeLine* pRenderingPipeLine, void* pEnvPackData/*, System::Skybox::Skybox* pSkybox*/)//@todo_saito
{
  m_pHeap = pHeap;
  m_pGLAllocator = pGLAllocator;

  ///m_pSkybox = pSkybox;

#if ENABLE_ENV
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( pEnvPackData);

  // EnvNode�쐬
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_ENV));

    gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData resourceData;
    resourceData.SetBinaryEnvData( pBinaryData);

    m_pDrawEnvResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode( m_pGLAllocator, m_pDrawEnvResourceRootNode);
  }

  // Env���[�V�����m�[�h�쐬
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_LIGHT));
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData);

    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_LIGHT] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);
  }

  // Env���[�V�����m�[�h�쐬(FOG)
  if( binLinkerAccessor.GetDataSize(PACK_INDEX_MOTION_FOG) != 0 )
  {
    // �o�C�i���f�[�^���擾
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_FOG) );
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData );

    // ���\�[�X�m�[�h���o�C�i���f�[�^����쐬
    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_FOG] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

    // Fog�A�j���[�V�����J�n
    m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_FOG  ], 0, System::EnvAccessor::ANIMATION_BIT_FOG  );
    m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_FOG );
  }

  // Env���[�V�����m�[�h�쐬(�u���[��)
  if( binLinkerAccessor.GetDataSize(PACK_INDEX_MOTION_BLOOM) != 0 )
  {
    // �o�C�i���f�[�^���擾
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_BLOOM) );
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData );

    // ���\�[�X�m�[�h�����o�C�i���f�[�^����쐬
    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_BLOOM] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

    // �u���[���A�j���[�V�����J�n
    m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_BLOOM], 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
    m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM );
  }

  m_EnvAccessor.Initialize( m_pHeap, m_pDrawEnvNode);
  m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_LIGHT], 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);

  // �u���[���`��p�X�̏�����
  {
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pRenderingPipeLine->GetBloomRenderPath();
    bloomRenderPath->SetDrawEnvNode( m_pDrawEnvNode );

    if( binLinkerAccessor.GetDataSize(PACK_INDEX_TEXTURE_BLOOM) != 0 )
    {
      // ���\�[�X�m�[�h
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
        texData.SetTextureData(static_cast<c8*>(binLinkerAccessor.GetData( PACK_INDEX_TEXTURE_BLOOM)));
        m_pDrawEnvResourceAlphaMask = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(&texData);
      }
      // �u���[���`��p�X�ɓn��
      {
        gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texResNode = poke_3d::renderer::BloomRenderPath::GetTextureResourceNode(m_pDrawEnvResourceAlphaMask);
        bloomRenderPath->SetAlphaMaskTexture(texResNode->GetTexture());
      }
    }
  }

#endif
}

/**
 * @brief �I��
 */
void EnvManager::Terminate()
{
  m_EnvAccessor.Terminate();

  for( u32 i = 0; i < ANIMATION_INDEX_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_pDrawEnvMotionResourceRootNode[i]);
  }
  GFL_SAFE_DELETE( m_pDrawEnvResourceRootNode);
  GFL_SAFE_DELETE( m_pDrawEnvNode );
  GFL_SAFE_DELETE( m_pDrawEnvResourceAlphaMask );
}

/**
 * @brief �X�V
 * @param �}�b�v�Ǘ����W
 * @param hour ����
 * @param minute ��
 * @param second �b
 */
void EnvManager::Update( const gfl2::math::Vector3& controlPos, s32 hour, s32 minute, s32 second, gfl2::math::Matrix44& projMatrix, MyRenderingPipeLine* pRenderingPipeLine)
{
  // �V���X�V
  ///m_pSkybox->Update( controlPos, hour, minute, second);

  // �t�H�O
  {
    // ���͕ύX���Ă��Ȃ��̂�
    //   �EZ�o�b�t�@
    //   �EZ�o�b�t�@�Ȃ̂ŃJ�����̃j�A�N���b�v�����A�t�@�[�N���b�v�����͎g��Ȃ�
    //   �E�[�x�o�b�t�@�͈̔͂̓f�t�H���g����ύX���Ȃ�
    // �ƂȂ��Ă���B

    //W�o�b�t�@�̃e�X�g
    //param.cameraNear     = 1.0f;
    //param.cameraFar      = 1000.0f;
    //param.depthRangeNear = 0.0f;
    //param.depthRangeFar  = 1.0f;
    //param.wScale         = 1.0f/param.cameraFar;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = projMatrix;
    m_pDrawEnvNode->UpdateFog(param);
  }

  // �u���[���`��p�X
  {
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pRenderingPipeLine->GetBloomRenderPath();
    if( bloomRenderPath )
    {
      bloomRenderPath->Update();
    }
  }

#if ENABLE_ENV
  // @todo �X�V�Ԋu�͖�����
  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.Update();
#endif

}

/**
 * @brief DrawEnvNode�擾
 */
gfl2::renderingengine::scenegraph::instance::DrawEnvNode* EnvManager::GetDrawEnvNode() const
{
  return m_pDrawEnvNode;
}

/**
 * @brief �t�H�O�f�[�^�̓K��
 * @param pModelInstanceNode �t�H�O��K�����������f���C���X�^���X
 */
void EnvManager::ApplyFogParam(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode)
{
  u32 materialCount = pModelInstanceNode->GetMaterialNum();
  for(u32 materialIndex=0; materialIndex<materialCount; ++materialIndex)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode*                 materialInstanceNode               = pModelInstanceNode->GetMaterialInstanceNode(materialIndex);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*                 materialResourceNode               = const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*>(materialInstanceNode->GetMaterialResourceNode());
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* materialInstanceNodeAttributeParam = materialInstanceNode->GetAttributeParam();
    materialInstanceNodeAttributeParam->m_FogEnable = true;
  }

  m_pDrawEnvNode->UpdateMaterialFog(pModelInstanceNode);
}

GFL_NAMESPACE_END( Field )

