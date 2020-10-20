//======================================================================
/**
 * @file    ResortDrawControl.h
 * @brief   アニメの指示を出している所
 * @author  k.ohno
 * @data    16/04/02
 */
//======================================================================

#ifndef __RESORT_DRAW_CONTROL_H__
#define __RESORT_DRAW_CONTROL_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "AppLib/include/Util/AppRenderingManager.h"

#include "PokeTool/include/PokeTool.h"

#include "Collision/include/gfl2_CollisionModel.h"

#include "App/Resort/include/ResortAppParam.h"
#include "System/include/EnvAccessor.h"

#include "ResortTouchToFlowListener.h"




GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)



GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
class BaseModel;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)


GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Util)
class AppModel;
GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(AppLib)

GFL_NAMESPACE_BEGIN(PokeTool)
class PokeModelSystem;
class PokeModel;
GFL_NAMESPACE_END(PokeTool)



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortInitLogic;  //参照



class ResortDrawControl
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortDrawControl); //コピーコンストラクタ＋代入禁止



public:
  
public:
  ResortDrawControl(); //コンストラクタ
  virtual ~ResortDrawControl(void);  //デストラクタ

  //------------------------------------------------------------------
  /**
   * @brief   初期化を行う関数
   * @return  初期化完了 true
   */
  //------------------------------------------------------------------
  bool InitFunc(void);

  //------------------------------------------------------------------
  /**
   * @brief   終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  bool EndFunc(void);

  //------------------------------------------------------------------
  /**
    * @brief   メインを行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
#if PM_DEBUG
  void UpdateFunc(int debugno);
#else
  void UpdateFunc(int debugno=0);
#endif

  //------------------------------------------------------------------
  /**
    * @brief   メインを行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
#if PM_DEBUG
  bool ControlFunc(bool bDebugCameraMode,bool bkeye);
#else
  bool ControlFunc(bool bkeye);
#endif


  void SetParam( ResortIslandDraw* pResortIslandDraw,ResortInitLogic* pLogic );

  void UpdateCameraMove( bool is_fadein, bool is_fadeout );


  void StartModelAnim(int type);

  
  private:

#if PM_DEBUG
  bool ControlCheck(bool bDebugCameraMode,bool bkeye);
#else
  bool ControlCheck(bool bkeye);
#endif

  
  int m_ModeSeq;
  int m_AnimTimer;
  int mLocalCount;
  int mBoardNo;
  u8 mBeansBuff[Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX];

  
private:
  ResortIslandDraw* mpResortIslandDraw;
  ResortInitLogic* mpLogic;
  
};  //class ResortDrawControll





GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)



#endif  //__RESORT_DRAW_CONTROL_H__
