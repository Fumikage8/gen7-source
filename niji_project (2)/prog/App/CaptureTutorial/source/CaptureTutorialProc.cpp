//=============================================================================
/**
 * @file CaptureTutorialProcProc.h
 * @brief 捕獲チュートリアルメインプロセス
 * @author uchida_yuto
 * @date 2015.05.28
 */
//=============================================================================
#include "App/CaptureTutorial/include/CaptureTutorialProc.h"
#include "App/CaptureTutorial/source/parts/CaptureTutorialMain.h"

//  gflb2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

// niji
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

//------------------------------------------------------------------------------
//  プロセス内で使用するヒープサイズ
#define CAPTURE_DEMO_SYS_HEAP_SIZE 0x20000

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  CaptureTutorialProc::CaptureTutorialProc( void ) :
    GameSys::GameProc()
  {
    _Claer();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  CaptureTutorialProc::~CaptureTutorialProc()
  {
  }

  //-------------------------------------------------------------------------
  /**
    * @brief	プロセスの初期化処理
    * @param proc_manager  プロセスのマネージャ
    * @retval RES_CONTINUE  初期化処理が継続中
    * @retval RES_FINISH    初期化処理が終了
    */
  //-------------------------------------------------------------------------
  gfl2::proc::Result CaptureTutorialProc::InitFunc( gfl2::proc::Manager* proc_manager )
  {
    _CreateHeap();

    //  強制フェードアウト状態にする
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );
    }

    // ここでメイン作成する
    m_pMain = GFL_NEW( m_heap ) CaptureTutorialMain( GameProc::GetGameManager() , this, static_cast<CaptureTutorialViewer::TutorialType>(this->GetTutorialType()) );
    m_pMain->Enter();

    return gfl2::proc::RES_FINISH;
  } 

  //-------------------------------------------------------------------------
  /**
    * @brief プロセスの終了処理
    * @param proc_manager  プロセスのマネージャ
    * @retval RES_CONTINUE  終了処理が継続中
    * @retval RES_FINISH    終了処理が終了
    */
  //-------------------------------------------------------------------------
  gfl2::proc::Result CaptureTutorialProc::EndFunc( gfl2::proc::Manager* proc_manager )
  {
    // メインの後片付け
    GFL_SAFE_DELETE( m_pMain );
      
    _DeleteHeap();
      
    return gfl2::proc::RES_FINISH;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief プロセスの更新処理
    * @param proc_manager  プロセスのマネージャ
    * @retval RES_CONTINUE  メイン処理が継続中
    * @retval RES_FINISH    メイン処理が終了
    */
  //-------------------------------------------------------------------------
  gfl2::proc::Result CaptureTutorialProc::UpdateFunc( gfl2::proc::Manager* proc_manager )
  { 
    if( m_pMain->Update() )
    {
      return gfl2::proc::RES_CONTINUE;
    }

    return gfl2::proc::RES_FINISH;
  }

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  void CaptureTutorialProc::PreDrawFunc( gfl2::proc::Manager* pManager )
  {
    m_pMain->PreDraw();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief プロセスの描画処理
    * @param proc_manager  プロセスのマネージャ
    */
  //-------------------------------------------------------------------------
  void CaptureTutorialProc::DrawFunc( gfl2::proc::Manager* proc_manager, gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pMain->Draw( displayNo );
  }

  //-------------------------------------------------------------------------
  void CaptureTutorialProc::_CreateHeap()
  {
    m_heap = GFL_CREATE_LOCAL_HEAP( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ), CAPTURE_DEMO_SYS_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  }

  void CaptureTutorialProc::_DeleteHeap()
  {
    gfl2::heap::Manager::DeleteHeap( m_heap );
  }

  u32 CaptureTutorialProc::GetTutorialType() const
  {
    return CaptureTutorialViewer::CAPTURE;
  }



  //--------
  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  ZenryokuWazaTutorialProc::ZenryokuWazaTutorialProc( void ) :
    CaptureTutorialProc()
  {
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  ZenryokuWazaTutorialProc::~ZenryokuWazaTutorialProc()
  {
  }

  u32 ZenryokuWazaTutorialProc::GetTutorialType() const
  {
    return CaptureTutorialViewer::ZENRYOKU_WAZA;
  }


GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)
