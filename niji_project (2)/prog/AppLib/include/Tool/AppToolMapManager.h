//=============================================================================
/**
 * @file    AppToolMapManager.h
 * @brief   2D分割マップ関連処理
 * @author  Hiroyuki Nakamura
 * @date    2015.08.25
 */
//=============================================================================
#if !defined( APP_TOOL_MAP_MANAGER_H_INCUDED )
#define APP_TOOL_MAP_MANAGER_H_INCUDED

#pragma once

// gflib2
#include <layout/include/gfl2_Layout.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>
// niji
#include "AppLib/include/Tool/app_tool_LytTexReplaceSystem.h"
#include "AppLib/include/Ui/UIResponder.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"
// resource
#include "niji_conv_header/field/subscreen_map_type/subscreen_map_type.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class G2DUtil;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class MapManager
 * @brief マップ処理
 * @date  2015.08.25
 */
//=============================================================================
class MapManager
{
  GFL_FORBID_COPY_AND_ASSIGN( MapManager );

public:
  //! イベントアイコンID
  enum EventIconID
  {
    EVENT_ICON_ID_MAIN = 0,   //!< メイン
    EVENT_ICON_ID_MAX,        //!< ID最大
  };

  //! 主人公アイコンID
  enum HeroIconID
  {
    HERO_ICON_ID_PLAYER = 0,    //!< 主人公
    HERO_ICON_ID_FINDER,        //!< ポケファインダー
    HERO_ICON_ID_FINDER_RARE,   //!< ポケファインダー（レア）
  };

  static const s32 MAP_3D_DEFAULT_X_SIZE = 37800;     //!< フィールドマップの基準X座標
  static const s32 MAP_3D_DEFAULT_Y_SIZE = 29400;     //!< フィールドマップの基準Y座標

  static const s32 MAP_2D_DEFAULT_X_SIZE = 778;       //!< マップの基準X座標
  static const s32 MAP_2D_DEFAULT_Y_SIZE = 606;       //!< マップの基準Y座標

  static const u32 MAP_ID_NONE = 0xffffffff;    //!< マップIDなし


private:
  static const s32 PANE_WIDTH  = 256;   //!< マップペインの幅
  static const s32 PANE_HEIGHT = 128;   //!< マップペインの高さ

  static const s32 PANE_H_MAX = 4;                      //!< マップペイン横配置数
  static const s32 PANE_V_MAX = 4;                      //!< マップペイン縦配置数
  static const s32 PANE_MAX = PANE_H_MAX * PANE_V_MAX;  //!< マップペイン配置総数

#if defined(GF_PLATFORM_CTR)
  //! マップテクスチャサイズ ( CTR )
//  static const u32 TEX_SIZE = 0x1000 + app::tool::LytTexReplaceSystem::BCLIM_FOOTER_SIZE;
  static const u32 TEX_SIZE = 0x4000 + app::tool::LytTexReplaceSystem::BCLIM_FOOTER_SIZE;
#elif defined(GF_PLATFORM_WIN32)
  //! マップテクスチャサイズ ( WIN )
//  static const u32 TEX_SIZE = ( 0x1000 + app::tool::LytTexReplaceSystem::BCLIM_FOOTER_SIZE ) * 10;
  static const u32 TEX_SIZE = ( 0x4000 + app::tool::LytTexReplaceSystem::BCLIM_FOOTER_SIZE ) * 10;
#endif

  static const s32 MAP_DATA_MAX = SUBSCREEN_MAP_TYPE_MAX - 1;   //!< マップデータ数

  //! マップデータ
  struct MAP_DATA
  {
    u16 data_id;      //!< データID
    u16 tex_id;       //!< 開始テクスチャID
    u16 tex_width;    //!< 横配置数
    u16 tex_height;   //!< 縦配置数
    u16 start_x;      //!< マップ開始X座標
    u16 start_y;      //!< マップ開始Y座標
    u16 end_x;        //!< マップ終了X座標
    u16 end_y;        //!< マップ終了Y座標
    s16 center_x;     //!< 中心X座標
    s16 center_y;     //!< 中心Y座標
  };

  static const MAP_DATA MapDataTable[MAP_DATA_MAX];   //!< マップデータテーブル

  static const s32 PANE_PUT_LIMIT_UP    = PANE_HEIGHT * ( PANE_V_MAX / 2 );   //!< ペイン配置限界座標（上）
  static const s32 PANE_PUT_LIMIT_DOWN  = -PANE_PUT_LIMIT_UP;                 //!< ペイン配置限界座標（下）
  static const s32 PANE_PUT_LIMIT_RIGHT = PANE_WIDTH * ( PANE_H_MAX / 2 );    //!< ペイン配置限界座標（右）
  static const s32 PANE_PUT_LIMIT_LEFT  = -PANE_PUT_LIMIT_RIGHT;              //!< ペイン配置限界座標（左）

  //! テクスチャ差し替えワーク
  struct TexRepWork
  {
    u32 seq;      //!< シーケンス
    u32 req_id;   //!< リクエストID
    u32 tex_id;   //!< 現在のテクスチャID
  };
  
  static const u32 TEXREP_REQ_ID_NONE = 0xffffffff;   //!< テクスチャ差し替えリクエストIDなし

  static const f32 MAP_SCALE_H;   //!< マップ横拡縮値
  static const f32 MAP_SCALE_V;   //!< マップ縦拡縮値
  static const f32 FIELD_SCALE_H; //!< フィールド横拡縮値
  static const f32 FIELD_SCALE_V; //!< フィールド縦拡縮値

  //! イベントアイコンデータ
  struct EventIcon
  {
    gfl2::lyt::LytPaneIndex parent; //!< 親ペイン
//    u32 arrow_on_anime;     //!< 矢印表示アニメ
//    u32 arrow_off_anime;    //!< 矢印非表示アニメ
  };

  static const EventIcon EventIconTable[EVENT_ICON_ID_MAX];   //!< イベントアイコンデータテーブル

  //! イベントアイコンワーク
  struct EventIconWork
  {
    f32 x;                //!< X座標
    f32 y;                //!< Y座標
    bool visible;         //!< 表示フラグ
//    bool visible_req;     //!< 表示リクエストフラグ
//    u32 last_arrow_anime; //!< 最後に再生した矢印表示アニメ
  };

  static const f32 HERO_ROTATE_MOVE_MAX;  //!< 主人公の向きを１フレームで変更できる最大角度


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap      ヒープ
   * @param   g2d       G2DUtil
   * @param   is_navi   true = ナビ画面, false = それ以外
   */
  //-----------------------------------------------------------------------------
  MapManager( app::util::Heap * heap, app::util::G2DUtil * g2d, bool is_navi );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MapManager();

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新処理
   * @date    2015.08.26
   */
  //-----------------------------------------------------------------------------
  void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画処理
   * @date    2015.08.26
   *
   * @param   render_man  描画マネージャ
   * @param   disp_no     ディスプレイ番号
   * @param   order       描画順
   * @param   layer_no    レイヤー番号
   */
  //-----------------------------------------------------------------------------
  void Draw( app::util::AppRenderingManager * render_man, gfl2::gfx::CtrDisplayNo disp_no, u8 order=gfl2::lyt::ORDER_NORMAL, u32 layer_no=0 );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisible
   * @brief   表示切り替え
   * @date    2015.08.26
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisible( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize
   * @brief   初期化
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Initialize(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitializeSync
   * @brief   初期化（同期版）
   * @date    2016.02.23
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool InitializeSync(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate
   * @brief   終了処理
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetLayoutResource
   * @brief   レイアウトリソース取得
   * @date    2015.09.04
   *
   * @return  レイアウトリソース
   */
  //-----------------------------------------------------------------------------
  void * GetLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetLayoutResourceID
   * @brief   レイアウトリソースID取得
   * @date    2015.09.04
   *
   * @return  レイアウトリソースID
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytMultiResID * GetLayoutResourceID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMapTextureReplace
   * @brief   画面内のテクスチャを変更中か
   * @date    2015.09.05
   *
   * @retval  true  = 変更中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsMapTextureReplace(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcIconPosition
   * @brief   フィールドの座標をマップ座標に変換（アイコン用）
   * @date    2016.03.03
   *
   * @param   x   X座標
   * @param   y   Y座標
   * @param   cx  中心X座標
   * @param   cy  中心Y座標
   */
  //-----------------------------------------------------------------------------
  void CalcIconPosition( f32 * x, f32 * y, f32 cx, f32 cy );

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   マップデータ設定
   * @date    2015.08.26
   *
   * @param   id  マップID
   * @param   x   中心X座標
   * @param   y   中心Y座標
   */
  //-----------------------------------------------------------------------------
  void Setup( u32 id, f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupEventArea
   * @brief   イベントアイコンの表示範囲を設定（範囲指定）
   * @date    2015.09.04
   *
   * @param   up      上
   * @param   down    下
   * @param   left    左
   * @param   right   右
   */
  //-----------------------------------------------------------------------------
  void SetupEventArea( f32 up, f32 down, f32 left, f32 right );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupEventArea
   * @brief   イベントアイコンの表示範囲を設定（ペインから）
   * @date    2015.09.07
   */
  //-----------------------------------------------------------------------------
  void SetupEventArea(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupPlayerSex
   * @brief   主人公の性別を設定
   * @date    2015.09.07
   *
   * @param   sex   性別
   */
  //-----------------------------------------------------------------------------
  void SetupPlayerSex( u32 sex );

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetMapData
   * @brief   マップデータを再設定
   * @date    2015.09.02
   *
   * @param   id  マップID
   * @param   x   中心X座標
   * @param   y   中心Y座標
   */
  //-----------------------------------------------------------------------------
  void ResetMapData( u32 id, f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdatePosition
   * @brief   座標更新
   * @date    2015.08.26
   *
   * @param   id  マップID
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void UpdatePosition( u32 id, f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    MovePosition
   * @brief   座標変更
   * @date    2015.10.21
   *
   * @param   x   X移動値
   * @param   y   Y移動値
   */
  //-----------------------------------------------------------------------------
  void MovePosition( f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutHeroIcon
   * @brief   主人公アイコン配置
   * @date    2015.08.31
   *
   * @param   x   X座標
   * @param   y   Y座標
   * @param   r   向き
   */
  //-----------------------------------------------------------------------------
  void PutHeroIcon( f32 x, f32 y, f32 r );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleHeroIcon
   * @brief   主人公アイコン表示切り替え
   * @date    2015.08.31
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleHeroIcon( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleHeroArrow
   * @brief   主人公の向きの表示切り替え
   * @date    2015.09.09
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleHeroArrow( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutEventIcon
   * @brief   イベントアイコン配置
   * @date    2015.08.31
   *
   * @param   id  イベントアイコンID
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void PutEventIcon( EventIconID id, f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleEventIcon
   * @brief   イベントアイコン表示切り替え
   * @date    2015.08.31
   *
   * @param   id    イベントアイコンID
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleEventIcon( EventIconID id, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButtonInfo
   * @brief   ボタンデータ生成
   * @date    2015.09.07
   *
   * @param   info  生成場所
   * @param   id    ボタンID
   */
  //-----------------------------------------------------------------------------
  void CreateButtonInfo( app::ui::ButtonInfoLytWk * info, app::tool::ButtonManager::ButtonId id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCenterPos
   * @brief   中心座標取得
   * @date    2015.09.12
   *
   * @param   x   X座標取得場所
   * @param   y   Y座標取得場所
   */
  //-----------------------------------------------------------------------------
  void GetCenterPos( f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneSizeNavi
   * @brief   ナビ用のペインサイズ設定
   * @date    2016.01.09
   */
  //-----------------------------------------------------------------------------
  void SetPaneSizeNavi(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetCenterSRT
   * @brief   中心のSRTをセット
   * @date    2016.01.12
   *
   * @param   scale   拡縮
   * @param   rotate  回転
   * @param   trans   座標
   */
  //-----------------------------------------------------------------------------
  void SetCenterSRT( gfl2::math::VEC2 * scale, gfl2::math::VEC3 * rotate, gfl2::math::VEC3 * trans );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetCenterSRT
   * @brief   中心のSRTをセット
   * @date    2016.01.12
   *
   * @param   mtx   グローバルマトリックス
   */
  //-----------------------------------------------------------------------------
//  void SetCenterSRT( const gfl2::math::MTX34 & mtx );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeHeroIcon
   * @brief   主人公アイコンの表示を変更
   * @date    2016.01.14
   *
   * @param   id  アイコンID
   */
  //-----------------------------------------------------------------------------
  void ChangeHeroIcon( HeroIconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishFinderButtonPane
   * @brief   ファインダーアイコンのボタンペインを非表示にする
   * @date    2017.03.15
   */
  //-----------------------------------------------------------------------------
  void VanishFinderButtonPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMapDataIndex
   * @brief   マップデータのインデックスを取得
   * @date    2016.03.03
   *
   * @param   type  SubScreenMapType
   *
   * @return  マップデータのインデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetMapDataIndex( SubScreenMapType type );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResource
   * @brief   レイアウトリソース読み込み
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResourceSync
   * @brief   レイアウトリソース読み込み（同期版）
   * @date    2016.02.23
   */
  //-----------------------------------------------------------------------------
  void LoadLayoutResourceSync(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadLayoutResource
   * @brief   レイアウトリソース削除
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OpenTextureFile
   * @brief   マップテクスチャファイルを開く
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool OpenTextureFile(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OpenTextureFileSync
   * @brief   マップテクスチャファイルを開く（同期版）
   * @date    2016.02.23
   */
  //-----------------------------------------------------------------------------
  void OpenTextureFileSync(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CloseTextureFile
   * @brief   マップテクスチャファイルを閉じる
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CloseTextureFile(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayout
   * @brief   レイアウト生成
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteLayout
   * @brief   レイアウト削除
   * @date    2015.08.26
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMapPane
   * @brief   マップのペインを取得
   * @date    2015.12.02
   */
  //-----------------------------------------------------------------------------
  void InitMapPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateTextureReplaceSystem
   * @brief   テクスチャ管理システム生成
   * @date    2015.08.26
   */
  //-----------------------------------------------------------------------------
  void CreateTextureReplaceSystem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteTextureReplaceSystem
   * @brief   テクスチャ管理システム削除
   * @date    2015.08.26
   */
  //-----------------------------------------------------------------------------
  void DeleteTextureReplaceSystem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsTextureReplaceSystemFree
   * @brief   テクスチャ管理システムを削除できるか
   * @date    2015.09.10
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsTextureReplaceSystemFree(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateTextureReplace
   * @brief   マップテクスチャ更新（全体）
   * @date    2015.08.27
   */
  //-----------------------------------------------------------------------------
  void UpdateTextureReplace(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateTextureReplace
   * @brief   マップテクスチャ更新（個別）
   * @date    2015.08.27
   *
   * @param   id  ワークID
   *
   * @retval  false = 読み込み中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UpdateTextureReplace( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequestTextureReplace
   * @brief   マップテクスチャ変更リクエスト
   * @date    2015.08.27
   *
   * @param   buf_id  ワークID
   * @param   tex_id  テクスチャID
   */
  //-----------------------------------------------------------------------------
  void SetRequestTextureReplace( u32 buf_id, u32 tex_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcPosition
   * @brief   フィールドの座標をマップ座標に変換
   * @date    2015.09.01
   *
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void CalcPosition( f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcField
   * @brief   マップの座標をフィールド座標に変換
   * @date    2015.09.12
   *
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void CalcField( f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    AdjustCenter
   * @brief   中心座標を補正
   * @date    2015.09.12
   *
   * @param   x   中心X座標
   * @param   y   中心Y座標
   */
  //-----------------------------------------------------------------------------
  void AdjustCenter( f32 * x, f32 * y );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetBasePaneIndex
   * @brief   基準マップペインのインデックスを取得
   * @date    2015.09.12
   *
   * @return  ペインテーブルのインデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetBasePaneIndex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPanePos
   * @brief   マップペインの座標を初期化
   * @date    2015.08.26
   */
  //-----------------------------------------------------------------------------
  void InitPanePos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMapPane
   * @brief   マップペインを取得
   * @date    2015.08.27
   *
   * @param   id    ペインテーブルID
   *
   * @return  ペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPicture * GetMapPane( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsDispAreaPane
   * @brief   画面内のマップペインか
   * @date    2015.08.28
   *
   * @param   id    ペインテーブルID
   *
   * @retval  true  = 画面内のペイン
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsDispAreaPane( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartAnimeFrame
   * @brief   アニメ開始
   * @date    2015.09.02
   *
   * @param   anm     アニメ番号
   * @param   frame   開始フレーム
   */
  //-----------------------------------------------------------------------------
  void StartAnimeFrame( u32 anm, f32 frame );

  //-----------------------------------------------------------------------------
  /**
   * @func    StopAnime
   * @brief   アニメ停止
   * @date    2015.09.02
   *
   * @param   anm   アニメ番号
   */
  //-----------------------------------------------------------------------------
  void StopAnime( u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsAnimeEnd
   * @brief   アニメが終了したか
   * @date    2015.09.02
   *
   * @param   anm   アニメ番号
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsAnimeEnd( u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetAnimeMaxFrame
   * @brief   アニメの最大フレームを取得
   * @date    2015.09.03
   *
   * @param   anm   アニメ番号
   *
   * @return  最大フレーム
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimeMaxFrame( u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitIcon
   * @brief   アイコン初期化
   * @date    2015.09.02
   */
  //-----------------------------------------------------------------------------
  void InitIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitHeroIcon
   * @brief   主人公アイコン初期化
   * @date    2015.09.07
   */
  //-----------------------------------------------------------------------------
  void InitHeroIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitEventIcon
   * @brief   イベントアイコン初期化
   * @date    2015.09.02
   *
   * @param   id  イベントアイコンID
   */
  //-----------------------------------------------------------------------------
  void InitEventIcon( EventIconID id );


private:
  app::util::Heap * m_heap;         //!< ヒープ
  app::util::G2DUtil * m_g2dUtil;   //!< G2DUtil

  bool m_isNaviMode;    //!< true = ナビ画面

  u16 m_mainSeq;    //!< メインシーケンス
  u16 m_subSeq;     //!< サブシーケンス

  void * m_pLytResBuff;   //!< レイアウトリソースバッファ
  bool m_isVisible;       //!< 表示フラグ

  gfl2::lyt::LytSys * m_lytsys;       //!< レイアウトシステム
  gfl2::lyt::LytWk * m_pLytWk;        //!< レイアウトワーク
  gfl2::lyt::LytMultiResID m_resID;   //!< リソースID

  gfl2::lyt::LytPane * m_centerPane;            //!< 中心ペイン
  gfl2::lyt::LytPicture * m_mapPane[PANE_MAX];  //!< マップペイン

  app::tool::LytTexReplaceSystem * m_pTexReplaceSystem;   //!< テクスチャシステム
  TexRepWork m_texRepWork[PANE_MAX];                      //!< テクスチャ差し替えワーク
//  s32 m_basePaneIndex;
  bool m_isTexReplace;                                    //!< 画面内のテクスチャを差し替え中か

  u32 m_paneWidth;    //!< マップペインの幅
  u32 m_paneHeight;   //!< マップペインの高さ

  s32 m_panePutLimitUp;     //!< ペイン配置限界座標（上）
  s32 m_panePutLimitDown;   //!< ペイン配置限界座標（下）
  s32 m_panePutLimitRight;  //!< ペイン配置限界座標（右）
  s32 m_panePutLimitLeft;   //!< ペイン配置限界座標（左）

  f32 m_mapScaleH;      //!< マップ横拡縮値
  f32 m_mapScaleV;      //!< マップ縦拡縮値
  f32 m_fieldScaleH;    //!< フィールド横拡縮値
  f32 m_fieldScaleV;    //!< フィールド縦拡縮値

  MAP_DATA m_mapData;           //!< マップデータ
  u32 m_mapID;                  //!< マップID
  f32 m_centerX;                //!< 中心X座標
  f32 m_centerY;                //!< 中心Y座標
  f32 m_centerMinX;             //!< 中心X座標最小値
  f32 m_centerMinY;             //!< 中心Y座標最小値
  f32 m_centerMaxX;             //!< 中心X座標最大値
  f32 m_centerMaxY;             //!< 中心Y座標最大値

  f32 m_evLimUp;                //!< イベントアイコンの表示限界座標（上）
  f32 m_evLimDown;              //!< イベントアイコンの表示限界座標（下）
  f32 m_evLimLeft;              //!< イベントアイコンの表示限界座標（左）
  f32 m_evLimRight;             //!< イベントアイコンの表示限界座標（右）

  EventIconWork m_eventIconWork[EVENT_ICON_ID_MAX];   //!< イベントアイコンワーク

  bool m_isInitialize;  //!< 初期化されたか
  bool m_isTerminate;   //!< 終了されたか

  u32 m_playerSex;    //!< 主人公の性別

  bool m_isScaleLinear;   //!< true = ペインの拡縮モードを変更

  u32 m_appScaleA;  //!< アプリごとの拡縮計算値
  u32 m_appScaleB;  //!< アプリごとの拡縮計算値
};

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // APP_TOOL_MAP_MANAGER_H_INCUDED
