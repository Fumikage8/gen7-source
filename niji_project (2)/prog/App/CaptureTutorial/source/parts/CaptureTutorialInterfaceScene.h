//============================================================================================
/**
 * @file    IScene.h
 * @brief   捕獲デモのSceneのインターフェースクラス
 * @author  ito_isao
 * @date    12.05.25
 */
//============================================================================================
#ifndef __APP_CAPTURE_TUTORIAL_I_SCENE_H__
#define __APP_CAPTURE_TUTORIAL_I_SCENE_H__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_assert.h>

#include <gfx/include/gfl2_types.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

  template<class T>
  class CaptureTutorialInterfaceScene
  {
  public:
    virtual ~CaptureTutorialInterfaceScene(){}
  
    virtual void Enter()  = 0;
    virtual bool Update() = 0;
    virtual void Exit()   = 0;
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){}
 
    void SetOwner( T* owner )
    {
      GFL_ASSERT( owner );
      m_owner = owner;
    }

  protected:
    T* m_owner;
  };

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)

#endif //__APP_CAPTURE_TUTORIAL_I_SCENE_H__

