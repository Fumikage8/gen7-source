//===================================================================
/**
 * @file    GameProc.cpp
 * @brief   ゲーム中核のプロセスクラス
 * @author  k.ohno
 * @date    2011.3.31
*/
//===================================================================

#include <GameSystem/include/GameProc.h>
#include <GameSystem/include/GameProcManager.h>


namespace GameSystem {

//-----------------------------------------------------------------------------
/**
*     Proc
 */
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
GameProc::GameProc(void) : 
  mpGameProcManager(NULL)
{
}



//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::InitFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void GameProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED(pManager);
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void GameProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_UNUSED(pManager);
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメインを行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result GameProc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED(pManager);

  return gfl2::proc::RES_FINISH;
}


//----------------------------------------------------------------------------
/**
 * @brief   PROCのイベント起動検知処理
 *
 *	@param	pManager    プロセスマネージャ
 */
//-----------------------------------------------------------------------------
void GameProc::EventCheckFunc(GameManager* pManager)
{
  GFL_UNUSED(pManager);
}

//----------------------------------------------------------------------------
/**
 *	@brief  GameManagerの取得
 *	@return GameManagerポインタ
 */
//-----------------------------------------------------------------------------
GameManager* GameProc::GetGameManager(void)
{
  return mpGameProcManager->GetGameManager(); 
}


} /* namespace GameSys */

