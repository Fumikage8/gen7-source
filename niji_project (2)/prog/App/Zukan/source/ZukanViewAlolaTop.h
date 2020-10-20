#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWALOLATOP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWALOLATOP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewAlolaTop.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑のアローラ図鑑トップ画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Tool/app_tool_PaneListView.h>

// 図鑑のインクルード
#include "ZukanDefine.h"

#include "ZukanSceneParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
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
  class ZukanPicScrollDataAccessor;
  class ZukanPicScrollBook;
  class ZukanPokeIconTextureManager;
  class ZukanFrameAlolaTopCell;
  class ZukanViewAlolaTopListener;
  class ZukanAlolaTopPaneListViewListener;
  class ZukanAlolaTopPaneListCallBack;
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  class ZukanPrismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewAlolaTop
 @brief     図鑑のアローラ図鑑トップ画面のビュー
 */
//==============================================================================
class ZukanViewAlolaTop : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewAlolaTop);

  friend class ZukanViewAlolaTopListener;
  friend class ZukanAlolaTopPaneListViewListener;
  friend class ZukanAlolaTopPaneListCallBack;

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewAlolaTop(const ZukanSceneParam& param);
  
  virtual ~ZukanViewAlolaTop();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);


  //! @name  friendクラスZukanViewLaunchListener, ZukanAlolaTopPaneListViewListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  enum ReqId
  {
    //REQ_ID_KEY_DECIDE        ,  // reqParamは使わない
    REQ_ID_BUTTON_DECIDE     ,  // reqParamはBUTTON_ID_
    REQ_ID_BUTTON_TOUCH      ,  // reqParamはBUTTON_ID_
    REQ_ID_PANE_LIST_DECIDE  ,  // reqParamはリストのインデックス
  };

  //! @brief  ボタン
  enum
  {
    BUTTON_ID_SEARCH,
    BUTTON_ID_BACK,       

    BUTTON_ID_NUM
  };
 
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}


  //! @name  friendクラスZukanAlolaTopPaneListCallBackからアクセスするメンバ
  //@{
private:
  void DoAtCursorMoving( const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
  void WriteList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  //@}


private:
  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,          // 表示していない
    DISP_STATE_ON_IN,        // 表示している(イン中)
    DISP_STATE_ON_WORK,      // 表示している(稼働中)
    DISP_STATE_ON_OUT,       // 表示している(アウト中)
    DISP_STATE_WAIT_SAFETY,  // 表示したまま安全に破棄できるようになるを待っている(一部表示を消したりしている)
    DISP_STATE_WAIT_END,     // 表示したまま終了するのを待っている(呼び出し元が描画しないようにしたりしている)
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_UPPER_0,
    LYT_WK_ID_UPPER_1,
    LYT_WK_ID_UPPER_2,
    LYT_WK_ID_UPPER_SEARCH,
    LYT_WK_ID_LOWER,
    LYT_WK_ID_UPPER_CURTAIN,
    LYT_WK_ID_UPPER_BACKMOST,
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    LYT_WK_ID_UPPER_PRISM_SHEET,
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
    LYT_WK_ID_NUM,
  };

private:
  void createButtonManager(void);
  void destroyButtonManager(void);

  void createPaneList(void);
  void destroyPaneList(void);

  void startInSeq(void);
  void startOutSeq(void);

  void changeDispState(DispState dispState);

  void createPage(void);

private:
  // リスト
  enum
  {
    PANE_LIST_ELEM_NUM_FOR_ID    = 8,
    PANE_LIST_ELEM_NUM_FOR_SPEC  = 6,

    PANE_LIST_ELEM_NUM_MAX = PANE_LIST_ELEM_NUM_FOR_ID,  // 大きいほう
  };

private:
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO_FOR_ID[BUTTON_ID_NUM];
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO_FOR_SPEC[BUTTON_ID_NUM];

  static const ZukanType::PaneListConstInfo     PANE_LIST_CONST_INFO_FOR_ID;
  static const ZukanType::PaneListConstInfo     PANE_LIST_CONST_INFO_FOR_SPEC;
  static const ZukanType::PaneListElemConstInfo PANE_LIST_ELEM_CONST_INFO_FOR_ID[PANE_LIST_ELEM_NUM_FOR_ID];
  static const ZukanType::PaneListElemConstInfo PANE_LIST_ELEM_CONST_INFO_FOR_SPEC[PANE_LIST_ELEM_NUM_FOR_SPEC];
  static const ZukanType::ScrollBarConstInfo    SCROLL_BAR_CONST_INFO_FOR_ID;
  static const ZukanType::ScrollBarConstInfo    SCROLL_BAR_CONST_INFO_FOR_SPEC;

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewAlolaTopListener* m_viewListener;
  
  bool m_orderTypeIsForId;  // 並びがNOかNAMEならtrue(個体を識別するもので並べるならtrue)
  
  ZukanType::ButtonPaneInfo  m_buttonPaneInfoForId[BUTTON_ID_NUM];
  ZukanType::ButtonPaneInfo  m_buttonPaneInfoForSpec[BUTTON_ID_NUM];
  ZukanType::ButtonPaneInfo* m_buttonPaneInfo;

  app::tool::PaneListView*               m_paneListView;
  ZukanAlolaTopPaneListViewListener*     m_paneListViewListener;
  ZukanAlolaTopPaneListCallBack*         m_paneListCallBack;
  app::tool::PaneList::PANE_DATA         m_paneListDataForId[PANE_LIST_ELEM_NUM_FOR_ID];
  app::tool::PaneList::PANE_DATA         m_paneListDataForSpec[PANE_LIST_ELEM_NUM_FOR_SPEC];
  u32                                    m_paneListElemNum;
  app::tool::PaneList::PANE_DATA*        m_paneListData;

  u32 m_paneListElemButtonSeIdForId[PANE_LIST_ELEM_NUM_FOR_ID];
  u32 m_paneListElemButtonSeIdForSpec[PANE_LIST_ELEM_NUM_FOR_SPEC];

  u32 m_waitAnimIndex[LYT_WK_ID_NUM];

  gfl2::lyt::LytPane*     m_searchTopPane;
  gfl2::lyt::LytTextBox*  m_searchOrderTextPane;
  gfl2::lyt::LytTextBox*  m_searchCountTextPane;
  
  gfl2::lyt::LytPane*  m_scrollBarTopPane;
  gfl2::lyt::LytPane*  m_listStartPane;

  DispState m_dispState;
  u32       m_dispStateCount;

  ZukanPokeIconTextureManager*  m_pokeIconTexManager;

  ZukanPicScrollDataAccessor* m_picScrollDataAccessor;
  ZukanPicScrollBook*         m_picScrollBook;

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  ZukanPrismSheet*   m_prismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

  // 入力排他制御
  bool m_uiViewManInputExclusiveKey;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWALOLATOP_H_INCLUDED

