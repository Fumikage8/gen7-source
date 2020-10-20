//======================================================================
/**
 * @file BattleVideoPlayerUppGameResultView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの上画面ゲーム結果View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerUppGameResultView.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/BattleVideoPlayer/BattleVideoPlayerVideoDataManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "AppLib/include/Tool/AppToolBaseGrpFont.h"
#include "AppLib/include/Tool/PokeIconTexPool.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_pane.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  UppGameResultView::UppGameResultView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  UppGameResultView::~UppGameResultView()
  {
    {
      //  ポケモンアイコンテクスチャーを作成
      u32 loopNum = GFL_NELEMS( m_pPokeIconTexDataArray );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pPokeIconTexDataArray[ i ] );
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
  bool UppGameResultView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
      gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  下側のメッセージペイン取得
      {
        app::util::GetPaneHelper  partsPaneHelper( pLayoutWork, &rMultiResId );
        partsPaneHelper.Push( PANENAME_BVP_MIN_UPP_002_PANE_PARTS_03 );

        m_pMessageTextBoxPane = partsPaneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00 );
        GFL_ASSERT( m_pMessageTextBoxPane );
      }

      //  固定テキスト表示
      {
        //  ターン
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          gfl2::lyt::LytTextBox*  pTextBoxPane  = paneHelper.GetTextBoxPane( PANENAME_BVP_MIN_UPP_002_PANE_TEXTBOX_00 );
          GFL_ASSERT( pTextBoxPane != NULL );

          pG2DUtil->SetTextBoxPaneString( pTextBoxPane, msg_bvp_sys_up_16 );
        }
      }

      //  テキストペイン取得
      {
        app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
        m_pTurnNumTextBoxPane = paneHelper.GetTextBoxPane( PANENAME_BVP_MIN_UPP_002_PANE_TEXTBOX_01 );
        GFL_ASSERT( m_pTurnNumTextBoxPane != NULL );
      }

      {
        app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
        m_pResultGrpFontPicPane = paneHelper.GetPicturePane( PANENAME_BVP_MIN_UPP_002_PANE_GRAFICFONT_01 );
        GFL_ASSERT( m_pResultGrpFontPicPane != NULL );
      }

      //  ポケモンアイコンリスト
      {
        const u32 aPartsPaneId[][ 2 ]  =
        {
          { PANENAME_BVP_MIN_UPP_002_PANE_POKEICON_00, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002__POKEICON_00_DIE },
          { PANENAME_BVP_MIN_UPP_002_PANE_POKEICON_01, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002__POKEICON_01_DIE },
          { PANENAME_BVP_MIN_UPP_002_PANE_POKEICON_02, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002__POKEICON_02_DIE },
          { PANENAME_BVP_MIN_UPP_002_PANE_POKEICON_03, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002__POKEICON_03_DIE },
          { PANENAME_BVP_MIN_UPP_002_PANE_POKEICON_04, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002__POKEICON_04_DIE },
          { PANENAME_BVP_MIN_UPP_002_PANE_POKEICON_05, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002__POKEICON_05_DIE }
        };

        u32 loopNum = GFL_NELEMS( m_aPokeIcon );
        for( u32 i = 0; i < loopNum; ++i )
        {
          app::util::GetPaneHelper  paneHelper( pLayoutWork, &rMultiResId );
          paneHelper.Push( aPartsPaneId[ i ][ 0 ] );

          m_aPokeIcon[ i ].pRootPane  = paneHelper.Peek();
          GFL_ASSERT( m_aPokeIcon[ i ].pRootPane );

          m_aPokeIcon[ i ].pPicPane   = paneHelper.GetPicturePane( PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON );
          GFL_ASSERT( m_aPokeIcon[ i ].pPicPane );

          m_aPokeIcon[ i ].dieAnimeId = aPartsPaneId[ i ][ 1 ];
          m_aPokeIcon[ i ].iconId = i;
        }
      }

      //  左
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_L ];

        pPlayerInfoPane->pFaceIconRootPane  = pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_UPP_002_PANE_PLAYERPHOTO_00 );
        GFL_ASSERT( pPlayerInfoPane->pFaceIconRootPane );

        pPlayerInfoPane->pFaceIconPane    = pLayoutWork->GetPicturePane( pPlayerInfoPane->pFaceIconRootPane, PANENAME_COMMON_PLT_001_PANE_PLAYERPHOTO, &rMultiResId );
        GFL_ASSERT( pPlayerInfoPane->pFaceIconPane != NULL );

        pPlayerInfoPane->pFaceIconFramePane    = pLayoutWork->GetPicturePane( pPlayerInfoPane->pFaceIconRootPane, PANENAME_COMMON_PLT_001_PANE_WINDOW_PLAYER_FRAME, &rMultiResId );
        GFL_ASSERT( pPlayerInfoPane->pFaceIconFramePane != NULL );
        
        pPlayerInfoPane->pNameTextBoxPane = pLayoutWork->GetTextBoxPane( pPlayerInfoPane->pFaceIconRootPane, PANENAME_COMMON_PLT_001_PANE_TEXTBOX_00, &rMultiResId );
        GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPane != NULL );
      }

      //  右
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_R ];

        pPlayerInfoPane->pFaceIconRootPane  = pLayoutWork->GetPartsPane( PANENAME_BVP_MIN_UPP_002_PANE_PLAYERPHOTO_01 );
        GFL_ASSERT( pPlayerInfoPane->pFaceIconRootPane );

        pPlayerInfoPane->pFaceIconPane    = pLayoutWork->GetPicturePane( pPlayerInfoPane->pFaceIconRootPane, PANENAME_COMMON_PLT_001_PANE_PLAYERPHOTO, &rMultiResId );
        GFL_ASSERT( pPlayerInfoPane->pFaceIconPane != NULL );

        pPlayerInfoPane->pFaceIconFramePane    = pLayoutWork->GetPicturePane( pPlayerInfoPane->pFaceIconRootPane, PANENAME_COMMON_PLT_001_PANE_WINDOW_PLAYER_FRAME, &rMultiResId );
        GFL_ASSERT( pPlayerInfoPane->pFaceIconFramePane != NULL );

        pPlayerInfoPane->pNameTextBoxPane = pLayoutWork->GetTextBoxPane( pPlayerInfoPane->pFaceIconRootPane, PANENAME_COMMON_PLT_001_PANE_TEXTBOX_00, &rMultiResId );
        GFL_ASSERT( pPlayerInfoPane->pNameTextBoxPane != NULL );
      }

      //  battlefes
      {
        m_pTextBoxPane_BattleFesPartnerName_0 = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_002_PANE_TEXTBOX_12 );
        m_pTextBoxPane_BattleFesPartnerName_1 = pLayoutWork->GetTextBoxPane( PANENAME_BVP_MIN_UPP_002_PANE_TEXTBOX_13 );
      }


      //  ポケモンアイコンテクスチャーを作成
      {
        u32 loopNum = GFL_NELEMS( m_pPokeIconTexDataArray );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pPokeIconTexDataArray[ i ] = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) App::Tool::PokeIconTexData( m_initParam.pHeap->GetDeviceHeap() );
        }
      }

      ++m_initSeq;
    }

    if( m_initSeq == 1 )
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
  bool UppGameResultView::EndFunc()
  {
    bool  bRet  = true;

    if( UIView::IsDrawing() == true )
    {
      bRet  = false;
    }

    return  bRet;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void UppGameResultView::Update()
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
  void UppGameResultView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
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
  void UppGameResultView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   ビデオ情報を設定
    * @param[in] pVideoData 画面更新するビデオデータ
    */
  //------------------------------------------------------------------
  void UppGameResultView::SetVideoData( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData )
  {
    if( pVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    //  いったんすべてのアイコンを非表示
    {
      u32 loopNum = GFL_NELEMS( m_aPokeIcon );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pG2DUtil->SetPaneVisible( m_aPokeIcon[ i ].pRootPane, false );
      }

      loopNum = GFL_NELEMS( m_aPlayerInfoPane );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pG2DUtil->SetPaneVisible( m_aPlayerInfoPane[ i ].pFaceIconRootPane, false );
        pG2DUtil->SetTextBoxPaneString( m_aPlayerInfoPane[ i ].pNameTextBoxPane, L"" );
      }
    }

    //  データエラーであれば設定は不要
    if( pVideoData->bErrorVersionFlg == 1 )
    {
      return;
    }

    //  勝敗によるプレイヤー参照設定
    NetAppLib::BattleVideoPlayer::PLAYER_DATA*  pDrawPlayerDataLeft   = NULL;
    NetAppLib::BattleVideoPlayer::PLAYER_DATA*  pDrawPlayerDataRight  = NULL;

    //  勝ちと負けの場合は自分の情報を出す
    if( 
      ( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_WIN )
      || ( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_LOSE ) )
    {
      pDrawPlayerDataLeft = &pVideoData->aPlayerData[ BTL_CLIENT_PLAYER ];
      pDrawPlayerDataRight = &pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ];
    }
    else
      if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_DRAW )
      {
        pDrawPlayerDataLeft  = NULL;
        pDrawPlayerDataRight  = NULL;
      }
      else
      {
        GFL_ASSERT( 0 );
      }

    Regulation::BATTLE_RULE     rule      = (Regulation::BATTLE_RULE)pVideoData->sdCardData.upload.header.rule;

    _StopAnimeScreen();

    //  ひきわけだと専用画面へ
    if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_DRAW )
    {
      m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_DRAW;
    }
    else
    {
      /*  バトルフェスは専用View  */
      if(((Regulation::BATTLE_CATEGORY)pVideoData->sdCardData.upload.header.category) == Regulation::BATTLE_CATEGORY_BATTLE_FES)
      {
        m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_BATTLEFES;
      }
      else
      {
        //  ルールによる画面切り替え
        if(
          ( rule == Regulation::BATTLE_RULE_SINGLE )       //! シングル
          || ( rule == Regulation::BATTLE_RULE_DOUBLE ) )       //! ダブル
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_00;

          //  一人しか出さない
          pDrawPlayerDataRight = NULL;
        }
        else
        if( rule == Regulation::BATTLE_RULE_ROYAL )
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_00;

          //  一人しか出さない
          pDrawPlayerDataRight = NULL;
        }
        else
        if( rule == Regulation::BATTLE_RULE_MULTI )
        {
          m_screenAnimeId = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_01;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }

    pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_screenAnimeId );

    UIView::Update();

    //  ターン数表示
    {
      // @fix GFNMCat[830] ターン数の値は０基準になっているので＋１にする
      // @fix GFNMCat[828] でバトル側が1基準になったので、↑を取り消し
      pG2DUtil->SetTextBoxPaneNumber( m_pTurnNumTextBoxPane, msg_bvp_sys_up_12, pVideoData->sdCardData.upload.header.end_turn_num, 3 );
    }

    //  勝敗表示
    {
      u32 uvId  = app::tool::BtlVideoPlayerGrpFontUVDataTale::TYPE_VS_DRAW_TEXT_LONG;
      if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_WIN )
      {
        uvId  = app::tool::BtlVideoPlayerGrpFontUVDataTale::TYPE_VS_WIN_TEXT_LONG;
      }
      else
      if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_LOSE )
      {
        uvId  = app::tool::BtlVideoPlayerGrpFontUVDataTale::TYPE_VS_LOSE_TEXT_LONG;
      }
      else
      if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_DRAW )
      {
        uvId  = app::tool::BtlVideoPlayerGrpFontUVDataTale::TYPE_VS_DRAW_TEXT_LONG;
      }
      else
      {
        GFL_ASSERT( 0 );
      }

      m_initParam.pGrpFont->ReplacePaneTexture( uvId, m_pResultGrpFontPicPane, 0, 0 );
    }

    //  メッセージ表示
    {
      u32 msgId = msg_bvp_sys_up_20;
      if( rule == Regulation::BATTLE_RULE_ROYAL )
      {
        // @fix GFNMCat[1313] バトルロイヤル順位値で結果テキストを切り替え
        const u32 caRoyalRankMsgId[] =
        {
          msg_bvp_sys_up_27,
          msg_bvp_sys_up_28,
          msg_bvp_sys_up_29,
          msg_bvp_sys_up_30,
        };

        if( pVideoData->sdCardData.upload.header.royal_result < GFL_NELEMS( caRoyalRankMsgId ) )
        {
          msgId = caRoyalRankMsgId[ pVideoData->sdCardData.upload.header.royal_result ];
        }
        else
        {
          //  順位がこちらの想定外
          GFL_ASSERT( 0 );
        }
      }
      else
      {
        if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_DRAW )
        {
          //  ひきわけ
          msgId = msg_bvp_sys_up_20;
        }
        else
        //  勝利
        if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_WIN )
        {
          if( pVideoData->sdCardData.upload.header.battle_result2 == BTL_RECORD_RESULT_2_DEAD )
          {
            //  ポケモン全滅
            msgId = msg_bvp_sys_up_17;
          }
          else
          if( pVideoData->sdCardData.upload.header.battle_result2 == BTL_RECORD_RESULT_2_TIMEOVER )
          {
            //  タイムオーバー
            msgId = msg_bvp_sys_up_18;
          }
          else
          if( pVideoData->sdCardData.upload.header.battle_result2 == BTL_RECORD_RESULT_2_SURRENDER )
          {
            //  こうさん
            msgId = msg_bvp_sys_up_19;
          }
          else
          {
            GFL_ASSERT( 0 );
          }
        }
        else
        //  負け
        if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_LOSE )
        {
          if( pVideoData->sdCardData.upload.header.battle_result2 == BTL_RECORD_RESULT_2_DEAD )
          {
            //  ポケモン全滅
            msgId = msg_bvp_sys_up_24;
          }
          else
          if( pVideoData->sdCardData.upload.header.battle_result2 == BTL_RECORD_RESULT_2_TIMEOVER )
          {
            //  タイムオーバー
            msgId = msg_bvp_sys_up_25;
          }
          else
          if( pVideoData->sdCardData.upload.header.battle_result2 == BTL_RECORD_RESULT_2_SURRENDER )
          {
            //  こうさん
            msgId = msg_bvp_sys_up_26;
          }
          else
          {
            GFL_ASSERT( 0 );
          }
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }

      pG2DUtil->SetTextBoxPaneString( m_pMessageTextBoxPane, msgId );
    }

    //  ポケモン表示
    {
      //  勝ちと負けでは自分の情報を出す
      {
        PokeTool::SimpleParam simplePokeParam;
        if( 
            ( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_WIN )
        ||  ( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_LOSE ) )
        {
          u32 loopNum = pVideoData->GetPokeMemberCount();
          if( pVideoData->sdCardData.upload.header.rule == BTL_RULE_ROYAL )
          {
            //  ロイヤルの場合は一人のみ表示なので所持ポケモン最大3体までしか表示しない
            loopNum /= 2;
          }

          for( u32 i = 0; i < loopNum; ++i )
          {
            if( pVideoData->OutputLeftPosPokeParam( &simplePokeParam, i ) == false )
            {
            }
            else
            {
              pG2DUtil->SetPaneVisible( m_aPokeIcon[ i ].pRootPane, true );
              m_initParam.pPokeIconTexPool->CreatePokeData( m_pPokeIconTexDataArray[ i ], m_initParam.pHeap->GetDeviceHeap()->GetLowerHandle(), simplePokeParam, false );
              m_pPokeIconTexDataArray[ i ]->ReplectPaneTexture( m_aPokeIcon[ i ].pPicPane, 0, 0 );
            }
          }
        }
        else
        if( pVideoData->sdCardData.upload.header.battle_result1 == BTL_RECORD_RESULT_1_DRAW )
        {
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }

    //  プレイヤーの設定
    {
      //  左
      if( pDrawPlayerDataLeft != NULL )
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_L ];

        pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPane, pDrawPlayerDataLeft->pName );

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
        {
          pVideoData->OutputIconObjectParam( &iconData, *pDrawPlayerDataLeft );

          iconData.pReplacePane     = pPlayerInfoPane->pFaceIconPane;
          iconData.pSyncVisiblePane = pPlayerInfoPane->pFaceIconRootPane;
          iconData.pTimerIconRootPane       = pPlayerInfoPane->pFaceIconRootPane;
          iconData.pInLoadVisibleChangePane = pPlayerInfoPane->pFaceIconFramePane;
          
        }
        m_initParam.pTrainerIconRendering->SetIconObjectData( m_initParam.aReservTrainerIconId[ PLAYER_INFO_POS_L ], iconData, pVideoData->IsPlayerTrainerIcon( *pDrawPlayerDataLeft ) );
      }

      //  右
      if( pDrawPlayerDataRight != NULL )
      {
        PLAYER_INFO_PANE* pPlayerInfoPane = &m_aPlayerInfoPane[ PLAYER_INFO_POS_R ];

        pG2DUtil->SetTextBoxPaneString( pPlayerInfoPane->pNameTextBoxPane, pDrawPlayerDataRight->pName );

        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
        {
          pVideoData->OutputIconObjectParam( &iconData, *pDrawPlayerDataRight );

          iconData.pReplacePane     = pPlayerInfoPane->pFaceIconPane;
          iconData.pSyncVisiblePane = pPlayerInfoPane->pFaceIconRootPane;
          iconData.pTimerIconRootPane       = pPlayerInfoPane->pFaceIconRootPane;
          iconData.pInLoadVisibleChangePane = pPlayerInfoPane->pFaceIconFramePane;
        }
        m_initParam.pTrainerIconRendering->SetIconObjectData( m_initParam.aReservTrainerIconId[ PLAYER_INFO_POS_R ], iconData, pVideoData->IsPlayerTrainerIcon( *pDrawPlayerDataRight ) );
      }

      //  battlefes
      if(((Regulation::BATTLE_CATEGORY)pVideoData->sdCardData.upload.header.category) == Regulation::BATTLE_CATEGORY_BATTLE_FES)
      {
        pG2DUtil->SetTextBoxPaneString( m_pTextBoxPane_BattleFesPartnerName_0, pVideoData->aPlayerData[ BTL_CLIENT_PARTNER ].pName);
        pG2DUtil->SetTextBoxPaneString( m_pTextBoxPane_BattleFesPartnerName_1, pVideoData->aPlayerData[ BTL_CLIENT_ENEMY2  ].pName);
      }
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
  void UppGameResultView::SetDrawVisible( bool isVisible )
  {
    m_bDrawVisible  = isVisible;
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( isVisible == false )
    {
      u32 loopNum = GFL_NELEMS( m_aPlayerInfoPane );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pG2DUtil->SetPaneVisible( m_aPlayerInfoPane[ i ].pFaceIconRootPane, isVisible );
      }
    }
    else
    {
      _StopAnimeScreen();

      pG2DUtil->StartAnime( LYT_WORK_NORMAL, m_screenAnimeId );

      UIView::Update();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void UppGameResultView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
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
        LYTRES_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_BFLYT,
        LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002___NUM,
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
  void UppGameResultView::_StopAnimeScreen()
  {
    const u32 aScreenChangeAnimeId[]  =
    {
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_DRAW,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_00,
      LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_002_LAYOUT_01,
    };

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    u32 loopNum = GFL_NELEMS( aScreenChangeAnimeId );
    for( u32 i = 0; i < loopNum; ++i )
    {
      pG2DUtil->StopAnime( LYT_WORK_NORMAL, aScreenChangeAnimeId[ i ] );
    }
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
