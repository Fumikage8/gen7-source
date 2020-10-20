//==============================================================================
/**
 * @file        GameOverFrame.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 21:18:08
 */
//==============================================================================

#if !defined(__GAMEOVERFRAME_H__) // 重複定義防止
#define __GAMEOVERFRAME_H__ // 重複定義防止マクロ
#pragma once


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

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)
  class GameOverView;
GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)
  class GameOverDataManager;
GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)



GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


//==============================================================================
/**
 @class     GameOverFrameListener
 @brief     図鑑のフレームのリスナー
 */
//==============================================================================
class GameOverFrameListener : public applib::frame::AppFrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN(GameOverFrameListener);

public:
  GameOverFrameListener(void);
  virtual ~GameOverFrameListener();

public:
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );
};




//==============================================================================
/**
 @class     GameOverFrameCell
 @brief     図鑑のフレームのセル
 */
//==============================================================================
class GameOverFrameCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(GameOverFrameCell);

public:
  class Param
  {
  public:
    GameOverProcParam*                 param;                 // paramのポインタを保持するだけ。
                                                           // 呼び出し元はparamを破棄しないように。
    app::util::Heap*                appHeap;
    GameSys::GameManager*           gameManager;
    GameOverDataManager*               dataManager;
    applib::frame::Manager*         frameManager;
    app::util::AppRenderingManager* appRenderingManager;
  public:
    Param(void)
      : param(NULL),
        appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL)
    {}
  };

public:
  GameOverFrameCell(void);
  virtual ~GameOverFrameCell();

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
  GameOverView*         m_view;
  bool               m_endReq;


};


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

#endif // __GAMEOVERFRAME_H__ 重複定義防止
