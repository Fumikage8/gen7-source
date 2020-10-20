#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWLAUNCH_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWLAUNCH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewLaunch.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の起動画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>

// 図鑑のインクルード
#include "ZukanType.h"
#include "ZukanSceneParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class StrBuf;
GFL_NAMESPACE_END(Str)
GFL_NAMESPACE_END(gfl2)

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
  class ZukanRotomVoice;
  class ZukanFrameLaunchCell;
  class ZukanViewLaunchListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewLaunch
 @brief     図鑑の起動画面のビュー
 */
//==============================================================================
class ZukanViewLaunch : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewLaunch);

  friend class ZukanViewLaunchListener;

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewLaunch(const ZukanSceneParam& param);
  
  virtual ~ZukanViewLaunch();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  //! @name  friendクラスZukanViewLaunchListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出されるリクエスト
  enum ReqId
  {
    REQ_ID_KEY_DECIDE        ,  // reqParamは使わない
    REQ_ID_KEY_TRIGGER_UP    ,  // reqParamは使わない
    REQ_ID_KEY_TRIGGER_DOWN  ,  // reqParamは使わない
    REQ_ID_KEY_REPEAT_UP     ,  // reqParamは使わない
    REQ_ID_KEY_REPEAT_DOWN   ,  // reqParamは使わない
    REQ_ID_BUTTON_DECIDE     ,  // reqParamはBUTTON_ID_
    REQ_ID_BUTTON_TOUCH      ,  // reqParamはBUTTON_ID_
  };

  //! @brief  ボタン
  enum
  {
    // この順番大事です。変えないで下さい。
    BUTTON_ID_ALOLA_TOP,  // ZukanType::InfoTypeと順番を合わせてある
    BUTTON_ID_ISLAND1_TOP,
    BUTTON_ID_ISLAND2_TOP,
    BUTTON_ID_ISLAND3_TOP,
    BUTTON_ID_ISLAND4_TOP,
    BUTTON_ID_ADMIRE,
    BUTTON_ID_BACK,       

    BUTTON_ID_NUM,

    BUTTON_ID_LIST_ELEM_START          = BUTTON_ID_ALOLA_TOP,  // BUTTON_ID_LIST_START<= <=BUTTON_ID_LIST_END
    BUTTON_ID_LIST_ELEM_END            = BUTTON_ID_ADMIRE,
    BUTTON_ID_LIST_ELEM_NUM            = BUTTON_ID_LIST_ELEM_END - BUTTON_ID_LIST_ELEM_START +1,
    BUTTON_ID_LIST_ELEM_END_FOR_ADMIRE = BUTTON_ID_ISLAND4_TOP,
    BUTTON_ID_LIST_ELEM_NUM_FOR_ADMIRE = BUTTON_ID_LIST_ELEM_END_FOR_ADMIRE - BUTTON_ID_LIST_ELEM_START +1,

    BUTTON_ID_LIST_ELEM_ZUKAN_START = BUTTON_ID_ALOLA_TOP,    // START<= <=END
    BUTTON_ID_LIST_ELEM_ZUKAN_END   = BUTTON_ID_ISLAND4_TOP,
    BUTTON_ID_LIST_ELEM_OTHER_START = BUTTON_ID_ADMIRE,       // START<= <=END
    BUTTON_ID_LIST_ELEM_OTHER_END   = BUTTON_ID_ADMIRE,

    BUTTON_ID_LIST_ELEM_ISLAND_START = BUTTON_ID_ISLAND1_TOP,  // START<= <=END
    BUTTON_ID_LIST_ELEM_ISLAND_END   = BUTTON_ID_ISLAND4_TOP,
    BUTTON_ID_LIST_ELEM_ISLAND_NUM   = BUTTON_ID_LIST_ELEM_ISLAND_END - BUTTON_ID_LIST_ELEM_ISLAND_START +1,
  };

  //! @brief  ビューのリスナーから呼び出されるリクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}

private:
  // 表示状態
  enum DispState
  {
    // 起動画面
    DISP_STATE_OFF,                  // 表示していない
    DISP_STATE_ON_FADE_IN,           // 表示している(フェードイン中)
    DISP_STATE_ON_FADE_IN_ROTOM_IN,  // 表示している(フェードイン中ロトムイン中)
    DISP_STATE_ON_ROTOM_IN,          // 表示している(ロトムイン中)
    DISP_STATE_ON_IN0,               // 表示している(イン中0/2)
    DISP_STATE_ON_IN1,               // 表示している(イン中1/2)
    DISP_STATE_ON_IN_FROM_ADMIRE,    // 表示している(「博士の評価」画面からのイン)
    DISP_STATE_ON_WORK,              // 表示している(稼働中)
    DISP_STATE_ON_OUT,               // 表示している(アウト中)
    DISP_STATE_ON_FADE_OUT,          // 表示している(フェードアウト中)
    DISP_STATE_WAIT_SAFETY,          // 表示したまま安全に破棄できるようになるを待っている(一部表示を消したりしている)
    DISP_STATE_WAIT_END,             // 表示したまま終了するのを待っている(呼び出し元が描画しないようにしたりしている)

    // 「博士の評価」画面
    DISP_STATE_ADMIRE_OFF,              // 表示していない
    DISP_STATE_ADMIRE_ON_IN,            // 表示している(イン中)
    DISP_STATE_ADMIRE_ON_SELECT_START,  // 表示している(図鑑選択開始)
    DISP_STATE_ADMIRE_ON_SELECT_MES,    // 表示している(図鑑選択メッセージ表示)
    DISP_STATE_ADMIRE_ON_SELECT_WAIT,   // 表示している(図鑑選択ユーザ入力待ち)
    DISP_STATE_ADMIRE_ON_RESULT_MES,    // 表示している(評価メッセージ表示＆ユーザ入力待ち)
    DISP_STATE_ADMIRE_ON_OUT,           // 表示している(アウト中)
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_LOWER,
    LYT_WK_ID_UPPER,
    LYT_WK_ID_ROTOM_LOWER,
    LYT_WK_ID_ROTOM_UPPER,
    LYT_WK_ID_NUM,
  };

  enum
  {
    UPPER_LOOP_ANIM_NEW_LOOP,
    UPPER_LOOP_ANIM_NUM
  };

  enum
  {
    STR_BUF_CHAR_NUM_MAX = 1024,  // 全言語で十分大きい値
  };

private:
  //! @param[in] playSEFlag             セットしたときにSEを鳴らすならtrue、鳴らさないならfalse。セットしなかったらSEは鳴りません。
  //! @param[in] notSetIfSameIndexFlag  m_cursorIndexとcursorIndexが同じならセットしないならtrue、同じでもセットするならfalse。
  void setCursorIndexAndPutMenuCursor(s32 cursorIndex, bool playSEFlag=true, bool notSetIfSameIndexFlag=true);
  s32 getUpCursorIndex(void);
  s32 getDownCursorIndex(void);
  bool isCursorIndexAtTop(void);
  bool isCursorIndexAtBottom(void);

  void createButtonManager(void);
  void destroyButtonManager(void);

  void startInSeq(void);
  void startOutSeq(void);

  void startAdmireInSeq(void);
  void startAdmireOutSeq(void);
  void startAdmireResultSeq(s32 buttonId);

  void getAdmireResultId(ZukanType::InfoType infoType, u32* admireResultMsgStrID, u32* admireResultMEID) const;

  void setParamWhenOutSeq(ZukanType::InfoType infoType, ZukanType::SceneType nextSceneType);

  void changeDispState(DispState dispState);

  void createPage(void);

  void setupButtonOpen(void);

  void changeMode(bool modeIsAdmire);
  void updateForLaunch(void);
  void updateForAdmire(void);

private:
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO[BUTTON_ID_NUM];

  struct OffPlate
  {
    gfl2::lyt::LytPane* pane;
    u32                 activeAnim;
    u32                 passiveAnim;
  };

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  bool m_modeIsAdmire;

  ZukanViewLaunchListener* m_viewListener;

  ZukanType::ButtonPaneInfo m_buttonPaneInfo[BUTTON_ID_NUM];
  
  s32 m_cursorIndex;  // 計算過程で一時的に負にも成り得るのでs32で。

  OffPlate  m_offPlate[BUTTON_ID_LIST_ELEM_ISLAND_NUM];

  u32 m_waitAnimIndex[LYT_WK_ID_NUM];
  bool m_fadeOutFlag;

  u32                  m_upperLoopAnimIndex[UPPER_LOOP_ANIM_NUM];

  gfl2::lyt::LytPane*    m_mesWinPane;
  gfl2::lyt::LytTextBox* m_mesWinTextPane[2];
  gfl2::lyt::LytPane*    m_mesCursorPane;
  gfl2::str::StrBuf*     m_strBuf;

  bool m_bgmChanged;

  bool m_rotomFadeFlag;

  ZukanRotomVoice*  m_rotomVoice;

  ZukanType::InfoType  m_nextInfoType;

  DispState m_dispState;
  u32       m_dispStateCount;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWLAUNCH_H_INCLUDED

