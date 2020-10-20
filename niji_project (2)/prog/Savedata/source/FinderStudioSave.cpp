//======================================================================
/**
 * @file    FinderStudioSave.cpp
 * @date    2017/01/11 19:24:59
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ用セーブデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Savedata/include/FinderStudioSave.h>
#include <Savedata/include/MyStatus.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

// ファインダースタジオUtil
#include "App/FinderStudioStatic/include/FinderStudioUtil.h"

GFL_NAMESPACE_BEGIN( Savedata )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
FinderStudioSave::FinderStudioSave( void )
{
  STATIC_ASSERT(sizeof(App::FinderStudio::Static::IntegratedData)==196);
  STATIC_ASSERT(sizeof(SaveData)==1024);
  FUKUSHIMA_PRINT( "FinderStudioSave : Class Size[%d]\n", sizeof(SaveData) );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    SaveDataInterface実装
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   読み込んだデータをセット
 *
 * @param   data    先頭のポインタ
 */
void FinderStudioSave::SetData( void* data )
{
  gfl2::std::MemCopy( data, &m_Data, sizeof(m_Data) );
}

/**
 * @brief   データの取得
 *
 * @retval  データの先頭ポインタ
 */
void* FinderStudioSave::GetData( void )
{
  return reinterpret_cast<void*>( &m_Data );
}

/**
 * @brief   データサイズの取得
 *
 * @retval  データサイズ(バイト)
 */
size_t FinderStudioSave::GetDataSize( void )
{
  return sizeof( SaveData );
}

/**
 * @brief   セーブデータのクリア
 *
 * @param   heap    テンポラリヒープ
 */
void FinderStudioSave::Clear( gfl2::heap::HeapBase* heap )
{
  GFL_UNUSED( heap );

  /// 背景NEWフラグの初期化
  for( u8 i = 0; i < BACKGROUND_MAX; ++i )
  {
    m_Data.bg_new_flg[i] = NEW_ICON_VISIBLE;
  }

  /// チュートリアルフラグの初期化
  m_Data.tutorial_state = TUTORIAL_STATE_NONE;

  /// 着せ替えデータの初期化
  poke_3d::model::DressUpParam initDressupParam;
  System::DressUp::GetDressUpParamDefaultHeroine( &initDressupParam );
  InitDressupParam( initDressupParam );

  for( u8 i = 0; i < DRESSUP_MAX; ++i )
  {
    m_Data.dressup_new_flg[i] = NEW_ICON_INVISIBLE;
  }
  m_Data.dressup_new_flg[4] = NEW_ICON_VISIBLE;   //!< ライド 陸
  m_Data.dressup_new_flg[5] = NEW_ICON_VISIBLE;   //!< ライド 水
  m_Data.dressup_new_flg[6] = NEW_ICON_VISIBLE;   //!< ライド 宇宙

  /// スタンプNEWフラグの初期化
  gfl2::std::MemFill( m_Data.stamp_s_new_flg, static_cast<u8>( NEW_ICON_VISIBLE ), sizeof( b8 )*STAMP_S_FLG_MAX );
  gfl2::std::MemFill( m_Data.stamp_m_new_flg, static_cast<u8>( NEW_ICON_VISIBLE ), sizeof( b8 )*STAMP_M_FLG_MAX );
  gfl2::std::MemFill( m_Data.stamp_l_new_flg, static_cast<u8>( NEW_ICON_VISIBLE ), sizeof( b8 )*STAMP_L_FLG_MAX );
  gfl2::std::MemFill( m_Data.stamp_char_new_flg, static_cast<u8>( NEW_ICON_VISIBLE ), sizeof( b8 )*STAMP_CHAR_FLG_MAX );
  gfl2::std::MemFill( m_Data.frame_new_flg, static_cast<u8>( NEW_ICON_VISIBLE ), sizeof( b8 )*FRAME_FLG_MAX );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    データ 設定・取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   背景のNEWを非表示にする
 *
 * @param   index   背景インデックス(全開放した時のインデックス)
 */
void FinderStudioSave::InvisibleBackGroundNewFlg( u8 index )
{
  GFL_ASSERT( index < BACKGROUND_MAX );

  m_Data.bg_new_flg[index] = NEW_ICON_INVISIBLE;

  GFL_PRINT( "BackGround NewIcon Invisible : Index[%d]\n", index );
}

/**
 * @brief   背景NEWフラグの取得
 *
 * @param   index   背景インデックス(全開放した時のインデックス)
 *
 * @retval  "true"ならNEWを表示
 * @retval  "false"ならNEWを非表示
 */
bool FinderStudioSave::IsVisibleBackGroundNewFlg( u8 index )
{
  GFL_ASSERT( index < BACKGROUND_MAX );

  return ( m_Data.bg_new_flg[index] == NEW_ICON_VISIBLE );
}

/**
 * @brief   チュートリアルステートのセット
 *
 * @param   state   更新するチュートリアルステート
 */
void FinderStudioSave::SetTutorialState( TutorialState state )
{
  m_Data.tutorial_state |= state;

  GFL_PRINT( "Update Tutorial State : add[%d] total[%d]\n", state, m_Data.tutorial_state );
}

/**
 * @brief   チュートリアルステートのチェック
 *
 * @param   state   チェックするステート
 *
 * @retval  "true"ならチュートリアル確認済み
 * @retval  "false"ならチュートリアル未確認
 */
bool FinderStudioSave::IsTutorialState( TutorialState state )
{
  return ( m_Data.tutorial_state & state ) != 0;
}

/**
 * @brief   着せ替えデータのセット
 *
 * @param   index   格納先のインデックス
 * @param   param   格納するデータ
 */
void FinderStudioSave::SetDressupParam( u8 index, poke_3d::model::DressUpParam& param )
{
  GFL_ASSERT( index < DRESSUP_SAVE_MAX );
  System::DressUpParamSaveData::ConvertFromDressUpParam( &m_Data.dressup_param[index], param );
}

/**
 * @brief   着せ替えデータの取得
 *
 * @param   index   取得先のインデックス
 *
 * @retval  DressUpParam
 */
poke_3d::model::DressUpParam FinderStudioSave::GetDressupParam( u8 index )
{
  GFL_ASSERT( index < DRESSUP_SAVE_MAX );
  poke_3d::model::DressUpParam dressUpParam;
  System::DressUpParamSaveData::ConvertToDressUpParam(&dressUpParam, m_Data.dressup_param[index]);
  return dressUpParam;
}

/**
 * @brief   着せ替えデータの初期化
 *
 * @param   param   着せ替えデータ
 *
 * @note    GameDataのプレイヤ設定時にコールし着せ替えデータを作成する。
 */
void FinderStudioSave::InitDressupParam( poke_3d::model::DressUpParam& param )
{
  for( u8 i = 0; i < DRESSUP_SAVE_MAX; ++i )
  {
    SetDressupParam( i, param );
  }
}

/**
 * @brief   着せ替えNEWを非表示にする
 *
 * @param   index   インデックス
 */
void FinderStudioSave::InvisibleDressupNewFlg( u8 index )
{
  m_Data.dressup_new_flg[index] = NEW_ICON_INVISIBLE;
}

/**
 * @brief   着せ替えNEWの表示フラグを取得
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool FinderStudioSave::IsVisibleDressupNewFlg( u8 index )
{
  return ( m_Data.dressup_new_flg[index] == NEW_ICON_VISIBLE );
}

/**
 * @brief   スタンプ(Ｓ)のNew表示フラグを非表示
 *
 * @param[in]   index   インデックス
 */
void FinderStudioSave::InvisibleStampSNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_S_FLG_MAX );

  m_Data.stamp_s_new_flg[index] = NEW_ICON_INVISIBLE;
}

/**
 * @brief   スタンプ(Ｓ)のNew表示フラグを取得
 *
 * @param[in]   index   インデックス
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool FinderStudioSave::IsVisibleStampSNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_S_FLG_MAX );

  return ( m_Data.stamp_s_new_flg[index] == NEW_ICON_VISIBLE );
}

/**
 * @brief   スタンプ(Ｍ)のNew表示フラグを非表示
 *
 * @param[in]   index   インデックス
 */
void FinderStudioSave::InvisibleStampMNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_M_FLG_MAX );

  m_Data.stamp_m_new_flg[index] = NEW_ICON_INVISIBLE;
}

/**
 * @brief   スタンプ(Ｍ)のNew表示フラグを取得
 *
 * @param[in]   index   インデックス
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool FinderStudioSave::IsVisibleStampMNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_M_FLG_MAX );

  return ( m_Data.stamp_m_new_flg[index] == NEW_ICON_VISIBLE );
}

/**
 * @brief   スタンプ(Ｌ)のNew表示フラグを非表示
 *
 * @param[in]   index   インデックス
 */
void FinderStudioSave::InvisibleStampLNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_L_FLG_MAX );

  m_Data.stamp_l_new_flg[index] = NEW_ICON_INVISIBLE;
}

/**
 * @brief   スタンプ(Ｌ)のNew表示フラグを取得
 *
 * @param[in]   index   インデックス
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool FinderStudioSave::IsVisibleStampLNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_L_FLG_MAX );

  return ( m_Data.stamp_l_new_flg[index] == NEW_ICON_VISIBLE );
}

/**
 * @brief   スタンプ(文字)のNew表示フラグを非表示
 *
 * @param[in]   index   インデックス
 */
void FinderStudioSave::InvisibleStampCharNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_CHAR_FLG_MAX );

  m_Data.stamp_char_new_flg[index] = NEW_ICON_INVISIBLE;
}

/**
 * @brief   スタンプ(文字)のNew表示フラグを取得
 *
 * @param[in]   index   インデックス
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool FinderStudioSave::IsVisibleStampCharNewFlg( u8 index )
{
  GFL_ASSERT( index < STAMP_CHAR_FLG_MAX );

  return ( m_Data.stamp_char_new_flg[index] == NEW_ICON_VISIBLE );
}

/**
 * @brief   フレームのNew表示フラグを非表示
 *
 * @param[in]   index   インデックス
 */
void FinderStudioSave::InvisibleFrameNewFlg( u8 index )
{
  GFL_ASSERT( index < FRAME_FLG_MAX );

  m_Data.frame_new_flg[index] = NEW_ICON_INVISIBLE;
}

/**
 * @brief   フレームのNew表示フラグを取得
 *
 * @param[in]   index   インデックス
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool FinderStudioSave::IsVisibleFrameNewFlg( u8 index )
{
  GFL_ASSERT( index < FRAME_FLG_MAX );

  return (m_Data.frame_new_flg[index] == NEW_ICON_VISIBLE);
}


/**
 * @brief   通信で共有する写真データを設定する
 * @param   ファインダースタジオ写真統合データ
 */
void FinderStudioSave::SetSendPhotoData( const App::FinderStudio::Static::IntegratedData& sendData )
{
  m_Data.sendData = sendData;
  m_Data.isSendDataEnable = true;

  // CRC計算
  m_Data.sendData.photo_data.deco_param.crc = App::FinderStudio::Static::FinderStudioUtil::GetPhotoDataCrc( &m_Data.sendData.photo_data );
}

/**
 * @brief   通信で共有する写真データが有効かどうか
 * @return  有効ならばtrue
 */
bool FinderStudioSave::IsSendPhotoDataEnable() const
{
  return m_Data.isSendDataEnable;
}

/**
 * @brief   通信で共有する写真データを取得する
 * @return  ファインダースタジオ写真統合データ
 */
const App::FinderStudio::Static::IntegratedData& FinderStudioSave::GetSendPhotoData() const
{
  GFL_ASSERT_MSG(m_Data.isSendDataEnable, "写真共有が無効になってます\n");
  return m_Data.sendData;
}

/**
 * @brief   通信で共有する写真データを解除する
 */
void FinderStudioSave::RemoveSendPhotoData()
{
  m_Data.isSendDataEnable = false;
}

GFL_NAMESPACE_END( Savedata )
