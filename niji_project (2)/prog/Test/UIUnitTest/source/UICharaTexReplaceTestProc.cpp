#if PM_DEBUG

#include "Test/UiUnitTest/include/UICharaTexReplaceTestProc.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>

#include <System/include/HeapDefine.h>
#include <model/include/gfl2_DressUpModel.h>

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_btl_app.h>

// arcファイル一覧
#include <arc_def_index/arc_def.h>
// 使用するarcファイル対象のデータ一覧
#include <arc_index/uiview_test.gaix>

// 着せ替えのインデックス
#include <niji_conv_header/chara_test/dressup_item_id.h>
#include <niji_conv_header/chara_test/dressup_color_id.h>

// モーションのインデックス
#include <niji_conv_header/battle/chara/p2_base.h>

// モデルインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>

#include <niji_conv_header/app/test/uchida_test/lyt_test/res2d/uiview_test.h>
#include <niji_conv_header/app/test/uchida_test/lyt_test/res2d/uiview_test_pane.h>
#include <niji_conv_header/app/test/uchida_test/lyt_test/res2d/uiview_test_anim_list.h>

#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

#include "GameSys/include/GameData.h"

GFL_NAMESPACE_BEGIN(Test)

    /** テスト用UIView */
    class UICharaTexReplaceViewTest: public app::ui::UIView, public app::ui::UIInputListener
    {
    public:
      UICharaTexReplaceViewTest( app::util::Heap * heap, app::util::AppRenderingManager* pRenderingManager, void* pLytBuff ) :
        UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
      {
        GFL_ASSERT( pLytBuff );
        GFL_ASSERT( heap );

        m_pHeap = heap;

        //  トレーナーアイコンテクスチャー
        {
          m_pTranerIconTexture01  = GFL_NEW( m_pHeap->GetDeviceHeap() ) app::tool::AppToolTrainerIconTexture( m_pHeap->GetDeviceHeap() );
          m_pTranerIconTexture02  = GFL_NEW( m_pHeap->GetDeviceHeap() ) app::tool::AppToolTrainerIconTexture( m_pHeap->GetDeviceHeap() );
        }

        //  レイアウト作成
        {
          // アーカイブIDテーブル
          const ::app::util::G2DUtil::LytResData resTbl[] =
          {
            {  pLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE, },
          };

          //  作成するレイアウトワーク
          const ::app::util::G2DUtil::LytwkData setupData[] =
          {
              //  選択画面
              {
                0,
                LYTRES_UIVIEW_TEST_CHARAICONTEST_BFLYT,
                LA_UIVIEW_TEST_CHARAICONTEST___NUM,
                pLytBuff,
                true,
                ::app::util::G2DUtil::SETUP_LOWER,
                true
              },
          };


          //  レイアウト作成
          UIView::Create2D( heap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), UIView::UNUSE_MESSAGE_DATA );
        }

        //  入力レスポンス受け取る設定
        UIResponder::SetInputListener( this );

        GFL_ASSERT( pRenderingManager );
        m_pAppRenderingManager  = pRenderingManager;

        m_bProcEnd  = false;
      }

      virtual ~UICharaTexReplaceViewTest()
      {
        GFL_SAFE_DELETE( m_pTranerIconTexture02 );
        GFL_SAFE_DELETE( m_pTranerIconTexture01 );

        UIView::Delete2D();
      }

      virtual void Update(void)
      {
        UIView::Update();
      }

      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo )
      {
        app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

        // ここですべて描画
        pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, displayNo, 0 );
      }

      void RegisterTexNPC( app::tool::AppToolTrainerIconRendering& rCharaTexPaneReplace, poke_3d::model::BaseCamera* pCamera, const u32 trainerId )
      {
        gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( 0 );

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA data;
        //  デフォルトデータを作成
        app::tool::AppToolTrainerIconRendering::OutputDefaultIconObjectData( 1, &data );

        //  NPCアイコン
        {
          gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL );
          pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL_02 );
          GFL_ASSERT( pPicPane );

          data.pReplacePane  = pPicPane;
          data.trainerId = trainerId;
          rCharaTexPaneReplace.SetIconObjectData( 3, data );
        }
      }

      void RegisterTex( app::tool::AppToolTrainerIconRendering& rCharaTexPaneReplace, poke_3d::model::BaseCamera* pCamera )
      {
        gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( 0 );

        // 着せ替えモデル用のパーツを非同期読み込みする
        // 適当な着せ替え設定
        poke_3d::model::DressUpParam dressUpParam;
        gfl2::std::MemClear( &dressUpParam, sizeof( dressUpParam ) );

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA data;
        //  デフォルトデータを作成
        app::tool::AppToolTrainerIconRendering::OutputDefaultIconObjectData( 1, &data );

        gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL );
        /*
        //  デフォルトアイコン(デフォルトパラメータではモデルがうまく生成できない)
        {
          GFL_ASSERT( pPicPane );
          data.pReplacePane  = pPicPane;
          rCharaTexPaneReplace.SetIconObjectData( 2, data );
        }

        //  タイマーアイコンを固定位置に
        {
          pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL_00 );
          GFL_ASSERT( pPicPane );
        
          data.pReplacePane  = pPicPane;
          gfl2::math::Vector3 timerPos( 0, 60, 0 );
          rCharaTexPaneReplace.SetIconObjectData( 1, data, timerPos );
        }
        */

        //  プレイヤーのステータスからアイコン表示
        {
          pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL_01 );
          GFL_ASSERT( pPicPane );

          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  myStatusIconData;
          myStatusIconData.pReplacePane  = pPicPane;

          //  ステータスからアイコン表示に必要なデータをコンバート
          {
            Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
            myStatusIconData.ConvFromMyStatus( *pMyStatus );
          }

          rCharaTexPaneReplace.SetIconObjectData( 0, myStatusIconData );
        }
      }

      void RegisterTexNPCOnly( app::tool::AppToolTrainerIconRendering& rCharaTexPaneReplace )
      {
        gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( 0 );

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA data;
        //  デフォルトデータを作成
        app::tool::AppToolTrainerIconRendering::OutputDefaultIconObjectData( 1, &data );

        gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL );
        GFL_ASSERT( pPicPane );
        data.pReplacePane  = pPicPane;
        data.trainerId = 3;
        rCharaTexPaneReplace.SetIconObjectData( 0, data );
      }

      //  テクスチャー貼り付けをして同時にテクスチャーデータを出力
      void RegisterTexOutputTexture( app::tool::AppToolTrainerIconRendering& rCharaTexPaneReplace, poke_3d::model::BaseCamera* pCamera )
      {
        gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( 0 );

        // 着せ替えモデル用のパーツを非同期読み込みする
        // 適当な着せ替え設定
        poke_3d::model::DressUpParam dressUpParam;
        gfl2::std::MemClear( &dressUpParam, sizeof( dressUpParam ) );

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA data;
        //  デフォルトデータを作成
        app::tool::AppToolTrainerIconRendering::OutputDefaultIconObjectData( 1, &data );


        //  タイマーアイコンを固定位置に
        if( m_pTranerIconTexture01->IsValid() == false )
        {
          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  myStatusIconData;

          //  ステータスからアイコン表示に必要なデータをコンバート
          {
            Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
            myStatusIconData.ConvFromMyStatus( *pMyStatus );
          }

          myStatusIconData.pReplacePane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL_00 );

          rCharaTexPaneReplace.SetIconObjectDataByOutputTexture( 1, myStatusIconData, true, m_pTranerIconTexture01 );
        }
        else
        {
          gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL_01 );
          m_pTranerIconTexture01->ReplacePicPane( pPicPane );
        }

        //  NPCアイコン
        if( m_pTranerIconTexture02->IsValid() == false )
        {
          data.pReplacePane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL_02 );
          data.trainerId = 3;
          rCharaTexPaneReplace.SetIconObjectDataByOutputTexture( 3, data, true, m_pTranerIconTexture02 );
        }
        else
        {
          gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_CHARAICONTEST_PANE_TYPE_ICON_00_NORMAL );
          m_pTranerIconTexture02->ReplacePicPane( pPicPane );
        }
      }

      //--------------------------------------------------------------------------------------------
      /**
      * @brief  タッチパネルイベントの検知
      *
      * @param  pTouchPanel タッチパネル
      * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
      *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
      *
      * @return 同フレーム内での他入力イベントを許可するならtrue、
      *         他のイベントを排除するならfalseを返却すること。
      */
      //--------------------------------------------------------------------------------------------
      virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
      {
        GFL_UNUSED( pTouchPanel );

        GFL_PRINT( "OnTouchEvent\n" );
        return ENABLE_ACCESS;
      }

      //--------------------------------------------------------------------------------------------
      /**
      * @brief   タッチパネルのダブルクリックイベントの検知
      *
      * @param  pTouchPanel タッチパネル
      *
      * @return 同フレーム内での他入力イベントを許可するならtrue、
      *         他のイベントを排除するならfalseを返却すること。
      */
      //--------------------------------------------------------------------------------------------
      virtual app::ui::UIInputListener::ListenerResult OnDoubleClickEvent( gfl2::ui::TouchPanel * pTouchPanel )
      {
        GFL_UNUSED( pTouchPanel );

        GFL_PRINT( "OnDoubleClickEvent\n" );
        return ENABLE_ACCESS;
      }

      //--------------------------------------------------------------------------------------------
      /**
      * @brief ペインイベントの検知
      *       【ボタンマネージャー関係】
      *
      * @param  painId   通知を受けるペインＩＤ
      *
      * @return 同フレーム内での他入力イベントを許可するならtrue、
      *         他のイベントを排除するならfalseを返却すること。
      */
      //--------------------------------------------------------------------------------------------
      virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId )
      {
        GFL_PRINT( "OnLayoutPaneEvent : painId = %d\n", painId );
        return ENABLE_ACCESS;
      }

      //--------------------------------------------------------------------------------------------
      /**
      * @brief  キー入力の検知
      *         ※タッチパネルの入力が優先される
      *
      * @param  pKey    十字キー
      * @param  pStick  アナログスティック
      *
      * @return 同フレーム内での他入力イベントを許可するならtrue、
      *         他のイベントを排除するならfalseを返却すること。
      */
      //--------------------------------------------------------------------------------------------
      virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
      {
        if( pKey->IsTrigger( gfl2::ui::BUTTON_B ) )
        {
          m_bProcEnd  = true;
        }
        else
        if( pKey->IsTrigger( gfl2::ui::BUTTON_A ) )
        {
        }

        return ENABLE_ACCESS;
      }

      bool IsProcEnd() const { return m_bProcEnd; }

    private:
      bool  m_bProcEnd;

      app::util::Heap*  m_pHeap;
      app::util::AppRenderingManager* m_pAppRenderingManager;
      app::tool::AppToolTrainerIconTexture* m_pTranerIconTexture01;
      app::tool::AppToolTrainerIconTexture* m_pTranerIconTexture02;
    };

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UICharaTexReplaceTestProc::InitFunc(gfl2::proc::Manager* pManager)
{
  int seq = BaseProcess::GetSubSeq();

  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  if( UIUnitTestBaseProc::InitFunc( pManager ) == gfl2::proc::RES_CONTINUE )
  {
    return  gfl2::proc::RES_CONTINUE;
  }

  if( seq == 0 )
  {
    m_pFileLoadState  = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( ARCID_UIVIEW_TEST_LAYOUT, GARC_uiview_test_uiview_test_APPLYT, mpAppHeap->GetDeviceHeap(), false, 128 );
    m_pFileLoadState->Start();

    //  モデル用カメラ作成して設定
    app::util::AppRenderingManager::CreateDefaultCamera( mpAppHeap, &m_pDrawEnvNodeOfOffScreenCamera, &m_pOffScreenCamera, app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );
    app::util::AppRenderingManager::CreateDefaultCamera( mpAppHeap, &m_pDrawEnvNodeOfUppScreenCamera, &m_pUppScreenCamera, app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );

    {
      gfl2::math::Vector3 camPos = m_pOffScreenCamera->GetPosition();
      camPos.SetX( 0.0f );
      camPos.SetY( 129.0f );
      camPos.SetZ( 66.0f );

      m_pOffScreenCamera->SetPosition( camPos );
      m_pUppScreenCamera->SetPosition( camPos );
    }

    mpAppRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pUppScreenCamera );

    m_pAppPlayerIconRendering = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::tool::AppToolTrainerIconRendering();
    //  下画面にアイコン表示
    //  表示アイコン最大数４
    if( m_bNPCOnly == false )
    {
      m_pAppPlayerIconRendering->Initialize( mpAppRenderingManager, mpAppHeap, GetGameManager()->GetAsyncFileManager(), 4, app::util::G2DUtil::SETUP_LOWER );
    }
    else
    {
      m_pAppPlayerIconRendering->InitializeByNPCOnly( mpAppRenderingManager, mpAppHeap, GetGameManager()->GetAsyncFileManager(), 4, app::util::G2DUtil::SETUP_LOWER );
    }

    ++seq;
  }
  
  if( seq == 1 )
  {
    bool  bOk = true;

    m_pFileLoadState->Update();

    if( m_pFileLoadState->IsEnd() == false )
    {
      bOk = false;
    }

    if( m_pAppPlayerIconRendering->IsReady() == false )
    {
      bOk = false;
    }

    if( bOk == true )
    {
      UICharaTexReplaceViewTest* pView = GFL_NEW( mpAppHeap->GetDeviceHeap() ) UICharaTexReplaceViewTest( mpAppHeap, mpAppRenderingManager, m_pFileLoadState->GetLoadDataBuffer() );

      if( m_bOutputTexture == true )
      {
        pView->RegisterTexOutputTexture( *m_pAppPlayerIconRendering, m_pOffScreenCamera );
      }
      else
      if( m_bNPCOnly == false )
      {
        pView->RegisterTex( *m_pAppPlayerIconRendering, m_pOffScreenCamera );
      }
      else
      {
        pView->RegisterTexNPCOnly( *m_pAppPlayerIconRendering );
      }

      m_npcTrainerId  = (u32)trainer::TRTYPE_TANPAN;
      pView->RegisterTexNPC( *m_pAppPlayerIconRendering, m_pOffScreenCamera, m_npcTrainerId );

      m_pView = pView;
      
      ++seq;
    }
  }

  if( seq == 2 )
  {
    result  = gfl2::proc::RES_FINISH;
  }

  BaseProcess::SetSubSeq( seq );

  return result;
}

//  テスト用のフレーム作成
applib::frame::CellSuper* UICharaTexReplaceTestProc::OnCreateFrame( applib::frame::Manager* pFrameManager, GameSys::GameManager* pGameManager )
{
  return NULL;
}

//------------------------------------------------------------------
/**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UICharaTexReplaceTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  UIUnitTestBaseProc::UpdateFunc( pManager );

  GFL_UNUSED( pManager );
  UICharaTexReplaceViewTest* pViewTest = reinterpret_cast<UICharaTexReplaceViewTest*>( m_pView );

  m_pAppPlayerIconRendering->Update();
  pViewTest->UpdateTree();

  gfl2::ui::DeviceManager*                pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*                       pButton         = pDeviceManager->GetButton(0);

  gfl2::math::Vector3 camPos = m_pOffScreenCamera->GetPosition();

  if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
  {
    //  再ロードチェック
    if( m_bOutputTexture == true )
    {
      pViewTest->RegisterTexOutputTexture( *m_pAppPlayerIconRendering, m_pOffScreenCamera );
    }
    else
    {
      pViewTest->RegisterTex( *m_pAppPlayerIconRendering, m_pOffScreenCamera );
    }
  }

  //  L/Rキー入力でＮＰＣアイコン切り替え
  if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
  {
    ++m_npcTrainerId;
    if( trainer::TRTYPE_MAX <= m_npcTrainerId )
    {
      m_npcTrainerId  = (u32)trainer::TRTYPE_TANPAN;
    }

    pViewTest->RegisterTexNPC( *m_pAppPlayerIconRendering, m_pOffScreenCamera, m_npcTrainerId );
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
  {
    --m_npcTrainerId;
    if( m_npcTrainerId < trainer::TRTYPE_TANPAN )
    {
      m_npcTrainerId  = (u32)trainer::TRTYPE_DOCTORNR;
    }

    pViewTest->RegisterTexNPC( *m_pAppPlayerIconRendering, m_pOffScreenCamera, m_npcTrainerId );
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    return  gfl2::proc::RES_FINISH;
  }

  m_pOffScreenCamera->SetPosition( camPos );
  m_pUppScreenCamera->SetPosition( camPos );

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void UICharaTexReplaceTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_ASSERT( m_pView );

  m_pView->DrawTree( displayNo );
  m_pAppPlayerIconRendering->Draw( displayNo );

  UIUnitTestBaseProc::DrawFunc( pManager, displayNo );
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result UICharaTexReplaceTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  u32 seq = GetSubSeq();
  if( seq == 0 )
  {
    m_pAppPlayerIconRendering->Terminate();
    ++seq;
  }

  if( seq == 1 )
  {
    if( ( m_pAppPlayerIconRendering->IsDelete() == true ) && ( m_pView->IsDrawing() == false ) )
    {
      mpAppRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
      mpAppRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );

      GFL_SAFE_DELETE( m_pUppScreenCamera );
      GFL_SAFE_DELETE( m_pOffScreenCamera );
      GFL_SAFE_DELETE( m_pDrawEnvNodeOfUppScreenCamera );
      GFL_SAFE_DELETE( m_pDrawEnvNodeOfOffScreenCamera );
      GFL_SAFE_DELETE( m_pAppPlayerIconRendering );
      GFL_SAFE_DELETE( m_pView );
      GFL_SAFE_DELETE( m_pFileLoadState );

      ++seq;
    }
  }

  if( seq == 2 )
  {
    result  = UIUnitTestBaseProc::EndFunc( pManager );
  }

  SetSubSeq( seq );

  return result;
}

GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
