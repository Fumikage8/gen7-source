/**
* @brief  フィールド地形の配置設定データ
* @file   FieldTerrainLayoutData.cpp
* @author ikuta_junya
* @data   2015.04.19
*/

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/EventWork.h"

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );


u32 TerrainLayoutData::GetHighModelDataIndex( u32 x, u32 z, u32 replaceNo ) const 
{
  if( x >= m_BlockNumOfXAxis || z >= m_BlockNumOfZAxis )
  {
    return DATA_ID_INVALID_VALUE;
  }

  if( replaceNo > 0 )
  {
    // リプレイスデータを検索
    BLOCK_REPLACE_DATA replaceData;
    if( this->SearchReplaceData( x, z, replaceNo, &replaceData ) )
    {
      return replaceData.highModelDataId;
    }
    else
    {
      GFL_ASSERT( 0 );    // 開発中に気づくためのアサート
    }
  }

  return m_ppBlockDataTable[z][x].highModelDataId;
}

/**
* @brief 指定ブロック番号のMiddleモデルのデータ番号を取得する
*
* @param  x      x軸番号
* @param  z      z軸番号
*
* @return 無し
*/
u32 TerrainLayoutData::GetMiddleModelDataIndex( u32 x, u32 z, u32 replaceNo ) const
{
  if( x >= m_BlockNumOfXAxis || z >= m_BlockNumOfZAxis )
  {
    return DATA_ID_INVALID_VALUE;
  }

  if( replaceNo > 0 )
  {
    // リプレイスデータを検索
    BLOCK_REPLACE_DATA replaceData;
    if( this->SearchReplaceData( x, z, replaceNo, &replaceData ) )
    {
      return replaceData.middleModelDataId;
    }
    else
    {
      GFL_ASSERT( 0 );    // 開発中に気づくためのアサート
    }
  }

  return m_ppBlockDataTable[z][x].middleModelDataId;
}

void TerrainLayoutData::Create( const void* pData, u32 dataSize, gfl2::heap::HeapBase* pHeap )
{
  m_pData = static_cast<const BIN_DATA_HEADER*>( pData );
  m_BlockNumOfXAxis = m_pData->blockNumOfXAxis;
  m_BlockNumOfZAxis = m_pData->blockNumOfZAxis;
  m_BlockSize = m_pData->blockSize;

  // アクセスしやすいようにポインタをテーブルで保持する [z][x]
  const BLOCK_DATA* pBlockData = static_cast<const BLOCK_DATA*>( &( m_pData->blockData ) );
  m_ppBlockDataTable = GFL_NEW_ARRAY( pHeap ) const BLOCK_DATA* [ m_BlockNumOfZAxis ];
  for( u32 z = 0; z < m_BlockNumOfZAxis; z++ )
  {
    m_ppBlockDataTable[z] = &pBlockData[ m_BlockNumOfXAxis * z ];
  }

  // リプレイスデータの開始位置を記憶
  m_pReplaceDataStart = reinterpret_cast< const BLOCK_REPLACE_DATA* >(
    reinterpret_cast< uptr >( &m_pData->blockData ) + sizeof( BLOCK_DATA ) * m_BlockNumOfXAxis * m_BlockNumOfZAxis
    );

  // @fix ctr_conv[11727]:範囲外アクセス警告、ケアとしてサイズチェックを入れておく
  uptr bin_data_end_addr = reinterpret_cast< uptr >( pData ) + dataSize;
  uptr data_end_addr = reinterpret_cast< uptr >( &m_pData->blockData ) + sizeof( BLOCK_DATA ) * m_BlockNumOfXAxis * m_BlockNumOfZAxis + sizeof( BLOCK_REPLACE_DATA ) * m_pData->replaceDataNum;
  GFL_ASSERT_MSG( data_end_addr <= bin_data_end_addr, "data_end_addr(%d) bin_data_end_addr(%d)", data_end_addr, bin_data_end_addr );

  Dump();
}


u16 TerrainLayoutData::GetReplaceNo( s32 x, s32 z ) const
{
  if( IsExist( x, z ) == false )
  {
    return 0;
  }

  const BLOCK_DATA* pData = &m_ppBlockDataTable[z][x];

  // SYS_WORKが指定されていないならリプレイス対象ではない
  if( pData->replaceWork == WORK_NULL )
  {
    return 0;
  }

  const EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  const u16 * pWk = pEventWork->GetEventWorkAdrs( pData->replaceWork );

  if( pWk )
  {
    return *pWk;
  }
  else
  {
    return 0;
  }
}


b32 TerrainLayoutData::SearchReplaceData( s32 x, s32 z, u16 replaceNo, BLOCK_REPLACE_DATA* pOut ) const
{
  if( m_pData->replaceDataNum == 0 )
  {
    return false;
  }

  const BLOCK_REPLACE_DATA* pTarget = m_pReplaceDataStart;
  for( u32 i = 0; i < m_pData->replaceDataNum; i++, pTarget++ )
  {
    if( pTarget->x == x && pTarget->z == z && pTarget->replaceNo == replaceNo )
    {
      *pOut = *pTarget;
      return true;
    }
  }

  return false;
}


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );