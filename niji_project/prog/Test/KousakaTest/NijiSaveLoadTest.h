//==============================================================================
/**
 * @file	NijiSaveLoadTest.h
 */
// =============================================================================

#ifdef  GF_PLATFORM_CTR
#ifndef __NIJISAVELOAD_TEST__
#define __NIJISAVELOAD_TEST__
#pragma once
#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>


// debug
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

// draw
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(NijiSaveLoadTest)

class NijiSaveLoadTest : public GameSys::GameProc 
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiSaveLoadTest); //コピーコンストラクタ＋代入禁止

  enum
  {
    MODE_SETTING = 0,
    MODE_WAIT_SAVE,
    MODE_WAIT_LOAD,
    MODE_SAVE_INTERVAL,
    MODE_LOAD_INTERVAL,
    MODE_ERROR,
  };

public:
  NijiSaveLoadTest(void);  //コンストラクタ
  virtual ~NijiSaveLoadTest(void);  //デストラクタ

public:
  //=============================================================================
  /**
  *  publicメソッド
  */
  //=============================================================================

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

  //------------------------------------------------------------------
  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

public:





private:
  //------------------------------------------------------------------
  /**
   * @brief 文字描画
   */
  //------------------------------------------------------------------
  void DrawText( void );

private:

	gfl2::heap::NwAllocator*		m_pMemAllocator;
	gfl2::heap::NwAllocator*		m_pDevAllocator;
  System::nijiAllocator*      m_pNijiAllocator;

	gfl2::util::UtilTextRenderingPipeLine*		m_pRenderingPipeLine;

	gfl2::util::DrawUtilText* m_pDrawUtilText;


  bool m_stopRequest;
  u32 m_interval;
  
  u32 m_lastLoadTime;
  u32 m_lastSaveTime;
  u32 m_errorCode;

  int m_mode;

  u64 m_waitTime;
  u64 m_DispTime;
};

GFL_NAMESPACE_END(NijiSaveLoadTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
#endif //__LOCAL_QUICK_MATCH_TEST__
#endif // GF_PLATFORM_CTR