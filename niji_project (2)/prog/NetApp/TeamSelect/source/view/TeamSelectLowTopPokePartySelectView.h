//======================================================================
/**
 * @file TeamSelectLowTopPokePartySelectView.h
 * @date 2015/07/02 19:29:18
 * @author uchida_yuto
 * @brief トップ画面のポケモンパーティー選択下画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_LOW_TOP_POKE_PARTY_SELECT_VIEW_H_INCLUDED__
#define __TEAM_SELECT_LOW_TOP_POKE_PARTY_SELECT_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/AppToolNewPaneList.h"

#include "NetApp/TeamSelect/source/Parts/TeamSelectPokePartyLayoutParts.h"
#include "NetApp/TeamSelect/source/TeamSelectShareParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  
  class MenuCursor;
  class PokeIcon;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  /**
   * @class TeamSelectLowTopPokePartySelectView
   */
  class TeamSelectLowTopPokePartySelectView : public app::ui::UIView, public app::ui::UIInputListener
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectLowTopPokePartySelectView);

  public:
    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pLytBinData           = NULL;
        pAppMsgData           = NULL;
        pWordSet              = NULL;
        pPokeIconTexManager   = NULL;
        pViewParamArray       = NULL;
        viewArrayNum          = 0;
      }

      void*                 pLytBinData;
      gfl2::str::MsgData*   pAppMsgData;
      print::WordSet*       pWordSet;
      app::tool::PokeIcon*  pPokeIconTexManager;

      struct TEAM_VIEW_PARAM* pViewParamArray;
      u32 viewArrayNum;
    };

    //  状態一覧
    enum State
    {
      STATE_IDLE  = 0,
      STATE_INPUT_EVENT_WAIT,
      STATE_INPUT_STOP,
      STATE_AUTO_SCROLL
    };

    //  ボタンid
    enum ButtonId
    {
      BUTTON_ID_TEAM_SETTING  = 0,
      BUTTON_ID_TEAM_ENTER,
      BUTTON_ID_SELECT_LEFT,
      BUTTON_ID_SELECT_RIGHT,

      BUTTON_ID_MAX
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] rInitParam 初期化パラメータ
      */
    //------------------------------------------------------------------
    TeamSelectLowTopPokePartySelectView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager );
    virtual ~TeamSelectLowTopPokePartySelectView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    void Initialize(  const struct INIT_PARAM& rIntiParam );

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

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      開始処理
    */
    //--------------------------------------------------------------------------------------------
    void Start();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
      * @brief   ペインタッチ時を検知
      * @param[in] id ボタンid
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 id );

    //------------------------------------------------------------------
    /**
      * @brief   キー入力通知用リスナ
      * @param[in] pButton  ボタン入力の情報
      * @param[in] pKey     キー入力の情報
      * @param[in] pStick   スティック入力の情報
      * @return 入力処理を続けるかを返す
      */
    //------------------------------------------------------------------
    virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

    //------------------------------------------------------------------
    /**
      * @brief   状態取得
      */
    //------------------------------------------------------------------
    State GetState() const { return m_state; }

    //------------------------------------------------------------------
    /**
      * @brief   押したボタンid取得
      */
    //------------------------------------------------------------------
    ButtonId  GetPushBtnId() const { return m_pushButtonId; }

    //------------------------------------------------------------------
    /**
      * @brief   選択したリストidx取得
      */
    //------------------------------------------------------------------
    u32  GetCurrentListIdx() const;

    //------------------------------------------------------------------
    /**
      * @brief   選択したレイアウトリストidx取得
      */
    //------------------------------------------------------------------
    u32  GetCurrentLayoutListIdx() const;

    //------------------------------------------------------------------
    /**
      * @brief      指定したチーム項目のオブジェクトのY座標設定をする
      * @param[in]  selectIdx : 座標設定するパーティーオブジェクトの選択idx
      * @param[in]  yPos : Y座標
      */
    //------------------------------------------------------------------
    void  SetPosYPokePartyFrame( const u32 selectIdx, const f32 yPos );

    //------------------------------------------------------------------
    /**
      * @brief      自動スクロールステート設定
      * @param[in]  listGoalIdx: スクロール後のゴールidx
      * @param[in]  bNextStateIdle: 処理終了時にステートをidleにするか
      * @return     設定成功 true / 設定失敗 false
      */
    //------------------------------------------------------------------
    bool SetStateAutoScroll( const u32 listGoalIdx, const bool bNextStateIdle = true );

    //------------------------------------------------------------------
    /**
      * @brief   チームデータ取得取得
      */
    //------------------------------------------------------------------
    const struct TEAM_VIEW_PARAM& GetTeamViewListItemParam( const u32 idx ) const
    {
      GFL_ASSERT( idx < m_initParam.viewArrayNum );
      return  m_initParam.pViewParamArray[ idx ];
    }

    //------------------------------------------------------------------
    /**
      * @brief   チームリスト側で再生するSEを有効にするかどうか
      */
    //------------------------------------------------------------------
    void SetEnableListPlaySE( const bool bFlg ) { m_bListPlaySE = bFlg; }

    //------------------------------------------------------------------
    /**
      * @brief   リストのＹ座標
      */
    //------------------------------------------------------------------
    f32 GetListYPos();

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    enum
    {
      POKE_PARTY_VIEW_MAX = 4,
    };

    //  バーティー選択用のリスト
    class ListParty
      : public App::Tool::NewPaneList::ListListener
    {
    public:
      //-----------------------------------------------------------------------------
      /**
       * @brief   コンストラクタ
       */
      //-----------------------------------------------------------------------------
      ListParty( TeamSelectLowTopPokePartySelectView* pRootView, app::util::Heap* pUtilHeap );

      //-----------------------------------------------------------------------------
      /**
       * @brief   デストラクタ
       */
      //-----------------------------------------------------------------------------
      virtual ~ListParty();

      //------------------------------------------------------------------
      /**
        * @brief   初期化
        */
      //------------------------------------------------------------------
      void Initialize();

      //------------------------------------------------------------------
      /**
        * @brief   終了処理
        */
      //------------------------------------------------------------------
      void Terminate();

      //------------------------------------------------------------------
      /**
        * @brief      リストスクロール移動
        * @param[in]  mode スクロールタイプ
        */
      //------------------------------------------------------------------
      void RequestScroll( const App::Tool::NewPaneList::ScrollMode mode );

      //------------------------------------------------------------------
      /**
        * @brief      リストスクロール移動量設定
        * @param[in]  pScrollSpeedParamArray: スピード値の配列
        */
      //------------------------------------------------------------------
      void SetSpeedScroll( const App::Tool::NewPaneList::ScrollParam* pScrollSpeedParamArray );

      //------------------------------------------------------------------
      /**
        * @brief   リストスクロール中
        * @return   リストスクロール中 true / リストスクロール停止 false
        */
      //------------------------------------------------------------------
      bool IsMoveScroll() const;

      //------------------------------------------------------------------
      /**
        * @brief    リストのカレントリストidx
        * @return   カレントリストidx
        */
      //------------------------------------------------------------------
      u32 GetCurrentListIdx() const;

      //------------------------------------------------------------------
      /**
        * @brief    リストのレイアウトリストidx
        * @return   レイアウトリストidx
        */
      //------------------------------------------------------------------
      u32 GetCurrentLayoutListIdx() const;

      //------------------------------------------------------------------
      /**
        * @brief    リストのSE再生を停止
        */
      //------------------------------------------------------------------
      void StopSE();

    private:
      virtual void PaneListListener_Write( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
      virtual void PaneListListener_MoveCursor( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
      virtual void PaneListListener_Decide( u32 pos );

    private:
      void _Clear()
      {
        m_pRootView     = NULL;
        m_pNewPaneList  = NULL;
        m_pUtilHeap     = NULL;
        gfl2::std::MemClear( m_aListPaneData, sizeof( m_aListPaneData ) );
      }

      TeamSelectLowTopPokePartySelectView*  m_pRootView;
      app::util::Heap*                      m_pUtilHeap;
      App::Tool::NewPaneList*               m_pNewPaneList;
      App::Tool::NewPaneList::PANE_DATA     m_aListPaneData[ POKE_PARTY_VIEW_MAX ];
    };

    //  ポケモンパーティレイアウト作成のセットアップ構造体
    struct POKE_PARTY_VIEW_SETUP_DATA
    {
      gfl2::lyt::LytPaneIndex  rootPaneIdx;

      struct TeamSelectPokePartyLayoutParts::LAYOUT_ANIME_DATA animeData;
    };

    //------------------------------------------------------------------
    /**
      * @brief   2D画面の作成
      * @param   pAppLytBuff レイアウトデータのバイナリポインタ
      */
    //------------------------------------------------------------------
    void _Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief       TeamSelectPokePartyLayoutPartsクラスのレイアウトパラメータを出力
      * @param[out]  出力パラメータ
      * @param[in]   viewレイアウトidx( 0 - 3 )
      */
    //------------------------------------------------------------------
    void _OuputPokePartyLayoutParam( struct TeamSelectPokePartyLayoutParts::LAYOUT_PARAM* pOutParam, const u32 layoutIdx );

    //------------------------------------------------------------------
    /**
      * @brief      レイアウトViewパラメータを設定
      * @param[in]  viewListIdx:     描画リストidx
      * @param[in]  viewDataListIdx: idx値の配列のViewデータを設定
      */
    //------------------------------------------------------------------
    void _SetListItemView( const u32 viewListIdx, const u32 viewDataListIdx );

    //------------------------------------------------------------------
    /**
      * @brief   リスト最大数取得
      */
    //------------------------------------------------------------------
    u32 _GetListMax() const 
    {
      return m_initParam.viewArrayNum;
    }

    //------------------------------------------------------------------
    /**
      * @brief      ポケモンパーティーリスト項目データ取得
      * @param[in]  idx: リスト項目のidx
      * @reutrn     idx値のリスト項目データ
      */
    //------------------------------------------------------------------
    const TeamSelectPokePartyLayoutParts& _GetPokePartyListItem( const u32 idx ) const
    {
      GFL_ASSERT( idx < GFL_NELEMS( m_pPokePartyLayoutList ) );
      return  *(m_pPokePartyLayoutList[ idx ]);
    }

    //------------------------------------------------------------------
    /**
      * @brief   ボタンの有効設定
      */
    //------------------------------------------------------------------
    void _SetEnableButtonInput( const bool bFlg );

    //------------------------------------------------------------------
    /**
      * @brief   矢印ボタンの表示更新
      */
    //------------------------------------------------------------------
    void _UpdateArrowButtonVisible();

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_pHeap                 = NULL;
      m_pRenderManager        = NULL;
      m_state                 = STATE_IDLE;
      m_pushButtonId          = BUTTON_ID_MAX;
      m_subState              = 0;
      m_autoScrollListGoalIdx = 0;
      m_bListPlaySE           = true;
      m_bMoveListSkipInput    = false;
      m_bAutoScrollEndNextStateIdle = true;

      gfl2::std::MemClear( m_pPokePartyLayoutList, sizeof( m_pPokePartyLayoutList ) ) ;
    }

    //  子クラスが親に自由にアクセスできる
    friend class ListParty;

    app::util::Heap*                m_pHeap;
    app::util::AppRenderingManager* m_pRenderManager;
    ButtonId                        m_pushButtonId;

    TeamSelectPokePartyLayoutParts* m_pPokePartyLayoutList[ POKE_PARTY_VIEW_MAX ];
    State                           m_state;
    u32                             m_subState;

    u32                             m_autoScrollListGoalIdx;
    bool                            m_bAutoScrollEndNextStateIdle;

    INIT_PARAM                      m_initParam;
    ListParty                       m_listParty;
    
    bool                            m_bListPlaySE;
    bool                            m_bMoveListSkipInput;

    static const struct POKE_PARTY_VIEW_SETUP_DATA  m_sPokePartyLayoutSetupDataArray[];

    //  ユーザー入力リストスクロール値
    static const App::Tool::NewPaneList::ScrollParam m_saDefaultScrollParam[ App::Tool::NewPaneList::SCROLL_PARAM_MAX ];
    //  ループ自動移動時のスクロール値
    static const App::Tool::NewPaneList::ScrollParam m_saAutoLoopScrollParam[ App::Tool::NewPaneList::SCROLL_PARAM_MAX ];
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_LOW_TOP_POKE_PARTY_SELECT_VIEW_H_INCLUDED__