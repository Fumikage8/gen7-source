//======================================================================
/**
 * @file PokeViewerView.h
 * @date 2015/12/01 13:11:35
 * @author araki_syo
 * @brief ポケビューアーのView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined POKEVIEWER_VIEW_H_INCLUDED
#define POKEVIEWER_VIEW_H_INCLUDED
#pragma once

#include "AppLib/include/Ui/UIView.h"

#include "PokeViewerFrame.h"
#include "PokeViewerDataManager.h"
#include "PokeViewerListener.h"
#include "PokeViewerPokeModelManager.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(pokeviewer)

class PokeViewerFrameCell;
class PokeViewerListener;

/**
 * @class MyClass
 * @brief クラスの説明
 */
class PokeViewerView : public app::ui::UIView
{
    GFL_FORBID_COPY_AND_ASSIGN(PokeViewerView);

public:
  class Param
  {
  public:
    app::util::Heap* pHeap;
    GameSys::GameManager* pGameManager;
    PokeViewerDataManager* pDataManager;
    applib::frame::Manager* pFrameManager;
    app::util::AppRenderingManager* pAppRenderingManager;
    PokeViewerFrameCell* pFrameCell;
    PokeViewerPokeModelManager* pPokeModelManager;

  public:
    Param() : pHeap(NULL),
              pGameManager(NULL),
              pDataManager(NULL),
              pFrameManager(NULL),
              pAppRenderingManager(NULL),
              pFrameCell(NULL),
              pPokeModelManager(NULL)
    {}
  };

public:

  /**
   * @fn PokeViewerView
   * @brief ctor
   */
  PokeViewerView(const Param& param);

  /**
   * @fn ~PokeViewerView
   * @brief dtor
   */
  virtual ~PokeViewerView();

  void Initialize();
  virtual void Update();
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  void SetPage(u8 page) {m_Page = page;}

  void UpdatePage(b32 isUp);
  void UpdatePageDisp();

public: // page 1
  void UpdatePage1();

public: // page 2
  void UpdatePage2();

public: // page 3
  void UpdatePage3();

public: // page 4
  void UpdatePage4();
  void LoadPokeModel();

public: // page 5
  void UpdatePage5();

public: // page 6
  void UpdatePage6();

  void SetPicPaneCol( u32 pane , u8 r = 255, u8 g = 255, u8 b = 255, u8 a = 255);

public:
  /**
   * @fn ReqEnd
   * @brief ビューのリスナーから呼び出される終了リクエスト
   */
  void ReqEnd();

private:
  Param m_Param;
  PokeViewerListener* m_pListener;

  s8 m_Page;

  static const int MAX_PAGE = 5;
};

  GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // POKEVIEWER_VIEW_H_INCLUDED

#endif // PM_DEBUG
