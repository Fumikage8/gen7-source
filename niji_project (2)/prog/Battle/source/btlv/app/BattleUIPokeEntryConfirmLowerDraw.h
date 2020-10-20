//=============================================================================
/**
 * @file BattleUIPokeEntryConfirmLowerDraw.h
 * @brief バトル下画面 >> 「バトル参加確認画面」画面
 * @author yuto_uchida
 * @date 2015.04.23
 */
//=============================================================================
#if !defined( __BATTLE_UI_POKE_ENTRY_CONFIRM_LOWER_DRAW_H__ )
#define __BATTLE_UI_POKE_ENTRY_CONFIRM_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"

#include "Battle/include/battle_SetupParam.h"
#include "Battle/source/btl_pokeparam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  class AppCommonGrpIconData;

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面のポケモン参加確認画面の通知クラス */
  class BattleUIlPokeEntryConfirmLowerDrawDelegate
  {
  public:
    BattleUIlPokeEntryConfirmLowerDrawDelegate() {}
    virtual ~BattleUIlPokeEntryConfirmLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUIPokeEntryConfirmLowerDraw* pDrawInst, class BattleUIPokeEntryConfirmtLowerButtonCtrl* pButtonCtrl, s32 buttonId )  = 0;

    /// キー入力通知用リスナ
    virtual bool OnKeyAction( class BattleUIPokeEntryConfirmLowerDraw* pDrawInst, class BattleUIPokeEntryConfirmtLowerButtonCtrl* pButtonCtrl, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /** ビューのボタン制御 */
  class BattleUIPokeEntryConfirmtLowerButtonCtrl : public ::app::ui::UIInputListener
  {
  public:
    friend class BattleUIPokeEntryConfirmLowerDraw;

    //  描画側で入力した結果id
    enum ButtonId
    {
      BUTTON_ID_CANCEL,
      BUTTON_ID_CANCEL_02,
      BUTTON_ID_LEFT_ARROW,
      BUTTON_ID_RIGHT_ARROW,

      BUTTON_ID_MAX
    };

    //  コンストラクタ
    BattleUIPokeEntryConfirmtLowerButtonCtrl( BattleUIlPokeEntryConfirmLowerDrawDelegate* pDelegate ) { _Clear(); m_pDelegate = pDelegate; }

    ~BattleUIPokeEntryConfirmtLowerButtonCtrl() {}

    //  ボタン関連のアニメが終わっているか
    bool IsAnimEnd();

    //  ボタンを押す
    void Push( const ButtonId id );

    // ペインタッチ時を検知
    virtual ListenerResult OnLayoutPaneEvent( const u32 buttonId );
    /// キー入力通知用リスナ
    virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //  ボタン表示設定
    void SetButtonVisible( const ButtonId id, const bool bFlg );

  private:
    //------------------------------------------------------------------
    /**
      * @brief   処理前に必ず呼ぶが、これはViewクラス内で呼ぶ前提で作成している
      */
    //------------------------------------------------------------------
    void _Setup( class BattleUIPokeEntryConfirmLowerDraw* pView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap, const bool bUseInputSE );

    void _Clear()
    {
      m_pDelegate = NULL;
      m_pView     = NULL;
      m_lytWkId   = 0;
    }

    BattleUIlPokeEntryConfirmLowerDrawDelegate* m_pDelegate;
    class BattleUIPokeEntryConfirmLowerDraw*    m_pView;
    s32                                         m_lytWkId;
  };

  /** バトル下画面のポケモン参加確認画面 */
  class BattleUIPokeEntryConfirmLowerDraw : public ::app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIPokeEntryConfirmLowerDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_INPUT_IDLE,
        STATE_TYPE_DATA_CHANGE,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      //  データ切り替えイベントタイプ
      enum ParamChangeEventType
      {
        PARAM_CHANGE_EVENT_TYPE_LEFT_MOVE = 0,
        PARAM_CHANGE_EVENT_TYPE_RIGHT_MOVE,
      };

      //  必要なポケモンデータ
      typedef struct
      {
        bool                                      b_on_battle_field;
        const pml::pokepara::PokemonParam*        p_poke_param;

      } POKE_DATA;

      //  プレイヤーパーティデータ
      typedef struct
      {
        bool                          b_enemy_party;
        bool                          b_rating;
        print::GrammaticalAttribute   player_grammer;
        const gfl2::str::StrBuf*      p_player_name;

        u32 rate;

        POKE_DATA poke_data[ BTL_PARTY_MEMBER_MAX ];

      } PARTY_DATA;

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                          pUtilHeap;
        ::app::util::AppRenderingManager*           pRenderingManager;
        //  アイコングラフィックデータ
        ::app::tool::AppCommonGrpIconData*          pAppCommonGrpIconData;

        BattleUIPokeEntryConfirmtLowerButtonCtrl*   pButtonCtrl;

        //  必要なメッセージとワード設定
        gfl2::str::MsgData*       pBtlAppMsgData;
        print::WordSet*           pBtlWordSet;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        //  入力関連のSEを使うか
        bool  bUseInputSE;

        _tag_init_param()
        {
          pUtilHeap          = NULL;
          pRenderingManager  = NULL;
          pLytBuff           = NULL;
          pButtonCtrl        = NULL;
          pBtlAppMsgData     = NULL;
          pBtlWordSet        = NULL;
          bUseInputSE        = false;

          pAppCommonGrpIconData = NULL;
        }

      } INIT_PARAM;

      //------------------------------------------------------------------
      /**
       * @brief   コンストラクタ
       */
      //------------------------------------------------------------------
      BattleUIPokeEntryConfirmLowerDraw( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //------------------------------------------------------------------
      virtual ~BattleUIPokeEntryConfirmLowerDraw();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      void Initialize();

      //------------------------------------------------------------------
      /**
       * @brief   開始
       */
      //------------------------------------------------------------------
      void Start() { _SetState( STATE_TYPE_IN ); }

      //------------------------------------------------------------------
      /**
       * @brief   終了
       */
      //------------------------------------------------------------------
      void End() { _SetState( STATE_TYPE_OUT ); }

      //------------------------------------------------------------------
      /**
        * @brief   終了処理
        */
      //------------------------------------------------------------------
      void Terminate();

      //------------------------------------------------------------------
      /**
        * @brief   破棄可能か
        */
      //------------------------------------------------------------------
      bool IsDelete();

      //------------------------------------------------------------------
      /**
       * @brief   更新処理
       */
      //------------------------------------------------------------------
      virtual void Update();

      //------------------------------------------------------------------
      /**
       * @brief   描画
       */
      //------------------------------------------------------------------
      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      //------------------------------------------------------------------
      /**
       * @brief   描画パラメータ設定
       * @note
       *    描画するパラメータを設定、矢印表示をするかどうかもここで決める
       */
      //------------------------------------------------------------------
      void SetParam( const PARTY_DATA& rPartyData, const bool bLeftArrowButtonVisible, const bool bRightArrowButtonVisible );
      
      //------------------------------------------------------------------
      /**
       * @brief   描画切り替えイベント
       * @note
       *    描画するパラメータを設定、矢印表示をするかどうかもここで決める
       */
      //------------------------------------------------------------------
      void ChangeParamEvent( const PARTY_DATA& rPartyData, const bool bLeftArrowButtonVisible, const bool bRightArrowButtonVisible, const ParamChangeEventType eventType );

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      //  データ切り替えイベントデータ
      struct CHANGE_PARAM_EVENT_DATA
      {
        CHANGE_PARAM_EVENT_DATA()
        {
          gfl2::std::MemClear( &partyData, sizeof( partyData ) );

          bLeftArrowButton  = false;
          bRightArrowButton = false;

          inAnimeId    = gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR;
          outAnimeId   = gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR;
        }

        PARTY_DATA  partyData;
        bool  bLeftArrowButton;
        bool  bRightArrowButton;

        u32   inAnimeId;
        u32   outAnimeId;
      };

      //  表示するポケモンオブジェクト
      class PokeObj
      {
      public:
        //  ポケモンアイコンアニメ
        enum PokeIconAnime
        {
          POKE_ICON_ANIME_DIE = 0,
          POKE_ICON_ANIME_REVIVE,

          POKE_ICON_GENDER_SEX_MALE,
          POKE_ICON_GENDER_SEX_FEMALE,

          POKE_ICON_ANIME_MAX,
        };

        //  作成ローカルデータ構造
        typedef struct _tag_poke_obj_init_param
        {
          //  アイコングラフィックデータ
          ::app::tool::AppCommonGrpIconData*          p_common_grp_icon_data;

          //  ポケモン表示ルートペイン
          gfl2::lyt::LytPane*  p_root_pane;

          //  ポケモンアイコンのペインアドレス
          gfl2::lyt::LytParts*  p_poke_icon_pane;

          //  レベルのテキストペイン
          gfl2::lyt::LytTextBox*  p_lv_num_pane;

          //  レベルの表記ペイン
          gfl2::lyt::LytTextBox*  p_lv_title_text_pane;

          //  性別のパーツペイン
          gfl2::lyt::LytParts*    p_gender_parts_pane;

          //  ポケモン非参加時の表示
          gfl2::lyt::LytPane*     p_not_join_pane;

          //  状態異常のアイコンルートペイン
          gfl2::lyt::LytParts*     p_sick_root_pane;

          //  状態異常のアイコンペイン
          gfl2::lyt::LytPicture*  p_sick_pic_pane;

          //  ポケモンアイコンにアニメ
          u32 a_anime_id[ POKE_ICON_ANIME_MAX ];

          //  初期化
          _tag_poke_obj_init_param()
          {
            p_common_grp_icon_data  = NULL;
            p_root_pane = NULL;
            p_poke_icon_pane = NULL;
            p_lv_num_pane = NULL;
            p_lv_title_text_pane = NULL;
            p_gender_parts_pane = NULL;
            p_not_join_pane = NULL;
            p_sick_root_pane = NULL;
            p_sick_pic_pane = NULL;

            for( u32 i = 0; i < POKE_ICON_ANIME_MAX; ++i )
            {
              a_anime_id[ i ] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            }
          }

        } INIT_PARAM;

        PokeObj()
        {
          m_pIcon             = NULL;
          m_pRootView         = NULL;
          m_pHaveItemIcon     = NULL;
          m_playAnimeIdx      = -1;
          m_lytId             = 0;

          gfl2::std::MemClear( m_aAnimeIdx, sizeof( m_aAnimeIdx ) );
        }

        ~PokeObj()
        {
          GFL_SAFE_DELETE( m_pIcon );
        }

        //  初期化
        void Initialize( UIView* pView, ::app::util::Heap* pUtilHeap, const u32 lytId, gfl2::lyt::LytMultiResID* pMultiResId, const INIT_PARAM& rInitParam );

        //  終了
        void Terminate()
        {
        }

        //  破棄完了か
        bool IsDelete()
        {
          if( m_pIcon != NULL )
          {
            if( m_pIcon->EndFunc() == false )
            {
              return  false;
            }
          }

          return  true;
        }

        //  更新
        void Update();

        //  表示パラメータ設定
        void SetDrawParam( const POKE_DATA* pPokeData );

        //  ポケモンアイコンアニメ再生
        void StartAnimePokeIcon( const PokeIconAnime animeIdx, const bool bForcePlayAnime );

        //  状態異常表示しているかどうか
        bool IsVisibleSick();

      private:
        ::app::tool::PokeIconToolEx*      m_pIcon;
        gfl2::lyt::LytPane*               m_pHaveItemIcon;
        PokeObj::INIT_PARAM               m_initParam;

        s32                               m_aAnimeIdx[ POKE_ICON_ANIME_MAX ];
        s32                               m_playAnimeIdx;

        UIView*                           m_pRootView;
        u32                               m_lytId;
      };

      void _Clear()
      {
        m_state               = STATE_TYPE_NONE;
        m_pPlayerNameTextPane = NULL;
        m_pRateNumberTextPane = NULL;
        m_pRateTitleTextPane  = NULL;
        m_subState            = 0;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //------------------------------------------------------------------
      /**
        * @brief   ポケモンオブジェクト作成
        */
      //------------------------------------------------------------------
      PokeObj* _CreatePokeData( const u32 pokeDataIdx );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;
      u32         m_subState;

      //  データ切り替えのイベントデータ
      CHANGE_PARAM_EVENT_DATA m_changeParamEventData;

      //  プレイヤー名のテキストペイン
      gfl2::lyt::LytTextBox*  m_pPlayerNameTextPane;

      //  レートのテキストペイン
      gfl2::lyt::LytTextBox*  m_pRateNumberTextPane;

      //  レートのタイトルペイン
      gfl2::lyt::LytTextBox*  m_pRateTitleTextPane;

      //  ポケモンオブジェクトリスト
      PokeObj m_aPokeObj[ BTL_PARTY_MEMBER_MAX ];

  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_POKE_ENTRY_CONFIRM_LOWER_DRAW_H__ 
