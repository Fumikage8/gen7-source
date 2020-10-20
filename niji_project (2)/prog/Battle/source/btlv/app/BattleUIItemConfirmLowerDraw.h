//=============================================================================
/**
 * @file BattleUIItemConfirmLowerDraw.h
 * @brief バトル下画面 >> 「アイテム確認」画面
 * @author yuto_uchida
 * @date 2015.04.07
 * @note
 *  アイテムアイコン画像の::app::tool::ItemIconクラスを使っているが、
 *  このクラス内ではファイルの開閉はしない、使う側はファイルの開閉をする前提としてほしい
 *  このクラス内ではテクスチャーをロードして破棄する所まで
 *  理由
 *    ・ファイル開閉の処理は一度のみなので、描画処理しかしないここで行う必要がないから。
 *    ・アイコンが複数表示すると何度もファイル開閉をして、少なからずメモリが消費し、不要処理を走らせることになるから
 */
//=============================================================================
#if !defined( __BATTLE_UI_ITEM_CONFIRM_LOWER_DRAW_H__ )
#define __BATTLE_UI_ITEM_CONFIRM_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

#include "pml/include/item/item_manager.h"

#include <print/include/ItemMessage.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面の２選択画面の通知クラス */
  class BattleUIItemConfirmLowerDrawDelegate
  {
  public:
    BattleUIItemConfirmLowerDrawDelegate() {}
    virtual ~BattleUIItemConfirmLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUIItemConfirmLowerDraw* pDrawInst, s32 buttonId )  = 0;

    //  キーとスティックの入力結果
    virtual void OnKeyAction( class BattleUIItemConfirmLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /** バトル下画面の２選択画面 */
  class BattleUIItemConfirmLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener, public App::Tool::CommonMessageWindowManipulator::IEventHandler
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIItemConfirmLowerDraw );  // コピーを禁止

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
        BUTTON_ID_CANCEL  = 0,
        BUTTON_ID_ENTER,
        BUTTON_ID_MOVE_LEFT_LIST,
        BUTTON_ID_MOVE_RIGHT_LIST,

        BUTTON_ID_MAX,
      };

      //  矢印オブジェクト一覧
      enum
      {
        ARROW_OBJECT_LEFT = 0,
        ARROW_OBJECT_RIGHT,
        ARROW_OBJECT_MAX,
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                    pUtilHeap;
        ::app::util::AppRenderingManager*     pRenderingManager;
        BattleUIItemConfirmLowerDrawDelegate* pDelegate;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        //  あらかじめバッファ確保しているメニューカーソルデータ
        void* pLytMenuCursorBuff;

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData*     pBtlAppMsgData;
        print::WordSet*         pBtlWordSet;

        //  アイテムメッセージ管理
        print::ItemMessage*     pItemMessage;

        _tag_init_param()
        {
          pUtilHeap           = NULL;
          pRenderingManager   = NULL;
          pDelegate           = NULL;
          pLytBuff            = NULL;
          pBtlAppMsgData      = NULL;
          pBtlWordSet         = NULL;
          pLytMenuCursorBuff  = NULL;
          pItemMessage        = NULL;
        }

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      BattleUIItemConfirmLowerDraw( const INIT_PARAM& param );
      virtual ~BattleUIItemConfirmLowerDraw();

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
      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      //------------------------------------------------------------------
      /**
       * @brief   メッセージイベント開始
       */
      //------------------------------------------------------------------
      void EventMessage( gfl2::str::StrBuf*  pMessageStrBuf );

      //------------------------------------------------------------------
      /**
       * @brief   メッセージイベント開始
       */
      //------------------------------------------------------------------
      bool IsEventMessage()
      {
        if( m_updateSubState == UPDATE_SUB_STATE_MSG_OPEN )
        {
          return  true;
        }

        if( m_updateSubState == UPDATE_SUB_STATE_MSG_UPDATE )
        {
          return  true;
        }

        if( m_updateSubState == UPDATE_SUB_STATE_MSG_CLOSE )
        {
          return  true;
        }

        return  false;
      }

      //------------------------------------------------------------------
      /**
       * @brief   指定したidのボタンを押す
       * @param   buttonId    : 押すボタンのid
       * @param   bForcePush  : 入力がfalseになっても押すかどうか（捕獲チュートリアルでは入力がfalseになっているのでその時に使用する）
       */
      //------------------------------------------------------------------
      void PushBtn( ButtonId buttonId, const bool bForcePush = false );

      //------------------------------------------------------------------
      /**
       * @brief   情報表示設定
       */
      //------------------------------------------------------------------
      void SetInfoData( const s32 itemId, const s32 itemNum, const item::BtlPocket itemPocketIdx );

      //------------------------------------------------------------------
      /**
       * @brief     背景表示設定
       * @param[in] bFlg : 表示するかのフラグ
       */
      //------------------------------------------------------------------
      void SetViaibleBG( const bool bFlg );

      //------------------------------------------------------------------
      /**
       * @brief   ボタン表示設定
       */
      //------------------------------------------------------------------
      void SetVisibleButton( const ButtonId id, const bool bFlg );

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

      //------------------------------------------------------------------
      /**
       * @brief   ペインタッチ時を検知
       */
      //------------------------------------------------------------------
      virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 buttonId );

      //------------------------------------------------------------------
      /**
       * @brief   キー入力通知用リスナ
       */
      //------------------------------------------------------------------
      virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);

      //------------------------------------------------------------------
      /**
       * @brief   汎用メッセージのコールバッグ
       */
      //------------------------------------------------------------------
      virtual void  CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event );

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      //  更新ステートのサブステート
      enum UpdateSubState
      {
        UPDATE_SUB_STATE_INFO = 0,
        UPDATE_SUB_STATE_MSG_OPEN,
        UPDATE_SUB_STATE_MSG_UPDATE,
        UPDATE_SUB_STATE_MSG_CLOSE,
      };

      //  矢印オブジェクト
      class ArrowObject
      {
      public:
        enum ArrowStateType
        {
          ARROW_STATE_TYPE_IDLE = 0,
          ARROW_STATE_TYPE_PUSH
        };

        typedef struct _tag_setup_
        {
          UIView* pView;
          u32     lytId;

          _tag_setup_()
          {
            pView       = NULL;
            lytId       = 0;
          }

        } SETUP_DATA;

        ArrowObject();

        //  初回で必ず呼ぶ
        void Setup( const SETUP_DATA& rSetupData );

        void Push();
        void Update();

      private:
        void _SetState( const ArrowStateType newState );

        void _Clear()
        {
          m_state = ARROW_STATE_TYPE_IDLE;
        }

        SETUP_DATA          m_setupData;
        ArrowStateType      m_state;
      };

      //  ボタンデータ
      typedef struct
      {
        ButtonId  id;
        u32       seId;
      } BUTTON_DATA;

      void _Clear()
      {
        m_state                       = STATE_TYPE_NONE;
        m_updateSubState              = UPDATE_SUB_STATE_INFO;
        m_pMenuCursor                 = NULL;
        m_pItemIcon                   = NULL;
        m_pMesssageStrBuf             = NULL;
        m_pItemCategoryNameTextPane   = NULL;
        m_pItemNameTextPane           = NULL;
        m_pItemHaveNumTextPane        = NULL;
        m_pItemDescriptionTextPane    = NULL;
        m_pItemIconPicturePane        = NULL;
        m_pCommonMessageWindow        = NULL;
        m_pBGPane                     = NULL;

        m_bFinishCommomMessageWindow  = false;
        m_itemChangeStep              = 0;
        m_itemChangeId                = -1;
      }

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );
      void _SetUpdateSubState( const UpdateSubState newState );

      //  変数宣言一覧

      INIT_PARAM                  m_initParam;
      StateType                   m_state;
      UpdateSubState              m_updateSubState;

      u32                         m_itemChangeStep;
      s32                         m_itemChangeId;

      gfl2::lyt::LytTextBox*      m_pItemCategoryNameTextPane;
      gfl2::lyt::LytTextBox*      m_pItemNameTextPane;
      gfl2::lyt::LytTextBox*      m_pItemHaveNumTextPane;
      gfl2::lyt::LytTextBox*      m_pItemDescriptionTextPane;
      gfl2::lyt::LytPicture*      m_pItemIconPicturePane;
      gfl2::lyt::LytPane*         m_pBGPane;

      //  左右矢印のオブジェクト
      ArrowObject                 m_aArrowObject[ ARROW_OBJECT_MAX ];

      ::app::tool::MenuCursor*    m_pMenuCursor;

      //  アイテムアイコン画像のファイル開閉は使う側で行う
      ::app::tool::ItemIcon*      m_pItemIcon;

      //  メッセージ用
      gfl2::str::StrBuf*          m_pMesssageStrBuf;

      //  汎用ウィンドウ
      App::Tool::CommonMessageWindowManipulator*  m_pCommonMessageWindow;

      //  汎用ウィンドウ終了フラグ
      bool                                        m_bFinishCommomMessageWindow;

      //  ボタンデータテーブル(現在はSEの設定、再生のみしかない、アニメid設定など細かいのは不要)
      static const BUTTON_DATA  m_aButtonDataTable[ BUTTON_ID_MAX ];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_ITEM_CONFIRM_LOWER_DRAW_H__ 
