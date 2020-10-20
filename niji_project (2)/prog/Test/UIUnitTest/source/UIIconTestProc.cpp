#if PM_DEBUG

#include "Test/UiUnitTest/include/UIIconTestProc.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>

#include <System/include/HeapDefine.h>

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_btl_app.h>

// arcファイル一覧
#include <arc_def_index/arc_def.h>
// 使用するarcファイル対象のデータ一覧
#include <arc_index/uiview_test.gaix>

#include <niji_conv_header/app/test/uchida_test/lyt_test/res2d/uiview_test.h>
#include <niji_conv_header/app/test/uchida_test/lyt_test/res2d/uiview_test_pane.h>
#include <niji_conv_header/app/test/uchida_test/lyt_test/res2d/uiview_test_anim_list.h>

#include "AppLib/include/Tool/AppCommonGrpIconData.h"

GFL_NAMESPACE_BEGIN(Test)

    /** テスト用UIView */
    class UIViewTest: public app::ui::UIView, public app::ui::UIInputListener
    {
    public:
      UIViewTest( app::util::Heap * heap, app::util::AppRenderingManager* pRenderingManager, void* pLytBuff ) :
        UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
      {
        GFL_ASSERT( pLytBuff );
        GFL_ASSERT( heap );

        m_pHeap = heap;

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
                LYTRES_UIVIEW_TEST_COMMON_ICN_002_BFLYT,
                LA_UIVIEW_TEST_COMMON_ICN_002___NUM,
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

      virtual ~UIViewTest()
      {
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

      void RegisterTex( app::tool::AppCommonGrpIconData& rCommonGrpIcon )
      {
        gfl2::lyt::LytWk* pLayoutWork = UIView::GetLayoutWork( 0 );

        //  アイコン貼り付け
        {
          gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_COMMON_ICN_002_PANE_ICON_SICK );
          GFL_ASSERT( pPicPane );

          rCommonGrpIcon.ReplacePaneTextureByPokeHinshiIcon( pPicPane );
        }

        //  newアイコン
        {
          gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_COMMON_ICN_002_PANE_ICON_NEW );
          GFL_ASSERT( pPicPane );

          rCommonGrpIcon.ReplacePaneTextureByNewIcon( pPicPane );          
        }

        //  状態異常
        {
          const s32 aData[][ 2 ] =
          {
            { PANENAME_COMMON_ICN_002_PANE_ICON_SICK, pml::pokepara::SICK_MAHI },
            { PANENAME_COMMON_ICN_002_PANE_ICON_SICK_00, pml::pokepara::SICK_NEMURI },
            { PANENAME_COMMON_ICN_002_PANE_ICON_SICK_01, pml::pokepara::SICK_KOORI },

            { -1, -1 }
          };

          for( u32 i = 0; aData[ i ][ 0 ] != -1; ++i )
          {
            gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( aData[ i ][ 0 ] );
            GFL_ASSERT( pPicPane );

            rCommonGrpIcon.ReplacePaneTextureByPokeSickIcon( (pml::pokepara::Sick)aData[ i ][ 1 ], pPicPane );
          }

          //  どくどく表示
          {
            gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_COMMON_ICN_002_PANE_ICON_SICK_02 );
            GFL_ASSERT( pPicPane );

            rCommonGrpIcon.ReplacePaneTextureByPokeDokudokuIcon( pPicPane );
          }

          //  ポケルス表示
          {
            gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( PANENAME_COMMON_ICN_002_PANE_ICON_SICK_03 );
            GFL_ASSERT( pPicPane );

            rCommonGrpIcon.ReplacePaneTextureByPokerusIcon( pPicPane );
          }
        }

        //  わざタイプ
        {
          const s32 aData[][ 2 ] =
          {
            { PANENAME_COMMON_ICN_002_PANE_ICON_SKILLTYPE,     POKETYPE_NORMAL },
            { PANENAME_COMMON_ICN_002_PANE_ICON_SKILLTYPE_00,  POKETYPE_KAKUTOU },
            { PANENAME_COMMON_ICN_002_PANE_ICON_SKILLTYPE_01,  POKETYPE_HIKOU },
            { PANENAME_COMMON_ICN_002_PANE_ICON_SKILLTYPE_02,  POKETYPE_DOKU },

            { -1, -1 }
          };

          for( u32 i = 0; aData[ i ][ 0 ] != -1; ++i )
          {
            gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( aData[ i ][ 0 ] );
            GFL_ASSERT( pPicPane );

            rCommonGrpIcon.ReplacePaneTextureByWazaTypeIcon( (PokeType_tag)aData[ i ][ 1 ], pPicPane );
          }
        }

        //  言語タイプ
        {
          const s32 aData[][ 2 ] =
          {
            { PANENAME_COMMON_ICN_002_PANE_LANG_00,  POKEMON_LANG_JAPAN },
            { PANENAME_COMMON_ICN_002_PANE_LANG_01,  POKEMON_LANG_ENGLISH },
            { PANENAME_COMMON_ICN_002_PANE_LANG_02,  POKEMON_LANG_FRANCE },
            { PANENAME_COMMON_ICN_002_PANE_LANG_03,  POKEMON_LANG_ITALY },
            { PANENAME_COMMON_ICN_002_PANE_LANG_04,  POKEMON_LANG_GERMANY },
            { PANENAME_COMMON_ICN_002_PANE_LANG_05,  POKEMON_LANG_SPAIN },
            { PANENAME_COMMON_ICN_002_PANE_LANG_06,  POKEMON_LANG_KOREA },
            { PANENAME_COMMON_ICN_002_PANE_LANG_08,  POKEMON_LANG_CHINA },
            { PANENAME_COMMON_ICN_002_PANE_LANG_07,  POKEMON_LANG_TAIWAN },
            { -1, -1 }
          };

          for( u32 i = 0; aData[ i ][ 0 ] != -1; ++i )
          {
            gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( aData[ i ][ 0 ] );
            GFL_ASSERT( pPicPane );

            rCommonGrpIcon.ReplacePaneTextureByLangIcon( aData[ i ][ 1 ], pPicPane );
          }
        }

        //  レーダーチャートのグラフ項目
        {
          const s32 aData[][ 2 ] =
          {
            { PANENAME_COMMON_ICN_002_PANE_GRAPHNAME_00,  pml::pokepara::POWER_HP },
            { PANENAME_COMMON_ICN_002_PANE_GRAPHNAME_01,  pml::pokepara::POWER_ATK },
            { PANENAME_COMMON_ICN_002_PANE_GRAPHNAME_02,  pml::pokepara::POWER_DEF },
            { PANENAME_COMMON_ICN_002_PANE_GRAPHNAME_03,  pml::pokepara::POWER_SPATK },
            { PANENAME_COMMON_ICN_002_PANE_GRAPHNAME_04,  pml::pokepara::POWER_SPDEF },
            { PANENAME_COMMON_ICN_002_PANE_GRAPHNAME_05,  pml::pokepara::POWER_AGI },

            { -1, -1 }
          };

          for( u32 i = 0; aData[ i ][ 0 ] != -1; ++i )
          {
            gfl2::lyt::LytPicture*  pPicPane  = pLayoutWork->GetPicturePane( aData[ i ][ 0 ] );
            GFL_ASSERT( pPicPane );

            rCommonGrpIcon.ReplacePaneTextureByPokeParamIcon( (pml::pokepara::PowerID)aData[ i ][ 1 ], pPicPane );
          }
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
          //  テクスチャー切り替え

        }

        return ENABLE_ACCESS;
      }

      bool IsProcEnd() const { return m_bProcEnd; }

    private:
      bool  m_bProcEnd;

      app::util::Heap*  m_pHeap;
      app::util::AppRenderingManager* m_pAppRenderingManager;
    };

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UIIconTestProc::InitFunc(gfl2::proc::Manager* pManager)
{
  int seq = BaseProcess::GetSubSeq();

  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  if( UIUnitTestBaseProc::InitFunc( pManager ) == gfl2::proc::RES_CONTINUE )
  {
    return  gfl2::proc::RES_CONTINUE;
  }

  if( seq == 0 )
  {
    _Claer();

    m_pFileLoadState  = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( ARCID_UIVIEW_TEST_LAYOUT, GARC_uiview_test_uiview_test_APPLYT, mpAppHeap->GetDeviceHeap(), false, 128 );
    m_pFileLoadState->Start();

    m_pAppCommonGrpIcon = GFL_NEW( mpAppHeap->GetDeviceHeap() ) app::tool::AppCommonGrpIconData();
    m_pAppCommonGrpIcon->Initialize( mpAppHeap );

    ++seq;
  }
  
  if( seq == 1 )
  {
    m_pFileLoadState->Update();

    if( ( m_pFileLoadState->IsEnd() == true ) && ( m_pAppCommonGrpIcon->IsReady() == true ) )
    {
      
      UIViewTest* pView = GFL_NEW( mpAppHeap->GetDeviceHeap() ) UIViewTest( mpAppHeap, mpAppRenderingManager, m_pFileLoadState->GetLoadDataBuffer() );

      //  レイアウトシステムにテクスチャーをロードして登録する
      pView->RegisterTex( *m_pAppCommonGrpIcon );

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
applib::frame::CellSuper* UIIconTestProc::OnCreateFrame( applib::frame::Manager* pFrameManager, GameSys::GameManager* pGameManager )
{
  return NULL;
}

//------------------------------------------------------------------
/**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UIIconTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  UIUnitTestBaseProc::UpdateFunc( pManager );

  GFL_UNUSED( pManager );
  UIViewTest* pViewTest = reinterpret_cast<UIViewTest*>( m_pView );

  pViewTest->UpdateTree();

  gfl2::ui::DeviceManager*                pDeviceManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button*                       pButton         = pDeviceManager->GetButton(0);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    return  gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void UIIconTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_ASSERT( m_pView );

  m_pView->DrawTree( displayNo );

  UIUnitTestBaseProc::DrawFunc( pManager, displayNo );
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result UIIconTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result  result  = gfl2::proc::RES_CONTINUE;

  u32 seq = GetSubSeq();
  if( seq == 0 )
  {
    if( m_pView->IsDrawing() == false )
    {
      ++seq;
    }
  }

  if( seq == 1 )
  {
    if( m_pAppCommonGrpIcon != NULL )
    {
      m_pAppCommonGrpIcon->Terminate();
    }

    GFL_SAFE_DELETE( m_pAppCommonGrpIcon );
    GFL_SAFE_DELETE( m_pView );
    GFL_SAFE_DELETE( m_pFileLoadState );

    ++seq;
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
