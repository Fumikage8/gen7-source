//======================================================================
/**
 * @file    FinderStudioDecorationDecoItemManager.cpp
 * @date    2017/02/03 11:27:48
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：デコ管理クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationDecoItemManager.h"

#include <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   コンストラクタ
*/
DecoItemManager::DecoItemManager( void )
  : m_FrameID(Static::FRAME_NULL)
  , m_DecoCount( 0 )
{
  // デコデータの初期化
  _ResetDecorationData();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    追加・削除
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   追加
*/
void DecoItemManager::AddDeco( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  if( IsCanNotAdd() )
  {
    GFL_PRINT( "DecoItem Buffer Over\n" );
    GFL_ASSERT( 0 );
    return;
  }

  // スタンプデータのセット
  Static::StampData data = _SetStampData( type, decoIndex, pos );
  m_DecoData.deco_param[m_DecoCount] = data;

  m_DecoCount++;
}

/**
* @brief   削除
*
* @param   index   削除対象のインデックス
*/
void DecoItemManager::RemoveDeco( u32 index )
{
  GFL_ASSERT( index < ADD_DECO_MAX );

  // データのリセット
  _ResetStampData( index );
  // データを詰める
  _StuffingStampData( index );

  m_DecoCount--;
}

/**
* @brief   フレームのセット
*/
void DecoItemManager::SetFrame( u32 frameIndex )
{
  m_FrameID = frameIndex;

  for( u8 i = 0; i < Static::DECO_ITEM_MAX; ++i )
  {
    m_DecoData.deco_param[i].frame_id = frameIndex;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    データセット
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   データセット：デコの表示位置
 *
 * @param   index
 * @param   pos        デコの位置(スクリーン座標)
 */
void DecoItemManager::SetDecoPos( u32 index, gfl2::math::VEC3 pos )
{
  Static::StampData* data = &m_DecoData.deco_param[index];

  data->x = static_cast<u32>( pos.x );
  data->y = static_cast<u32>( pos.y );

  FUKUSHIMA_PRINT( "DecoItemManager::SetDecoPos : \n" );
  FUKUSHIMA_PRINT( " index=[%d]\n", index );
  FUKUSHIMA_PRINT( " pos x=[%d] y=[%d]\n", data->x, data->y );

  GFL_ASSERT( data->x <= 400 );
  GFL_ASSERT( data->y <= 240 );
}

/**
 * @brief   データセット：スタンプデータ
 *
 * @param   type
 * @param   decoIndex
 * @param   pos        デコの位置(スクリーン座標)
 *
 * @retval  スタンプデータ
 */
Static::StampData DecoItemManager::_SetStampData( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  Static::StampData data = Static::STAMP_NULL_DATA;

  // スクリーン座標に変換
  data.id       = Static::DecoDataAccessor::GetDecoDataListIndex( type, decoIndex );
  data.x        = static_cast<u32>( pos.x );
  data.y        = static_cast<u32>( pos.y );
  data.frame_id = m_FrameID;

  FUKUSHIMA_PRINT( "DecoItemManager::SetDecoPos : \n" );
  FUKUSHIMA_PRINT( " index=[%d]\n", decoIndex );
  FUKUSHIMA_PRINT( " pos(screen) x=[%d] y=[%d]\n", data.x, data.y );

  GFL_ASSERT( data.x <= 400 );
  GFL_ASSERT( data.y <= 240 );

  return data;
}


/**
 * @brief   デコデータのリセット
 */
void DecoItemManager::_ResetDecorationData( void )
{
  gfl2::std::MemClear( &m_DecoData, sizeof( Static::DecorationData ) );

  for( u8 i = 0; i < Static::DECO_ITEM_MAX; ++i )
  {
    _ResetStampData( &m_DecoData.deco_param[i] );
  }
}

/**
 * @brief   スタンプデータのリセット
 */
void DecoItemManager::_ResetStampData( u32 index )
{
  GFL_ASSERT( index < ADD_DECO_MAX );

  _ResetStampData( &m_DecoData.deco_param[index] );
}

/**
 * @brief   スタンプデータのリセット
 */
void DecoItemManager::_ResetStampData( Static::StampData* data )
{
  *data = Static::STAMP_NULL_DATA;
}


/**
* @brief   データを詰める
*
* @param   startIndex    開始インデックス
*/
void DecoItemManager::_StuffingStampData( u32 startIndex )
{
  for( u32 i = startIndex; i < ADD_DECO_MAX - 1; ++i )
  {
    m_DecoData.deco_param[i] = m_DecoData.deco_param[i + 1];
  }

  _ResetStampData( ADD_DECO_MAX - 1 );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    取得
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   スタンプデータの取得
*/
Static::StampData DecoItemManager::GetStampData( u8 index )
{
  return m_DecoData.deco_param[index];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   チェック：追加が出来るか？
*
* @retval  "true"なら追加可能
* @retval  "false"なら追加不可
*/
bool DecoItemManager::IsCanNotAdd( void ) const
{
  return ( m_DecoCount == ADD_DECO_MAX );
}

/**
 * @brief   デコレーションがされているかどうか
 *
 * @retval  "true"ならデコレーション済み
 * @retval  "false"ならデコレーションされていない
 */
bool DecoItemManager::IsDecoration( void ) const
{
  return ( m_DecoCount > 0 ) || (m_FrameID != Static::FRAME_NULL);
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
