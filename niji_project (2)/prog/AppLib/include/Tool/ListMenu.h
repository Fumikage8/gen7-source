//============================================================================================
/**
 * @file		ListMenu.h
 * @brief		リストメニュー処理
 * @author	Hiroyuki Nakamura  => 移植k.ohno
 * @date		12.07.05
 */
//============================================================================================
#if !defined( __APP_TOOL_LISTMENU_H__ )
#define __APP_TOOL_LISTMENU_H__
#pragma once

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::tool
 * @brief			アプリツール
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )

//--------------------------------------------------------------------------------------------
/**
 * @brief	リストメニュー
 */
//--------------------------------------------------------------------------------------------
class ListMenu : public app::ui::UIView, public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( ListMenu );

public:

  static const int SIZE_LISTMENU_APP = 0x20000;       ///  アプリメモリも容量管理のため分けて管理
  static const int SIZE_LISTMENU_APPDEV = 0x20000;    /// アプリメモリも容量管理のため分けて管理


  static const u32 SELECT_NONE = 0;	//!< 選んでない
  static const u32 SELECT_CANCEL = 0xfffffffe;	//!< キャンセル
  static const u32 SELECT_SUCCESS = 0xffffffff;	//!< 選んだ

  static const u32 RESID_NONE = 0xffffffff;			//!< リソース外部設定

  static const u32 ITEMLIST_MAX = 128;			//!< リスト最大

  //! 配置位置定義
  enum {
    LIST_POS_CENTER = 0,							//!< 中央

    LIST_POS_LEFT,										//!< 左
    LIST_POS_RIGHT,										//!< 右

    LIST_POS_TOP = LIST_POS_CENTER+1,	//!< 上
    LIST_POS_BOTTOM,									//!< 下
  };

  static const u32 B_BUTTON_MODE_NONE   = 0xfffffffe;		//!< Bボタンモード：処理なし
  static const u32 B_BUTTON_MODE_CANCEL = 0xffffffff;		//!< Bボタンモード：キャンセル

  //! ウィンドウタイプ
  enum {
    WIN_TYPE_WHITE = 0,		//!< 白
    WIN_TYPE_BLACK,				//!< 黒
  };

  enum HorizontalPos{
    HORIZONTALPOS_CENTER,// 	中央を基準にします。
    HORIZONTALPOS_LEFT,// 	左を基準にします。
    HORIZONTALPOS_RIGHT,// 	右を基準にします。
    HORIZONTALPOS_MAX,// 	列挙子の総数です。

  };


  //--------------------------------------------------------------------------------------------
  /**
   * @brief	初期化パラメータ
   */
  //--------------------------------------------------------------------------------------------
  struct PARAM{
    app::util::Heap* pHeap;   //heap
    app::util::AppRenderingManager* pAppRenderingManager;   //レンダリングマネージャー

    gfl2::fs::AsyncFileManager* pFileManager;

    void* lytRes;  //外からリソースをわたしてもらう場合

    u8	initPos;			//!< 初期位置

    u8	listMax;			//!< 項目最大数

    u8	posTypeX;			//!< X座標基準位置
    u8	posTypeY;			//!< Y座標基準位置

    f32	x;						//!< 基準X座標
    f32	y;						//!< 基準Y座標

    HorizontalPos strPosX;

    //		nw::lyt::HorizontalPosition strPosX;		//!< 文字表示基準位置

    u32	startWait;		//!< 処理開始までのウェイト

    u32 bMode;				//!< Bボタン処理モード

    bool visible;			//!< 表示フラグ
    bool autoDraw;		//!< レイアウト自動描画フラグ

    u32	winType;			//!< ウィンドウタイプ

  };


public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief		コンストラクタ
   *
   * @param		初期化パラメータ
   */
  //--------------------------------------------------------------------------------------------
  ListMenu( const PARAM * prm );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  ~ListMenu();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		スタート
   */
  //--------------------------------------------------------------------------------------------
  void Start(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		アップデート
   *
   * @param		none
   *
   * @return	処理結果
   */
  //--------------------------------------------------------------------------------------------
  virtual void Update(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		描画
   *
   * @param		none
   *
   * @return	none
   *
   *	オート描画の場合は必要なし
   */
  //--------------------------------------------------------------------------------------------
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		項目追加
   *
   * @param		id		決定時の戻り値
   * @param		str		文字列
   *
   * @return	none
   */
  //--------------------------------------------------------------------------------------------
  void AddItem( u32 id, gfl2::str::StrBuf * str );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief		レイアウトの表示プライオリティ設定
   *
   * @param		priority		プライオリティ
   *
   * @return	none
   *
   *	オート描画時のみ有効な設定
   */
  //--------------------------------------------------------------------------------------------
  void SetPriority( u32 priority );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		メニュー表示フラグ設定
   *
   * @param		flg		true = 表示, false = 非表示
   *
   * @return	none
   */
  //--------------------------------------------------------------------------------------------
  void SetVisible( bool flg );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		現在選択している項目のIdを取得
   *
   * @return	現在選択している項目のId  キャンセルした場合SELECT_CANCEL
   */
  //--------------------------------------------------------------------------------------------
  u32 GetSelectId( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		削除 いつでもよんでよい
   *
   * @param		none
   *
   * @return	none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteMenu(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		インスタンスを消してよいかどうか必ず確認すること
   *
   * @param		none
   *
   * @return	消して良い場合true
   */
  //--------------------------------------------------------------------------------------------
  bool IsInstanceDelete(void);

private:

  virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  void LoadResource(void);

  bool CheckLoadResource(void);

  bool CancelLoadResource(void);

  void CreateLayout(void);

  void RemoveLayout(void);

  u32 Main(void);

  bool MoveCursor( int mv, bool is_trg );

  bool MoveCursorDirect( u32 pos );

  void Put( u32 pos );

  void PutWindowPos(void);

  void SetSize(void);

  int GetTextPaneWidth(void);

  void PutString( u32 pane, const gfl2::str::StrBuf * str );

  void SetTextPane( u32 max );

  u32 ScrollPane( int mv );

  void PutCursor(void);

  void PutArrow(void);

  void SetVisiblePane( const gfl2::lyt::LytPaneIndex index, bool flg );

  void SetAnime( u32 id );

  void StopAnime( u32 id );

  bool CheckAnime( u32 id );

  void fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr);
  bool fileReadBlock( app::util::FileAccessor::RequestData* pfrs, gfl2::fs::AsyncFileManager* pAsyncFileMan );


private:
  enum {
    _OPEN,
    _OPENING,
    _LOAD,
    _CLOSE,
    _END,
  };


  enum {
    SEQ_START,
    SEQ_CHECK,
    SEQ_MAIN,
    SEQ_SELECT,
    SEQ_ENDSETUP,
    SEQ_ENDCHK,
    SEQ_END,
  };

  
private:
  PARAM	m_param;	//!< パラメータ

  void * m_pResBuff;												//!< バッファ

  bool m_LoadFlag;												//!< リソース読み込みフラグ

  print::LYT_TEXT_EX_WORK* mLytTextWork;  //!<テキスト表示用ワーク

  void* m_pArcReadBuff;

  app::util::FileAccessor::RequestData m_frs;

  int m_loadseq;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief	メニュー項目データ
   */
  //--------------------------------------------------------------------------------------------
  typedef struct {
    u32 id;										//!< 決定時の戻り値
    gfl2::str::StrBuf * str;		//!< 文字列
  }ITEM;

  ITEM * m_itemArray[ITEMLIST_MAX];			//!< メニュー項目

  int m_curPos;				//!< カーソル位置
  int m_scroll;				//!< スクロール値
  int m_addCount;			//!< メニュー項目追加数

  int	m_paneCount;		//!< 最上部に表示されているテキストペインのカウンタ

  s16 m_mainseq;					//!< シーケンス
  s16	m_cancelSeq;		//!< 終了シーケンス
  int m_Sequence;

};


GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

#endif	// __APP_TOOL_LISTMENU_H__
