//======================================================================
/**
* @file FieldContactPokemonAccessor.cpp
* @date 2017/02/09 18:27:27
* @author kawazoe_shinichi
* @brief ふれあいポケモン用のアクセサー
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "Field/FieldRo/include/PlacementData/accessor/FieldContactPokemonAccessor.h"
#include "Savedata/include/EventWork.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 *  @biref  コンストラクタ
 */
FieldContactPokemonAccessor::FieldContactPokemonAccessor() :
  m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldContactPokemonAccessor::~FieldContactPokemonAccessor()
{
}

/**
 *  @brief  初期化
 */
void FieldContactPokemonAccessor::Initialize(const void * data)
{
  m_pData = static_cast<const ContactPokemonDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldContactPokemonAccessor::GetDataMax() const
{
  return m_pData->num;
}


/**
 *  @brief エイリアス　データ数
 */
u32 FieldContactPokemonAccessor::GetAliasDataMax() const
{
  u32 count = 0;

  for( int i=0; i<m_pData->num; ++i )
  {
    if( m_pData->data[i].moveModel.alias )
    {
      count ++;
    }
  }

  return count;
}

/**
 *  @brief エイリアス以外のデータ数
 */
u32 FieldContactPokemonAccessor::GetNotAliasDataMax() const
{
  u32 alias = GetAliasDataMax();

  return GetDataMax() - alias;
}

/**
*  @brief 動作モデルのみの情報を取得
*/
const FieldMoveModelAccessor::MoveModelData * FieldContactPokemonAccessor::GetData(u32 idx) const
{
  if (m_pData->num > idx){
    return &m_pData->data[idx].moveModel;
  }

  return NULL;
}

/**
*  @brief ふれあい情報のみを取得
*/
const FieldContactPokemonAccessor::ContactPokemonData * FieldContactPokemonAccessor::GetDataForContact(u32 idx) const
{
  if (m_pData->num > idx){
    return &m_pData->data[idx].contactPokemon;
  }

  return NULL;
}

/**
 *  @brief 話しかけコリジョン情報の取得
 */
const void * FieldContactPokemonAccessor::GetTalkCollisionData(const FieldMoveModelAccessor::MoveModelData & data) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.talkCollisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
const void * FieldContactPokemonAccessor::GetCollisionData(const FieldMoveModelAccessor::MoveModelData & data) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
*  @brief 移動領域用コリジョン情報の取得
*/
const void * FieldContactPokemonAccessor::GetMoveAreaCollisionData(const FieldContactPokemonAccessor::ContactPokemonData & data) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.moveCollisionOffset;
  return reinterpret_cast<void*>(addr);
}

/**
*  @brief 予備領域用コリジョン情報の取得
*/
const void * FieldContactPokemonAccessor::GetReserveAreaCollisionData(const FieldContactPokemonAccessor::ContactPokemonData & data) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.reserveCollisionOffset;
  return reinterpret_cast<void*>(addr);
}


/**
 *  @brief デフォルトのアイドルモーション取得
 */
u32 FieldContactPokemonAccessor::GetDefaultWaitMotionId(const EventWork& eventWork, const DefaultWaitAnimeData& animeData) const
{
  // まずは条件つきのデフォルトモーションをチェック
  for (u32 i = 0; i<FieldMoveModelAccessor::DEFAULT_WAIT_ANIME_IF_NUM; ++i)
  {
    // 有効データ以上のインデックスになったら終了
    if( i >= animeData.defaultMotionIDNum ){ break; }

    // フラグワークのチェック
    if( EventWork::CheckIDFlagRenge( animeData.anime_if[i].flagwork ) )
    {
      if( animeData.anime_if[i].flagwork != 0 )
      {
        if( eventWork.CheckEventFlag( animeData.anime_if[i].flagwork ) == false )
        { 
          return animeData.anime_if[i].motionId;
        }
      }
      else
      {
        GFL_ASSERT(animeData.anime_if[i].flagwork != 0);
      }
    }
    else
    {
      const u16 * pwork = eventWork.GetEventWorkAdrs( animeData.anime_if[i].flagwork );
      if(pwork)
      {
        if( (*pwork) == animeData.anime_if[i].flagwork_num )
        {
          return animeData.anime_if[i].motionId;
        }
      }
      else
      {
        GFL_ASSERT( pwork );
      }
    }
  }


  return animeData.defaultMotionID;
}



GFL_NAMESPACE_END(Field);
