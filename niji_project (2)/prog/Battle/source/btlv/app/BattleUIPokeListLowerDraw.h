//=============================================================================
/**
 * @file BattleUIPokeListLowerDraw.h
 * @brief バトル下画面 >> 「ポケモンリスト」画面
 * @author yuto_uchida
 * @date 2015.04.09
 */
//=============================================================================
#if !defined( __BATTLE_UI_POKE_LIST_LOWER_DRAW_H__ )
#define __BATTLE_UI_POKE_LIST_LOWER_DRAW_H__
#pragma once

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

#include "Battle/include/battle_SetupParam.h"
#include "Battle/source/btl_common.h"
#include "Battle/source/btl_pokeparam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  class AppCommonGrpIconData;

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /** バトル下画面のポケモンリスト画面の通知クラス */
  class BattleUIPokeListLowerDrawDelegate
  {
  public:
    typedef struct _tag_result_data_
    {
      s32 buttonId;

      //  わざ選択時のデータ
      struct _tag_skill_data_
      {
        WazaID  id;
        u8      nowPPNum, maxPPNum;
      } skill_data;

      s32 select_tokusei_id;
      u16 select_item_id;
      gfl2::math::Vector3 select_info_icon_pos;

      _tag_result_data_()
      {
        Clear();
      }

      void Clear()
      {
        buttonId          = -1;
        select_tokusei_id = TOKUSEI_NULL;
        select_item_id    = ITEM_DUMMY_DATA;

        gfl2::std::MemClear( &skill_data, sizeof( skill_data ) );
      }

    } RESULT_DATA;

    BattleUIPokeListLowerDrawDelegate() {}
    virtual ~BattleUIPokeListLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUIPokeListLowerDraw* pDrawInst, const RESULT_DATA& rResultData )  = 0;

    //  キー入力通知用リスナ
    virtual bool OnKeyAction( class BattleUIPokeListLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;


    //------------------------------------------------------------------
    /**
      * @brief わざタイプから現在配置している敵を参照して効果タイプを取得
      * @param[out]   pPutFlg       : 表示可能
      * @param[in]    wazaNo        : 対象のわざNo
      * @param[in]    pBtlPokeParam : wazaNoも持っているバトルポケモンのパラメータ
      * @note
      *  描画側でバトルのパラメータを持たせる必要性がないので、どうやって取得しようかと考えた所、デリケーター関数を経由して取得しようと考えた
      */
    //------------------------------------------------------------------

    virtual BtlTypeAffAbout OnGetKoukaType( bool* pPutFlg, const WazaNo wazaNo, const BTL_POKEPARAM* pBtlPokeParam )  = 0;
  };

  /** ビューのボタン制御 */
  class BattleUIPokeListLowerButtonCtrl : public ::app::ui::UIInputListener
  {
  public:
    //  描画側で入力した結果id
    enum ButtonId
    {
      BUTTON_ID_NONE  = -1,
      BUTTON_ID_BACK  = 0,
      BUTTON_ID_CHANGE,
      BUTTON_ID_CURE,
      BUTTON_ID_STATUS,
      BUTTON_ID_POKE_ICON_0,
      BUTTON_ID_POKE_ICON_1,
      BUTTON_ID_POKE_ICON_2,
      BUTTON_ID_POKE_ICON_3,
      BUTTON_ID_POKE_ICON_4,
      BUTTON_ID_POKE_ICON_5,

      BUTTON_ID_SKILL_ITEM_00,
      BUTTON_ID_SKILL_ITEM_01,
      BUTTON_ID_SKILL_ITEM_02,
      BUTTON_ID_SKILL_ITEM_03,

      BUTTON_ID_ITEM_INFO_ICON,
      BUTTON_ID_TOKUSEI_INFO_ICON,

      BUTTON_ID_CHANGE_POS_LEFT,
      BUTTON_ID_CHANGE_POS_RIGHT,

      BUTTON_ID_MAX
    };

    //  カーソル移動タイプ
    enum CursorMoveType
    {
      CURSOR_MOVE_TYPE_LIST = 0,
      CURSOR_MOVE_TYPE_SPECIAL,
    };

    //  コンストラクタ
    BattleUIPokeListLowerButtonCtrl() { _Clear(); }

    virtual ~BattleUIPokeListLowerButtonCtrl()
    {
      GFL_SAFE_DELETE( m_pSubButtonCtrl );
    }

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    virtual void Initialize( class BattleUIPokeListLowerDraw* pRootView, ::app::ui::UIView* pActiveView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap, ::app::tool::MenuCursor* pMenuCursor );

    //  サブボタン管理を作成(クラス型はテンプレートで指定)
    template<class T>
    T* CreateSubButtonCtrlInst( ::app::ui::UIView* pView, ::app::tool::MenuCursor* pMenuCursor )
    {
      DeleteSubInput();

      GFL_ASSERT( pView );
      T* pInst = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) T();
      m_pSubButtonCtrl = pInst;
      m_pSubButtonCtrl->Initialize( m_pRootView, pView, m_useLytWkId, m_pMultiResId, m_pUtilHeap, pMenuCursor );

      return pInst;
    }

    //  サブ入力を削除
    void DeleteSubInput()
    {
      GFL_SAFE_DELETE( m_pSubButtonCtrl );
    }

    //  更新処理
    virtual void Update();

    //  サブ入力インスタンスを取得
    BattleUIPokeListLowerButtonCtrl* GetSubInput() { return m_pSubButtonCtrl; }

    //------------------------------------------------------------------
    /**
      * @brief   カレントカーソル項目を決定
      */
    //------------------------------------------------------------------
    virtual bool PushBtnCurrentCursor();

    //------------------------------------------------------------------
    /**
      * @brief   指定したボタンidを押す
      */
    //------------------------------------------------------------------
    bool PushButtonId( const ButtonId id );

    //------------------------------------------------------------------
    /**
      * @brief   カーソル移動
      */
    //------------------------------------------------------------------
    virtual bool MoveCursorUp( const bool bLoop );
    virtual bool MoveCursorDown( const bool bLoop );
    virtual bool MoveCursorLeft( const bool bLoop );
    virtual bool MoveCursorRight( const bool bLoop );

    //  カーソル移動種類取得
    virtual CursorMoveType GetCursorMoveTyoe() const;

    //  カレントカーソルを設定(押せるButtonId値で判別、一致しない場合は何も処理しない)
    virtual void SetCurrentCursor( const ButtonId id, const bool bUseMoveSE );

    //  カーソル移動idxを設定
    virtual bool SetCursorMoveIdx( const s32 cursorIdx ) { return true; }

    //  現在のカーソル移動idx
    s32 GetCursorMoveIdx() const;

    //  カーソル移動最大idx
    virtual s32 GetCursorMoveIdxMax();

    //  ボタンのテキスト設定
    virtual void SetNameButton( const ButtonId id, const s32 textId );

    //  ボタンの表示設定(ボタンを非表示設定してもカーソルは消えない)
    void SetVisibleButton( const ButtonId id, const bool bFlg );

    //  ボタンすべての有効設定( bFlg = false ならカーソルを非表示 )
    virtual void SetEnableButtonAll( const bool bFlg );

    //  ボタンの有効設定
    void SetEnableButton( const ButtonId id, const bool bFlg );

    //  ペインタッチ時を検知
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

    //  ペインタッチして離した時に検知
    virtual ListenerResult OnLayoutPaneEvent( const u32 buttonId );

    /// キー入力通知用リスナ
    virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  protected:

    // ボタン入力結果を出力
    virtual void _OutputInputResultData( BattleUIPokeListLowerDrawDelegate::RESULT_DATA* pResultData, const ButtonId pushButtonId );

    //  基本パラメータ設定
    void _SetBaseParam( class BattleUIPokeListLowerDraw* pRootView, ::app::ui::UIView* pActiveView, const s32 useLytWkId, gfl2::lyt::LytMultiResID* pMultiResId, ::app::util::Heap* pUtilHeap );

    //  タッチトリガー用のボタンidかチェック
    bool _IsTouchTriggerButtonId( const ButtonId id );

    //  変数宣言
    class BattleUIPokeListLowerButtonCtrl* m_pSubButtonCtrl;
    class BattleUIPokeListLowerDraw*  m_pRootView;
    ::app::ui::UIView*                m_pActiveView;
    s32                               m_useLytWkId;
    gfl2::lyt::LytWk*                 m_pLayoutWork;
    gfl2::lyt::LytMultiResID*         m_pMultiResId;
    ::app::util::Heap*                m_pUtilHeap;
    ::app::util::G2DUtil*             m_pG2DUtil;

    s32                               m_cursorIdx;
    s32                               m_cursorOldIdx;

    bool                              m_bInputEnable;

    bool                              m_bFirstCursorUpdate;
    bool                              m_bUseInputSE;
  private:
    //  変数初期化
    void _Clear()
    {
      m_pLayoutWork         = NULL;
      m_pSubButtonCtrl      = NULL;
      m_pRootView           = NULL;
      m_pActiveView         = NULL;
      m_useLytWkId          = -1;
      m_pMultiResId         = NULL;
      m_pUtilHeap           = NULL;
      m_pG2DUtil            = NULL;
      m_cursorIdx           = -1;
      m_cursorOldIdx        = -1;
      m_bFirstCursorUpdate  = true;
      m_bInputEnable        = true;
      m_bUseInputSE         = false;
    }
  };

  /** バトル下画面のポケモンリスト画面 */
  class BattleUIPokeListLowerDraw : public ::app::ui::UIView, public App::Tool::CommonMessageWindowManipulator::IEventHandler
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIPokeListLowerDraw );  // コピーを禁止

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

      //  ポケモンタイプ
      enum PokeType
      {
        POKE_TYPE_PLAYER  = 0,
        POKE_TYPE_PARTNER,
      };

      //  Viewモード
      enum ViewMode
      {
        //  コマンド選択モード
        VIEW_MODE_CMD_SELECT  = 0,

        //  つよさを見るモード
        VIEW_MODE_STATUS,

        //  ポケモン配置選択モード
        VIEW_MODE_POKE_POS_SELECT,
      };

      enum
      {
        EVENT_MESSAGE_SPEED_FRAME = 15,
      };

      //  ポケモン選択リスト項目データのパラメータ
      typedef struct _tag_poke_param_list
      {
        const BTL_POKEPARAM*  pParam;
        u8                   pokeIdx;

        PokeType              type;

        //  すでに選択済み
        bool                  bSelected;

        _tag_poke_param_list()
        {
          Clear();
        }

        void Clear()
        {
          pParam    = NULL;
          //  ポケidxで判定できない無効値に設定
          pokeIdx   = 0xff;
          type      = POKE_TYPE_PLAYER;
          bSelected = false;
        }

      } POKE_SELECT_ITEM_PARAM;

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                  pUtilHeap;
        ::app::util::AppRenderingManager*   pRenderingManager;

        BattleUIPokeListLowerDrawDelegate*  pDelegate;

        //  アイコングラフィックデータ
        ::app::tool::AppCommonGrpIconData*  pAppCommonGrpIconData;

        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData* pBtlAppMsgData;
        print::WordSet*     pBtlWordSet;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        //  メニューカーソルのレイアウトバイナリ
        void* pMenuCursorLytBuff;

        //  入力SEを使用するかどうか
        bool  bUseInputSE;

        //  ポケモンリストのパラメータ
        POKE_SELECT_ITEM_PARAM aPokeDataList[ BTL_PARTY_MEMBER_MAX ];

        _tag_init_param()
        {
          pUtilHeap             = NULL;
          pRenderingManager     = NULL;
          pLytBuff              = NULL;
          pBtlAppMsgData        = NULL;
          pBtlWordSet           = NULL;
          pMenuCursorLytBuff    = NULL;
          pDelegate             = NULL;
          pAppCommonGrpIconData = NULL;
          bUseInputSE           = false;

          gfl2::std::MemClear( aPokeDataList, sizeof( aPokeDataList ) );
        }

      } INIT_PARAM;

      //------------------------------------------------------------------
      /**
       * @brief   コンストラクタ
       */
      //------------------------------------------------------------------
      BattleUIPokeListLowerDraw( const INIT_PARAM& param );

      //------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //------------------------------------------------------------------
      virtual ~BattleUIPokeListLowerDraw();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      bool Initialize();

      //------------------------------------------------------------------
      /**
       * @brief   終了
       */
      //------------------------------------------------------------------
      void Terminate();

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
       * @brief   ボタン制御取得
       */
      //------------------------------------------------------------------
      BattleUIPokeListLowerButtonCtrl* GetButtonCtrl() const { return m_pButtonCtrl; }

      //------------------------------------------------------------------
      /**
       * @brief   メッセージイベント中か
       */
      //------------------------------------------------------------------
      bool IsProcEventMesssage();

      //------------------------------------------------------------------
      /**
       * @brief   現在のステート
       */
      //------------------------------------------------------------------
      StateType GetState() const { return m_state; }

      //------------------------------------------------------------------
      /**
       * @brief   ViewModeを変更
       */
      //------------------------------------------------------------------
      void SetViewMode( const ViewMode viewMode );

      //  ViewMode取得
      ViewMode GetViewMode() const { return m_viewMode; }

      //  ボタンアニメが終了しているか
      bool IsButtonEndAnime();

      //  イベントメッセージを開始
      void EventMessage( gfl2::str::StrBuf* pMessageStrBuf );

      //  ポケオブジェクトリストのアクティブ設定
      void SetActivePokeObjList( const s32 listIdx );

      //  ポケオブジェクトリスト内で選択しているポケモン項目データのアドレス取得
      const POKE_SELECT_ITEM_PARAM* GetSelectPokeObjDataPtr();

      //  設定した初期化パラメータ取得
      INIT_PARAM& GetInitParam() { return m_initParam; }

      //  ポケモンアイコンのボタン設定データ作成
      const ::app::ui::ButtonInfoEx CreatePokeIconButtonInfoEx( const s32 idx, const BattleUIPokeListLowerButtonCtrl::ButtonId buttonId );

      //  ポケモンリストが有効か
      bool IsEnablePokeObjList() const { return m_bEnablePokeObjList; }

      //  選択指定したポケモンデータを使って画面更新
      void UpdateCurrentPokeStatusView( const s32 currentListIdx );

      //------------------------------------------------------------------
      /**
       * @brief   汎用メッセージのコールバッグ
       */
      //------------------------------------------------------------------
      virtual void  CommonMessageWindowManipulator_OnEvent( const App::Tool::CommonMessageWindowManipulator::EventID event );

    private:
      friend class BattleUIPokeListLowerSubViewBase;
      friend class BattleUIPokeListLowerSimpleStatusSubView;
      friend class BattleUIPokeListLowerDetailStatusSubView;

      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      //  ポケモンオブジェクト最大数
      enum
      {
        POKE_OBJ_MAX  = 6,
      };

      //  更新ステートのサブステート
      enum UpdateSubState
      {
        UPDATE_SUB_STATE_NORMAL = 0,
        UPDATE_SUB_STATE_MSG_OPEN,
        UPDATE_SUB_STATE_MSG_UPDATE,
        UPDATE_SUB_STATE_MSG_CLOSE,
      };

      //  配置されているボタン一覧
      enum ButtonObj
      {
        BUTTON_OBJ_BACK  = 0,
        BUTTON_OBJ_POKE_ICON_0,
        BUTTON_OBJ_POKE_ICON_1,
        BUTTON_OBJ_POKE_ICON_2,
        BUTTON_OBJ_POKE_ICON_3,
        BUTTON_OBJ_POKE_ICON_4,
        BUTTON_OBJ_POKE_ICON_5,
        BUTTON_OBJ_MAX,
      };

      enum
      {
        POKE_TYPE_MAX = 2,
      };

      //  メッセージデータ
      typedef struct
      {
        gfl2::str::StrBuf*  pStrBuf;
        gfl2::str::StrBuf*  pBlankStrBuf;

      } MESSAGE_DATA;

      //  ポケモンオブジェクトカーソル
      //  吉田さん要望：選択時にアニメ再生
      class PokeObjectCursor
      {
      public:
        PokeObjectCursor();

        void Initialize( UIView* pView );

        void SetPos( const gfl2::math::VEC3& rPos );
        void SetActive( const bool bFlg );
        bool IsActive() const { return m_bActive; }

        void Update();

      private:
        enum StatePokeObjectCursorState
        {
          STATE_POKE_OBJECT_CURSOR_NONE  = 0,
          STATE_POKE_OBJECT_CURSOR_START,
          STATE_POKE_OBJECT_CURSOR_KEEP,
          STATE_POKE_OBJECT_CURSOR_END,
        };

        void _Clear()
        {
          m_pPokeListCursorPane = NULL;
          m_pView = NULL;
          m_state = STATE_POKE_OBJECT_CURSOR_NONE;
          m_bActive = false;
        }

        UIView* m_pView;

        //  ポケモンアイコンカーソルペイン
        gfl2::lyt::LytPane*        m_pPokeListCursorPane;

        StatePokeObjectCursorState m_state;
        bool  m_bActive;
      };

      //  ポケモンオブジェクト
      class PokeObject
      {
      public:
        enum AnimeType
        {
          ANIME_TYPE_POKEICON_DIE = 0,
          ANIME_TYPE_POKEICON_REVIVE,
          ANIME_TYPE_POKEICON_ITEM_CATCH,
          ANIME_TYPE_POKEICON_TEM_RELEASE,
          ANIME_TYPE_POKEICON_JAMP_HIGH,
          ANIME_TYPE_POKEICON_JAMP_MIDDLE,
          ANIME_TYPE_POKEICON_JAMP_LOW,
          ANIME_TYPE_POKEICON_SICK,
          ANIME_TYPE_POKEICON_TOUCH,
          ANIME_TYPE_POKEICON_RELEASE,
          ANIME_TYPE_POKEICON_TOUCH_RELEASE,
          ANIME_TYPE_POKEICON_JAMP_SELECT,
          ANIME_TYPE_POKEICON_STOP,
          ANIME_TYPE_MAX,
        };

        //  初期化パラメータ
        typedef struct _tag_init_param_
        {
          ::app::ui::UIView*    pUIView;
          ::app::util::Heap*    pUtilHeap;
          const BTL_POKEPARAM*  pBtlParam;
          PokeType              pokeType;

          gfl2::lyt::LytParts*  pIconPartsPane;

          gfl2::str::StrBuf*    pTokuseiName;
          gfl2::str::StrBuf*    pHaveItemName;

          //  必要なアニメ
          s32 aAnimeIdx[ ANIME_TYPE_MAX ];

          //  持っているわざ名をあらかじめ持っておく
          struct _tag_waza_param_
          {
            WazaID                id;
            u8                    nowPP;
            u8                    maxPP;
            gfl2::str::StrBuf*    pWazaNameBuffer;
          } aWazaParam[ 4 ];
          
          u8  wazaCount;


          _tag_init_param_()
          {
            pUIView         = NULL;
            pUtilHeap       = NULL;
            pBtlParam       = NULL;
            pTokuseiName    = NULL;
            pHaveItemName   = NULL;

            wazaCount = 0;

            pIconPartsPane   = NULL;
            pokeType         = POKE_TYPE_PLAYER;
            gfl2::std::MemFill( aWazaParam, NULL, sizeof( aWazaParam ) );
          }

        } INIT_PARAM;

        PokeObject()
        {
          m_pIcon           = NULL;
          m_bActive         = false;
          m_bVisible        = false;
          m_bEnable         = false;
          m_bLoadIcon       = false;
          m_skillSelectIdx  = 0;
          m_itemIdx         = 0;
        }

        ~PokeObject()
        {
          s32 loopNum = GFL_NELEMS( m_initParam.aWazaParam );
          for( s32 i = 0; i < loopNum; ++i )
          {
            GFL_SAFE_DELETE( m_initParam.aWazaParam[ i ].pWazaNameBuffer );
          }

          GFL_SAFE_DELETE( m_initParam.pTokuseiName );
          GFL_SAFE_DELETE( m_initParam.pHaveItemName );

          GFL_SAFE_DELETE( m_pIcon );
        }

        //  初期化
        void Initialize( const INIT_PARAM& rInitParam, const u8 itemIdx );
        
        //  準備よし
        bool IsReady();

        //  終了
        void Terminate();

        //  インスタンスの破棄可能か
        bool IsDelete();

        //  更新
        void Update();

        //  処理有効設定(有効になっていないと更新が動かないので注意. でも表示設定はできる)
        void SetEnable( const bool bFlg ) { m_bEnable = bFlg; }

        //------------------------------------------------------------------
        /**
         * @brief   オブジェクトのアクティブ設定
         * @return  状態変更 -> true / なにも変わっていない -> false
         * @note
         *  アクティブ位置に移動させるカーソル用のペインを指定
         */
        //------------------------------------------------------------------
        bool SetAcitve( const bool bFlg, PokeObjectCursor* pObjectCursor );

        //  オブジェクトがアクティブになっているかどうか
        s32 IsActive() const { return m_bActive; }

        //  表示設定
        void SetVisible( const bool bFlg );

        //  ボタンデータを取得
        const ::app::ui::ButtonInfoEx CreateButtonInfoEx( const s32 buttonId );

        //  初期化パラメータを取得
        const INIT_PARAM& GetInitParam() const { return m_initParam; }

      //  データ保存のみなのでわざわざ関数を作る必要もない
        s32                         m_skillSelectIdx;
        u8                          m_itemIdx;

      private:
        ::app::tool::PokeIconToolEx*  m_pIcon;
        bool                          m_bActive;
        bool                          m_bVisible;
        bool                          m_bEnable;
        bool                          m_bLoadIcon;

        INIT_PARAM                  m_initParam;
      };

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();

      void _SetState( const StateType newStateType );
      //  更新内のステート
      void _SetUpdateSubState( const UpdateSubState newState );

      //  ポケモンリストの表示設定
      void _SetEnablePokeList( const bool bFlg );

      //  PokeObjのINIT_PARAM生成
      void _OutputPokeObjInitParam( PokeObject::INIT_PARAM* pOutputData, const s32 listIdx );

      //  ポケオブジェクトリスト内で選択しているポケモンオブジェクト取得
      PokeObject* _GetSelectPokeObjPtr();

      //  メッセージ処理中か
      bool _IsMessageProc();

      //  変数初期化
      void _Clear()
      {
        m_state                   = STATE_TYPE_NONE;
        m_viewMode                = VIEW_MODE_CMD_SELECT;
        m_pSexIconPartsPane       = NULL;
        m_pLvNumTextBoxPane       = NULL;
        m_cmdViewCurrentCursorIdx = 0;
        m_pActiveSubView          = NULL;
        m_pButtonCtrl             = NULL;
        m_updateSubState          = UPDATE_SUB_STATE_NORMAL;
        m_pHelpTextPane           = NULL;
        m_pCommonMessageWindow    = NULL;
        m_pBtnMenuCursor          = NULL;
        m_pSickIconPicturePane    = NULL;
        m_pLvIconTextBoxPane      = NULL;
        m_bEnablePokeObjList      = false;

        m_bFinishCommomMessageWindow  = false;

        gfl2::std::MemFill( m_pPokeTypeIconPicPaneList, NULL, sizeof( m_pPokeTypeIconPicPaneList ) );
        gfl2::std::MemFill( &m_messageData, NULL, sizeof( m_messageData ) );

        m_initSeq = 0;
      }

      //  変数宣言一覧
      INIT_PARAM        m_initParam;
      StateType         m_state;
      UpdateSubState    m_updateSubState;
      ViewMode          m_viewMode;
      PokeObject        m_aPokeObj[ POKE_OBJ_MAX ];

      //  コマンドカーソルidx
      s32               m_cmdViewCurrentCursorIdx;

      MESSAGE_DATA      m_messageData;

      //  状態異常アイコンペイン
      gfl2::lyt::LytPicture*                  m_pSickIconPicturePane;
      //  性別アイコンペイン
      gfl2::lyt::LytParts*                    m_pSexIconPartsPane;
      //  Lvアイコンテキストボックスペイン
      gfl2::lyt::LytTextBox*                  m_pLvIconTextBoxPane;
      //  Lvテキストボックスペイン
      gfl2::lyt::LytTextBox*                  m_pLvNumTextBoxPane;
      //  ポケモンタイプアイコンペイン画像ペイン
      gfl2::lyt::LytPicture*                  m_pPokeTypeIconPicPaneList[ POKE_TYPE_MAX ];
      //  ヘルプテキストペイン
      gfl2::lyt::LytTextBox*                  m_pHelpTextPane;

      //  現在有効になっているサブビュー
      class BattleUIPokeListLowerSubViewBase* m_pActiveSubView;

      //  入力制御
      BattleUIPokeListLowerButtonCtrl*        m_pButtonCtrl;

      bool                                    m_bEnablePokeObjList;

      ::app::tool::MenuCursor*                m_pBtnMenuCursor;

      //  汎用ウィンドウ
      App::Tool::CommonMessageWindowManipulator*  m_pCommonMessageWindow;

      //  汎用ウィンドウ終了フラグ
      bool                                        m_bFinishCommomMessageWindow;

      //  ポケモンオブジェクト用のカーソル
      PokeObjectCursor                            m_pokeObjectCursor;

      u32 m_initSeq;
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_POKE_LIST_LOWER_DRAW_H__ 
