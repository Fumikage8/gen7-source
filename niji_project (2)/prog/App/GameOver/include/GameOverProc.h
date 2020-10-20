//==============================================================================
/**
 * @file        GameOverProc.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 22:10:03
 */
//==============================================================================

#if !defined(__GAMEOVERPROC_H__)  // 重複定義防止
#define __GAMEOVERPROC_H__  // 重複定義防止マクロ
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

// インクルード
#include <App/GameOver/include/GameOverProcParam.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)
  class GameOverImpl;
GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


//==============================================================================
/**
 @class     GameOverProc
 @brief     図鑑のProc
 */
//==============================================================================
class GameOverProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(GameOverProc);

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  
  GameOverProc(GameOverProcParam* param);

  virtual ~GameOverProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

private:
  GameOverProcParam        m_param;
  gfl2::heap::HeapBase* m_devMem;
  app::util::Heap*      m_appHeap;
  GameOverImpl*            m_impl;  // create判定にも使う
};


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

#endif // __GAMEOVERPROC_H__ 重複定義防止
