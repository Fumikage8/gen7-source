// ============================================================================
/*
 * @file BattleRoyalResultUpperRankingView.cpp
 * @brief バトルロイヤル勝敗アプリの順位上画面を表示するクラスです。
 * @date 2015.10.26
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/View/BattleRoyalresultUpperRankingView.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultResourceID.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"
#include "AppLib/include/Tool/AppToolGaugePartsLayout.h"

#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/battle_royal/res2d/BattleRoyalResultLyt.h>
#include <niji_conv_header/app/battle_royal/res2d/BattleRoyalResultLyt_anim_list.h>
#include <niji_conv_header/app/battle_royal/res2d/BattleRoyalResultLyt_pane.h>

#include <niji_conv_header/message/msg_battleroyal_result.h>

//  サウンドのインクルード
#include <Sound/include/Sound.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(View)

  BattleRoyalResultUpperRankingView::BattleRoyalResultUpperRankingView( App::BattleRoyalResult::System::BattleRoyalResultWork* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::BattleRoyalResult::BATTLE_ROYAL_RESULT_RESOURCE_ID_LAYOUT,
      LYTRES_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000_BFLYT,
      LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000___NUM,
      app::util::G2DUtil::SETUP_UPPER,
      pWork->GetMessageUtility(),
      GARC_message_battleroyal_result_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    app::util::Heap*    pAppHeap  = m_pWork->GetAppHeap();
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
    GFL_ASSERT( pLayoutWork );

    m_pTrainerFramePriorityPane = pLayoutWork->GetPane( PANENAME_BRL_MIN_UPP_000_PANE_WINDOW_PRIORITY );
    GFL_ASSERT( m_pTrainerFramePriorityPane );

    //  トレーナーフレーム構築
    {
      const TrainerFrame::INIT_PARAM  aInitParam[ App::BattleRoyalResult::TRAINER_FRAME_POS_MAX ]  =
      {
        //  window_01
        {
          LAYOUT_WORK_ID_UPPER,

          pLayoutWork->GetPartsPane( PANENAME_BRL_MIN_UPP_000_PANE_WINDOW_01 ),
          {
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_01_IN,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_01_GLITTER,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_01_NO1_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_01_NO2_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_01_GLOBE_COLOR
          },
        },

        //  window_02
        {
          LAYOUT_WORK_ID_UPPER,

          pLayoutWork->GetPartsPane( PANENAME_BRL_MIN_UPP_000_PANE_WINDOW_02 ),
          {
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_02_IN,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_02_GLITTER,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_02_NO1_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_02_NO2_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_02_GLOBE_COLOR
          },
        },

        //  window_03
        {
          LAYOUT_WORK_ID_UPPER,

          pLayoutWork->GetPartsPane( PANENAME_BRL_MIN_UPP_000_PANE_WINDOW_03 ),
          {
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_03_IN,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_03_GLITTER,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_03_NO1_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_03_NO2_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_03_GLOBE_COLOR
          },
        },

        //  window_04
        {
          LAYOUT_WORK_ID_UPPER,

          pLayoutWork->GetPartsPane( PANENAME_BRL_MIN_UPP_000_PANE_WINDOW_04 ),
          {
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_04_IN,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_04_GLITTER,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_04_NO1_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_04_NO2_BG,
            LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000__WINDOW_04_GLOBE_COLOR
          },
        }
      };

      u32 loopNum = GFL_NELEMS( m_pTrainerFrame );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pTrainerFrame[ i ]  = GFL_NEW( pAppHeap->GetDeviceHeap() ) TrainerFrame( i, aInitParam[ i ], this, pLayoutWork, &rResourceID, pAppHeap );
      }
    }
  }

  BattleRoyalResultUpperRankingView::~BattleRoyalResultUpperRankingView()
  {
    u32 loopNum = GFL_NELEMS( m_pTrainerFrame );
    for( u32 i = 0; i < loopNum; ++i )
    {
      GFL_SAFE_DELETE( m_pTrainerFrame[ i ] );
    }
  }

  //  初期化
  bool BattleRoyalResultUpperRankingView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      bool  bNextSeq  = true;

      App::BattleRoyalResult::APP_PARAM*  pAppParam = m_pWork->GetAppParam();
      GFL_ASSERT( pAppParam );

      u32 loopNum = GFL_NELEMS( m_pTrainerFrame );
      GFL_ASSERT( GFL_NELEMS( pAppParam->aTrainerData ) == loopNum );

      m_pWork->GetTrainerIconUtil()->Update();
      for( u32 i = 0; i < loopNum; ++i )
      {
        //  トレーナーアイコンのロードする
        if( m_pTrainerFrame[ i ]->InitFunc( pAppParam->aTrainerData[ i ] ) == false )
        {
          bNextSeq  = false;
        }
      }

      if( bNextSeq == true )
      {
        ++m_initSeq;
      }
    }
    else
    {
      //  表示順序にリストを設定( 4位から1位の順序にする )
      //  順位が被っても問題ないようにしている
      {
        TrainerFrame*  pTmpTrainerFrame[ App::BattleRoyalResult::TRAINER_FRAME_POS_MAX ];
        gfl2::std::MemCopy( m_pTrainerFrame, pTmpTrainerFrame, sizeof( pTmpTrainerFrame ) );

        u8  chkRanking  = 0;
        s32 setListIdx  = -1;
        u32 loopNum = GFL_NELEMS( pTmpTrainerFrame );
        for( u32 i = 0; i < loopNum; ++i )
        {
          setListIdx  = -1;
          chkRanking  = 0;
          for( u32 chkIdx = 0; chkIdx < loopNum; ++chkIdx )
          {
            if( pTmpTrainerFrame[ chkIdx ] != NULL ) 
            {
              if( chkRanking < pTmpTrainerFrame[ chkIdx ]->GetRanking() )
              {
                chkRanking  = pTmpTrainerFrame[ chkIdx ]->GetRanking();
                setListIdx  = chkIdx;
              }
            }
          }

          if( 0 <= setListIdx )
          {
            m_pTrainerFrameEvtList[ i ] =  pTmpTrainerFrame[ setListIdx ];           
            pTmpTrainerFrame[ setListIdx ]  = NULL;
          }
        }
      }

      m_endSeq  = 0;
      return  true;
    }

    return  false;
  }

  //  終了処理
  bool BattleRoyalResultUpperRankingView::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bNextSeq  = true;
      u32 loopNum = GFL_NELEMS( m_pTrainerFrame );

      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_pTrainerFrame[ i ]->EndFunc() == false )
        {
          bNextSeq  = false;
        }
      }

      if( bNextSeq == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      m_initSeq = 0;
      return  true;
    }

    return  false;
  }

  //  Start前の事前準備
  bool BattleRoyalResultUpperRankingView::PreStart()
  {
    if( m_startSeq == 0 )
    {
      App::BattleRoyalResult::APP_PARAM*  pAppParam = m_pWork->GetAppParam();
      GFL_ASSERT( pAppParam );

      //  トレーナーアイコンロード
      bool  bNextSeq  = true;

      u32 loopNum = GFL_NELEMS( m_pTrainerFrame );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( m_pTrainerFrame[ i ]->LoadTrainerIcon( pAppParam->aTrainerData[ i ], m_pWork->GetTrainerIconUtil() ) == false )
        {
          bNextSeq  = false;
        }
      }

      if( bNextSeq == true )
      {
        ++m_startSeq;
      }
    }
    else
    if( m_startSeq == 1 )
    {
      ++m_startSeq;
    }
    else
    {
      return  true;
    }

    return  false;
  }

    //  開始
  void BattleRoyalResultUpperRankingView::Start()
  {
    //  演出開始
    _SetState( STATE_ENTRY_EVENT );
  }

  //  更新
  void BattleRoyalResultUpperRankingView::Update()
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    if( m_nowState == STATE_ENTRY_EVENT )
    {
      //  登場イベント
      //  どの順番で登場させるか決める
      App::BattleRoyalResult::APP_PARAM*  pAppParam = m_pWork->GetAppParam();
      GFL_ASSERT( pAppParam );

      bool  bNextState  = false;
      if( _IsNextEntryTrainerFrameOrder() == true )
      {
        if( _EntryTrainerFrameOrder() == true )
        {
        }
        else
        {
          //  すべてのトレーナーの演出終了
          bNextState  = true;
        }
      }

      if( bNextState == true )
      {
        //  自身のトレーナーが１位なら特別演出をする
        if( pAppParam->selfRanking == 1 )
        {
          _SetState( STATE_SELF_NO_1_EVENT );
        }
        else
        {
          //  終了
          _SetState( STATE_IDLE );
        }
      }

      //  常時再生するアニメ
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000_LOOP );
    }

    if( m_nowState == STATE_SELF_NO_1_EVENT )
    {
      if( m_nowStateSeq == 0 )
      {
        if( pG2DUtil->IsAnimeEnd( LAYOUT_WORK_ID_UPPER, LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000_NO1_EFFECT_IN ) == true )
        {
          pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000_NO1_EFFECT_LOOP );

          ++m_nowStateSeq;
        }
      }
      else
      {
        _SetState( STATE_IDLE );
      }
    }

    //  基本のも実行
    NetAppLib::System::NetApplicationViewBase::Update();
  }

  void BattleRoyalResultUpperRankingView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
  }

  //  ステート設定
  void BattleRoyalResultUpperRankingView::_SetState( const State newState )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    m_nowState    = newState;
    m_nowStateSeq = 0;

    if( m_nowState == STATE_ENTRY_EVENT )
    {
      //  登場
      _EntryTrainerFrameOrder();
    }
    else
    //  自身が１位の時の特殊演出
    if( m_nowState == STATE_SELF_NO_1_EVENT )
    {
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, LA_BATTLEROYALRESULTLYT_BRL_MIN_UPP_000_NO1_EFFECT_IN );

      Sound::PlaySE( SEQ_SE_SYS_NJ_001 );
    }
  }

  //  トレーナーフレームの順位エントリー開始
  bool BattleRoyalResultUpperRankingView::_EntryTrainerFrameOrder()
  {
    m_pNowEntryEventTrainerFrame  = NULL;

    //  どの順番に出すか
    u32 loopNum = GFL_NELEMS( m_pTrainerFrame );
    if( loopNum <= m_rankEntryOrderCnt )
    {
      //  もうしない
      return  false;
    }

    { 
      m_pNowEntryEventTrainerFrame  = m_pTrainerFrameEvtList[ m_rankEntryOrderCnt ];
      m_pNowEntryEventTrainerFrame->StartAnime( TrainerFrame::ANIME_ID_IN );

      if( m_pNowEntryEventTrainerFrame->GetRankingAnimeId() == BattleRoyalResultUpperRankingView::TrainerFrame::ANIME_ID_WIN )
      {
        //  1位の場合は専用アニメを追加
        m_pNowEntryEventTrainerFrame->StartAnime( TrainerFrame::ANIME_ID_GETTER );
      }

      //  表示優先度を変える
      m_pNowEntryEventTrainerFrame->SetAttachPane( m_pTrainerFramePriorityPane );

      Sound::PlaySE( SEQ_SE_SYS_NJ_000 );

      ++m_rankEntryOrderCnt;

    }

    return  true;
  }

  //  次のトレーナーフレームの順位エントリーできるか
  bool BattleRoyalResultUpperRankingView::_IsNextEntryTrainerFrameOrder()
  {
    if( m_pNowEntryEventTrainerFrame == NULL )
    {
      return  true;
    }

    return  m_pNowEntryEventTrainerFrame->IsEndAnime( TrainerFrame::ANIME_ID_IN );

  }

//  TrainerFrameクラスの定義
  BattleRoyalResultUpperRankingView::TrainerFrame::TrainerFrame( const u8 id, const INIT_PARAM& rInitParam, app::ui::UIView* pRootView, gfl2::lyt::LytWk* pLytWork, gfl2::lyt::LytMultiResID* pResId, app::util::Heap* pAppHeap )
  {
    GFL_ASSERT( pRootView );
    GFL_ASSERT( pAppHeap );

    _Clear();

    m_initParam = rInitParam;
    m_pG2DUtil  = pRootView->GetG2DUtil();
    m_pAppHeap  = pAppHeap;

    GFL_ASSERT( m_initParam.pPartsPane );
    GFL_ASSERT( pLytWork );
    GFL_ASSERT( pResId );

    m_id  = id;

    //  トレーナーアイコンペイン取得
    {
      m_pIconPicPane  = pLytWork->GetPicturePane( m_initParam.pPartsPane, PANENAME_BRL_WIN_UPP_000_PANE_PLAYERPHOTO, pResId );
      GFL_ASSERT( m_pIconPicPane );
    }

    //  名前テキストペイン取得
    {
      m_pNameTextPane = pLytWork->GetTextBoxPane( m_initParam.pPartsPane, PANENAME_BRL_WIN_UPP_000_PANE_PLAYER_NAME_00, pResId );
      GFL_ASSERT( m_pNameTextPane );
    }

    //  ポケモンを倒した数
    {
      const u32 aStarIconPaneIdx[]  =
      {
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_00,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_01,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_02,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_03,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_04,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_05,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_06,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_07,
        PANENAME_BRL_WIN_UPP_000_PANE_STARICON_08,
      };

      GFL_ASSERT( GFL_NELEMS( aStarIconPaneIdx ) == GFL_NELEMS( m_pStarPaneList ) );

      gfl2::lyt::LytPane* pStarPane = NULL;
      u32 loopNum = GFL_NELEMS( aStarIconPaneIdx );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pStarPane = pLytWork->GetPane( m_initParam.pPartsPane, aStarIconPaneIdx[ i ], pResId );
        m_pG2DUtil->SetPaneVisible( pStarPane, false );

        GFL_ASSERT( pStarPane );
        m_pStarPaneList[ i ]  = pStarPane;
      }
    }

    //  ポケモンのモンスターボール一覧
    {
      const u32 aMonBallIconPaneIdx[]  =
      {
        PANENAME_BRL_WIN_UPP_000_PANE_BALLICON_00,
        PANENAME_BRL_WIN_UPP_000_PANE_BALLICON_01,
        PANENAME_BRL_WIN_UPP_000_PANE_BALLICON_02,
      };

      GFL_ASSERT( GFL_NELEMS( aMonBallIconPaneIdx ) == GFL_NELEMS( m_pMonBallPaneList ) );

      gfl2::lyt::LytPane* pMonBallPane = NULL;
      u32 loopNum = GFL_NELEMS( aMonBallIconPaneIdx );
      for( u32 i = 0; i < loopNum; ++i )
      {
        pMonBallPane = pLytWork->GetPane( m_initParam.pPartsPane, aMonBallIconPaneIdx[ i ], pResId );
        m_pG2DUtil->SetPaneVisible( pMonBallPane, false );

        GFL_ASSERT( pMonBallPane );
        m_pMonBallPaneList[ i ]  = pMonBallPane;
      }
    }
  }

  BattleRoyalResultUpperRankingView::TrainerFrame::~TrainerFrame()
  {
  }

  //  開始
  bool BattleRoyalResultUpperRankingView::TrainerFrame::InitFunc( const App::BattleRoyalResult::TRINER_BTL_RESULT_DATA& rTrainerData )
  {
    if( m_initSeq == 0 )
    {
      bool  bNextSeq  = true;
      if( bNextSeq == true )
      {
        ++m_initSeq;
      }
    }
    else
    if( m_initSeq == 1 )
    {
      //  パラメータで表現を変える

      //  順位でフレーム表示変える
      {
        const AnimeId aLytAnimIdIdx[] =
        {
          ANIME_ID_LOSE,
          ANIME_ID_WIN,
          ANIME_ID_LOSE,
          ANIME_ID_LOSE,
          ANIME_ID_LOSE,
        };

        GFL_ASSERT( rTrainerData.ranking < GFL_NELEMS( aLytAnimIdIdx ) );
        //  1 - 4 の値でないとアウト
        GFL_ASSERT( ( 1 <= rTrainerData.ranking ) && ( rTrainerData.ranking <= 4 ) );

        m_ranking = rTrainerData.ranking;
        m_rankingAnimeId  = aLytAnimIdIdx[ rTrainerData.ranking ];
        m_pG2DUtil->StartAnime( m_initParam.lytId, m_initParam.aAnimeId[ m_rankingAnimeId ] );
      }

      //  名前設定
      {
        m_pG2DUtil->SetRegisterWord( 0, *rTrainerData.pNameBuf );
        m_pG2DUtil->SetTextBoxPaneStringExpand( m_pNameTextPane, battle_r_result_01_01 );
      }

      //  コーナーの色を指定
      {
        m_pG2DUtil->StartAnimeFrame( m_initParam.lytId, m_initParam.aAnimeId[ ANIME_ID_CORRER_COLOR ], rTrainerData.correrColor, false );
      }

      //  ポケモンを倒した数
      {
        u32 loopNum = rTrainerData.killCount;
        loopNum = gfl2::math::Clamp<u32>( loopNum, 0, GFL_NELEMS( m_pStarPaneList ) );
        for( u32 i = 0; i < rTrainerData.killCount; ++i )
        {
          m_pG2DUtil->SetPaneVisible( m_pStarPaneList[ i ], true );
        }
      }

      //  プレイヤーのポケモン状況
      {
        u32 loopNum = GFL_NELEMS( rTrainerData.aPokeData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          //  ＨＰがあるポケモンはボール表示
          m_pG2DUtil->SetPaneVisible( m_pMonBallPaneList[ i ], (  0 < rTrainerData.aPokeData[ i ].hp ) );
        }
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 2 )
    {
      bool  bNextSeq  = true;
      if( bNextSeq == true )
      {
        ++m_initSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  bool BattleRoyalResultUpperRankingView::TrainerFrame::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bNextSeq  = true;
      if( bNextSeq ==  true )
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

  //  開始
  void BattleRoyalResultUpperRankingView::TrainerFrame::StartAnime( const AnimeId animeId )
  {
    m_pG2DUtil->StartAnime( m_initParam.lytId, m_initParam.aAnimeId[ animeId ] );
  }

  //  アニメが終了しているか
  bool BattleRoyalResultUpperRankingView::TrainerFrame::IsEndAnime( const AnimeId animeId )
  {
    return  m_pG2DUtil->IsAnimeEnd( m_initParam.lytId, m_initParam.aAnimeId[ animeId ] );
  }

  //  ペインをアタッチ
  void BattleRoyalResultUpperRankingView::TrainerFrame::SetAttachPane( gfl2::lyt::LytPane* pAttachPane )
  {
    if( pAttachPane == NULL )
    {
      GFL_ASSERT( pAttachPane );
      return;
    }

    gfl2::lyt::LytPane* pParentPane = m_initParam.pPartsPane->GetParent();
    if( pParentPane != NULL )
    {
      //  はずす
      pParentPane->RemoveChild( m_initParam.pPartsPane );
    }

    //  新しいのにくっつける
    pAttachPane->AppendChild( m_initParam.pPartsPane );
  }

  //  トレーナーアイコンをロード
  bool BattleRoyalResultUpperRankingView::TrainerFrame::LoadTrainerIcon( const App::BattleRoyalResult::TRINER_BTL_RESULT_DATA& rTrainerData, NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtil )
  {
    GFL_ASSERT( pTrainerIconUtil );

    if( m_loadTraineIconSeq == 0 )
    {
      //  アイコンをロード
      {
        //  トレーナーアイコンデータを使えるデータに変換
        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA  iconData;
        iconData.ConvFromMyStatusIconData( rTrainerData.iconObjData );

        //  NPCアイコンの場合もありえるのでtrainerIdを設定しなおし
        iconData.trainerId  = rTrainerData.trainerType;

        iconData.pReplacePane = m_pIconPicPane;

        pTrainerIconUtil->SetIconObjectData( m_id, iconData );
      }

      ++m_loadTraineIconSeq;
    }
    else
    if( m_loadTraineIconSeq == 1 )
    {
      bool  bNextSeq  = true;
      //  トレーナーアイコンのロードが終っているか
      if( pTrainerIconUtil->IsEndIconObjectData( m_id ) == false )
      {
        bNextSeq  = false;
      }

      if( bNextSeq == true )
      {
        ++m_loadTraineIconSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
