//===================================================================
/**
 * @file    StaticModelResourceNodeManager.cpp
 * @brief   配置モデルリソースノード管理クラス
 * @author  saita_kazuki
 * @date    2016.01.26
 */
//===================================================================

#include "Field/FieldRo/include/StaticModel/StaticModelResourceNodeManager.h"
#include "./StaticModelPackAccessor.h"

#include "Field/FieldRo/include/FieldRootNode.h" // ノードの登録のため

#include "System/include/nijiAllocator.h"

// gfl2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @brief   コンストラクタ
 */
StaticModelResourceNodeManager::StaticModelResourceNodeManager() :
  m_pHeap( NULL),
  m_pAllocator( NULL),
  m_pFieldRootNode( NULL),
  m_pResourceWork( NULL),
  m_resorceWorkNum( 0)
{
}

/**
 * @brief   デストラクタ
 */
StaticModelResourceNodeManager::~StaticModelResourceNodeManager()
{
}

/**
 * @brief 初期化
 * @param desc 初期化情報
 */
void StaticModelResourceNodeManager::Initialize( const InitializeDescription& desc)
{
  m_pHeap             = desc.pHeap;
  m_pAllocator        = desc.pAllocator;
  m_pFieldRootNode    = desc.pFieldRootNode;
  m_resorceWorkNum    = desc.areaResourceNum;

  m_pResourceWork = GFL_NEW_ARRAY( m_pHeap) ResourceWork[ m_resorceWorkNum ];
}

/**
 * @brief 破棄
 */
void StaticModelResourceNodeManager::Terminate()
{
  GFL_SAFE_DELETE_ARRAY( m_pResourceWork);

  m_resorceWorkNum  = 0;
  m_pFieldRootNode  = NULL;
  m_pAllocator      = NULL;
  m_pHeap           = NULL;
}

/**
 * @brief リソースノード生成
 * @param modelID 配置モデルID
 * @param pStaticModelPackBinary 配置モデルパックバイナリ
 */
s32 StaticModelResourceNodeManager::Create( u32 modelID, void* pStaticModelPackBinary)
{
  s32 index;
  index = this->GetResourceWorkIndex( modelID);
  if( index == WORK_INDEX_ERROR)
  {
    index = this->GetEmptyResourceWorkIndex();
  }
  m_pResourceWork[index].Create( m_pAllocator, m_pFieldRootNode, pStaticModelPackBinary);
  return index;
}

/**
 * @brief リソースノード破棄
 * @param modelID 配置モデルID
 */
void StaticModelResourceNodeManager::Delete( u32 modelID)
{
  s32 index;
  index = this->GetResourceWorkIndex( modelID);
  if( index == WORK_INDEX_ERROR)
  {
    GFL_ASSERT_MSG( 0, "Not create model resource\n");
    return;
  }
  m_pResourceWork[index].Delete( m_pFieldRootNode);
}

/**
 * @brief モデルリソースノード取得
 * @param workIndex ワーク番号。Create関数の戻り値
 * @param modelType モデルのLOD種類
 * @return 指定したモデルのリソースノード
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* StaticModelResourceNodeManager::GetModelResourceNode( s32 workIndex, ModelType modelType) const
{
  if( workIndex == WORK_INDEX_ERROR)
  {
    GFL_ASSERT_MSG( 0, "Invalid work index\n");
    return NULL;
  }
  return m_pResourceWork[workIndex].GetModelResourceNode( modelType);
}

/**
 * @brief モーションリソースノード取得
 * @param workIndex ワーク番号。Create関数の戻り値
 * @param modelType モデルのLOD種類
 * @param animationIndex モーション番号
 * @return 指定したモーションのリソースノード
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* StaticModelResourceNodeManager::GetMotionResourceNode( s32 workIndex, ModelType modelType, AnimationIndex animationIndex) const
{
  if( workIndex == WORK_INDEX_ERROR)
  {
    GFL_ASSERT_MSG( 0, "Invalid work index\n");
    return NULL;
  }
  return m_pResourceWork[workIndex].GetMotionResourceNode( modelType, animationIndex);
}

/**
 * @brief 指定モデルIDのリソースワークを取得
 * @param modelID 配置モデルID
 * @return 指定モデルIDのリソースワークポインタ
 * @note まだ指定モデルIDのリソースがない場合は-1を返す
 */
s32 StaticModelResourceNodeManager::GetResourceWorkIndex( u32 modelID)
{
  for( u32 i = 0; i < m_resorceWorkNum; ++i)
  {
    if( m_pResourceWork[i].IsMatchModelID( modelID))
    {
      return i;
    }
  }
  return WORK_INDEX_ERROR;
}

/**
 * @brief 空のリソースワークを取得
 * @return 空のリソースワークポインタ
 */
s32 StaticModelResourceNodeManager::GetEmptyResourceWorkIndex()
{
  for( u32 i = 0; i < m_resorceWorkNum; ++i)
  {
    if( m_pResourceWork[i].GetReferenceCount() == 0)
    {
      return i;
    }
  }
  GFL_ASSERT_STOP_MSG( 0, "Not found empty resource work\n");
  return WORK_INDEX_ERROR;
}

//*******************************************************************
// ResourceWork
//*******************************************************************

/**
 * @brief コンストラクタ
 */
StaticModelResourceNodeManager::ResourceWork::ResourceWork()
  : m_referenceCount( 0)
  , m_pStaticModelPackBinary( NULL)
{
  gfl2::std::MemClear( m_pModelResourceNode, sizeof( m_pModelResourceNode));
  gfl2::std::MemClear( m_pMotionResourceNode, sizeof( m_pMotionResourceNode));
}

/**
 * @brief デストラクタ
 */
StaticModelResourceNodeManager::ResourceWork::~ResourceWork()
{
}

/**
 * @brief リソースノード生成
 * @param pAllocator アロケーター
 * @param pFieldRootNode フィールドルートノード
 * @param pStaticModelPackBinary 配置モデルパックバイナリ
 */
void StaticModelResourceNodeManager::ResourceWork::Create( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode, void* pStaticModelPackBinary)
{
  if( m_referenceCount == 0)
  {
    m_pStaticModelPackBinary = pStaticModelPackBinary;
    this->CreateAllResourceNode( pAllocator, pFieldRootNode);
  }
  m_referenceCount++;
}

/**
 * @brief リソースノード破棄
 * @param pFieldRootNode フィールドルートノード
 */
void StaticModelResourceNodeManager::ResourceWork::Delete( FieldRootNode* pFieldRootNode)
{
  m_referenceCount--;
  if( m_referenceCount == 0)
  {
    this->DeleteAllResourceNode( pFieldRootNode);
    m_pStaticModelPackBinary = NULL;
  }
}

/**
 * @brief モデルリソースノード取得
 * @param modelType モデルのLOD種類
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* StaticModelResourceNodeManager::ResourceWork::GetModelResourceNode( ModelType modelType) const
{
  return m_pModelResourceNode[ modelType ];
}

/**
 * @brief モーションリソースノード取得
 * @param modelType モデルのLOD種類
 * @param animationIndex モーション番号
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* StaticModelResourceNodeManager::ResourceWork::GetMotionResourceNode( ModelType modelType, AnimationIndex animationIndex) const
{
  return m_pMotionResourceNode[ modelType ][ animationIndex ];
}

/**
 * @brief 参照カウントの取得
 */
u32 StaticModelResourceNodeManager::ResourceWork::GetReferenceCount() const
{
  return m_referenceCount;
}

/**
 * @brief 指定したモデルIDとリソースのモデルIDが合っているか
 */
bool StaticModelResourceNodeManager::ResourceWork::IsMatchModelID( u32 modelID) const
{
  if( m_pStaticModelPackBinary == NULL)
  {
    // 未初期化
    return false;
  }

  StaticModelPackAccessor accessor;
  accessor.SetData( m_pStaticModelPackBinary);

  StaticModelPackAccessor::StaticModelInfo* pStaticModelInfo = reinterpret_cast<StaticModelPackAccessor::StaticModelInfo*>( accessor.GetStaticModelInfo());
  if( pStaticModelInfo->modelID == modelID)
  {
    // IDが合致した
    return true;
  }
  // IDが合致しない
  return false;
}

/**
 * @brief パックに含まれるリソースノード全ての生成
 * @param pAllocator アロケーター
 * @param pFieldRootNode フィールドルートノード
 */
void StaticModelResourceNodeManager::ResourceWork::CreateAllResourceNode( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode)
{
  StaticModelPackAccessor accessor;
  accessor.SetData( m_pStaticModelPackBinary);

  for( u32 cntModelType = 0; cntModelType < MODEL_TYPE_MAX; ++cntModelType)
  {    
    void* pModelData = accessor.GetModelBinary( static_cast<ModelType>( cntModelType));
    if( pModelData == NULL)
    {
      continue;
    }

    this->CreateModelResourceNode( pAllocator, pFieldRootNode, cntModelType, pModelData);

    for( s32 cntAnimation = 0; cntAnimation < ANIMATION_INDEX_MAX; ++cntAnimation)
    {
      void* pMotionData = accessor.GetMotionBinary( static_cast<ModelType>( cntModelType), cntAnimation);
      if( pMotionData == NULL)
      {
        continue;
      }

      this->CreateMotionResourceNode( pAllocator, pFieldRootNode, cntModelType, cntAnimation, pMotionData);
    }
  }
}

/**
 * @brief モデルリソースノードの生成
 * @param pAllocator アロケーター
 * @param pFieldRootNode フィールドルートノード
 * @param modelType モデルのLOD種類
 * @param pModelData モデルバイナリ
 */
void StaticModelResourceNodeManager::ResourceWork::CreateModelResourceNode( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode, u32 modelType, void* pModelData)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelData));
  m_pModelResourceNode[modelType] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData);

  pFieldRootNode->AddResourceChild( m_pModelResourceNode[modelType]);
}

/**
 * @brief モーションリソースノードの生成
 * @param pAllocator アロケーター
 * @param pFieldRootNode フィールドルートノード
 * @param modelType モデルのLOD種類
 * @param animationIndex アニメーション種類
 * @param pMotionData モーションバイナリ
 */
void StaticModelResourceNodeManager::ResourceWork::CreateMotionResourceNode( gfl2::gfx::IGLAllocator* pAllocator, FieldRootNode* pFieldRootNode, u32 modelType, u32 animationIndex, void* pMotionData)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionData));
  m_pMotionResourceNode[modelType][animationIndex] =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData);

  pFieldRootNode->AddResourceChild( m_pMotionResourceNode[modelType][animationIndex]);
}

/**
 * @brief リソースノード全ての破棄
 * @param pFieldRootNode フィールドルートノード
 */
void StaticModelResourceNodeManager::ResourceWork::DeleteAllResourceNode( FieldRootNode* pFieldRootNode)
{
  for( u32 cntModelType = 0; cntModelType < MODEL_TYPE_MAX; ++cntModelType)
  {
    this->DeleteModelResourceNode( pFieldRootNode, cntModelType);

    for( s32 cntAnimation = 0; cntAnimation < ANIMATION_INDEX_MAX; ++cntAnimation)
    {
      this->DeleteMotionResourceNode( pFieldRootNode, cntModelType, cntAnimation);
    }
  }
}

/**
 * @brief モデルリソースノードの破棄
 * @param pFieldRootNode フィールドルートノード
 * @param modelType モデルのLOD種類
 * @param animationIndex アニメーション種類
 */
void StaticModelResourceNodeManager::ResourceWork::DeleteModelResourceNode( FieldRootNode* pFieldRootNode, u32 modelType)
{
  if( m_pModelResourceNode[modelType])
  {
    pFieldRootNode->RemoveResourceChild( m_pModelResourceNode[modelType]);

    GFL_DELETE( m_pModelResourceNode[modelType]);
    m_pModelResourceNode[modelType] = NULL;
  }
}

/**
 * @brief モーションリソースノードの破棄
 * @param pFieldRootNode フィールドルートノード
 * @param modelType モデルのLOD種類
 * @param animationIndex アニメーション種類
 */
void StaticModelResourceNodeManager::ResourceWork::DeleteMotionResourceNode( FieldRootNode* pFieldRootNode, u32 modelType, u32 animationIndex)
{
  if( m_pMotionResourceNode[modelType][animationIndex])
  {
    pFieldRootNode->RemoveResourceChild( m_pMotionResourceNode[modelType][animationIndex]);

    GFL_DELETE( m_pMotionResourceNode[modelType][animationIndex]);
    m_pMotionResourceNode[modelType][animationIndex] = NULL;
  }
}


GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )
