//=============================================================================
/**
 * @file    AppToolItemEffect.h
 * @brief   道具関連のエフェクト処理
 * @author  Hiroyuki Nakamura
 * @date    2016.01.18
 */
//=============================================================================
#if !defined( APP_TOOL_ITEM_EFFECT_H_INCUDED )
#define APP_TOOL_ITEM_EFFECT_H_INCUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pml
#include <pml/include/pmlib.h>
// niji
#include "AppLib/include/Util/AppRenderingManager.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class EffectUtil;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//=============================================================================
/**
 * @class ItemEffect
 * @brief 道具関連のエフェクト
 * @date  2016.01.18
 */
//=============================================================================
class ItemEffect
{
  GFL_FORBID_COPY_AND_ASSIGN( ItemEffect );

public:
  //! エフェクトタイプ
  enum Type
  {
    TYPE_Z_BEADS_HONOO = 0,       //!< Zビーズ: ほのお
    TYPE_Z_BEADS_MIZU,            //!< Zビーズ: みず
    TYPE_Z_BEADS_KUSA,            //!< Zビーズ: くさ
    TYPE_Z_BEADS_DENKI,           //!< Zビーズ: でんき
    TYPE_Z_BEADS_KOORI,           //!< Zビーズ: こおり
    TYPE_Z_BEADS_KAKUTOU,         //!< Zビーズ: かくとう
    TYPE_Z_BEADS_DOKU,            //!< Zビーズ: どく
    TYPE_Z_BEADS_JIMEN,           //!< Zビーズ: じめん
    TYPE_Z_BEADS_HIKOU,           //!< Zビーズ: ひこう
    TYPE_Z_BEADS_ESPER,           //!< Zビーズ: エスパー
    TYPE_Z_BEADS_MUSHI,           //!< Zビーズ: むし
    TYPE_Z_BEADS_IWA,             //!< Zビーズ: いわ
    TYPE_Z_BEADS_GHOST,           //!< Zビーズ: ゴースト
    TYPE_Z_BEADS_DRAGON,          //!< Zビーズ: ドラゴン
    TYPE_Z_BEADS_AKU,             //!< Zビーズ: あく
    TYPE_Z_BEADS_HAGANE,          //!< Zビーズ: はがね
    TYPE_Z_BEADS_FAIRY,           //!< Zビーズ: フェアリー
    TYPE_Z_BEADS_NORMAL,          //!< Zビーズ: ノーマル

    TYPE_Z_BEADS_ZYUNAIPAA,       //!< Zビーズ: ジュナイパー
    TYPE_Z_BEADS_GAOGAEN,         //!< Zビーズ: ガオガエン
    TYPE_Z_BEADS_ASIREENU,        //!< Zビーズ: アシレーヌ
    TYPE_Z_BEADS_PiKATYUU,        //!< Zビーズ: ピカチュウ
    TYPE_Z_BEADS_KAPU,            //!< Zビーズ: カプ
    TYPE_Z_BEADS_A_RAITYUU,       //!< Zビーズ: A.ライチュウ
    TYPE_Z_BEADS_KABIGON,         //!< Zビーズ: カビゴン
    TYPE_Z_BEADS_IIBUI,           //!< Zビーズ: イーブイ
    TYPE_Z_BEADS_MYUU,            //!< Zビーズ: ミュウ
    TYPE_Z_BEADS_MAASYADOO,       //!< Zビーズ: マーシャドー
    TYPE_Z_BEADS_SATOPIKA,        //!< Zビーズ: サトピカ
    TYPE_Z_BEADS_NEKUROZUMA,      //!< Zビーズ: ネクロズマ
    TYPE_Z_BEADS_SORUGAREO,       //!< Zビーズ: ソルガレオ
    TYPE_Z_BEADS_RUNAAARA,        //!< Zビーズ: ルナアーラ
    TYPE_Z_BEADS_MIMIKKYU,        //!< Zビーズ: ミミッキュ
    TYPE_Z_BEADS_RUGARUGAN,       //!< Zビーズ: ルガルガン
    TYPE_Z_BEADS_ZYARARANGA,      //!< Zビーズ: ジャラランガ

    TYPE_GRATHINA,                //!< ギラティナ（フォルムチェンジ）
    TYPE_SHEIMI,                  //!< シェイミ（フォルムチェンジ）
    TYPE_TORUNEROSU,              //!< トルネロス（フォルムチェンジ）
    TYPE_BORUTOROSU,              //!< ボルトロス（フォルムチェンジ）
    TYPE_RANDOROSU,               //!< ランドロス（フォルムチェンジ）
    TYPE_KYUREMU_Z_COMBINE,       //!< ゼクロム合体
    TYPE_KYUREMU_R_COMBINE,       //!< レシラム合体
    TYPE_KYUREMU_C_COMBINE,       //!< キュレム合体
    TYPE_KYUREMU_Z_SEPARATE,      //!< ブラックキュレム分離
    TYPE_KYUREMU_R_SEPARATE,      //!< ホワイトキュレム分離
    TYPE_HUUPA,                   //!< フーパ（フォルムチェンジ）
    TYPE_ZIGARUDE_UP,             //!< ジガルデ（10%=>50%へのフォルムチェンジ）
    TYPE_ZIGARUDE_DOWN,           //!< ジガルデ（50%=>10%へのフォルムチェンジ）
    TYPE_MITUSUI,                 //!< ミツスイ（フォルムチェンジ）
    TYPE_NEKUROZUMA_S_COMBINE,    //!< ソルガレオ合体
    TYPE_NEKUROZUMA_L_COMBINE,    //!< ルナアーラ合体
    TYPE_NEKUROZUMA_N_COMBINE,    //!< ネクロズマ合体
    TYPE_NEKUROZUMA_S_SEPARATE,   //!< ネクロズマソル分離
    TYPE_NEKUROZUMA_L_SEPARATE,   //!< ネクロズマルナ分離
    TYPE_MAX
  };


private:
  static const s32 EFFECT_MAX = 2;   //! エフェクト生成数

  //! エフェクトデータ
  struct EFFECT_DATA
  {
    u32 dat_id;   //!< ARCDATID
    u32 se;       //!< SE
    u32 timing;   //!< 特定の処理を実行するタイミング（アイコンの書き換えなど）
  };

  static const u32 SE_NONE = 0xffffffff;  //!< SEなし

  static const EFFECT_DATA EffectDataTable[TYPE_MAX];   //!< エフェクトデータテーブル

  //! エフェクトワーク
  struct EFFECT_WORK
  {
    Type type;              //!< エフェクトタイプ
    void * buf;             //!< リソースバッファ
    u16 seq:15;             //!< シーケンス
    u16 is_update:1;        //!< 1 = 動作中
    u16 frame;              //!< フレーム
    gfl2::math::VEC3 pos;   //!< 座標
  };

  static const u32 EFFECT_FRAME_MAX = 0xffff;   //!< フレーム最大値

  static const s32 ADD_INDEX_NONE = 0xff;   //!< 未登録インデックス


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap                ヒープ
   * @param   rendering_manager   描画マネージャ
   * @param   disp                表示画面
   */
  //-----------------------------------------------------------------------------
  ItemEffect( app::util::Heap * heap, app::util::AppRenderingManager * rendering_manager, app::util::AppRenderingManager::RenderPlaceType::Tag disp );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~ItemEffect();

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2016.01.18
   */
  //-----------------------------------------------------------------------------
  void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize
   * @brief   初期化処理
   * @date    2016.01.18
   *
   * @param   is_sync   true = 同期, false = 非同期
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Initialize( bool is_sync );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate
   * @brief   終了処理
   * @date    2016.01.18
   *
   * @param   is_sync   true = 同期, false = 非同期
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate( bool is_sync );

  //-----------------------------------------------------------------------------
  /**
   * @func    Create
   * @brief   エフェクト生成
   * @date    2016.01.18
   *
   * @param   index     インデックス
   * @param   type      エフェクトタイプ
   * @param   pos       表示座標
   * @param   is_sync   true = 同期, false = 非同期
   */
  //-----------------------------------------------------------------------------
  void Create( u32 index, Type type, gfl2::math::VEC3 * pos, bool is_sync );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCreate
   * @brief   エフェクト生成待ち
   * @date    2016.01.18
   *
   * @param   index     インデックス
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCreate( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    Delete
   * @brief   エフェクト削除
   * @date    2016.01.18
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void Delete( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   エフェクト開始
   * @date    2016.01.18
   */
  //-----------------------------------------------------------------------------
  void Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   エフェクト終了
   * @date    2016.01.18
   */
  //-----------------------------------------------------------------------------
  void End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEnd
   * @brief   エフェクト終了チェック
   * @date    2016.01.18
   *
   * @param   index   インデックス
   *
   * @retval  true  = 終了
   * @retva;  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsEnd( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsUpdateTiming
   * @brief   エフェクトごとの更新タイミングかをチェック
   * @date    2016.01.18
   *
   * @param   index   インデックス
   *
   * @retval  true  = 更新タイミング
   * @retva;  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsUpdateTiming( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetFormChangeEffectType
   * @brief   フォルムチェンジエフェクトタイプを取得
   * @date    2016.02.01
   *
   * @param   mons  ポケモン番号
   * @param   form  フォルム番号
   * @param   item  アイテム番号
   *
   * @return  エフェクトタイプ
   */
  //-----------------------------------------------------------------------------
  Type GetFormChangeEffectType( MonsNo mons, FormNo form, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetZPowerItemEffectType
   * @brief   Zビース用エフェクトタイプを取得
   * @date    2017.02.27
   *
   * @param   item  アイテム番号
   *
   * @return  エフェクトタイプ
   */
  //-----------------------------------------------------------------------------
  Type GetZPowerItemEffectType( u16 item );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    InitWork
   * @brief   ワーク初期化（個別）
   * @date    2016.01.18
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void InitWork( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitWork
   * @brief   ワーク初期化（全体）
   * @date    2016.01.18
   */
  //-----------------------------------------------------------------------------
  void InitWork(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateEffectUtil
   * @brief   EffectUtil作成
   * @date    2016.01.18
   */
  //-----------------------------------------------------------------------------
  void CreateEffectUtil(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteEffectUtil
   * @brief   EffectUtil削除
   * @date    2016.01.18
   */
  //-----------------------------------------------------------------------------
  void DeleteEffectUtil(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Push
   * @brief   リストに追加
   * @date    2016.01.19
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void Push( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    Pop
   * @brief   リストから削除
   * @date    2016.01.19
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void Pop( u32 index );


private:
  app::util::Heap * m_heap;                                     //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;          //!< 描画マネージャ
  app::util::AppRenderingManager::RenderPlaceType::Tag m_disp;  //!< 表示画面

  app::util::EffectUtil * m_pEffectUtil;    //!< EffectUtil

  EFFECT_WORK m_work[EFFECT_MAX];   //!< ワーク
  u8 m_addIndex[EFFECT_MAX];        //!< 登録順

  u8 m_initializeSeq;    //!< 初期化シーケンス
  u8 m_terminateSeq;     //!< 終了シーケンス
};


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // APP_TOOL_ITEM_EFFECT_H_INCUDED
