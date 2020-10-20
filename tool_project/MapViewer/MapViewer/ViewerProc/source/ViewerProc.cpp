//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   FieldProc.cpp
 *  @brief  フィールドプロセスクラス
 *  @author tomoya takahashi
 *  @date   2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// gfl2
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_heap_manager.h>

// niji
#include "System/include/GflvUse.h"              // for DisplayDeviceHandle @todo GflUse.h参照箇所を増やしたくないので、別ファイル化したい 150215 ikuta_junya 
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "GameSystem/include/GameManager.h"


// Field
#include "ViewerProc/include/ViewerProc.h"
#include "ViewerProc/include/FieldStatic/FieldResident/FieldResident.h"


GFL_NAMESPACE_BEGIN( Field );

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

/**------------------------------------------------------------------
 *      公開関数
 ------------------------------------------------------------------*/

/**------------------------------------------------------------------
 *      GameProc仮想関数
 ------------------------------------------------------------------*/
/**
* @brief コンストラクタ
*
* @return 無し
*/ 
ViewerProc::ViewerProc( void/*FieldmapDesc* desc*/ )
{
	m_pFieldmap = NULL;
  ///m_desc = *desc;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ViewerProc::~ViewerProc( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result ViewerProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  enum
  {
    SEQ_CREATE,
    SEQ_INITIALIZE,
  };
  int seq = this->GetSubSeq();
  switch(seq){
  case SEQ_CREATE:
    {
      // フィールド常駐データ生成
      gfl2::heap::HeapBase *pResidentHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT );
      FieldResident * pResident = GFL_NEW(pResidentHeap) FieldResident();
      {
				GameSystem::GameManager *gmgr = this->GetGameManager();
        gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP); 
        gfl2::heap::HeapBase* pAppHeapDevice = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); 
        pResident->Initialize( pAppHeap, pAppHeapDevice, gmgr->GetAsyncFileManager() );
        gmgr->SetFieldResident( pResident );
      }
      
      gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP );
      m_pFieldmap = GFL_NEW(systemHeap) Fieldmap(/*m_desc*/);
      this->GetGameManager()->SetFieldmap( m_pFieldmap );
      this->AddSubSeq();
    }
    // break through
  case SEQ_INITIALIZE:
    if( m_pFieldmap->Initialize(this->GetGameManager()) ){
      return gfl2::proc::RES_FINISH; // 終了
    }
    break;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief    PROCのメイン更新を行う関数
* @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result ViewerProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if( m_pFieldmap->Update() ){
    return gfl2::proc::RES_FINISH; // 終了
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief  PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void ViewerProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
}

/**
* @brief  PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void ViewerProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pFieldmap->Draw( displayNo );
}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result ViewerProc::NextFrameMainFunc(gfl2::proc::Manager* /*pManager*/)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result ViewerProc::EndFunc(gfl2::proc::Manager* /*pManager*/)
{
  enum
  {
    SEQ_ENDWAIT,
    SEQ_DELETE,
  };
  int seq = this->GetSubSeq();
  switch(seq){

  case SEQ_ENDWAIT:
    {
      // 完了のチェック
      if( m_pFieldmap->Terminate() ){
        this->AddSubSeq();
      }
    }
    break;

  case SEQ_DELETE:
    {
      GFL_DELETE(m_pFieldmap);
      m_pFieldmap = NULL;
      this->GetGameManager()->SetFieldmap( NULL );

			GameSystem::GameManager *gmgr = this->GetGameManager();
			FieldResident * pResident = gmgr->GetFieldResident();
			pResident->Terminate();
      GFL_DELETE(pResident);
			gmgr->SetFieldResident( NULL );
    }
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

#if 0
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
void ViewerProc::EventCheckFunc(GameSystem::GameManager* /*pManager*/)
{
  if( m_pFieldmap ){
    m_pFieldmap->EventCheck();
  }
}
#endif


GFL_NAMESPACE_END( Field );
