//=============================================================================
/**
 * @file ConfigLowerUIDraw.h
 * @brief コンフィグの下UI画面
 * @author yuto_uchida
 * @date 2015.05.19
 */
//=============================================================================
#if !defined( NIJI_PROJECT_CONFIG_LOWER_UI_DRAW_H_INCLUDED )
#define NIJI_PROJECT_CONFIG_LOWER_UI_DRAW_H_INCLUDED
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

#include "App/config/source/parts/ConfigLowerUIListItemObj.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  /** コンフィグ下UI画面 */
  class ConfigLowerUIDraw : public ::app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN( ConfigLowerUIDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_UPDATE,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      //  ボタンid一覧
      enum ButtonId
      {
        BUTTON_ID_LEFT_ARROW_ITEM_00,
        BUTTON_ID_RIGHT_ARROW_ITEM_00,

        BUTTON_ID_LEFT_ARROW_ITEM_01,
        BUTTON_ID_RIGHT_ARROW_ITEM_01,

        BUTTON_ID_LEFT_ARROW_ITEM_02,
        BUTTON_ID_RIGHT_ARROW_ITEM_02,

        BUTTON_ID_LEFT_ARROW_ITEM_03,
        BUTTON_ID_RIGHT_ARROW_ITEM_03,

        BUTTON_ID_LEFT_ARROW_ITEM_04,
        BUTTON_ID_RIGHT_ARROW_ITEM_04,

        BUTTON_ID_LEFT_ARROW_ITEM_05,
        BUTTON_ID_RIGHT_ARROW_ITEM_05,

        //  矢印のボタンと重なっているのでタッチ判定を矢印ボタンを優先にして、項目ボタンの判定を後にするために定義順で優先度を変えた
        //  ※ボタン管理クラス側で判定の優先度設定ができない
        BUTTON_ID_LIST_ITEM_00,
        BUTTON_ID_LIST_ITEM_01,
        BUTTON_ID_LIST_ITEM_02,
        BUTTON_ID_LIST_ITEM_03,
        BUTTON_ID_LIST_ITEM_04,
        BUTTON_ID_LIST_ITEM_05,

        BUTTON_ID_BACK,
        BUTTON_ID_ENTER,

        BUTTON_ID_MAX
      };

      //  設定タイプ
      enum ConfigType
      {
        CONFIG_TYPE_TALK_SPEED  = 0,
        CONFIG_TYPE_TEXT_MODE,
        CONFIG_TYPE_BATTLE_ANIM_SHOW,
        CONFIG_TYPE_BATTLE_RULE,
        CONFIG_TYPE_BUTTOM_MODE,
        CONFIG_TYPE_BOX_STATUS,

        CONFIG_TYPE_MAX,
      };

      //  モード
      enum DrawMode
      {
        DRAW_MODE_SELECT_LIST  = 0,
        //  データ切り替えメッセージモード
        DRAW_MODE_MESSAGE_CHANGE_DATA
      };

      //  項目のプッシュボタン
      enum PushButtonListItem
      {
        PUSH_BUTTON_LIST_ITEM_LEFT_ARROW  = 0,
        PUSH_BUTTON_LIST_ITEM_RIGHT_ARROW,

        PUSH_BUTTON_LIST_ITEM_MAX
      };

      enum
      {
        SELECT_LIST_MAX = 6,
      };

      //  下画面リスト項目作成データ
      typedef struct
      {
        u32 title_text_id;
        u32 help_msg_id;

        u32 left_arrow_button_id;
        u32 right_arrow_button_id;

        u32 a_select_text_id[ 3 ];

      } LIST_ITEM_CREATE_DATA;

      //  リスト項目データ
      typedef struct _tag_list_item_param
      {
        ConfigType type;

        u32 helpMsgId;

        //  選択idx
        u32 firstSelectIdx;

        ConfigLowerUIListItemObj::DATA_PARAM  itemData;

        _tag_list_item_param()
        {
          type            = CONFIG_TYPE_MAX;
          firstSelectIdx  = 0;
          helpMsgId       = 0;

          gfl2::std::MemClear( &itemData, sizeof( itemData ) );
        };

      } LIST_ITEM_PARAM;

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::AppRenderingManager* pRenderingManager;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void*               pLytBuff;
        void*               pMenuCursorLytDataBuffer;

        gfl2::str::MsgData* pMsgData;
        print::WordSet*     pWordSet;

        LIST_ITEM_PARAM     aListItemParam[ SELECT_LIST_MAX ];
        u32                 listMax;

        _tag_init_param()
        {
          pRenderingManager         = NULL;
          pLytBuff                  = NULL;
          pMenuCursorLytDataBuffer  = NULL;
          pMsgData                  = NULL;
          pWordSet                  = NULL;
          listMax                   = 0;
        }

      } INIT_PARAM;

      //  各リスト作成データを取得
      static const LIST_ITEM_CREATE_DATA&  GetCreateListItemData( const ConfigType type );

      //  各リスト作成データの個数
      static u32 GetCreateListItemDataNum();

      //  テキストエラーid
      static u32 GetTextErrorId();

      /**
      * @brief コンストラクタ
      */
      ConfigLowerUIDraw( app::util::Heap* pUtilHeap );

      /**
      * @brief デストラクタ
      */
      virtual ~ConfigLowerUIDraw();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      void Initialize( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   終了処理
       */
      //------------------------------------------------------------------
      void Terminate();

      //------------------------------------------------------------------
      /**
       * @brief   クラス破棄可能か
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
      virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

      //------------------------------------------------------------------
      /**
       * @brief   現在のステートを取得
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

      //------------------------------------------------------------------
      /**
       * @brief   初期設定パラメータ取得
       */
      //------------------------------------------------------------------
      const INIT_PARAM& GetInitParam() const { return m_initParam; }

      //------------------------------------------------------------------
      /**
       * @brief   指定したリスト項目のデータ位置を変える
       * @param   activeListItemIdx : 設定対象のリストアイテムidx
       * @param   addMoveDataNum    : データ移動数
       * @return  変更した場合 -> 変更したデータ結果のアドレス / 変更をなにもしていない場合 -> NULL
       */
      //------------------------------------------------------------------
      const LIST_ITEM_PARAM* MoveListItemSelectDataPos( u32 activeListItemIdx, s32 addMoveDataNum );

      //------------------------------------------------------------------
      /**
       * @brief   指定したリスト項目の押しアニメ再生
       * @param   idx       : リストidx
       * @param   pushType  : 押すアニメ種類
       */
      //------------------------------------------------------------------
      void  PlayPushAnimeListItem( const u32 idx, const PushButtonListItem pushType );

      //------------------------------------------------------------------
      /**
       * @brief   指定ボタンを押す
       * @param   id : 押すボタンidを指定
       * @return  ボタンを押した -> true / ボタンを押せなかった -> false
       */
      //------------------------------------------------------------------
      bool PushButton( const ButtonId id );

      //------------------------------------------------------------------
      /**
       * @brief   カレントのリスト項目のボタンプッシュ
       * @param   pushId : 項目にボタン押しid
       * @return  押すのに成功 -> true / 押すのに失敗 -> false
       */
      //------------------------------------------------------------------
      bool PushButtonCurrentListItem( const PushButtonListItem pushId );

      //------------------------------------------------------------------
      /**
       * @brief   ボタンidからリスト項目のカレント設定
       * @param   id : ボタンid
       * @return  変更成功 -> true / 変更失敗 -> false
       */
      //------------------------------------------------------------------
      bool SetListItemCurrentPosFormButtonId( const ButtonId id );

      //------------------------------------------------------------------
      /**
       * @brief   現在のリスト項目のカレント位置を指定
       * @param   pos : カレント位置指定
       * @return  設定したカレント位置を返す / 設定失敗の場合は -1
       */
      //------------------------------------------------------------------
      s32 SetListItemCurrentPos( const s32 pos );

      //------------------------------------------------------------------
      /**
       * @brief   現在のリスト項目のカレント位置を返す
       */
      //------------------------------------------------------------------
      s32 GetListItemCurrentPos() const { return m_nowListItemCurrentPos; }

      //------------------------------------------------------------------
      /**
       * @brief   現在のリスト項目のカレントデータを取得
       */
      //------------------------------------------------------------------
      const LIST_ITEM_PARAM* GetListItemCurrentData() const;

      //------------------------------------------------------------------
      /**
       * @brief   現在のリスト項目データを取得
       * @parma   idx : リストidx
       */
      //------------------------------------------------------------------
      const LIST_ITEM_PARAM* GetListItemData( const u32 idx ) const;

      //------------------------------------------------------------------
      /**
       * @brief   テキスト更新
       */
      //------------------------------------------------------------------
      void UpdateText( gfl2::str::MsgData* pMsgData );

      //------------------------------------------------------------------
      /**
       * @brief   モード設定
       */
      //------------------------------------------------------------------
      void SetDrawMode( const DrawMode nowMode, const bool bInput );

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      typedef struct
      {
        app::ui::ButtonInfoEx infoEx;
        s32                   seId;

      } BUTTON_CREATE_DATA;

      void _Clear()
      {
        m_pUtilHeap               = NULL;
        m_pMenuCursor             = NULL;

        m_nowListItemCurrentPos   = 0;
        m_drawMode                = DRAW_MODE_SELECT_LIST;
        m_state                   = STATE_TYPE_NONE;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //  変数宣言一覧
      INIT_PARAM                m_initParam;
      StateType                 m_state;

      DrawMode                  m_drawMode;

      //  選択リスト項目オブジェクトバッファ
      ConfigLowerUIListItemObj  m_aSelectListItem[ SELECT_LIST_MAX ];

      app::tool::MenuCursor*    m_pMenuCursor;
      app::util::Heap*          m_pUtilHeap;

      s32                       m_nowListItemCurrentPos;

      //  選択リストの初期化データテーブル
      static const ConfigLowerUIListItemObj::INIT_PARAM m_sSelectListItemInitParamTable[ SELECT_LIST_MAX ];
      
      //  ボタン作成データ固定値専用
      static const BUTTON_CREATE_DATA m_sCreateButtonInfoTable[ BUTTON_ID_MAX ];

      //  リスト項目作成のデータリスト
      static const LIST_ITEM_CREATE_DATA  m_sListItemCreateDataTable[ CONFIG_TYPE_MAX ];

  };

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )

#endif // NIJI_PROJECT_CONFIG_LOWER_UI_DRAW_H_INCLUDED 
