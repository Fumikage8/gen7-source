#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWSEARCH_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWSEARCH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewSearch.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の検索画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <thread/include/gfl2_thread.h>
#include <thread/include/gfl2_threadstatic.h>
#include <thread/include/gfl2_Event.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Tool/app_tool_PaneListView.h>

// 図鑑のインクルード
#include "ZukanSceneParam.h"
#include "ZukanSearchPokeIconDisplay.h"
#include "ZukanContents.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class StrBuf;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanSearchThread;
  class ZukanFrameSearchCell;
  class ZukanViewSearchListener;
  class ZukanSearchPaneListViewListener;
  class ZukanSearchPaneListCallBack;
  class ZukanSearchMessageMenuEventHandler;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewSearch
 @brief     図鑑の検索画面のビュー
 */
//==============================================================================
class ZukanViewSearch : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewSearch);

  friend class ZukanViewSearchListener;
  friend class ZukanSearchPaneListViewListener;
  friend class ZukanSearchPaneListCallBack;
  friend class ZukanSearchThread;
  friend class ZukanSearchMessageMenuEventHandler;

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewSearch(const ZukanSceneParam& param);
  
  virtual ~ZukanViewSearch();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  //! @name  friendクラスZukanViewLaunchListener, ZukanSearchPaneListViewListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  enum ReqId
  {
    REQ_ID_BUTTON_DECIDE     ,  // reqParamはBUTTON_ID_
    REQ_ID_BUTTON_TOUCH      ,  // reqParamはBUTTON_ID_
    REQ_ID_PANE_LIST_DECIDE  ,  // reqParamはリストのインデックス
    REQ_ID_MESSAGE_MENU      ,  // reqParamは0ならYES, 1ならNO
  };

  //! @brief  ボタン
  enum
  {
    BUTTON_ID_BACK                 =   0                            ,       
    BUTTON_ID_NUM_FOR_LOWER_SUB                                     ,
    
    BUTTON_ID_RESET                =   BUTTON_ID_NUM_FOR_LOWER_SUB  ,
    BUTTON_ID_DECIDE                                                ,
    BUTTON_ID_NUM_FOR_LOWER_MAIN                                    ,

    BUTTON_ID_NUM                  =   BUTTON_ID_NUM_FOR_LOWER_MAIN ,
  };
 
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}


  //! @name  friendクラスZukanSearchPaneListCallBackからアクセスするメンバ
  //@{
private:
  void MoveCursor( const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
  void WriteList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  //@}


  //! @name  friendクラスZukanSearchThreadからアクセスするメンバ
  //@{
private:
  gfl2::thread::Event* GetSearchEvent(void) const { return m_searchEvent; }
  void SearchFunction(void);
  //@}


private:
  static const u32 POKE_ICON_DISPLAY_MEM_SIZE = 0x300000;

  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,                    // 表示していない
    DISP_STATE_ON_IN,                  // 表示している(イン中)
    DISP_STATE_ON_WORK,                // 表示している(稼働中)
    DISP_STATE_ON_WAIT_START_OUT,      // 表示している(アウト開始待ち)
    DISP_STATE_ON_OUT,                 // 表示している(アウト中)
    DISP_STATE_ON_LOWER_START_CHANGE,  // 表示している(下画面の変更を開始する)
    DISP_STATE_ON_LOWER_CHANGING,      // 表示している(下画面を変更中)
    DISP_STATE_ON_START_CONFIRM,       // 表示している(終了確認中)
    DISP_STATE_ON_WAIT_CONFIRM,        // 表示している(終了確認中)
    DISP_STATE_ON_END_CONFIRM,         // 表示している(終了確認中)
    DISP_STATE_ON_FADE_OUT,            // 表示している(フェードアウト中)
    DISP_STATE_WAIT_SAFETY,            // 表示したまま安全に破棄できるようになるを待っている(一部表示を消したりしている)
    DISP_STATE_WAIT_END,               // 表示したまま終了するのを待っている(呼び出し元が描画しないようにしたりしている)
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_UPPER,
    LYT_WK_ID_LOWER,
    LYT_WK_ID_NUM,
  };

  // 定数
  enum
  {
    CANCEL_POS = 0xFFFF,

    STR_BUF_CHAR_NUM_MAX        = 512,  // 全言語で十分大きい値
    STR_BUF_SHORT_CHAR_NUM_MAX  =  32,  // 全言語で十分大きい値
  };

private:
  void createButtonManager(void);
  void destroyButtonManager(void);

  void createPaneListForLowerMain(void);
  void destroyPaneListForLowerMain(void);

  void createPaneListForLowerSub(void);
  void destroyPaneListForLowerSub(void);

  void createMesMenuViewItems(void);
  void destroyMesMenuViewItems(void);

  void startInSeq(void);
  void startOutSeq1(bool isBack);  // DISP_STATE_ON_WAIT_START_OUTにする
  void startOutSeq2(void);  // DISP_STATE_ON_OUTにする

  void startLowerChangeSeq1(u32 pos);  // Reqから呼ぶので軽い処理
  void startLowerChangeSeq2(void);  // Updateから呼ぶので重い処理

  void startConfirm1(void);  // Reqから呼ぶので軽い処理
  void startConfirm2(void);  // Updateから呼ぶので重い処理
  void endConfirm1(bool isYes);  // Reqから呼ぶので軽い処理  // trueならYES, falseならNO
  void endConfirm2(bool isYes);  // Updateから呼ぶので重い処理

  void changeDispState(DispState dispState);

  void createPageOnce(void);
  void createPageForLowerSub(void);

  void setItem(gfl2::lyt::LytParts* itemPartsPane, u32 itemAnim, u32 styleAnim, ZukanType::Subject subject, u32 pos);
  
  void rewriteListElemsForLowerMain(void);  // resetSearchSetting時に呼び出し、リセットで変わった設定を反映する。
  void writeListElemForLowerMain(u32 pane_index, u32 pos);

  void cancelPokeIconDisplayIfChanged(void);  // 変更があったらポケアイコン表示を直ちにやめる

  void setDecideButtonEnable(void);  // 現在の状態で決定ボタンの押せる/押せないを変更する

  void startSearch(void);

private:
  // 「開始時の検索設定」を設定する
  void setSearchSubjectPaneListPosInfoOnStarting(void);
  // 検索設定をリセットする
  void resetSearchSetting(void);
  // 「表示中の検索設定」と「検索中の検索設定」を比較する
  bool isSearchSubjectPaneListPosInfoDifferent(void) const;
  // 「開始時の検索設定」と「表示中の検索設定」を比較する
  bool isSearchSubjectPaneListPosInfoChanged(void) const;

  // スレッド
  void requestSearchThreadSearch(void);
  void requestSearchThreadEnd(void);

private:
  // リスト
  enum
  {
    PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN  = 7,
    PANE_LIST_ELEM_NUM_FOR_LOWER_SUB   = 7,
  };

private:
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO_FOR_LOWER_MAIN[BUTTON_ID_NUM_FOR_LOWER_MAIN];
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO_FOR_LOWER_SUB[BUTTON_ID_NUM_FOR_LOWER_SUB];

  static const ZukanType::PaneListConstInfo     PANE_LIST_CONST_INFO_FOR_LOWER_MAIN;
  static const ZukanType::PaneListConstInfo     PANE_LIST_CONST_INFO_FOR_LOWER_SUB;
  static const ZukanType::PaneListElemConstInfo PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_MAIN[PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN];
  static const ZukanType::PaneListElemConstInfo PANE_LIST_ELEM_CONST_INFO_FOR_LOWER_SUB[PANE_LIST_ELEM_NUM_FOR_LOWER_SUB];
  static const ZukanType::ScrollBarConstInfo    SCROLL_BAR_CONST_INFO_FOR_LOWER_MAIN;
  static const ZukanType::ScrollBarConstInfo    SCROLL_BAR_CONST_INFO_FOR_LOWER_SUB;

  static u32 getSubjectMsgId(ZukanType::Subject subjectId);
  static u32 getOrderMsgId(ZukanType::OrderType orderType);
  static u32 getInitialMsgId(ZukanType::InitialType initialType, ZukanType::ZukanLang zukanLang);
  static u32 getColorMsgId(ZukanType::ColorType colorType);
  static u32 getStyleTexPatternIndex(ZukanType::StyleType styleType);
  static u32 getFigureMsgId(ZukanType::FigureType figureType);
  static u32 getGetMsgId(ZukanType::GetType getType);
  
private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  gfl2::heap::HeapBase*  m_pokeIconDisplayDevMem;
  
  ZukanViewSearchListener* m_viewListener;

  bool m_lowerIsSub;  // 下画面がサブのときtrue

  ZukanType::ButtonPaneInfo  m_buttonPaneInfoForLowerMain[BUTTON_ID_NUM_FOR_LOWER_MAIN];
  ZukanType::ButtonPaneInfo  m_buttonPaneInfoForLowerSub[BUTTON_ID_NUM_FOR_LOWER_SUB];

  app::tool::PaneListView*               m_paneListViewForLowerMain;
  ZukanSearchPaneListViewListener*       m_paneListViewListenerForLowerMain;
  ZukanSearchPaneListCallBack*           m_paneListCallBackForLowerMain;

  app::tool::PaneListView*               m_paneListViewForLowerSub;
  ZukanSearchPaneListViewListener*       m_paneListViewListenerForLowerSub;
  ZukanSearchPaneListCallBack*           m_paneListCallBackForLowerSub;

  app::tool::PaneList::PANE_DATA         m_paneListDataForLowerMain[PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN];
  app::tool::PaneList::PANE_DATA         m_paneListDataForLowerSub[PANE_LIST_ELEM_NUM_FOR_LOWER_SUB];

  ZukanType::ZukanLang            m_gameLang;  // ZUKAN_LANG_  ZUKAN_LANG_を入れる  // ゲーム開始時に選んだ言語。図鑑中に変更されることはない。

  ZukanType::PaneListPosInfo  m_paneListPosInfoForLowerMain;
  u32                         m_paneIndexPosForLowerMain[PANE_LIST_ELEM_NUM_FOR_LOWER_MAIN];  // pane_indexからposを取得できるように記憶しておく
  ZukanType::PaneListPosInfo  m_paneListPosInfoForLowerSubTmp;

  gfl2::lyt::LytParts*   m_resetButtonPartsPane;
  gfl2::lyt::LytParts*   m_decideButtonPartsPane;
  gfl2::lyt::LytTextBox* m_subjectNameTextPaneForLowerSub;

  gfl2::lyt::LytPane*  m_scrollBarTopPaneForLowerMain;
  gfl2::lyt::LytPane*  m_scrollBarTopPaneForLowerSub;

  gfl2::lyt::LytPane*  m_listStartPaneForLowerMain;
  gfl2::lyt::LytPane*  m_listStartPaneForLowerSub;

  ZukanSearchMessageMenuEventHandler*  m_mesMenuViewEventHandler;
  bool                                 m_mesMenuViewResultIsYes;
  gfl2::str::StrBuf*                   m_mesMenuViewStrBufWindowMessage;
  gfl2::str::StrBuf*                   m_mesMenuViewStrBufYes;
  gfl2::str::StrBuf*                   m_mesMenuViewStrBufNo;

  u32 m_waitAnimIndex[LYT_WK_ID_NUM];
 
  DispState m_dispState;
  u32       m_dispStateCount;
  
  ZukanSearchPokeIconDisplay*                    m_pokeIconDisplay;

private:
  // 排他制御
  gfl2::thread::CriticalSection* m_searchCriticalSection;  // m_searchIsFinishedへのアクセスを見張る。
  // 同期機構
  gfl2::thread::Event*           m_searchEvent;
  // スレッド
  ZukanSearchThread*             m_searchThread;

  // 表示中の検索設定
  ZukanType::PaneListPosInfo     m_searchSubjectPaneListPosInfoOnDisplay[ZukanType::SUBJECT_NUM];
  // 検索中の検索設定
  ZukanType::PaneListPosInfo     m_searchSubjectPaneListPosInfoOnSearching[ZukanType::SUBJECT_NUM];
  // 検索結果の「閲覧中の容姿データのインデックス」
  u8*                            m_drawFigureDataIndexList;
  // 検索結果の「表示用ポケモンデータインデックス」
  u16                            m_dispPokeDataIndexNum;
  u16*                           m_dispPokeDataIndexList;
  // 状態
  bool                           m_searchIsFinished;  // trueのとき検索が終わっている、falseのとき検索中。
  bool                           m_startPokeIconDisplay;  // trueのときポケアイコン表示を開始している。

  // 入力排他制御
  bool m_uiViewManInputExclusiveKey;

  // セーブデータが保持している閲覧中の容姿データ
  ZukanContents::SaveDrawData    m_saveDrawData[MONSNO_END];  // monsNoのポケモンのデータはsaveDrawData[monsNo-1]に入る。
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWSEARCH_H_INCLUDED

