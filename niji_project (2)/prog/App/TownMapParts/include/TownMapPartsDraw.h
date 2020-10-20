//=============================================================================
/**
 * @file    TownMapPartsDraw.h
 * @brief   タウンマップ下画面マップ描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.13
 */
//=============================================================================
#if !defined( TOWNMAP_PARTS_DRAW_H_INCLUDED )
#define TOWNMAP_PARTS_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/FieldMenuSave.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
// module
#include "App/TownMap/include/TownMapDef.h"
// resource
#include "niji_conv_header/field/zone/zone_id.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class MapManager;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Field)
class ZoneDataLoader;
class Location;
class EventWork;
class World;
class ZoneDataAccessor;
GFL_NAMESPACE_BEGIN(EventList)
class EventListDataManager;
GFL_NAMESPACE_END(EventList)
GFL_NAMESPACE_END(Field)

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)
#endif

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)

//=============================================================================
/**
 * @class TownMapPartsDraw
 * @brief タウンマップ下画面マップ描画クラス
 * @date  2015.10.13
 */
//=============================================================================
class TownMapPartsDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //=============================================================================
  /**
   * @class SubScreenMapListener
   * @brief 下画面マップ描画リスナークラス
   * @date  2015.10.13
   */
  //=============================================================================
  class SubScreenMapListener
  {
  public:
    //! 動作ID
    enum ActionID
    {
      ACTION_ID_RETURN = 0,       //!< 戻る
      ACTION_ID_SELECT,           //!< 選択
      ACTION_ID_ZOOM,             //!< 拡縮開始
      ACTION_ID_ZOOM_END,         //!< 拡縮終了
      ACTION_ID_LAND_CHANGE,      //!< 島切り替え
      ACTION_ID_LAND_CHANGE_END,  //!< 島切り替え終了
      ACTION_ID_SET_ICONDATA,     //!< アイコンデータ設定
    };

  public:
    //-----------------------------------------------------------------------------
    /**
     * @func    OnAction
     * @brief   入力時のリスナー
     * @date    2015.10.02
     *
     * @param   id    動作ID
     */
    //-----------------------------------------------------------------------------
    virtual void OnAction( ActionID id ) = 0;
  };

  //! 呼び出しモード
  enum CallMode
  {
    CALL_TOWNMAP = 0,   //!< タウンマップ
    CALL_RIDE_FLY,      //!< そらをとぶ
    CALL_ZUKAN,         //!< 図鑑
  };

  //! 島切り替えモード
  enum LandChangeMode
  {
    LAND_CHANGE_FREE = 0,   //!< 近い方向に切り替える
    LAND_CHANGE_LEFT,       //!< 左方向に切り替える
    LAND_CHANGE_RIGHT,      //!< 右方向に切り替える
  };

  //! 外部設定用アイコンデータ
  struct EX_ICON_DATA
  {
    PlaceType type;   //!< アイコンタイプ
    s32 param;        //!< パラメータ
    f32 x;            //!< X座標
    f32 y;            //!< Y座標
  };


private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_BUTTON = 0,   //!< ボタン
    LYTID_ISLAND_MAP,   //!< 全体マップ
    LYTID_ZOOM_MAP,     //!< 拡大マップ
    LYTID_MAX
  };

  static const s32 PLACE_ICON_MAX = 40;   //!< 場所アイコン数

  //! ボタンID
  enum ButtonID
  {
    BTNID_RETURN = 0,   //!< 戻る
    BTNID_ZOOM,         //!< 拡縮
    BTNID_ARROW_L,      //!< 島切り替え矢印：左
    BTNID_ARROW_R,      //!< 島切り替え矢印：右

    BTNID_PLACE_ICON,   //!< 場所アイコン（開始位置）

    BTNID_MAX = BTNID_PLACE_ICON + PLACE_ICON_MAX,  //!< ボタン数
  };

  static const s32 CURSOR_MOVE_SPEED = 4;   //!< カーソル移動速度

  static const f32 MAP_SCALE;         //!< マップスケール値
  static const f32 DEFAULT_X_SIZE;    //! 全体マップの基準Xサイズ
  static const f32 DEFAULT_Y_SIZE;    //! 全体マップの基準Yサイズ

  static const f32 DISP_WIDTH_HALF;   //!< 画面幅 / 2
  static const f32 DISP_HEIGHT_HALF;  //!< 画面高さ / 2

  //! ゾーンデータ
  struct ZONE_DATA
  {
    ZoneID zone_id;                     //!< ゾーンID
    gfl2::lyt::LytPaneIndex area_pane;  //!< 範囲ペイン
    gfl2::lyt::LytPaneIndex mask_pane;  //!< マスクペイン
  };

  //! 島ごとのデータ
  struct LAND_DATA
  {
    ZONE_DATA zone_data[LAND_ZONE_DATA_MAX];  //!< ゾーンデータ
    gfl2::lyt::LytPaneIndex parent_pane;      //!< 親ペイン
    gfl2::lyt::LytPaneIndex range_pane;       //!< 範囲ペイン
  };

  static const LAND_DATA LandData[LAND_DATA_MAX];   //!< 島ごとのデータ

  static const u32 MAP_ZOOM_COUNT = 4;                //!< 全体マップ拡縮カウンタ
  static const f32 MAP_ZOOM_SCALE_MIN;                //!< 全体マップスケール最小値
  static const f32 MAP_ZOOM_SCALE_MAX;                //!< 全体マップスケール最大値
  static const f32 MAP_ZOOM_SCALE_VAL;                //!< 全体マップ拡縮増減値
  static const u32 MAP_ZOOM_MOVE_FRAME = (4+3+2+1);   //!< 全体マップの動作フレーム
  static const s32 MAP_ZOOM_ALPHA_VAL = 63;           //!< 全体マップアルファ増減値

  static const u32 LandLowTable[LAND_DATA_MAX];   //!< 島の並び順テーブル
  static const s32 LAND_MOVE_COUNT = 2;           //!< 島切り替え移動カウンタ

  static const s32 PLACE_ICON_ANIME_MAX = PLACE_TYPE_NONE+1;  //!< 場所アイコンアニメ数

  //! 場所アイコンデータ
  struct PLACE_ICON_DATA
  {
    gfl2::lyt::LytPaneIndex pane;       //!< ペイン
    u32 anm_type[PLACE_ICON_ANIME_MAX]; //!< タイプ変更アニメ
    u32 anm_touch;                      //!< ボタンアニメ：タッチ
    u32 anm_release;                    //!< ボタンアニメ：リリース
    u32 anm_cancel;                     //!< ボタンアニメ：キャンセル
    u32 anm_select;                     //!< ボタンアニメ：キーセレクト
    u32 anm_active;                     //!< ボタンアニメ：アクティブ
    u32 anm_passive;                    //!< ボタンアニメ：パッシブ
  };

  static const PLACE_ICON_DATA PlaceIconTable[PLACE_ICON_MAX];  //!< 場所アイコンテーブル

  //! 場所アイコンボタン
  struct PLACE_ICON_BUTTON
  {
    PlaceType type;   //!< アイコンタイプ
    s32 param;        //!< パラメータ

    f32 x;  //!< X座標
    f32 y;  //!< Y座標
  };

  static const u32 WORLD_HEAP_SIZE = 0x10000;   //!< ワールドデータ用ヒープサイズ


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   mode        呼び出しモード
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   land_no     島番号 ( 0オリジン )
   */
  //-----------------------------------------------------------------------------
  TownMapPartsDraw( CallMode mode, app::util::Heap * heap, app::util::AppRenderingManager * render_man, u32 land_no );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~TownMapPartsDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTownmapData
   * @brief   タウンマップデータセット
   * @date    2015.10.26
   *
   * @param   zone      ゾーンデータリソース
   * @param   zone_len  ゾーンデータリソース長
   * @param   icon      アイコンデータリソース
   * @param   iconlen   アイコンデータリソース長
   */
  //-----------------------------------------------------------------------------
  void SetTownmapData( ZONE_RESOURCE * zone, u32 zone_len, ICON_RESOURCE * icon_res, u32 icon_len );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMapListener
   * @brief   リスナーをセット
   * @date    2015.10.02
   *
   * @param   listener リスナー
   */
  //-----------------------------------------------------------------------------
  void SetMapListener( SubScreenMapListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.10.02
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ　※デフォルト = NULL
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Init( void * res_buf, void * msg_buf = NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.10.02
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2015.10.02
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsStart
   * @brief   開始処理が終了したかをチェック
   * @date    2015.10.14
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadResource
   * @brief   リソース読み込み
   * @date    2015.10.13
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadResource
   * @brief   リソース削除
   * @date    2015.10.13
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UnloadResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetLayoutResource
   * @brief   レイアウトリソース取得
   * @date    2015.10.13
   *
   * @return  レイアウトリソース
   */
  //-----------------------------------------------------------------------------
  void * GetLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeLandNo
   * @brief   島番号を変更
   * @date    2015.10.21
   *
   * @param   land_no   島番号 ( 0オリジン )
   * @param   mode      切り替えモード
   */
  //-----------------------------------------------------------------------------
  void ChangeLandNo( u32 land_no, LandChangeMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetNextLand
   * @brief   次の島番号を取得
   * @date    2015.10.21
   *
   * @param   mv  切り替え方向
   *
   * @return  島番号
   */
  //-----------------------------------------------------------------------------
  s32 GetNextLand( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetLandNo
   * @brief   島番号を取得
   * @date    2015.10.22
   *
   * @return  島番号
   */
  //-----------------------------------------------------------------------------
  u32 GetLandNo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetSelectWarpID
   * @brief   選択したアイコンのワープIDを取得
   * @date    2015.10.26
   *
   * @return  ワープID
   */
  //-----------------------------------------------------------------------------
  s32 GetSelectWarpID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPosIconParam
   * @brief   カーソル位置のアイコンのパラメータを取得
   * @date    2015.10.30
   *
   * @param   type        アイコンタイプ
   * @param   is_active   ture = アクティブなボタンのみ取得
   *
   * @retval  アイコンあり = アイコンに設定したパラメータ
   * @retval  アイコンなし = TM_DUMMY_DATA
   *
   * @note:
   *    ・type = PLACE_TYPE_MAX : 全アイコンタイプを参照する
   *    ・PLACE_TYPE_NONEのアイコンは取得しない
   *    ・ボタンとして機能しない場合、is_activeはfalseを指定すること！
   */
  //-----------------------------------------------------------------------------
  s32 GetCursorPosIconParam( PlaceType type, bool is_active );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutWarpPointWindow
   * @brief   "どこにとびますか？"のウィンドウ表示
   * @date    2015.10.29
   */
  //-----------------------------------------------------------------------------
  void PutWarpPointWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetWarpPointMessage
   * @brief   "○○にとびますか？"のメッセージ表示
   * @date    2015.10.29
   *
   * @param   str   表示する文字
   */
  //-----------------------------------------------------------------------------
  void SetWarpPointMessage( gfl2::str::StrBuf * str );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetWarpPointPlaceNameID
   * @brief   カーソル位置のワープIDの施設名を取得
   * @date    2015.10.29
   *
   * @param   id              文字列ID取得場所
   * @param   is_place_name   場所名かどうかを取得する場所
   *
   * @retval  true  = 成功
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool GetWarpPointPlaceNameID( s32 * id, u32 * is_place_name );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetExIconData
   * @brief   アイコンデータを設定
   * @date    2015.10.29
   *
   * @param   data      アイコンデータ
   * @param   data_max  データ数
   */
  //-----------------------------------------------------------------------------
  void SetExIconData( const EX_ICON_DATA * data, u32 data_max );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPosZoneID
   * @brief   カーソル位置のゾーンIDを取得
   * @date    2015.10.29
   *
   * @return  ゾーンID
   *
   * @note    場所不明の場合はZONE_ID_MAXを返す
   */
  //-----------------------------------------------------------------------------
  ZoneID GetCursorPosZoneID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMapPos
   * @brief   マップ上の座標を取得
   * @date    2015.11.02
   *
   * @param   pane  基準ペイン
   * @param   x     X座標
   * @param   y     Y座標
   *
   * @note    [in]: フィールドの座標
   * @note   [out]: マップの座標
   */
  //-----------------------------------------------------------------------------
  static void GetMapPos( gfl2::lyt::LytPane * pane, f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetIcon
   * @brief   アイコンを再設定
   * @date    2015.11.16
   *
   * @note  図鑑分布用に追加
   */
  //-----------------------------------------------------------------------------
  void ResetIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetIconPosition
   * @brief   アイコンを再設定
   * @date    2016.05.17
   *
   * @note  図鑑分布用に追加
   */
  //-----------------------------------------------------------------------------
  void ResetIconPosition(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsZoom
   * @brief   拡大中か縮小中か
   * @date    2015.12.17
   *
   * @return  true = 拡大中, false = 縮小中
   */
  //-----------------------------------------------------------------------------
  bool IsZoom(void) const { return m_isZoom; }

  //-----------------------------------------------------------------------------
  /**
   * @func    GetHeroPosition
   * @brief   主人公の位置データを取得
   * @date    2016.03.03
   *
   * @param   accessor  ゾーンデータアクセサー
   * @param   x         X座標格納場所（フィールドのＸ座標）
   * @param   y         Y座標格納場所（フィールドのＺ座標）
   * @param   r         回転格納場所
   * @param   is_arrow  矢印表示格納場所 ( true = 表示 )
   */
  //-----------------------------------------------------------------------------
  static void GetHeroPosition( Field::ZoneDataAccessor * accessor, f32 * x, f32 * y, f32 * r, bool * is_arrow );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   GameData関連取得
   * @date    2015.10.23
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.10.02
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.10.02
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.10.02
   *
   * @param   res_buf   レイアウトリソースバッファ
   * @param   msg_buf   メッセージリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.10.02
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMapManager
   * @brief   マップマネージャ生成
   * @date    2015.10.14
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateMapManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMapManager
   * @brief   マップマネージャ削除
   * @date    2015.10.14
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMapManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateEventListDataManager
   * @brief   イベントリストデータマネージャ削除
   * @date    2015.11.03
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateEventListDataManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteEventListDataManager
   * @brief   イベントリストデータマネージャ削除
   * @date    2015.11.03
   */
  //-----------------------------------------------------------------------------
  void DeleteEventListDataManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.10.14
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetButtonInfoEx
   * @brief   ボタン情報生成
   * @date    2015.10.14
   *
   * @param   info        生成場所
   * @param   info_tbl    デフォルト情報テーブル
   * @param   parts_tbl   部品ペインテーブル
   * @param   bound_tbl   境界ペインテーブル
   * @param   max         生成数
   */
  //-----------------------------------------------------------------------------
  void SetButtonInfoEx(
        app::ui::ButtonInfoLytWk * info,
        const app::ui::ButtonInfoLytWk * info_tbl, 
        const gfl2::lyt::LytPaneIndex * parts_tbl,
        const gfl2::lyt::LytPaneIndex * bound_tbl,
        u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateNormalButton
   * @brief   基本のボタン生成
   * @date    2015.10.14
   *
   * @param   info  生成場所
   */
  //-----------------------------------------------------------------------------
  void CreateNormalButton( app::ui::ButtonInfoLytWk * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePlaceIconButton
   * @brief   場所アイコンのボタン生成
   * @date    2015.10.23
   *
   * @param   info  生成場所
   */
  //-----------------------------------------------------------------------------
  void CreatePlaceIconButton( app::ui::ButtonInfoLytWk * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeZoomButton
   * @brief   拡縮ボタンの切り替え
   * @date    2015.10.14
   *
   * @param   is_zoom   true = 拡大ボタン, false = 縮小ボタン
   */
  //-----------------------------------------------------------------------------
  void ChangeZoomButton( bool is_zoom );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutLandChangeButton
   * @brief   島切り替えボタンの表示切り替え
   * @date    2015.10.23
   *
   * @param   flg   true = 表示
   */
  //-----------------------------------------------------------------------------
  void PutLandChangeButton( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitChangeMapMode
   * @brief   マップ表示モード切り替え初期設定
   * @date    2015.10.14
   */
  //-----------------------------------------------------------------------------
  void InitChangeMapMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartChangeMapMode
   * @brief   マップ表示モード切り替え開始
   * @date    2015.10.14
   */
  //-----------------------------------------------------------------------------
  void StartChangeMapMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndChangeMapMode
   * @brief   マップ表示モード切り替え終了
   * @date    2015.10.14
   */
  //-----------------------------------------------------------------------------
  void EndChangeMapMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeMapMode
   * @brief   マップ表示モード切り替え
   * @date    2015.10.14
   */
  //-----------------------------------------------------------------------------
  void ChangeMapMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitCursorPos
   * @brief   カーソル初期配置
   * @date    2015.12.16
   */
  //-----------------------------------------------------------------------------
  void InitCursorPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutCursor
   * @brief   カーソル配置
   * @date    2015.10.19
   *
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void PutCursor( f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorMoveMax
   * @brief   カーソル移動座標の最大値を取得
   * @date    2016.05.05
   *
   * @param   u   上
   * @param   d   下
   * @param   l   左
   * @param   r   右
   */
  //-----------------------------------------------------------------------------
  void GetCursorMoveMax( f32 * u, f32 * d, f32 * l, f32 * r );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCursor
   * @brief   カーソル移動
   * @date    2015.10.16
   *
   * @param   mx        X方向移動方向
   * @param   my        Y方向移動方向
   * @param   is_boost  true = 速度アップ
   */
  //-----------------------------------------------------------------------------
  void MoveCursor( s32 mx, s32 my, bool is_boost );

  //-----------------------------------------------------------------------------
  /**
   * @func    AutoMoveCursor
   * @brief   カーソル自動移動
   * @date    2015.10.16
   */
  //-----------------------------------------------------------------------------
  void AutoMoveCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPos
   * @brief   カーソル座標を取得
   * @date    2015.10.16
   *
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void GetCursorPos( f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetSmallMapArea
   * @brief   縮小マップのフィールド範囲取得
   * @date    2016.04.26
   *
   * @param   au  上Y座標格納場所
   * @param   ad  下Y座標格納場所
   * @param   al  左X座標格納場所
   * @param   ar  右X座標格納場所
   */
  //-----------------------------------------------------------------------------
  void GetSmallMapArea( f32 * au, f32 * ad, f32 * al, f32 * ar );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMapPos
   * @brief   マップ上の座標を取得
   * @date    2015.10.16
   *
   * @param   x   X座標
   * @param   y   Y座標
   *
   * @note    [in]: フィールドの座標
   * @note   [out]: マップの座標
   */
  //-----------------------------------------------------------------------------
  void GetMapPos( f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMapPos
   * @brief   マップ上の座標を取得
   * @date    2015.11.02
   *
   * @param   pos   基準座標
   * @param   x     X座標
   * @param   y     Y座標
   *
   * @note    [in]: フィールドの座標
   * @note   [out]: マップの座標
   */
  //-----------------------------------------------------------------------------
  static void GetMapPos( gfl2::math::VEC3 * pos, f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetFieldPos
   * @brief   フィールドの座標を取得
   * @date    2015.10.16
   *
   * @param   x   X座標
   * @param   y   Y座標
   *
   * @note    [in]: マップの座標
   * @note   [out]: フィールドの座標
   */
  //-----------------------------------------------------------------------------
  void GetFieldPos( f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetInitLandMapPos
   * @brief   島ペインのデフォルト座標を記憶
   * @date    2015.10.19
   */
  //-----------------------------------------------------------------------------
  void GetInitLandMapPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetLandMapPos
   * @brief   島ペインを指定座標に移動
   * @date    2015.10.19
   *
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void SetLandMapPos( f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeLand
   * @brief   島変更処理
   * @date    2015.10.21
   */
  //-----------------------------------------------------------------------------
  void ChangeLand(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitChangeLand
   * @brief   島変更処理：初期化
   * @date    2015.10.21
   */
  //-----------------------------------------------------------------------------
  void InitChangeLand(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MainChangeLand
   * @brief   島変更処理：メイン
   * @date    2015.10.21
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool MainChangeLand(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndChangeLand
   * @brief   島変更処理：終了
   * @date    2015.10.21
   */
  //-----------------------------------------------------------------------------
  void EndChangeLand(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutLandPane
   * @brief   島ペイン配置
   * @date    2015.12.08
   */
  //-----------------------------------------------------------------------------
  void PutLandPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetLandIndex
   * @brief   島番号のテーブルインデックスを取得
   * @date    2015.10.22
   *
   * @param   land_no   島番号
   *
   * @return  インデックス
   */
  //-----------------------------------------------------------------------------
  s32 GetLandIndex( u32 land_no );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitIcon
   * @brief   アイコン初期化
   * @date    2015.10.23
   */
  //-----------------------------------------------------------------------------
  void InitIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutIcon
   * @brief   アイコン表示切り替え
   * @date    2015.10.23
   *
   * @param   flg   true = 表示
   */
  //-----------------------------------------------------------------------------
  void PutIcon( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveIcon
   * @brief   アイコン移動
   * @date    2015.10.26
   */
  //-----------------------------------------------------------------------------
  void MoveIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutHeroIcon
   * @brief   主人公アイコン表示切り替え
   * @date    2015.10.23
   *
   * @param   flg   true = 表示
   */
  //-----------------------------------------------------------------------------
  void PutHeroIcon( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveHeroIcon
   * @brief   主人公アイコン移動
   * @date    2015.10.26
   */
  //-----------------------------------------------------------------------------
  void MoveHeroIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitHeroIcon
   * @brief   主人公アイコン初期化
   * @date    2015.10.23
   */
  //-----------------------------------------------------------------------------
  void InitHeroIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPlaceIcon
   * @brief   場所アイコン表示切り替え
   * @date    2015.10.23
   *
   * @param   flg   true = 表示
   */
  //-----------------------------------------------------------------------------
  void PutPlaceIcon( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPlaceIconType
   * @brief   場所アイコン表示タイプ切り替え
   * @date    2015.11.04
   */
  //-----------------------------------------------------------------------------
  void SetPlaceIconType(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MovePlaceIcon
   * @brief   場所アイコン移動
   * @date    2015.10.26
   */
  //-----------------------------------------------------------------------------
  void MovePlaceIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPlaceIconData
   * @brief   場所アイコンのデータを設定
   * @date    2015.10.26
   *
   * @param   is_zoom   true = 拡大
   */
  //-----------------------------------------------------------------------------
  void SetPlaceIconData( bool is_zoom );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPlaceIcon
   * @brief   場所アイコン初期化
   * @date    2015.10.29
   */
  //-----------------------------------------------------------------------------
  void InitPlaceIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPosIconButtonID
   * @brief   カーソル位置のボタンIDを取得
   * @date    2015.10.26
   *
   * @param   is_active   ture = アクティブなボタンのみ取得
   *
   * @return  ボタンID
   */
  //-----------------------------------------------------------------------------
  u32 GetCursorPosIconButtonID( bool is_active );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateWorld
   * @brief   ワールドデータ生成
   * @date    2015.10.30
   */
  //-----------------------------------------------------------------------------
  void CreateWorld(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteWorld
   * @brief   ワールドデータ削除
   * @date    2015.10.30
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteWorld(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitializeWorld
   * @brief   ワールドデータ設定
   * @date    2015.10.30
   *
   * @param   land  島番号
   */
  //-----------------------------------------------------------------------------
  void InitializeWorld( u32 land );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsInitializeWorld
   * @brief   ワールドデータの設定が完了したか
   * @date    2015.10.30
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsInitializeWorld(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetWorld
   * @brief   ワールドデータ再設定
   * @date    2015.10.30
   *
   * @param   land  島番号
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool ResetWorld( u32 land );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateWorld
   * @brief   ワールドデータ自動更新
   * @date    2015.10.30
   */
  //-----------------------------------------------------------------------------
  void UpdateWorld(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateWorld
   * @brief   ワールドデータ切り替え
   * @date    2015.10.30
   *
   * @param   land  島番号
   */
  //-----------------------------------------------------------------------------
  void ChangeWorld( u32 land );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitZoneMask
   * @brief   到着フラグをチェックしてマスク表示
   * @date    2015.11.02
   */
  //-----------------------------------------------------------------------------
  void InitZoneMask(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutCursorPosArea
   * @brief   カーソル位置のエリア枠表示
   * @date    2015.11.02
   */
  //-----------------------------------------------------------------------------
  void PutCursorPosArea(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutLandName
   * @brief   島名表示
   * @date    2015.12.08
   */
  //-----------------------------------------------------------------------------
  void PutLandName(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishLandNameWindow
   * @brief   島名のウィンドウを非表示にする
   * @date    2015.12.08
   */
  //-----------------------------------------------------------------------------
  void VanishLandNameWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateAutoMoveCursor
   * @brief   カーソル吸着
   * @date    2015.12.08
   */
  //-----------------------------------------------------------------------------
  void UpdateAutoMoveCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorSpeed
   * @brief   カーソル速度取得
   * @date    2015.12.09
   *
   * @return  カーソル速度
   */
  //-----------------------------------------------------------------------------
  f32 GetCursorSpeed(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2015.10.19
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
   * @func    OnLayoutPaneStartingAnimeEvent
   * @brief   ペイン選択アニメーションの開始を検知
   * @date    2015.10.14
   *
   * @param   button_id   通知されたボタンID
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.10.14
   *
   * @param   button_id   通知されたボタンID
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.03.05
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
  CallMode m_callMode;    //!< 呼び出しモード

  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  u32 m_landNo;     //!< 島番号 ( 0オリジン )

  Savedata::FieldMenu * m_saveFieldMenu;            //!< フィールドメニューセーブデータ
  Savedata::MyStatus * m_myStatus;                  //!< 自機セーブデータ
  const Field::Location * m_fieldLocation;          //!< フィールドロケーション
  const Field::ZoneDataLoader * m_zoneDataLoader;   //!< ゾーンデータローダー
  const Field::EventWork * m_eventWork;             //!< イベントワーク

  gfl2::ui::VectorDevice * m_devKey;        //!< 十字キー
  gfl2::ui::Button * m_devButton;           //!< ボタン
  gfl2::ui::TouchPanel * m_devTouchPanel;   //!< タッチパネル

  SubScreenMapListener * m_listener;   //!< リスナー

  void * m_pLytResBuffer;   //!< レイアウトリソースバッファ

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  void * m_pResZoneData;          //!< タウンマップデータリソース：ゾーン
  ZONE_RESOURCE * m_resZoneData;  //!< タウンマップデータ：ゾーン
  u32 m_resZoneDataLen;           //!< ゾーンデータ数

  ICON_RESOURCE * m_resIconData;  //!< タウンマップデータ：アイコン
  u32 m_resIconDataLen;           //!< アイコンデータ数

  App::Tool::MapManager * m_pMapManager;  //!< マップマネージャ

  Field::EventList::EventListDataManager * m_pEventListDataManager;   //!< イベントリスト
  u32 m_mainEventID;      //!< メインイベントID
  u32 m_mainEventIndex;   //!< メインイベントインデックス

  Field::World * m_pWorld;  //!< ワールドクラス
  bool m_isWorldCreate;     //!< ワールドクラスを生成したか
  u32 m_chgWorldLandNo;     //!< ワールドデータの切り替え島番号

  bool m_isZoom;          //!< マップ表示の拡縮状態
  s32 m_modeChangeCount;  //!< マップ拡縮切り替えカウンタ

  ZoneID m_cursorZoneID;  //!< カーソル位置のゾーンID
  bool m_isZoneGetLock;   //!< true = ゾーン取得禁止

  bool m_isTouch;   //!< true = タッチ中
  u16 m_touchX;     //!< タッチX座標
  u16 m_touchY;     //!< タッチY座標

  f32 m_endCursorX;   //!< 拡縮後のカーソルX座標
  f32 m_endCursorY;   //!< 拡縮後のカーソルY座標
  f32 m_moveCursorX;  //!< 拡縮時のカーソルX移動値
  f32 m_moveCursorY;  //!< 拡縮時のカーソルY移動値

  f32 m_initLandMapPX;                  //!< 拡縮時の全体マップの初期X座標
  f32 m_initLandMapPY;                  //!< 拡縮時の全体マップの初期Y座標
  f32 m_moveLandMapX[MAP_ZOOM_COUNT];   //!< 拡縮時の全体マップのX座標テーブル
  f32 m_moveLandMapY[MAP_ZOOM_COUNT];   //!< 拡縮時の全体マップのY座標テーブル

  s16 m_landChangeMove;   //!< 島切り替え移動値
  s16 m_landMoveCount;    //!< 島切り替えカウンタ
  f32 m_landMoveX;        //!< 島移動X座標

  PLACE_ICON_BUTTON m_placeButton[PLACE_ICON_MAX];  //!< 場所アイコンボタン
  u32 m_selectButtonID;   //!< 選択されたボタンID

  u32 m_heroLandNo;     //!< 主人公がいる島番号
  f32 m_heroPX;         //!< 主人公のX座標
  f32 m_heroPY;         //!< 主人公のY座標
  bool m_isHeroRotate;  //!< 主人公の向きを表示するか

  u8 m_updateMain;      //!< 更新処理用メインシーケンス
  u8 m_updateSub;       //!< 更新処理用サブシーケンス
  u8 m_mainSeq;         //!< メインシーケンス
  u8 m_subSeq;          //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ

  bool m_isCursorAutoMoveEnable;  //!< true = カーソル自動移動可（そらをとぶアイコン吸着用）


/**
  デバッグ項目
*/
#if PM_DEBUG
  class Debug
  {
    GFL_FORBID_COPY_AND_ASSIGN(Debug);
  public:
    Debug(void);
    virtual ~Debug();
    
    void  SetOwner(TownMapPartsDraw* pOwner)  {m_pOwner = pOwner;}
    void  SetWorldPos(const s32 wX, const s32 wY) {m_WX=wX; m_WY=wY;}

  protected:
    TownMapPartsDraw*             m_pOwner;
    gfl2::debug::DebugWinGroup*   m_pGroup;
    s32                           m_WX, m_WY;

  private:
  }m_Debug;
#endif
};


GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)

#endif  // TOWNMAP_PARTS_DRAW_H_INCLUDED
