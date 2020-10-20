//===================================================================
/**
 * @file    SkyboxSky.cpp
 * @brief   天球空クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#include "./SkyboxSky.h"
#include <model/include/gfl2_BaseModel.h>

// debug
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief コンストラクタ
 */
Sky::Sky( gfl2::heap::HeapBase* pParent, void* pResourceData) :
  PartsBase( pParent, 0x3500, "Sky", MODEL_TYPE_MAX, ANIMATION_TYPE_MAX)
{
  this->Initialize( pResourceData);
}

/**
 * @brief デストラクタ
 */
Sky::~Sky()
{
  this->Terminate();
}

/**
 * @brief 初期化
 */
void Sky::Initialize( void* pResourceData)
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

  m_ppModel[MODEL_TYPE_NORMAL] = this->CreateModel( m_ppModelResourceRootNode[ANIMATION_TYPE_ALLTIME]);
  this->AttachAnimation( m_ppModel[MODEL_TYPE_NORMAL], m_ppMotionResourceRootNode[ANIMATION_TYPE_ALLTIME], false, ANIMATION_INDEX_ALLTIME, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);

  this->SetEnable(true);

#if defined( DEBUG_ONLY_FOR_saita_kazuki )
  m_pHeap->PrintHeapInfo( "Skybox Sky");
#endif
}

/**
 * @brief 破棄
 */
void Sky::Terminate()
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
void Sky::Update( const gfl2::math::Vector3& pos)
{
#if 0
  if( m_isEnable == false)
  {
    return;
  }
#endif
  if( m_isUpdateAnimation)
  {
    m_ppModel[MODEL_TYPE_NORMAL]->SetAnimationFrame( m_allTimeAnimationFrame, ANIMATION_INDEX_ALLTIME, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);

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
  }
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )
