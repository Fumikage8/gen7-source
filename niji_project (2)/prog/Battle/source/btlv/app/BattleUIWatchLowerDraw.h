//=============================================================================
/**
 * @file BattleUIWatchLowerDraw.cpp
 * @brief バトル下画面 >> 「観戦画面」画面
 * @author yuto_uchida
 * @date 2015.05.07
 */
//=============================================================================
#if !defined( __BATTLE_UI_WATCH_LOWER_DRAW_H__ )
#define __BATTLE_UI_WATCH_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

#include "Battle/source/btl_pokeparam.h"
#include "Battle/source/btlv/app/BattleUIPokeStatusLowerDraw.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面の観戦画面の通知クラス */
  class BattleUIWatchLowerDrawDelegate
  {
  public:
    BattleUIWatchLowerDrawDelegate() {}
    virtual ~BattleUIWatchLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUIWatchLowerDraw* pDrawInst, s32 inputType )  = 0;

    /// キー入力通知用リスナ
    virtual bool OnKeyAction( class BattleUIWatchLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /**
  * @brief バトル下画面の観戦画面の入力制御クラス
  */
  class BattleUIWatchLowerInputInterface
  {
  public:
      BattleUIWatchLowerInputInterface( class BattleUIWatchLowerDraw* pDrawCore )
      {
        GFL_ASSERT( pDrawCore );
        m_pDrawCore = pDrawCore;
      }

      //------------------------------------------------------------------
      /**
       * @brief   もどるボタンを押した
       */
      //------------------------------------------------------------------
      virtual void PushBtnBack()  = 0;

   protected:
      class BattleUIWatchLowerDraw* m_pDrawCore;
  };

  /**
  * @brief バトル下画面の行動選択画面基本クラス
  */
  class BattleUIWatchLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIWatchLowerDraw );  // コピーを禁止

    public:
      friend class BattleUIWatchLowerInputNormal;

      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_IDLE,
        STATE_TYPE_ENTER_ANIM_WAIT,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      //  画面タイプ
      enum ScreenType
      {
        SCREEN_TYPE_NORMAL  = 0,
        SCREEN_TYPE_DOUBLE,
        SCREEN_TYPE_ROYALE,

        SCREEN_TYPE_MAX,
      };

      //  ボタンId一覧
      enum ButtonId
      {
        //  戻る
        BUTTON_ID_BACK  = 0,

        //  左下のポケモン、わざインフォ、プレイヤー名
        BUTTON_ID_POKE_ICON_LEFT_DOWN,
        BUTTON_ID_USER_NAME_LEFT_DOWN,
        BUTTON_ID_SKILL_INFO_LEFT_DOWN,

        //  右下のポケモン、わざインフォ、プレイヤー名
        BUTTON_ID_POKE_ICON_RIGHT_DOWN,
        BUTTON_ID_USER_NAME_RIGHT_DOWN,
        BUTTON_ID_SKILL_INFO_RIGHT_DOWN,

        //  左上のポケモン、わざインフォ、プレイヤー名
        BUTTON_ID_POKE_ICON_LEFT_UP,
        BUTTON_ID_USER_NAME_LEFT_UP,
        BUTTON_ID_SKILL_INFO_LEFT_UP,

        //  右上のポケモン、わざインフォ、プレイヤー名
        BUTTON_ID_POKE_ICON_RIGHT_UP,
        BUTTON_ID_USER_NAME_RIGHT_UP,
        BUTTON_ID_SKILL_INFO_RIGHT_UP,

        BUTTON_ID_MAX,
      };

      //  ポケモンアイコン数
      enum PokeIconPosType
      {
        POKE_ICON_POS_TYPE_LEFT_DOWN  = 0,
        POKE_ICON_POS_TYPE_RIGHT_DOWN,
        POKE_ICON_POS_TYPE_LEFT_UP,
        POKE_ICON_POS_TYPE_RIGHT_UP,

        POKE_ICON_POS_TYPE_MAX,
      };

      //  ポケモンパラメータ
      typedef struct
      {
        s32                                               type;
        btl::BTL_POKEPARAM*                               p_btl_poke_param;
        const BattleUIDataManager::POKE_STATUS_DATA*  p_btl_poke_status;

        //  最後に使ったわざ
        struct
        {
          WazaID                                          normal_waza_id;
          WazaID                                          zenryoku_waza_id;
          u32                                             pp_now;
          u32                                             pp_max;
          bool                                            b_zenryoku_waza;

        } last_waza_data;

        //  プレイヤー名
        gfl2::str::StrBuf*                                p_player_name;
      } POKE_DATA;

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        //  描画用ヒープ
        ::app::util::Heap* pUtilHeap;

        ::app::util::AppRenderingManager* pRenderingManager;

        BattleUIWatchLowerDrawDelegate* pDelegate;
        void* pLytBuff;

        POKE_DATA   aPokeData[ POKE_ICON_POS_TYPE_MAX ];

        //  画面タイプ
        ScreenType  screenType;

        gfl2::str::MsgData*       pBtlAppMsgData;
        print::WordSet*           pBtlWordSet;

        bool        bUseInputSE;

        _tag_init_param()
        {
          pUtilHeap           = NULL;
          pRenderingManager   = NULL;
          pDelegate           = NULL;
          pLytBuff            = NULL;
          pBtlAppMsgData      = NULL;
          pBtlWordSet         = NULL;

          screenType          = SCREEN_TYPE_NORMAL;
          bUseInputSE         = false;

          gfl2::std::MemFill( aPokeData, NULL, sizeof( aPokeData ) );
        }

      } INIT_PARAM;

      //------------------------------------------------------------------
      /**
       * @brief   コンストラクタ
       * @note
       *    あらかじめ使う側がレイアウトデータを作成して渡す。
       */
      //------------------------------------------------------------------
      BattleUIWatchLowerDraw( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //------------------------------------------------------------------
      virtual ~BattleUIWatchLowerDraw();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      virtual void Initialize();

      //------------------------------------------------------------------
      /**
       * @brief   終了処理
       */
      //------------------------------------------------------------------
      virtual void Terminate()
      {
        u32 loopNum = GFL_NELEMS( m_aPokeObj );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_aPokeObj[ i ].Terminate();
        }
      }

      //------------------------------------------------------------------
      /**
       * @brief   破棄可能か
       */
      //------------------------------------------------------------------
      virtual bool IsDelete()
      {
        bool  bRet  = true;

        u32 loopNum = GFL_NELEMS( m_aPokeObj );
        for( u32 i = 0; i < loopNum; ++i )
        {
          //  アイコン処理が終わるまでアイコン以外の更新は進めない
          if( m_aPokeObj[ i ].IsDelete() == false )
          {
            bRet  = false;
            break;
          }
        }

        if( UIView::IsDrawing() == true )
        {
          bRet  = false;
        }

        return bRet;
      }

      //------------------------------------------------------------------
      /**
       * @brief   開始
       */
      //------------------------------------------------------------------
      void Start( void ) { _SetState( STATE_TYPE_IN ); }

      //------------------------------------------------------------------
      /**
       * @brief   終了
       */
      //------------------------------------------------------------------
      void End( void ) { _SetState( STATE_TYPE_OUT ); }

      //------------------------------------------------------------------
      /**
       * @brief   更新処理
       */
      //------------------------------------------------------------------
      virtual void Update( void );

      //------------------------------------------------------------------
      /**
       * @brief   描画
       */
      //------------------------------------------------------------------
      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      //------------------------------------------------------------------
      /**
       * @brief   ボタン項目表示設定
       */
      //------------------------------------------------------------------
      void SetVisibleButton( ButtonId buttonId, bool bVisible );

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

      //------------------------------------------------------------------
      /**
       * @brief   初期化データ取得
       */
      //------------------------------------------------------------------
      INIT_PARAM& GetInitParam() { return m_initParam; }

      //------------------------------------------------------------------
      /**
       * @brief   プレイヤー名のテキスト設定
       */
      //------------------------------------------------------------------
      void SetPlayerNameText( gfl2::str::StrBuf* pPlayerNameArray, const s32 arrayNum );
      
      //------------------------------------------------------------------
      /**
       * @brief   ポケモンデータ取得
       */
      //------------------------------------------------------------------
      const POKE_DATA& GetPokeData( const PokeIconPosType pokeIconPosType )
      {
        return m_aPokeObj[ pokeIconPosType ].GetPokeData();
      }

      //------------------------------------------------------------------
      /**
       * @brief   ペインタッチ時を検知
       */
      //------------------------------------------------------------------
      virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 buttonId );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief ペインタッチ時を検知
      * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
      */
      //--------------------------------------------------------------------------------------------
      virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

      //------------------------------------------------------------------
      /**
       * @brief   キー入力通知用リスナ
       */
      //------------------------------------------------------------------
      virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      enum
      {
        PLYAER_NAME_TEXT_PANE_MAX = 3,
      };

      //  ポケモンオブジェクト
      class PokeObj
      {
      public:
        //  ポケモンアイコンアニメ
        enum PokeIconAnime
        {
          POKE_ICON_ANIME_DIE = 0,
          POKE_ICON_ANIME_REVIVE,

          POKE_ICON_ANIME_MAX,
        };

        //  作成ローカルデータ構造
        typedef struct
        {
          BattleUIWatchLowerDraw::PokeIconPosType  pos_type;

          //  ポケモンアイコンのペインアドレス
          gfl2::lyt::LytParts*  p_poke_icon_pane;

          //  ポケモンアイコンボタンid
          ButtonId  poke_icon_button_id;

          //  わざインフォボタンid
          ButtonId  waza_info_icon_buffon_id;

          //  ポケモンアイコンにくっついているインフォペインid
          u32 info_icon_parts_pane_id;

          //  ポケモンが最後に使った技名のペインのルートペイン
          u32 waza_name_root_pane_id;

          //  ポケモンが最後に使った技名のペイン
          u32 waza_name_pane_id;

          //  プレイヤー名表示パーツペインid
          u32 player_name_parts_pane_id;

          //  ポケモンアイコンにアニメ
          u32 a_anime_id[ POKE_ICON_ANIME_MAX ];

        } INIT_PARAM;

        PokeObj()
        {
          m_pIcon             = NULL;
          m_pInfoIconPane     = NULL;
          m_pWazaNamePane     = NULL;
          m_pWazaNameRootPane = NULL;
          m_playAnimeIdx      = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;

          //  レイアウトのアニメidのエラー値で初期化
          {
            u32 loopNum = GFL_NELEMS( m_aAnimeIdx );
            for( u32 i = 0; i < loopNum; ++i )
            {
              m_aAnimeIdx[ i ]  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            }
          }
          gfl2::std::MemFill( &m_baseData, NULL, sizeof( m_baseData ) );
        }

        ~PokeObj()
        {
          GFL_SAFE_DELETE( m_pIcon );
        }

        //  初期化
        void Initialize( UIView* pView, ::app::util::Heap* pUtilHeap, const u32 lytId, gfl2::lyt::LytMultiResID* pMultiResId, const INIT_PARAM& rInitParam, const POKE_DATA* pPokeData );

        //  終了
        void Terminate()
        {
        }

        //  破棄完了か
        bool IsDelete();

        //  更新
        void Update();

        //  ポケモンアイコンアニメ再生
        void StartAnimePokeIcon( const PokeIconAnime animeIdx, const bool bForcePlayAnime );

        //  ポケモンデータ取得
        const POKE_DATA& GetPokeData() const
        {
          GFL_ASSERT( m_baseData.p_btl_poke_param );
          return m_baseData;
        }

      private:
        ::app::tool::PokeIconToolEx*      m_pIcon;
        gfl2::lyt::LytParts*              m_pInfoIconPane;
        gfl2::lyt::LytTextBox*            m_pWazaNamePane;
        gfl2::lyt::LytPane*               m_pWazaNameRootPane;

        POKE_DATA                         m_baseData;
        s32                               m_aAnimeIdx[ POKE_ICON_ANIME_MAX ];
        s32                               m_playAnimeIdx;
      };

      //  ボタン作成データ
      typedef struct
      {
        u32 touch_anime_index;
        u32 release_anime_index;
        u32 cancel_anime_index;
        u32 key_select_anime_index;
        u32 active_anime_index;
        u32 passive_anime_index;

        u32 buttonPartPaneId;
        u32 partsBoundPaneId;

      } BUTTON_CREATE_DATA;

      //------------------------------------------------------------------
      /**
       * @brief   画面タイプ設定
       */
      //------------------------------------------------------------------
      void _SetScreenType( const ScreenType newType );

      //------------------------------------------------------------------
      /**
       * @brief   ステート設定
       */
      //------------------------------------------------------------------
      virtual void _SetState( const StateType newStateType );

      //------------------------------------------------------------------
      /**
       * @brief   各ボタンの登録データ作成
       * @param   createButtonType  : 作成するボタンデータのタイプ
       * @param   buttonId          : ボタンID
       */
      //------------------------------------------------------------------
      ::app::ui::ButtonInfoEx _CreateButtonData( ButtonId createButtonType );

      //------------------------------------------------------------------
      /**
       * @brief   ポケモンオブジェクト作成
       */
      //------------------------------------------------------------------
      PokeObj* _CreatePokeData( const s32 pokeDataIdx, const POKE_DATA* pPokeData );

      void _Clear()
      {
        gfl2::std::MemFill( m_pBtnPartRootPaneList, NULL, sizeof( m_pBtnPartRootPaneList ) );
        gfl2::std::MemClear( m_pPlayerNameTextBoxPaneArray, sizeof( m_pPlayerNameTextBoxPaneArray ) );

        m_state               = STATE_TYPE_NONE;
        m_pDelegate           = NULL;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();

      //  変数宣言一覧

      INIT_PARAM                            m_initParam;

      StateType                             m_state;

      BattleUIWatchLowerDrawDelegate*       m_pDelegate;

      PokeObj                               m_aPokeObj[ POKE_ICON_POS_TYPE_MAX ];

      //  プレイヤー名テキストペイン一覧
      gfl2::lyt::LytTextBox*                m_pPlayerNameTextBoxPaneArray[ PLYAER_NAME_TEXT_PANE_MAX ];

      //  画面ボタンルートペイン一覧
      gfl2::lyt::LytParts* m_pBtnPartRootPaneList[ BUTTON_ID_MAX ];

      //  ボタン作成データテーブル
      static const BUTTON_CREATE_DATA m_aCreateButtonDataTable[ BUTTON_ID_MAX ];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_WATCH_LOWER_DRAW_H__ 
