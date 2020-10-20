//======================================================================
/**
 * @file BattleVideoPlayerLowDataCtrlView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの下画面データ制御View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_VIDEO_PLAYER_LOW_DATA_CTRL_VIEW_H_INCLUDED__
#define __BATTLE_VIDEO_PLAYER_LOW_DATA_CTRL_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class MenuCursor;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  /**
   * @class LowDataCtrlView
   * @brief アプリの下画面データリストView
   */
  class LowDataCtrlView : 
  public app::ui::UIView,
  public app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN(LowDataCtrlView);

  public:
    //  状態一覧
    enum State
    {
      STATE_IDLE  = 0,
      STATE_INPUT_EVENT_WAIT,
      STATE_INPUT_STOP,
    };

    //  インフォモード
    enum InfoMode
    {
      INFO_MODE_DETAIL  = 0,
      INFO_MODE_RESULT
    };

    //  ボタンid一覧
    enum ButtonId
    {
      BUTTON_ID_PLAYER  = 0,
      BUTTON_ID_SELECT_01,
      BUTTON_ID_SELECT_02,
      BUTTON_ID_SELECT_03,
      BUTTON_ID_CHNAGE_INFO,
      BUTTON_ID_MAX,

      BUTTON_ID_NONE
    };

    enum ButtonContent
    {
      BUTTON_CONTENT_VS = 0,
      BUTTON_CONTENT_GO_UPLOAD,
      BUTTON_CONTENT_DEL,
      BUTTON_CONTENT_CANCEL_UPLOAD,

      BUTTON_CONTENT_MAX,
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pHeap           = NULL;
        pRenderManager  = NULL;
        pLytBinData     = NULL;
        pAppMsgData     = NULL;
        pWordSet        = NULL;

        pMenuCursorBinData  = NULL;
      }

      app::util::Heap*                pHeap;
      app::util::AppRenderingManager* pRenderManager;
      void*                           pLytBinData;
      gfl2::str::MsgData*             pAppMsgData;
      print::WordSet*                 pWordSet;
      void*                           pMenuCursorBinData;
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    LowDataCtrlView( INIT_PARAM& rInitParam );

    virtual ~LowDataCtrlView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    bool InitFunc();

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    bool EndFunc();

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
      * @brief   インフォモード設定
      * @param[in] mode 設定するモード
      */
    //------------------------------------------------------------------
    void SetInfoMode( const InfoMode mode );

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

    //------------------------------------------------------------------
    /**
      * @brief   Viewの入力終了か
      */
    //------------------------------------------------------------------
    bool IsEndInput() const { return ( m_state == STATE_INPUT_STOP ); }

    //------------------------------------------------------------------
    /**
      * @brief   選択ボタンを非表示
      */
    //------------------------------------------------------------------
    void SetContentButtonNonVisible();
    void SetContentButtonNonVisible( const ButtonId btnId );

    //------------------------------------------------------------------
    /**
      * @brief   ボタン内容を設定
      * @param[in] buttonId 表示設定するボタンid
      * @param[in] content  設定内容
      * @param[in] attachId ボタンに割り当ているid
      * @note 設定と同時に表示フラグも立てる
      */
    //------------------------------------------------------------------
    void SetContentButton( const ButtonId id, const ButtonContent content, const u32 attachId );

    //  画面終了時に押したボタンidを取得
    ButtonId  GetPushButtonId() const { return m_pushButtonId; }

    //  ボタンに割り当ているid
    u32 GetButtonAttachId( const ButtonId id ) { return m_aButtonData[ id ].id; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインタッチ時を検知
    *       【ボタンマネージャー関係】
    *
    * @param  painId   通知を受けるペインＩＤ
    *
    * @return none
    * 
    * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
    * 
    *                 TRIGER判定として使いたい場合は、ここを使わずに
    *                 ボタンマネージャーモードをTRIGGERにして
    *                 OnLayoutPaneEventを使うべき。
    */
    //--------------------------------------------------------------------------------------------
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 id );

    //------------------------------------------------------------------
    /**
      * @brief   ペインタッチ時を検知
      * @param[in] buttonId ボタンid
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
    
  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    enum MoveCursorType
    {
      MOVE_CURSOR_TYPE_ADD,
      MOVE_CURSOR_TYPE_SUB,
    };

    struct BUTTON_DATA
    {
      gfl2::lyt::LytParts*  pRootPane;
      gfl2::lyt::LytPane*   pCursorPane;
      u32 id;

      //  初期化
      BUTTON_DATA()
      {
        pRootPane = NULL;
        pCursorPane = NULL;
        id          = 0;
      }
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
      * @brief    カーソル位置を設定
      * @note     指定した位置になければ設定できる位置に補正
      */
    //------------------------------------------------------------------
    void _SetCursorPosFit( const s32 pos );

    //------------------------------------------------------------------
    /**
      * @brief    カーソル位置を設定
      * @return   設定成功: true / 失敗: false
      */
    //------------------------------------------------------------------
    bool _SetCursorPos( const s32 pos, const bool bChangeSE );

    //------------------------------------------------------------------
    /**
      * @brief   カーソル位置移動を設定
      */
    //------------------------------------------------------------------
    void _SetCursorMovePos( const MoveCursorType type );

    //------------------------------------------------------------------
    /**
      * @brief   カーソル位置のエリア切り替え
      * @note    再生ボタンと選択ボタンを二つのエリアとみなして切り替え
      */
    //------------------------------------------------------------------
    void _ChangeCursorAreaMove();

    //------------------------------------------------------------------
    /**
      * @brief   カーソル位置からボタンidを取得
      */
    //------------------------------------------------------------------
    ButtonId _GetButtonIdFromCursorPos( const s32 pos );

    //------------------------------------------------------------------
    /**
      * @brief   ボタンidからカーソル位置を取得
      */
    //------------------------------------------------------------------
    s32 _GetCursorPosFromButtonId( const ButtonId id );

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_state           = STATE_IDLE;
      m_initSeq         = 0;
      m_endSeq          = 0;
      m_cursorPos       = 0;

      m_activeSelectButtonId  = BUTTON_ID_NONE;
      m_pushButtonId          = BUTTON_ID_NONE;

      m_pMenuCursor = NULL;
    }

    State                           m_state;

    u32                             m_initSeq;
    u32                             m_endSeq;

    INIT_PARAM                      m_initParam;

    app::tool::MenuCursor*          m_pMenuCursor;

    BUTTON_DATA                     m_aButtonData[ BUTTON_ID_MAX ];

    s32                             m_cursorPos;

    ButtonId                        m_activeSelectButtonId;
    ButtonId                        m_pushButtonId;

    static const ButtonId ms_aSelectButtonId[];
    static const ButtonId ms_aCursorPosRootButtonId[];
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_PLAYER_LOW_DATA_CTRL_VIEW_H_INCLUDED__