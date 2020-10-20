//==============================================================================
/**
 * @file        GameOverView.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 21:18:43
 */
//==============================================================================

#if !defined(__GAMEOVERVIEW_H__)  // 重複定義防止
#define __GAMEOVERVIEW_H__  // 重複定義防止マクロ
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
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)

class GameOverFrameCell;
class GameOverView;
class GameOverDataManager;


//==============================================================================
/**
 @class     GameOverViewListener
 @brief     図鑑のビューのリスナー
 */
//==============================================================================
class GameOverViewListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(GameOverViewListener);

public:
  GameOverViewListener(GameOverView* view);
  virtual ~GameOverViewListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:
  GameOverView* m_view;
};




//==============================================================================
/**
 @class     GameOverView
 @brief     図鑑のビュー
 */
//==============================================================================
class GameOverView : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(GameOverView);

public:
  class Param
  {
  public:
    GameOverProcParam*              param;                 // paramのポインタを保持するだけ。
                                                           // 呼び出し元はparamを破棄しないように。
    app::util::Heap*                appHeap;
    GameSys::GameManager*           gameManager;
    GameOverDataManager*            dataManager;
    applib::frame::Manager*         frameManager;
    app::util::AppRenderingManager* appRenderingManager;
    GameOverFrameCell*              frameCell;
  public:
    Param(void)
      : param(NULL),
        appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        frameCell(NULL)
    {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  GameOverView(const Param& param);
  
  virtual ~GameOverView();

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
  GameOverViewListener* m_viewListener;
};


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

#endif // __GAMEOVERVIEW_H__ 重複定義防止
