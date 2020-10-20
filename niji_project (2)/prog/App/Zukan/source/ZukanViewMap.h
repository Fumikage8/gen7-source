#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWMAP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWMAP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewMap.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の分布画面のビュー
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

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)
  class TownMapPartsDraw;
  //struct TownMapPartsDraw::EX_ICON_DATA;  // warning : declaration does not declare anything
GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanPokeIconTextureManager;
  class ZukanDistributionPokeDataAccessor;
  class ZukanDistributionPositionDataAccessor;
  class ZukanDistributionZoneDataAccessor;
  class ZukanFrameMapCell;
  class ZukanViewMapListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewMap
 @brief     図鑑の分布画面のビュー
 */
//==============================================================================
class ZukanViewMap : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewMap);

  friend class ZukanViewMapListener;

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewMap(const ZukanSceneParam& param);
  
  virtual ~ZukanViewMap();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

public:
  //! @brief  タウンマップのビューを設定する
  void SetTownMapPartsDraw(App::TownMap::TownMapPartsDraw* townMapPartsDraw);
  //! @brief  TownMapPartsDrawのリスナーから呼ばれる。島変更ボタンの表示非表示を切り替える。
  void SetIslandButtonVisible(bool isVisible);
  //! @brief  TownMapPartsDrawのリスナーから呼ばれる。アイコンを設定する。小さなマップ(=全体を一望できるマップ)のときisZoomにfalseを渡す。
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  void SetTownMapPartsDrawIconData(bool isZoom, s32 mapIconCheckMode);
#endif
  void SetTownMapPartsDrawIconData(bool isZoom);
  //! @brief  外部から呼び出される終了リクエスト
  void ReqEnd(void);

  //! @brief  最初に表示する島番号(0始まり)を取得する
  static s32 GetFirstIslandNo(const ZukanContents* contents);

  //! @name  friendクラスZukanViewLaunchListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出されるリクエスト
  enum ReqId
  {
    /*
    REQ_ID_KEY_TRIGGER_LEFT  ,  // reqParamは使わない
    REQ_ID_KEY_TRIGGER_RIGHT ,  // reqParamは使わない
    REQ_ID_KEY_REPEAT_LEFT   ,  // reqParamは使わない
    REQ_ID_KEY_REPEAT_RIGHT  ,  // reqParamは使わない
    */
    REQ_ID_KEY_TRIGGER_X     ,  // reqParamは使わない
    
    REQ_ID_BUTTON_DECIDE     ,  // reqParamはBUTTON_ID_
  };

  //! @brief  ボタン
  enum
  {
    // 使っているのでこの順番、値を変更しないように。
    BUTTON_ID_ISLAND1,  // 0始まり
    BUTTON_ID_ISLAND2,
    BUTTON_ID_ISLAND3,
    BUTTON_ID_ISLAND4,

    BUTTON_ID_PREV_MONS,
    BUTTON_ID_NEXT_MONS,

    BUTTON_ID_NUM,
  };

  //! @brief  ビューのリスナーから呼び出されるリクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}

private:
  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,       // 表示していない
    DISP_STATE_ON_IN,     // 表示している(イン中)
    DISP_STATE_ON_WORK,   // 表示している(稼働中)
    DISP_STATE_ON_OUT,    // 表示している(アウト中)
    DISP_STATE_WAIT_END,  // 表示したまま終了するのを待っている(呼び出し元が描画しないようにしたりしている)
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_UPPER,
    LYT_WK_ID_LOWER,
    LYT_WK_ID_NUM,
  };

  enum
  {
    TOWN_MAP_ICON_DATA_NUM_MAX = 40
  };

private:
  void createButtonManager(void);
  void destroyButtonManager(void);

  void createPage(void);
  void setCurrDispPoke(void);  // ポケモンが変わったとき(この中からsetCurrDispIslandも呼ぶ)
  void setCurrDispIsland(void);  // 島が変わったとき

  void startInSeq(void);
  void startOutSeq(void);
  
  void changeDispState(DispState dispState);

private:
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO[BUTTON_ID_NUM];

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param;

  App::TownMap::TownMapPartsDraw*  m_townMapPartsDraw;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewMapListener* m_viewListener;

  ZukanType::ButtonPaneInfo m_buttonPaneInfo[BUTTON_ID_NUM];

  gfl2::str::StrBuf*  m_placeNameStrBuf;
 
  gfl2::lyt::LytTextBox* m_islandNameTextPane;
  //gfl2::lyt::LytTextBox* m_placeNameTextPane;  // 地名表示なくなりました
  gfl2::lyt::LytTextBox* m_pokeNoTextPane;
  gfl2::lyt::LytTextBox* m_pokeNameTextPane;
  gfl2::lyt::LytTextBox* m_unknownHabitatTextPane;
  
  gfl2::lyt::LytPane* m_xButtonPane;

  gfl2::lyt::LytPicture*        m_pokeIconPicPane;
  ZukanPokeIconTextureManager*  m_pokeIconTexManager;

  s32 m_islandId;  // BUTTON_ID_ISLAND番号を使う。0は始まり。計算過程で一時的に負の値をなることがある。

  ZukanDistributionPokeDataAccessor*     m_distributionPokeDataAccessor;
  ZukanDistributionPositionDataAccessor* m_distributionPositionDataAccessor;
  ZukanDistributionZoneDataAccessor*     m_distributionZoneDataAccessor;

  App::TownMap::TownMapPartsDraw::EX_ICON_DATA*  m_townMapIconData;      // m_townMapIconData[TOWN_MAP_ICON_DATA_NUM_MAX]
  App::TownMap::TownMapPartsDraw::EX_ICON_DATA*  m_townMapIconDataPtr;
  u32                                            m_townMapIconDataNum;

  u32 m_waitAnimIndex[LYT_WK_ID_NUM];
  
  DispState m_dispState;
  u32       m_dispStateCount;

  // @fix NMCat[2109] 島を切り替えた直後、エンカウントマークが一瞬異なる位置に表示される  →一瞬違うアイコンが表示されるので、ResetIconをResetIcon(アニメを1フレーム進める)とResetIconPosition(表示する)に分けてもらった。
  bool m_resetIconPositionCallFlag;
      // m_resetIconPositionCallFlagがtrueのとき、ResetIconPositionを呼んで自前で表示する。
      // m_resetIconPositionCallFlagがtrueになるのはResetIconを呼んだとき。
      // m_resetIconPositionCallFlagがfalseになるのはResetIconPositionを呼んだとき、もしくは、
      //     SetTownMapPartsDrawIconDataを呼んだとき(TownMapPartsDrawがアクション終了時(ACTION_ID_ZOOM_END, ACTION_ID_LAND_CHANGE_END)にResetIcon,ResetIconPosition相当のことをしてくれるので、
      //     SetTownMapPartsDrawIconDataを呼んだ後は「自前で表示するか」「TownMapPartsDrawのアクション終了時に表示してもらうか」判別が付かないため、falseにしておいて呼び出し元に判断を任せる)。

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  u32  m_debugIconDividedNo;
  bool m_debugIconDividedAnimStop;  // ズームのときのコマ送り
#endif
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWMAP_H_INCLUDED

