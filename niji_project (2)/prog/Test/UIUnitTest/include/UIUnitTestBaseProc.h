#if PM_DEBUG

#ifndef __UI_UNIT_TEST_BASE_PROC_H__
#define __UI_UNIT_TEST_BASE_PROC_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>

// niji
#include <GameSys/include/GameProc.h>

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/AppRenderingManager.h>


GFL_NAMESPACE_BEGIN(Test)

class UIUnitTestBaseProc : public GameSys::GameProc {
  GFL_FORBID_COPY_AND_ASSIGN(UIUnitTestBaseProc); //コピーコンストラクタ＋代入禁止

public:
  UIUnitTestBaseProc(void);  //コンストラクタ
  virtual ~UIUnitTestBaseProc(void);  //デストラクタ

  //------------------------------------------------------------------
  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
    * @brief   PROCのメインを行う関数
    *  1/30フレームの②フレーム目の処理です。
    *  1/60フレーム動作時には、使われません。
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCのイベント起動検知処理
    *
    *  以下のタイミングで行うべき、イベント起動処理
    *  　GameProc->EventCheckFunc
    *  　EventManager->Main
    *  　GameProc->Main
    */
  //------------------------------------------------------------------
  virtual void EventCheckFunc(GameSys::GameManager* pManager);

protected:
    //  レンダリングタイプ取得
    //  作成するレンダリングタイプを派生先で変更することが可能
    virtual app::util::AppRenderingManager::RenderOrderType::Tag _OnGetDispLowAppRenderOrderType()
    {
      return  app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
    }

    virtual app::util::AppRenderingManager::RenderOrderType::Tag _OnGetDispUppAppRenderOrderType()
    {
      return  app::util::AppRenderingManager::RenderOrderType::LAYOUT_POKEMODEL;
    }

    virtual app::util::AppRenderingManager::RenderOrderType::Tag _OnGetDispOffScreenAppRenderOrderType()
    {
      return  app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
    }

    //  テスト用のフレーム作成
    virtual applib::frame::CellSuper* OnCreateFrame( applib::frame::Manager* pFrameManager, GameSys::GameManager* pGameManager )  = 0;

    applib::frame::Manager *mpFrameManager;

    app::util::AppRenderingManager* mpAppRenderingManager;
    gfl2::heap::HeapBase *mpManagerHeap;

    app::util::Heap* mpAppHeap;

    applib::frame::CellSuper* mpMenuCellFrame;

    //  3Dモデル描画用
    System::nijiAllocator*            m_pNijiAllocatorForEdgeMapTexture;  // VRAM限定。

    bool  m_bInit;
    bool  m_bEnd;

};  //class Proc

GFL_NAMESPACE_END(Test)

#endif // __UI_UNIT_TEST_BASE_PROC_H__

#endif // PM_DEBUG
