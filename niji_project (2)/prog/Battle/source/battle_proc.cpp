//===================================================================
/**
 * @file    app_proc.cpp
 * @brief   スケルトンアプリケーションのプロセスクラス
 * @author  k.ohno
 * @date    2014.6.19
 */
//===================================================================


#include <heap/include/gfl2_heap.h>


#include "Battle/include/battle_proc.h"
#include "Battle/include/battle_SetupParam.h"
#include "./btl_Heap.h"
#include "./btl_mainmodule.h"
#include "./btl_BattleRule.h"
#include "./btl_PokemonPosition.h"
#include "./btl_BossBattle.h"

#include "btlv/btlv_debug.h"  //tick計測用

GFL_NAMESPACE_BEGIN(btl)


//-----------------------------------------------------------------------------
/**
 *     Proc
 */
//-----------------------------------------------------------------------------

BattleProc::BattleProc(BATTLE_PROC_PARAM* pApp)
{
  BtlvTickStart(2,"BattleProcStart");

  mInitParam.pAppMemory = pApp->pAppMemory;
  mInitParam.pAppDeviceMemory = pApp->pAppDeviceMemory;
  mInitParam.pGameManager = pApp->pGameManager;
  m_setupParam = pApp->pSetupParam;
  //srand((unsigned) time(NULL));

#if PM_DEBUG
  this->CheckSetupValidity( *m_setupParam );
#endif
}

#if PM_DEBUG

/**
 * @brief セットアップパラメータの正当性をチェックする
 * @param setupParam  チェック対象のセットアップパラメータ
 */
void BattleProc::CheckSetupValidity( const BATTLE_SETUP_PARAM& setupParam ) const
{
  // 必要なパーティデータが存在するか？
  {
    const u32 clientNum = BattleRule::GetClientNum( setupParam.rule, setupParam.multiMode );

    for( u32 i=0; i<clientNum; ++i )
    {
      const BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );
      if( !BattleRule::IsClientExist( setupParam.rule, setupParam.multiMode, clientId ) )
      {
        continue;
      }

      // 通信対戦なら、自分のパーティのみをチェックする
      if( ( setupParam.commMode != BTL_COMM_NONE ) &&
          ( i != 0 ) )
      {
        return;
      }

      if( setupParam.party[i] == NULL )
      {
        GFL_PRINT( "ERROR!! BATTLE_SETUP_PARAMに必要なパーティデータが設定されていません( index=%d )\n", i );
        GFL_ASSERT(0);
      }

      const u32 coverPosNum = PokemonPosition::GetClientCoverPosNum( setupParam.rule, setupParam.multiMode, clientId );
      const u32 memberNum = setupParam.party[i]->GetMemberCount();
      if( memberNum < coverPosNum )
      {
        GFL_PRINT( "ERROR!! BATTLE_SETUP_PARAMのパーティに必要なメンバーが足りていません( index=%d, memberNum=%d/%d )\n", i, memberNum, coverPosNum );
        GFL_ASSERT(0);
      }
    }
  }
}

#endif


//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result BattleProc::InitFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  gfl2::heap::HeapBase *heap = mInitParam.pAppMemory;
//  gfl2::heap::HeapBase *devHeap = mInitParam.pAppDeviceMemory;
  m_heap_total  = GFL_CREATE_LOCAL_HEAP_NAME( heap,         Heap::HEAPSIZE_TOTAL,  gfl2::heap::HEAP_TYPE_EXP, false , "BtlTotal" );
  m_heap_system = GFL_CREATE_LOCAL_HEAP_NAME( m_heap_total, Heap::HEAPSIZE_SYSTEM, gfl2::heap::HEAP_TYPE_EXP, false , "BtlSystem" );
  m_heap_view   = GFL_CREATE_LOCAL_HEAP_NAME( m_heap_total, Heap::HEAPSIZE_VIEW,   gfl2::heap::HEAP_TYPE_EXP, false , "BtlView" );
  m_heap_net    = GFL_CREATE_LOCAL_HEAP_NAME( m_heap_total, Heap::HEAPSIZE_NET,    gfl2::heap::HEAP_TYPE_EXP, false , "BtlNet" );

  // ボス戦なら、ボス戦用のデフォルト能力アップを適用する
  if( BATTLE_PARAM_CheckBtlStatusFlag( m_setupParam, BTL_STATUS_FLAG_VS_BOSS ) )
  {
    BossBattle::ApplyDefaultPowerUp( m_setupParam );
  }

  m_mainModule = GFL_NEW(heap) MainModule( m_setupParam, m_heap_system, m_heap_view, m_heap_net,  mInitParam.pGameManager ,mInitParam.pGameManager->GetUiDeviceManager() ,this);

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result BattleProc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  if( m_mainModule )
  {
    GFL_DELETE( m_mainModule );
    m_mainModule = NULL;
  }

  GFL_DELETE_HEAP( m_heap_net );
  GFL_DELETE_HEAP( m_heap_view );
  GFL_DELETE_HEAP( m_heap_system );
  GFL_DELETE_HEAP( m_heap_total );

  m_setupParam = NULL;
  GFL_RELEASE_PRINT("BattleEnd.\n");
  return gfl2::proc::RES_FINISH;
}


//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result BattleProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  enum {
    SEQ_INITIALIZE,
    SEQ_MAINLOOP,
    SEQ_FINALIZE,
  };

  // リリース時にASSERT失敗を通過した場合の対処
  if( !m_mainModule ){
    return gfl2::proc::RES_FINISH;
  }


  switch( m_mainSeq ){
  case SEQ_INITIALIZE:
    if( m_mainModule->Initialize(&m_subSeq) )
    {
      m_subSeq = 0;
      m_mainSeq = SEQ_MAINLOOP;
    }
    break;

  case SEQ_MAINLOOP:
    if( m_mainModule->MainLoop(&m_subSeq) )
    {

      m_subSeq = 0;
      m_mainSeq = SEQ_FINALIZE;
    }
    break;

  case SEQ_FINALIZE:
    if( m_mainModule->Finalize(&m_subSeq) )
    {
      return gfl2::proc::RES_FINISH;
    }
    break;

  }

  return gfl2::proc::RES_CONTINUE;

}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void BattleProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  m_mainModule->PreDrawLoop(&m_subSeq);
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void BattleProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_mainModule->DrawLoop(&m_subSeq , displayNo);
}

GFL_NAMESPACE_END(btl)
