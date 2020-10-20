//======================================================================
/**
 * @file FieldGimmickEncountDataAccessor.cpp
 * @date 2015/08/20 12:30:34
 * @author saita_kazuki
 * @brief ギミックエンカウントバイナリデータアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"

#include "debug/include/gfl2_Assert.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @brief コンストラクタ
 */
DataAccessor::DataAccessor()
  : m_pAllPackData( NULL)
  , m_pHeaderData( NULL)
  , m_pStandbyData( NULL)
  , m_pChaseData( NULL)
  , m_pPatrolData( NULL)
  , m_pEscapeData( NULL)
  , m_pPokeData( NULL)
{
}

DataAccessor::DataAccessor( void* pAllPackData)
  : m_pAllPackData( pAllPackData)
  , m_pHeaderData( NULL)
{
  this->initializeAccessor( pAllPackData);
}

/**
 * @brief デストラクタ
 */
DataAccessor::~DataAccessor()
{
}

/**
 * @brief パックデータセット
 */
void DataAccessor::SetData( void* pAllPackData)
{
  this->initializeAccessor( pAllPackData);
}

/**
 * @brief パックデータ取得
 * @param presetID プリセットID (テーブルID)
 * @return 指定したデータバイナリ。FieldGimmickEncountData.hに定義してあるフォーマットに入れて使う
 * @note 不正な指定をした場合はNULL
 */
void* DataAccessor::GetData( u32 presetID) const
{
  GFL_ASSERT( m_pAllPackData);

  u32 type = this->GetGimmickType( presetID);
  GFL_ASSERT( type != GIMMICK_TYPE_INVALID);

  u32 index = presetID - m_pHeaderData->presetIDOffset[type].startIndex;

  switch( type)
  {
  case GIMMICK_TYPE_STANDBY:
    GFL_ASSERT( index < m_pHeaderData->standbyNum);
    return &m_pStandbyData[index];
  case GIMMICK_TYPE_CHASE:
    GFL_ASSERT( index < m_pHeaderData->chaseNum);
    return &m_pChaseData[index];
  case GIMMICK_TYPE_PATROL:
    GFL_ASSERT( index < m_pHeaderData->patrolNum);
    return &m_pPatrolData[index];
  case GIMMICK_TYPE_ESCAPE:
    GFL_ASSERT( index < m_pHeaderData->escapeNum);
    return &m_pEscapeData[index];
  }

  GFL_ASSERT( 0);
  return NULL;
}

/**
 * @brief ポケモンデータ取得
 * @param monsNo モンスターNo
 * @return 指定したMonsNoのデータ
 * @note 見つからない、不正なID指定をした場合はNULL
 */
const PokeData* DataAccessor::GetPokeData( u16 monsNo) const
{
  GFL_ASSERT( m_pAllPackData);

  for( u32 i = 0; i < m_pHeaderData->pokeNum; ++i)
  {
    if( monsNo == m_pPokeData[i].monsNo)
    {
      return &m_pPokeData[i];
    }
  }
  return NULL;
}

/**
 * @brief ギミックエンカウントの種類(型)取得
 * @param presetID プリセットID (テーブルID)
 * @return ギミックエンカウントの種類(型)取得
 */
GimmickType DataAccessor::GetGimmickType( u32 presetID) const
{
  GFL_ASSERT( m_pAllPackData);

  for( u32 i = 0; i < GIMMICK_ENCOUNT_TABLE_NUM; ++i)
  {
    if( presetID >= m_pHeaderData->presetIDOffset[i].startIndex &&
        presetID <= m_pHeaderData->presetIDOffset[i].endIndex
      )
    {
      return static_cast<GimmickType>( i);
    }
  }
  GFL_ASSERT( 0);
  return GIMMICK_TYPE_INVALID;
}

/**
 * @brief ギミックエンカウント用シンボルエンカウントIDオフセット値の取得
 * @return ギミックエンカウント用シンボルエンカウントIDオフセット値
 * @note ここで返ってくる値以上がエンカウントIDに設定されていたら、そのエンカウントIDはシンボルエンカウントID
 */
u32 DataAccessor::GetSymbolEncountIDOffset() const
{
  GFL_ASSERT( m_pHeaderData);

  return m_pHeaderData->symbolEncountIDOfset;
}

/**
 * @brief アクセサー初期化
 * @param pAllPackData 全てが含まれたパックデータ
 */
void DataAccessor::initializeAccessor( void* pAllPackData)
{
  m_pAllPackData = pAllPackData;

  gfl2::fs::BinLinkerAccessor accessor( m_pAllPackData);

  m_pHeaderData   = reinterpret_cast<HeaderData*> ( accessor.GetData( DATA_INDEX_HEADER));
  m_pStandbyData  = reinterpret_cast<StandbyData*>( accessor.GetData( DATA_INDEX_STANDBY));
  m_pChaseData    = reinterpret_cast<ChaseData*>  ( accessor.GetData( DATA_INDEX_CHASE));
  m_pPatrolData   = reinterpret_cast<PatrolData*> ( accessor.GetData( DATA_INDEX_PATROL));
  m_pEscapeData   = reinterpret_cast<EscapeData*> ( accessor.GetData( DATA_INDEX_ESCAPE));
  m_pPokeData     = reinterpret_cast<PokeData*>   ( accessor.GetData( DATA_INDEX_POKE));
}

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )
