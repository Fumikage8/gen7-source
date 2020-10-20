#ifndef NIJI_PROJECT_APP_POKELIST_ITEMEXCHANGE_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_ITEMEXCHANGE_H_INCLUDED
//==============================================================================
/**
 * @file    PokeListItemExchange.h
 * @author  fukushima_yuya
 * @date    2015.03.30
 * @brief   アイテムの入れ替え
 */
//==============================================================================

// niji 
#include <PokeTool/include/PokeTool.h>

// gflib2
#include <macro/include/gfl2_Macros.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;   //!< 描画共通処理
class CursorDraw;     //!< カーソル
class PlateDraw;      //!< プレート
class PlatePosition;  //!< プレート位置
class MessageWindow;  //!< メッセージウィンドウ
class PlateMessageWindow;
class PokeListWindow;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_BEGIN(Tool)
class ItemEffect;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   フォルムチェンジ
//------------------------------------------------------------------------------
class FormChange
{
  GFL_FORBID_COPY_AND_ASSIGN( FormChange );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FormChange( DrawerBase* pBase, PlateDraw** ppPlate, PokeListWindow* pWindow, u8 memberCount, App::Tool::ItemEffect* itemEffect );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~FormChange( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   破棄処理
   *
   * @return  "true  == 破棄完了"
   * @return  "false == 破棄中"
   */
  //------------------------------------------------------------------
  bool DeleteFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   * @return  true = 継続、false = 終了
   */
  //------------------------------------------------------------------
  bool Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの開始
   *
   * @param   once    １回だけの処理かどうか
   */
  //------------------------------------------------------------------
  void Start( bool once = false );

  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの終了
   */
  //------------------------------------------------------------------
  void End( void );

  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの対象の追加
   * @param   cpos  プレート位置
   */
  //------------------------------------------------------------------
  void AddFormChangeTarget( CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   入力フラグのセット
   * @param   flag    true = 入力あり、false = 入力なし
   */
  //------------------------------------------------------------------
  void SetInputFlag( bool input );

  //------------------------------------------------------------------
  /**
   * @brief   アクティブなプレート位置
   * @return  対象のプレート位置
   */
  //------------------------------------------------------------------
  CursorPos GetActCursorPos( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジのチェック
   * @return  true = フォルムチェンジあり、false = フォルムチェンジなし
   */
  //------------------------------------------------------------------
  bool CheckFormChange( void );
  bool _CheckFormChange( CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   エフェクトの有無のチェック
   * @return  true = あり、false = なし
   */
  //------------------------------------------------------------------
  bool CheckEffect( void );

  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジ処理
   */
  //------------------------------------------------------------------
  void FormChangeProc( void );

  //------------------------------------------------------------------
  /**
   * @brief   道具名欄の一括処理
   * @brief   in    true = フェードイン、false = フェードアウト
   */
  //------------------------------------------------------------------
  void ItemNameFade( bool in );

  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウの入力有効設定
   * @param   enable    true = 有効、false = 無効
   */
  //------------------------------------------------------------------
  void SetWindowEnable( bool enable );

private:
  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジ
   *
   * @param   pp    ポケパラム
   * @param   form  フォルム
   */
  //------------------------------------------------------------------
  void _ChangeForm( pml::pokepara::PokemonParam* pp, pml::FormNo form );

  //------------------------------------------------------------------
  /**
   * @brief   フォルム設定
   *
   * @param   pp    ポケパラム
   * @param   form  フォルム
   */
  //------------------------------------------------------------------
  void _SetFormNo( pml::pokepara::PokemonParam* pp, pml::FormNo form );

private:
  //------------------------------------------------------------------
  /**
   * @brief   エフェクトの生成
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CreateEffect( void );

  //------------------------------------------------------------------
  /**
   * @brief   エフェクトの破棄
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool DeleteEffect( void );

  //------------------------------------------------------------------
  /**
   * @brief   アイコンの差し替えが可能か？
   *
   * @return  "true  = 可"
   * @return  "false = 不可"
   */
  //------------------------------------------------------------------
  bool IsReplaceIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコン座標のセット
   */
  //------------------------------------------------------------------
  void SetPokeIconPos( void );

  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの演出をスキップ
   *
   * @param   pp    ポケモンパラメータ
   * @param   form  変更フォルムNo
   */
  //------------------------------------------------------------------
  bool IsSkipFormChangePerformance( pml::pokepara::PokemonParam* pp, pml::FormNo form );

private:
  //------------------------------------------------------------------
  /**
   * @brief   泣き声の再生
   */
  //------------------------------------------------------------------
  void PlayPokeVoice( void );


private:
  DrawerBase*                 m_pBase;

  PlateDraw**                 m_ppPlate;        //!< プレート(アイコンの差し替え)

  PokeListWindow*             m_pWindow;        //!< ウィンドウ

  App::Tool::ItemEffect*      m_pItemEffect;    //!< フォルムチェンジエフェクト

  gfl2::math::VEC3            m_pPokeIconPos[pml::PokeParty::MAX_MEMBERS];

  PokeTool::SimpleParam       m_SimpleParam;

private:
  CursorPos       m_CursorPos[2];

  CursorPos       m_ActCPos;

  pml::FormNo     m_FormNo;

  u16             m_ItemNo;

private:
  u8    m_MemberCount;

  u8    m_Seq;

  u8    m_AddCnt;

  u8    m_TargetCount;

  u8    m_LoopCnt;

  bool  m_isEffect;

  bool  m_isInput;

  bool  m_isOnce;

private:
  u8    m_CreateEffectSeq;

  u8    m_DeleteEffectSeq;
};


//------------------------------------------------------------------------------
// @brief   道具アイコンのドラッグ処理
//------------------------------------------------------------------------------
class ItemIconDrag
{
  GFL_FORBID_COPY_AND_ASSIGN( ItemIconDrag );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  ItemIconDrag( DrawerBase* pBase, CursorDraw* pCursor );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ItemIconDrag( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ドラッグの開始
   * @param   movePos   選択したプレート位置
   * @param   itemNo    道具No
   */
  //------------------------------------------------------------------
  void StartDrag( CursorPos movePos, u16 itemNo );

  //------------------------------------------------------------------
  /**
   * @brief   ドラッグ処理
   */
  //------------------------------------------------------------------
  void UpdateDrag( gfl2::math::Vector3& tp );

  //------------------------------------------------------------------
  /**
   * @brief   ドラッグの終了
   */
  //------------------------------------------------------------------
  void EndDrag( CursorPos destPos, u16 moveItemNo );

private:
  DrawerBase*   m_pBase;

  CursorDraw*     m_pCursor;

  CursorPos   m_MovePos;

  gfl2::lyt::LytParts*    m_pIconParts;

  gfl2::lyt::LytPicture*  m_pIconPicture;

  u32   m_CatchAnim;

  u32   m_ReleaseAnim;
};


//------------------------------------------------------------------------------
// @brief   道具入れ替えクラス
//------------------------------------------------------------------------------
class ItemExchange
{
  GFL_FORBID_COPY_AND_ASSIGN( ItemExchange );

public:
  //! @brief  並べ替えシーケンス
  enum ItemExchangeSequence
  {
    SEQ_ITEM_NONE,                //!< 他のモード中
    SEQ_ITEM_MOVE_SELECT,         //!< 道具入れ替え元の選択
    SEQ_ITEM_DEST_SELECT,         //!< 道具入れ替え先の選択
    SEQ_ITEM_EXCHANGE,            //!< 道具の入れ替えの開始
    SEQ_ITEM_EXCHANGE_CLOSE,      //!< ウィンドウのフェードアウト
    SEQ_ITEM_EXCHANGE_OPEN,       //!< ウィンドウのフェードイン
    SEQ_ITEM_EXCHANGE_END,        //!< 道具入れ替えの終了
    SEQ_ITEM_FORMCHANGE_BEGIN,    //!< フォルムチェンジ開始
    SEQ_ITEM_FORMCHANGE_END,      //!< フォルムチェンジ終了
    SEQ_ITEM_NUM
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pBase         DrawerBaseクラスポインタ
   * @param   ppPokePlate   PlateDrawクラスポインタ配列
   * @param   pCursor       CursorDrawクラスポインタ
   * @param   pMsgWindow    MessageWindowクラスポインタ
   * @param   memberCount   メンバー数
   */
  //------------------------------------------------------------------
  ItemExchange( DrawerBase* pBase, PlateDraw** ppPokePlate,
    CursorDraw* pCursor, MessageWindow* pMsgWindow, u8 memberCount );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ItemExchange( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   道具の入れ替えの開始
   * @param   loop    連続処理フラグ(デフォルト = true)
   */
  //------------------------------------------------------------------
  void StartExchange( bool loop = true );

  //------------------------------------------------------------------
  /**
   * @brief   道具の入れ替えの終了
   */
  //------------------------------------------------------------------
  void EndExchange( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタン選択処理
   * @param   cpos    カーソル位置
   */
  //------------------------------------------------------------------
  void Decide( CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセル処理
   * @param   true = 入れ替えの終了、false = 入れ替え継続中
   */
  //------------------------------------------------------------------
  bool Cancel( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の更新
   */
  //------------------------------------------------------------------
  void UpdateCursorPos( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   移動元のセット
   * @param   cpos    選択したプレート位置
   * @param   isDrag  ドラッグでの入れ替えかどうか(デフォルトは選択入れ替え)
   * @return  "true  = セット完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool SetMovePos( CursorPos cpos, bool isDrag = false );

  //------------------------------------------------------------------
  /**
   * @brief   移動先のセット
   * @param   cpos    選択したプレート位置
   */
  //------------------------------------------------------------------
  void SetDestPos( CursorPos cpos );

private:
  //------------------------------------------------------------------
  /**
   * @brief   データの初期化
   */
  //------------------------------------------------------------------
  void Reset( void );

  //------------------------------------------------------------------
  /**
   * @brief   プレートが選択できるかどうか
   * @return  true = 選択可、false = 選択不可
   */
  //------------------------------------------------------------------
  bool IsSelectPlate( CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモン入れ替え
   */
  //------------------------------------------------------------------
  void Exchange( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの入力の有効設定の変更
   * @param   enable    有効フラグ(true = 有効、false = 無効)
   */
  //------------------------------------------------------------------
  void SetPokeListEnable( bool enable );


public:
  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジのセット
   * @param   pFormChange   FormChangeクラスポインタ
   */
  //------------------------------------------------------------------
  void SetFormChange( FormChange* pFormChange );

  //------------------------------------------------------------------
  /**
   * @brief   フォルムチェンジの開始
   * @param   cpos    プレート位置
   */
  //------------------------------------------------------------------
  void StartFormChange( CursorPos cpos );

public:
  //------------------------------------------------------------------
  /**
   * @brief   シーケンスの取得
   *
   * @return  現在のシーケンスID
   */
  //------------------------------------------------------------------
  u8 GetSequence( void ) const { return m_Seq; }

  //------------------------------------------------------------------
  /**
   * @brief   ドラッグモードかどうか
   *
   * @return  "true  == ドラッグモード"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsDragMode( void ) const { return m_DragMode; }


public:  
  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント
   *
   * @param   touchPanel    タッチパネル
   *
   * @return  "true  == 入れ替えイベント開始"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool TouchEvent( gfl2::ui::TouchPanel* touchPanel );

private:
  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント：リスト入替時
   *
   * @param   touchPanel    タッチパネル
   *
   * @return  "true  == タッチイベントあり"
   * @return  "false == タッチイベントなし"
   */
  //------------------------------------------------------------------
  bool TouchEvent_Exchange( gfl2::ui::TouchPanel* touchPanel );

private:
  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント：トリガー
   *
   * @param   touchPanel    タッチパネル
   * @param   destID        選択ボタンID格納先
   *
   * @return  "true  == タッチイベントあり"
   * @return  "false == タッチイベントなし"
   */
  //------------------------------------------------------------------
  bool TouchEvent_Trigger( gfl2::ui::TouchPanel* touchPanel, u32* destID = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント：ホールド
   *
   * @param   touchPanel    タッチパネル
   * @param   destID        選択ボタンID格納先
   * @param   except        除外ボタンID
   *
   * @return  "true  == タッチイベントあり"
   * @return  "false == タッチイベントなし"
   */
  //------------------------------------------------------------------
  bool TouchEvent_Hold( gfl2::ui::TouchPanel* touchPanel, u32* destID = NULL, u32 except = BTN_ID_NUM );

  //------------------------------------------------------------------
  /**
   * @brief   タッチイベント：リリース
   *
   * @param   touchPanel    タッチパネル
   *
   * @return  "true  == タッチイベントあり"
   * @return  "false == タッチイベントなし"
   */
  //------------------------------------------------------------------
  bool TouchEvent_Release( gfl2::ui::TouchPanel* touchPanel );

private:
  //------------------------------------------------------------------
  /**
   * @brief   タッチしているボタンIDの取得
   *
   * @param   touchPanel    タッチパネル
   * @param   except        除外ボタンID
   *
   * @return  ボタンID
   */
  //------------------------------------------------------------------
  u32 GetTouchBtnID( gfl2::ui::TouchPanel* touchPanel, u32 except = BTN_ID_NUM );

  //------------------------------------------------------------------
  /**
   * @brief   タッチ座標の取得
   *
   * @param   touchPanel    タッチパネル
   *
   * @return  タッチ座標
   */
  //------------------------------------------------------------------
  gfl2::math::Vector3 GetTouchPos( gfl2::ui::TouchPanel* touchPanel );

private:
  //------------------------------------------------------------------
  /**
   * @brief   所持道具IDの取得
   *
   * @param   btnID   ボタンID
   *
   * @return  道具ID
   */
  //------------------------------------------------------------------
  u16 GetItemNo( u32 btnID );


private:
  DrawerBase*   m_pBase;

  PlateDraw**     m_ppPokePlate;

  CursorDraw*     m_pCursor;

  MessageWindow*  m_pMsgWindow;

  u8    m_MemberCount;

  bool  m_LoopExchange;

  bool  m_DragMode;

  u8    m_Seq;

  CursorPos     m_MovePos;    //!< 移動元カーソル位置
  CursorPos     m_DestPos;    //!< 移動先カーソル位置

  PlateDraw*    m_pMovePlate;   //!< 移動元プレート
  PlateDraw*    m_pDestPlate;   //!< 移動先プレート

  PlateMessageWindow*   m_pMoveMsgWnd;
  PlateMessageWindow*   m_pDestMsgWnd;

  u16   m_MoveItem;
  u16   m_DestItem;

  gfl2::lyt::LytPane*   m_pNullVisible;

  CursorPos     m_SelectPlate;
  CursorPos     m_DecidePlate;

  ItemIconDrag*   m_pItemDrag;

  FormChange*     m_pFormChange;

private:
  u32     m_TriggerBtnID;

  u32     m_DestBtnID;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


#endif // #ifndef NIJI_PROJECT_APP_POKELIST_ITEMEXCHANGE_H_INCLUDED
