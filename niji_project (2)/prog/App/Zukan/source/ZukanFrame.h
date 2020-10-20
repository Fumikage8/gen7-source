#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAME_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAME_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrame.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のフレーム
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanType.h"
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

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
  class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanTool;
  //class ZukanContents;
  class ZukanFrameSceneBaseCell;
  class ZukanPokePicManager;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameListener
 @brief     図鑑のフレームのリスナー
 */
//==============================================================================
class ZukanFrameListener : public applib::frame::AppFrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFrameListener);

public:
  ZukanFrameListener(void);
  virtual ~ZukanFrameListener();

public:
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );
};




//==============================================================================
/**
 @class     ZukanFrameCell
 @brief     図鑑のフレームのセル
 */
//==============================================================================
class ZukanFrameCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFrameCell);

public:
  class Param
  {
  public:
    ZukanProcParam*                 param;                 // paramのポインタを保持するだけ。
                                                           // 呼び出し元はparamを破棄しないように。
    //gfl2::heap::HeapBase*           contentsDevMem;
    app::util::Heap*                instanceAppHeap;
    GameSys::GameManager*           gameManager;
    ZukanDataManager*               dataManager;
    applib::frame::Manager*         frameManager;
    app::util::AppRenderingManager* appRenderingManager;
    gfl2::lyt::LytSys*              lytSys;
    app::ui::UIView*                rootView;
    ZukanPokePicManager*            pokePicManager;
    ZukanTool*                      tool;
    ZukanContents*                  contents;
    ZukanType::SetupState*          setupState;
    app::tool::RotomToneGenerator*  pRotomToneGenerator;
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
    ZukanDebugInfo*                 debugInfo;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
  public:
    Param(void)
      : param(NULL),
        //contentsDevMem(NULL),
        instanceAppHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        lytSys(NULL),
        rootView(NULL),
        pokePicManager(NULL),
        tool(NULL),
        contents(NULL),
        setupState(NULL),
        pRotomToneGenerator(NULL)
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
        , debugInfo(NULL)
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
    {}
  };

public:
  ZukanFrameCell(void);
  virtual ~ZukanFrameCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);

public:
  //! @brief  引数を渡す
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  void Setup(const Param& param);

private:
  template<typename T_FRAME_SCENE_CELL>
  void callFrameScene(void)
  {
    T_FRAME_SCENE_CELL* frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, T_FRAME_SCENE_CELL>(m_param.frameManager);
    
    m_sceneParam.prevSceneType = m_prevSceneType;

    // 今コールしたフレームから戻ってきたときの状態を設定しておく
    m_prevSceneType = m_currSceneType;
    m_currSceneType = ZukanType::SCENE_TYPE_WAIT_NEXT;
    m_nextSceneType = ZukanType::SCENE_TYPE_NONE;

    frameCell->Setup(m_sceneParam);
  }

  void changeSeq(int seq);

private:
  applib::frame::Result updateFuncForCallerZukanEvent(void);
  applib::frame::Result updateFuncForCallerZukanRegisterEvent(void);
  applib::frame::Result updateFuncForCallerZukanRegisterEventAfterPokeListStatus(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // シーケンス制御
  u32   m_seqCount;

  // コンテンツ
  //ZukanContents* m_contents;

  // シーン
  ZukanSceneParam         m_sceneParam;
  ZukanType::SceneType    m_prevSceneType;
  ZukanType::SceneType    m_currSceneType;
  ZukanType::SceneType    m_nextSceneType;
  s32                     m_launchSceneCursorIndex;
  bool                    m_alolaTopSceneChangeFlag;
  u32                     m_alolaTopScenePaneListSetupParamListPos;
  f32                     m_alolaTopScenePaneListSetupParamListScroll;
  bool                    m_launchAndAlolaTopSceneDefaultFlag;  // 起動画面とアローラ図鑑画面の出入りでのみ使うフラグ
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAME_H_INCLUDED

