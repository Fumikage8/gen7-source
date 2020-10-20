#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANUTIL_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANUTIL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanUtil.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.09.04
 @brief   図鑑の便利機能
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <math/include/gfl2_Vector3.h>

// nijiのインクルード
#include <AppLib/include/Tool/app_tool_PaneList.h>
#include <AppLib/include/Tool/app_tool_PaneListView.h>

// 図鑑のインクルード
#include "ZukanType.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(ui)
  class DeviceManager;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
  class LytWk;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(PokeTool)
  struct SimpleParam;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class MenuCursor;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
  class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanTool;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanUtil
 @brief     図鑑の便利機能
 */
//==============================================================================
class ZukanUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanUtil);

public:
  static void CreateButtonManager(
      app::util::Heap*                  appHeap,
      app::ui::UIView*                  uiView,
      u32                               res_i,
      u32                               wk_i,  // ZUKAN_CONST_G2DUTIL_LYT_WK_ID_NULLでもよい  // wk_iとlytWkArrayはどちらかに有効値が入っていないといけない
      u32                               buttonIdNum,
      const ZukanType::ButtonConstInfo* buttonConstInfo,  // buttonConstInfo[buttonIdNum]
      gfl2::lyt::LytWk**                lytWkArray,  // lytWkArray[buttonIdNum]  // NULLでもよい  // wk_iとlytWkArrayはどちらかに有効値が入っていないといけない
      ZukanType::ButtonPaneInfo*        buttonPaneInfo  // buttonPaneInfo[buttonIdNum]  // [out]
  );
  static void DestroyButtonManager(
      app::ui::UIView*                  uiView
  );

  static app::tool::PaneListView* CreatePaneList(  // 生成したpaneListViewを返す
      app::util::Heap*                        appHeap,
      app::ui::UIView*                        uiView,
      u32                                     res_i,
      u32                                     wk_i,
      app::tool::PaneListView::Listener*      paneListViewListener,
      u32                                     paneListElemNum,
      const ZukanType::PaneListElemConstInfo* paneListElemConstInfo,  // paneListElemConstInfo[paneListElemNum]
      app::tool::PaneList::PANE_DATA*         paneListData,  // paneListData[paneListElemNum]  // [out]
      const ZukanType::PaneListConstInfo*     paneListConstInfo,
      gfl2::lyt::LytPane*                     listStartPane,
      app::tool::PaneList::CallBack*          paneListCallBack,
      app::tool::MenuCursor*                  menuCursor,
      gfl2::ui::DeviceManager*                uiDeviceManager,
      const ZukanType::ScrollBarConstInfo*    scrollBarConstInfo,
      u32                                     paneListSetupParamValMax,
      u32                                     paneListSetupParamListPos,
      f32                                     paneListSetupParamListScroll,
      bool                                    paneListEffectFlag = false
  );
  static void DestroyPaneList(
      app::ui::UIView*                        uiView,
      app::tool::PaneListView*                paneListView
  );
  static void RemovePaneListViewFromSuperView(
      app::tool::PaneListView*                paneListView
  );
  static void MakePaneListSetupParam(
      const ZukanType::PaneListConstInfo*   paneListConstInfo,            // [in]
      u32                                   paneListSetupParamValMax,     // [in]  項目の個数
      u32                                   currVal,                      // [in]  現在の項目(0<= currVal < paneListSetupParamValMax)
      u32*                                  paneListSetupParamListPos,    // [out]
      f32*                                  paneListSetupParamListScroll  // [out]
  );

  static void* CreatePokeIconTexture(  // ポケアイコンのテクスチャのバッファを返す。GflHeapFreeMemoryで破棄して下さい。
      gfl2::heap::HeapBase*         devMem,   // デバイスメモリ
      gfl2::heap::HeapBase*         tempMem,  // 一時的に使用するメモリ。ヒープメモリでもデバイスメモリでもいい。
      gfl2::lyt::LytPicture*        pane,
      u32                           material,
      u32                           texture,
      const PokeTool::SimpleParam&  sp,
      bool                          flip,
      const ZukanDataManager*       dataManager,
      gfl2::lyt::LytResTextureInfo* a_textureInfo = NULL
  );

  // ペインのワールド座標を得る(ScaleとRotateは無視した値)
  static gfl2::math::Vector3 GetPaneWorldPosIgnoringSR(const gfl2::lyt::LytPane* pane);

  // レイアウトのアニメ
  static void StartLytWaitAnim(app::util::G2DUtil* pG2DUtil, const u32* waitAnimIndexArray, u32 waitAnimIndexArraySize);
  static bool IsLytWaitAnimEnd(app::util::G2DUtil* pG2DUtil, u32* waitAnimIndexArray, u32 waitAnimIndexArraySize);  // waitAnimIndexArrayはアニメが終わったら初期化される
  static void BindLytWaitAnim(app::util::G2DUtil* pG2DUtil, const u32* waitAnimIndexArray, u32 waitAnimIndexArraySize, f32 frame);
  static void BindLytAnim(app::util::G2DUtil* pG2DUtil, u32 lytWkId, u32 animIndex, f32 frame);

  // ボタンマネージャのボタンの表示/非表示を切り替える(表示のときは動作有効、非表示のときは動作無効にする)
  static void SetButtonVisible(app::tool::ButtonManager* buttonManager, app::tool::ButtonManager::ButtonId buttonId, bool visible);
  // このクラスのstatic関数SetButtonVisibleで設定したボタンしか正確な値を取れないので注意すること。
  static bool IsButtonVisible(const app::tool::ButtonManager* buttonManager, app::tool::ButtonManager::ButtonId buttonId);

  // ポケモンモデルの表示/非表示を切り替える(同じ状態のときは切り替え関数を呼ばない)
  static void SetPokeModelVisible(ZukanTool* tool, bool visible);
};




//==============================================================================
/**
 @class     ZukanRotomVoice
 @brief     ロトムボイス
 */
//==============================================================================
class ZukanRotomVoice
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanRotomVoice);
public:
  ZukanRotomVoice(void)
    : m_enable(false),
      m_randomPlayEnable(false),
      m_reserveFirstVoiceFlag(false),
      m_count(0)
  {}
  virtual ~ZukanRotomVoice()
  {}
  void Update(void);
  void Start(bool playFirstVoiceFlag, s32 firstCount);
  void Stop(void);
  void SetRandomPlayEnable(bool enable);
private:
  s32 getIntervalCount(void) const;
private:
  bool m_enable;
  bool m_randomPlayEnable;
  bool m_reserveFirstVoiceFlag;
  s32  m_count;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANUTIL_H_INCLUDED

