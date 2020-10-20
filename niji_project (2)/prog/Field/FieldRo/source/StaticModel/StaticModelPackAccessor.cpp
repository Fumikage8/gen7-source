//===================================================================
/**
 * @file    StaticModel.cpp
 * @brief   配置モデルクラス
 * @author  saita_kazuki
 * @date    2015.04.08
 */
//===================================================================

#include "./StaticModelPackAccessor.h"
#include <Fs/include/gfl2_BinLinkerAccessor.h> // パックへのアクセスのため

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @brief コンストラクタ
 */
StaticModelPackAccessor::StaticModelPackAccessor() :
  m_pStaticModelPack( NULL)
{
}

/**
 * @brief デストラクタ
 */
StaticModelPackAccessor::~StaticModelPackAccessor()
{
}

/**
 * @brief 配置モデル1つのパックバイナリをセット
 */
void StaticModel::StaticModelPackAccessor::SetData( void* pData)
{
  m_pStaticModelPack = pData;
}

/**
 * @brief 配置モデル情報バイナリを取得
 * @return 配置モデル情報バイナリ。アニメーション情報などが格納されている
 */
void* StaticModel::StaticModelPackAccessor::GetStaticModelInfo() const
{
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( m_pStaticModelPack);
  return binLinkerAccessor.GetData( PACK_INDEX_INFO);
}

/**
 * @brief モデルリソースバイナリを取得
 * @param modelType どのモデルのリソースか。ハイ、ミドル、ローから選択
 * @return モデルリソースバイナリ
 */
void* StaticModel::StaticModelPackAccessor::GetModelBinary( ModelType modelType) const
{
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( m_pStaticModelPack);

  const u32 MODEL_INDEX[ MODEL_TYPE_MAX ] =
  {
    PACK_INDEX_HIGH_MODEL,
    PACK_INDEX_MIDDLE_MODEL,
    PACK_INDEX_LOW_MODEL,
  };
  void* pData = binLinkerAccessor.GetData( MODEL_INDEX[modelType]);
  u32 size = binLinkerAccessor.GetDataSize( MODEL_INDEX[modelType]);

  if( size != 0)
  {
    return pData;
  }
  return NULL;
}

/**
 * @brief モーションリソースバイナリを取得
 * @param modelType どのモデルのリソースか。ハイ、ミドル、ローから選択
 * @return モーションリソース
 */
void* StaticModel::StaticModelPackAccessor::GetMotionBinary( ModelType modelType, u32 index) const
{
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( m_pStaticModelPack);

  const u32 MOTION_INDEX[ MODEL_TYPE_MAX ] =
  {
    PACK_INDEX_HIGH_ANIMATION_START,
    PACK_INDEX_MIDDLE_ANIMATION_START,
    PACK_INDEX_LOW_ANIMATION_START,
  };
  u32 packIndex = MOTION_INDEX[modelType] + index;

  void* pData = binLinkerAccessor.GetData( packIndex);
  u32 size = binLinkerAccessor.GetDataSize( packIndex);

  if( size != 0)
  {
    return pData;
  }
  return NULL;
}

/**
 * @brief バウンディングボックスアニメーションコリジョンモデル取得
 * @return  バウンディングボックスアニメーションコリジョンモデル。なければNULL
 */
void* StaticModelPackAccessor::GetBBAnimationCollisionModel() const
{
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( m_pStaticModelPack);

  void* pData = binLinkerAccessor.GetData( PACK_INDEX_BB_ANIMATION_COLLISION_MODEL);
  u32 size = binLinkerAccessor.GetDataSize( PACK_INDEX_BB_ANIMATION_COLLISION_MODEL);

  if( size != 0)
  {
    return pData;
  }
  return NULL;
}

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )
