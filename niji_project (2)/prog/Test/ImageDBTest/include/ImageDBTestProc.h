#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_PROC_H_INCLUDED__)
#define __IMAGEDB_TEST_PROC_H_INCLUDED__
#pragma once
//======================================================================
/**
* @file    ImageDBTestProc.h
* @date    2016/10/21 12:21:32
* @author  fukushima_yuya
* @brief   ImageDBテスト
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// niji
#include <AppLib/include/Frame/AppFrameListener.h>
#include <GameSys/include/GameProc.h>
// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//----------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------
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
// ---[ end ]---


//----------------------------------------------------------------------
// @brief   ImageDBテスト
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(ImageDB)

class Frame;

class Proc
  : public applib::frame::AppFrameListener
  , public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( Proc );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  Proc( void );
  virtual ~Proc( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* manager);
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* manager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* manager);
  virtual void               PreDrawFunc(gfl2::proc::Manager* manager);
  virtual void               DrawFunc(gfl2::proc::Manager* manager, gfl2::gfx::CtrDisplayNo no);

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );

private:
  //-------------------------------------------------------
  // @brief   生成関数群
  //-------------------------------------------------------
  void CreateFrame( void );

public:
  //-------------------------------------------------------
  // @brief   取得関数群
  //-------------------------------------------------------
  applib::frame::Manager* GetFrameManager(void) const { return m_pFrameManager; }

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;
  applib::frame::Manager*           m_pFrameManager;
  gfl2::heap::HeapBase*             m_pManagerHeap;

  Frame*   m_pFrame;

private:
  u8    m_Seq;
};

GFL_NAMESPACE_END(ImageDB)
GFL_NAMESPACE_END(Test)

#endif  // __IMAGEDB_TEST_PROC_H_INCLUDED__

#endif  // #if PM_DEBUG
