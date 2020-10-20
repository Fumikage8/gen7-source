/**
 *  GAME FREAK inc.
 *
 *  @file   FieldFishingSpotAccessor.cpp
 *  @brief  釣りスポット配置データアクセサ
 *  @author Masanori Kanamaru
 *  @date   2015.10.22
 */

#include "Field/FieldRo/include/PlacementData/accessor/FieldFishingSpotAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

/**
  *  @brief  コンストラクタ
  */
FieldFishingSpotAccessor::FieldFishingSpotAccessor()
  :pData(NULL)
{
}

/**
  *  @brief  デストラクタ
  */
FieldFishingSpotAccessor::~FieldFishingSpotAccessor()
{}

void FieldFishingSpotAccessor::Initialize( const void *data)
{
  this->pData = static_cast<const DataHeader*>(data);
}

u32 FieldFishingSpotAccessor::GetDataMax() const
{
  if( !(this->pData) ){ return 0; }
  return this->pData->dataAmount;
}

const FieldFishingSpotAccessor::Data* FieldFishingSpotAccessor::GetData(const u32 idx) const
{
  GFL_ASSERT(idx < this->pData->dataAmount);
  return &(this->pData->data[ idx ]);
}

/**
  *  @brief  コリジョン情報取得
  */
const void* FieldFishingSpotAccessor::GetCollisionData( const Data *pData, CollisionType type) const
{
  u32 offset = 0;
  switch(type)
  {
  case Watching:
    offset = pData->watchingAreaOffset;
    break;
  case Action:
    offset = pData->actionAreaOffset;
    break;      
  default :
    GFL_ASSERT_MSG(false, "Invalid Type\n");
  }

  uptr addr = reinterpret_cast<uptr>( this->pData );
  addr = addr + offset;
  return reinterpret_cast<void*>( addr );
}

GFL_NAMESPACE_END( Field );