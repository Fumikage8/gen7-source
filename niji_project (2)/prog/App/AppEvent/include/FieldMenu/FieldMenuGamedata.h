//=============================================================================
/**
 * @file    FieldMenuGamedata.h
 * @brief   フィールド下画面 ゲーム中に保持し続けるデータ
 * @author  Hiroyuki Nakamura
 * @date    2015.06.23
 */
//=============================================================================
#if !defined( FIELDMENU_GAMEDATA_H_INCLUDED )
#define FIELDMENU_GAMEDATA_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// momiji
#include "App/FieldMenu/include/RideDrawParam.h"
#include "Savedata/include/FieldMenuSave.h"
// resource
#include "niji_conv_header/message/msg_rotom_message.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(Savedata)
class Misc;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//=============================================================================
/**
 * @class PlayData
 * @brief フィールド下画面ゲームデータ
 * @date  2015.06.23
 */
//=============================================================================
class PlayData
{
public:
  //! ロトムメッセージインデックス
  enum RotomMsgIndex
  {
    ROTOM_MSG_SCRIPT = 0,   //!< スクリプトイベント用
    ROTOM_MSG_EVENT,        //!< メインイベント用
    ROTOM_MSG_POS,          //!< POSイベント用
    ROTOM_MSG_ETC,          //!< その他
    ROTOM_MSG_REPORT,       //!< ロトム状況報告
    ROTOM_MSG_REQUEST,      //!< ロトムお願い
    ROTOM_MSG_FREE,         //!< ロトム会話
    ROTOM_MSG_NICKNAME,     //!< ニックネーム設定（最初の１回のみ）
    ROTOM_MSG_MAX           //!< 最大
  };

  //! ロトムメッセージデータ
  struct ROTOM_MSG_DATA
  {
    u32 text:31;      //!< テキスト番号
    u32 is_update:1;  //!< メッセージ表示中に更新した = 1
    u32 work;         //!< イベント用ワーク
  };

  static const u32 ROTOM_TEXT_NONE = 0x7fffffff;    //!< テキスト番号無効値
  static const u32 ROTOM_WORK_NONE = 0xffffffff;    //!< イベントワーク無効値
  static const u32 ROTOM_WORK_REPORT = 0xfffffffe;  //!< レポート喚起用イベントワーク値

  //! ロトム親密度データ
  struct ROTOM_FRIENDSHIP_DATA
  {
    u8 touch_pos;           //!< 最後にタッチした位置
    u8 touch_count;         //!< 連続でタッチされた回数
    u8 up_enable:1;         //!< 1 = タッチで親密度を上げられる
    u8 up_diff_minute:7;    //!< タッチで親密度を上げられるようになる時間カウンタ
    u8 talk_diff_minute:7;  //!< 会話用プレイ時間カウンタ
    u8 is_report_talk:1;    //!< レポート喚起の会話を行ったか
#if defined(GF_PLATFORM_CTR)
    nn::os::Tick touch_start_tick;  //!< 連続タッチ開始時間
#endif  // GF_PLATFORM_CTR
  };

  static const u32 ROTOM_TOUCH_CONTINUE_MAX = 3;  //!< ロトムが怒る連続タッチ数
  static const u32 ROTOM_TOUCH_CONTINUE_SEC = 5;  //!< ロトムが怒る連続タッチ秒

  static const u32 FIRIENDSHIP_UP_DIFF_MINUTE = 10;   //!< タッチで親密度が上げられるようになるまでの時間（分）
  static const u32 TALK_PLAYTIME_DIFF_MINUTE = 60;    //!< プレイ時間関連のメッセージ表示時間（分）


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  PlayData(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~PlayData();

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPos
   * @brief   カーソル位置を取得
   * @date    2015.06.23
   *
   * @return  カーソル位置
   */
  //-----------------------------------------------------------------------------
  u32 GetCursorPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetCursorPos
   * @brief   カーソル位置をセット
   * @date    2015.06.23
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void SetCursorPos( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRideCursorPos
   * @brief   ライド選択画面のカーソル位置を取得
   * @date    2015.08.06
   *
   * @return  カーソル位置
   */
  //-----------------------------------------------------------------------------
  u32 GetRideCursorPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRideCursorPos
   * @brief   ライド選択画面のカーソル位置をセット
   * @date    2015.08.06
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void SetRideCursorPos( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetSelectRideID
   * @brief   ライド選択画面で選択されたIDを取得（説明デモ用）
   * @date    2016.04.13
   *
   * @return  ライドID
   */
  //-----------------------------------------------------------------------------
  u32 GetSelectRideID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSelectRideID
   * @brief   ライド選択画面で選択されたIDをセット（説明デモ用）
   * @date    2016.04.13
   *
   * @param   id  ライドID
   */
  //-----------------------------------------------------------------------------
  void SetSelectRideID( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitRotomMessage
   * @brief   ロトムメッセージデータクリア（個別）
   * @date    2015.11.10
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void InitRotomMessage( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomMessage
   * @brief   ロトムメッセージセット
   * @date    2015.11.10
   *
   * @param   index       インデックス
   * @param   text        テキスト
   * @param   work        ワーク
   * @param   is_update   true = 表示中
   */
  //-----------------------------------------------------------------------------
  void SetRotomMessage( PlayData::RotomMsgIndex index, u32 text, u32 work, bool is_update );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetEnableRottomMsgIndex
   * @brief   ロトムメッセージインデックスを取得
   * @date    2015.11.10
   *
   * @return  優先度の高いインデックス
   */
  //-----------------------------------------------------------------------------
  RotomMsgIndex GetEnableRottomMsgIndex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMessageText
   * @brief   ロトムメッセージテキストを取得
   * @date    2015.11.10
   *
   * @param   index   インデックス
   *
   * @return  テキストID
   */
  //-----------------------------------------------------------------------------
  u32 GetRotomMessageText( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMessageText
   * @brief   ロトムメッセージワークを取得
   * @date    2016.01.14
   *
   * @param   index   インデックス
   *
   * @return  ワーク
   */
  //-----------------------------------------------------------------------------
  u32 GetRotomMessageWork( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMessageIsUpdate
   * @brief   ロトムメッセージの表示中更新フラグを取得
   * @date    2016.01.15
   *
   * @param   index   インデックス
   *
   * @retval  true  = 表示中に更新された
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool GetRotomMessageIsUpdate( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMessageIsUpdate
   * @brief   ロトムメッセージの表示中更新フラグを落とす
   * @date    2016.01.15
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void ResetRotomMessageIsUpdate( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEndContinueRotomMessage
   * @brief   ゲーム開始時のロトムメッセージを表示したか
   * @date    2016.01.14
   *
   * @retval  true  = 表示した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsEndContinueRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEndContinueRotomMessage
   * @brief   ゲーム開始時のロトムメッセージを表示済みにする
   * @date    2016.01.14
   */
  //-----------------------------------------------------------------------------
  void SetEndContinueRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEndInputMode
   * @brief   下画面終了時の入力モードを記憶
   * @date    2015.11.19
   *
   * @param   flg   true = アクティブ, false = パッシブ
   */
  //-----------------------------------------------------------------------------
  void SetEndInputMode( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetEndInputMode
   * @brief   下画面終了時の入力モードを記憶
   * @date    2015.11.19
   *
   * @retval  true  = アクティブ
   * @retval  false = パッシブ
   */
  //-----------------------------------------------------------------------------
  bool GetEndInputMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRideParam
   * @brief   ライド選択画面のパラメータを設定
   * @date    2016.03.30
   *
   * @param   param   セットするパラメータ
   */
  //-----------------------------------------------------------------------------
  void SetRideParam( App::FieldMenu::RIDE_PARAM * param );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRideParam
   * @brief   ライド選択画面のパラメータを取得
   * @date    2016.03.30
   *
   * @return  パラメータ
   */
  //-----------------------------------------------------------------------------
  const App::FieldMenu::RIDE_PARAM * GetRideParam(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    RotomTouch
   * @brief   ロトムにタッチ
   * @date    2016.12.01
   *
   * @param   pos   タッチした位置
   *
   * @retval  true  = 通常
   * @retval  false = 連続でタッチしすぎた
   */
  //-----------------------------------------------------------------------------
  bool RotomTouch( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitRotomTouch
   * @brief   ロトムのタッチデータを初期化
   * @date    2016.12.01
   */
  //-----------------------------------------------------------------------------
  void InitRotomTouch(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomTouchFriendshipUpFlag
   * @brief   ロトムをタッチした際の親密度アップ許可設定
   * @date    2016.12.01
   *
   * @param   flg   true = 許可, false = 禁止
   */
  //-----------------------------------------------------------------------------
  void SetRotomTouchFriendshipUpFlag( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomTouchFriendshipUp
   * @brief   ロトムをタッチした際に親密度を上げられるか
   * @date    2016.12.01
   *
   * @retval  true  = 上げられる
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomTouchFriendshipUp(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateTime
   * @brief   時間経過処理
   * @date    2016.12.12
   *
   * @param   diff_minute   経過時間（分）
   */
  //-----------------------------------------------------------------------------
  void UpdateTime( u32 diff_minute );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomTalkDiffTime
   * @brief   プレイ時間関連のロトム会話が発生するか
   * @date    2016.12.12
   *
   * @retval  true  = する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomTalkDiffTime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetRotomTalkDiffTime
   * @brief   ロトム会話用時間カウンタをリセット
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void ResetRotomTalkDiffTime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomReportTalkEnable
   * @brief   ロトムがレポート喚起するか
   * @date    2017.03.28
   *
   * @retval  true  = する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomReportTalkEnable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomReportTalkEnable
   * @brief   ロトムのレポート喚起フラグを設定
   * @date    2017.03.28
   *
   * @param   flg   true  = する, false = しない
   */
  //-----------------------------------------------------------------------------
  void SetRotomReportTalkEnable( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomRequestAchieved
   * @brief   ロトムのお題を達成
   * @date    2017.02.07
   *
   * @param   req   達成したお題
   *
   * @note  常駐領域なのでここに置く
   */
  //-----------------------------------------------------------------------------
  static void SetRotomRequestAchieved( Savedata::FieldMenu::RotomRequest req );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomRequestEvolution
   * @brief   ロトポンの進化後のお願い発生フラグをセット
   * @date    2017.03.23
   *
   * @param   flg   発生フラグ
   */
  //-----------------------------------------------------------------------------
  void SetRotomRequestEvolution( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomRequestEvolution
   * @brief   ロトポンの進化後のお願いを発生させるか
   * @date    2017.03.23
   *
   * @retval  true  = 発生させる
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomRequestEvolution(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    InitRotomMessage
   * @brief   ロトムメッセージデータクリア（全体）
   * @date    2015.11.10
   */
  //-----------------------------------------------------------------------------
  void InitRotomMessage(void);


private:
  u16 m_cursorPos;      //!< フィールドメニューのカーソル位置
  u8 m_rideCursorPos;   //!< ライドポケモン選択画面のカーソル位置
  u8 m_selectRideID;    //!< 選択されたライドID（説明デモ用）

  ROTOM_MSG_DATA m_rotomReq[ROTOM_MSG_MAX];   //!< ロトムメッセージ

  bool m_endContinueRotomMsg;   //!< ゲーム開始時のロトムメッセージを表示したか

  bool m_isInput;   //!< 下画面終了時に入力状態

  App::FieldMenu::RIDE_PARAM m_rideParam;   //!< ライド選択画面復帰時のパラメータ

  ROTOM_FRIENDSHIP_DATA m_rotomFriendshipData;  //!< ロトム親密度データ

  bool m_isRotomRequestEvolution;   //!< 進化後のロトムのお願いが発生したか

#if PM_DEBUG
public:
  void DebugSetRotomAnimeLoop( u32 flg );
  u32 DebugIsRotomAnimeLoop(void);
  void DebugSetRotomRequest( Savedata::FieldMenu::RotomRequest req );
  Savedata::FieldMenu::RotomRequest DebugGetRotomRequest(void);
  void DebugSetRotomReportFlag( bool flg );
  bool DebugGetRotomReportFlag(void);
  void DebugSetRotomFreeTalkID( u32 val );
  u32 DebugGetRotomFreeTalkID(void);
  void DebugSetRotomGameClearMsgID( u32 val );
  u32 DebugGetRotomGameClearMsgID(void);
private:
  u32 m_debugIsRotomAnimeLoop;
  Savedata::FieldMenu::RotomRequest m_debugRotomReq;
  bool m_debugIsRotomReport;
  u32 m_debugRotomFreeTalkID;
  u32 m_debugRotomGameClearMsgID;
#endif  // PM_DEBUG

};


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // FIELDMENU_GAMEDATA_H_INCLUDED
