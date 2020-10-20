#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWBG_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWBG_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewBg.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の背景のビュー
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
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

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

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanProcParam;
  class ZukanDataManager;
  class ZukanFrameRegisterCell;
  class ZukanViewBgListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewBg
 @brief     図鑑の背景のビュー
 */
//==============================================================================
class ZukanViewBg : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewBg);

  friend class ZukanViewBgListener;

public:
  class Param
  {
  public:
    ZukanProcParam*                 param;                 // paramのポインタを保持するだけ。
                                                           // 呼び出し元はparamを破棄しないように。
    app::util::Heap*                appHeap;
    GameSys::GameManager*           gameManager;
    ZukanDataManager*               dataManager;
    applib::frame::Manager*         frameManager;
    app::util::AppRenderingManager* appRenderingManager;
    gfl2::lyt::LytSys*              lytSys;
    app::ui::UIView*                parentView;
  public:
    Param(void)
      : param(NULL),
        appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        lytSys(NULL),
        parentView(NULL)
    {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewBg(const Param& param);
  
  virtual ~ZukanViewBg();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewBgListener* m_viewListener;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWBG_H_INCLUDED

