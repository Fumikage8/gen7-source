//=============================================================================
/**
 * @file    ReportDraw.h
 * @brief   レポート画面
 * @author  Hiroyuki Nakamura
 * @date    2015.08.17
 */
//=============================================================================
#if !defined( REPORT_DRAW_H_INCLUDED )
#define REPORT_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class MenuWindow;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Savedata)
class ZukanData;
class PlayTime;
class MyItem;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(Field)
class Fieldmap;
class ZoneDataLoader;
class EventWork;
class FieldReportPlayer;
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//=============================================================================
/**
 * @class ReportDraw
 * @brief レポート画面
 * @date  2015.08.17
 */
//=============================================================================
class ReportDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::MenuWindowListener
{
public:
  //! 呼び出しモード
  enum CallMode
  {
    CALL_MODE_NORMAL = 0,   //!< 通常
    CALL_MODE_NET,          //!< 通信用
  };


private:
  static const u32 END_COUNT_MAX = 30;  //!< 終了待ちカウント

  static const u32 COMPLETE_WAIT = 20;  //!< レポート成功時の終了待ち @note SE待ちが長すぎるので代わりのウェイト

  static const f32 MENU_PX;   //!< メニュー表示X座標
  static const f32 MENU_PY;   //!< メニュー表示Y座標

  static const f32 BEADS_ICON_PX;   //!< ビーズアイコン初期X座標
  static const f32 BEADS_ICON_PY1;  //!< ビーズアイコン初期Y座標（上）
  static const f32 BEADS_ICON_PY2;  //!< ビーズアイコン初期Y座標（下）
  static const f32 BEADS_ICON_SX;   //!< ビーズアイコンX表示間隔


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   mode        呼び出しモード
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   cursor      カーソルリソースバッファ
   */
  //-----------------------------------------------------------------------------
  ReportDraw( CallMode mode, app::util::Heap * heap, app::util::AppRenderingManager * render_man, void * cursor );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ReportDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize
   * @brief   初期化
   * @date    2015.08.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Initialize(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate
   * @brief   終了処理
   * @date    2015.08.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate(void);

  void Start(void);   // @note 本体なし
  void Stop(void);    // @note 本体なし

  //-----------------------------------------------------------------------------
  /**
   * @func    IsModuleEnd
   * @brief   終了チェック
   * @date    2015.08.17
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsModuleEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCancel
   * @brief   キャンセルされたか
   * @date    2015.08.18
   *
   * @retval  true  = キャンセル
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCancel(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   ゲームデータ取得
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新処理
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画処理
   * @date    2015.08.17
   *
   * @param   displayNo   描画ディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayout
   * @brief   レイアウトリソース読み込み
   * @date    2015.08.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadLayout
   * @brief   レイアウトリソース削除
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void UnloadLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayout
   * @brief   レイアウト生成
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void CreateLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteLayout
   * @brief   レイアウト削除
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void DeleteLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2015.08.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadMessage
   * @brief   メッセージ削除
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePokeIcon
   * @brief   ポケモンアイコン生成
   * @date    2015.08.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreatePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePokeIcon
   * @brief   ポケモンアイコン削除
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void DeletePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuWindow
   * @brief   メニューウィンドウ生成
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void CreateMenuWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuWindow
   * @brief   メニューウィンドウ削除
   * @date    2015.08.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectMenu
   * @brief   項目選択時のリスナー
   * @date    2015.08.18
   *
   * @param   id  メニュー項目のID
   *
   * @note  override MenuWindowListener
   */
  //-----------------------------------------------------------------------------
  virtual void SelectMenu( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDefaultString
   * @brief   デフォルト文字列書き込み
   * @date    2015.08.17
   */
  //-----------------------------------------------------------------------------
  void SetDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutTime
   * @brief   時間表示
   * @date    2015.08.18
   *
   * @param   h       時
   * @param   m       分
   * @param   pane_h  時表示ペイン
   * @param   pane_c  ":"表示ペイン
   * @param   pane_m  分表示ペイン
   */
  //-----------------------------------------------------------------------------
  void PutTime( u32 h, u32 m, gfl2::lyt::LytPaneIndex pane_h, gfl2::lyt::LytPaneIndex pane_c, gfl2::lyt::LytPaneIndex pane_m );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutDay
   * @brief   日付表示
   * @date    2015.08.18
   *
   * @param   y     年
   * @param   m     月
   * @param   d     日
   * @param   pane  表示ペイン
   */
  //-----------------------------------------------------------------------------
  void PutDay( u32 y, u32 m, u32 d, gfl2::lyt::LytPaneIndex pane );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateNowDate
   * @brief   現在時刻更新
   * @date    2015.08.18
   */
  //-----------------------------------------------------------------------------
  void UpdateNowDate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPokeIcon
   * @brief   ポケモンアイコン初期化
   * @date    2015.08.18
   */
  //-----------------------------------------------------------------------------
  void InitPokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitBeadsIcon
   * @brief   ビーズアイコン初期化
   * @date    2015.11.09
   */
  //-----------------------------------------------------------------------------
  void InitBeadsIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMessage
   * @brief   メッセージをセット
   * @date    2015.08.18
   *
   * @param   id  メッセージID
   */
  //-----------------------------------------------------------------------------
  void SetMessage( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTimerIcon
   * @brief   タイマーアイコンセット
   * @date    2015.08.18
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetTimerIcon( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishSaveTimeWindow
   * @brief   前回のセーブ時間のウィンドウを非表示
   * @date    2015.08.18
   */
  //-----------------------------------------------------------------------------
  void VanishSaveTimeWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEnd
   * @brief   終了へ
   * @date    2015.08.18
   */
  //-----------------------------------------------------------------------------
  void SetEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFieldReportPlayer
   * @brief   主人公動作クラス生成
   * @date    2015.12.09
   */
  //-----------------------------------------------------------------------------
  void CreateFieldReportPlayer(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFieldReportPlayer
   * @brief   主人公動作クラス削除
   * @date    2015.12.09
   */
  //-----------------------------------------------------------------------------
  void DeleteFieldReportPlayer(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2015.08.18
   *
   * @param   touch_panel タッチパネル
   * @param   is_touch    true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.08.17
   *
   * @param   button  ボタン
   * @param   key     十字キー
   * @param   stick   アナログスティック
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );


private:
  app::util::Heap * m_heap;   //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  void * m_cursorBuff;    //!< カーソルリソースバッファ
  CallMode m_mode;        //!< 呼び出しモード

  GameSys::GameData * m_gameData;             //!< ゲームデータ
  const Savedata::MyStatus * m_myStatus;      //!< プレイヤーデータ
  const Savedata::ConfigSave * m_configSave;  //!< コンフィグデータ
  const Savedata::ZukanData * m_zukanData;    //!< 図鑑セーブデータ
  Savedata::PlayTime * m_playTime;            //!< プレイ時間
  const Field::EventWork * m_eventWork;       //!< イベントワーク
  const Savedata::MyItem * m_myItem;          //!< 道具セーブデータ

  const pml::PokeParty * m_pokeParty;   //!< 手持ちポケモン
  u32 m_pokePartyCount;                 //!< 手持ちポケモン数

  Field::Fieldmap * m_fieldMap;   //!< フィールドマップ
  const Field::ZoneDataLoader * m_zoneDataLoader; //!< ゾーンデータローダー

  app::tool::PokeIcon * m_pPokeIcon;  //!< ポケモンアイコン
  u32 m_pokeIconLoadCount;            //!< ポケモンアイコン読み込み終了数

  void * m_pLytResBuff;   //!< レイアウトリソースバッファ
  void * m_pMsgBuff;      //!< メッセージリソースバッファ

  gfl2::str::MsgData * m_pMsgData;      //!< メッセージデータ
  print::WordSet * m_pWordSet;          //!< 単語セット

  Field::FieldReportPlayer * m_pReportPlayer;  //!< 主人公動作
  bool m_isMovePlayer;  //!< true = 主人公モデルがレポート動作中
  bool m_playerUpdate;  //!< true = 主人公モデルのレポート動作許可

  app::tool::MenuWindow * m_pMenuWindow;  //!< メニューウィンドウ
  u8 m_menuResult;  //!< メニューウィンドウ動作結果

  u8 m_mainSeq;   //!< メインシーケンス
  u8 m_subSeq;    //!< サブシーケンス
  u8 m_setupSeq;  //!< セットアップシーケンス

  u32 m_endCount;   //!< 終了カウント

  bool m_isEnd;   //!< true = レポート処理終了

  bool m_isDrawEnable;  //!< 描画許可フラグ
};


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // REPORT_DRAW_H_INCLUDED
