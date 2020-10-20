#ifndef NIJI_PROJECT_APP_POKELIST_WINDOW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_WINDOW_H_INCLUDED
//==============================================================================
/**
 * @file    PokeListWindow.h
 * @author  fukushima_yuya
 * @date    2015.04.24
 * @brief   ポケモンリストのウィンドウ
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListConst.h"

#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>

// gflib2
#include <str/include/gfl2_Str.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;   //!< 共通描画クラス
class MenuWindowDraw; //!< メニューウィンドウクラス
class CursorDraw;     //!< カーソルクラス
class PokeListDrawListener;   //!< UIイベントリスナー
class ItemExchange;   //!< 道具入れ替え
class MessageWindow;  //!< 下部メッセージウィンドウ
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   メッセージウィンドウ
//------------------------------------------------------------------------------
class MessageWindow
{
  GFL_FORBID_COPY_AND_ASSIGN( MessageWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  MessageWindow( DrawerBase* pBase );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~MessageWindow( void ) { ; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウを開く
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウのメッセージを設定する
   * @param   msgID
   */
  //------------------------------------------------------------------
  void SetMessage( s32 msgID );


private:
  DrawerBase*   m_pBase;

  bool    m_isOpen;

  bool    m_FieldBtnVis;
  
  bool    m_JoinBtnVis;

  gfl2::lyt::LytParts*    m_pMsgParts;

  gfl2::lyt::LytPane*     m_pFieldBtnPane;

  gfl2::lyt::LytPane*     m_pJoinBtnPane;

  gfl2::lyt::LytTextBox*  m_pMsgText;
};



//------------------------------------------------------------------------------
// @brief   ウィンドウクラス
// @note    MenuWindowなどの管理クラス
//------------------------------------------------------------------------------
class PokeListWindow
  : public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListWindow );

public:
  // @brief   メッセージタイプ
  enum MessageType {
    MSG_TYPE_MSG_ONLY,      //!< メッセージのみ
    MSG_TYPE_MSG_NAME,      //!< 名前＋メッセージ
    MSG_TYPE_MSG_NAME_ITEM, //!< 名前＋道具名＋メッセージ
  };
  
  // @brief   ウィンドウのアニメタイプ
  enum WndAnimType
  {
    WND_ANIM_ALL_IN,
    WND_ANIM_ALL_OUT,
    WND_ANIM_OTHER_IN,
    WND_ANIM_OTHER_OUT,
    WND_ANIM_NUM,
  };

  // @brief   アイテムを預けた結果
  enum ITEM_STORE_RESULT
  {
    RESULT_STORE_OK,        //!< 道具を預けた：通常アイテム
    RESULT_HAVE_NONE,       //!< 所持していない
    RESULT_BAG_FULL,        //!< バッグがいっぱい
    RESULT_STORE_OK_ZITEM,  //!< 道具を預けた：ゼンリョクアイテム
    RESULT_NUM
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListWindow( DrawerBase* pBase, PlateDraw** ppPokePlate, CursorDraw* pCursor, app::util::AppRenderingManager* pRenderManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListWindow( void ) { ; }


public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  bool Terminate( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの生成
   */
  //------------------------------------------------------------------
  void CreateMenuWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの破棄
   */
  //------------------------------------------------------------------
  bool DeleteMenuWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウの生成
   */
  //------------------------------------------------------------------
  void CreateMessageWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウの破棄
   */
  //------------------------------------------------------------------
  void DeleteMessageWindow( void );

  //------------------------------------------------------------------
  /**
   * @brief   アイテム欄の初期化
   */
  //------------------------------------------------------------------
  void InitializePane_Item( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   *
   * @return  "true  == 完了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsReady( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンのセット
   *
   * @param   itemNo    アイテムNo
   */
  //------------------------------------------------------------------
  void SetItemIcon( u16 itemNo );


public:
  //------------------------------------------------------------------
  /**
   * @brief   セットアップ(フィールド用)
   * @param   pItemExchange   道具入れ替えクラス
   * @param   pMsgWnd         下部メッセージウィンドウクラス
   */
  //------------------------------------------------------------------
  void SetupModeField( ItemExchange* pItemExchange, MessageWindow* pMsgWnd );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   上部に表示するメッセージの設定
   * @param   msgType   メッセージのタイプ
   * @param   msgID     表示するメッセージのID
   * @param   cpos      プレート位置
   */
  //------------------------------------------------------------------
  void SetMessage( MessageType msgType, s32 msgID, CursorPos cpos = CPOS_NULL );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンニックネームの設定
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokeNickName( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   アイテム名の設定
   *
   * @param   msgType   メッセージの種類
   * @param   pp        ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetItemName( PokeListWindow::MessageType msgType, const pml::pokepara::PokemonParam* pp );


public:
  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウを開く(全て)
   * @param   menuID    メニューウィンドウID
   * @param   item_out  アイテム欄の強制非表示フラグ(デフォルトfalse)
   */
  //------------------------------------------------------------------
  void Open( MenuID menuID, bool item_out = false );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウを開く(メニューウィンドウ＋アイテム欄)
   * @param   menuID    メニューウィンドウID
   * @param   item_out  アイテム欄の強制非表示フラグ(デフォルトfalse)
   */
  //------------------------------------------------------------------
  void OpenOther( MenuID menuID, bool item_out = false );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージのみのウィンドウをを開く
   */
  //------------------------------------------------------------------
  void OpenMsgOnly( void );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる(全て)
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる(メニューウィンドウ＋アイテム欄)
   */
  //------------------------------------------------------------------
  void CloseOther( void );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウを閉じる(メニューウィンドウ＋アイテム欄)
   */
  //------------------------------------------------------------------
  void CloseWindow_MenuAndItem( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージのみのウィンドウを閉じる
   */
  //------------------------------------------------------------------
  void CloseMsgOnly( void );

  //------------------------------------------------------------------
  /**
   * @brief   開く・閉じるのアニメーションの終了確認
   * @param   anim_type   アニメーションタイプ
   */
  //------------------------------------------------------------------
  bool IsAnimEnd( WndAnimType anim_type );

  //------------------------------------------------------------------
  /**
   * @brief   UIイベントリスナーのセット
   * @param   pListener   UIイベントリスナー
   */
  //------------------------------------------------------------------
  void SetUIListener( PokeListDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   入力待ちフラグの取得
   * @return  
   */
  //------------------------------------------------------------------
  bool GetInputWait( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの設定
   * @param   enable    入力設定やカーソルの表示などのフラグ
   */
  //------------------------------------------------------------------
  void SetPokeListEnable( bool enable );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を有効に
   */
  //------------------------------------------------------------------
  void SetPokeListButtonEnable( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を無効に
   */
  //------------------------------------------------------------------
  void SetPokeListButtonDisable( void );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムを預けられるかの結果を取得
   *
   * @param   cpos    対象のプレート位置
   *
   * @return  RESULT_STORE_OK   = 道具を預けた
   *          RESULT_HAVE_NONE  = 道具を所持していない
   *          RESULT_BAG_FULL   = バッグがいっぱい
   */
  //------------------------------------------------------------------
  ITEM_STORE_RESULT GetItemStoreResult( CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムを預ける
   *
   * @param   cpos    対象のプレート位置
   * @param   res     アイテムを預けた結果
   */
  //------------------------------------------------------------------
  void ItemStore( CursorPos cpos, ITEM_STORE_RESULT res );

  //------------------------------------------------------------------
  /**
   * @brief   対象選択が必要なフィールド技か？
   * @param   wazaNo    技番号
   * @return  true = 対象選択が必要なフィールド技、false = それ以外
   */
  //------------------------------------------------------------------
  bool IsPokeSelectFieldWaza( WazaNo wazaNo );


public:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージのみのウィンドウをを開く
   */
  //------------------------------------------------------------------
  void OpenMsgOnly( MessageType msgType, s32 msgID, CursorPos cpos = CPOS_NULL, bool disableContinue = false );

  // ボタンの無効化リクエスト
  void RequestButtonDisable( void );    // @fix GFNMCat[5661]
  // 画面状態の変更リクエスト
  void RequestSetWindowMode( void );    // @fix GFNMCat[5661]

public:
  bool IsWindowIdle( void ) const { return (m_Seq == 0); }

  // タイマーアイコンの再生
  void StartTimeIcon( void );


private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの選択
   * @param   選択された項目ID
   */
  //------------------------------------------------------------------
  virtual void SelectMenu( u32 id );


private:
  app::util::AppRenderingManager* m_pRenderingManager;
private:
  DrawerBase*     m_pBase;

  MenuWindowDraw*   m_pMenuWindow;

  PlateDraw**       m_ppPokePlate;

  CursorDraw*       m_pCursor;

  ItemExchange*     m_pItemExchange;      //!< 道具入れ替え

  MessageWindow*    m_pLowMsgWnd;         //!< 下部メッセージウィンドウ

  PokeListDrawListener*   m_uiListener;   //!< UIイベントリスナー

  u8    m_Seq;      //!< シーケンス

  u32   m_MenuCPos; //!< メニューウィンドウのカーソル位置

  bool  m_isOpen;   //!< メッセージ＋半透明板が開いているかどうか

  bool  m_InputWait;    //!< 入力待ちフラグ(trueの時に入力が行われたらウィンドウを閉じる)

  bool  m_isFormChange; //!< 道具を預けた際に行うフォルムチェンジの開始フラグ

  bool  m_BtnDisableFlag;    //!< ボタンの入力変更を行ったかどうかのフラグ

  bool  m_BtnEnable[BTN_ID_NUM];

  bool  m_DisableContinue;

private:    //!< ペイン関連
  gfl2::lyt::LytParts*    m_pCmdSelectParts;
  gfl2::lyt::LytPane*     m_pItemNamePane;
  gfl2::lyt::LytTextBox*  m_pItemNameText;
  gfl2::lyt::LytTextBox*  m_pMsg00TextBox2;
  gfl2::lyt::LytPicture*  m_pItemIconPicture;
  gfl2::lyt::LytPane*     m_pPaneNullText1;
  gfl2::lyt::LytParts*    m_pMsgWindowParts;

private:
  u16     m_PrevItemNo;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_POKELIST_WINDOW_H_INCLUDED
