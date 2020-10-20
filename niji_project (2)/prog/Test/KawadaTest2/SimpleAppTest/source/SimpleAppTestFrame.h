#if PM_DEBUG


#if !defined(NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTFRAME_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTFRAME_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    SimpleAppTestFrame.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.22
 @brief   シンプルなアプリのフレーム
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


// 前方宣言
GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)
  class SimpleAppTestViewParam;
  class SimpleAppTestView;
GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)


//==============================================================================
/**
 @class     SimpleAppTestFrameParam
 @brief     シンプルなアプリのフレームに渡す引数
 */
//==============================================================================
class SimpleAppTestFrameParam
{
public:
  app::util::Heap*                appHeap;
  GameSys::GameManager*           gameManager;
  app::util::AppRenderingManager* appRenderingManager;

public:
  SimpleAppTestFrameParam(void)
    : appHeap(NULL),
      gameManager(NULL),
      appRenderingManager(NULL)
  {}
};




//==============================================================================
/**
 @class     SimpleAppTestFrameListener
 @brief     シンプルなアプリのフレームのリスナー
 */
//==============================================================================
class SimpleAppTestFrameListener : public applib::frame::AppFrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN(SimpleAppTestFrameListener);

public:
  SimpleAppTestFrameListener(void);
  virtual ~SimpleAppTestFrameListener();

public:
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );
};




//==============================================================================
/**
 @class     SimpleAppTestFrameCell
 @brief     シンプルなアプリのフレームのセル
 */
//==============================================================================
class SimpleAppTestFrameCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(SimpleAppTestFrameCell);

public:
  SimpleAppTestFrameCell(void);
  virtual ~SimpleAppTestFrameCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);

public:
  //! @brief  引数を渡す
  //! @param[in,out]  param  呼び出し元はparamを削除しないようにして下さい。
  void Setup(SimpleAppTestFrameParam* param);

  //! @brief  ビューから呼び出される終了リクエスト
  void ReqEnd(void);

private:
  void startAsyncLoadData(void);
  bool isDataLoaded(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  SimpleAppTestFrameParam*  m_param;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // データ
  void*  m_appLytDataBuf;

  // ビュー
  SimpleAppTestViewParam*  m_viewParam;
  SimpleAppTestView*       m_view;
  bool                     m_endReq;
};


GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)


#endif  // NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTFRAME_H_INCLUDED


#endif  // PM_DEBUG

