#if !defined __FINDER_STUDIO_DECORATION_SDCARD_SAVE_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_DECORATION_SDCARD_SAVE_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationSDCardSaveFrame.h
 * @date    2017/01/31 15:40:12
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：SDカード書き込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameBase.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioErrorConverter.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ImageDBUtil;
class FinderStudioExtSaveDataAccessor;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_END( gfl2 )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    SDカード書き込みフレーム
///
//////////////////////////////////////////////////////////////////////////
class SDCardSaveFrame
  : public FrameBase
  , public Static::MessageMenuViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( SDCardSaveFrame );

private:
  enum {
    SEQ_FRAME_COUNT,
    SEQ_SAVE_IMAGEDB,
    SEQ_SAVE_EXTSAVEDATA,
    SEQ_SAVE_RESULT,
    SEQ_CHECK_REGION,
    SEQ_ERROR_MESSAGE,    //!< エラーメッセージ
    SEQ_FORCED_END,       //!< 強制終了
    SEQ_FATAL_ERROR,      //!< フェイタルエラー
    SEQ_END,
    SEQ_NUM,
  };

  enum SaveMsgWnd
  {
    SAVE_MSGWND_ID_SAVE_SUCCESS,
    SAVE_MSGWND_ID_IMDDB_ERROR,
    SAVE_MSGWND_ID_EXTSAVE_BROKEN,
    SAVE_MSGWND_ID_EXTSAVE_ERROR,
    SAVE_MSGWND_ID_REGION,
    SAVE_MSGWND_ID_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  SDCardSaveFrame( void );
  virtual ~SDCardSaveFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  applib::frame::Result _UpdateFrameCount( void );
  applib::frame::Result _UpdateSaveImageDB( void );
  applib::frame::Result _UpdateSaveExtSaveData( void );
  applib::frame::Result _UpdateSaveResult( void );
  applib::frame::Result _UpdateCheckRegion( void );
  applib::frame::Result _UpdateErrorMessage( void );

  //-------------------------------------------------------
  // @brief   初期化
  //-------------------------------------------------------
  void _InitFunc_ExtSaveData( void );
  void _InitFunc_DecoData( void );
  void _InitFunc_OverWrite( void );
  void _InitFunc_NewWrite( void );

  //-------------------------------------------------------
  // @brief   保存
  //-------------------------------------------------------
  bool _SaveImageDB( void );
  bool _SaveImageDB_OverWrite( void );
  bool _SaveImageDB_NewWrite( void );

  bool _SaveExtSavedata( void );

  //-------------------------------------------------------
  // @brief   保存データ関連
  //-------------------------------------------------------
  void _SetDate( void );

  //-------------------------------------------------------
  // @brief   メッセージウィンドウ
  //-------------------------------------------------------
  void _ShowMsgWnd( SaveMsgWnd id );
  void _HideMsgWnd( void );

  void _ShowErrorMessage( void );

  //-------------------------------------------------------
  // @brief   リスナー関数：非公開
  //-------------------------------------------------------
  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

private:
  Static::FinderStudioExtSaveDataAccessor*    m_pExtSaveData;
  Static::ImageDBUtil*                        m_pImageDBUtil;

  Static::IntegratedData    m_SavePhotoData;

  ErrorConverter::Message*            m_pMessage;
  ErrorConverter::ImageDBResultCode   m_ImageDBResultCode;
  ErrorConverter::SDCardResultCode    m_SDCardResultCode;

  SaveType      m_SaveType;

  u32           m_SavePhotoNum;

  u32           m_DataIndex;
  wchar_t*      m_pFilePath;
  wchar_t       m_DeleteFilePath[Static::SAVE_FILEPATH_LENGTH];

  bool          m_bBrokenData;

  SaveMsgWnd    m_MsgWndID;

  u32           m_FrameCount;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_SDCARD_SAVE_FRAME_H_INCLUDED__
