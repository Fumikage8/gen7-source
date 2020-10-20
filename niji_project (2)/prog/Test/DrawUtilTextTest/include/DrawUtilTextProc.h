//==============================================================================
/**
 @file    DrawUtilTextProc.h
 @author  fukushima_yuya
 @date    2015.02.19
 @brief   DrawUtilTextのTestProc
 */
//==============================================================================
#if PM_DEBUG

#if !defined(NIJI_PROJECT_TEST_DRAWUTILTEXTPROC_H_INCLUDED)
#define NIJI_PROJECT_TEST_DRAWUTILTEXTPROC_H_INCLUDED
#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>

#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

// niji
#include <GameSys/include/GameProc.h>

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(UtilText)


class DrawUtilTextProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( DrawUtilTextProc );

public:
  //! @brief  コンストラクタ
  DrawUtilTextProc( void );
  //! @brief  デストラクタ
  virtual ~DrawUtilTextProc( void );

  //! @brief   PROCの初期化を行う関数
  //! @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );

  //! @brief    PROCのメイン更新を行う関数
  //! @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );

  //! @brief    PROCのメイン描画を行う関数
  //!          純粋仮想ではありません。
  virtual void   DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo);

  //! @brief   PROCのメインを行う関数
  //!  1/30フレームの②フレーム目の処理です。
  //!  1/60フレーム動作時には、使われません。
  //! @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);

  //! @brief   PROCの終了を行う関数
  //! @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

private:
  //! @brief  初期化処理
  void Initialize( gfl2::heap::HeapBase* pMemHeap, gfl2::heap::HeapBase* pDevHeap );

  //! @brief  終了処理
  void Finalize( void );

  //! @brief  更新処理
  void Update( void );

private:
  gfl2::heap::NwAllocator*    m_pMemAllocator;
  gfl2::heap::NwAllocator*    m_pDevAllocator;

  System::nijiAllocator*      m_pNijiAllocator;

  gfl2::util::UtilTextRenderingPipeLine*    m_pRenderingPipeLine;

  gfl2::str::ResFont*         m_pResFont;
  gfl2::util::DrawUtilText*   mpDrawUtilText;

private:
  s32    m_count;
};


GFL_NAMESPACE_END(UtilText)
GFL_NAMESPACE_END(Test)

#endif  // NIJI_PROJECT_TEST_DRAWUTILTEXTPROC_H_INCLUDED

#endif // PM_DEBUG
