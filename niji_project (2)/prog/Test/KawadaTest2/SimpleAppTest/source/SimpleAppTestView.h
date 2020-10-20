#if PM_DEBUG


#if !defined(NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTVIEW_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTVIEW_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    SimpleAppTestView.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.22
 @brief   シンプルなアプリのビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>


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
  class SimpleAppTestFrameCell;
  class SimpleAppTestView;
GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)


//==============================================================================
/**
 @class     SimpleAppTestViewParam
 @brief     シンプルなアプリのビューに渡す引数
 */
//==============================================================================
class SimpleAppTestViewParam
{
public:
  app::util::Heap*                appHeap;
  GameSys::GameManager*           gameManager;
  app::util::AppRenderingManager* appRenderingManager;
  void*                           appLytDataBuf;
  SimpleAppTestFrameCell*         frameCell;

public:
  SimpleAppTestViewParam(void)
    : appHeap(NULL),
      gameManager(NULL),
      appRenderingManager(NULL),
      appLytDataBuf(NULL),
      frameCell(NULL)
  {}
};




//==============================================================================
/**
 @class     SimpleAppTestViewListener
 @brief     シンプルなアプリのビューのリスナー
 */
//==============================================================================
class SimpleAppTestViewListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(SimpleAppTestViewListener);

public:
  SimpleAppTestViewListener(SimpleAppTestView* view);
  virtual ~SimpleAppTestViewListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:
  SimpleAppTestView* m_view;
};




//==============================================================================
/**
 @class     SimpleAppTestView
 @brief     シンプルなアプリのビュー
 */
//==============================================================================
class SimpleAppTestView : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(SimpleAppTestView);

public:
  //! @brief  コンストラクタ
  //! @param[in,out]  param  呼び出し元はparamを削除しないようにして下さい。
  SimpleAppTestView(SimpleAppTestViewParam* param);
  
  virtual ~SimpleAppTestView();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

public:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void ReqEnd(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  SimpleAppTestViewParam*  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  SimpleAppTestViewListener*  m_viewListener;
};


GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)


#endif  // NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTVIEW_H_INCLUDED


#endif  // PM_DEBUG
