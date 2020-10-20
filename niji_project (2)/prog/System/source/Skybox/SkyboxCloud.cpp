//===================================================================
/**
 * @file    SkyboxCloud.cpp
 * @brief   天球雲クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#include "./SkyboxCloud.h"
#include <model/include/gfl2_BaseModel.h>

// debug
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief コンストラクタ
 */
Cloud::Cloud( gfl2::heap::HeapBase* pParent, void* pResourceData, bool isAllocateByVRAM, System::nijiAllocator::VramMallocMode vramMallocMode) :
  PartsBase( pParent, 0x8B00, "Cloud", MODEL_TYPE_MAX, ANIMATION_TYPE_MAX, isAllocateByVRAM, vramMallocMode)
{
  this->Initialize( pResourceData);
}

/**
 * @brief デストラクタ
 */
Cloud::~Cloud()
{
  this->Terminate();
}

/**
 * @brief 初期化
 */
void Cloud::Initialize( void* pResourceData)
{
  ResourceData* pData = reinterpret_cast<ResourceData*>( pResourceData);

  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_ppModelResourceRootNode[i] = this->CreateModelResourceNode( pData->pModelData[i]);
  }

  for( u32 i = 0; i < ANIMATION_TYPE_MAX; ++i)
  {
    m_ppMotionResourceRootNode[i] = this->CreateMotionResourceNode( pData->pMotionData[i]);
  }

  m_ppModel[MODEL_TYPE_NORMAL] = this->CreateModel( m_ppModelResourceRootNode[MODEL_TYPE_NORMAL]);

  this->AttachAnimation( m_ppModel[MODEL_TYPE_NORMAL], m_ppMotionResourceRootNode[ANIMATION_TYPE_ALLTIME_SHADER], false, ANIMATION_INDEX_ALLTIME_SHADER, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  m_ppModel[MODEL_TYPE_NORMAL]->SetAnimationStepFrame( 0.0f, ANIMATION_INDEX_ALLTIME_SHADER);

  this->AttachAnimation( m_ppModel[MODEL_TYPE_NORMAL], m_ppMotionResourceRootNode[ANIMATION_TYPE_UV], true, ANIMATION_INDEX_UV, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);

  this->SetEnable( true);

#if defined( DEBUG_ONLY_FOR_saita_kazuki )
  m_pHeap->PrintHeapInfo( "Skybox Cloud");
#endif
}

/**
 * @brief 破棄
 */
void Cloud::Terminate()
{
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_ppModel[i]->Destroy();
    GFL_SAFE_DELETE( m_ppModel[i]);
  }

  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_ppModelResourceRootNode[i]);
  }

  for( u32 i = 0; i < ANIMATION_TYPE_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_ppMotionResourceRootNode[i]);
  }
}

/**
 * @brief 更新
 */
void Cloud::Update( const gfl2::math::Vector3& pos)
{
  if( m_isEnable == false)
  {
    return;
  }

  if( m_isUpdateAnimation)
  {
    m_ppModel[MODEL_TYPE_NORMAL]->SetAnimationFrame( m_allTimeAnimationFrame, ANIMATION_INDEX_ALLTIME_SHADER, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);

    poke_3d::model::BaseModel::AnimationBit bit = static_cast<poke_3d::model::BaseModel::AnimationBit>( poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL );
    for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
    {
      m_ppModel[i]->SetPosition( pos);
      m_ppModel[i]->UpdateAnimation( bit);
    }

    m_isUpdateAnimation = false;
  }
  else
  {
    for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
    {
      m_ppModel[i]->SetPosition( pos);
    }

    m_ppModel[MODEL_TYPE_NORMAL]->UpdateAnimation( poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  }

}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )
