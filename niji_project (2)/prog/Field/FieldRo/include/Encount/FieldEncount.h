//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file FieldEncount.h
 *  @brief エンカウントチェック
 *  @author tomoya takahashi
 *  @date 2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#if !defined( FIELD_ENCOUNT_H_INCLUDED )
#define FIELD_ENCOUNT_H_INCLUDED
#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// conv_header
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <niji_conv_header/field/attribute/attribute_id.h>
#include <niji_conv_header/battle/battle_effect_def.h>

// field
#include "Field/FieldRo/include/Encount/FieldEncountData.h"
#include "Field/FieldRo/include/Encount/FieldEncountPokeParam.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldRo/include/Encount/FieldEncountData.h"

// sound
#include "Sound/include/Sound.h"


// 前方参照
namespace GameSys {
  class GameManager;
}

namespace Field {

  class Fieldmap;
  class EventBattleCall;

  namespace MoveModel {
    class FieldMoveModelPlayer;
  }
  namespace Encount  {
    class PokeSet;
  }
}

namespace gfl2 {
  namespace fs {
    class BinLinkerAccessor;
  }
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
 */
//-----------------------------------------------------------------------------

// エンカウントチェック
class Encount
{
  GFL_FORBID_COPY_AND_ASSIGN(Encount); // コピーコンストラクタ＋代入禁止

public:

  enum Type
  {
    TYPE_NORMAL,  // 通常エンカウント
    TYPE_FORCE,   // 強制エンカウント

    TYPE_MAX,   // 強制エンカウント
  };

  static const u32 ENCOUNT_DATA_ID_ERROR = 0xFFFFFFFF;  // エンカウントデータID無効値

public:
  // コンストラクタ
  Encount( Field::Fieldmap* p_fieldmap, gfl2::fs::BinLinkerAccessor* pEncountDataPackAccessor);

  // デストラクタ
  ~Encount();

  //----------------------------------------------------------------------------
  /**
   *  @brief  シンボルエンカウントチェック<static>
   *
   *  @param  p_gman
   *  @param  symbol_id
   *  @param  call_option enum EvBattleOptionFlag
   *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合にのみ指定
   *
   *  @retval true  バトルイベント起動
   *  @retval false バトルイベント起動なし
   */
  //-----------------------------------------------------------------------------
  static void CallSymbolEncount( GameSys::GameManager* p_gman, u32 symbol_id, u32 call_option = 0, u32 ret_bgm_id = Sound::SOUND_ITEM_ID_NONE );

  //----------------------------------------------------------------------------
  /**
   *  @brief  初期化
   *  @param  pHeap 読み込みに使用するヒープ 
   */
  //-----------------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* pHeap);

  //----------------------------------------------------------------------------
  /**
   *  @brief  初期化待ち
   *  @retval true 完了
   *  @retval false 実行中
   */
  //-----------------------------------------------------------------------------
  bool WaitInitialize();

  //----------------------------------------------------------------------------
  /**
   *  @brief  破棄
   */
  //-----------------------------------------------------------------------------
  void Terminate();

  //----------------------------------------------------------------------------
  /**
   *  @brief  強制エンカウントバトルコール
   *  @param  p_gman GameManager
   *  @param  encountDataID エリアごとに固めたエンカウントテーブルパック内のインデックス
   *  @param  call_option enum EvBattleOptionFlag
   *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合にのみ指定
   */
  //-----------------------------------------------------------------------------
  bool CallForceEncount( GameSys::GameManager* p_gman, u32 encountDataID, bool isFishing, u32 call_option = 0, u32 ret_bgm_id = Sound::SOUND_ITEM_ID_NONE );

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウントポケモンの抽選までを行う
   *  @param  p_gman GameManager
   *  @param  encountDataID エリアごとに固めたエンカウントテーブルパック内のインデックス
   *  @param  call_option enum EvBattleOptionFlag
   *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合にのみ指定
   *  @return 抽選したポケモンのエンカウントパラメータ
   *  @note   抽選したポケモンNoはPokeSet::GetTopMonsNoからも取得できます
   */
  //-----------------------------------------------------------------------------
  const EncountPokeParam* LotteryEncountPoke( GameSys::GameManager* p_gman, u32 encountDataID, bool isFishing, BattleEffectId btl_eff = BATTLE_EFFECT_MAX);

  //----------------------------------------------------------------------------
  /**
   *  @brief  指定したエンカウントデータの最初のポケモンと、指定レベルでエンカウントするようにセット
   *
   *  @note   現時点で「きのみエンカウント」のみに利用する予定
   */
  //----------------------------------------------------------------------------
  void SetEncountPokeFromFirstRecord( GameSys::GameManager* p_gman, u32 encountDataID, u32 level, bool isFishing);

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウントチェック
   *
   *  @param  type  エンカウントタイプ
   *  @param  encountDataID エリアごとに固めたエンカウントテーブルパック内のインデックス
   *  @param  attr  アトリビュート ATTR_ERROR なら自機から取得する
   *
   *  @retval true  バトルイベント起動
   *  @retval false バトルイベント起動なし
   *
   *  @niji 
   *
   *  １、足元アトリビュートからIsEncountを取得。
   *  ２、IsEncountがTrueだったら、エンカウントイベントとのHitチェック
   *  ３、エンカウントイベントがHitしたら、参照するエンカウントデータ(FM由来)が決まる
   *  ４、PokeSetクラスをこのタイミングで生成。バトルが終わるまで取っておきたいので、EncountWorkクラスに持たせよう
   *  ５、天候などや特性補正も加味して、エンカウントするか？の確率抽選
   *  ６、エンカウントする事になったら、データ使ってエンカウントするポケモン個体の抽選処理(EncountPokeSet)
   *  ７、個体が決まったタイミングで、最後のエンカウントするかチェック。これは虫除けスプレーや特性の低レベルエンカウント回避のため
   *  ８、PokeSetにパラメータを詰め込んで、EventBattleCallへGO
   *      PokePartyの実体はBattleSetupParamに持つことになるのでEventBattleCallのフロー中で生成する
   */
  //-----------------------------------------------------------------------------
  bool CheckEncount( Type type, u32 encountDataID, Attr attr = ATTR_ERROR );

  //----------------------------------------------------------------------------
  /**
   *  @brief  通常野生ランダムエンカウント率制御
   *
   *  @param  move_f     一歩移動フラグ（振り向きを含まない）
   *  @param  ex_move_f  波乗り開始時などの特殊イベント一歩フラグ
   */
  //-----------------------------------------------------------------------------
  void CalcEncountProb( Attr attr, bool move_f, bool ex_move_f, u32 encountDataID, u8 attrNearBGPattern );

private:

  //----------------------------------------------------------------------------
  /**
   *  @brief  通常野生ランダムエンカウント率制御データ取得
   */
  //-----------------------------------------------------------------------------
  const ENCPROB_PATTERN* GetEncProbPattern( const ENCOUNT_DATA * cp_encdata ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウント有効チェック
   */
  //-----------------------------------------------------------------------------
  bool IsEncountEnable( GameSys::GameManager* p_gman ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウントデータポインタ取得
   *  @param  packIndex パック内の番号。各エンカウント範囲配置情報からエンカウントテーブルIDを取得して渡す
   */
  //-----------------------------------------------------------------------------
  const ENCOUNT_DATA* GetEncountData( u32 packIndex) const ;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ギミック系強制エンカウントで共通初期化処理
   */
  //-----------------------------------------------------------------------------
  bool InitializeForForceEncount(GameSys::GameManager* p_gman, const ENCOUNT_DATA* cp_encdata, PokeSet& poke_set, bool isFishing);

private:

  Fieldmap*                         m_pFieldmap;
  MoveModel::FieldMoveModelPlayer*  m_pPlayer;
  gfl2::fs::BinLinkerAccessor*      m_pEncountDataPackAccessor;

  gfl2::heap::HeapBase*             m_pHeap;
  SymbolEncountParam*               m_pSymbolEncountParam;
  u32                               m_symbolEncountDataSize;
  u32                               m_symbolEncountParamNum;
  u32                               m_seq;
};

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

#endif  // FIELD_ENCOUNT_H_INCLUDED
