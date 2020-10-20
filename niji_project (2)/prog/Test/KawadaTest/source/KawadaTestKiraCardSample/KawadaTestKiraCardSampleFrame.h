#if !defined(NIJI_PROJECT_TEST_KAWADATESTKIRACARDSAMPLEFRAME_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTKIRACARDSAMPLEFRAME_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestKiraCardSampleFrame.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.23
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


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
  class RenderTargetTexture;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)
  class KawadaTestKiraCardSampleDataManager;
  class KawadaTestKiraCardSampleView;
GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestKiraCardSampleFrameListener
 @brief     サンプルのフレームのリスナー
 */
//==============================================================================
class KawadaTestKiraCardSampleFrameListener : public applib::frame::AppFrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestKiraCardSampleFrameListener);

public:
  KawadaTestKiraCardSampleFrameListener(void);
  virtual ~KawadaTestKiraCardSampleFrameListener();

public:
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );
};




//==============================================================================
/**
 @class     KawadaTestKiraCardSampleFrameCell
 @brief     サンプルのフレームのセル
 */
//==============================================================================
class KawadaTestKiraCardSampleFrameCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestKiraCardSampleFrameCell);

public:
  class Param
  {
  public:
    app::util::Heap*                   appHeap;
    GameSys::GameManager*              gameManager;
    KawadaTestKiraCardSampleDataManager* dataManager;
    applib::frame::Manager*            frameManager;
    app::util::AppRenderingManager*    appRenderingManager;
    void*                              texPhysicalAddr;
    app::util::RenderTargetTexture*    renderTargetTexture;
  public:
    Param(void)
      : appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        texPhysicalAddr(NULL),
        renderTargetTexture(NULL)
    {}
  };

public:
  KawadaTestKiraCardSampleFrameCell(void);
  virtual ~KawadaTestKiraCardSampleFrameCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);
 
public:
  //! @brief  引数を渡す
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  void Setup(const Param& param);

  //! @brief  ビューから呼び出される終了リクエスト
  void ReqEnd(void);

private:
  void changeSeq(int seq);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // フェード色
  gfl2::math::Vector4  m_fadeColor;

  // seq
  u32   m_seqCount;

  // ビュー
  KawadaTestKiraCardSampleView*  m_view;
  bool                         m_endReq;
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTKIRACARDSAMPLEFRAME_H_INCLUDED

