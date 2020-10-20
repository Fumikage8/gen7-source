//======================================================================
/**
 * @file PokeViewerFrame.h
 * @date 2015/11/24 19:50:44
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined POKEVIEWER_FRAME_H_INCLUDED
#define POKEVIEWER_FRAME_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "AppLib/include/Frame/AppFrameManager.h"
#include "AppLib/include/Frame/AppFrameListener.h"
#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "PokeViewerDataManager.h"
#include "PokeViewerView.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(pokeviewer)

class PokeViewerView;
class PokeViewerPokeModelManager;

/**
 * @class PokeViewerFrameCell
 * @brief PokeViewerのフレームセル
 */
class PokeViewerFrameCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeViewerFrameCell);

public:
  class Param
  {
  public:
    //PokeViewerProcParam* m_pParam;
    app::util::Heap* pAppHeap;
    GameSys::GameManager* pGameManager;
    PokeViewerDataManager* pDataManager;
    applib::frame::Manager* pFrameManager;
    app::util::AppRenderingManager* pAppRenderingManager;
    PokeViewerPokeModelManager* pPokeModelManager;

  public:
    Param() : // m_pParam(NULL),
              pAppHeap(NULL),
              pGameManager(NULL),
              pDataManager(NULL),
              pFrameManager(NULL),
              pAppRenderingManager(NULL),
              pPokeModelManager(NULL)
    {}
  };

public:
  /**
   * @fn PokeViewerFrameCell()
   * @brief ctor
   */
  PokeViewerFrameCell();

  /**
   * @fn ~PokeViewerFrameCell()
   * @brief dtor
   */
  virtual ~PokeViewerFrameCell();

  virtual applib::frame::Result InitFunc();
  virtual applib::frame::Result UpdateFunc();
  virtual void DrawFunc(gfl2::gfx::CtrDisplayNo displayNo);
  virtual applib::frame::Result EndFunc();

public:

  /**
   * @fn Setup
   * @brief パラメータを設定する
   */
  void Setup(const Param& param);

  /**
   * @fn ReqEnd()
   * @brief Viewから呼び出される終了リクエスト
   */
  void ReqEnd();

private:
  void ChangeSeq(int seq);

private:
  Param m_Param;

  u32 m_SeqCounter;
  gfl2::math::Vector4 m_FadeColor;

  PokeViewerView* m_pView;
  b32 m_EndReq;
};

  GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // POKEVIEWER_FRAME_H_INCLUDED

#endif // PM_DEBUG
