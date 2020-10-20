// ============================================================================
/*
 * @file BattleRoyalResultUpperRankingView.h
 * @brief バトルロイヤル勝敗アプリの順位上画面を表示するクラスです。
 * @date 2015.10.26
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATLLE_ROYAL_RESULT_UPPER_RANKING_VIEW_H_INCLUDE )
#define BATLLE_ROYAL_RESULT_UPPER_RANKING_VIEW_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

//  アプリのパラメータ
#include "App/BattleRoyalResult/include/BattleRoyalResultAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

  class NetAppTrainerIconUtility;

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeIconToolEx;
  class AppToolHPGaugePartsLayoutGAG_002;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)

  class BattleRoyalResultWork;

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(View)

  class BattleRoyalResultUpperRankingView :
    public NetAppLib::System::NetApplicationViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultUpperRankingView );
  public:
    enum State
    {
      STATE_NONE  = 0,
      STATE_ENTRY_EVENT,
      STATE_SELF_NO_1_EVENT,
      STATE_IDLE
    };

    BattleRoyalResultUpperRankingView( App::BattleRoyalResult::System::BattleRoyalResultWork* pWork );
    virtual~BattleRoyalResultUpperRankingView();

    //  初期化
    bool InitFunc();
    //  終了
    bool EndFunc();

    //  Start前の事前準備
    bool PreStart();

    //  開始
    void Start();

    //  更新
    virtual void Update();

    // UIView
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    //  現在のステート取得
    State GetState() const { return m_nowState; }

  private:
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID_UPPER,
    };

    //  トレーナーフレーム
    class TrainerFrame
    {
    public:
      //  扱うアニメ一覧
      enum AnimeId
      {
        ANIME_ID_IN = 0,
        ANIME_ID_GETTER,
        ANIME_ID_WIN,
        ANIME_ID_LOSE,
        ANIME_ID_CORRER_COLOR,
        ANIME_ID_MAX
      };

      //  初期パラメータ
      struct INIT_PARAM
      {
        u32 lytId;
        
        gfl2::lyt::LytParts*  pPartsPane;

        //  再生するアニメid
        u32 aAnimeId[ ANIME_ID_MAX ];
      };

      TrainerFrame( const u8 id, const INIT_PARAM& rInitParam, app::ui::UIView* pRootView, gfl2::lyt::LytWk* pLytWork, gfl2::lyt::LytMultiResID* pResId, app::util::Heap* pAppHeap );
      virtual ~TrainerFrame();

      bool InitFunc( const App::BattleRoyalResult::TRINER_BTL_RESULT_DATA& rTrainerData );
      bool EndFunc();

      bool LoadTrainerIcon( const App::BattleRoyalResult::TRINER_BTL_RESULT_DATA& rTrainerData, NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtil );

      //  開始
      void StartAnime( const AnimeId animeId );
      //  アニメが終了しているか
      bool IsEndAnime( const AnimeId animeId );

      const AnimeId GetRankingAnimeId() const { return m_rankingAnimeId; }

      u8 GetRanking() const { return m_ranking; }

      //  ペインをアタッチ
      void SetAttachPane( gfl2::lyt::LytPane* pAttachPane );

    private:
      enum
      {
        STAR_PANE_MAX = 9,
      };

      void _Clear()
      {
        m_pNameTextPane = NULL;
        m_pIconPicPane  = NULL;
        m_pG2DUtil      = NULL;
        m_pAppHeap      = NULL;

        gfl2::std::MemClear( m_pStarPaneList, sizeof( m_pStarPaneList ) );
        gfl2::std::MemClear( m_pMonBallPaneList, sizeof( m_pMonBallPaneList ) );

        m_initSeq = m_endSeq = m_loadTraineIconSeq = 0;
        m_id      = 0;
        m_ranking = 0;
        m_rankingAnimeId = ANIME_ID_MAX;
      }

      //  名前テキストペイン
      gfl2::lyt::LytTextBox*  m_pNameTextPane;

      //  トレーナーアイコンペイン
      gfl2::lyt::LytPicture*  m_pIconPicPane;

      //  星アイコンペインリスト
      gfl2::lyt::LytPane* m_pStarPaneList[ STAR_PANE_MAX ];

      //  パーティーのモンスターボールアイコンペインリスト
      gfl2::lyt::LytPane* m_pMonBallPaneList[ App::BattleRoyalResult::POKE_DATA_MAX ];

      INIT_PARAM  m_initParam;

      app::util::G2DUtil* m_pG2DUtil;
      app::util::Heap*    m_pAppHeap;

      App::BattleRoyalResult::TRINER_BTL_RESULT_DATA  m_trainerData;      

      u32 m_initSeq, m_endSeq, m_loadTraineIconSeq;
      u8  m_ranking;
      u8  m_id; //  0～3まで
      AnimeId m_rankingAnimeId;
    };

    //  ステート設定
    void _SetState( const State newState );

    //  トレーナーフレームの順位エントリー開始
    bool _EntryTrainerFrameOrder();

    //  次のトレーナーフレームの順位エントリーできるか
    bool _IsNextEntryTrainerFrameOrder();

    void _Clear()
    {
      m_pWork                       = NULL;
      m_pNowEntryEventTrainerFrame  = NULL;
      m_pTrainerFramePriorityPane   = NULL;
      gfl2::std::MemClear( m_pTrainerFrame, sizeof( m_pTrainerFrame ) );
      gfl2::std::MemClear( m_pTrainerFrameEvtList, sizeof( m_pTrainerFrameEvtList ) );

      m_initSeq  = m_endSeq  = m_startSeq = 0;
      m_rankEntryOrderCnt = 0;
      m_nowState  = STATE_NONE;
      m_nowStateSeq = 0;
    }

    TrainerFrame*  m_pTrainerFrame[ App::BattleRoyalResult::TRAINER_FRAME_POS_MAX ];
    TrainerFrame*  m_pTrainerFrameEvtList[ App::BattleRoyalResult::TRAINER_FRAME_POS_MAX ];

    App::BattleRoyalResult::System::BattleRoyalResultWork*     m_pWork;

    u32 m_initSeq, m_endSeq, m_startSeq;
    u32 m_rankEntryOrderCnt;

    State m_nowState;
    u32   m_nowStateSeq;

    TrainerFrame* m_pNowEntryEventTrainerFrame;

    gfl2::lyt::LytPane* m_pTrainerFramePriorityPane;
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // BATLLE_ROYAL_RESULT_UPPER_RANKING_VIEW_H_INCLUDE
