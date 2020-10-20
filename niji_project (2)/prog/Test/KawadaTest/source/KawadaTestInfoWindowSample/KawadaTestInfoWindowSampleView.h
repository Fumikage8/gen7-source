//==============================================================================
/**
 @file    KawadaTestInfoWindowSampleView.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.01
 @brief   KawadaTest
 */
//==============================================================================
#if PM_DEBUG

#if !defined(NIJI_PROJECT_TEST_KAWADATESTINFOWINDOWSAMPLEVIEW_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTINFOWINDOWSAMPLEVIEW_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/gfl2_Fs.h>

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

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)
  class KawadaTestInfoWindowSampleDataManager;
  class KawadaTestInfoWindowSampleFrameCell;
  class KawadaTestInfoWindowSampleView;
GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class MsgData;
  class StrBuf;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)
 
GFL_NAMESPACE_BEGIN(print)
  class WordSet;
GFL_NAMESPACE_END(print)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestInfoWindowSampleViewListener
 @brief     サンプルのビューのリスナー
 */
//==============================================================================
class KawadaTestInfoWindowSampleViewListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestInfoWindowSampleViewListener);

public:
  KawadaTestInfoWindowSampleViewListener(KawadaTestInfoWindowSampleFrameCell* frameCell);
  virtual ~KawadaTestInfoWindowSampleViewListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:
  KawadaTestInfoWindowSampleFrameCell* m_frameCell;
};




//==============================================================================
/**
 @class     KawadaTestInfoWindowSampleView
 @brief     サンプルのビュー
 */
//==============================================================================
class KawadaTestInfoWindowSampleView : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestInfoWindowSampleView);

public:
  class Param
  {
  public:
    app::util::Heap*                    appHeap;
    GameSys::GameManager*               gameManager;
    KawadaTestInfoWindowSampleDataManager*  dataManager;
    applib::frame::Manager*             frameManager;
    app::util::AppRenderingManager*     appRenderingManager;
  public:
    Param(void)
      : appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL)
    {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  KawadaTestInfoWindowSampleView(const Param& param);
  
  virtual ~KawadaTestInfoWindowSampleView();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

public:
  //! @brief  メッセージデータを得る
  //! @param[in]  arcIdLang  print::GetMessageArcId, print::GetMessageArcId_Debug, print::GetMessageArcId_Scriptで取得したもの
  //! @param[in]  datId      GARC_message_..._DAT
  gfl2::str::MsgData* GetMsgData(gfl2::fs::ArcFile::ARCID arcIdLang, gfl2::fs::ArcFile::ARCDATID datId) const;
  //! @brief  メッセージ関連の作業に使えるオブジェクト郡を得る
  //! @param[out] wordSet
  //! @param[out] strBuf0
  //! @param[out] strBuf1
  void GetMsgWorkObject(print::WordSet** wordSet, gfl2::str::StrBuf** strBuf0, gfl2::str::StrBuf** strBuf1) const;

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param; 
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // NIJI_PROJECT_TEST_KAWADATESTINFOWINDOWSAMPLEVIEW_H_INCLUDED

#endif // PM_DEBUG
