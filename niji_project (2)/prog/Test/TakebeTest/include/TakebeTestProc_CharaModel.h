/**
* @brief  竹部テスト用プロック：キャラモデル
* @file   TakebeTestProc_CharaModel.h
* @author takebe_makoto
* @data   2015.04.14
*/
#if PM_DEBUG

#ifndef	__TAKEBE_TEST_PROC_CHARA_MODEL_H_INCLUDE__
#define	__TAKEBE_TEST_PROC_CHARA_MODEL_H_INCLUDE__

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"
#include <system/include/nijiAllocator.h>

#include <model/include/gfl2_CharaModelFactory.h>
#include <model/include/gfl2_CharaModel.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>


namespace Test { namespace TakebeTest {

class TakebeTestProc_CharaModel : public GameSys::GameProc
{
public:

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TakebeTestProc_CharaModel( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TakebeTestProc_CharaModel( void );

  /**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );

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

  /**
  * @brief   PROCのメインを行う関数
  *  1/30フレームの②フレーム目の処理です。
  *  1/60フレーム動作時には、使われません。
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief   FrameModeの取得
  * @retval   FrameMode
  *
  * フレームモードを切り替えるときは、戻り値を変えてください。
  */
  virtual GameSys::FrameMode GetFrameMode( void ) const { return GameSys::FRAMEMODE_30; }

private:

	gfl2::heap::HeapBase* m_pHeap; // 適当なヒープ
  System::nijiAllocator* m_pNijiAllocator;
  s32 m_StartFreeSize;
  poke_3d::model::CharaModelFactory m_CharaModelFactory;
  poke_3d::model::CharaModel m_CharaModel;
  u32 m_InitSeq;
  u32 m_UpdateSeq;
  u32 m_EndSeq;

  s32 m_Counter;

	gfl2::renderingengine::renderer::DefaultRenderingPipeLine* m_pRenderingPipeLine;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;
};

}} // Test::TakebeTest

#endif	// __TAKEBE_TEST_PROC_H_INCLUDE__

#endif // PM_DEBUG
