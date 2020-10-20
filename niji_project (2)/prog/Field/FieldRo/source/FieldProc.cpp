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
#include "System/include/GflUse.h"              // for DisplayDeviceHandle
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
#include "GameSys/include/GameManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

// Field
#include "Field/FieldRo/include/FieldProc.h"


#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field\FieldRo\include\Subscreen\FieldSubScreenProcManager.h"


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
FieldProc::FieldProc( FieldmapDesc* desc ) : 
m_pFieldmap( NULL)
{
  m_desc = *desc;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
FieldProc::~FieldProc( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result FieldProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  GetGameManager()->GetFieldResident()->GetFieldSubScreenProcManager()->Initialize( GetGameManager()->GetSubScreen() );

  return gfl2::proc::RES_FINISH;
}

/**
* @brief    PROCのメイン更新を行う関数
* @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  int seq = this->GetSubSeq();
  switch(seq){
  case SEQ_CREATE:
    {
      gfl2::heap::HeapBase *systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_DEVICE );
      m_pFieldmap = GFL_NEW(systemHeap) Fieldmap(m_desc);
      this->GetGameManager()->SetFieldmap( m_pFieldmap );
      this->AddSubSeq();
    }
    // break through
  case SEQ_INITIALIZE:
    if( m_pFieldmap->Initialize(this->GetGameManager()) ){
      NetLib::NijiNetworkSystem::RestartBeaconSystem();//ビーコンシステム再開
      this->AddSubSeq();
    }
#if PM_DEBUG
    m_pFieldmap->UpdateCheckFileRead_InitializeTime();
#endif
    break;

  case SEQ_UPDATE:
    GetGameManager()->GetFieldResident()->GetFieldSubScreenProcManager()->Update();
    if( !m_pFieldmap->Update() )
    {
#if PM_DEBUG
      m_pFieldmap->UpdateCheckFileRead_NormalTime();
#endif
      break;
    }
    this->AddSubSeq();
    //break; through

  case SEQ_ENDWAIT:
    {
      // 完了のチェック
      if( !m_pFieldmap->Terminate() )
      {
        break;
      }
      this->AddSubSeq();
    }
    //break through

  case SEQ_DELETE:
    {
      GFL_DELETE(m_pFieldmap);
      m_pFieldmap = NULL;
      this->GetGameManager()->SetFieldmap( NULL );
      TOMOYA_PRINT( "FieldProcEnd\n" );
      this->AddSubSeq();
    }
    //break through

  case SEQ_STOP:
    break;

  case SEQ_END:
    return gfl2::proc::RES_FINISH;
  }


  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief  PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void FieldProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
  if(m_pFieldmap && (this->GetSubSeq() == SEQ_UPDATE))
  {
    m_pFieldmap->PreDraw();
  }
}

/**
* @brief  PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void FieldProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  if(m_pFieldmap && (this->GetSubSeq() == SEQ_UPDATE))
  {
    m_pFieldmap->Draw( displayNo );
  }
  GetGameManager()->GetFieldResident()->GetFieldSubScreenProcManager()->Draw( displayNo );
}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldProc::NextFrameMainFunc(gfl2::proc::Manager* /*pManager*/)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result FieldProc::EndFunc(gfl2::proc::Manager* /*pManager*/)
{
  if(false == GetGameManager()->GetFieldResident()->GetFieldSubScreenProcManager()->Finalize()){
    return gfl2::proc::RES_CONTINUE;
  }
  return gfl2::proc::RES_FINISH;
}

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
void FieldProc::EventCheckFunc(GameSys::GameManager* /*pManager*/)
{
  if(m_pFieldmap && (this->GetSubSeq() == SEQ_UPDATE))
  {
    m_pFieldmap->EventCheck();
  }
}


/**
* @brief マップジャンプ復帰時の初期化開始のリクエスト発行
*/ 
void FieldProc::RequestReturnInitialize( FieldmapDesc* desc )
{
  if( this->GetSubSeq() == SEQ_STOP )
  {
    m_desc = *desc;
    this->SetSubSeq( SEQ_CREATE );
  }
  else
  {
    GFL_ASSERT_STOP(this->GetSubSeq() == SEQ_STOP);
  }
}

/**
* @brief プロセス破棄のリクエスト発行
*/ 
void FieldProc::RequestTerminate( void )
{
  if( this->GetSubSeq() == SEQ_STOP )
  {
    this->SetSubSeq( SEQ_END );
  }
  else
  {
    GFL_ASSERT_STOP(this->GetSubSeq() == SEQ_STOP);
  }
}


GFL_NAMESPACE_END( Field );
