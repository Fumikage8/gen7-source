#ifndef NIJI_PROJECT_APP_POKELIST_POKEEXCHANGE_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_POKEEXCHANGE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListPokeExchange.h
 * @author  fukushima_yuya
 * @date    2015.03.27
 * @brief   ポケモンの入れ替え
 */
//==============================================================================

// gflib2
#include <macro/include/gfl2_Macros.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;   //!< 共通描画クラス
class CursorDraw;     //!< カーソル
class PlateDraw;      //!< プレート
class PlatePosition;  //!< プレート位置
class MessageWindow;  //!< メッセージウィンドウ
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモン入れ替えクラス
//------------------------------------------------------------------------------
class PokeExchange
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeExchange );

public:
  //! @brief  並べ替えシーケンス
  enum PokeExchangeSequence
  {
    SEQ_POKE_NONE,
    SEQ_POKE_MOVE_SELECT,   //!< 移動元のポケモン選択
    SEQ_POKE_DEST_SELECT,   //!< 移動先のポケモン選択
    SEQ_POKE_EXCHANGE,      //!< ポケモンの入れ替え
    SEQ_POKE_EXCHANGING,    //!< ポケモンの入れ替え中
    SEQ_POKE_EXCHANGE_END,  //!< ポケモンの入れ替え終了
    SEQ_POKE_NUM  
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
  PokeExchange( DrawerBase* pBase, PlateDraw** pPokePlate,
    CursorDraw* pCursor, MessageWindow* pMsgWindow, u8 memberCount );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~PokeExchange( void );

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
   * @brief   ポケモンの入れ替えの開始
   */
  //------------------------------------------------------------------
  void StartExchange( bool oneTime = false );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンの入れ替えの終了
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

public:
  //------------------------------------------------------------------
  /**
   * @brief   移動元のセット
   * @param   cpos    選択したプレート位置
   * @param   isDrag  ドラッグでの入れ替えかどうか(デフォルトは選択入れ替え)
   */
  //------------------------------------------------------------------
  void SetMovePos( CursorPos cpos, bool isDrag = false );

  //------------------------------------------------------------------
  /**
   * @brief   移動先のセット
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
   * @brief   移動元プレート位置クラスの取得
   * @return  PlatePositionクラスポインタ
   */
  //------------------------------------------------------------------
  PlatePosition* GetMovePlatePos( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   プレートの描画プライオリティの変更
   */
  //------------------------------------------------------------------
  void AppendChild( CursorPos cpos );

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
   * @brief   タッチイベント：リスト通常時
   *
   * @param   touchPanel    タッチパネル
   *
   * @return  "true  == タッチイベントあり"
   * @return  "false == タッチイベントなし"
   */
  //------------------------------------------------------------------
  bool TouchEvent_ListMode( gfl2::ui::TouchPanel* touchPanel );

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
  DrawerBase*   m_pBase;

  PlateDraw**     m_ppPokePlate;

  CursorDraw*     m_pCursor;

  MessageWindow*  m_pMsgWindow;

  u8    m_MemberCount;

  bool  m_DragMode;

  bool  m_OneTime;

  u8    m_Seq;

  CursorPos     m_MovePos;    //!< 移動元カーソル位置
  CursorPos     m_DestPos;    //!< 移動先カーソル位置

  PlateDraw*    m_pMovePlate;   //!< 移動元プレート
  PlateDraw*    m_pDestPlate;   //!< 移動先プレート

  gfl2::lyt::LytPane*   m_pNullVisible;

  CursorPos     m_SelectPlate;
  CursorPos     m_DecidePlate;
  u8            m_HoldFrame;

private:
  u32     m_TriggerBtnID;

  u32     m_DestBtnID;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


#endif // #ifndef NIJI_PROJECT_APP_POKELIST_POKEEXCHANGE_H_INCLUDED
