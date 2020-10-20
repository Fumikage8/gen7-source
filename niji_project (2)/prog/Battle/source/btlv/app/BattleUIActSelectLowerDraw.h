//=============================================================================
/**
 * @file BattleUIActSelectLowerDraw.cpp
 * @brief バトル下画面 >> 「行動選択画面」画面
 * @author yuto_uchida
 * @date 2015.03.10
 */
//=============================================================================
#if !defined( __BATTLE_UI_ACT_SELECT_LOWER_DRAW_H__ )
#define __BATTLE_UI_ACT_SELECT_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/ItemIcon.h"

#include "Battle/source/btl_pokeparam.h"
#include "Battle/source/btlv/app/BattleUIPokeStatusLowerDraw.h"

#include "Print/include/GrammaticalAttributeProvider.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

//@memo  各ボタン項目のグラフィックフォントはレイアウトデータで対応

  /** バトル下画面の行動選択画面の通知クラス */
  class BattleUIActSelectLowerDrawDelegate
  {
  public:
    BattleUIActSelectLowerDrawDelegate() {}
    virtual ~BattleUIActSelectLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUIActSelectLowerDraw* pDrawInst, s32 inputType )  = 0;

    /// キー入力通知用リスナ
    virtual bool OnKeyAction( class BattleUIActSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /**
  * @brief バトル下画面の行動選択画面の入力制御クラス
  */
  class BattleUIActSelectLowerInputInterface
  {
      GFL_FORBID_COPY_AND_ASSIGN( BattleUIActSelectLowerInputInterface );  // コピーを禁止

  public:
      BattleUIActSelectLowerInputInterface()
      {
        m_pDrawCore = NULL;
      }

      BattleUIActSelectLowerInputInterface( class BattleUIActSelectLowerDraw* pDrawCore )
      {
        GFL_ASSERT( pDrawCore );
        m_pDrawCore = pDrawCore;
      }

      virtual ~BattleUIActSelectLowerInputInterface() {}

      //------------------------------------------------------------------
      /**
       * @brief   カーソル移動
       */
      //------------------------------------------------------------------
      virtual bool MoveUpCursor() = 0;
      virtual bool MoveDownCursor() = 0;
      virtual bool MoveLeftCursor() = 0;
      virtual bool MoveRightCursor()  = 0;

      //------------------------------------------------------------------
      /**
       * @brief   Yボタンを押した
       */
      //------------------------------------------------------------------
      virtual void PushBtnY()  = 0;

      //------------------------------------------------------------------
      /**
       * @brief   カーソル選択している項目を押す
       */
      //------------------------------------------------------------------
      virtual void PushBtnCurrentCursorItem() = 0;

      //------------------------------------------------------------------
      /**
       * @brief   指定したカーソルを選択して項目を押す
       */
      //------------------------------------------------------------------
      virtual void PushBtnSelectCursorItem( const s32 newCursorType ) = 0;

      //------------------------------------------------------------------
      /**
       * @brief   もどるボタンを押した
       */
      //------------------------------------------------------------------
      virtual void PushBtnBack()  = 0;

   protected:
      class BattleUIActSelectLowerDraw* m_pDrawCore;
  };

  /**
  * @brief バトル下画面の行動選択画面基本クラス
  */
  class BattleUIActSelectLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIActSelectLowerDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_IDLE,
        STATE_TYPE_ENTER_ANIM_WAIT,
        STATE_TYPE_OUT,
        STATE_TYPE_END
      };

      //  画面タイプ
      enum ScreenType
      {
        SCREEN_TYPE_NORMAL  = 0,
        SCREEN_TYPE_DOUBLE,
        SCREEN_TYPE_ROYALE,
        SCREEN_TYPE_INTRUSION,

        SCREEN_TYPE_MAX
      };

      //  コマンドモード
      enum CommandMode
      {
        COMMAND_MODE_NONE  = -1,

        //  にげる用のモード
        COMMAND_MODE_RUN  = 0,
        //  もどる用のモード
        COMMAND_MODE_BACK
      };

      //  表示モード
      enum DrawMode
      {
        //  通常
        DRAW_MODE_NORMAL  = 0,

        //  通信
        DRAW_MODE_NET,

        //  バトル施設
        DRAW_MODE_BTL_FACLIITY
      };

      //  カーソル一種類
      enum CursorType
      {
        CURSOR_POS_TYPE_NONE  = -1,
        CURSOR_POS_TYPE_FIGHT = 0,
        CURSOR_POS_TYPE_POKE,
        CURSOR_POS_TYPE_BAG,
        CURSOR_POS_TYPE_RUN,

        CURSOR_POS_TYPE_MAX
      };

      //  ボタンId一覧
      enum ButtonId
      {
        BUTTON_ID_FIGHT         = 0,
        BUTTON_ID_POKE,
        BUTTON_ID_BAG,
        BUTTON_ID_RUN,
        BUTTON_ID_BACK,
        BUTTON_ID_BALL,
        BUTTON_ID_POKE_ICON_LEFT_DOWN,
        BUTTON_ID_POKE_ICON_RIGHT_DOWN,
        BUTTON_ID_POKE_ICON_LEFT_UP,
        BUTTON_ID_POKE_ICON_RIGHT_UP,

        BUTTON_ID_USER_NAME,

        BUTTON_ID_MAX
      };

      //  ポケモンアイコン数
      enum PokeIconPosType
      {
        POKE_ICON_POS_TYPE_LEFT_DOWN  = 0,
        POKE_ICON_POS_TYPE_RIGHT_DOWN,
        POKE_ICON_POS_TYPE_LEFT_UP,
        POKE_ICON_POS_TYPE_RIGHT_UP,

        POKE_ICON_POS_TYPE_MAX
      };

      //  ポケモンパラメータ
      typedef struct
      {
        s32                                             type;
        btl::BTL_POKEPARAM*                             p_btl_poke_param;
        const BattleUIDataManager::POKE_STATUS_DATA*    p_btl_poke_status;

      } POKE_DATA;

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        //  描画用ヒープ
        ::app::util::Heap* pUtilHeap;

        ::app::util::AppRenderingManager* pRenderingManager;

        BattleUIActSelectLowerDrawDelegate* pDelegate;
        void* pLytBuff;
        void* pMenuCursorLytBuff;

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData*     pBtlAppMsgData;
        print::WordSet*         pBtlWordSet;

        POKE_DATA aPokeData[ POKE_ICON_POS_TYPE_MAX ];

        s32 selectItemIdx;

        //  画面タイプ
        ScreenType  screenType;

        //  描画モード
        DrawMode    drawMode;

        //  味方のマルチモードかどうか( drawMode == DRAW_MODE_NET のみパターンで使う )
        bool        bFriendMuliti;

        //  通信対戦相手名( drawMode == DRAW_MODE_NET のみパターンで使う )
        const gfl2::str::StrBuf*    pEnemyPlayerName;
        //  通信対戦相手の性別;
        print::GrammaticalAttribute enemyPlayerGrammer;

        bool        bUseInputSE;

        _tag_init_param()
        {
          pBtlAppMsgData      = NULL;
          pBtlWordSet         = NULL;
          pUtilHeap           = NULL;
          pRenderingManager   = NULL;
          pMenuCursorLytBuff  = NULL;
          pDelegate           = NULL;
          pLytBuff            = NULL;
          pEnemyPlayerName    = NULL;
          selectItemIdx       = CURSOR_POS_TYPE_FIGHT;
          screenType          = SCREEN_TYPE_NORMAL;
          drawMode            = DRAW_MODE_NORMAL;
          bUseInputSE         = false;
          bFriendMuliti       = false;

          enemyPlayerGrammer.Raw  = 0;
          gfl2::std::MemFill( aPokeData, NULL, sizeof( aPokeData ) );
        }

      } INIT_PARAM;

      //  ポケモンオブジェクト
      class PokeObj
      {
      public:
        //  ポケモンアイコンアニメ
        enum PokeIconAnime
        {
          POKE_ICON_ANIME_DIE = 0,
          POKE_ICON_ANIME_IN,
          POKE_ICON_ANIME_DIR_LEFT,
          POKE_ICON_ANIME_DIR_RIGHT,
          POKE_ICON_ANIME_MAX
        };

        //  アイコンの向き
        enum IconDirType
        {
          ICON_DIR_TYPE_LEFT  = 0,
          ICON_DIR_TYPE_RIGHT,
        };

        typedef struct
        {
          IconDirType                 dir_type;
          gfl2::lyt::LytParts*        p_icon_parts_pane;
          gfl2::lyt::LytPane*         p_info_icon_pane;
          gfl2::lyt::LytBounding*     p_touch_hit_pane;

          u32                     a_icon_anime_id[ POKE_ICON_ANIME_MAX ];
        } INIT_DATA;

        PokeObj()
        {
          m_pRootView         = NULL;
          m_pIcon             = NULL;
          m_playIconAnimeIdx  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_bChangeStatus     = false;

          gfl2::std::MemFill( &m_initData, NULL, sizeof( m_initData ));
          gfl2::std::MemFill( &m_pokeData, NULL, sizeof( m_pokeData ));

          {
            m_buttonInfo.button_id  = BUTTON_ID_MAX;
            m_buttonInfo.picture_pane = NULL;
            m_buttonInfo.bound_pane   = NULL;
            m_buttonInfo.touch_anime_index        = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_buttonInfo.release_anime_index      = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;     
            m_buttonInfo.cancel_anime_index       = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
            m_buttonInfo.key_select_anime_index   = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;  
            m_buttonInfo.active_anime_index       = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
            m_buttonInfo.passive_anime_index      = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          }
        }

        ~PokeObj()
        {
          GFL_SAFE_DELETE( m_pIcon );
        }

        //  初期化
        void Initialize( 
          const INIT_DATA& rInitData, 
          const ::app::ui::ButtonInfoEx& rButtonInfo, 
          ::app::ui::UIView* pRootView, 
          u32 lytId, 
          const POKE_DATA& rPokeData, 
          ::app::util::Heap* pUtilHeap );

        //  終了処理
        void Terminate();

        // 破棄可能か
        bool IsDelete();

        //  インフォアイコン表示設定
        void SetIconVisible( const bool bFlg );

        //  アイコン用アニメ再生(再生したidxを保持)
        void StartIconAnime( const PokeIconAnime animeIdx, const bool bForcePlayAnime );

        //  汎用アニメ再生(単純再生)
        void StartAnime( const PokeIconAnime animeIdx );

        //  アルファ設定
        void SetAlpha( const u32 alpha );

        //  アイコン向き指定
        void SetDirType( const IconDirType dir );

        //  更新
        void Update();

        //  表示できるかどうか
        bool IsPut() { return ( m_pIcon != NULL ); }

        //  状態変化しているか
        bool IsChangeStatus() const { return m_bChangeStatus; }

        //  くっついているポケモンデータを取得
        const POKE_DATA& GetPokeData() const { return m_pokeData; }

        //  ボタンinfoデータ種痘
        const ::app::ui::ButtonInfoEx& GetButtonInfoData() const { return m_buttonInfo; }

      private:
        ::app::ui::UIView*                m_pRootView;
        ::app::tool::PokeIconToolEx*      m_pIcon;
        ::app::ui::ButtonInfoEx           m_buttonInfo;
        
        u32                               m_playIconAnimeIdx;
        INIT_DATA                         m_initData;
        POKE_DATA                         m_pokeData;

        bool                              m_bChangeStatus;
      };

      //------------------------------------------------------------------
      /**
       * @brief   コンストラクタ
       * @note
       *    あらかじめ使う側がレイアウトデータを作成して渡す。
       */
      //------------------------------------------------------------------
      BattleUIActSelectLowerDraw( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //------------------------------------------------------------------
      virtual ~BattleUIActSelectLowerDraw();

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
        u32 loopNum = GFL_NELEMS( m_aPokeObjData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_aPokeObjData[ i ].Terminate();
        }
      }

      //------------------------------------------------------------------
      /**
       * @brief   破棄可能か
       */
      //------------------------------------------------------------------
      virtual bool IsDelete();

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
       * @brief   ボールアイテムid設定
       * @param   itemId : ボールのアイテムidを指定 / Savedata::MyItem::ITEM_NONE ならボール項目は非表示に
       * @param   bPassive : 状態をパッシブにするかどうか
       */
      //------------------------------------------------------------------
      void SetBallItemId( const u32 itemId, const bool bPassive );

      //------------------------------------------------------------------
      /**
       * @brief   ボール使用ショートカットが使えるか
       * @return  使える true / 使えない false
       */
      //------------------------------------------------------------------
      bool IsBtnUseBallShortCut() const { return m_bBtnBallShortCut; }

      //------------------------------------------------------------------
      /**
       * @brief   コマンドモード設定
       * @param   newMode     : モード指定
       * @param   bEventAnime : モード切替イベントアニメを実行するか 
       */
      //------------------------------------------------------------------
      void SetCommandMode( const CommandMode newMode, const bool bEventAnime );

      //  コマンドモード取得
      CommandMode GetCoomandMode() const { return m_nowCommandMode; }

      //------------------------------------------------------------------
      /**
       * @brief   現在の選択しているカーソル位置
       */
      //------------------------------------------------------------------
      s32 GetCurrentCursorIdx() const { return m_initParam.selectItemIdx; }

      //------------------------------------------------------------------
      /**
       * @brief   現在選択しているカーソルの前のカーソル位置
       */
      //------------------------------------------------------------------
      s32 GetOldCursorIdx() const { return m_oldCursor; }

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

      //------------------------------------------------------------------
      /**
       * @brief   現在の描画モード
       */
      //------------------------------------------------------------------
      DrawMode GetDrawMode() const { return m_initParam.drawMode; }
      
      //------------------------------------------------------------------
      /**
       * @brief   ポケモンオブジェクト取得
       */
      //------------------------------------------------------------------
      PokeObj* GetPokeObj( const PokeIconPosType pokeIconPosType )
      {
        return &m_aPokeObjData[ pokeIconPosType ];
      }

      //------------------------------------------------------------------
      /**
       * @brief   選択項目のカーソルactive設定
       * @return  true -> カーソル位置を変更 / false -> カーソル位置を変更していない
       */
      //------------------------------------------------------------------
      bool SetActiveSelectItemCursor( const bool bActive, const s32 selectItemType );

      //------------------------------------------------------------------
      /**
       * @brief   カーソル選択している項目を押す
       */
      //------------------------------------------------------------------
      virtual void PushBtnCurrentCursorItem();

      //------------------------------------------------------------------
      /**
       * @brief   指定したカーソルを選択して項目を押す
       */
      //------------------------------------------------------------------
      virtual void PushBtnSelectCursorItem( const CursorType newCursorType );

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
        LYT_WK_TYPE_MAX
      };

      enum
      {
        PLYAER_NAME_TEXT_PANE_MAX = 3
      };

      //  ボールオブジェクト
      class BallObj
      {
      public:
        typedef struct _tag_init_
        {
          gfl2::heap::HeapBase*   pSysHeap;
          gfl2::heap::HeapBase*   pDevHeap;
          ::app::util::G2DUtil*   pG2DUtil;

          u32                     itemId;
          gfl2::lyt::LytPicture*  pPanePicture;

          _tag_init_()
          {
            pSysHeap      = NULL;
            pDevHeap      = NULL;
            itemId        = 0xffffffff;
            pPanePicture  = NULL;
            pG2DUtil      = NULL;
          }

        } INIT_DATA;

        BallObj()
        {
          m_pItemIcon = NULL;
        }

        BallObj( const INIT_DATA& rInitData )
        {
          m_initData  = rInitData;
          m_pItemIcon = GFL_NEW( rInitData.pDevHeap ) ::app::tool::ItemIcon( rInitData.pSysHeap, rInitData.pDevHeap, 1 );

          m_iconOrgColor  = m_initData.pG2DUtil->GetMaterialColor( m_initData.pPanePicture, nw::lyt::INTERPOLATECOLOR_WHITE );

          _SetItemId( rInitData.itemId );
        }

        ~BallObj()
        {
          GFL_SAFE_DELETE( m_pItemIcon );
        }

        //  アイテムid設定
        //  保持しているidと異なる場合のみ処理
        //  内部でファイル開閉とロードしているので処理が重くなるので、すでに設定しているのを何度もやるのは無駄
        void SetItemId( const u32 itemId )
        {
          if( m_initData.itemId != itemId )
          {
            _SetItemId( itemId );
          }
        }

        //  アイコンのパッシブ設定
        //  パッシブだとアイコンの色が変わる
        void SetPassive( const bool bFlg )
        {
          if( bFlg )
          {
            //  灰色に
            m_initData.pG2DUtil->SetMaterialColor( m_initData.pPanePicture, nw::lyt::INTERPOLATECOLOR_WHITE, gfl2::lyt::ColorU8( m_iconOrgColor.r, m_iconOrgColor.g, m_iconOrgColor.b, 150 ) );
          }
          else
          {
            m_initData.pG2DUtil->SetMaterialColor( m_initData.pPanePicture, nw::lyt::INTERPOLATECOLOR_WHITE, m_iconOrgColor );
          }
        }

      private:
        //  アイテムid設定
        //  id値の変更チェックなし、初回処理にはチェックは不要なので、チェックなしようのを用意
        void _SetItemId( const u32 itemId )
        {
          m_initData.itemId = itemId;
          m_pItemIcon->ReplacePaneTexture( 0, itemId, m_initData.pPanePicture );
        }

        ::app::tool::ItemIcon*    m_pItemIcon;
        INIT_DATA                 m_initData;

        gfl2::lyt::ColorU8        m_iconOrgColor;

      } BALL_OBJ_DATA;

      //  カーソル設定データ
      typedef struct
      {
        s32 pos_id;
        s32 button_id;

      } CURSOR_CONFIG_DATA;

      //  ボタン作成データ
      typedef struct
      {
        u32 touch_anime_index;
        u32 release_anime_index;
        u32 cancel_anime_index;
        u32 key_select_anime_index;
        u32 active_anime_index;
        u32 passive_anime_index;

        u32 selectLoopAnimeId;

        u32 buttonPartPaneId;
        u32 partsBoundPaneId;

      } BUTTON_CREATE_DATA;

      //  カーソル項目データ
      struct CUROSR_ITEM_DATA
      {
        CUROSR_ITEM_DATA()
        {
          pNullCursorPane   = NULL;
          pNullGlarePane    = NULL;
          selectLoopAnimeId = ::app::tool::ButtonManager::ANIMATION_NULL;
        }

        gfl2::lyt::LytPane* pNullCursorPane;
        gfl2::lyt::LytPane* pNullGlarePane;
        u32 selectLoopAnimeId;
      };

      //------------------------------------------------------------------
      /**
       * @brief   画面タイプ設定
       */
      //------------------------------------------------------------------
      void _SetScreenType( const ScreenType newType );

      //------------------------------------------------------------------
      /**
       * @brief   表示モード設定
       */
      //------------------------------------------------------------------
      void _SetDrawMode( const DrawMode newMode );

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
       * @brief   ポケモンデータ作成
       */
      //------------------------------------------------------------------
      void _CreatePokeData( const s32 pokeDataIdx, const POKE_DATA& rPokeData, gfl2::lyt::LytMultiResID* pLytMultiResID, const ScreenType screenType );

      void _Clear()
      {
        gfl2::std::MemFill( m_pBtnPartRootPaneList, NULL, sizeof( m_pBtnPartRootPaneList ) );
        gfl2::std::MemClear( m_pPlayerNameTextBoxPaneArray, sizeof( m_pPlayerNameTextBoxPaneArray ) );

        m_state                 = STATE_TYPE_NONE;
        m_pMenuCursor           = NULL;
        m_pBallObj              = NULL;
        m_nowCommandMode        = COMMAND_MODE_NONE;
        m_bBtnBallShortCut      = false;
        m_oldCursor             = 0;
        
        {
          u32 loopNum = GFL_NELEMS( m_aCreateButtonDataTable );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_aCreateButtonDataTable[ i ].touch_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].release_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].cancel_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].key_select_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].active_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].passive_anime_index  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].selectLoopAnimeId  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].buttonPartPaneId = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
            m_aCreateButtonDataTable[ i ].partsBoundPaneId = gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
          }
        }
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();

      //  プレイヤー名のテキスト設定
      void _SetPlayerNameText( const gfl2::str::StrBuf* pPlayerNameStrBuf, const print::GrammaticalAttribute grammer );

      //  カーソルタイプ値からボタンid値を変換して取得
      s32 _ConvCursorTypeToButtonId( const s32 cursorType );

      //  ボタンid値からカーソルタイプ値を変換して取得
      s32 _ConvButtonIdToCursorType( const s32 buttonId );

      //  変数宣言一覧

      //  カーソル設定データテーブル
      static const CURSOR_CONFIG_DATA m_saCursorConfigDataTable[ CURSOR_POS_TYPE_MAX ];

      //  ボタン作成元データテーブル
      static const BUTTON_CREATE_DATA m_saCreateButtonOrgDataTable[ BUTTON_ID_MAX ];

      //  ボタン作成データテーブル（画面のモードによってデータが可変するので元のデータから調整したデータ変数を使う）
      BUTTON_CREATE_DATA                    m_aCreateButtonDataTable[ BUTTON_ID_MAX ];

      INIT_PARAM                            m_initParam;

      StateType                             m_state;

      CommandMode                           m_nowCommandMode;

      s32                                   m_oldCursor;

      ::app::tool::MenuCursor*              m_pMenuCursor;
      CUROSR_ITEM_DATA                      m_aMenuCursorItemData[ CURSOR_POS_TYPE_MAX ];

      PokeObj                               m_aPokeObjData[ POKE_ICON_POS_TYPE_MAX ];

      BallObj*                              m_pBallObj;

      //  ボール使用ショートカットが使えるか
      bool                                  m_bBtnBallShortCut;

      //  プレイヤー名テキストペイン一覧
      gfl2::lyt::LytTextBox*                m_pPlayerNameTextBoxPaneArray[ PLYAER_NAME_TEXT_PANE_MAX ];

      //  画面ボタンルートペイン一覧
      gfl2::lyt::LytParts* m_pBtnPartRootPaneList[ BUTTON_ID_MAX ];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_ACT_SELECT_LOWER_DRAW_H__ 
