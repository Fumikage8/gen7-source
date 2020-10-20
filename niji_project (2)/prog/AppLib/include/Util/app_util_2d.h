//============================================================================================
/**
 * @file    app_util_2d.h
 * @brief   2D関連
 * @author  Hiroyuki Nakamura
 * @date    11.10.04
 */
//============================================================================================
#if !defined( __APP_UTIL_2D_H__ )
#define __APP_UTIL_2D_H__
#pragma once

// gflib2
#include <fs/include/gfl2_Fs.h>
#include <Layout/include/gfl2_Layout.h>
#include <Layout/include/gfl2_LayoutRenderPath.h>
// niji
#include "System/include/PokemonVersion.h"
#include "GameSys/include/GameManager.h"
#include "Print/include/PrintSystem.h"
#include "Print/include/MessageWindow.h"
#include "Print/include/WordSet.h"
#include "Print/include/WordSetLoader.h"
#include "Savedata/include/MyStatus.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_AppLytAccessor.h"
#include "AppLib/include/Tool/app_tool_MsgCursor.h"


namespace app {

namespace tool{
  class LytTexReplaceSystem;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	util
 * @brief			ユーティリティ処理
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace util {

	class AppRenderingManager;

//--------------------------------------------------------------------------------------------
/**
 * @brief	2D関連ユーティリティ処理
 */
//--------------------------------------------------------------------------------------------
class G2DUtil {
	GFL_FORBID_COPY_AND_ASSIGN( G2DUtil );

public:

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief	表示ディスプレイ定義
	 */
	//--------------------------------------------------------------------------------------------
  typedef enum {
    SETUP_UPPER = 0,	//!< 上画面
    SETUP_LOWER,			//!< 下画面
    SETUP_MAX					//!< 最大値
  }SETUP_DISPLAY_TYPE;

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief	レイアウトリソースデータ
	 */
	//--------------------------------------------------------------------------------------------
  struct LytResData{
    const void * buf;                      //!< リソースデータ  // applytバイナリ
    u32  addTexNum;                        //!< 指定のリソースに追加登録するリソース数の最大数
    u32  addRegisterFlag;                  //!< ０番目のリソースに連結して一括転送対象にするか
                                           //!< （ATTACH_ROOT=連結元,ATTACH_RESOURCE=連結する共通リソース
  };

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief	レイアウトワーク管理
	 */
	//--------------------------------------------------------------------------------------------
  typedef struct {
    gfl2::lyt::LytWk* pLytWk;    //!< レイアウトワーク
    SETUP_DISPLAY_TYPE    setupDisp; //!< 描画画面
    bool                  enable;    //!< 表示設定
  }LAYOUT_WORK;

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief	レイアウトワーク設定データ
	 */
	//--------------------------------------------------------------------------------------------
  //! レイアウト生成データ
  struct LytwkData
  {
    u32 res_index;                      //!< 使用するリソースの番号
    gfl2::lyt::LytArcIndex lytres_id;   //!< レイアウトデータID ( *_BFLYT )
    u32 anm_max;                        //!< アニメ数
    const void * buf;                   //!< リソースデータ
    bool calculateMtxFlag;              //!< 非表示ペインに行列計算を行うか
    SETUP_DISPLAY_TYPE  setupDisp;      //!< ディスプレイ
    bool enable;                        //!< 表示設定
  };

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief	親子ペイン追加モード
	 */
	//--------------------------------------------------------------------------------------------
  // u32として扱うことが多いのでenumをやめた。
  static const u32  APPEND_CHILD_MODE_BOTTOM = 0;           //!< 末尾
  static const u32  APPEND_CHILD_MODE_TOP    = 1;           //!< 先頭
  static const u32  APPEND_CHILD_MODE_INSERT = 2;           //!< 位置指定
  static const u32  APPEND_CHILD_INSERT_ROOT = 0xffffffff;  //!< ルートペインを親に指定

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief	共通リソース用定義（LytResDataのaddRegisterFlagに格納する値)
	 */
	//--------------------------------------------------------------------------------------------
  // u32として扱うことが多いのでenumをやめた。
  static const u32  ATTACH_ROOT     = 0xffffffff;  //!< 連結先になるリソース
  static const u32  ATTACH_NONE     = 0;           //!< 連結とは関係ないリソース
  static const u32  ATTACH_RESOURCE = 1;           //!< 連結対象になるリソース

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief	マテリアルカラー変更定義
	 */
	//--------------------------------------------------------------------------------------------
  enum {
    COLOR_INDEX_BLACK = nw::lyt::INTERPOLATECOLOR_BLACK,		//!< 黒カラー補間
    COLOR_INDEX_WHITE = nw::lyt::INTERPOLATECOLOR_WHITE,				//!< 白カラー補間
  };

private:
  static const u32 LYT_RES_BUFFER_ARC_NONE = 0xffffffff;	//!< リソースデータのアークIDなし定義

  static const u32 CREATE_ANIME_RECURSIVE_LIMIT = 4;  // 部品アニメ生成関数が再帰できる回数

  static const u32 TEMP_STRBUF_NUM = 2;         //!< 汎用文字列バッファ数
  static const u32 TEMP_STRBUF_LENGTH = 512;    //!< 汎用文字列バッファの文字数


public:
  static const u32 DEFAULT_LYTSYS_SETUP_RES_MAX = 128;  //!< レイアウトシステムの管理リソース最大数
  static const u32 DEFAULT_LYTSYS_SETUP_WORK_MAX = 10;  //!< レイアウトシステムの管理ワーク最大数

  static const b32 DEFAULT_CREATE_PANETBL_USE      = false; //!ペインテーブルを作成するか　デフォルト引数で指定している
  static const b32 DEFAULT_CREATE_PANETBL_HASH_USE = false; //!ペインテーブル生成時にハッシュリストを作成するか　デフォルト引数で指定している

  static const u32 APP_UTIL_LYRES_MAX = 20; //!< AttachLayoutResourceReferenceで使用するLYT_DATAの最大数


public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap 内部で使用するヒープ情報
   */
  //--------------------------------------------------------------------------------------------
  G2DUtil( Heap * heap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  virtual ~G2DUtil();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アップデート
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void Update2D(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク描画
   *
   * @param   AppRM       描画マネージャ
   * @param   displayNo   描画面
   * @param   id          描画するレイアウトのID
   * @param   order       描画順(デフォルト：ORDER_NORMAL(128))
   * @param   layerNo     レイヤーNo
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void AddDrawableLytWkForOneFrame( app::util::AppRenderingManager* AppRM, gfl2::gfx::CtrDisplayNo displayNo, u32 id, u8 order = gfl2::lyt::ORDER_NORMAL, u32 layerNo = 0 );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトシステム作成
   *
   * @param   heap    ヒープ
   * @param   resMax  管理リソース最大数　デフォルト = DEFAULT_LYTSYS_SETUP_RES_MAX
   * @param   wkMax   管理レイアウトワーク最大数　デフォルト = DEFAULT_LYTSYS_SETUP_WORK_MAX
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateLayoutSystem( Heap * heap, u32 resMax = DEFAULT_LYTSYS_SETUP_RES_MAX, u32 wkMax = DEFAULT_LYTSYS_SETUP_WORK_MAX );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトシステム削除
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteLayoutSystem(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  レイアウトシステムを設定（外部レイアウトシステムを使用する）
   *
   * @param   lytSys 外部レイアウトシステムへのポインタ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetLayoutSystem( gfl2::lyt::LytSys* lytSys );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトシステム取得
   *
   * @param   none
   *
   * @return  レイアウトシステム
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytSys * GetLayoutSystem(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   レイアウト環境作成
	 *
	 * @param   heap					ヒープ
	 * @param   isStereoView	3D表示を有効にするか　※デフォルト = true
	 * @param		projection		プロジェクションパラメータ　※デフォルト = NULL
	 * @param		view					ビューパラメータ　※デフォルト = NULL
	 *
	 * @return  none
	 */
	//--------------------------------------------------------------------------------------------
	void CreateLayoutEnv(
				Heap * heap, bool isStereoView = true,
				const gfl2::lyt::ProjectionParam * projection = NULL, const gfl2::lyt::ViewParam * view = NULL );

	//--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウト環境作成
   *
   * @param   heap        ヒープ
   * @param   isStereoView 3D表示を有効にするか
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateLayoutEnvPerspective( Heap * heap, bool isStereoView = true );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウト環境削除
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteLayoutEnv(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク作成（リソースのバイナリの読み込みは外部で済んでいるとき）
   *
   * @param   resTbl        レイアウトリソースデータ
   * @param   tblMax        上記配列の数
   * @param   lytMax        作成数
   * @param   heap          ヒープ
   * @param		area			    転送エリア
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateLayoutWork( const LytResData * resTbl, u32 tblMax, u32 lytMax, Heap * heap, gfl2::lyt::MemoryArea area = gfl2::lyt::MEMORY_AREA_FCRAM );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク削除（リソース削除込み）
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteLayoutWork(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク作成
   *
   * @param   lytMax	作成数
   * @param   heap		ヒープ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateLayoutWork( u32 lytMax, Heap * heap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク削除
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteLayoutWorkCore(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソース作成
   *
   * @param   lytMax	作成数
   * @param   heap		ヒープ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateLayoutResource( u32 max, Heap * heap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソース削除
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteLayoutResource(void);
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief レイアウトリソース　バイナリ直接渡し版
   *
   * @param resID     リソースID
   * @param buff      アーカイブバイナリ
   * @param addTextureNum テクスチャ追加数
   * @param heap          ヒープ
   */
  //--------------------------------------------------------------------------------------------
  void LoadLayoutResourceBinary( u32 resID, const void *buff, u32 addTextureNum, Heap * heap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソース追加登録
   *
   * @param   srcID   追加するリソースID
   * @param   dstID   追加先のリソースID
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void RegistLayoutResource( u32 srcID, u32 dstID );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   SetLayoutWorkを呼び出す際に対象になるLytResDataを参照して連結する
   *          (内部的にはRegisterLayoutResourceをATTACH_ROO指定されているLytResDataに対して呼び出します）
   *
   * @param   resTbl        レイアウトリソースデータ
   * @param   tblMax        上記配列の数
   *
   * @note 旧処理のコメントをコピペ
   *  LYT_RES_DATA lytdata={
   *    { ARCID_GTS_GRA,         0, ID_MAX, ATTACH_ROOT },  // ARCID, ARCの中のBCLYTのID, 追加登録する最大数, 追加登録するか？(true,false)
   *    { ARCID_COMMON_MSGWIN,   0, 0,      ATTACH_RESOURCE  },  // ATTACH_ROOT指定されているレイアウトデータに連結される共通リソース
   *    { ARCID_COMMON_TOUCHBAR, 0, 0,      ATTACH_RESOURCE  },  // ATTACH_ROOT指定されているレイアウトデータに連結される共通リソース
   *    { ARCID_POKEICON,        0, 0,      ATTACH_NONE      },  // 連結対象とは関係なし
   *  };
   *  という風にLYT_RES_DATAを定義しておいてこの関数呼び出すと
   *  ARCID_GTS_GRAに対してARCID_COMMON_MSGWIN,ARCID_COMMON_TOUCHBARが連結され、
   *  ARCID_POKEICONは連結されない(4番目のメンバがATTACH_NONEだから）
   *  実際の定義はnamespaceも記述が必要です。
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void G2DUtil::AttachLayoutResourceReference( const LytResData * resTbl, u32 tblMax );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースID取得
   *
   * @param   index		登録インデックス
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytMultiResID& GetLayoutResourceID( u32 index );
  gfl2::lyt::LytMultiResID* GetLayoutResourceID();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースID設定
   *
   * @param   pLayoutResouceID		登録インデックス
   */
  //--------------------------------------------------------------------------------------------
  void SetLayoutResourceID( gfl2::lyt::LytMultiResID* pLayoutResouceID );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テクスチャ転送
   */
  //--------------------------------------------------------------------------------------------
  void TransferLayoutTexture(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   外部レイアウトを設定
   *          （※解放しないように！）
   *
   * @param   lytMax  レイアウト最大数
   * @param   lytWork レイアウトワーク
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetLayoutWork( LAYOUT_WORK *lytWork, const u32 lytMax );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク取得
   *
   * @param   id	レイアウトのID
   *
   * @return  レイアウトワークのポインタ
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytWk * GetLayoutWork( u32 id ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウト表示設定
   *
   * @param   id		レイアウトのID
   * @param   flg		true = 表示, false = 非表示
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetLayoutDrawEnable( u32 id, bool flg );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウトワークのセットアップデータ取得
   *
   * @param   disp	ディスプレイ
   *
   * @return	セットアップデータ
   */
  //--------------------------------------------------------------------------------------------
  const gfl2::lyt::LytWkSetUp * GetLayoutWorkSetup( gfl2::lyt::DisplayType disp );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   レイアウト表示状態取得
   *
   * @param   id	レイアウトのID
   *
   * @retval  "true = 表示"
   * @retval  "false = 非表示"
   */
  //--------------------------------------------------------------------------------------------
  bool GetLayoutDrawEnable( u32 id ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペイン表示設定
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   flg     true = 表示, false = 非表示
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetPaneVisible( u32 lytID, gfl2::lyt::LytPaneIndex index, bool flg );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペイン表示設定(ペインポインタ版)
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   flg     true = 表示, false = 非表示
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetPaneVisible( gfl2::lyt::LytPane* pane, bool flg );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペイン表示状態取得
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   *
   * @retval  "true = 表示"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool GetPaneVisible( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペイン表示状態取得(ペインポインタ版)
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   *
   * @retval  "true = 表示"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool GetPaneVisible( gfl2::lyt::LytPane* pane ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ペインの矩形を取得する
   * @param layout_work   ペインが所属するレイアウトワーク
   * @param pane_index    対象ペインのインデックス
   * @return ペインの矩形サイズ
   */
  //--------------------------------------------------------------------------------------------
  const nw::ut::Rect GetPaneRect( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのアルファ値設定
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   alpha   アルファ値
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetPaneAlpha( u32 lytID, gfl2::lyt::LytPaneIndex index, u8 alpha );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのアルファ値設定(ペイン直接指定版)
   *
   * @param		pane		ペイン
   * @param   alpha   アルファ値
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetPaneAlpha( gfl2::lyt::LytPane* pane, u8 alpha );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのアルファ値を取得
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   *
   * @return  アルファ値
   */
  //--------------------------------------------------------------------------------------------
  u8 GetPaneAlpha( u32 lytID, gfl2::lyt::LytPaneIndex index );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのアルファ値を取得(ペイン直接指定版)
   *
   * @param		pane		ペイン
   *
   * @return  アルファ値
   */
  //--------------------------------------------------------------------------------------------
  u8 GetPaneAlpha( gfl2::lyt::LytPane* pane );

  //-----------------------------------------------------------------------------
  /**
   * @brief ピクチャーペインの頂点カラー成分を設定する
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   color   カラー値
   */
  //-----------------------------------------------------------------------------
  void SetVtxColorElementOfPicturePane( u32 lytID, gfl2::lyt::LytPaneIndex index, nw::ut::Color8 color );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		マテリアルカラーを設定
   *
   * @param		lytID		レイアウトID
   * @param		pane		ペイン
   * @param		index		設定するカラーのインデックス
   * @param		color		カラー
   *
   * @return  none
   *
   * @li	LayoutEditor上のカラー補間の変更
   * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
   * @li	index = COLOR_INDEX_WHITE : 白カラー補間
   */
  //--------------------------------------------------------------------------------------------
  void SetMaterialColor( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 index, nw::ut::Color8 color );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		マテリアルカラーを設定
   *
   * @param		pane		ペイン
   * @param		index		設定するカラーのインデックス
   * @param		color		カラー
   *
   * @return  none
   *
   * @li	LayoutEditor上のカラー補間の変更
   * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
   * @li	index = COLOR_INDEX_WHITE : 白カラー補間
   */
  //--------------------------------------------------------------------------------------------
  void SetMaterialColor( gfl2::lyt::LytPane* pane, u32 index, nw::ut::Color8 color );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		マテリアルカラーを取得
   *
   * @param		lytID		レイアウトID
   * @param		pane		ペイン
   * @param		index		設定するカラーのインデックス
   *
   * @return  カラー
   *
   * @li	LayoutEditor上のカラー補間の変更
   * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
   * @li	index = COLOR_INDEX_WHITE : 白カラー補間
   */
  //--------------------------------------------------------------------------------------------
  const nw::ut::Color8 & GetMaterialColor( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 index );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		マテリアルカラーを取得
   *
   * @param		pane		ペインのアドレス
   * @param		index		設定するカラーのインデックス
   *
   * @return  カラー
   *
   * @li	LayoutEditor上のカラー補間の変更
   * @li	index = COLOR_INDEX_BLACK : 黒カラー補間
   * @li	index = COLOR_INDEX_WHITE : 白カラー補間
   */
  //--------------------------------------------------------------------------------------------
  const nw::ut::Color8 & GetMaterialColor( gfl2::lyt::LytPane* pane, u32 index );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインの座標取得
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   *
   * @return  ペインの座標
   */
  //--------------------------------------------------------------------------------------------
  gfl2::math::VEC3 GetPanePos( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインの座標取得（ポインタ指定版）
   *
   * @param   pane  ペイン
   *
   * @return  ペインの座標
   *
   * @note  親がいないペインは取得できない
   */
  //--------------------------------------------------------------------------------------------
  gfl2::math::VEC3 GetPanePos( const gfl2::lyt::LytPane * pane ) const;


  // つくろうと思って中断した。.cppに書いていない。
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのレイアウト座標をGFベクトル3の形式で取得する（レイアウト座標とは、画面の中心が(0,0,0)、下画面なら右下が(160,-120,0)、となるもの）
   *
   * @param   lytID   レイアウトのID
   * @param   lytwk   レイアウトワークのポインタ
   * @param   index   ペインのインデックス
   * @param   pane    ペインのポインタ
   *
   * @return  ペインのレイアウト座標（GFベクトル3の形式）
   */
  //--------------------------------------------------------------------------------------------
/*
  gfl2::math::Vector3 GetPaneLayoutPosByGfVec3( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;
  gfl2::math::Vector3 GetPaneLayoutPosByGfVec3( u32 lytID, const gfl2::lyt::LytPane * pane ) const;
  gfl2::math::Vector3 GetPaneLayoutPosByGfVec3( const gfl2::lyt::LytWk* lytwk, gfl2::lyt::LytPaneIndex index ) const;
  gfl2::math::Vector3 GetPaneLayoutPosByGfVec3( const gfl2::lyt::LytWk* lytwk, const gfl2::lyt::LytPane * pane ) const;
*/


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのローカル座標を取得
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   *
   * @return  ペインの座標
   */
  //--------------------------------------------------------------------------------------------
  gfl2::math::VEC3 GetPaneLocalPos( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのローカル座標を取得（ポインタ指定版）
   *
   * @param   pane  ペイン
   *
   * @return  ペインの座標
   */
  //--------------------------------------------------------------------------------------------
  gfl2::math::VEC3 GetPaneLocalPos( const gfl2::lyt::LytPane * pane ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインの座標設定
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   pos     座標
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetPanePos( u32 lytID, gfl2::lyt::LytPaneIndex index, const gfl2::math::VEC3 * pos );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   タッチ座標にペインを移動
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   x       タッチX座標
   * @param   y       タッチY座標
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetPaneTouchPos( u32 lytID, gfl2::lyt::LytPaneIndex index, u16 x, u16 y );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのサイズ取得
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   *
   * @return  サイズ
   */
  //--------------------------------------------------------------------------------------------
  const nw::lyt::Size & GetSize( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのサイズ設定
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   size		サイズ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetSize( u32 lytID, gfl2::lyt::LytPaneIndex index, const nw::lyt::Size & size );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのスケール設定
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   scale   スケール値
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetScale( u32 lytID, gfl2::lyt::LytPaneIndex index, gfl2::math::VEC2 & scale );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのスケール設定(ペイン指定版)
   *
   * @param   pPane   ペイン
   * @param   scale   スケール値
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetScale( gfl2::lyt::LytPane* pPane, gfl2::math::VEC2 & scale );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   ペインのスケール取得
	 *
	 * @param   lytID   レイアウトのID
	 * @param   index   ペインのインデックス
	 * @param   scale   取得場所
	 *
	 * @return  none
	 */
	//--------------------------------------------------------------------------------------------
	void GetScale( u32 lytID, gfl2::lyt::LytPaneIndex index, gfl2::math::VEC2 * scale ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @func    GetRotate
   * @brief   ペインの回転を取得
   * @date    2015/02/26
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   rotate  取得場所
   *
   * @note  親ペインからのオフセット回転を返します
   * @note  app_utilから移植
   */
  //--------------------------------------------------------------------------------------------
  void GetRotate( u32 lytID, gfl2::lyt::LytPaneIndex index, gfl2::math::VEC3 * rotate ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @func    SetRotate
   * @brief   ペインの回転を変更
   * @date    2015/02/26
   *
   * @param   lytID   レイアウトのID
   * @param   index   ペインのインデックス
   * @param   rotate  変更する値
   *
   * @note  親ペインからのオフセット回転を変更します
   * @note  app_utilから移植
   */
  //--------------------------------------------------------------------------------------------
  void SetRotate( u32 lytID, gfl2::lyt::LytPaneIndex index, const gfl2::math::VEC3 & rotate );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アニメ開始
   *
   * @param   lytID         レイアウトのID
   * @param   anm           アニメ番号
   * @param   auto_anm_flag オートアニメーションするかどうか（デフォルト：true）
   * @param   auto_unbind   終了時に自動でアンバインドするか（デフォルト：true）
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void StartAnime( u32 lytID, u32 anm, bool auto_anm_flag = true, bool auto_unbind = true );
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   フレーム指定してアニメ開始
   *
   * @param   lytID         レイアウトのID
   * @param   anm           アニメ番号
   * @param   frame         フレーム
   * @param   auto_anm_flag オートアニメーションするかどうか（デフォルト：true）
   * @param   auto_unbind   終了時に自動でアンバインドするか（デフォルト：true）
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void StartAnimeFrame( u32 lytID, u32 anm, f32 frame, bool auto_anm_flag = true, bool auto_unbind = true );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アニメ停止
   *
   * @param   lytID   レイアウトのID
   * @param   anm     アニメ番号
   * @param   clear   アニメを初期化するか true = 初期化 (デフォルト: false)
   *
   * @return  none
   *
   * @li  clear = true の場合、Unbindされません
   */
  //--------------------------------------------------------------------------------------------
  void StopAnime( u32 lytID, u32 anm, bool clear = false );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   アニメの再生速度を指定
  *
  * @param   lytID         レイアウトのID
  * @param   speed         再生速度
  *
  * @return  none
  */
  //--------------------------------------------------------------------------------------------
  void SetAnimeSpeed( u32 lytID, f32 speed );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アニメ終了チェック
   *
   * @param   lytID   レイアウトのID
   * @param   anm     アニメ番号
   *
   * @retval  "false = 再生中"
   * @retval  "true = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool IsAnimeEnd( u32 lytID, u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @brief アニメーションが一時停止中か
   * @param   lytID   レイアウトのID
   * @param   anm     アニメ番号
   * @retval  true  = 一時停止中
   *          false = 再生中かバインドされていない
   */
  //-----------------------------------------------------------------------------
  bool IsAnimePause( u32 lytID, u32 anm );
  //-----------------------------------------------------------------------------
  /**
   * @brief アニメーションを一時停止する
   * @param   lytID   レイアウトのID
   * @param   anm     アニメ番号
   * @param notPause            trueなら再開, falseなら停止
   */
  //-----------------------------------------------------------------------------
  void PauseAnime( u32 lytID, u32 anm, bool notPause );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アニメーションフレームセット
   *
   * @param   lytID   レイアウトのID
   * @param   anm     アニメ番号
   * @param   frame   セットするフレーム
   */
  //--------------------------------------------------------------------------------------------
  void SetAnimeFrame( u32 lytID, u32 anm, f32 frame );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アニメーションフレーム取得
   *
   * @param   lytID   レイアウトのID
   * @param   anm     アニメ番号
   *
   * @return	アニメフレーム
   */
  //--------------------------------------------------------------------------------------------
  f32 GetAnimeFrame( u32 lytID, u32 anm );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   最大アニメーションフレーム取得
   *
   * @param   lytID   レイアウトのID
   * @param   anm     アニメ番号
   *
   * @return	最大アニメフレーム
   */
  //--------------------------------------------------------------------------------------------
  f32 GetAnimeMaxFrame( u32 lytID, u32 anm );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief バインドアニメする
   *
   * @param LytID レイアウト
   * @param anm   アニメ
   * @param auto_unbind オートアンバインド設定
   */
  //--------------------------------------------------------------------------------------------
  void BindAnime( u32 lytID, u32 anm, bool auto_unbind = true );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   子ペインを追加
   *
   * @param   lytID   レイアウトのID
   * @param   p       親ペインのインデックス
   * @param   c       子ペインのインデックス
   * @param   mode    追加モード
   * @param   n       追加する位置の前のペインインデックス ( mode = INSERT のときのみ )
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void AppendChild(
        u32 lytID, gfl2::lyt::LytPaneIndex p, gfl2::lyt::LytPaneIndex c,
        u32 mode=APPEND_CHILD_MODE_BOTTOM, gfl2::lyt::LytPaneIndex n=0 );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   子ペインを親子リストから削除
   *
   * @param   lytID   レイアウトのID
   * @param   p       親ペインのインデックス
   * @param   c       子ペインのインデックス
   *
   * @return  none
   *
   * @li  削除した子の親はルートペインにする。
   */
  //--------------------------------------------------------------------------------------------
  void RemoveChild( u32 lytID, gfl2::lyt::LytPaneIndex p, gfl2::lyt::LytPaneIndex c );

  //--------------------------------------------------------------------------------------------
  /**
   * @func    IsPaneTouchTrigger
   * @brief   ペインがタッチされたかどうか
   * @date    2015.02.26
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペイン
   *
   * @retval  true  = タッチされた
   * @retval  false = それ以外
   *
   * @note  app_utilから移植
   */
  //--------------------------------------------------------------------------------------------
  bool IsPaneTouchTrigger( u32 lytID, gfl2::lyt::LytPaneIndex pane ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインがタッチされたかどうか（ポインタ指定版）
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペイン
   *
   * @retval  true  = タッチされた
   * @retval  false = それ以外
   */
  //--------------------------------------------------------------------------------------------
  bool IsPaneTouchTrigger( u32 lytID, gfl2::lyt::LytPane * pane ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインをタッチ中かどうか
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペイン
   *
   * @retval  "true = タッチ中"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool IsPaneTouchHold( u32 lytID, gfl2::lyt::LytPaneIndex pane ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインをタッチ中かどうか（ポインタ指定版）
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペイン
   *
   * @retval  "true = タッチ中"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool IsPaneTouchHold( u32 lytID, gfl2::lyt::LytPane * pane ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @func    GetTouchTriggerPaneIndex
   * @brief   タッチされたペインを取得
   * @date    2015.02.26
   *
   * @param   lytID       レイアウトのID
   * @param   group       グループ
   * @param   bound_only  true = 境界ペインのみ
   *
   * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR != 取得したペイン
   * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR = 取得できなかった
   *
   * @note  app_utilから移植
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytPaneIndex GetTouchTriggerPaneIndex(
                            u32 lytID,
                            gfl2::lyt::LytPaneIndex group = gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
                            bool bound_only = false ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @func    GetTouchHoldPaneIndex
   * @brief   タッチされているペインを取得
   * @date    2015.02.26
   *
   * @param   lytID       レイアウトのID
   * @param   group       グループ
   * @param   bound_only  true = 境界ペインのみ
   *
   * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR != 取得したペイン
   * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR = 取得できなかった
   *
   * @note  app_utilから移植
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytPaneIndex GetTouchHoldPaneIndex(
                            u32 lytID,
                            gfl2::lyt::LytPaneIndex group = gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
                            bool bound_only = false ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定座標がペインにＨＩＴするかどうか
   *
   * @param   lytID     レイアウトのID
   * @param   pane      ペイン
   * @param   x         X座標
   * @param   y         Y座標
   * @param   visible   非表示ペインをチェックするか true = チェック ※デフォルト = true
   *
   * @retval  "true = ヒット中"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool GetHitPane( u32 lytID, gfl2::lyt::LytPaneIndex pane, u16 x, u16  y, bool visible=true ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定座標がペインにＨＩＴするかどうか
   *
   * @param   lytID     レイアウトのID
   * @param   pane      ペイン
   * @param   x         X座標
   * @param   y         Y座標
   * @param   visible   非表示ペインをチェックするか true = チェック ※デフォルト = true
   *
   * @retval  "true = ヒット中"
   * @retval  "false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool GetHitPane( u32 lytID, gfl2::lyt::LytPane * pane, u16 x, u16 y, bool visible=true ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのテクスチャを変更 ( 登録したインデックスを指定 )
   *
   * @param   lytID     レイアウトID
   * @param   pane      ペイン
   * @param   resID     テクスチャのリソースID
   * @param   resIndex  リソース内のインデックス
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTexMap( u32 lytID, u32 pane, u32 resID, u32 resIndex );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのテクスチャを変更 ( 登録したインデックスを指定 )(ペインポインタ指定版)
   *
   * @param   pane      ペインポインタ
   * @param   resID     テクスチャのリソースID
   * @param   resIndex  リソース内のインデックス
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTexMap( gfl2::lyt::LytPane* pane, u32 resID, u32 resIndex );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   ペインのテクスチャを変更 ( LytResID指定版 )
	 *
	 * @param   lytID     レイアウトID
	 * @param   pane      ペイン
	 * @param   resID     テクスチャのリソースID
	 * @param   resIndex  リソース内のインデックス
	 *
	 * @return  none
	 */
	//--------------------------------------------------------------------------------------------
	void SetTexMapEx( u32 lytID, u32 pane, gfl2::lyt::LytMultiResID resID, u32 resIndex );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   ペインのテクスチャを変更 ( LytResID指定版 )(ペインポインタ指定版)
	 *
	 * @param   pane      ペインポインタ
	 * @param   resID     テクスチャのリソースID
	 * @param   resIndex  リソース内のインデックス
	 *
	 * @return  none
	 */
	//--------------------------------------------------------------------------------------------
	void SetTexMapEx( gfl2::lyt::LytPane* pane, gfl2::lyt::LytMultiResID resID, u32 resIndex );

  //--------------------------------------------------------------------------------------------
  /**
   * @func    CreateStrBuf
   * @brief   汎用文字列バッファ作成
   * @date    2015/02/27
   *
   * @param   heap  ヒープ
   */
  //--------------------------------------------------------------------------------------------
  void CreateStrBuf( Heap * heap );

  //--------------------------------------------------------------------------------------------
  /**
   * @func    DeleteStrBuf
   * @brief   汎用文字列バッファ削除
   * @date    2015/02/27
   */
  //--------------------------------------------------------------------------------------------
  void DeleteStrBuf(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージ関連作成
   *
   * @param   datID         アーカイブデータID
   * @param   loadType      読み込みタイプ
   * @param   heap    ヒープ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateMessageData( u32 datID, gfl2::str::MsgData::LoadType loadType, Heap * heap , s32 wordsetBufferNum = print::WordSet::DEFAULT_WORD_MAX );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   外部メッセージデータ＆ワードセットを設定
   *
   * @param   messageData メッセージデータインスタンス
   * @param   wordset     ワードセットインスタンス
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetMessageData( gfl2::str::MsgData *messageData, print::WordSet *wordset );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		メッセージデータ取得
   *
   * @param		none
   *
   * @return	メッセージデータ
   */
  //--------------------------------------------------------------------------------------------
  gfl2::str::MsgData * GetMsgData(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		ワードセット取得
   *
   * @param		none
   *
   * @return	ワードセット
   */
  //--------------------------------------------------------------------------------------------
  print::WordSet * GetWordSet(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージ関連削除
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteMessageData(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		ワードセットの事前バッファ読み込み
   *
   * @param		heap		ヒープ
   * @param		readID	読み込むデータのID（ビット指定）
   *
   * @return	none
   */
  //--------------------------------------------------------------------------------------------
  void WordSetPreLoad( Heap * heap, u32 readID );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		ワードセットの事前バッファ読み込み終了待ち
   *
   * @param		none
   *
   * @retval	"true = 終了"
   * @retval	"false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool WordSetWaitLoad(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		ワードセットローダークラスの直接指定
   * @param		WordSetLoader
   */
  //--------------------------------------------------------------------------------------------
  void SetWordSetLoader(WordSetLoader* ploader);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   文字列取得
   *
   * @param   buf     取得場所
   * @param   id      文字列ID
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void GetString( gfl2::str::StrBuf * buf, u32 id );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   文字列を展開して取得
   *
   * @param   buf     取得場所
   * @param   id      文字列ID
   *
   * @return  none
   *
   * @li  展開領域にテンポラリバッファの[0]を使用しているため、取得場所には使えないので注意！
   */
  //--------------------------------------------------------------------------------------------
  void GetStringExpand( gfl2::str::StrBuf * buf, u32 id );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに数字を描画
   *
   * @param   lytID       レイアウトのID
   * @param   pane        ペインのインデックス
   * @param   strID				文字列ID
   * @param   value       数値
   * @param   keta        桁
   * @param		bufID				バッファID　※デフォルト = 0
   * @param   dispType    整形タイプ　※デフォルト = NUM_DISP_LEFT
   * @param   codeType    文字コード　※デフォルト = NUM_CODE_DEFAULT
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneNumber(
        u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 strID,
        u32 value, u32 keta, u32 bufID=0,
				print::NumberDispType dispType=print::NUM_DISP_LEFT,
				print::NumberCodeType codeType=print::NUM_CODE_DEFAULT );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに数字を描画（StrBuf版）
   *
   * @param   lytID       レイアウトのID
   * @param   pane        ペインのインデックス
   * @param   str					文字列
   * @param   value       数値
   * @param   keta        桁
   * @param		bufID				バッファID
   * @param   dispType    整形タイプ
   * @param   codeType    文字コード
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneNumber(
      u32 lytID, gfl2::lyt::LytPaneIndex pane, gfl2::str::StrBuf* str,
      u32 value, u32 keta, u32 bufID, print::NumberDispType dispType, print::NumberCodeType codeType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに数字を描画(ペインポインタ指定版)
   *
   * @param   pane        ペインのアドレス
   * @param   strID				文字列ID
   * @param   value       数値
   * @param   keta        桁
   * @param		bufID				バッファID　※デフォルト = 0
   * @param   dispType    整形タイプ　※デフォルト = NUM_DISP_LEFT
   * @param   codeType    文字コード　※デフォルト = NUM_CODE_DEFAULT
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneNumber(
        gfl2::lyt::LytTextBox* pane, u32 strID,
        u32 value, u32 keta, u32 bufID=0,
        print::NumberDispType dispType=print::NUM_DISP_LEFT,
        print::NumberCodeType codeType=print::NUM_CODE_DEFAULT );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに数字を描画（ペインポインタ指定 ＋ StrBuf版）
   *
   * @param   pane        ペインのアドレス
   * @param   str					文字列
   * @param   value       数値
   * @param   keta        桁
   * @param		bufID				バッファID
   * @param   dispType    整形タイプ
   * @param   codeType    文字コード
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneNumber(
      gfl2::lyt::LytTextBox* pane, gfl2::str::StrBuf* str,
      u32 value, u32 keta, u32 bufID, print::NumberDispType dispType, print::NumberCodeType codeType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( STRCODE直指定 )
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペインのインデックス
   * @param   str     文字列
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, const gfl2::str::STRCODE * str );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( StrBuf指定 )
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペインのインデックス
   * @param   buf     文字列
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, gfl2::str::StrBuf * buf );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( STRCODE直指定 )
   *
   * @param   pane    ペインのポインタ
   * @param   str     文字列
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneString( gfl2::lyt::LytTextBox * pane, const gfl2::str::STRCODE * str );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( ポインタ/StrBuf指定 )
   *
   * @param   pane    ペインのポインタ
   * @param   buf     文字列
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneString( gfl2::lyt::LytTextBox * pane, gfl2::str::StrBuf * buf );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( 文字列ID指定 )
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペインのインデックス
   * @param   strID   文字列ID
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 strID );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( ポインタ/文字列ID指定 )
   *
   * @param   pane    ペインのポインタ
   * @param   strID   文字列ID
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneString( gfl2::lyt::LytTextBox * pane, u32 strID );

#if PM_DEBUG
  //--------------------------------------------------------------------------------------------
  /**
   * @func    SetTextBoxPaneString
   * @brief   テキストボックスペインに文字列を描画 ( char型文字列指定 )
   * @date    2015/02/26
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペインのインデックス
   * @param   string  文字列
   *
   * @note  app_utilから移植
   */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneString( u32 lytID, gfl2::lyt::LytPaneIndex pane, const char * string );

  //--------------------------------------------------------------------------------------------
  /**
   * @func    SetTextBoxPaneString
   * @brief   テキストボックスペインに文字列を描画 ( char型文字列指定 / ペインポインタ指定版 )
   * @date    2015.04.02
   *
   * @param   pTextPane     ペインポインタ
   * @param   string        文字列
   */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneStringByDebug( gfl2::lyt::LytTextBox* pTextPane, const char * string );

#endif

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( 展開版 )
   *
   * @param   lytID   レイアウトのID
   * @param   pane    ペインのインデックス
   * @param   str     文字列ID
   *
   * @return  none
   *
   * @li  WORDSETを使用した場合はこちら。
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneStringExpand( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 str );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   テキストボックスペインに文字列を描画 ( StrBuf指定,展開版 ) 
	 *
	 * @param   lytID   レイアウトのID
	 * @param   pane    ペインのインデックス
	 * @param   str     文字列
	 *
	 * @return  none
	 *
	 * @li  WORDSETを使用した場合はこちら。
	 */
	//--------------------------------------------------------------------------------------------
  void SetTextBoxPaneStringExpand( u32 lytID, gfl2::lyt::LytPaneIndex pane, gfl2::str::StrBuf* str );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( StrBuf指定,展開版,ペインポインタ指定 )
   *
   * @param   pane    ペインのポインタ
   * @param   rStr    文字列
   * @note    暗黙の形変換による u32 str 版との曖昧さ回避（#defineによるマクロ定数を与えたときなど）のため、参照で受け取る
   *
   * @return  none
   *
   * @li  WORDSETを使用した場合はこちら。
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneStringExpand( gfl2::lyt::LytTextBox * pane, gfl2::str::StrBuf& rStr );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画 ( 展開版,ペインポインタ指定 )
   *
   * @param   pane    ペインのポインタ
   * @param   str     文字列ID
   *
   * @return  none
   *
   * @li  WORDSETを使用した場合はこちら。
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneStringExpand( gfl2::lyt::LytTextBox * pane, u32 str );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファに数値をセット
   *
   * @param   bufID       バッファID
   * @param   value       数値
   * @param   keta        桁
   * @param   dispType    整形タイプ　※デフォルト = NUM_DISP_LEFT
   * @param   codeType    文字コード　※デフォルト = NUM_CODE_DEFAULT
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterNumber( u32 bufID, u32 value, u32 keta, print::NumberDispType dispType=print::NUM_DISP_LEFT, print::NumberCodeType codeType=print::NUM_CODE_DEFAULT );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファに単語をセット
   *
   * @param   bufID       バッファID
   * @param   word        単語
   * @param   grm         文法性
   * @param   form        単複
   * @param   langID      言語ID
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterWord( u32 bufID, const gfl2::str::StrBuf & word, print::WordSet::Grammer grm=print::WordSet::GRAMMER_DEFAULT, print::WordSet::Form form=print::WordSet::FORM_DEFAULT );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファに単語をセット
   *
   * @param   bufID       バッファID
   * @param   strID       文字列ID
   * @param   form        単複
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterWord( u32 bufID, u32 strID, print::WordSet::Form form=print::WordSet::FORM_DEFAULT );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファにプレイヤー名をセット
   *
   * @param   bufID   バッファID
   * @param   myst    プレイヤーデータ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterPlayerName( u32 bufID, const Savedata::MyStatus * myst );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファにアイテム名をセット
   *
   * @param   bufID   バッファID
   * @param   item    アイテム番号
   * @param		count		個数　※デフォルト = 1
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterItemName( u32 bufID, u32 itemID, u32 count = 1 );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファにポケモン名をセット
   *
   * @param   bufID   バッファID
   * @param   pp      CoreParam
   *
   * @return  none
   *
   * @note ppのタマゴフラグが立っていてもポケモン種族名を代入します
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterMonsName( u32 bufID, const pml::pokepara::CoreParam * pp );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファにポケモン名をセット
   *
   * @param   bufID   バッファID
   * @param   pp      CoreParam
   *
   * @return  none
   *
   * @note ppのタマゴフラグが立っているときはタマゴを代入し、立っていないときはポケモン種族名を代入します。
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterMonsNameOrTamago( u32 bufID, const pml::pokepara::CoreParam * pp );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   指定バッファにポケモン名をセット（MonsNo指定版）
	 *
	 * @param   bufID   バッファID
	 * @param   monsno	ポケモン番号
	 *
	 * @return  none
	 */
	//--------------------------------------------------------------------------------------------
  void SetRegisterMonsNameNo( u32 bufID, MonsNo monsno );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   指定バッファにポケモン名をセット（MsgData）
	 *
   * @param[in]  bufID
   * @param[in]  msgData  ポケモン種族名が書かれたmstxtを読み込んだもの
   * @param[in]  strID    msgDataの何番目のラベルに書かれているポケモン種族名かを指定
   *                      mstxtがMonsNo順に並んでいるならstrIDとMonsNoは同じになる
	 *
	 * @return  none
	 */
	//--------------------------------------------------------------------------------------------
  void SetRegisterMonsNameMsgData( u32 bufID, gfl2::str::MsgData* msgData, u32 strID );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファにニックネームをセット
   *
   * @param   bufID   バッファID
   * @param   pp      CoreParam
   *
   * @return  none
   *
   * @note ppのタマゴフラグが立っていてもニックネームを代入しますが、
   *       タマゴフラグ立っているときはニックネームにタマゴと入っているので
   *       そのまま代入すればOKです。
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterPokeNickName( u32 bufID, const pml::pokepara::CoreParam * pp );
  void SetRegisterPokeNickName( u32 bufID, const gfl2::str::StrBuf& str, pml::Sex sex );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファに技名をセット
   *
   * @param   bufID   バッファID
   * @param   waza    技番号
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterWazaName( u32 bufID, u32 waza );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファにタイプ名をセット
   *
   * @param   bufID   バッファID
   * @param   type		タイプ名
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterPokeTypeName( u32 bufID, u32 type );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファに特性名をセット
   *
   * @param   bufID     バッファID
   * @param   tokusei   特性番号
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterTokuseiName( u32 bufID, u32 tokusei );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   指定バッファに性格名をセット
   *
   * @param   bufID     バッファID
   * @param   seikaku   性格番号
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetRegisterSeikakuName( u32 bufID, u32 seikaku );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テンポラリ文字列バッファ取得
   *
   * @param   id    バッファID
   *
   * @return  文字列バッファ
   */
  //--------------------------------------------------------------------------------------------
  gfl2::str::StrBuf * GetTempStrBuf( u32 id );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		文字色変更
   *
   * @param   lytID     レイアウトのID
   * @param   pane      ペイン
   * @param		top				上端カラー
   * @param		bottom		下端カラー
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneColor(
        u32 lytID, gfl2::lyt::LytPaneIndex pane, const gfl2::lyt::ColorU8 & top, const gfl2::lyt::ColorU8 & bottom );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   文字色変更
   *
   * @param   pTextBoxPane  テキストボックスペイン
   * @param   top           上端カラー
   * @param   bottom        下端カラー
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetTextBoxPaneColor(
        gfl2::lyt::LytTextBox* pTextBoxPane, const gfl2::lyt::ColorU8 & top, const gfl2::lyt::ColorU8 & bottom );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   文字色取得
   *
   * @param   pTextBoxPane  テキストボックスペイン
   * @param   pOutTop       上端カラー取得用
   * @param   pOutBottom    下端カラー取得用
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void GetTextBoxPaneColor( gfl2::lyt::LytTextBox* pTextBoxPane, gfl2::lyt::ColorU8* pOutTop, gfl2::lyt::ColorU8* pOutBottom );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウ作成
   *
   * @param   lytID     レイアウトID
   * @param   paneTbl   文字描画に使用するペインのテーブル（２つ必要）
   * @param   wait      メッセージ表示ウェイト
   * @param   heap      ヒープ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateMessageWindow(  u32 lytID, const gfl2::lyt::LytPaneIndex * paneTbl, int wait, Heap * heap );
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウ作成
   *
   * @param   devMan    デバイスマネージャ
   * @param   lytID     レイアウトID
   * @param   paneTbl   文字描画に使用するペインのテーブル（２つ必要）
   * @param   wait      メッセージ表示ウェイト
   * @param   heap      ヒープ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateMessageWindow(
        const gfl2::ui::DeviceManager * devMan,
        u32 lytID, const gfl2::lyt::LytPaneIndex * paneTbl, int wait, Heap * heap );

  void CreateMessageWindow(
        const gfl2::ui::DeviceManager * devMan,
        u32 lytID, gfl2::lyt::LytTextBox* pPane1, gfl2::lyt::LytTextBox *pPane2,
				int wait, Heap * heap );
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウ作成（ウェイト自動指定版）
   *
   * @param   devMan    デバイスマネージャ
   * @param   lytID     レイアウトID
   * @param   paneTbl   文字描画に使用するペインのテーブル
   * @param   paneNum   文字描画に使用するペインの数
   * @param   heap      ヒープ
   * @param   is60Fps   60FPSかどうか
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void G2DUtil::CreateMessageWindow(
    const gfl2::ui::DeviceManager * devMan,
    u32 lytID, int painnum,const gfl2::lyt::LytPaneIndex * paneTbl, Heap * heap, bool is60Fps);
    
    //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウ作成（ウェイト自動指定版）
   *
   * @param   devMan    デバイスマネージャ
   * @param   lytID     レイアウトID
   * @param   paneTbl   文字描画に使用するペインのテーブル（２つ必要）
   * @param   heap      ヒープ
   * @param   is60Fps   60FPSかどうか
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateMessageWindow(
        const gfl2::ui::DeviceManager * devMan,
        u32 lytID, const gfl2::lyt::LytPaneIndex * paneTbl, Heap * heap, bool is60Fps = false );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウ作成（ウェイト自動指定版）
   *
   * @param   devMan    デバイスマネージャ
   * @param   lytID     レイアウトID
   * @param   pPane1    テキストボックスペイン１
   * @param   pPane2    テキストボックスペイン２
   * @param   heap      ヒープ
   * @param   is60Fps   60FPSかどうか
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void CreateMessageWindow(
        const gfl2::ui::DeviceManager * devMan,
        u32 lytID,
        gfl2::lyt::LytTextBox * pPane1,
        gfl2::lyt::LytTextBox * pPane2,
        Heap * heap,
        bool is60Fps = false );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウ削除
   *
   * @param   none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void DeleteMessageWindow(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウ取得
   *
   * @param   none
   *
   * @return  メッセージウィンドウ取得
   */
  //--------------------------------------------------------------------------------------------
  print::MessageWindow * GetMsgWin(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージウィンドウにコールバック関数を設定
   *
   * @param   cb	コールバック関数
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetMessageCallback( print::MsgCallback * cb );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージカーソル作成
   *
   * @param   heap    ヒープ
   * @param   lytID   レイアウトID
   * @param   pane    カーソルペイン
   */
  //--------------------------------------------------------------------------------------------
  void CreateMsgCursor( Heap * heap,u32 lytID, gfl2::lyt::LytPaneIndex pane );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージカーソル作成（部品指定版）
   *
   * @param   heap    ヒープ
   * @param   lytID   レイアウトID
   * @param   parts   親部品ペイン
   * @param   pane    カーソルペイン
   * @param   resIndex  リソースの登録インデックス(GetLayoutResourceIDに渡すインデックス)
   */
  //--------------------------------------------------------------------------------------------
  void CreateMsgCursor( Heap * heap, u32 lytID, gfl2::lyt::LytPaneIndex parts, gfl2::lyt::LytPaneIndex pane, u32 resIndex=0 );
 
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージカーソル作成（ペインポインタ版）
   *
   * @param   heap    ヒープ
   * @param   pane    カーソルのペインポインタ。カーソルのペインは部品ペインになっているはずです。
   */
  //--------------------------------------------------------------------------------------------
  void CreateMsgCursor( Heap* heap, gfl2::lyt::LytPane* pPane );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージカーソル削除
   *
   * @param   none
   *
   * @retval	"true = 削除完了"
   * @retval	"false = それ以外"
   */
  //--------------------------------------------------------------------------------------------
  bool DeleteMsgCursor(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージカーソル表示非表示
   *
   * @param   bUser 自分でやる場合 true
   * @param   flg  onoff
   */
  //--------------------------------------------------------------------------------------------
  void SetMsgCursorUserVisible(bool bUser, bool flg);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージセット
   *
   * @param   str   文字列
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetMessage( const gfl2::str::StrBuf * str );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージ描画
   *
   * @param   none
   *
   * @return  処理結果
   */
  //--------------------------------------------------------------------------------------------
  gfl2::str::StrWin::Result PrintMessage(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージ終了モードを設定
   *
   * @param   type    終了タイプ
   * @param   frame   時間用フレーム
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void SetMessageFinishMode( gfl2::str::StrWin::FinishType type, u8 frame );

  //--------------------------------------------------------------------------------------------
  /**
   * @func    SetTextboxPaneTagProcessor
   * @brief   タグプロセッサーの登録
   * @date    2015/02/26
   *
   * @param   lytID     レイアウトID
   * @param   pane      対象ペインのインデックス
   * @param   tag_proc  タグプロセッサへのポインタ
   *
   * @note  app_utilから移植
   */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneTagProcessor(
        u32 lytID, gfl2::lyt::LytPaneIndex pane, nw::lyt::TextBox::TagProcessor * tag_proc );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テクスチャーリソースデータのアドレスを取得する
   *
   * @param   lytID     レイアウトID
   * @param   textureID テクスチャID
   *
   * @return  指定したテクスチャーのリソースアドレス
   */
  //--------------------------------------------------------------------------------------------
#if 0   // TexResourceが解決できないためコメントアウト。必要になったら改めて考える。 2015.07.06 by nakahiro
  void* GetTextureResourceAddress( u32 lytID, u32 textureID );
#endif

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   WordSetクラスインスタンスを取得
   *
   * @param   none
   *
   * @return  WordSet
   */
  //--------------------------------------------------------------------------------------------
//  inline print::WordSet * GetWordSet( void ) const { return pWordSet; }   // @note inline関数じゃないとダメなら有効にしてください

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   MsgDataクラスインスタンスを取得
   *
   * @param   none
   *
   * @return  gfl2::str::MsgData
   */
  //--------------------------------------------------------------------------------------------
//  inline gfl2::str::MsgData * GetMessageData( void ) const { return pMsgData; }   // @note inline関数じゃないとダメなら有効にしてください

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 上画面用ENV取得
   *
   * @return　ENV
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::Env* GetUpperEnv( void ) const { return pUpperLayoutEnv; }
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 下画面用ENV取得
   *
   * @return ENV
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::Env* GetLowerEnv( void ) const { return pLowerLayoutEnv; }

  //--------------------------------------------------------------------------------------------
  /**
   * @func    CreateAppLytAccessor
   * @brief   applytファイルのアクセサを作成
   * @date    2015.02.25
   *
   * @param   heap  ヒープ
   */
  //--------------------------------------------------------------------------------------------
  void CreateAppLytAccessor( Heap * heap );
  //--------------------------------------------------------------------------------------------
  /**
   * @func    DeleteAppLytAccessor
   * @brief   applytファイルのアクセサを削除
   * @date    2015.02.25
   */
  //--------------------------------------------------------------------------------------------
  void DeleteAppLytAccessor(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @func    GetAppLytAccessor
   * @brief   applytファイルのアクセサを取得
   * @date    2015.02.25
   *
   * @return  applytファイルアクセサ
   */
  //--------------------------------------------------------------------------------------------
  app::util::AppLytAccessor * GetAppLytAccessor(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @func    SetLayoutWork
   * @brief   レイアウトをapplytファイルから自動生成
   * @date    2015.02.25
   *
   * @param   id                    生成するレイアウトの開始ID
   * @param   dat                   生成するレイアウトデータ
   * @param   num                   生成するレイアウト数
   * @param   allocator             アロケータ ( NULL = レイアウトシステムから確保 )
   * @param   dev_allocator         デバイスアロケータ ( NULL = レイアウトシステムから確保 )
   * @param   createPaneTbl         ペインテーブルを生成し検索時間を短縮するか
   * @param   createPaneTblWithHash ペインテーブル生成時にハッシュテーブルを作り生成時間を短縮するか
   */
  //--------------------------------------------------------------------------------------------
  void SetLayoutWork(
        u32 id,
        const LytwkData * dat,
        u32 num,
        gfl2::heap::NwAllocator * allocator = NULL,
        gfl2::heap::NwAllocator * dev_allocator = NULL,
        b32 createPaneTbl = DEFAULT_CREATE_PANETBL_USE,
        b32 createPaneTblWithHash=DEFAULT_CREATE_PANETBL_HASH_USE );

  
  //----------------------------------------------------------------------------
  /**
   *  @brief  描画中か
   *  @return  G2DUtil内で生成されたLytWkのうち１つでも描画中のものがあればtrue(LytWk１つずつ確認したい場合はGetLayoutWorkで取得して個別にLytWk::IsDrawingで確認して下さい)
   */
  //----------------------------------------------------------------------------
  bool IsDrawing(void) const;


  //--------------------------------------------------------------------------------------------
  /**
   * @func       CTRCalcLytTextureInfoTexObject
   * @brief      CTR版レイアウトのTextureInfoに渡すtexObjectを計算する
   * @date       2015.08.03
   *
   * @param[in]  virtualAddr  テクスチャイメージの仮想アドレス
   * @return     texObjectの値
   * @attention  Win32版ではASSERTになります。ASSERTを抜けるとnw::lyt::TextureInfo::INVALID(無効値)を返します。
   */
  //--------------------------------------------------------------------------------------------
  static u32 CTRCalcLytTextureInfoTexObject(uptr virtualAddr);

  //--------------------------------------------------------------------------------------------
  /**
   * @func       MakeTextureInfo
   * @brief      「gfxのテクスチャ」の内容を「レイアウトのテクスチャ情報」に反映する
   * @date       2015.08.19
   *
   * @param[out]  textureInfo  「レイアウトのテクスチャ情報」を受け取るオブジェクト。NULLを渡さないようにして下さい。
   * @parame[in]  texture      「gfxのテクスチャ」
   */
  //--------------------------------------------------------------------------------------------
  static void MakeTextureInfo(gfl2::lyt::LytResTextureInfo* textureInfo, const gfl2::gfx::Texture* texture);

  //--------------------------------------------------------------------------------------------
  /**
   * @func    CreatePartsAnime
   * @brief   部品アニメを生成
   * @date    2015.02.25
   *
   * @param   lytwk             レイアウトワーク
   * @param   res_id            リソースID
   * @param   lytres_id         レイアウトデータID ( *_BFLYT )
   * @param   start_anm_no      追加開始アニメ番号
   * @param   parent_parts      部品ペイン ( NULL = 親レイアウトからの呼び出し時 = NULL )
   * @param   recursive_count   再帰呼び出し回数 ( 最大回数 = CREATE_ANIME_RECURSIVE_LIMIT )
   *
   * @note  再帰が最大回数を超えるとアニメが生成されません
   */
  //--------------------------------------------------------------------------------------------
  void CreatePartsAnime(
        gfl2::lyt::LytWk * lytwk,
        const gfl2::lyt::LytMultiResID * res_id,
        u32 lytres_id,
        u32 * start_anm_no,
        gfl2::lyt::LytParts * parent_parts,
        u32 recursive_count );

    //--------------------------------------------------------------------------------------------
  /**
   * @func    IsTouchPane
   * @brief   タッチ座標にペインがあるか
   * @date    2015.02.26
   *
   * @param   tp      タッチパネル
   * @param   lytID   レイアウトのID
   * @param   pane    ペイン
   *
   * @retval  true  = ある
   * @retval  false = ない
   */
  //--------------------------------------------------------------------------------------------
  bool IsTouchPane( gfl2::ui::TouchPanel * tp, u32 lytID, gfl2::lyt::LytPaneIndex pane ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @func    IsTouchPane
   * @brief   タッチ座標にペインがあるか（ポインタ指定版）
   * @date    2015.03.31
   *
   * @param   tp      タッチパネル
   * @param   lytID   レイアウトのID
   * @param   pane    ペイン
   *
   * @retval  true  = ある
   * @retval  false = ない
   */
  //--------------------------------------------------------------------------------------------
  bool IsTouchPane( gfl2::ui::TouchPanel * tp, u32 lytID, gfl2::lyt::LytPane * pane ) const;


private:
  //--------------------------------------------------------------------------------------------
  /**
   * @func    GetTouchPanel
   * @brief   タッチ中パネルを取得
   * @date    2015.02.26
   *
   * @return  タッチパネル
   */
  //--------------------------------------------------------------------------------------------
  gfl2::ui::TouchPanel * GetTouchPanel(void) const;



  //--------------------------------------------------------------------------------------------
  /**
   * @func    GetTouchPaneIndex
   * @brief   タッチ座標のペインを取得
   * @date    2015.02.26
   *
   * @param   tp          タッチパネル
   * @param   lytID       レイアウトのID
   * @param   group       グループ
   * @param   bound_only  true = 境界ペインのみ
   *
   * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR != 取得したペイン
   * @retval  gfl2::lyt::LYTSYS_PANEINDEX_ERROR = 取得できなかった
   */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytPaneIndex GetTouchPaneIndex( gfl2::ui::TouchPanel * tp, u32 lytID, gfl2::lyt::LytPaneIndex group, bool bound_only ) const;


private:
  static const gfl2::lyt::LytWkSetUp LytWkDefaultSetup[2];	//!< レイアウトセットアップデータ

  gfl2::lyt::LytSys * pLayoutSystem;        //!< レイアウトシステム
  gfl2::lyt::LytMultiResID* pLayoutResID;		//!< リソースID
  gfl2::lyt::Env * pUpperLayoutEnv;			//!< 上画面のレイアウト環境
  gfl2::lyt::Env * pLowerLayoutEnv;			//!< 下画面のレイアウト環境
  LAYOUT_WORK * pLayoutWork;	//!< レイアウトワーク管理データ
  u32 lwkMax;									//!< レイアウトワーク管理データ数
  u32 resIdMax;								//!< リソースID最大数

  gfl2::str::MsgData * pMsgData;                  //!< メッセージデータ
  WordSetLoader * pWordSetLoader;                 //!< 単語セット事前読み込み管理
  print::WordSet * pWordSet;                      //!< 単語セット
  gfl2::str::StrBuf * m_strbuf[TEMP_STRBUF_NUM];  //!< 汎用文字列バッファ

  print::MessageWindow * pMsgWin;     //!< メッセージウィンドウ
  App::Tool::MsgCursor * pMsgCursor;  //!< メッセージカーソル

  app::util::AppLytAccessor * m_pLytAccessor;   //!< applytファイルのアクセサー

  bool m_bMessageCreate;

  bool m_bIsSetLayoutResourceID;    //! レイアウトリソースIDが外部から渡されたフラグ

};


} // util
} // app

#endif // __APP_UTIL_2D_H__
