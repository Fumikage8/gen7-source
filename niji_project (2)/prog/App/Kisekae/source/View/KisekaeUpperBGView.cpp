// ============================================================================
/*
 * @file KisekaeUpperBGView.cpp
 * @brief きせかえアプリの下画面を表示するクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/View/KisekaeUpperBGView.h"

#include "App/Kisekae/source/KisekaeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
//  nijiのインクルード
#include "Savedata/include/MiscSave.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData.h>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData_anim_list.h>
#include <niji_conv_header/app/shop_kisekae/res2d/ShopKisekaeLytData_pane.h>

#include <niji_conv_header/message/msg_kisekae.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(View)

  UpperBGView::UpperBGView( App::Kisekae::System::Work* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::Kisekae::RESOURCE_ID_LAYOUT,
      LYTRES_SHOPKISEKAELYTDATA_SHOPKISEKAE_MIN_UPP_000_BFLYT,
      LA_SHOPKISEKAELYTDATA_SHOPKISEKAE_MIN_UPP_000___NUM,
      app::util::G2DUtil::SETUP_UPPER,
      pWork->GetMessageUtility(),
      GARC_message_kisekae_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
    GFL_ASSERT( pLayoutWork );

    app::util::GetPaneHelper  helper( pLayoutWork, &rResourceID );

    //  所持金ウィンドウペイン取得
    {
      m_pMoneyWindowPane  = helper.GetPane( PANENAME_SHOPKISEKAE_MIN_UPP_000_PANE_WALLETICON );
      GFL_ASSERT( m_pMoneyWindowPane );
    }

    //  所持金数字テキストペイン取得
    {
      m_pMoenyWindowNumTextPane = helper.GetTextBoxPane( PANENAME_SHOPKISEKAE_MIN_UPP_000_PANE_TEXTBOX_01 );
      GFL_ASSERT( m_pMoenyWindowNumTextPane );
    }

    //  きせかえアイテム名のテキストペイン取得
    {
      m_pItemNameTextPane = helper.GetTextBoxPane( PANENAME_SHOPKISEKAE_MIN_UPP_000_PANE_TEXTBOX_03 );
      GFL_ASSERT( m_pItemNameTextPane );

      pG2DUtil->SetTextBoxPaneString( m_pItemNameTextPane, GFLSTR( "" ) );
    }

    //  きせかえアイテム名のテキストフレームのペイン取得
    {
      m_pItemNameFramePane  = helper.GetPane( PANENAME_SHOPKISEKAE_MIN_UPP_000_PANE_BG_FITTING );
      GFL_ASSERT( m_pItemNameFramePane );
      pG2DUtil->SetPaneVisible( m_pItemNameFramePane, false );
    }

    //  きせかえアイテム名の色情報テキストペイン取得
    {
      m_pItemColorNameTextPane = helper.GetTextBoxPane( PANENAME_SHOPKISEKAE_MIN_UPP_000_PANE_TEXTBOX_02 );
      GFL_ASSERT( m_pItemColorNameTextPane );

      pG2DUtil->SetTextBoxPaneString( m_pItemColorNameTextPane, GFLSTR( "" ) );
    }

    //  カメラ入力チュートリアルペイン取得
    {
      m_pUserInputTutorialIconPane  = helper.GetPane( PANENAME_SHOPKISEKAE_MIN_UPP_000_PANE_CAMERAICON );
      GFL_ASSERT( m_pUserInputTutorialIconPane );
      pG2DUtil->SetPaneVisible( m_pUserInputTutorialIconPane, false );
    }

    //  デフォルトモード
    SetGoldMode( MONEY_MODE_TYPE_GOLD );

    _SetMoneyText( 0 );

  }

  UpperBGView::~UpperBGView()
  {
  }

  void UpperBGView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
  }

  //  金額の単位モード設定
  void UpperBGView::SetGoldMode( const MoneyMode mode )
  {
    m_moneyMode = mode;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
    GFL_ASSERT( pLayoutWork );

    //  固定テキスト設定
    {
        NetAppLib::Message::MessageUtility* pMessageUtility = m_pWork->GetMessageUtility();
        GFL_ASSERT( pMessageUtility );

      //  おこづかい(ゴールド)
      u32 moneyTitleTxt = kisekae_01_02;
      if( m_moneyMode == MONEY_MODE_TYPE_GOLD )
      {
        moneyTitleTxt = kisekae_01_02;
      }
      else
      if( m_moneyMode == MONEY_MODE_TYPE_FC )
      {
        moneyTitleTxt = kisekae_10_01;
      }

      app::util::GetPaneHelper  helper( pLayoutWork, &rResourceID );

      gfl2::str::StrBuf&  rText = pMessageUtility->GetString( GARC_message_kisekae_DAT, moneyTitleTxt );
      pG2DUtil->SetTextBoxPaneString( helper.GetTextBoxPane( PANENAME_SHOPKISEKAE_MIN_UPP_000_PANE_TEXTBOX_00 ), &rText );
    }
  }

  //  所持金表示
  void UpperBGView::ShowMoneyWindow( const u32 initMoneyNum )
  {
    GFL_ASSERT( m_pMoneyWindowPane );

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pMoneyWindowPane, true );

    //  所持金表示
    _SetMoneyText( initMoneyNum );
  }

  //  所持金値を設定
  void UpperBGView::SetMoneyNum( const u32 moneyNum )
  {
    GFL_ASSERT( m_pMoneyWindowPane );

    _SetMoneyText( moneyNum );
  }

  //  所持金非表示
  void UpperBGView::HideMoneyWindow()
  {
    GFL_ASSERT( m_pMoneyWindowPane );

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pMoneyWindowPane, false );
  }

  //  アイテム関連の名前表示
  void UpperBGView::SetItemNameViaible( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pItemNameTextPane, bFlg );
    pG2DUtil->SetPaneVisible( m_pItemNameFramePane, bFlg );
  }

  //  アイテム関連の名前設定
  void UpperBGView::SetItemName( const s32 itemTxtId, const s32 colorTxtId )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWork->GetMessageUtility();

    // @fix 守屋さん要望　着せ替え画面中にカーテン表示するために今まで表示フレームを非表示設定しているところをテキストのみ非表示するように変更

    //  アイテム名表示
    {
      pG2DUtil->SetPaneVisible( m_pItemNameTextPane, true );
      if( itemTxtId < 0 )
      {
        // @fix GFNMCat[4917] テキストを非表示でなく、空にすることで見えないように変更
        pG2DUtil->SetTextBoxPaneString( m_pItemNameTextPane, GFLSTR( "" ) );
      }
      else
      {
        print::WordSet* pWordSet  = pG2DUtil->GetWordSet();
        if( pWordSet == NULL )
        {
          GFL_ASSERT( 0 );
        }
        else
        {
          //  きせかえの購入個数は常に１つなので第３引数の購入個数値は１でよい
          pWordSet->RegisterKisekaeItemName( 0, itemTxtId, 1 );
          pG2DUtil->SetTextBoxPaneStringExpand( m_pItemNameTextPane, kisekae_01_01 );
        }
      }
    }

    pG2DUtil->SetPaneVisible( m_pItemColorNameTextPane, true );
    if( colorTxtId < 0 )
    {
      // @fix GFNMCat[4917] テキストを非表示でなく、空にすることで見えないように変更
      pG2DUtil->SetTextBoxPaneString( m_pItemColorNameTextPane, GFLSTR( "" ) );
    }
    else
    {
      pG2DUtil->SetRegisterWord( 0, pMessageUtility->GetString( App::Kisekae::System::ItemList::GetColorTextMsTxtArcDatId(), (u32)colorTxtId ) );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pItemColorNameTextPane, kisekae_01_08 );
    }
  }

  //  ユーザー操作説明アイコン表示設定
  void UpperBGView::SetVisibleUserInputTutorialIcon( const bool bFlg )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pUserInputTutorialIconPane, bFlg );
  }

  //  所持金をテキスト表示設定
  void UpperBGView::_SetMoneyText( const u32 moneyNum )
  {
    GFL_ASSERT( m_pMoneyWindowPane );
    GFL_ASSERT( m_pMoenyWindowNumTextPane );

    NetAppLib::Message::MessageUtility* pMessageUtility = m_pWork->GetMessageUtility();
    GFL_ASSERT( pMessageUtility );

    u32 moneyTitleTxt = kisekae_01_03;
    if( m_moneyMode == MONEY_MODE_TYPE_GOLD )
    {
      moneyTitleTxt = kisekae_01_03;
    }
    else
    if( m_moneyMode == MONEY_MODE_TYPE_FC )
    {
      moneyTitleTxt = kisekae_10_02;
    }

    gfl2::str::StrBuf&  rText = pMessageUtility->GetString( GARC_message_kisekae_DAT, moneyTitleTxt );

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    pG2DUtil->SetTextBoxPaneNumber( m_pMoenyWindowNumTextPane, &rText, moneyNum, Savedata::Misc::MY_GOLD_KETA, 0, print::NUM_DISP_SPACE, print::NUM_CODE_DEFAULT );
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
