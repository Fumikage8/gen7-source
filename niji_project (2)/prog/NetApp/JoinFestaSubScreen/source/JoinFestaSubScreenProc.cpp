//==============================================================================
/**
 * @file	JoinFestaSubScreenProc.cpp
 * @brief	ジョインフェスタ下画面Proc
 * @author	ichiraku_katsuhiko
 * @date	2015/06/10 14:23:23
 */
// =============================================================================
#include <debug/include/gfl2_DebugPrint.h>

#include "NetApp/JoinFestaSubScreen/include/JoinFestaSubScreenProc.h"
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenBuilder.h"
#include "NetApp/JoinFestaSubScreen/source/ApplicationSystem/JoinFestaSubScreenApplicationSystem.h"
#include "NetApp/JoinFestaSubScreen\source/ApplicationSystem/JoinFestaSubScreenWork.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenFrameResult.h"
#include "NetApp/JoinFestaSubScreen/source/JoinFestaSubScreenResourceID.h"

#include "System/include/HeapDefine.h"

// mine
#include "NetApp/JoinFestaSubScreen/include/JoinFestaSubScreenProc.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)

// コンストラクタ
JoinFestaSubScreenProc::JoinFestaSubScreenProc() :
  m_pAppParam( NULL ),
  m_pJoinFestaSubScreenWork( NULL ),
  m_pApplicationSystem( NULL ),
  m_pRotomFade(NULL)
{
}
JoinFestaSubScreenProc::JoinFestaSubScreenProc( NetApp::JoinFestaSubScreen::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pJoinFestaSubScreenWork( NULL ),
  m_pApplicationSystem( NULL ),
  m_pRotomFade(NULL)
{
}

// デストラクタ
JoinFestaSubScreenProc::~JoinFestaSubScreenProc()
{
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result JoinFestaSubScreenProc::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
    INIT_SEQUENCE_FADEIN,
    INIT_SEQUENCE_FADEIN_WAIT,
  };

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
      gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_SUBSCREEN_DEVICE );

      m_pJoinFestaSubScreenWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork();

      app::util::Heap* pAppHeap = m_pJoinFestaSubScreenWork->GetAppHeap();
      NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenBuilder netAppSkeletonBuilder( m_pJoinFestaSubScreenWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenApplicationSystem();
      m_pApplicationSystem->NonUseFadeRequest(); // フェードを使用しない
      m_pApplicationSystem->Build( pAppHeap, &netAppSkeletonBuilder );
      SetSubSeq( INIT_SEQUENCE_RESOURCE_LOADING );
    }
    /* break through */

    case INIT_SEQUENCE_RESOURCE_LOADING:
    {
      if( !m_pApplicationSystem->Loading() )
      {

        // 空きメモリ
        {
          gfl2::heap::HeapBase *pSysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_SUBSCREEN );
          gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FIELD_SUBSCREEN_DEVICE );
          GFL_PRINT("");
          GFL_PRINT( "----------------------------------------------\n" );
          GFL_PRINT( "memory debug\n" );
          GFL_PRINT( "----- HEAPID_FIELD_SUBSCREEN -----\n" );
          GFL_PRINT( "Total size                = %d\n", pSysHeap->GetTotalSize() );
          GFL_PRINT( "Total free size           = %d\n", pSysHeap->GetTotalFreeSize() );
          GFL_PRINT( "Total allocatable size    = %d\n", pSysHeap->GetTotalAllocatableSize() );
          GFL_PRINT( "Memory fragmentation size = %d\n", pSysHeap->GetTotalFreeSize() - pSysHeap->GetTotalAllocatableSize() );
          GFL_PRINT( "Use memory size           = %d\n", pSysHeap->GetTotalSize() - pSysHeap->GetTotalFreeSize() );

          GFL_PRINT( "----- HEAPID_FIELD_SUBSCREEN_DEVICE -----\n" );
          GFL_PRINT( "Total size                = %d\n", pDevHeap->GetTotalSize() );
          GFL_PRINT( "Total free size           = %d\n", pDevHeap->GetTotalFreeSize() );
          GFL_PRINT( "Total allocatable size    = %d\n", pDevHeap->GetTotalAllocatableSize() );
          GFL_PRINT( "Memory fragmentation size = %d\n", pDevHeap->GetTotalFreeSize() - pDevHeap->GetTotalAllocatableSize() );
          GFL_PRINT( "Use memory size           = %d\n", pDevHeap->GetTotalSize() - pDevHeap->GetTotalFreeSize() );
          GFL_PRINT( "----------------------------------------------\n" );
        }

        result = gfl2::proc::RES_FINISH;
      }
      //GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_UPPER );
      //GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_LOWER );
    }
    break;
  }

  return result;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result JoinFestaSubScreenProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  // リクエストは毎フレームリセットする
  m_pJoinFestaSubScreenWork->ResetRequestID();

  m_pApplicationSystem->Update();
  if( m_pRotomFade )
  {
    m_pRotomFade->Update();
  }

  // 終了はリクエストをフィールドを受け付けて、フィールドから終了リクエストが通知された時だけ。
  if( this->IsReqEnd() )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

void JoinFestaSubScreenProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void JoinFestaSubScreenProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_pRotomFade )
  {
    m_pRotomFade->DrawTree( displayNo );
  }
  m_pApplicationSystem->Draw( displayNo );
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result JoinFestaSubScreenProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pRotomFade );

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaSubScreenWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フィールド復帰　再稼働
 */
//-----------------------------------------------------------------------------
void JoinFestaSubScreenProc::Resume( void )
{
  if( m_pApplicationSystem != NULL )
  {
    m_pApplicationSystem->Resume();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  フィールド削除　一時停止
 */
//-----------------------------------------------------------------------------
void JoinFestaSubScreenProc::Suspend( void )
{
  if( m_pApplicationSystem != NULL )
  {
    m_pApplicationSystem->Suspend();
  }
}

//----------------------------------------------------------------------------
/**
 * @brief	  起動リクエストを返す
 *          フィールドでポーリングされます。
 */
//-----------------------------------------------------------------------------
Field::SubScreen::FieldMenuOutRequestID JoinFestaSubScreenProc::GetEventRequestID()
{
  return m_pJoinFestaSubScreenWork->GetRequestID();
}


/**
 * @brief   ロトムフェードイン開始
 */
void JoinFestaSubScreenProc::RotomFadeInStart()
{
  app::util::Heap* pAppHeap = m_pJoinFestaSubScreenWork->GetAppHeap();

  if( m_pRotomFade == NULL )
  {
    m_pRotomFade = GFL_NEW( pAppHeap->GetDeviceHeap() ) View::RotomFade( pAppHeap,
                                                                         m_pJoinFestaSubScreenWork->GetAppRenderingManager(),
                                                                         m_pJoinFestaSubScreenWork->GetResourceManager()->GetResourceBuffer(JOINFESTASUBSCREEN_RESOURCE_ID_ROTOM_FADE) );
  }
  m_pRotomFade->StartRotomFade();
}

/**
 * @brief   ロトムフェードイン終了チェック
 * @return  trueで終了
 */
bool JoinFestaSubScreenProc::IsRotomFadeInEnd()
{
  return m_pRotomFade->IsEndRotomFade();
}


GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)
