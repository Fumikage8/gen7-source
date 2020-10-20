/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBerryPointAccessor.cpp
 *  @brief  きのみ配置データアクセサ
 *  @author Masanori Kanamaru
 *  @date   2015.10.07
 */

#include "Field/FieldRo/include/PlacementData/accessor/FieldBerryPointAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

void FieldBerryPointAccessor::Initialize( const void *data )
{
  m_pData = static_cast<const BerryPointDataHeader*>(data);
}

u32 FieldBerryPointAccessor::GetDataMax() const
{
  if( !m_pData ){ return 0; }
  return m_pData->dataAmount;
}

const FieldBerryPointAccessor::BerryPointData* FieldBerryPointAccessor::GetData(const u32 idx) const
{
  GFL_ASSERT(idx < m_pData->dataAmount);
  return &m_pData->data[ idx ];
}

GFL_NAMESPACE_END( Field );