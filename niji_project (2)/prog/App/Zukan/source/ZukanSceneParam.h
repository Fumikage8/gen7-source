#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANSCENEPARAM_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANSCENEPARAM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanSceneParam.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.27
 @brief   図鑑のシーンに渡す引数
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanType.h"


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
GFL_NAMESPACE_BEGIN(ui)
  class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class RotomToneGenerator;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

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
  class ZukanProcParam;
  class ZukanDataManager;
  class ZukanTool;
  class ZukanContents;
  class ZukanFrameSceneBaseCell;
  class ZukanPokePicManager;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanSceneParam
 @brief     図鑑のシーンに渡す引数
 @note      ZukanSceneParamを遣り取りするときはコピーして遣り取りして下さい。
            コピーするので、コピー元はZukanSceneParamを破棄してもいいが、
            ZukanSceneParamの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
 */
//==============================================================================
class ZukanSceneParam
{
public:
  ZukanProcParam*                 param;  // paramのポインタを保持するだけ。
                                          // 呼び出し元はparamを破棄しないように。
  app::util::Heap*                appHeap;
  GameSys::GameManager*           gameManager;
  ZukanDataManager*               dataManager;
  applib::frame::Manager*         frameManager;
  app::util::AppRenderingManager* appRenderingManager;
  gfl2::lyt::LytSys*              lytSys;
  ZukanPokePicManager*            pokePicManager;
  ZukanTool*                      tool;
  ZukanContents*                  contents;
  app::ui::UIView*                rootView;
  app::ui::UIView*                parentView;
  ZukanFrameSceneBaseCell*        frameCell;
  ZukanType::SetupState*          setupState;
  ZukanType::SceneType            prevSceneType;  //!< [in]  前の画面
  ZukanType::SceneType*           nextSceneType;  //!< [out] 次の画面
  s32*                            launchSceneCursorIndex;                     //!< [in,out] 起動画面のカーソル位置
  bool*                           alolaTopSceneChangeFlag;                    //!< [in,out] アローラ図鑑トップ画面のカーソル位置
  u32*                            alolaTopScenePaneListSetupParamListPos;     //!< [in,out] アローラ図鑑トップ画面のカーソル位置
  f32*                            alolaTopScenePaneListSetupParamListScroll;  //!< [in,out] アローラ図鑑トップ画面のカーソル位置
  bool*                           launchAndAlolaTopSceneDefaultFlag;          //!< [in,out] 起動画面とアローラ図鑑画面の出入りでのみ使うフラグ  // @fix NMCat[1492] 「検索」画面に出入りするとカーソルが一番上のデフォルト位置に戻る  →アローラ図鑑のカーソル判定フラグを追加。
  app::tool::RotomToneGenerator*  pRotomToneGenerator;

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
  ZukanDebugInfo*                 debugInfo;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE

public:
  ZukanSceneParam(void)
    : param(NULL),
      appHeap(NULL),
      gameManager(NULL),
      dataManager(NULL),
      frameManager(NULL),
      appRenderingManager(NULL),
      lytSys(NULL),
      pokePicManager(NULL),
      tool(NULL),
      contents(NULL),
      rootView(NULL),  // @fix cov_ctr[10546]: 未初期化をNULLで初期化
      parentView(NULL),
      frameCell(NULL),
      setupState(NULL),
      prevSceneType(ZukanType::SCENE_TYPE_NONE),
      nextSceneType(NULL),
      launchSceneCursorIndex(NULL),
      alolaTopSceneChangeFlag(NULL),             
      alolaTopScenePaneListSetupParamListPos(NULL),
      alolaTopScenePaneListSetupParamListScroll(NULL),
      launchAndAlolaTopSceneDefaultFlag(NULL),
      pRotomToneGenerator(NULL)
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
      , debugInfo(NULL)
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
  {}
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANSCENEPARAM_H_INCLUDED
