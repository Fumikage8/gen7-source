//======================================================================
/**
 * @file    FinderStudioErrorConverter.cpp
 * @date    2017/01/10 10:42:26
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：エラーコンバータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioErrorConverter.h>

#include <GameSys/include/GameManager.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSet.h>

#include <debug/include/gfl2_Assert.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_fs_sd.h>
#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )

//////////////////////////////////////////////////////////////////////////
/// ---[ グローバル変数宣言 begin ]---
gfl2::str::MsgData* ErrorConverter::m_pMsgData = NULL;
/// ---[ グローバル変数宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    独自コードへのコンバート
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   独自のSDカードリザルトコードへ変換
 *
 * @param   result    SDカードリザルト(gfl2::fs::ExtSaveFile::Result)
 *
 * @retval  SDCardResultCode：独自のSDカードリザルトコード
 */
ErrorConverter::SDCardResultCode ErrorConverter::ConvertSDCardErrorFromExtSaveResult( gfl2::fs::ExtSaveFile::Result result )
{
  // フェイタルエラーチェック
  if( _IsFatalError( result ) )
  {
    return SDCARD_RESULT_FATAL_ERROR;
  }

  switch( result )
  {
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:        //!< パスが存在しない＝指定したファイルが見つからない
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:    //!< すでにパスが存在する＝存在するファイル名でCreate/Renameの操作を行った
    {
      return SDCARD_RESULT_FATAL_ERROR;
    } // no break;

    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:           //!< ファイルが壊れている＝正常でないため、削除が必要です
    {
      return SDCARD_RESULT_BROKEN_DATA;
    } // no break;

    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:         //!< 空き容量が不足している＝ファイルを作成した時に発生
    {
      return SDCARD_RESULT_NOT_SPACE;
    } // no break;

    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                       //!< 成功
    {
      return SDCARD_RESULT_SUCCESS;
    } // no break;
  }

  GFL_ASSERT( 0 );    //!< 仕様にない分岐
  return SDCARD_RESULT_FATAL_ERROR;
}

/**
 * @brief   独自のImageDBリザルトコードへ変換
 *
 * @param   imgRes    ImageDBリザルト
 *
 * @retval  ImageDBResultCode：独自のImageDBリザルトコード
 */
ErrorConverter::ImageDBResultCode ErrorConverter::ConvertImageDBErrorFromImgDBResult( gfl2::imagedb::ImageDBAccessor::imgDbResult imgRes )
{
#if defined(GF_PLATFORM_CTR)
  switch( imgRes )
  {
    case imgdb::ResultErrorFileOperationFailure:        //!< ファイル操作失敗
    case imgdb::ResultErrorReadOnlyDbFile:              //!< 管理ファイルが読み取り専用属性になっている
    case imgdb::ResultErrorNotExistStorage:             //!< SDカードが挿されていない
    case imgdb::ResultErrorWriteProtect:                //!< SDカードがライトプロテクトされている
    case imgdb::ResultErrorNotAvailableStorage:         //!< 利用可能なSDカードでない
    {
      return IMGDB_RESULT_FATAL_ERROR;
    } // no break;

    case imgdb::ResultErrorInsufficientStorageSpace:    //!< SDカードの空き不足
    {
      return IMGDB_RESULT_NOT_SPACE;
    } // no break;

    case imgdb::ResultErrorOverNumImages:               //!< SDカードの空き不足
    {
      return IMGDB_RESULT_OVER_IMAGE;
    } // no break;

    case imgdb::ResultSuccess:                          //!< 成功
    {
      return IMGDB_RESULT_SUCCESS;
    } // no break;
  }

  return IMGDB_RESULT_OTHER_ERROR;                      //!< その他のエラー
#else
  return IMGDB_RESULT_SUCCESS;
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    便利関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   フェイタルエラーチェック
 *
 * @param   result    SDカードリザルト
 *
 * @retval  "true"ならフェイタルエラー
 * @retval  "false"ならそれ以外
 */
bool ErrorConverter::_IsFatalError( gfl2::fs::ExtSaveFile::Result result )
{
  return ExtSavedata::NijiExtSaveSystem::IsFatalError( result );
}








//////////////////////////////////////////////////////////////////////////
///
/// @brief    Message：メッセージクラス
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
ErrorConverter::Message::Message( gfl2::heap::HeapBase* heap, u32 charMax /*= 1024*/ )
{
  // メッセージデータの生成
  m_pMsgData = GFL_NEW( heap ) gfl2::str::MsgData(
    print::GetMessageArcId(), GARC_message_fs_sd_DAT, heap, gfl2::str::MsgData::LOAD_FULL );
  // 文字バッファの生成
  m_pStrBuf = GFL_NEW( heap ) gfl2::str::StrBuf( charMax, heap );
  m_pTempStrBuf = GFL_NEW( heap ) gfl2::str::StrBuf( charMax, heap );
  // WordSetの生成
  m_pWordSet = GFL_NEW( heap ) print::WordSet( heap );
}

/**
 * @brief   デストラクタ
 */
ErrorConverter::Message::~Message( void )
{
  // WordSetの破棄
  GFL_DELETE( m_pWordSet );
  // 文字バッファの破棄
  GFL_DELETE( m_pTempStrBuf );
  GFL_DELETE( m_pStrBuf );
  // メッセージデータの破棄
  GFL_DELETE( m_pMsgData );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージ 取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージの取得
 *
 * @param   msgID   メッセージID
 * @param   strBuf  文字列
 */
void ErrorConverter::Message::GetString( u32 msgID, gfl2::str::StrBuf* strBuf )
{
  _GetString( msgID, strBuf );
}

/**
 * @brief   メッセージの取得：実体
 *
 * @param   msgID   メッセージID
 * @param   strBuf  文字列
 */
void ErrorConverter::Message::_GetString( u32 msgID, gfl2::str::StrBuf* strBuf )
{
  GFL_ASSERT( m_pMsgData );
  GFL_ASSERT( strBuf );

  // メッセージの取得
  m_pMsgData->GetString( msgID, *strBuf );
}


/**
 * @brief   メッセージの取得：開始メッセージ
 *
 * @param   type    メッセージタイプ
 *
 * @retval  メッセージ文字列
 */
gfl2::str::StrBuf* ErrorConverter::Message::GetBeginMessage( MsgType type )
{
  u32 c_BeginMsgID[MSG_TYPE_NUM] = {
    fs_sd_00,
    fs_sd_00,
    fs_sd_00,
    fs_sd_00,
    fs_sd_15,   //!< Delete
    fs_sd_16,   //!< OverWrite
  };
  
  // メッセージの取得
  _GetString( c_BeginMsgID[type], m_pStrBuf );

  return m_pStrBuf;
}

/**
 * @brief   メッセージの取得：終了メッセージ
 *
 * @param   type    メッセージタイプ
 *
 * @retval  メッセージ文字列
 */
gfl2::str::StrBuf* ErrorConverter::Message::GetFinishMessage( MsgType type )
{
  u32 c_FinishMsgID[MSG_TYPE_NUM] = {
    fs_sd_12,
    fs_sd_12,
    fs_sd_12,
    fs_sd_12,
    fs_sd_17,   //!< Delete
    fs_sd_18,   //!< OverWrite
  };
  
  GFL_ASSERT( type != MSG_TYPE_LOAD_A );    //!< 読込みでは使用されないメッセージのため
  GFL_ASSERT( type != MSG_TYPE_LOAD_B );    //!< 読込みでは使用されないメッセージのため
  
  // メッセージの取得
  _GetString( c_FinishMsgID[type], m_pStrBuf );

  return m_pStrBuf;
}

/**
 * @brief   メッセージの取得：ImageDBエラーメッセージ
 *
 * @param   type    メッセージタイプ
 * @param   code    エラーコード
 *
 * @retval  メッセージ文字列
 */
gfl2::str::StrBuf* ErrorConverter::Message::GetImgDBErrorMessage( MsgType type, ErrorConverter::ImageDBResultCode code )
{
  u32 c_ImgDBErrorMsgID[ErrorConverter::IMGDB_RESULT_NUM] = {
    fs_sd_00,   //!< SUCCESS
    fs_sd_00,   //!< FATAL_ERROR
    fs_sd_01,   //!< NOT_SPACE
    fs_sd_02,   //!< OVER_IMAGE
    fs_sd_03,   //!< OTHER_ERROR
  };
  
  GFL_ASSERT( code != ErrorConverter::IMGDB_RESULT_SUCCESS );
  GFL_ASSERT( code != ErrorConverter::IMGDB_RESULT_FATAL_ERROR );
  
  u32 msgID = c_ImgDBErrorMsgID[code];
  
  if( code == ErrorConverter::IMGDB_RESULT_OVER_IMAGE )
  {
    if( type == MSG_TYPE_LOAD_B || type == MSG_TYPE_SAVE_B )
    {
      msgID = fs_sd_08;
    }
  }
  
  // メッセージの取得
  _GetString( msgID, m_pStrBuf );

  return m_pStrBuf;
}

/**
 * @brief   メッセージの取得：拡張セーブデータエラーメッセージ
 *
 * @param   code    エラーコード
 *
 * @retval  メッセージ文字列
 */
gfl2::str::StrBuf* ErrorConverter::Message::GetExtSaveErrorMessage( ErrorConverter::SDCardResultCode code )
{
  u32 c_ExtSaveErrorMsgID[ErrorConverter::SDCARD_RESULT_NUM] = {
    fs_sd_00,   //!< SUCCESS
    fs_sd_00,   //!< FATAL_ERROR
    fs_sd_04,   //!< BROKEN_DATA
    fs_sd_05,   //!< NOT_SPACE
  };
  
  // メッセージの取得
  _GetString( c_ExtSaveErrorMsgID[code], m_pStrBuf );

  return m_pStrBuf;
}

/**
 * @brief   メッセージの取得：管理領域残数のメッセージ
 *
 * @param   type    メッセージタイプ
 * @param   num     残り枚数
 *
 * @retval  メッセージ文字列
 */
gfl2::str::StrBuf* ErrorConverter::Message::GetRegionMessage( MsgType type, u32 num )
{
  enum RegionType {
    REGION_FEW,   //!< 少ない
    REGION_NONE,  //!< ない
    REGION_NUM,
  };

  u32 c_RegionMsgID[MSG_TYPE_NUM][REGION_NUM] = {
    { fs_sd_06, fs_sd_07 },
    { fs_sd_09, fs_sd_10 },
    { fs_sd_13, fs_sd_14 },
    { fs_sd_13, fs_sd_14 },
    { fs_sd_07, fs_sd_08 },   //!< DELETE
    { fs_sd_07, fs_sd_08 },   //!< OVERWRITE
  };

  GFL_ASSERT( type != MSG_TYPE_DELETE );
  GFL_ASSERT( type != MSG_TYPE_OVERWRITE );

  RegionType region = ( num == 0 ) ? REGION_NONE : REGION_FEW;

  if( region == REGION_FEW )
  {
    // 数値のセット
    m_pWordSet->RegisterNumber( 0, num, 1, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
    // メッセージのセット
    _GetString( c_RegionMsgID[type][region], m_pTempStrBuf );
    // Expand
    m_pWordSet->Expand( m_pStrBuf, m_pTempStrBuf );
  }
  else {
    // メッセージの取得
    _GetString( c_RegionMsgID[type][region], m_pStrBuf );
  }

  return m_pStrBuf;
}

GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
