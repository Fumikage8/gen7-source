//=============================================================================
/**
 * @file	 ConfigFrame.h
 * @brief	 コンフィグ制御フレーム
 * @author yuto_uchida
 * @date	 2015.05.19
 */
//=============================================================================
#ifndef NIJI_PROJECT_CONFIG_FRAME_H_INCLUDED
#define NIJI_PROJECT_CONFIG_FRAME_H_INCLUDED
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

//  nijiのインクルード
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/app_tool_MessageMenuView.h"

#include "App/config/source/parts/ConfigLowerUIDraw.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( gfl2 )

GFL_NAMESPACE_BEGIN( str )

  class  MsgData;

GFL_NAMESPACE_END( str )

GFL_NAMESPACE_BEGIN( fs )

  class  AsyncFileManager;

GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )

GFL_NAMESPACE_BEGIN( tool )

  class  MenuWindow;

GFL_NAMESPACE_END( tool )

GFL_NAMESPACE_BEGIN( util )

  class  AppUtilFileControlState;
  class  AppRenderingManager;
  class  Heap;
  class  KeyRepeatController;

GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( Savedata )

  class ConfigSave;

GFL_NAMESPACE_END( Savedata )

GFL_NAMESPACE_BEGIN( print )

  class WordSet;

GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  //  前方宣言
  class ConfigUIViewPool;

  /** コンフィグ制御フレーム */
  class ConfigFrame : public 
  applib::frame::CellSuper, 
  app::ui::UIInputListener, 
  App::Tool::MessageMenuView::IEventHandler
  {
    // コピーを禁止
    GFL_FORBID_COPY_AND_ASSIGN(ConfigFrame);
  public:
    //  フレームの状態
    enum StateType
    {
      STATE_TYPE_NONE = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_OPEN,
      STATE_TYPE_UPDATE,
      STATE_TYPE_CHANGE_ITEM_DATA,
      STATE_TYPE_SCENE_BACK_EVENT,
      STATE_TYPE_ENTER,
      STATE_TYPE_CHANGE_DATA_YESNO_WINDOW,
      STATE_TYPE_CLOSE,
      STATE_TYPE_END,
    };

    //  フレームアプリ用のパラメータ
    struct APP_PARAM
    {
      /** 入力用パラメータ */
      struct _tag_in_param
      {
        _tag_in_param()
        {
          pParentAppHeap    = NULL;
          pParentDevHeap    = NULL;
          pRenderingManager = NULL;
          pConfigData       = NULL;
          pNormalMessage    = NULL;
          pKanaMessage      = NULL;
          pKanjiMessage     = NULL;
          pViewPool         = NULL;
        }

        //  フレーム内で使用する親ヒープ
        gfl2::heap::HeapBase *pParentAppHeap;
        gfl2::heap::HeapBase *pParentDevHeap;

        //  描画パイプライン
        app::util::AppRenderingManager* pRenderingManager;

        //  コンフィグデータアドレス
        Savedata::ConfigSave* pConfigData;

        //  デフォルトメッセージデータ
        gfl2::str::MsgData*                 pNormalMessage;

        //  オプションの仮名/漢字切り替えで表示している日本語を即時切替ために必要(しかしこれは日本語版限定)
        gfl2::str::MsgData*                 pKanaMessage;
        gfl2::str::MsgData*                 pKanjiMessage;

        //  viewプール
        ConfigUIViewPool* pViewPool;

      } in_param;

      struct _tag_out_
      {
        _tag_out_()
        {
          bMoziChange = false;
        }

        bool  bMoziChange;
      } out_param;

    };

    //------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     */
    //------------------------------------------------------------------
    ConfigFrame();

    //------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //------------------------------------------------------------------
    virtual ~ConfigFrame();

  public:

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ず最初に呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( APP_PARAM* pAppParam);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの初期化を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result InitFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result UpdateFunc();

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン描画を行う関数
     *          純粋仮想ではありません。
     */
    //------------------------------------------------------------------
    virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCの終了を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
     */
    //------------------------------------------------------------------
    virtual applib::frame::Result EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペイン選択アニメーションの開始を検知
    *       【ボタンマネージャー関係】
    *
    * @param  buttonId   通知を受けるペインＩＤ
    *
    * @return none
    */
    //--------------------------------------------------------------------------------------------
    virtual void OnLayoutPaneStartingAnimeEvent( const u32 buttonId );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインイベントの検知
    *       【ボタンマネージャー関係】
    *
    * @param  buttonId   通知を受けるペインＩＤ
    *
    * @return 同フレーム内での他入力イベントを許可するならtrue、
    *         他のイベントを排除するならfalseを返却すること。
    */
    //--------------------------------------------------------------------------------------------
    virtual ListenerResult OnLayoutPaneEvent( const u32 buttonId );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief ペインタッチ時を検知
    *       【ボタンマネージャー関係】
    *
    * @param  buttonId   通知を受けるペインＩＤ
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
    virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief  キー入力の検知
     *         ※タッチパネルの入力が優先される
     *
     * @param  pButton  ボタン ( A,B,X,Y,L,R... )
     * @param  pKey     十字キー
     * @param  pStick   アナログスティック
     *
     * @return 同フレーム内での他入力イベントを許可するならtrue、
     *         他のイベントを排除するならfalseを返却すること。
     */
    //--------------------------------------------------------------------------------------------
    virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    //  MessageMenuWindowのイベント
    virtual void  MessageMenuView_OnEvent(const App::Tool::MessageMenuView::IEventHandler::EventCode event);

  private:
    enum
    {
      //  このフレームと描画を動かすのに必要なheapサイズを定義
      //  450kbyte
      SIZE_MY_APP         = 1024 * 450,

      //  1Mbyte
      SIZE_MY_APPDEV      = 1024 * 1024 * 1,

      //  2Kbyte
      SIZE_FRAME_MANAGER  = 4096,
    };


    //  ファイル制御一覧
    enum
    {
      FILE_DATA_UI_LYT  = 0,
      FILE_DATA_MENU_WINDOW,

      FILE_DATA_MAX
    };

    //  汎用メニューの項目一覧
    enum
    {
      MENU_WINDOW_ITEM_ID_YES = 0,
      MENU_WINDOW_ITEM_ID_NO,

      MENU_WINDOW_ITEM_ID_MAX,
    };

    //  リスト項目データ変更イベント
    typedef struct
    {
      u32 list_idx;
      s32 select_move;

    } LIST_ITEM_CHANGE_DATA_EVENT;

    // 状態ステート設定
    void _SetState( const StateType newStateType );

    //  項目のデータ変更開始
    void _StartEventChangeItemData( const LIST_ITEM_CHANGE_DATA_EVENT& rData );

    //  ヘルプテキスト更新
    void _UpdateHelpText( const ConfigLowerUIDraw::LIST_ITEM_PARAM& rCurrentListItemParam );

    //  コンフィグデータを変更しているかどうか
    bool _IsChangeConfigSaveData();

    //------------------------------------------------------------------
    /**
     * @brief	  コンフィグデータに書き込み
     * @return	書き込みした -> true / 書き込みしていない -> false
     */
    //------------------------------------------------------------------
    bool _WriteConfigSaveData();

    //------------------------------------------------------------------
    /**
      * @brief ボタンidからリスト項目切り替えのイベントデータを出力
      * @param pOutputData  : 出力データのアドレス
      * @param buttonId     : 出力対象のリストアイテムのボタンid
      * @return 成功 true / 失敗時 false
    */
    //------------------------------------------------------------------
    bool _OutputChangeListItemDataFromButtonid( LIST_ITEM_CHANGE_DATA_EVENT* pOutputData, const u32 buttonId );

    //  変数クリア
    void _Clear()
    {
      m_state                     = STATE_TYPE_NONE;
      m_subState                  = 0;

      m_pAppHeap                  = NULL;
      m_pManagerHeapBase          = NULL;
      m_pAppParam                 = NULL;
      m_pUseMsgData               = NULL;
      m_pKeyRepeatCtrl            = NULL;

      m_bDataChange               = false;

      m_lowerSystemMessageEventCode = App::Tool::MessageMenuView::IEventHandler::EVENT_;

      gfl2::std::MemClear( &m_listItemChangeDataEvent, sizeof( m_listItemChangeDataEvent ) );
    }

    //  変数宣言一覧

    APP_PARAM*                          m_pAppParam;
    app::util::Heap*                    m_pAppHeap;
    gfl2::heap::HeapBase*               m_pManagerHeapBase;

    //  使用するメッセージデータ
    gfl2::str::MsgData*                 m_pUseMsgData;

    StateType                           m_state;
    u32                                 m_subState;

    //  データ変更しているかどうか
    bool                                m_bDataChange;
    LIST_ITEM_CHANGE_DATA_EVENT         m_listItemChangeDataEvent;

    app::util::KeyRepeatController*     m_pKeyRepeatCtrl;

    App::Tool::MessageMenuView::IEventHandler::EventCode m_lowerSystemMessageEventCode;
};

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )


#endif // NIJI_PROJECT_CONFIG_FRAME_H_INCLUDED
