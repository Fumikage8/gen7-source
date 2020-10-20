//======================================================================
/**
 * @file BattleUIRecPlayLowerView.h
 * @date 2015/10/26 12:47:37
 * @author uchida_yuto
 * @brief バトル録画再生の下画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_UI_REC_PLAY_LOWER_VIEW_H_INCLUDED__
#define __BATTLE_UI_REC_PLAY_LOWER_VIEW_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Util/AppUtilFileControlState.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
  class CommonMessageWindowView;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  /**
   * @class BattleUIRecPlayLowerView
   *  ファイルロードは内部で、しかしメッセージ関連の設定は外側で
   */
  class BattleUIRecPlayLowerView
  : public ::app::ui::UIView
  , public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN(BattleUIRecPlayLowerView);
  public:
    enum ButtonId
    {
      BUTTON_ID_CANCEL  = 0,
      BUTTON_ID_ADVANCE,
      BUTTON_ID_RETURN,
      BUTTON_ID_MAX,
      BUTTON_ID_NONE,
    };

    //  初期化パラメータ(事前に必要なデータ)
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pUtilHeap         = NULL;
        pRenderingManager = NULL;
        pMsgData          = NULL;
        pWordSet          = NULL;
      }

      //  描画用ヒープ
      ::app::util::Heap* pUtilHeap;

      ::app::util::AppRenderingManager* pRenderingManager;

      //  GARC_message_btl_app_DATのメッセージデータを渡す
      gfl2::str::MsgData*   pMsgData;
      print::WordSet*       pWordSet;
    };

    //  セットアップパラメータ(InitFunc()で必要なデータ)
    struct SETUP_PARAM
    {
      SETUP_PARAM()
      {
        pMenuCursorLytBuffer  = NULL;
      }

      void*                     pMenuCursorLytBuffer;
    };

    BattleUIRecPlayLowerView( const INIT_PARAM& param );
    virtual ~BattleUIRecPlayLowerView();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期化処理
    * @return     true 処理終了 / false 処理が終了していないので再度呼び出す
    * @note
    *         返り値が true になるまで呼び出す
    */
    //--------------------------------------------------------------------------------------------
    bool InitFunc( const SETUP_PARAM& rSetupParam );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  終了処理
    * @return true 処理終了 / false 処理が終了していないので再度呼び出す
    * @note
    *         返り値が true になるまで
    *         この関数を呼び出すとInitFunc()で初期化しないと使えないので注意
    *         使うケースとしては画面をもう使わないときの後処理に使う
    */
    //--------------------------------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  画面を開く
    */
    //--------------------------------------------------------------------------------------------
    void Open();
    bool IsOpen() const { return m_bOpen; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  画面を閉じる
    * @note
    *         画面を非表示にする時にはこれを使う
    *         SetVisible関数で非表示にしても3Dモデル表示は非表示にできないから
    */
    //--------------------------------------------------------------------------------------------
    void Close();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

    //  押したボタンidを返す
    //  m_updateSeq == UPDATE_SEQ_PAUSE の時のみ押したボタンidを返すようにしている
    ButtonId GetPushButtonId();

    //  押したボタンをクリアする
    void ClearPushButtonId() { m_pushButtonId = BUTTON_ID_NONE; }

    //  チャプター最大数を取得
    void SetChapterMax( const u16 maxChapter );

    //  スキップモードを有効にする
    void SetEnableSkipMode( const bool bFlg );

    //  終了モードを有効にする
    void SetEnableQuitMode();

    //  現在のチャプター数を設定
    void SetChapterNum( const u16 currentChapter );

    //  スキップ用のカラー設定するか
    void SetChapterNumSkipColor( const bool bFlg );

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

  protected:
    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum
    {
      LYT_WORK_NORMAL,
      LYT_WORK_MAX,
    };

    enum
    {
      MENU_CURSOR_POS_MAX = 2,
    };

    enum UpdateSeq
    {
      UPDATE_SEQ_IDLE = 0,
      UPDATE_SEQ_INPUT_ANIME,
      UPDATE_SEQ_PAUSE,
      UPDATE_SEQ_SKIP_VIEW,
      UPDATE_SEQ_QUIT_VIEW,
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  2D初期化処理
    * @note
    *     レイアウトデータから描画に必要な生成をしている
    */
    //--------------------------------------------------------------------------------------------
    void _Init2D();

    //  更新シーケンス設定
    void _SetUpdateSeq( const UpdateSeq newSeq );

    //  カーソル移動
    //  カーソル移動したら true
    void _SetCursorMove( const s32 move );

    //  カーソル位置設定
    bool _SetCursorPos( const u32 pos );

    //  カーソル位置設定(ボタンid版)
    void _SetCursorButtonId( const ButtonId buttonId );

    //  決定
    void _PushEntry();

    //  数字の桁数取得
    u32 _GetNumberKeta( const u32 num );

    void _Clear()
    {
      m_pFileCtrlState  = NULL;
      m_pMenuCursor     = NULL;

      m_initSeq       = 0;
      m_endSeq        = 0;
      m_updateSeq     = UPDATE_SEQ_IDLE;
      m_bInit         = false;
      m_bOpen         = false;
      m_bSkipMode     = false;
      m_menuCursorPos = 0;
      m_maxChapter    = 0;

      m_pushButtonId  = m_tmpPushButtonId = BUTTON_ID_NONE;

      m_pCurrentChapterNumTexBox  = NULL;
      m_pChapterNumSlashTexBox    = NULL;
      m_pMaxChapterNumTexBox      = NULL;
      m_pCommonMessageWindowView  = NULL;
      m_pCommonMessageWindowFileCtrlState = NULL;

      {
        u32 loopNum = GFL_NELEMS( m_aMenuCursorPosData );
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_aMenuCursorPosData[ i ].pPosPane  = NULL;
          m_aMenuCursorPosData[ i ].buttonId  = BUTTON_ID_NONE;
        }
      }
    }

    //  カーソル位置データ
    struct MENU_CURSOR_POS_DATA
    {
      gfl2::lyt::LytPane* pPosPane;
      ButtonId            buttonId;
    };

    //  共通パラメータのペイン一覧
    u32         m_initSeq;
    u32         m_endSeq;
    UpdateSeq   m_updateSeq;

    u16         m_maxChapter;

    bool  m_bInit;
    bool  m_bOpen;
    bool  m_bSkipMode;

    INIT_PARAM  m_initParam;
    SETUP_PARAM m_setupParam;

    ButtonId    m_pushButtonId;
    ButtonId    m_tmpPushButtonId;

    gfl2::lyt::LytTextBox*  m_pCurrentChapterNumTexBox;
    gfl2::lyt::LytTextBox*  m_pChapterNumSlashTexBox;
    gfl2::lyt::LytTextBox*  m_pMaxChapterNumTexBox;

    ::app::tool::MenuCursor*              m_pMenuCursor;
    u32                                   m_menuCursorPos;
    MENU_CURSOR_POS_DATA                  m_aMenuCursorPosData[ MENU_CURSOR_POS_MAX ];

    //  共通メッセージ
    App::Tool::CommonMessageWindowView*   m_pCommonMessageWindowView;

    gfl2::lyt::ColorU8                    m_defaultCurrentChapterNumTexBoxTopColor;
    gfl2::lyt::ColorU8                    m_defaultCurrentChapterNumTexBoxBottomColor;

    //  ロードファイル
    ::app::util::AppUtilFileControlState* m_pFileCtrlState;
    ::app::util::AppUtilFileControlState* m_pCommonMessageWindowFileCtrlState;
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_REC_PLAY_LOWER_VIEW_H_INCLUDED__