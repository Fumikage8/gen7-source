#if !defined __FINDER_STUDIO_CAPTURE_SDCARD_SAVE_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_SDCARD_SAVE_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureSDCardSaveFrame.h
 * @date    2016/11/28 17:26:35
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：SDカード書き込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameBase.h"

#include <App/FinderStudioStatic/include/FinderStudioErrorConverter.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioUtil.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ImageDBUtil;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
class PhotoListView;
class SelectButtonView;
class PrepareCommonView;
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
  /// 初期化シーケンス
  enum InitSequence
  {
    INIT_SEQ_VIEW_LIST,
    INIT_SEQ_NUM,

    INIT_SEQ_BEGIN = INIT_SEQ_VIEW_LIST,
    INIT_SEQ_END = INIT_SEQ_NUM,
  };
  /// メインシーケンス
  enum
  {
    MAIN_SEQ_SAVE,          //!< 保存シーケンス
    MAIN_SEQ_CONFIRM,       //!< 撮影継続確認
    MAIN_SEQ_CURTAIN_OPEN,  //!< カーテンオープン
    MAIN_SEQ_FATAL_ERROR,   //!< フェイタルエラー
    MAIN_SEQ_FORCED_END,    //!< 強制終了
    MAIN_SEQ_NUM,

    MAIN_SEQ_BEGIN = MAIN_SEQ_SAVE,
    MAIN_SEQ_END = MAIN_SEQ_NUM,
  };
  /// 保存シーケンス
  enum
  {
    SAVE_SEQ_INIT,            //!< 初期化
    SAVE_SEQ_CURTAIN_OPEN,    //!< カーテンを開く
    SAVE_SEQ_IMAGEDB,         //!< ImageDBへの保存
    SAVE_SEQ_IMAGEDB_ERROR,   //!< ImageDBエラー
    SAVE_SEQ_EXTSAVE,         //!< 拡張セーブデータの保存
    SAVE_SEQ_EXTSAVE_ERROR,   //!< 拡張セーブデータエラー
    SAVE_SEQ_FINISH,          //!< 保存終了
    SAVE_SEQ_REGION,          //!< 残数チェック
    SAVE_SEQ_EVALUATION,      //!< 評価
    SAVE_SEQ_CURTAIN_CLOSE,   //!< カーテンを閉じる
    SAVE_SEQ_NUM,

    SAVE_SEQ_BEGIN = SAVE_SEQ_INIT,
    SAVE_SEQ_END = SAVE_SEQ_NUM,
  };
  /// 継続選択シーケンス
  enum
  {
    CONFIRM_SEQ_SHOW,
    CONFIRM_SEQ_MAIN,
    CONFIRM_SEQ_NUM,

    CONFIRM_SEQ_BEGIN = CONFIRM_SEQ_SHOW,
    CONFIRM_SEQ_END = CONFIRM_SEQ_NUM,
  };
  /// カーテン演出シーケンス
  enum CurtainSequence
  {
    CURTAIN_SEQ_START,
    CURTAIN_SEQ_WAIT,
    CURTAIN_SEQ_NUM,

    CURTAIN_SEQ_BEGIN = CURTAIN_SEQ_START,
    CURTAIN_SEQ_END = CURTAIN_SEQ_NUM,
  };
  /// ImageDB保存シーケンス
  enum ImageDBSequence
  {
    IMAGEDB_SEQ_START,
    IMAGEDB_SEQ_WAIT,
    IMAGEDB_SEQ_NUM,

    IMAGEDB_SEQ_BEGIN = IMAGEDB_SEQ_START,
    IMAGEDB_SEQ_END = IMAGEDB_SEQ_NUM,
  };
  /// 拡張セーブデータ保存シーケンス
  enum ExtSaveSequence
  {
    EXTSAVE_SEQ_INIT,
    EXTSAVE_SEQ_START,
    EXTSAVE_SEQ_WAIT,
    EXTSAVE_SEQ_FINISH,
    EXTSAVE_SEQ_NUM,

    EXTSAVE_SEQ_BEGIN = EXTSAVE_SEQ_INIT,
    EXTSAVE_SEQ_END = EXTSAVE_SEQ_NUM,
  };
  /// 保存結果シーケンス
  enum SaveFinSequence
  {
    SAVE_FIN_SEQ_CHECK,
    SAVE_FIN_SEQ_MSGWND,
    SAVE_FIN_SEQ_MSGWND_WAIT,
    SAVE_FIN_SEQ_NUM,

    SAVE_FIN_SEQ_BEGIN = SAVE_FIN_SEQ_CHECK,
    SAVE_FIN_SEQ_END = SAVE_FIN_SEQ_NUM,
  };
  /// 残数チェックシーケンス
  enum RegionSequence
  {
    REGION_SEQ_CHECK,
    REGION_SEQ_MSGWND,
    REGION_SEQ_MSGWND_WAIT,
    REGION_SEQ_NUM,

    REGION_SEQ_BEGIN = REGION_SEQ_CHECK,
    REGION_SEQ_END = REGION_SEQ_NUM,
  };
  /// 評価シーケンス
  enum EvaluationSequence
  {
    EVAL_SEQ_MSGWND,
    EVAL_SEQ_MSGWND_WAIT,
    EVAL_SEQ_VOICE,
    EVAL_SEQ_VOICE_WAIT,
    EVAL_SEQ_FRIENDSHIP,
    EVAL_SEQ_NUM,

    EVAL_SEQ_BEGIN = EVAL_SEQ_MSGWND,
    EVAL_SEQ_END = EVAL_SEQ_NUM,
  };

  /// メッセージウィンドウID
  enum MsgWndID
  {
    MSGWND_ID_SAVE_START,               //!< 保存開始
    MSGWND_ID_SAVE_SUCCESS,             //!< 保存完了
    MSGWND_ID_IMGDB_ERROR_STORAGE,      //!< ImageDBエラー：空き容量不足
    MSGWND_ID_IMGDB_ERROR_OVER_NUM,     //!< ImageDBエラー：管理領域オーバー
    MSGWND_ID_IMGDB_ERROR_OTHER,        //!< ImageDBエラー：それ以外のエラー
    MSGWND_ID_EXTSAVE_ERROR_BROKEN,     //!< ExtSaveエラー：データ破損
    MSGWND_ID_EXTSAVE_ERROR_STORAGE,    //!< ExtSaveエラー：空き容量不足
    MSGWND_ID_REGION_CAUTION,           //!< 保存枚数の注意喚起
    MSGWND_ID_REGION_FULL,              //!< 保存枚数の最大
    MSGWND_ID_EVALUATION_A,             //!< 評価パターンＡ
    MSGWND_ID_EVALUATION_B,             //!< 評価パターンＢ
    MSGWND_ID_EVALUATION_C,             //!< 評価パターンＣ
    MSGWND_ID_EVALUATION_D,             //!< 評価パターンＤ
    MSGWND_ID_EVALUATION_E,             //!< 評価パターンＥ
    MSGWND_ID_NUM,

    MSGWND_ID_NULL = MSGWND_ID_NUM,
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
  void _UpdateSave( void );
  void _UpdateConfirm( void );
  void _UpdateCurtainOpen( void );

  //-------------------------------------------------------
  // @brief   保存処理の初期化：非公開
  //-------------------------------------------------------
  void _SaveInit( void );
  void _SaveInit_PhotoData( void );
  void _SaveInit_SortData( void );
  void _SaveInit_Date( void );
  void _SaveInit_Record( void );
  void _SaveInit_UpperView( void );
  void _SaveInit_MsgWnd( void );

  //-------------------------------------------------------
  // @brief   保存処理 ImageDB：非公開
  //-------------------------------------------------------
  void _SaveImageDB( void );
  bool _SaveImageDB_Proc( void );
  void _SaveImageDB_MsgWnd( void );
  void _SaveImageDB_MsgWndEvent( MsgWndID id );

  /// ImageDB保存シーケンスを初期化
  void _InitImageDBSequence( void ) { m_ImageDBSeq = IMAGEDB_SEQ_BEGIN; }

  //-------------------------------------------------------
  // @brief   保存処理 ExtSave：非公開
  //-------------------------------------------------------
  void _SaveExtSave( void );
  bool _SaveExtSave_Proc( void );
  void _SaveExtSave_MsgWnd( void );
  void _SaveExtSave_MsgWndEvent( MsgWndID id );

  /// 拡張セーブデータ保存シーケンスを初期化
  void _InitExtSaveSequence( void ) { m_ExtSaveSeq = EXTSAVE_SEQ_BEGIN; }

  //-------------------------------------------------------
  // @brief   保存処理 終了：非公開
  //-------------------------------------------------------
  void _SaveFinish( void );
  bool _SaveFinish_Check( void );
  void _SaveFinish_MsgWnd( void );
  void _SaveFinish_MsgWndEvent( MsgWndID id );

  //-------------------------------------------------------
  // @brief   残数チェック：非公開
  //-------------------------------------------------------
  void _SaveRegion( void );
  bool _SaveRegion_Check( void );
  void _SaveRegion_MsgWnd( void );
  void _SaveRegion_MsgWndEvent( MsgWndID id );

  //-------------------------------------------------------
  // @brief   評価：非公開
  //-------------------------------------------------------
  void _SaveEvaluation( void );
  void _SaveEvaluation_MsgWnd( void );
  void _SaveEvaluation_MsgWndEvent( void );
  void _SaveEvaluation_Voice( void );
  bool _SaveEvaluation_VoiceWait( void );
  void _SaveEvaluation_Friendship( void );

  //-------------------------------------------------------
  // @brief   終了確認：非公開
  //-------------------------------------------------------
  void _FinishConfirm_ShowView( void );
  void _FinishConfirm_Proc( void );
  void _FinishConfirm_Result( u32 result );

  //-------------------------------------------------------
  // @brief   カーテン演出：非公開
  //-------------------------------------------------------
  bool _CurtainOpen( void );
  bool _CurtainClose( void );

  /// カーテン演出のシーケンスを初期化
  void _InitCurtainSequence( void ) { m_CurtainSeq = CURTAIN_SEQ_BEGIN; }

private:
  //-------------------------------------------------------
  // @brief   メッセージウィンドウ
  //-------------------------------------------------------
  void _ShowMessageWindow( MsgWndID id );
  void _HideMessageWindow( void );

  void _MessageWindowEvent( void );

  //-------------------------------------------------------
  // @brief   取得関数
  //-------------------------------------------------------
  ViewerWork  _GetViewerWork( void ) { return m_pMainWork->work->viewer; }
  PrepareWork _GetPrepareWork( void ) { return m_pMainWork->work->prepare; }
  pml::pokepara::PokemonParam* _GetPokemonParam( void ) { return m_pMainWork->work->prepare.pokemon_param; }

  //-------------------------------------------------------
  // @brief   チェック関数
  //-------------------------------------------------------
  bool _IsSavePhotoDataExist( void ) const;

  //-------------------------------------------------------
  // @brief   ホーム／スリープ禁止
  //-------------------------------------------------------
  void _SetEnableHomeSleep( void );
  void _SetDisableHomeSleep( void );
  bool _IsEnableHomeSleep( void );

  //-------------------------------------------------------
  // @brief   リスナー関数：非公開
  //-------------------------------------------------------
  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

private:
  /// メッセージウィンドウの表示メッセージタイプ
  static const ErrorConverter::Message::MsgType MESSAGE_TYPE = ErrorConverter::Message::MSG_TYPE_SAVE_A;

private:
  UpperView*          m_pUpperView;
  PhotoListView*      m_pPhotoListView;
  SelectButtonView*   m_pSelectButtonView;
  PrepareCommonView*  m_pPrepareCommonView;

private:
  void*               m_pPhotoData[PHOTO_MAX];
  Viewer::PhotoParam  m_PhotoParam[PHOTO_MAX];
  u32                 m_SavePhotoNum;
  u32                 m_SaveRegionNum;

  Static::IntegratedData    m_IntegratedData;
  Static::ManagementData    m_ManagementData;

  u8    m_SaveCount;

private:
  Static::ImageDBUtil*                m_pImageDBUtil;
  ErrorConverter::ImageDBResultCode   m_ImageDBResultCode;

private:
  Static::FinderStudioExtSaveDataAccessor*    m_pExtSaveData;
  ErrorConverter::SDCardResultCode            m_ExtSaveResultCode;
  bool                                        m_bBrokenData;

private:
  ErrorConverter::Message*    m_pMessage;
  MsgWndID                    m_MsgWndID;

private:
  InitSequence        m_InitSeq;
  CurtainSequence     m_CurtainSeq;
  ImageDBSequence     m_ImageDBSeq;
  ExtSaveSequence     m_ExtSaveSeq;
  SaveFinSequence     m_SaveFinSeq;
  RegionSequence      m_RegionSeq;
  EvaluationSequence  m_EvaluationSeq;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SDCARD_SAVE_FRAME_H_INCLUDED__
