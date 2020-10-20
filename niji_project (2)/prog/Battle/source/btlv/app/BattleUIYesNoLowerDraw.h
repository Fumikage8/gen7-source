//=============================================================================
/**
 * @file BattleUIYesNoLowerDraw.h
 * @brief バトル下画面 >> 「二者択一」画面
 * @author yuto_uchida
 * @date 2015.03.04
 */
//=============================================================================
#if !defined( __BATTLE_UI_YES_NO_LOWER_DRAW_H__ )
#define __BATTLE_UI_YES_NO_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面の２選択画面の通知クラス */
  class BattleUIYesNoLowerDrawDelegate
  {
  public:
    BattleUIYesNoLowerDrawDelegate() {}
    virtual ~BattleUIYesNoLowerDrawDelegate() {}

    // 選択した選択項目
    virtual bool OnPushSelectItem( const u32 in_itemIdx ) = 0;

    //  キーとスティックの入力結果
    virtual void OnKeyAction( class BattleUIYesNoLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /** バトル下画面の２選択画面 */
  class BattleUIYesNoLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIYesNoLowerDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_INPUT_IDLE,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      enum
      {
        SELECT_ITEM_MAX  = 2,
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                pUtilHeap;
        ::app::util::AppRenderingManager* pRenderingManager;
        BattleUIYesNoLowerDrawDelegate*   pDelegate;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        //  メニューカーソルのレイアウトバイナリ
        void* pMenuCursorLytBuff;

        // 初回選択項目メッセージId
        s32 aItemTextIdList[ SELECT_ITEM_MAX ];

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData* pBtlAppMsgData;
        print::WordSet*     pBtlWordSet;

        //  選択項目位置
        s32 selectItemIdx;

        //  入力関連のSEを使うか
        bool                 bUseInputSE;

        _tag_init_param()
        {
          pUtilHeap             = NULL;
          pRenderingManager     = NULL;
          pDelegate             = NULL;
          pLytBuff              = NULL;
          pMenuCursorLytBuff    = NULL;
          pBtlAppMsgData        = NULL;
          pBtlWordSet           = NULL;
          bUseInputSE           = false;
          
          //  初回カーソル位置はいいえに
          selectItemIdx         = SELECT_ITEM_MAX - 1;

          ::std::memset( aItemTextIdList, -1, sizeof( aItemTextIdList ) );
        }

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
      * @brief コンストラクタ
      */
      BattleUIYesNoLowerDraw( const INIT_PARAM& param );

      /**
      * @brief デストラクタ
      */
      virtual ~BattleUIYesNoLowerDraw();

      /**
        * @brief  初期化
        */
      bool InitFunc();

      /**
        * @brief  終了
        */
      bool EndFunc();

      /**
        * @brief 開始
        */
      void Start( void ) { _SetState( STATE_TYPE_IN ); }

      /**
        * @brief 終了
        */
      void End( void ) { _SetState( STATE_TYPE_OUT ); }

      /**
        * @brief 更新処理
        */
      virtual void Update( void );

      /**
      * @brief 描画
      */
      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      /**
      * @brief  現在のステート
      */
      StateType GetState() const { return m_state; }

      /**
      * @brief  acitveになっている選択項目
      */
      s32 GetActiveSelectItemIdx() const { return m_initParam.selectItemIdx; }

      /**
      * @brief  指定した選択項目をプッシュ
      */
      void PushActiveSelectItem( const s32 selectItemIdx );

      /**
      * @brief  上カーソル移動
      */
      bool MoveUpCursor();

      /**
      * @brief  下カーソル移動
      */
      bool MoveDownCursor();

      // ペインタッチ時を検知
      virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 buttonId );

      // ペインタッチ時を検知
      // 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
      virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

      /// キー入力通知用リスナ
      virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      //  選択項目のConfigデータ
      typedef struct
      {
        s32 root_pane_id;

      } SELECT_ITEM_CONFIG_PARAM;

      /** 選択項目オブジェクト */
      class SelectItem
      {
      public:
        typedef struct
        {
          //  基本設定
          SELECT_ITEM_CONFIG_PARAM  r_config_param;

          //  ボタン登録データ
          ::app::ui::ButtonInfoEx   button_info_ex;

          //  レイアウト
          gfl2::lyt::LytWk*         p_layout_work;

          //  参照するリソース
          gfl2::lyt::LytMultiResID* p_lyt_muitl_resId;

          //  2d制御
          ::app::util::G2DUtil*     p_g2d_util;

        } INIT_PARAM;

        SelectItem( const INIT_PARAM& rInitParam );

        //  初期パラメータ取得
        const INIT_PARAM& GetInitParam() const { return m_initParam; }

        /** テキスト設定 */
        void SetText( const s32 messageID, UIView* pView );

        /** アクティブ設定 */
        void SetActive( bool bFlg );

        /** カーソル位置ペイン */
        gfl2::lyt::LytPane* GetCursorPane() { return m_pCursorNullPane; }

      private:
        //  部品ペインを辿ってテキストペインを取得して表示設定
        gfl2::lyt::LytParts*    m_pPartsPane;
        gfl2::lyt::LytPane*     m_pCursorNullPane;
        gfl2::lyt::LytTextBox*  m_pTextBox;

        ::app::ui::ButtonInfoEx m_buttonInfo;
        INIT_PARAM              m_initParam;

        bool  m_bActive;
      };

      void _Clear()
      {
        m_initSeq = 0;
        m_endSeq  = 0;

        m_state = STATE_TYPE_NONE;
        m_pMenuCursor = NULL;

        gfl2::std::MemClear( m_pSelectItemList, sizeof( m_pSelectItemList ) ) ;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      // 選択項目のカーソルactive設定
      void _SetActiveSelectItemCursor( const s32 selectItemType );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;

      ::app::tool::MenuCursor*  m_pMenuCursor;

      //  選択項目オブジェクトリスト
      SelectItem* m_pSelectItemList[ SELECT_ITEM_MAX ];

      u32 m_initSeq;
      u32 m_endSeq;
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_YES_NO_LOWER_DRAW_H__ 
