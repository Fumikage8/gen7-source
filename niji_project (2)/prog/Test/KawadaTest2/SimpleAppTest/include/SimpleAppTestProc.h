#if PM_DEBUG


#if !defined(NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTPROC_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTPROC_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    SimpleAppTestProc.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.22
 @brief   シンプルなアプリのProc
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>

// 本アプリのインクルード
#include <Test/KawadaTest2/SimpleAppTest/include/SimpleAppTestProcParam.h>


// 前方宣言
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

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)
  class SimpleAppTestFrameListener;
  class SimpleAppTestFrameParam;
  class SimpleAppTestFrameCell;
GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)


//==============================================================================
/**
 @class     SimpleAppTestProc
 @brief     シンプルなアプリのProc
 */
//==============================================================================
class SimpleAppTestProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(SimpleAppTestProc);

public:
  //! @brief  コンストラクタ
  //! @param[in,out]  param  呼び出し元はparamを削除しないようにして下さい。
  SimpleAppTestProc(SimpleAppTestProcParam* param);

  virtual ~SimpleAppTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  SimpleAppTestProcParam*  m_param;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // ヒープ 
  gfl2::heap::HeapBase*    m_devMem;
  gfl2::heap::HeapBase*    m_frameManagerDevMem;
  app::util::Heap*         m_appHeap;

  // レンダリングマネージャ
  app::util::AppRenderingManager* m_appRenderingManager;

  // フレーム
  applib::frame::Manager*     m_frameManager;
  SimpleAppTestFrameListener* m_frameListener;
  SimpleAppTestFrameParam*    m_frameParam;
  SimpleAppTestFrameCell*     m_frameCell;
};


GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)


#endif  // NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTPROC_H_INCLUDED


#endif  // PM_DEBUG

