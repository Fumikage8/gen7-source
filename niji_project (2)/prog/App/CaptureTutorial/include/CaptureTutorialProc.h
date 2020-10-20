//=============================================================================
/**
 * @file CaptureTutorialProcProc.h
 * @brief 捕獲チュートリアルメインプロセス
 * @author uchida_yuto
 * @date 2015.05.28
 */
//=============================================================================
#ifndef __CAPTURE_TUTORIAL_PROC_H__
#define __CAPTURE_TUTORIAL_PROC_H__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "GameSys/include/GameProc.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

  //  前方宣言
  class CaptureTutorialMain;

  class CaptureTutorialProc : public GameSys::GameProc
  {
  public: 

    CaptureTutorialProc( void );
    virtual ~CaptureTutorialProc();

    //---------------------------------------------------------------------
    // プロセスとしての動作
    //----------------------------------------------------------------------
    /**
     * @brief	プロセスの初期化処理
     * @param proc_manager  プロセスのマネージャ
     * @retval RES_CONTINUE  初期化処理が継続中
     * @retval RES_FINISH    初期化処理が終了
     */
    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* proc_manager );

    /**
     * @brief プロセスの更新処理
     * @param proc_manager  プロセスのマネージャ
     * @retval RES_CONTINUE  メイン処理が継続中
     * @retval RES_FINISH    メイン処理が終了
     */
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* proc_manager );

    /**
    * @brief	PROCの描画処理(下準備)
    * @param pManager   プロセス管理マネージャー
    * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
    * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
    */
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

    /**
     * @brief プロセスの描画処理
     * @param proc_manager  プロセスのマネージャ
     */
    virtual void DrawFunc( gfl2::proc::Manager* proc_manager, gfl2::gfx::CtrDisplayNo displayNo );

    /**
     * @brief プロセスの終了処理
     * @param proc_manager  プロセスのマネージャ
     * @retval RES_CONTINUE  終了処理が継続中
     * @retval RES_FINISH    終了処理が終了
     */
    virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* proc_manager );
  
    // フレームレートを30にする
    virtual GameSys::FrameMode GetFrameMode(void) const { return GameSys::FRAMEMODE_30; }

  protected:

    // @brief チュートリアルタイプの取得
    virtual u32 GetTutorialType() const;

  private:
    void _Claer()
    {
      m_heap  = NULL;
      m_pMain = NULL;
    }

    void _CreateHeap();
    void _DeleteHeap();

    gfl2::heap::HeapBase* m_heap;
    CaptureTutorialMain*  m_pMain;
  };


  // 全力技チュートリアル
  class ZenryokuWazaTutorialProc : public CaptureTutorialProc
  {
  public:
    ZenryokuWazaTutorialProc( void );
    virtual ~ZenryokuWazaTutorialProc();

  protected:


    virtual u32 GetTutorialType() const;
  };

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)

#endif // __CAPTURE_TUTORIAL_PROC_H__
