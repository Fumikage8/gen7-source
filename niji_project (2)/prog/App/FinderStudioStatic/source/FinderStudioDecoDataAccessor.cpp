//======================================================================
/**
 * @file    FinderStudioDecoDataAccessor.cpp
 * @date    2017/02/02 11:22:01
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：デコデータアクセサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
/// @brief    デコデータリスト定義ファイルのインクルード
//////////////////////////////////////////////////////////////////////////
#include <niji_conv_header/app/data/finder_studio/finder_studio_deco_data.cdat>



//////////////////////////////////////////////////////////////////////////
///
/// @brief    データ取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   データ取得：デコID
 *
 * @param   index   全デコ中のインデックス
 *
 * @retval  デコID
 */
 u32 DecoDataAccessor::GetDecoID( u32 index )
{
  DecoData data = c_DecoDataList[index];

  return data.id;
}

/**
 * @brief   データ取得：デコID
 *
 * @param   type    デコの種類
 * @param   index   各種類ごとのインデックス
 *
 * @retval  デコID
 */
u32 DecoDataAccessor::GetDecoID( DECO_TYPE type, u32 index )
{
  u32 dataIndex = _GetDecoDataListIndex( type, index );

  return GetDecoID( dataIndex );
}


/**
 * @brief   データ取得：種類ごとのデコ最大数
 *
 * @param   type    デコの種類
 *
 * @retval  最大数
 */
u32 DecoDataAccessor::GetDecoTypeMax( DECO_TYPE type )
{
  return _GetDecoTypeMax( type );
}


/**
 * @brief   データ取得：デコテクスチャの取得
 *
 * @param   index   全デコ中のインデックス
 *
 * @retval  テクスチャID
 */
u32 DecoDataAccessor::GetDecoTexID( u32 index )
{
  DecoData data = c_DecoDataList[index];

  return data.deco_tex;
}

/**
 * @brief   データ取得：デコテクスチャの取得
 *
 * @param   type    デコの種類
 * @param   index   各種類ごとのインデックス
 *
 * @retval  テクスチャID
 */
u32 DecoDataAccessor::GetDecoTexID( DECO_TYPE type, u32 index )
{
  u32 dataIndex = _GetDecoDataListIndex( type, index );
  
  return GetDecoTexID( dataIndex );
}


/**
 * @brief   データ取得：デコサムネイルテクスチャの取得
 *
 * @param   index   全デコ中のインデックス
 *
 * @retval  サムネイルテクスチャID
 */
u32 DecoDataAccessor::GetDecoThumbID( u32 index )
{
  DecoData data = c_DecoDataList[index];

  return data.thumb_tex;
}

/**
 * @brief   データ取得：デコサムネイルテクスチャの取得
 *
 * @param   type    デコの種類
 * @param   index   各種類ごとのインデックス
 *
 * @retval  サムネイルテクスチャID
 */
u32 DecoDataAccessor::GetDecoThumbID( DECO_TYPE type, u32 index )
{
  u32 dataIndex = _GetDecoDataListIndex( type, index );

  return GetDecoThumbID( dataIndex );
}


u32 DecoDataAccessor::GetDecoViewTexID( const u32 index )
{
  return c_DecoDataList[index].decoview_tex;
}


u32 DecoDataAccessor::GetDecoViewFrameTexID( const u32 frameIndex )
{
  return GetDecoViewTexID(_GetDecoDataListIndex(DECO_TYPE_FRAME, frameIndex));
}


/**
 * @brief   開放済みのデコ数を取得
 *
 * @param   デコの種類
 *
 * @retval  開放されているデコの数
 */
u32 DecoDataAccessor::GetUnlockCount( DECO_TYPE type )
{
  // 開放数
  u32 unlockCount = 0;
  // デコの最大数
  u32 decoMax = _GetDecoTypeMax( type );

  for( u32 i = 0; i < decoMax; ++i )
  {
    // 開放チェック
    if( _IsUnlock( type, i ) )
    {
      unlockCount++;
    }
  }

  FUKUSHIMA_PRINT( "GetUnlockCount : count=[%d]\n", unlockCount );

  return unlockCount;
}


/**
 * @brief   データ取得：デコの種類ごとの最大数を取得
 *
 * @param   type    デコの種類
 *
 * @retval  各種デコの最大数
 */
u32 DecoDataAccessor::_GetDecoTypeMax( DECO_TYPE type )
{
  const u32 c_DecoItemMax[DECO_TYPE_NUM] = {
    DECO_STAMP_S_NUM,
    DECO_STAMP_M_NUM,
    DECO_STAMP_L_NUM,
    DECO_STAMP_TEXT_NUM,
    DECO_FRAME_NUM,
  };

  GFL_ASSERT( type < GFL_NELEMS(c_DecoItemMax) );

  return c_DecoItemMax[type];
}

/**
 * @brief   データ取得：データリストインデックスの取得
 *
 * @param   type    デコの種類
 * @param   index   デコのインデックス
 */
u32 DecoDataAccessor::_GetDecoDataListIndex( DECO_TYPE type, u32 index )
{
  u32 offset = 0;

  // インデックスの開始位置を計算
  for( u32 i = 0; i < static_cast<u32>( type ); ++i )
  {
    offset += _GetDecoTypeMax( static_cast<DECO_TYPE>( i ) );
  }

  FUKUSHIMA_PRINT( "_GetDecoDataListIndex : type=[%d] index=[%d] offset[%d]\n", type, index, offset );

  return offset + index;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   チェック：開放
 */
bool DecoDataAccessor::IsUnlock( DECO_TYPE type, u32 index )
{
  return _IsUnlock( type, index );
}

/**
 * @ brief    チェック：開放(実体)
 *
 * @retval    "true"なら開放済み
 * @retval    "false"なら未開放
 */
bool DecoDataAccessor::_IsUnlock( DECO_TYPE type, u32 index )
{
  u32 dataIndex = _GetDecoDataListIndex( type, index );
  DecoData data = c_DecoDataList[dataIndex];

  // イベントワークの取得
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Field::EventWork* eventWork = gameData->GetEventWork();

  // イベントワークのチェック
  return eventWork->CheckEventFlag( data.unlock );
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
