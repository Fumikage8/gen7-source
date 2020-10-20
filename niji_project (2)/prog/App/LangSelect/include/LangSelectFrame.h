#ifndef NIJI_PROJECT_APP_LANG_SELECT_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_FRAME_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectFrame.h
 * @date 2015/07/23 15:11:42
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/include/LangSelectAppParam.h"
#include "App/LangSelect/source/LangSelectConst.h"
#include "App/LangSelect/source/LangSelectDrawListener.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )
class LangSelectUpperDraw;
class LangSelectLowerDraw;
class MenuCursor;
GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   言語選択フレーム
//------------------------------------------------------------------------------
class LangSelectFrame
  : public applib::frame::CellSuper
  , public LangSelectDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LangSelectFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  LangSelectFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LangSelectFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ダイナミックヘッダーの読み込み
   */
  //------------------------------------------------------------------
  bool LoadDynamicHeader( void );

  //------------------------------------------------------------------
  /**
   * @brief   レイアウトデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadMsgData( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの生成
   */
  //------------------------------------------------------------------
  bool CreateMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager );

private:
  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：セーブ状態
   */
  //------------------------------------------------------------------
  void Sequence_SaveStatus( void );

  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：フォーマット
   */
  //------------------------------------------------------------------
  void Sequence_Format( void );

  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：データ破損
   */
  //------------------------------------------------------------------
  void Sequence_ReportBroken( void );

  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：データ修復
   */
  //------------------------------------------------------------------
  void Sequence_ReportRepair( void );

  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：拡張セーブ状態
   */
  //------------------------------------------------------------------
  void Sequence_ExsaveStatus( void );

  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：拡張セーブフォーマット
   */
  //------------------------------------------------------------------
  void Sequence_ExsaveFormat( void );

  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：拡張セーブ破損
   */
  //------------------------------------------------------------------
  void Sequence_ExsaveBroken( void );

  //------------------------------------------------------------------
  /**
   * @brief   シーケンス：拡張セーブエラー
   */
  //------------------------------------------------------------------
  void Sequence_ExsaveError( void );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   言語選択の開始
   */
  //------------------------------------------------------------------
  virtual void StartLangSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief   画面モードの設定
   * @param   dispMode    画面モード
   */
  //------------------------------------------------------------------
  virtual void SetDispMode( DispMode dispMode );

  //------------------------------------------------------------------
  /**
   * @brief   言語選択画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndLangSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief   選択した言語
   * @param   langID    言語ID
   * @param   kanji     漢字フラグ
   */
  //------------------------------------------------------------------
  virtual void SetLangID( u8 langID, bool kanji );

  //------------------------------------------------------------------
  /**
   * @brief   セーブデータ削除コマンドの入力
   */
  //------------------------------------------------------------------
  virtual void InputSavedataDeleteCommand( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   Aボタン入力
   */
  //--------------------------------------------------------------------------
  virtual void InputDecide( bool input );

private:
  //-----------------------------------------------------------------------------
  /**
   * @brief   TalkWindow用リソース読み込み
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadTalkWindowResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   言語切り替え時のデータ再設定
   * @caution   これ以降、メッセージウィンドウ以外への文字描画は禁止！
   */
  //-----------------------------------------------------------------------------
  void ReloadLangData(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   メッセージ関連専用のローカルヒープを生成
   */
  //-----------------------------------------------------------------------------
  void CreateMessageHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   メッセージ関連専用のローカルヒープを削除
   */
  //-----------------------------------------------------------------------------
  void DeleteMessageHeap(void);

private:
  static const u32 MESSAGE_HEAP_SIZE = 0x100000;  //!< メッセージ関連のローカルヒープサイズ

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  app::util::Heap*                  m_pAppHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::str::MsgData*   m_pMsgData;
  
  print::WordSet*       m_pWordSet;

  void*   m_pArcReadBuff;
  void*   m_pMsgReadBuff;

  void*   m_pWinResBuff;    //!< TalkWindow用リソースバッファ

  u8      m_DynaLoadSeq;
  u8      m_LytCount;
  u8      m_LytLoadSeq;
  u8      m_MsgLoadSeq;
  u8      m_CursorSeq;

  u8      m_winResLoadSeq;  //!< TalkWindowリソース読み込みシーケンス

  int     m_SaveSeq;
  bool    m_SaveDelete;

  bool    m_bInputDecide;

  APP_PARAM*                    m_pAppParam;

  LangSelectUpperDraw*          m_pUpperDraw;

  LangSelectLowerDraw*          m_pLowerDraw;

  MenuCursor*                   m_pCursor;

  GameSys::GameData*            m_pGameData;
  GameSys::GameManager*         m_pGameManager;

  gfl2::heap::HeapBase*         m_pMsgHeap;
};

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_LANG_SELECT_FRAME_H_INCLUDED