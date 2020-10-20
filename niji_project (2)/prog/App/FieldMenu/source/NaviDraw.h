//=============================================================================
/**
 * @file    NaviDraw.h
 * @brief   フィールド下画面マップ表示
 * @author  Hiroyuki Nakamura
 * @date    2015.05.20
 */
//=============================================================================
#if !defined( NAVI_DRAW_H_INCLUDED )
#define NAVI_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/FieldMenuSave.h"
#include "Savedata/include/MiscSave.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class MapManager;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Field)
class ZoneDataLoader;
class ZoneDataAccessor;
GFL_NAMESPACE_BEGIN(EventList)
class EventListDataManager;
GFL_NAMESPACE_END(EventList)
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

// 前方宣言
class FrameListener;

//=============================================================================
/**
 * @class NaviDraw
 * @brief 下画面マップ表示クラス
 * @date  2015.05.22
 */
//=============================================================================
class NaviDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  static const u32 EVENT_MSG_NONE = 0xffffffff;   //!< イベントメッセージなし

  //! 位置アイコンの表示モード
  enum PosIconMode
  {
    MODE_ICON_INIT = 0,   //!< 初期値
    MODE_HERO_ICON,       //!< 主人公アイコン
    MODE_FINDER_NORMAL,   //!< ポケファインダー（通常）
    MODE_FINDER_RARE,     //!< ポケファインダー（レア）
  };


private:
  static const u32 EVENT_ID_NONE = 0xffffffff;  //!< イベントなし（イベントリスト）

  static const u32 MAP_CENTER_PANE_MAX = 4;   //!< マップ中央のペイン最大数


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap          ヒープ
   * @param   render_man    描画マネージャ
   * @param   is_open       メニューを開いているか
   * @param   mode          表示モード
   * @param   center_pane   マップ中央のペイン
   */
  //-----------------------------------------------------------------------------
  NaviDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const bool * is_open, const bool * is_passive, const s32 * mode, const gfl2::lyt::LytPane * center_pane );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~NaviDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize
   * @brief   初期化
   * @date    2015.07.14
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
   * @date    2015.07.14
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFrameListener
   * @brief   リスナーをセット
   * @date    2015.05.22
   *
   * @parma   listener  リスナー
   */
  //-----------------------------------------------------------------------------
  void SetFrameListener( FrameListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始
   * @date    2015.05.25
   */
  //-----------------------------------------------------------------------------
  void Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Stop
   * @brief   停止
   * @date    2015.05.25
   */
  //-----------------------------------------------------------------------------
  void Stop(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OpenMenu
   * @brief   フィールドメニューが開いた時の処理
   * @date    2015.06.09
   */
  //-----------------------------------------------------------------------------
  void OpenMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CloseMenu
   * @brief   フィールドメニューが閉じた時の処理
   * @date    2015.06.09
   */
  //-----------------------------------------------------------------------------
  void CloseMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMainEventMsgID
   * @brief   メインイベントのメッセージIDを取得
   * @date    2015.09.17
   *
   * @retrun  メッセージID
   */
  //-----------------------------------------------------------------------------
  u32 GetMainEventMsgID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMainEventWorkValue
   * @brief   メインイベントのワーク値を取得
   * @date    2015.11.20
   *
   * @return  ワーク値
   */
  //-----------------------------------------------------------------------------
  u32 GetMainEventWorkValue(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveMapCenter
   * @brief   マップの中心を移動
   * @date    2016.01.12
   */
  //-----------------------------------------------------------------------------
  void MoveMapCenter(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePosIcon
   * @brief   現在地アイコンの表示モードを変更
   * @date    2016.01.15
   *
   * @param   icon  表示モード
   */
  //-----------------------------------------------------------------------------
  void ChangePosIcon( PosIconMode icon );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFinderIcon
   * @brief   現在地アイコンの表示モードがファインダーか
   * @date    2016.01.15
   *
   * @retval  true  = ファインダー
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFinderIcon(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   ゲームデータ関連取得
   * @date    2015.09.01
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Create2D
   * @brief   2D関連生成
   * @date    2015.09.04
   */
  //-----------------------------------------------------------------------------
  void Create2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Delete2D
   * @brief   2D関連削除
   * @date    2015.09.04
   */
  //-----------------------------------------------------------------------------
  void Delete2D(void);

  f32 GetAnimeMaxFrame( u32 anm );  // @note 本体なし

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   アップデート
   * @date    2015.05.25
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.05.25
   *
   * @param   displayNo   描画ディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMapManager
   * @brief   マップマネージャ生成
   * @date    2016.02.23
   */
  //-----------------------------------------------------------------------------
  void CreateMapManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMapManager
   * @brief   マップマネージャ削除
   * @date    2016.02.23
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMapManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMapManagerIcon
   * @brief   マップマネージャのアイコン初期化
   * @date    2015.09.07
   */
  //-----------------------------------------------------------------------------
  void InitMapManagerIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMapPosition
   * @brief   マップ表示更新
   * @date    2015.09.01
   */
  //-----------------------------------------------------------------------------
  void UpdateMapPosition(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMapData
   * @brief   マップデータ取得
   * @date    2016.02.17
   *
   * @param   map_id    マップID格納場所
   * @param   x         X座標格納場所
   * @param   y         Y座標格納場所
   * @param   r         向き格納場所
   * @param   is_arrow  向きの矢印を表示するかの格納場所
   * @param   zone_id   ゾーンID
   * @param   accessor  ゾーンデータアクセサー
   */
  //-----------------------------------------------------------------------------
  void GetMapData( u32 * map_id, f32 * x, f32 * y, f32 * r, bool * is_arrow, Field::ZoneID zone_id, Field::ZoneDataAccessor * accessor );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateEventListData
   * @brief   イベントリストデータ生成
   * @date    2015.09.07
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateEventListData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteEventListData
   * @brief   イベントリストデータ削除
   * @date    2015.09.07
   */
  //-----------------------------------------------------------------------------
  void DeleteEventListData(void);

  void CreateButton(void);  // @note 本体なし

  //-----------------------------------------------------------------------------
  /**
   * @func    GetHeroRotate
   * @brief   主人公の向きを取得
   * @date    2015.09.11
   *
   * @return  主人公の向き
   */
  //-----------------------------------------------------------------------------
  f32 GetHeroRotate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutMainEventIcon
   * @brief   メインイベントのアイコンを配置
   * @date    2015.09.17
   *
   * @param   accessor  ゾーンデータアクセサー
   */
  //-----------------------------------------------------------------------------
  void PutMainEventIcon( s32 id, Field::ZoneDataAccessor * accessor );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMapCenterPane
   * @brief   マップ中央のペインを初期化
   * @date    2016.01.13
   *
   * @param   pane  ペイン
   */
  //-----------------------------------------------------------------------------
  void InitMapCenterPane( const gfl2::lyt::LytPane * pane );


private:
  app::util::Heap * m_heap;   //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  const s32 * m_dispMode;   //!< 下画面表示モード

//  gfl2::lyt::LytWk * m_pLytWk;        //!< レイアウトワーク

  FrameListener * m_frameListener;  //!< リスナー

  const bool * m_isMenuOpen;  //!< メニューが開いているか
  const bool * m_isPassive;   //!< 下画面の入力が禁止されているか

  const gfl2::lyt::LytPane * m_centerPane[MAP_CENTER_PANE_MAX];   //!< 中心ペイン

  GameSys::GameManager * m_gameManager;             //!< ゲームマネージャ
  gfl2::fs::AsyncFileManager * m_fileManager;       //!< ファイルマネージャ
  Savedata::FieldMenu * m_saveFieldMenu;            //!< フィールドメニューセーブデータ
  Savedata::Misc * m_saveMisc;                      //!< 未分類セーブデータ
  Savedata::MyStatus * m_myStatus;                  //!< プレイヤーステータス
  const Field::ZoneDataLoader * m_zoneDataLoader;   //!< ゾーンデータローダー
  const Savedata::ConfigSave * m_configSave;        //!< コンフィグセーブデータ

  void * m_pLytResBuff;   //!< レイアウトリソースバッファ

  u16 m_mainSeq;    //!< メインシーケンス
  u16 m_subSeq;     //!< サブシーケンス

  App::Tool::MapManager * m_pMapManager;  //!< マップマネージャ

  Field::EventList::EventListDataManager * m_pEventListDataManager;   //!< イベントリストデータマネージャ
  u32 m_mainEventID;      //!< メインイベントのID
  u32 m_mainEventIndex;   //!< メインイベントのインデックス

  PosIconMode m_posIconMode;  //!< 現在地アイコンの表示モード
};


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // NAVI_DRAW_H_INCLUDED
