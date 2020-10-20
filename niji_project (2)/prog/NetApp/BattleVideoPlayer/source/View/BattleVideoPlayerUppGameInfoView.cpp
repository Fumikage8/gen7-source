//======================================================================
/**
 * @file BattleVideoPlayerUppGameInfoView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの上画面ゲーム情報View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "./BattleVideoPlayerUppGameInfoView.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "AppLib/include/Tool/PokeIconTexPool.h"

#include "NetStatic/NetAppLib/include/Util/NetAppConvertUtility.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_pane.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  UppGameInfoView::UppGameInfoView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  UppGameInfoView::~UppGameInfoView()
  {
    //  アイコンテクスチャーデータを作成
    {
      //  左側
      {
        u32 loopNum = GFL_NELEMS( m_pLeftPokeIconTexDataArray );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_SAFE_DELETE( m_pLeftPokeIconTexDataArray[ i ] );
        }
      }

      //  右側
      {
        u32 loopNum = GFL_NELEMS( m_pRightPokeIconTexDataArray );
        for( u32 i = 0; i < loopNum; ++i )
        {
          GFL_SAFE_DELETE( m_pRightPokeIconTexDataArray[ i ] );
        }
      }
    }

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool UppGameInfoView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
      gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      u32 iconId  = 0;
      //  左のポケモンアイコンリスト
      {
        const u32 aPartsPaneId[][ 2 ]  =
        {
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_01, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_01_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_02, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_02_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_03, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_03_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_04, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_04_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_05, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_05_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_06, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_06_DIE }
        };

        u32 loopNum = GFL_NELEMS( m_aLeftPokeIcon );
        for( u32 i = 0; i < loopNum; ++i )
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          paneHelper.Push( aPartsPaneId[ i ][ 0 ] );

          m_aLeftPokeIcon[ i ].pRootPane  = paneHelper.Peek();
          GFL_ASSERT( m_aLeftPokeIcon[ i ].pRootPane );

          m_aLeftPokeIcon[ i ].pPicPane   = paneHelper.GetPicturePane( PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON );
          GFL_ASSERT( m_aLeftPokeIcon[ i ].pPicPane );

          m_aLeftPokeIcon[ i ].dieAnimeId = aPartsPaneId[ i ][ 1 ];
          m_aLeftPokeIcon[ i ].iconId = iconId;

          ++iconId;
        }
      }

      //  右のポケモンアイコンリスト
      {
        const u32 aPartsPaneId[][ 2 ]  =
        {
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_07, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_07_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_08, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_08_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_09, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_09_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_10, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_10_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_11, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_11_DIE },
          { PANENAME_BVP_MIN_UPP_001_PANE_POKEICON_12, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001__POKEICON_12_DIE }
        };

        u32 loopNum = GFL_NELEMS( m_aRightPokeIcon );
        for( u32 i = 0; i < loopNum; ++i )
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          paneHelper.Push( aPartsPaneId[ i ][ 0 ] );

          m_aRightPokeIcon[ i ].pRootPane  = paneHelper.Peek();
          GFL_ASSERT( m_aLeftPokeIcon[ i ].pRootPane );

          m_aRightPokeIcon[ i ].pPicPane   = paneHelper.GetPicturePane( PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON );
          GFL_ASSERT( m_aLeftPokeIcon[ i ].pPicPane );

          m_aRightPokeIcon[ i ].dieAnimeId = aPartsPaneId[ i ][ 1 ];
          m_aRightPokeIcon[ i ].iconId  = iconId;

          ++iconId;
        }
      }

      //  テキストペイン取得
      {
        m_pRuleBtlTitleTextBoxPane   = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_00 );
        GFL_ASSERT( m_pRuleBtlTitleTextBoxPane );

        m_pSakasaBtlTitleTextBoxPane  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_01 );
        GFL_ASSERT( m_pRuleBtlTitleTextBoxPane );

        m_pCodeNumTextBoxPane         = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_08 );
        GFL_ASSERT( m_pCodeNumTextBoxPane );

        m_pPlayeNumTextBoxPane        = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_04 );
        GFL_ASSERT( m_pPlayeNumTextBoxPane );

        m_pDateNumTextBoxPane         = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_05 );
        GFL_ASSERT( m_pDateNumTextBoxPane );

        // @fixTPC要望 バトル施設の殲滅数のテキスト表記を数字とまとめるように変更
        m_pBattleHouseGameCountNumTextBoxPane = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_17 );
        GFL_ASSERT( m_pBattleHouseGameCountNumTextBoxPane );
      }

      pG2DUtil->SetTextBoxPaneString( m_pRuleBtlTitleTextBoxPane, L"" );

      //  固定テキスト設定
      {
        //  さかさかバトル
        {
          pG2DUtil->SetTextBoxPaneString( m_pSakasaBtlTitleTextBoxPane, msg_bvp_sys_up_04 );
        }

        //  再生
        {
          gfl2::lyt::LytTextBox*  pTextBoxPane  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_02 );
          GFL_ASSERT( pTextBoxPane );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, msg_bvp_sys_up_06 );
        }

        //  登録
        {
          gfl2::lyt::LytTextBox*  pTextBoxPane  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_03 );
          GFL_ASSERT( pTextBoxPane );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, msg_bvp_sys_up_07 );
        }

        /*  仕様変更により未使用になった
        //  せんめ
        {
          gfl2::lyt::LytTextBox*  pTextBoxPane  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_17 );
          GFL_ASSERT( pTextBoxPane );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, msg_bvp_sys_up_14 );
        }
        */

        // @fix 寺地さん要望　仕様変更により未使用になった
        /*
        //  コード
        {
          gfl2::lyt::LytTextBox*  pTextBoxPane  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_09 );
          GFL_ASSERT( pTextBoxPane );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, msg_bvp_sys_up_08 );
          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, L"" );
        }
        */

      }

      //  プレイヤー情報ペイン
      {
        //  左左
        {
          PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_LL ];

          pPlayerInfoPane->pFaceIconPane    = pLayoutWork->GetPicturePane( PANENAME_BVP_MIN_UPP_001_PANE_PLAYERPHOTO_00 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconPane != NULL );

          pPlayerInfoPane->pFaceIconFramePane = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_WINDOW_PLAYER_FRAME_00 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconFramePane );

          pPlayerInfoPane->pNameTextBoxPane = pPlayerInfoPane->pNameTextBoxPaneByRoyalRule  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_06 );
          GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPane != NULL );

          pPlayerInfoPane->pFaceIconRootPane  = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_NULL_00 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconRootPane );
        }

        //  左右
        {
          PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_LR ];

          pPlayerInfoPane->pFaceIconPane      = pLayoutWork->GetPicturePane( PANENAME_BVP_MIN_UPP_001_PANE_PLAYERPHOTO_01 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconPane != NULL );

          pPlayerInfoPane->pFaceIconFramePane = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_WINDOW_PLAYER_FRAME_01 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconFramePane );

          pPlayerInfoPane->pNameTextBoxPane = pPlayerInfoPane->pNameTextBoxPaneByRoyalRule  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_07 );
          GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPane != NULL );

          pPlayerInfoPane->pFaceIconRootPane  = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_NULL_01 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconRootPane );
        }

        //  右左
        {
          PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_RL ];

          pPlayerInfoPane->pFaceIconPane    = pLayoutWork->GetPicturePane( PANENAME_BVP_MIN_UPP_001_PANE_PLAYERPHOTO_02 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconPane != NULL );

          pPlayerInfoPane->pFaceIconFramePane = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_WINDOW_PLAYER_FRAME_02 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconFramePane );

          pPlayerInfoPane->pNameTextBoxPane = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_14 );
          GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPane != NULL );

          pPlayerInfoPane->pNameTextBoxPaneByRoyalRule  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_10 );
          GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPaneByRoyalRule != NULL );

          pPlayerInfoPane->pFaceIconRootPane  = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_NULL_02 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconRootPane );
        }

        //  右右
        {
          PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_RR ];

          pPlayerInfoPane->pFaceIconPane    = pLayoutWork->GetPicturePane( PANENAME_BVP_MIN_UPP_001_PANE_PLAYERPHOTO_03 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconPane != NULL );

          pPlayerInfoPane->pFaceIconFramePane = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_WINDOW_PLAYER_FRAME_03 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconFramePane );

          pPlayerInfoPane->pNameTextBoxPane = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_15 );
          GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPane != NULL );

          pPlayerInfoPane->pNameTextBoxPaneByRoyalRule  = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_11 );
          GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPaneByRoyalRule != NULL );

          pPlayerInfoPane->pFaceIconRootPane  = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_001_PANE_NULL_03 );
          GFL_ASSERT( pPlayerInfoPane->pFaceIconRootPane );
        }
        
        //  battlefes
        {
          m_pTextBoxPane_BattleFesPartnerName_0 = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_12 );
          m_pTextBoxPane_BattleFesPartnerName_1 = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_001_PANE_TEXTBOX_13 );
        }
      }

      //  アイコンテクスチャーデータを作成
      {
        //  左側
        {
          u32 loopNum = GFL_NELEMS( m_pLeftPokeIconTexDataArray );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_pLeftPokeIconTexDataArray[ i ]  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) App::Tool::PokeIconTexData( m_initParam.pHeap->GetDeviceHeap() );
          }
        }

        //  右側
        {
          u32 loopNum = GFL_NELEMS( m_pRightPokeIconTexDataArray );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_pRightPokeIconTexDataArray[ i ]  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) App::Tool::PokeIconTexData( m_initParam.pHeap->GetDeviceHeap() );
          }
        }
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bRet  = true;
      return  bRet;
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  bool UppGameInfoView::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bRet  = true;
      if( UIView::IsDrawing() == true )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }
    
    return  false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void UppGameInfoView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void UppGameInfoView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( m_bDrawVisible == false )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

      u32 loopNum = GFL_NELEMS( m_aPlayerInfoPane );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pG2DUtil->SetPaneVisible( m_aPlayerInfoPane[ i ].pFaceIconRootPane, m_bDrawVisible );
      }

      return;
    }

    if( ( displayNo == gfl2::gfx::CtrDisplayNo::LEFT )
     || ( displayNo == gfl2::gfx::CtrDisplayNo::RIGHT ) )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderManager, displayNo, i );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void UppGameInfoView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   可視状態の設定
  *          不可視状態時は描画、操作通知ともに無効となる。
  *
  * @param   isVisible  可視状態に設定するならtrueを指定
  */
  //--------------------------------------------------------------------------------------------
  void UppGameInfoView::SetDrawVisible( bool isVisible )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    m_bDrawVisible  = isVisible;
    if( isVisible == false )
    {
      u32 loopNum = GFL_NELEMS( m_aPlayerInfoPane );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pG2DUtil->SetPaneVisible( m_aPlayerInfoPane[ i ].pFaceIconRootPane, isVisible );
      }
    }
    else
    if( m_screenAnimeId != gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR )
    {
      //  いったん画面切り替えアニメは停止
      _StopAnimeScreen();

      pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_screenAnimeId );
      //  [fix]GFNMCat[1678]:コードのon/off表示切替を追加
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_screenCodeDrawAnimeId );

      UIView::Update();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ビデオ情報を設定
    * @param[in] pVideoData 画面更新するビデオデータ
    */
  //------------------------------------------------------------------
  void UppGameInfoView::SetVideoData( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData )
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();

    Regulation::BATTLE_CATEGORY category  = (Regulation::BATTLE_CATEGORY)pVideoData->sdCardData.upload.header.category;
    Regulation::BATTLE_RULE     rule      = (Regulation::BATTLE_RULE)pVideoData->sdCardData.upload.header.rule;

    //  左のポケモン表示
    {
      //  いったんすべてのアイコンを非表示
      {
        u32 loopNum = GFL_NELEMS( m_aLeftPokeIcon );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pG2DUtil->SetPaneVisible( m_aLeftPokeIcon[ i ].pRootPane, false );
        }
      }
    }

    //  右のポケモン表示
    {
      //  いったんすべてのアイコンを非表示
      {
        u32 loopNum = GFL_NELEMS( m_aLeftPokeIcon );
        for( u32 i = 0; i < loopNum; ++i )
        {
          pG2DUtil->SetPaneVisible( m_aRightPokeIcon[ i ].pRootPane, false );
        }
      }
    }

    //  プレイヤーのアイコンは非表示にしておく
    {
      u32 loopNum = GFL_NELEMS( m_aPlayerInfoPane );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pG2DUtil->SetPaneVisible( m_aPlayerInfoPane[ i ].pFaceIconRootPane, false );
      }
    }

    //  いったん画面切り替えアニメは停止
    _StopAnimeScreen();

    //  データエラーであれば設定は不要
    if( pVideoData->bErrorVersionFlg == 1 )
    {
      return;
    }

    //  画面構成の設定
    {
      if( 
        ( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_NORMAL )     //! バトルロイヤル施設ノーマル
        || ( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_SUPER  )     //! バトルロイヤル施設スーパー
        || ( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_HYPER  )     //! バトルロイヤル施設ハイパー
        || ( category == Regulation::BATTLE_CATEGORY_ROYAL_FACILITY_MASTER ) )   //! バトルロイヤル施設マスター
      {
        m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_ROYAL_LAYOUT;
      }
      else
      if(
            ( category == Regulation::BATTLE_CATEGORY_TREE_NORMAL )          //! バトルツリーノーマル
        ||  ( category == Regulation::BATTLE_CATEGORY_TREE_SUPER ) )         //! バトルツリースーパー
      {
        if(
          ( rule == Regulation::BATTLE_RULE_SINGLE )       //! シングル
          || ( rule == Regulation::BATTLE_RULE_DOUBLE ) )  //! ダブル
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_TREE_SINGLE_DOUBLE_LAYOUT;
        }
        else
        if( rule == Regulation::BATTLE_RULE_MULTI )
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_TREE_MULTI_LAYOUT;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
      else
      if( category == Regulation::BATTLE_CATEGORY_BATTLE_FES )  /*  バトルフェス  */
      {
        m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLEFES_LAYOUT_00;
      }
      else
      {
        if(
             ( rule == Regulation::BATTLE_RULE_SINGLE )       //! シングル
          || ( rule == Regulation::BATTLE_RULE_DOUBLE ) )     //! ダブル
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_SINGLE_DOUBLE_LAYOUT;
        }
        else
        if( rule == Regulation::BATTLE_RULE_ROYAL )
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_ROYAL_LAYOUT_00;
        }
        else
        if( rule == Regulation::BATTLE_RULE_MULTI )
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_MULTI_BATTLE_ROYAL_LAYOUT;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }

      pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_screenAnimeId );
    }

    //  アップロード画面更新
    UpdateVideoDataUpload( pVideoData );

    UIView::Update();

    //  左のポケモン表示
    PokeTool::SimpleParam simplePokeParam;
    {
      {
        u32 loopNum = pVideoData->GetPokeMemberCount();
        
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( pVideoData->OutputLeftPosPokeParam( &simplePokeParam, i ) == false )
          {
          }
          else
          {
            pG2DUtil->SetPaneVisible( m_aLeftPokeIcon[ i ].pRootPane, true );
            m_initParam.pPokeIconTexPool->CreatePokeData( m_pLeftPokeIconTexDataArray[ i ], m_initParam.pHeap->GetDeviceHeap()->GetLowerHandle(), simplePokeParam, false );
            m_pLeftPokeIconTexDataArray[ i ]->ReplectPaneTexture( m_aLeftPokeIcon[ i ].pPicPane, 0, 0 );
          }
        }
      }
    }

    //  右のポケモン表示
    {
      {
        u32 loopNum = pVideoData->GetPokeMemberCount();
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( pVideoData->OutputRightPosPokeParam( &simplePokeParam, i ) == false )
          {
          }
          else
          {
            pG2DUtil->SetPaneVisible( m_aRightPokeIcon[ i ].pRootPane, true );
            m_initParam.pPokeIconTexPool->CreatePokeData( m_pRightPokeIconTexDataArray[ i ], m_initParam.pHeap->GetDeviceHeap()->GetLowerHandle(), simplePokeParam, false );
            m_pRightPokeIconTexDataArray[ i ]->ReplectPaneTexture( m_aRightPokeIcon[ i ].pPicPane, 0, 0 );
          }
        }
      }
    }

    //  プレイヤー設定
    {
      //  左左
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_LL ];

        if( 0 < pVideoData->aPlayerData[ BTL_CLIENT_PLAYER ].pName->GetLength() )
        {
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPane, pVideoData->aPlayerData[ BTL_CLIENT_PLAYER ].pName );
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPaneByRoyalRule, pVideoData->aPlayerData[ BTL_CLIENT_PLAYER ].pName );

          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
          {
            pVideoData->OutputIconObjectParam( &iconData, pVideoData->aPlayerData[ BTL_CLIENT_PLAYER ] );

            iconData.pReplacePane             = pPlayerInfoPane->pFaceIconPane;
            iconData.pInLoadVisibleChangePane = pPlayerInfoPane->pFaceIconFramePane;
            iconData.pSyncVisiblePane         = pPlayerInfoPane->pFaceIconRootPane;
            iconData.pTimerIconRootPane       = pPlayerInfoPane->pFaceIconRootPane;
          }
          m_initParam.pTrainerIconRendering->SetIconObjectData( m_initParam.aReservTrainerIconId[ PLAYER_INFO_POS_LL ], iconData, pVideoData->IsPlayerTrainerIcon( pVideoData->aPlayerData[ BTL_CLIENT_PLAYER ] ) );
        }
      }

      //  左右
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_LR ];

        if( 0 < pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ].pName->GetLength() )
        {
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPane, pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ].pName );
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPaneByRoyalRule, pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ].pName );

          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
          {
            pVideoData->OutputIconObjectParam( &iconData, pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ] );

            iconData.pReplacePane       = pPlayerInfoPane->pFaceIconPane;
            iconData.pInLoadVisibleChangePane = pPlayerInfoPane->pFaceIconFramePane;
            iconData.pSyncVisiblePane   = pPlayerInfoPane->pFaceIconRootPane;
            iconData.pTimerIconRootPane = pPlayerInfoPane->pFaceIconRootPane;
          }
          m_initParam.pTrainerIconRendering->SetIconObjectData( m_initParam.aReservTrainerIconId[ PLAYER_INFO_POS_LR ], iconData, pVideoData->IsPlayerTrainerIcon( pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ] ) );
        }
      }

      //  右左
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_RL ];

        if( 0 < pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2 ].pName->GetLength() )
        {
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPane, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2 ].pName );
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPaneByRoyalRule, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2 ].pName );

          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
          {
            pVideoData->OutputIconObjectParam( &iconData, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2 ] );

            iconData.pReplacePane     = pPlayerInfoPane->pFaceIconPane;
            iconData.pInLoadVisibleChangePane = pPlayerInfoPane->pFaceIconFramePane;
            iconData.pSyncVisiblePane   = pPlayerInfoPane->pFaceIconRootPane;
            iconData.pTimerIconRootPane = pPlayerInfoPane->pFaceIconRootPane;
          }

          m_initParam.pTrainerIconRendering->SetIconObjectData( m_initParam.aReservTrainerIconId[ PLAYER_INFO_POS_RL ], iconData, pVideoData->IsPlayerTrainerIcon( pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2 ] ) );
        }
      }

      //  右右
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_RR ];

        if( 0 < pVideoData->aPlayerData[ BTL_CLIENT_ENEMY1 ].pName->GetLength() )
        {
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPane, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY1 ].pName );
          pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPaneByRoyalRule, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY1 ].pName );

          app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
          {
            pVideoData->OutputIconObjectParam( &iconData, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY1 ] );

            iconData.pReplacePane       = pPlayerInfoPane->pFaceIconPane;
            iconData.pInLoadVisibleChangePane = pPlayerInfoPane->pFaceIconFramePane;
            iconData.pSyncVisiblePane   = pPlayerInfoPane->pFaceIconRootPane;
            iconData.pTimerIconRootPane = pPlayerInfoPane->pFaceIconRootPane;
          }
          m_initParam.pTrainerIconRendering->SetIconObjectData( m_initParam.aReservTrainerIconId[ PLAYER_INFO_POS_RR ], iconData, pVideoData->IsPlayerTrainerIcon( pVideoData->aPlayerData[ BTL_CLIENT_ENEMY1 ] ) );
        }
      }
      
      //  battlefes
      {
        if(
          (     0 < pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ].pName->GetLength() )
          &&  ( 0 < pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2  ].pName->GetLength() )
        )
        {
          pG2DUtil->SetTextBoxPaneString( m_pTextBoxPane_BattleFesPartnerName_0, pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ].pName );
          pG2DUtil->SetTextBoxPaneString( m_pTextBoxPane_BattleFesPartnerName_1, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2  ].pName );
        }
      }
    }

    //  バトルハウスのゲームカウント数
    {
      // @fixTPC要望 タイトルと数字がそれぞれテキストボックスで分かれていたが、一つになった。
      // @fix NMCat[686] ３桁から４桁に変更
      const u32 messageID = (category == Regulation::BATTLE_CATEGORY_BATTLE_FES) ? msg_bvp_sys_up_31 : msg_bvp_sys_up_14;
      pG2DUtil->SetTextBoxPaneNumber( m_pBattleHouseGameCountNumTextBoxPane, messageID, pVideoData->sdCardData.upload.header.battle_counter, 4, 0 );
    }

    //  ルール表記
    {
      pG2DUtil->SetPaneVisible( m_pRuleBtlTitleTextBoxPane, true );

      //! シングル
      if( rule == Regulation::BATTLE_RULE_SINGLE )
      {
        pG2DUtil->SetTextBoxPaneString( m_pRuleBtlTitleTextBoxPane, msg_bvp_battle_rule_01 );
      }
      else
      //! ダブル
      if( rule == Regulation::BATTLE_RULE_DOUBLE )
      {
        pG2DUtil->SetTextBoxPaneString( m_pRuleBtlTitleTextBoxPane, msg_bvp_battle_rule_02 );
      }
      else
      //! ロイヤル
      if( rule == Regulation::BATTLE_RULE_ROYAL )
      {
        pG2DUtil->SetTextBoxPaneString( m_pRuleBtlTitleTextBoxPane, msg_bvp_battle_rule_03 );
      }
      else
      //! マルチ
      if( rule == Regulation::BATTLE_RULE_MULTI )
      {
        pG2DUtil->SetTextBoxPaneString( m_pRuleBtlTitleTextBoxPane, msg_bvp_battle_rule_04 );
      }
      else
      {
        pG2DUtil->SetPaneVisible( m_pRuleBtlTitleTextBoxPane, false );
      }
    }

    //  さかさバトルか
    if( pVideoData->sdCardData.upload.header.sakasaBattle == true )
    {
      pG2DUtil->SetPaneVisible( m_pSakasaBtlTitleTextBoxPane, true );
    }
    else
    {
      pG2DUtil->SetPaneVisible( m_pSakasaBtlTitleTextBoxPane, false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ビデオ情報のアップロード部分を更新
    */
  //------------------------------------------------------------------
  void UppGameInfoView::UpdateVideoDataUpload( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData )
  {
    if( pVideoData == NULL )
    {
      GFL_ASSERT( pVideoData );
      return;
    }

    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    if( pVideoData->sdCardData.upload_flag  == true )
    {
      //  アップロードしている
      m_screenCodeDrawAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_VIDEO_CODE_ON;

      //  コードid設定
      {
        u64 tmpNum  = pVideoData->sdCardData.dataID;

        gfl2::str::StrBuf outStr( 64 , m_initParam.pHeap->GetDeviceHeap() );
        NetApp::Util::ConvertUtility::U64ToStr( tmpNum, &outStr , false );

        //  コードは４つ区切りに
        const u32 cCodeNumberCount  = 4;
        gfl2::str::StrBuf tempStr( 64 , m_initParam.pHeap->GetDeviceHeap() );
        for( u32 i = 0; i < cCodeNumberCount; ++i )
        {
          gfl2::str::STRCODE buf[5];
          std::memset( buf , 0 , sizeof(buf) );
          std::memcpy( buf , &( outStr.GetPtr()[i*4] ) , sizeof(buf) - 2 );
          tempStr.SetStr( buf );
          m_initParam.pWordSet->RegisterWord( i , tempStr );
        }

        //  テキスト設定
        {
          gfl2::str::StrBuf*  pDstStrBuf  = pG2DUtil->GetTempStrBuf( 0 );
          gfl2::str::StrBuf*  pSrcStrBuf  = pG2DUtil->GetTempStrBuf( 1 );
          m_initParam.pAppMsgData->GetString( msg_bvp_sys_up_11, *pSrcStrBuf );

          m_initParam.pWordSet->Expand( pDstStrBuf, pSrcStrBuf );

          pG2DUtil->SetTextBoxPaneString( m_pCodeNumTextBoxPane, pDstStrBuf );
        }
      }

      //  再生数設定
      {
        pG2DUtil->SetTextBoxPaneNumber( m_pPlayeNumTextBoxPane, msg_bvp_sys_up_09, pVideoData->sdCardData.playCount, 7, 0, print::NUM_DISP_SPACE );
      }

      //  登録日時設定
      {
        m_initParam.pWordSet->RegisterNumber( 0, pVideoData->registDate.GetYear(),   4, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        m_initParam.pWordSet->RegisterNumber( 1, pVideoData->registDate.GetMonth(),  2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
        m_initParam.pWordSet->RegisterNumber( 2, pVideoData->registDate.GetDay(),    2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );

        gfl2::str::StrBuf*  pDstStrBuf  = pG2DUtil->GetTempStrBuf( 0 );
        gfl2::str::StrBuf*  pSrcStrBuf  = pG2DUtil->GetTempStrBuf( 1 );
        m_initParam.pAppMsgData->GetString( msg_bvp_sys_up_10, *pSrcStrBuf );

        m_initParam.pWordSet->Expand( pDstStrBuf, pSrcStrBuf );

        pG2DUtil->SetTextBoxPaneString( m_pDateNumTextBoxPane, pDstStrBuf );
      }
    }
    else
    {
      //  アップロードしていない
      m_screenCodeDrawAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_VIDEO_CODE_OFF;
    }

    pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_screenCodeDrawAnimeId );
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void UppGameInfoView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 16, app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BFLYT,
        LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

  //------------------------------------------------------------------
  /**
    * @brief   画面変更アニメ停止
    */
  //------------------------------------------------------------------
  void UppGameInfoView::_StopAnimeScreen()
  {
    app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();

    const u32 aScreenChangeAnimeId[]  =
    {
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_VIDEO_CODE_OFF,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_VIDEO_CODE_ON,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_ROYAL_LAYOUT,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_TREE_SINGLE_DOUBLE_LAYOUT,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_TREE_MULTI_LAYOUT,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_SINGLE_DOUBLE_LAYOUT,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_BATTLE_ROYAL_LAYOUT_00,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_001_MULTI_BATTLE_ROYAL_LAYOUT,
    };

    u32 loopNum = GFL_NELEMS( aScreenChangeAnimeId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, aScreenChangeAnimeId[ i ] );
    }
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
