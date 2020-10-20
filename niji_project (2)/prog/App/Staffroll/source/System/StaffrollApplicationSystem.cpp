// ============================================================================
/*
 * @file StaffrollApplicationSystem.cpp
 * @brief スタッフロールアプリのアプリケーションシステムです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/System/StaffrollApplicationSystem.h"
#include "App/Staffroll/source/System/StaffrollBuilder.h"
#include "App/Staffroll/source/System/StaffrollWork.h"

//  Viewのインクルード
#include "App/Staffroll/source/System/StaffrollViewList.h"
#include "App/Staffroll/source/System/StaffrollViewObjects.h"

/// idのインクルード
#include "App/Staffroll/source/StaffrollResourceID.h"

//  nijiのインクルード
#include "System/include/ArcIdGetter.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"
#include "AppLib/include/Util/AppUtilFileControlState.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <arc_def_index/arc_def.h>
#include <arc_index/staffroll_data.gaix>
#include <arc_index/StaffrollLyt.gaix>
#include <arc_index/StaffrollGrpFont.gaix>

//  バトルフォント設定に必要
#include "Print/include/SystemFont.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)


  StaffrollApplicationSystem::StaffrollApplicationSystem()
    : m_pWork( NULL )
#if PM_DEBUG
    , m_debugHeapSize( 0xffffffff )
    , m_debugDevHeapSize( 0xffffffff )
#endif
  {
    _Clear();
  }


  StaffrollApplicationSystem::~StaffrollApplicationSystem()
  {
//    GFL_SAFE_DELETE( m_pImgFileCtrlState );
    GFL_SAFE_DELETE( m_pViewSystemAccessor );
    GFL_SAFE_DELETE( m_pViewList );
    GFL_SAFE_DELETE( m_pViewObjects );

#if PM_DEBUG
    GFL_PRINT( "★HeapSizeCheck: sys[0x%08x] dev[0x%08x]\n", m_debugHeapSize, m_debugDevHeapSize );
#endif
  }


  void StaffrollApplicationSystem::Build( App::Staffroll::System::StaffrollWork* pStaffrollWork, App::Staffroll::System::StaffrollBuilder* pApplicationSystemBuilder )
  {
    GFL_ASSERT( pStaffrollWork );

    m_pWork = pStaffrollWork;

    //  フェードは自前でやるので強制フェードはなし
    NonUseFadeRequest();

    //  上下画面の色を黒にする（終了したら元に戻す）
    {
      GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

      u32 loopNum = GFL_NELEMS( m_aOldClearSetting );
      GameSys::GameManager::BUFFER_CLEAR_SETTING  newClearSetting;
      for( u32 i = 0; i < loopNum; ++i )
      {
        pGameManager->GetBufferClearSetting( (gfl2::gfx::CtrDisplayNo)i, &m_aOldClearSetting[ i ] );

        //  copy
        {
          newClearSetting = m_aOldClearSetting[ i ];
          newClearSetting.color.SetR( 0.0f );
          newClearSetting.color.SetG( 0.0f );
          newClearSetting.color.SetB( 0.0f );
        }

        pGameManager->SetBufferClearSetting( (gfl2::gfx::CtrDisplayNo)i, newClearSetting );
      }
    }

    app::util::Heap* pAppHeap = m_pWork->GetAppHeap();
    GFL_ASSERT( pAppHeap );

    //  View関連のインスタンス作成
    {
      m_pViewList           = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::System::StaffrollViewList( pAppHeap );
      m_pViewObjects        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::System::StaffrollViewObjects();
      m_pViewSystemAccessor = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::System::StaffrollViewSystemAccessor( m_pViewList, m_pViewObjects );

      //  workにViewのアクセサを参照
      m_pWork->SetViewSystemAccessor( m_pViewSystemAccessor );
    }

    //  画像テクスチャーファイルを開く
/*
    {
      m_pImgFileCtrlState  = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( m_pWork->GetImgTexArcId(), pAppHeap->GetDeviceHeap(), m_pWork->IsImgTexFileComp(), 128 );
    }
*/

    //  バトルフォントを設定
    {
      print::SystemFont_SetLytSystemBattleFont();
    }

    ApplicationSystemBase::Build( pApplicationSystemBuilder );
  }


  void StaffrollApplicationSystem::LoadResourceRequest()
  {
    //  レイアウトデータをロード
    {
      m_pResourceManager->AddLoadRequest( App::Staffroll::STAFFROLL_RESOURCE_ID_LAYOUT, ARCID_STAFF_ROLL_LYT_DATA, GARC_StaffrollLyt_StaffrollLyt_applyt_COMP, true, true );

      //  グラフィックフォントをロード
      //  日本語と英語しか出さないがデータ９言語分に日本語と英語のみのフォントを用意するのでプログラム側で指定の必要はない
      {
        m_pResourceManager->AddLoadRequest( App::Staffroll::STAFFROLL_RESOURCE_ID_GRPFONT_LAYOUT, ARCID_STAFFROLL_GRPFONT, GARC_StaffrollGrpFont_StaffrollGrpFont_applyt_COMP, true, true );
      }
    }

    //  イベントデータをロード
    {
      //  テキストイベントをロード
      m_pResourceManager->AddLoadRequest( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_TEXT_DATA, ARCID_STAFF_ROLL_EVENT_DATA, GARC_staffroll_data_staffroll_data_DAT, false, true );
      m_pResourceManager->AddLoadRequest( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_TEXT_CONFIG_DATA, ARCID_STAFF_ROLL_EVENT_DATA, GARC_staffroll_data_staffroll_data_param_DAT, false, true );

      //  １枚絵イベントをロード
      m_pResourceManager->AddLoadRequest( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_IMG_DATA, ARCID_STAFF_ROLL_EVENT_DATA, GARC_staffroll_data_staffroll_data_img_evt_DAT, false, true );
    }
    
    //  メッセージのロード
    {
      m_pResourceManager->AddLoadRequest( App::Staffroll::STAFFROLL_RESOURCE_ID_MESSAGE, print::GetMessageArcId(), GARC_message_staff_list_DAT, false, true );
    }

    //  画像テクスチャーファイルを開く
//    m_pImgFileCtrlState->Start();
  }

  void StaffrollApplicationSystem::CreateMessageData()
  {
    //  メッセージ制御を作成
    m_pMessageUtility->LoadFromMemory( GARC_message_staff_list_DAT, m_pResourceManager->GetResourceBuffer( App::Staffroll::STAFFROLL_RESOURCE_ID_MESSAGE ) );
    m_pWork->SetMessageUtility( m_pMessageUtility );
  }


  void StaffrollApplicationSystem::CreateView()
  {
    //  ここにきたらリソースのロードは終っている
    {
      //  データ設定
      size_t size = m_pWork->GetResourceManager()->GetResourceSize( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_TEXT_CONFIG_DATA );
      GFL_ASSERT( size != 0 );
      
      // 開発中にエラーを見付けるためのASSERT。
      GFL_ASSERT( size == sizeof(StaffrollParamBinaryFormat) );

      StaffrollParamBinaryFormat* pParam = (StaffrollParamBinaryFormat*)m_pWork->GetResourceManager()->GetResourceBuffer( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_TEXT_CONFIG_DATA );
      GFL_ASSERT( pParam != NULL );

      m_pWork->SetEventTextParam( *pParam );
    }
  }


  void StaffrollApplicationSystem::UpdateView()
  {
  /*
    s32 playSpeed = m_pWork->GetPlaySpeed();
    for( s32 i = 0; i < playSpeed; ++i )
    {
      m_pViewList->Update();
    }
  */
#if PM_DEBUG
    app::util::Heap * heap = m_pWork->GetAppHeap();
    u32 sys_size = heap->GetSystemHeap()->GetTotalFreeSize();
    u32 dev_size = heap->GetDeviceHeap()->GetTotalFreeSize();
    if( sys_size < m_debugHeapSize )
    {
      m_debugHeapSize = sys_size;
    }
    if( dev_size < m_debugDevHeapSize )
    {
      m_debugDevHeapSize = dev_size;
    }
#endif
  }


  void StaffrollApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
  {
    m_pViewList->Draw( displayNo );
  }


  bool StaffrollApplicationSystem::IsExitView()
  {
    bool bExit = true;

    if( m_pViewObjects )
    {
      if( m_pViewObjects->IsDrawing() )
      {
        bExit = false;
      }
    }

    return bExit;
  }

  //! @brief アプリケーションの終了処理を行います。
  //! @retval true 終了処理中
  //! @retval false 終了処理完了
  bool StaffrollApplicationSystem::End()
  {
    if( m_endSeq == 0 )
    {
      //  最初にViewの破棄をする
      if( m_pViewObjects->EndFunc() == false )
      {
        return  true;
      }

      ++m_endSeq;
    }
    else
    if( m_endSeq == 1 )
    {
//      m_pImgFileCtrlState->End();

      ++m_endSeq;
    }
    else
    if( m_endSeq == 2 )
    {
/*
      m_pImgFileCtrlState->Update();
      if( m_pImgFileCtrlState->IsEnd() == false )
      {
        return  true;
      }
*/
      ++m_endSeq;
    }
    else
    {
      if( NetAppLib::System::ApplicationSystemBase::End() == false )
      {
        //  上下画面の色を元に戻す
        {
          GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

          u32 loopNum = GFL_NELEMS( m_aOldClearSetting );
          for( u32 i = 0; i < loopNum; ++i )
          {
            pGameManager->SetBufferClearSetting( (gfl2::gfx::CtrDisplayNo)i, m_aOldClearSetting[ i ] );
          }
        }

        //  バトルフォントを消す
        print::SystemFont_ResetDefaultFontInfo();

        return  false;
      }
    }

    return  true;
  }

  //! @brief ビューの作成が完了したか
  bool StaffrollApplicationSystem::IsCreateViewComplete()
  {
    bool  bComplete = true;
    if( m_pViewObjects->InitFunc( m_pWork ) == false )
    {
      bComplete = false;
    }
/*
    m_pImgFileCtrlState->Update();
    if( m_pImgFileCtrlState->IsManualIdleState() == false )
    {
      bComplete = false;
    }
*/

    return bComplete;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
