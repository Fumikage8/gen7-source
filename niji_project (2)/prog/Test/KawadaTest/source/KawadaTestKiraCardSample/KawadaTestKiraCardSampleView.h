//==============================================================================
/**
 @file    KawadaTestKiraCardSampleView.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.23
 @brief   KawadaTest
 */
//==============================================================================
#if PM_DEBUG

#if !defined(NIJI_PROJECT_TEST_KAWADATESTKIRACARDSAMPLEVIEW_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTKIRACARDSAMPLEVIEW_H_INCLUDED
#pragma once

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
  class RenderTargetTexture;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)
  class KawadaTestKiraCardSampleDataManager;
  class KawadaTestKiraCardSampleFrameCell;
  class KawadaTestKiraCardSampleView;
GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestKiraCardSampleViewListener
 @brief     サンプルのビューのリスナー
 */
//==============================================================================
class KawadaTestKiraCardSampleViewListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestKiraCardSampleViewListener);

public:
  KawadaTestKiraCardSampleViewListener(KawadaTestKiraCardSampleView* view);
  virtual ~KawadaTestKiraCardSampleViewListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:
  KawadaTestKiraCardSampleView* m_view;
};




//==============================================================================
/**
 @class     KawadaTestKiraCardSampleView
 @brief     サンプルのビュー
 */
//==============================================================================
class KawadaTestKiraCardSampleView : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestKiraCardSampleView);

public:
  class Param
  {
  public:
    app::util::Heap*                    appHeap;
    GameSys::GameManager*               gameManager;
    KawadaTestKiraCardSampleDataManager*  dataManager;
    applib::frame::Manager*             frameManager;
    app::util::AppRenderingManager*     appRenderingManager;
    KawadaTestKiraCardSampleFrameCell*    frameCell;
    void*                               texPhysicalAddr;
    app::util::RenderTargetTexture*     renderTargetTexture;
  public:
    Param(void)
      : appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        frameCell(NULL),
        texPhysicalAddr(NULL),
        renderTargetTexture(NULL)
    {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  KawadaTestKiraCardSampleView(const Param& param);
  
  virtual ~KawadaTestKiraCardSampleView();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

public:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void ReqEnd(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  KawadaTestKiraCardSampleViewListener* m_viewListener;

  // ペイン増殖
  static const u32 PRISM_BLOCK_ROW_NUM =  9;
  static const u32 PRISM_BLOCK_COL_NUM = 13;
  enum
  {
    PRISM_PIECE_LT,
    PRISM_PIECE_RT,
    PRISM_PIECE_LB,
    PRISM_PIECE_RB,
    PRISM_PIECE_NUM,
  };
  class PrismBlock
  {
  public:
    gfl2::lyt::LytPane*    blockPane;
    gfl2::lyt::LytPicture* piecePicPane[PRISM_PIECE_NUM];
  public:
    PrismBlock(void)
      : blockPane(NULL)
    {
      for(s32 i=0; i<PRISM_PIECE_NUM; ++i)
      {
        piecePicPane[i] = NULL;
      }
    }
  };
  PrismBlock m_prismBlock[PRISM_BLOCK_ROW_NUM][PRISM_BLOCK_COL_NUM];
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // NIJI_PROJECT_TEST_KAWADATESTKIRACARDSAMPLEVIEW_H_INCLUDED

#endif // PM_DEBUG
