//=============================================================================
/**
 * @file BattleUITargetSelectLowerDraw.h
 * @brief バトル下画面 >> 「わざ対象選択」画面
 * @author yuto_uchida
 * @date 2015.03.31
 */
//=============================================================================
#if !defined( __BATTLE_UI_TARGET_SELECT_LOWER_DRAW_H__ )
#define __BATTLE_UI_TARGET_SELECT_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

#include "Battle/include/battle_SetupParam.h"
#include "Battle/source/btl_common.h"
#include "Battle/source/btl_pokeparam.h"

#include "Battle/source/btlv/app/parts/BattleUIKoukaIconPartsLayout.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面の技選択画面の通知クラス */
  class BattleUITargetSelectLowerDrawDelegate
  {
  public:
    BattleUITargetSelectLowerDrawDelegate() {}
    virtual ~BattleUITargetSelectLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUITargetSelectLowerDraw* pDrawInst, s32 buttonId )  = 0;

    /// キー入力通知用リスナ
    virtual bool OnKeyAction( class BattleUITargetSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /** バトル下画面のポケモン対象選択画面 */
  class BattleUITargetSelectLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUITargetSelectLowerDraw );  // コピーを禁止

    public:
      //  描画モード
      enum DrawMode
      {
        DRAW_MODE_NORMAL  = 0,
        DRAW_MODE_ROYAL,
      };

      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_UPDATE,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      //  配置されているボタン一覧
      enum ButtonId
      {
        BUTTON_ID_BACK  = 0,
        BUTTON_ID_LEFT_UP,
        BUTTON_ID_RIGHT_UP,
        BUTTON_ID_LEFT_DOWN,
        BUTTON_ID_RIGHT_DOWN,

        BUTTON_ID_MAX,
      };

      //  選択項目の位置
      enum SelectItemPos
      {
        SELECT_ITEM_POS_NONE      = -1,

        SELECT_ITEM_POS_LEFT_DOWN = 0,
        SELECT_ITEM_POS_RIGHT_DOWN,
        SELECT_ITEM_POS_LEFT_UP,
        SELECT_ITEM_POS_RIGHT_UP,

        SELECT_ITEM_POS_MAX,
      };

      enum SelectItemStatus
      {
        SELECT_ITEM_STATUS_NONE = -1,

        //  味方で技を使っている
        SELECT_ITEM_STATUS_SELF_USE_WAZA = 0,
        //  味方
        SELECT_ITEM_STATUS_SELF,
        //  敵
        SELECT_ITEM_STATUS_ENEMY,
      };

      //  選択項目データリスト
      typedef struct _tag_select_item_param
      {
        //  プレイヤー名(描画モードが DRWA_MODE_ROYAL 時のみ値が入る)
        gfl2::str::STRCODE                  aPlayerName[ System::STRLEN_PLAYER_NAME + System::EOM_LEN ];
        //  プレイヤー名の文法(描画モードが DRWA_MODE_ROYAL 時のみ値が入る)
        print::GrammaticalAttribute         playerNameAtribute;

        //  NPC名かどうか(描画モードが DRWA_MODE_ROYAL 時のみ値が入る)
        //  true だと playerNameAtribute変数を使って名前表示する
        bool                                bNPCName;

        //  コーナー色（ロイヤル専用）
        CornerColor                         cornerColor;

        //  項目のステータス
        SelectItemStatus                    status;

        //  ポケモンデータ
        const btl::BTL_POKEPARAM*           pBtlPokeParam;

        //  選択許可
        bool  bSelectPermission;

        //  ターゲットの位置（単体・複数だと値が異なる、この値をコマンドに返す）
        s32 btlvTargetPos;

        //  効果タイプ
        BtlTypeAffAbout koukaType;
        //  効果タイプ表示
        bool  bKoukaTypePutFlg;

        _tag_select_item_param()
        {
          gfl2::std::MemClear( aPlayerName, sizeof( aPlayerName ) );

          cornerColor       = CORRER_COLOR_BLUE;
          status            = SELECT_ITEM_STATUS_NONE;
          pBtlPokeParam     = NULL;
          bNPCName          = false;
          bSelectPermission = false;
          bKoukaTypePutFlg  = false;
          btlvTargetPos     = BTL_VPOS_ERROR;
          koukaType         = pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
        }

      } SELECT_ITEM_PARAM;

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                      pUtilHeap;
        ::app::util::AppRenderingManager*       pRenderingManager;

        //  デリゲーター
        BattleUITargetSelectLowerDrawDelegate*  pDelegate;

        //  描画モード
        DrawMode            drawMode;

        //  入力関連のSEを使うか
        bool                bUseInputSE;

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData* pBtlAppMsgData;
        print::WordSet*     pBtlWordSet;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        //  メニューカーソルのレイアウトバイナリ
        void* pMenuCursorLytBuff;

        //  最初に選択するカーソル項目
        SelectItemPos cursorItemPos;

        //  選択項目データリスト
        SELECT_ITEM_PARAM aSelectItem[ SELECT_ITEM_POS_MAX ];

        //  連結している項目の値( 設定する時は配列の先頭から順に値が代入するようにしないとバグる )
        SelectItemPos aLinkItemPos[ SELECT_ITEM_POS_MAX ];

        _tag_init_param()
        {
          drawMode              = DRAW_MODE_NORMAL;
          pUtilHeap             = NULL;
          pRenderingManager     = NULL;
          pLytBuff              = NULL;
          pBtlAppMsgData        = NULL;
          pBtlWordSet           = NULL;
          pMenuCursorLytBuff    = NULL;
          pDelegate             = NULL;
          cursorItemPos         = SELECT_ITEM_POS_LEFT_UP;
          bUseInputSE           = false;

          ::std::memset( aLinkItemPos, SELECT_ITEM_POS_NONE, sizeof( aLinkItemPos ) );
        }

      } INIT_PARAM;

      //------------------------------------------------------------------
      /**
       * @brief   コンストラクタ
       */
      //------------------------------------------------------------------
      BattleUITargetSelectLowerDraw( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //------------------------------------------------------------------
      virtual ~BattleUITargetSelectLowerDraw();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      void Initialize();

      //------------------------------------------------------------------
      /**
       * @brief   終了
       */
      //------------------------------------------------------------------
      void Terminate();

      //------------------------------------------------------------------
      /**
       * @brief   使える状態か
       */
      //------------------------------------------------------------------
      bool IsReady();

      //------------------------------------------------------------------
      /**
       * @brief   削除可能か
       */
      //------------------------------------------------------------------
      bool IsDelete();

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
       * @brief   バックボタンを押す
       */
      //------------------------------------------------------------------
      void PushBtnBack();

      //------------------------------------------------------------------
      /**
       * @brief   カレントカーソル項目を決定
       */
      //------------------------------------------------------------------
      void PushBtnCurrentCursor();

      //------------------------------------------------------------------
      /**
       * @brief   カーソル項目を指定して決定
       */
      //------------------------------------------------------------------
      void PushBtnSelectCursor( const SelectItemPos itemPos );

      //------------------------------------------------------------------
      /**
       * @brief   カーソル移動
       */
      //------------------------------------------------------------------
      bool MoveCursorUp();
      bool MoveCursorDown();
      bool MoveCursorLeft();
      bool MoveCursorRight();

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

      //  カーソル位置
      SelectItemPos GetCurrentCursorPos() const { return  m_currentItemPos; }

      //  設定した初期化パラメータ取得
      const INIT_PARAM& GetInitParam() const { return m_initParam; }

      // ペイン選択アニメーションの開始を検知
      virtual void OnLayoutPaneStartingAnimeEvent( const u32 buttonId );

      // ペインタッチ時を検知
      virtual ListenerResult OnLayoutPaneEvent( const u32 buttonId );

      // ペインタッチ時を検知
      // 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
      virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

      /// キー入力通知用リスナ
      virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      /** 選択項目のオブジェクトクラス */
      class SelectItemObject
      {
      public:
        typedef struct
        {
          SELECT_ITEM_PARAM   select_item_data;

          s32                 root_parts_pane_id;       //  レイアウトのパーツペインid;
          
          s32                 player_text_pane_id;      //  プレイヤー名のテキストペインid(ロイヤル専用)

          u32                 male_sex_anime_idx;       //  性別アニメ(男性)
          u32                 female_sex_anime_idx;     //  性別アニメ(女性)

          u32                 friend_color_anime_idx;
          u32                 enemy_color_anime_idx;

          u32                 friend_non_select_color_anime_idx;
          u32                 enemy_non_select_color_anime_idx;

          //  カーソルが配置されている時のアニメ
          u32                 friend_select_anime_idx;
          u32                 enemy_select_anime_idx;

          //  ボタンアニメidx一覧
          struct _tag_btn_anime_data
          {
            u32                 touch_anime_index;        //!< タッチ時のアニメ
            u32                 release_anime_index;      //!< タッチ決定時のアニメ
            u32                 cancel_anime_index;       //!< タッチ状態からのキャンセルアニメ
            u32                 key_select_anime_index;   //!< キー決定時のアニメ ( touch => release の連続アニメ )
            u32                 active_anime_index;       //!< アクティブアニメ
            u32                 passive_anime_index;      //!< パッシブアニメ
          } btn_anime_data;

          //  効果アイコンのアニメ一覧
          struct _tag_kouka_icon_anime_data
          {
            //  有効化アニメidx(アニメ名「active」)
            s32 active_anime_idx;
            //  無効化化アニメidx(アニメ名「passive」)
            s32 passive_anime_idx;
            //  効果なしアニメidx(アニメ名「kouka_02」)
            s32 kouka_none_anime_idx;
            //  効果いまひとつアニメidx(アニメ名「kouka_01」)
            s32 kouka_imahitotsu_anime_idx;
            //  効果ばつぐんアニメidx(アニメ名「kouka_00」)
            s32 kouka_batsugun_anime_idx;
            //  効果無記名アニメ名(アニメ名「kouka_03」)
            s32 kouka_normal_anime_idx;

          } kouka_icon_anime_data;

          //  グローブマークの色指定アニメidx(ロイヤル専用)
          struct _tag_grobe_anime_data
          {
            s32 globe_blue_anime_idx;
            s32 globe_yeloow_anime_idx;
            s32 globe_green_anime_idx;
            s32 globe_red_anime_idx;
          } grobe_anime_data;

        } INIT_SELECT_ITEM_PARAM;

        SelectItemObject();

        virtual ~SelectItemObject();

        //  初期化
        void  Initialize( const INIT_SELECT_ITEM_PARAM& rInitParam, UIView* pParentView, const s32 lytId, const s32 resId, ::app::util::Heap* pUseHeapUtil, void* pMenuCursorResBuf );

        //  終了通知
        void  Terminate();

        //  使用準備OKか
        bool  IsReady();

        /** 削除可能か */
        bool  IsDelete();

        void  Update();

        void  Draw( ::app::util::AppRenderingManager* pRenderManager, gfl2::gfx::CtrDisplayNo displayNo, u8 order );

        //  ボタンExデータを作成
        const ::app::ui::ButtonInfoEx CreateButtonInfoEx( const ButtonId buttonId );

        //  ボタンid取得
        ::app::tool::ButtonManager::ButtonId GetButtonId() const { return m_buttonInfoExData.button_id; }

        //  有効設定
        void SetEnable( const bool bFlg );

        //  カーソル表示設定
        void SetVisibleCursor( const bool bFlg, const s32 visibieType );

        //  カーソル表示しているか
        bool IsVisibleCursor();

        //  ボタンを押す
        void PushBtn();

      private:
        //  更新内のステート
        enum UpdateSubState
        {
          UPDATE_SUB_STATE_CMD_SELECT = 0,
          UPDATE_SUB_STATE_STATUS,
        };

        void  _UpdateKoukaItem();

        void  _Clear()
        {
          m_pPokeIcon             = NULL;
          m_pParentView           = NULL;
          m_pPartsPane            = NULL;
          m_pCursorNullPane       = NULL;
          m_lytId                 = -1;
          m_pKoukaIconParts       = NULL;
          m_pMenuCursor           = NULL;
          m_bSelectAnime          = false;

          m_buttonInfoExData.button_id     = BUTTON_ID_MAX;
          m_buttonInfoExData.picture_pane  = NULL;
          m_buttonInfoExData.bound_pane    = NULL;
          m_buttonInfoExData.touch_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_buttonInfoExData.release_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;     
          m_buttonInfoExData.cancel_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
          m_buttonInfoExData.key_select_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;  
          m_buttonInfoExData.active_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
          m_buttonInfoExData.passive_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;     
        }

        bool                                  m_bSelectAnime;
        INIT_SELECT_ITEM_PARAM                m_initParam;

        ::app::tool::PokeIconToolEx*          m_pPokeIcon;
        UIView*                               m_pParentView;
        s32                                   m_lytId;
        ::app::ui::ButtonInfoEx               m_buttonInfoExData;

        gfl2::lyt::LytParts*                  m_pPartsPane;
        gfl2::lyt::LytPane*                   m_pCursorNullPane;

        ::app::tool::MenuCursor*              m_pMenuCursor;

        parts::BattleUIKoukaIconPartsLayout*  m_pKoukaIconParts;
      };

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //  カーソル移動
      bool _MoveCursor( const s32 aMoveDataTable[][ 2 ], const SelectItemPos currentCursorPos, const s32 rowNum );

      //  カーソル設定
      bool _SetCursor( const SelectItemPos itemPos );

      //  ボタンidから選択項目の座標タイプを変換
      SelectItemPos ConvButtonIdToSelectItemPos( const ButtonId buttonId );

      void _Clear()
      {
        m_state     = STATE_TYPE_NONE;
        m_currentItemPos  = SELECT_ITEM_POS_NONE;

        gfl2::std::MemFill( m_pSelectItemObjectArray, NULL, sizeof( m_pSelectItemObjectArray ) );
      }

      //  変数宣言一覧
      INIT_PARAM        m_initParam;
      StateType         m_state;
      SelectItemPos     m_currentItemPos;

      SelectItemObject* m_pSelectItemObjectArray[ SELECT_ITEM_POS_MAX ];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_TARGET_SELECT_LOWER_DRAW_H__ 
