#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWISLANDTOP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWISLANDTOP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewIslandTop.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の島図鑑トップ画面のビュー
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
  class ZukanFrameIslandTopCell;
  class ZukanViewIslandTopListener;
  class ZukanDistributionPokeDataAccessor;
  class ZukanDistributionPositionDataAccessor;
  class ZukanDistributionZoneDataAccessor;
  class ZukanSmallIslandMap;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewIslandTop
 @brief     図鑑の島図鑑トップ画面のビュー
 */
//==============================================================================
class ZukanViewIslandTop : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewIslandTop);

  friend class ZukanViewIslandTopListener;

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewIslandTop(const ZukanSceneParam& param);
  
  virtual ~ZukanViewIslandTop();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  //! @name  friendクラスZukanViewLaunchListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  enum ReqId
  {
    REQ_ID_KEY_DECIDE        ,  // reqParamは使わない
    REQ_ID_KEY_UP            ,  // reqParamはKEY_INPUT_
    REQ_ID_KEY_DOWN          ,  // reqParamはKEY_INPUT_
    REQ_ID_KEY_LEFT          ,  // reqParamはKEY_INPUT_
    REQ_ID_KEY_RIGHT         ,  // reqParamはKEY_INPUT_
    REQ_ID_BUTTON_DECIDE     ,  // reqParamはBUTTON_ID_
    REQ_ID_BUTTON_TOUCH      ,  // reqParamはBUTTON_ID_
  };

  //! @brief  キー入力の種類
  enum
  {
    KEY_INPUT_REPEAT,
    KEY_INPUT_TRIGGER,
  };

  //! @brief  ボタン
  enum
  {
    BUTTON_ID_POKE000  =   0,  // ポケモンボタンのIDを0開始にしてあるので、ポケモンボタンのIDへの変換をし忘れていても問題は起きない。
    BUTTON_ID_POKE164  = 164,
    BUTTON_ID_BACK,       

    BUTTON_ID_NUM,

    BUTTON_ID_POKE_START = BUTTON_ID_POKE000,  // START<= <=END
    BUTTON_ID_POKE_END   = BUTTON_ID_POKE164,
    BUTTON_ID_POKE_NUM   = BUTTON_ID_POKE_END - BUTTON_ID_POKE_START +1,
    BUTTON_ID_POKE_X_NUM = 15,
    BUTTON_ID_POKE_Y_NUM = 11,
  };
 
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}

private:
  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,       // 表示していない
    DISP_STATE_ON_IN,     // 表示している(イン中)
    DISP_STATE_ON_WORK,   // 表示している(稼働中)
    DISP_STATE_ON_POKE_BUTTON_DECIDE,  // 表示している(ポケモンボタン決定アニメ中)
    DISP_STATE_ON_OUT,    // 表示している(アウト中)
    DISP_STATE_WAIT_END,  // 表示したまま終了するのを待っている
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_UPPER,
    LYT_WK_ID_LOWER,
    LYT_WK_ID_UPPER_BG,
    LYT_WK_ID_NUM,
  };

private:
  void createButtonManager(void);
  void destroyButtonManager(void);

  void startInSeq(void);
  void startOutSeq(void);

  void changeDispState(DispState dispState);

  void createPage(void);
  void setCurrDispPoke(void);

  //! @param[in] playSEFlag             セットしたときにSEを鳴らすならtrue、鳴らさないならfalse。セットしなかったらSEは鳴りません。
  //! @param[in] notSetIfSameIndexFlag  m_cursorIndexとcursorIndexが同じならセットしないならtrue、同じでもセットするならfalse。
  void setCursorVector(u32 button_id, bool playSEFlag=true, bool notSetIfSameIndexFlag=true);  // button_idはポケモンボタンのIDの範囲内
  void moveCursorVector(const ZukanType::CursorVector& moveDir, bool moveIsTrigger);
  void moveCursorVectorMatrix(const ZukanType::CursorVector& moveDir, bool moveIsTrigger);

  void putCursorPane(void);  // 現在m_cursorVectorが示す位置にカーソルペインを置く

  static ZukanType::CursorVector convertButtonIdToCursorVector(u32 button_id);  // button_idはポケモンボタンのIDの範囲内
  static u32                     convertCursorVectorToButtonId(const ZukanType::CursorVector& cursorVector);  // 戻り値はポケモンボタンのIDの範囲内

private:
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO[BUTTON_ID_NUM];

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewIslandTopListener* m_viewListener;

  ZukanType::ButtonPaneInfo  m_buttonPaneInfo[BUTTON_ID_NUM];

  ZukanType::CursorVector m_cursorVector;  // ポケモンボタンを指すカーソル

  u32 m_waitAnimIndex[LYT_WK_ID_NUM];
  
  DispState m_dispState;
  u32       m_dispStateCount;

  void* m_pokeIconTexBuf[BUTTON_ID_POKE_NUM];
  
  gfl2::lyt::LytPane*    m_pokeBgPane;

  gfl2::lyt::LytTextBox* m_pokeNoTextPane;
  gfl2::lyt::LytTextBox* m_pokeNameTextPane;
  gfl2::lyt::LytTextBox* m_whereTextPane;

  gfl2::lyt::LytPane* m_cursorPane;

  bool m_isKeyDecide;          // キー入力で決定したときtrue、タッチ入力で決定したときfalse
  bool m_isMoveCursorByTouch;  // タッチ入力でカーソルが動いたときtrue

  ZukanDistributionPokeDataAccessor*     m_distributionPokeDataAccessor;
  ZukanDistributionPositionDataAccessor* m_distributionPositionDataAccessor;
  ZukanDistributionZoneDataAccessor*     m_distributionZoneDataAccessor;

  ZukanSmallIslandMap* m_smallIslandMap;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWLAUNCH_H_INCLUDED

