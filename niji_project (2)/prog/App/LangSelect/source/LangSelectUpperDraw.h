#ifndef NIJI_PROJECT_APP_LANG_SELECT_UPPER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_UPPER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectUpperDraw.h
 * @date 2015/07/23 15:45:09
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/include/LangSelectAppParam.h"
#include "App/LangSelect/source/LangSelectConst.h"

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )
class LangSelectCursor;
class LangSelectButton;
class MenuCursor;
GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class KeyRepeatController;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
class TalkWindow;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   言語選択下画面
//------------------------------------------------------------------------------
class LangSelectUpperDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LangSelectUpperDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  LangSelectUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager, MenuCursor* pCursor );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LangSelectUpperDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam       外部設定パラメータ
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   pWinLytBuff     TalkWindow用レイアウトバイナリ
   * @param   msgData         メッセージデータ
   * @param   wordSet         ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* pWinLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void LangSelectOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void LangSelectClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

  //------------------------------------------------------------------
  /**
   * @brief   言語選択の開始
   */
  //------------------------------------------------------------------
  void StartLangSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( LangSelectDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージ関連のデータを再設定
   * @param   msg_data  メッセージデータ
   * @param   word_set  単語セット
   */
  //------------------------------------------------------------------
  void ResetMessage( gfl2::str::MsgData * msg_data, print::WordSet * word_set );


private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pAppLytBuff     レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   デフォルト言語のセット
   * @param   langID    言語IG
   */
  //------------------------------------------------------------------
  void SetDefaultLang( u8 langID );

  //------------------------------------------------------------------
  /**
   * @brief   画面モードの設定
   * @param   dispMode    画面モード
   */
  //------------------------------------------------------------------
  void SetDispMode( DispMode dispMode );

  //------------------------------------------------------------------
  /**
   * @brief   選択言語のセット
   * @param   pos   選択位置
   */
  //------------------------------------------------------------------
  void SelectLangID( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   着地SEの再生
   */
  //------------------------------------------------------------------
  void PlaySE_Landing( void );

  //------------------------------------------------------------------
  /**
   * @brief   TalkWindow生成
   * @param   res_buf   リソースバッファ
   */
  //------------------------------------------------------------------
  void CreateTalkWindow( void * res_buf );

  //------------------------------------------------------------------
  /**
   * @brief   TalkWindow削除
   */
  //------------------------------------------------------------------
  void DeleteTalkWindow(void);

  //------------------------------------------------------------------
  /**
   * @brief   メッセージ表示開始
   * @param   id  メッセージID
   */
  //------------------------------------------------------------------
  void PrintMessage( u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージ表示終了チェック
   * @retval  true  = 表示中
   * @retval  false = 終了
   */
  //------------------------------------------------------------------
  bool IsPrintMessage(void);

  //------------------------------------------------------------------
  /**
   * @brief   メッセージ終了処理
   */
  //------------------------------------------------------------------
  void EndMessage(void);

  //------------------------------------------------------------------
  /**
   * @brief   設定終了の次へ進めるか
   * @retval  true  = ok
   * @retval  false = ng
   */
  //------------------------------------------------------------------
  bool IsEndSettingSeq(void);

  //------------------------------------------------------------------
  /**
   * @brief   博士からの連絡アニメの次へ進めるか
   * @retval  true  = ok
   * @retval  false = ng
   */
  //------------------------------------------------------------------
//  bool IsEndCallSeq(void);

  //------------------------------------------------------------------
  /**
   * @brief   博士からの連絡メッセージの次へ進めるか
   * @retval  true  = ok
   * @retval  false = ng
   */
  //------------------------------------------------------------------
  bool IsEndCallMessageSeq(void);

  //------------------------------------------------------------------
  /**
   * @brief   言語選択を変更できるタイミングか
   * @retval  true  = ok
   * @retval  false = ng
   */
  //------------------------------------------------------------------
  bool IsSetLangFrame(void);

  //------------------------------------------------------------------
  /**
   * @brief   設定後のウェイト開始
   * @param   flg   true = ウェイト後にメッセージ送り矢印表示
   */
  //------------------------------------------------------------------
  void StartSettingWait( bool flg = false );

  //------------------------------------------------------------------
  /**
   * @brief   設定後のウェイト更新処理
   */
  //------------------------------------------------------------------
  void UpdateSettingWait(void);

  //------------------------------------------------------------------
  /**
   * @brief   設定後のウェイト中か
   * @retval  true  = ウェイト中
   * @retval  false = それ以外
   */
  //------------------------------------------------------------------
  bool IsSettingWait(void);

  //------------------------------------------------------------------
  /**
   * @brief   博士からの呼び出しSEをリプレイ
   * @retval  true  = 再生した
   * @retval  false = それ以外
   */
  //------------------------------------------------------------------
  bool ReplayCallSE(void);


public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:   //!< ボタンマネージャ
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

  //------------------------------------------------------------------
  /**
   * @brief   画面モードごとのボタン設定
   * @param   dispMode    画面モード
   */
  //------------------------------------------------------------------
  void SetDispModeEnable( DispMode dispMode );

private:
  static const u8 DISP_LANG_SELECT_NUM = DISP_MODE_NUM - 3;

  static const u8 SETTING_WAIT_COUNT = 15;  //!< 設定終了後のウェイト


private:
  app::util::Heap*        m_pHeap;        //!< ヒープ
  LangSelectDrawListener*  m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  GameSys::GameManager*     m_pGameManager;

  gfl2::Fade::FadeManager*  m_pFadeManager;

  u8    m_Seq;

  APP_PARAM*                m_pAppParam;

  gfl2::str::MsgData*       m_pMsgData;

  print::WordSet*           m_pWordSet;

  DrawerBase*               m_pDrawerBase;

  MenuCursor*               m_pCursor;

  LangSelectButton*         m_pLangButton;

  DispMode                  m_DispMode;

  LangData                  m_LangData[LANG_ID_MAX];

  u32                       m_ActCharaAnim;

  gfl2::lyt::LytPane*       m_pDispPane[DISP_LANG_SELECT_NUM];

  gfl2::lyt::LytParts*      m_pMsgPlate[2];

  gfl2::lyt::LytPane*       m_pPlatePane;

  gfl2::lyt::LytPane*       m_pCharaPane;

  gfl2::lyt::LytPane*       m_pMantaPane;

  app::util::KeyRepeatController*   m_pKeyController[DISP_LANG_SELECT_NUM];

  u8                                m_CursorPos[DISP_LANG_SELECT_NUM];

  bool                              m_DecideFlg;

  bool                              m_PlaySE_Landing;

  App::Tool::TalkWindow * m_pTalkWindow;  //!< TalkWindow
  void * m_talkWindowLytBuff;
  u32 m_printMsgID;
  bool m_isPutMsgArrow;
  b8 m_talkStartFlag;
  u8 m_settingWait;
  b8 m_settingWaitFlag;
  u8 m_selectLangIndex;
  bool m_isKanji;
};

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_LANG_SELECT_UPPER_DRAW_H_INCLUDED