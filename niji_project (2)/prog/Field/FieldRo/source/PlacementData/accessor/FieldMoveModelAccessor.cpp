//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelAccessor.cpp
 *	@brief  動作モデル配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"
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
FieldMoveModelAccessor::FieldMoveModelAccessor() : 
  m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldMoveModelAccessor::~FieldMoveModelAccessor()
{
}

/**
 *  @brief  初期化
 */
void FieldMoveModelAccessor::Initialize( const void * data )
{
  m_pData = static_cast<const MoveModelDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldMoveModelAccessor::GetDataMax() const
{
  return m_pData->num;
}


/**
 *  @brief エイリアス　データ数
 */
u32 FieldMoveModelAccessor::GetAliasDataMax() const
{
  u32 count = 0;

  for( int i=0; i<m_pData->num; ++i )
  {
    if( m_pData->data[i].alias )
    {
      count ++;
    }
  }

  return count;
}

/**
 *  @brief エイリアス以外のデータ数
 */
u32 FieldMoveModelAccessor::GetNotAliasDataMax() const
{
  u32 alias = GetAliasDataMax();

  return GetDataMax() - alias;
}

/**
 *  @brief 情報取得
 */
const FieldMoveModelAccessor::MoveModelData * FieldMoveModelAccessor::GetData( u32 idx ) const
{
  if( m_pData->num > idx ){
    return &m_pData->data[ idx ];
  }

  return NULL;
}

/**
 *  @brief 話しかけコリジョン情報の取得
 */
const void * FieldMoveModelAccessor::GetTalkCollisionData( const FieldMoveModelAccessor::MoveModelData & data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.talkCollisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
const void * FieldMoveModelAccessor::GetCollisionData( const FieldMoveModelAccessor::MoveModelData & data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}


/**
 *  @brief デフォルトのアイドルモーション取得
 */
u32 FieldMoveModelAccessor::GetDefaultWaitMotionId( const EventWork& eventWork, const DefaultWaitAnimeData& animeData ) const
{
  // まずは条件つきのデフォルトモーションをチェック
  for( u32 i=0; i<DEFAULT_WAIT_ANIME_IF_NUM; ++i )
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
