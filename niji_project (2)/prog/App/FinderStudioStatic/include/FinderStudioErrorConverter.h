#if !defined( __FINDER_STUDIO_ERROR_CONVERTER_H_INCLUDED__ )
#define __FINDER_STUDIO_ERROR_CONVERTER_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioErrorConverter.h
 * @date    2017/01/10 10:29:43
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：エラーコンバータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <ExtSavedata/include/NijiExtSaveSystem.h>

#include <imagedb/include/gfl2_ImageDBAccessor.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    エラーコンバータ
///
//////////////////////////////////////////////////////////////////////////
class ErrorConverter
{
public:
  /// 独自の拡張セーブデータ用リザルトコード
  enum SDCardResultCode
  {
    SDCARD_RESULT_SUCCESS,       //!< 成功
    SDCARD_RESULT_FATAL_ERROR,   //!< フェイタルエラー
    SDCARD_RESULT_BROKEN_DATA,   //!< データ破損または改ざん
    SDCARD_RESULT_NOT_SPACE,     //!< 空き容量不足
    SDCARD_RESULT_NUM,
  };
  /// 独自のImageDB用リザルトコード
  enum ImageDBResultCode
  {
    IMGDB_RESULT_SUCCESS,        //!< 成功
    IMGDB_RESULT_FATAL_ERROR,    //!< フェイタルエラー
    IMGDB_RESULT_NOT_SPACE,      //!< 空き容量不足
    IMGDB_RESULT_OVER_IMAGE,     //!< ImgeDB管理枚数オーバー
    IMGDB_RESULT_OTHER_ERROR,    //!< その他のエラー
    IMGDB_RESULT_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   独自コードへのコンバート関数
  //-------------------------------------------------------
  /// コンバート：gfl2::fs::ExtSaveFile::Result => SDCardErrorCode
  static SDCardResultCode  ConvertSDCardErrorFromExtSaveResult( gfl2::fs::ExtSaveFile::Result result );
  /// コンバート：gfl2::fs::ExtSaveFile::Result => SDCardErrorCode
  static ImageDBResultCode ConvertImageDBErrorFromImgDBResult( gfl2::imagedb::ImageDBAccessor::imgDbResult imgRest );

private:
  //-------------------------------------------------------
  // @brief   便利関数：非公開
  //-------------------------------------------------------
  static bool _IsFatalError( gfl2::fs::ExtSaveFile::Result result );

private:
  static gfl2::str::MsgData*     m_pMsgData;
  
public:
  
  //////////////////////////////////////////////////////////////////////////
  ///
  /// @brief    メッセージ
  ///
  //////////////////////////////////////////////////////////////////////////
  class Message
  {
  public:
    /// メッセージタイプ
    enum MsgType
    {
      MSG_TYPE_LOAD_A,      //!< 読込みA：ファインダースタジオ仕様書を確認
      MSG_TYPE_LOAD_B,      //!< 読込みB：ファインダースタジオ仕様書を確認
      MSG_TYPE_SAVE_A,      //!< 書き込みA：ファインダースタジオ仕様書を確認
      MSG_TYPE_SAVE_B,      //!< 書き込みB(保存)：ファインダースタジオ仕様書を確認
      MSG_TYPE_DELETE,      //!< 書き込みB(削除)：ファインダースタジオ仕様書を確認
      MSG_TYPE_OVERWRITE,   //!< 書き込みB(上書き)：ファインダースタジオ仕様書を確認
      MSG_TYPE_NUM,
    };
  
  public:
    //-------------------------------------------------------
    // @brief   コンストラクタ・デストラクタ
    //-------------------------------------------------------
    Message( gfl2::heap::HeapBase* heap, u32 charMax = 1024 );
    ~Message( void );
  
  public:
    //-------------------------------------------------------
    // @brief   メッセージ 取得
    //-------------------------------------------------------
    void GetString( u32 msgID, gfl2::str::StrBuf* strBuf );
    
    gfl2::str::StrBuf* GetBeginMessage( MsgType type );
    gfl2::str::StrBuf* GetFinishMessage( MsgType type );
    
    gfl2::str::StrBuf* GetImgDBErrorMessage( MsgType type, ImageDBResultCode code );
    gfl2::str::StrBuf* GetExtSaveErrorMessage( SDCardResultCode code );
    gfl2::str::StrBuf* GetRegionMessage( MsgType type, u32 num );
  
  private:
    //-------------------------------------------------------
    // @brief   メッセージ：非公開
    //-------------------------------------------------------
    void _GetString( u32 msgID, gfl2::str::StrBuf* strBuf );

  private:
    gfl2::str::MsgData*   m_pMsgData;
    gfl2::str::StrBuf*    m_pStrBuf;
    gfl2::str::StrBuf*    m_pTempStrBuf;
    print::WordSet*       m_pWordSet;
  };
};

GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_ERROR_CONVERTER_H_INCLUDED__