//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file EncountPokeSet.h
 *  @brief エンカウントするポケモンセットを取得
 *  @author tomoya takahashi
 *  @date 2011.12.21
 *
 *  @niji 基本的には移植してくる
 *  背景定義など、現時点で定義が無いものは一回消すか、fakeでダミーのタイプ定義をして残す。
 *  ダミーのパラメータはゼロ初期化でOK。現時点で定義がないものは6月は使わない
 *
 *  また、各種特性適用は打ち合わせたとおり、6月は未実装で構わない。
 *  特性「じりょく」等の中身ははコメントアウトで処理をスルー
 *  確率変動系は移植で大丈夫。後から調整が入るかもしれないが、今は移植でいい
 *
 *  @niji このクラスはsangoまではBattleSetupParamを作ったら捨てていたが
 *  乱入対応のために、バトルが終了するまで維持するつくりに変更する
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#if !defined( FIELD_ENCOUNT_POKE_SET_H_INCLUDED )
#define FIELD_ENCOUNT_POKE_SET_H_INCLUDED
#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// conv_header
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <niji_conv_header/battle/battle_effect_def.h>
#include <niji_conv_header/field/zone/zone_id.h>
#include <niji_conv_header/field/attribute/attribute_id.h>
#include <niji_conv_header/battle/background/bgsys_near_type_def.h>
#include <niji_conv_header/battle/background/bgsys_far_type_def.h>

// field
#include "Field/FieldRo/include/Encount/FieldEncount.h"
#include "Field/FieldRo/include/Encount/FieldEncountData.h"
#include "Field/FieldRo/include/Encount/FieldEncountPokeParam.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"

// system
#include "Battle/include/battle_DefaultPowerUp.h"

// conv_header
#include "niji_conv_header/field/WeatherKindDef.h"


namespace Field {
  class Fieldmap;
  namespace MoveModel {
    class FieldMoveModelPlayer;
  }
}


namespace gamesystem{
  class GameManager;
}

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

// エンカウントヒットしたポケモンを設定
class PokeSet
{
public:

  // 特性、アイテム効果の適用チェックをするためのポケモン情報
  struct EffectApplyCheckInfo
  {
    u16 mons_no;    ///< 手持ち先頭モンスターNo
    u16 mons_item;  ///< アイテムチェック
    u8  mons_spa;   ///< 手持ち先頭特性チェック
    u8  mons_sex;   ///< 性別チェック
    u8  mons_chr;   ///< 性格チェック
    u8  spray_lv;   ///< むしよけスプレー及び低レベルエンカウント回避チェックに用いるレベル
  };

  // 各種フラグ
  struct EffectApplyCheckFlag
  {
    union{
      struct{
        u32 gameclear_f:1;            ///< ゲームクリアしているかどうか？
        u32 fishing_f:1;              ///< 釣りエンカウントフラグ
        u32 spray_f:1;                ///< スプレーチェックするかのフラグ  TRUE:チェックする
        u32 enc_force_f:1;            ///< 戦闘回避無効  TRUE:無効  FALSE:有効    (現状では、あまいかおり・あまいミツ用)
        u32 rare_up_f:1;              ///< レア率アップ(ひかるおまもり)
        u32 flash_f:1;                ///< フラッシュフラグ(エンカウント率1/4)
        u32 kuroi_biidoro_f:1;        ///< くろいビードロ レベル1~4アップ
        u32 siroi_biidoro_f:1;        ///< くろいビードロ レベル1~4ダウン
        u32 legend_f:1;               ///< 伝説フラグ 3V

        u32 common_dummy:23;
      };
      u32 common_flags;
    };

    union{
      struct{
        //発動率100
        u32 spa_rate_up:1;            ///< エンカウント率2倍(ありじごく他)
        u32 spa_rate_down:1;          ///< エンカウント率1/2(あくしゅう他)
        u32 spa_item_rate_up:1;       ///< アイテムが手に入りやすい(ふくがん他)
        u32 spa_chr_fix:1;            ///< 手持ちと同じ性格(シンクロ他)

        //発動率2/3
        u32 spa_sex_fix:1;            ///< 手持ちと異なる性別(メロメロボディ他)

        //発動率1/2
        u32 spa_hagane_up:1;          ///< ハガネタイプとのエンカウント率アップ
        u32 spa_denki_up:1;           ///< 電気タイプとのエンカウント率アップ
        u32 spa_high_lv_hit:1;        ///< レベルの高いポケモンとエンカウントしやすい(やるき他)
        u32 spa_low_lv_rm:1;          ///< レベルの低いポケモンとエンカウントしない(威嚇他)

        u32 spa_dummy:23;
      };
      u32 spa_flags;
    };
  };

public:

  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeSetコンストラクタ 
   */
  //-----------------------------------------------------------------------------
  PokeSet( void );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeSet 初期化
   *
   *  @param  p_gman    ゲームマネージャー
   *  @param  enc_type  エンカウントタイプ
   *  @param  weather   天気
   */
  //-----------------------------------------------------------------------------
  void Initialize(GameSys::GameManager* p_gman, Encount::Type enc_type, Attr attr, weather::WeatherKind weather, bool isFishing );

  //----------------------------------------------------------------------------
  /**
   *  @brief  クリア処理
   */
  //-----------------------------------------------------------------------------
  void Clear();

  //----------------------------------------------------------------------------
  /**
   *  @brief  特性/道具使用によるエンカウント率変動計算
   *
   *  @param  in_prob 計算元の確率
   *
   *  @retval out_prob  計算後の確率
   */
  //-----------------------------------------------------------------------------
  u32 EncountProbCalc( const u32 in_prob, MoveModel::FieldMoveModelPlayer * cp_player );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief エンカウント制御パターンデータをセット
   *
   * @param  p_data  ENCPROB_PATTERN構造体型データへのポインタ
   */
  /* -------------------------------------------------------------------------*/
  void SetEncProbPatternData( const ENCPROB_PATTERN* p_data );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief ROMのエンカウントテーブルからポケモンを抽選
   *
   * @param data            エンカウントデータ
   *
   * @retval true    取得できた。
   * @retval false   取得できない。
   */
  /* -------------------------------------------------------------------------*/
  bool SetNormalEncountData( const ENCOUNT_DATA& data, BattleEffectId btl_eff = BATTLE_EFFECT_MAX );

  //----------------------------------------------------------------------------
  /**
   *  @brief  シンボルエンカウントパラメータを 汎用エンカウントポケモンデータ型にコンバート
   *
   *  @retval true    取得できた。
   *  @retval false   取得できない。
   */
  //-----------------------------------------------------------------------------
  bool ConvSymbolEncountPokeParam( const SymbolEncountParam* param, EncountPokeParam* epp );

  //----------------------------------------------------------------------------
  /**
   *  @brief  シンボルエンカウントデータをセット
   *
   *  @retval true    取得できた。
   *  @retval false   取得できない。
   */
  //-----------------------------------------------------------------------------
  bool SetSymbolEncountData( const SymbolEncountParam* param );

  //----------------------------------------------------------------------------
  /**
   *  @brief  パラメータ指定でエンカウントデータをセット
  */
  //-----------------------------------------------------------------------------
  bool SetEncountDataFromParam( MonsNo monsNo, FormNo formNo, u32 level, BattleEffectId btl_eff );

  //----------------------------------------------------------------------------
  /**
   *  @brief  パラメータ指定でエンカウントデータをセット(構造体による指定)
   */
  //-----------------------------------------------------------------------------
  bool SetEncountDataFromParam( const EncountPokeParam& param,  BattleEffectId btl_eff );

  //----------------------------------------------------------------------------
  /**
   *  @brief 抽選したポケモンの情報を使用して引数で渡したポケモンパラメーターのセットアップをする
   *  @param pPokeParam セットアップするポケモンパラメーター
   *  @param pRandom 乱数管理インスタンス
   *  @param intruder_type 乱入種類
   *  @param rare_try_count レアになるまで乱数を引き直す回数
   *  @retval true 抽選成功
   *  @retval true 抽選失敗
   */
  //-----------------------------------------------------------------------------
  bool SetupLotteryIntruderPokeParam( pml::pokepara::PokemonParam* pPokeParam, gfl2::math::SFMTRandom* pRandom, IntruderType intruder_type, u8 rare_try_count);

  //----------------------------------------------------------------------------
  /**
   *  @brief シンボル援軍ポケモンのデータ数を取得する
   *
   *  @retval 援軍ポケモンの数
   */
  //-----------------------------------------------------------------------------
  int GetReinforcePokeNum( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief シンボル援軍ポケモンの情報をセット
   *
   *  @param pPokeParam セットアップするポケモンパラメーター
   *  @param reinforce_type  援軍抽選タイプ
   *  @param reinforce_count 援軍抽選カウント上限
   *  @retval true 取得成功
   *  @retval true 取得失敗
   */
  //-----------------------------------------------------------------------------
  bool SetReinforcePokeParam( const SymbolEncountParam* param, const ReinforcePokeType reinforce_type, const int reinforce_count );

  //----------------------------------------------------------------------------
  /**
   *  @brief シンボル援軍ポケモンの情報を取得する
   *
   *  @param index   援軍ポケモンデータIndex
   *  @param pPokeParam セットアップするポケモンパラメーター
   *  @param reinforce_type  援軍抽選タイプ
   *  @param reinforce_count 援軍抽選カウント上限
   *  @retval true 取得成功
   *  @retval true 取得失敗
   */
  //-----------------------------------------------------------------------------
  bool GetReinforcePokeParam( int index, pml::pokepara::PokemonParam* pPokeParam, ReinforcePokeType* reinforce_type, int* reinforce_count );

  //----------------------------------------------------------------------------
  /**
   *  @brief 指定ポケモンパラメーターから特性、アイテム効果の発動をチェックする
   *  @param pPokeParam チェックするポケモンパラメーター
   *  @param pRandom 乱数管理インスタンス
   *  @note SetupLotteryIntruderPokeParam()の直前に呼んでください
   */
  //-----------------------------------------------------------------------------
  void CheckEffectApplyIntruder( const pml::pokepara::PokemonParam& pPokeParam, gfl2::math::SFMTRandom* pRandom);

  //----------------------------------------------------------------------------
  /**
   *  @brief  メンバにセットされたENC_POKE_PARAM[]を用いてPokePartyをセット
   *
   *  @retval true    取得できた。
   *  @retval false   取得できない。
   */
  //-----------------------------------------------------------------------------
  bool GetPokeParty( gfl2::heap::HeapBase* p_heap, pml::PokeParty* p_party ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  メンバにセットされたENC_POKE_PARAM[]を用いてDefaultPowerUpDescをセット
   *
   *  @retval true    取得できた。
   *  @retval false   取得できない。
   */
  //-----------------------------------------------------------------------------
  bool GetDefaultPowerUp( gfl2::heap::HeapBase* p_heap, DefaultPowerUpDesc* p_powerUp ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  遠景IDを取得
   */
  //-----------------------------------------------------------------------------
  inline btl::bg::bgsys_far_typeID GetBgFarTypeId(void) const { return m_far_bg; }
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  内景IDを取得
   */
  //-----------------------------------------------------------------------------
  inline btl::bg::bgsys_near_typeID GetBgNearTypeId(void) const { return m_near_bg; }
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  Initializeに引き渡されたマップアトリビュート取得
   *
   *  @note 通常はエンカウント時の足元アトリビュート
   *        釣りの際には、ルアー座標のアトリビュートが格納される
   */
  //-----------------------------------------------------------------------------
  inline Attr GetEncountAttr(void) const { return m_attr; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  セットされたパラメータからバトルエフェクトIDを取得
   */
  //-----------------------------------------------------------------------------
  BattleEffectId GetBattleEffectId( void ) const;
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  野生戦のモードを取得
   */
  //-----------------------------------------------------------------------------
  WildEncMode GetWildEncMode( void ) const { return m_btl_mode; };
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  釣り戦闘かどうかを取得
   */
  //-----------------------------------------------------------------------------
  bool IsFishing( void ) const { return m_flags.fishing_f; };

  /* -------------------------------------------------------------------------*/
  /**
   * @brief セットされているポケモンが指定したポケモンと同じか
   *
   * @return trueの時同じ
   */
  /* -------------------------------------------------------------------------*/ 
  bool IsSame( u32 mons_no ) const;

  /* -------------------------------------------------------------------------*/
  /**
   * @brief セットされているポケモンがすべて同じか
   *
   * @return trueの時同じ
   */
  /* -------------------------------------------------------------------------*/
  bool IsAllSame( void ) const;

  /* -------------------------------------------------------------------------*/
  /**
   * @brief エンカウントするポケモンナンバー（先頭のポケモンナンバー）
   *
   * @return モンスターナンバー
   */
  /* -------------------------------------------------------------------------*/
  u32 GetTopMonsNo( void ) const;

  /* -------------------------------------------------------------------------*/
  /**
   * @brief エンカウントするポケモンデータ（先頭のポケモンデータ）
   *
   * @return モンスターデータ
   */
  /* -------------------------------------------------------------------------*/
  const EncountPokeParam* GetTopMonsEncountParam( void ) const;


  /* -------------------------------------------------------------------------*/
  /**
   * @brief エンカウト数を取得
   *
   * @return エンカウント数
   */
  /* -------------------------------------------------------------------------*/
  int GetEncountNum( void ) const;

  /* -------------------------------------------------------------------------*/
  /**
   * @breif エンカウントデータからMonsNoとFormNoを取得する
   * @param pEncData エンカウントデータ
   * @parma pOutMonsNo モンスターNoが格納される
   * @parma pOutFormNo フォルムNoが格納される
   */
  /* -------------------------------------------------------------------------*/
  void GetFirstMonsInfo(const ENCOUNT_DATA& pEncData, MonsNo* pOutMonsNo, FormNo* pOutFormNo) const;

  /* -------------------------------------------------------------------------*/
  /**
   * @breif エンカウントデータのセット
   * @param data エンカウントデータ
   */
  /* -------------------------------------------------------------------------*/
  void SetEncountData( const ENCOUNT_DATA& data);

  /* -------------------------------------------------------------------------*/
  /**
   * @breif エンカウントデータのリセット
   */
  /* -------------------------------------------------------------------------*/
  void ResetEncountData();

  /* -------------------------------------------------------------------------*/
  /**
   * @breif エンカウントデータが存在するか
   */
  /* -------------------------------------------------------------------------*/
  bool IsEncountDataExist() const ;

  /* -------------------------------------------------------------------------*/
  /**
   * @breif ポケモンサーチエンカウントを回避するか
   * @param encPokeLevel エンカウントポケモンのレベル
   * @retval 回避する
   * @retval 回避しない
   */
  /* -------------------------------------------------------------------------*/
  bool IsPokemonSearchEncountAvoid( u32 encPokeLevel);

private:

  //エンカウント抽選パターン定義
  enum EncProbCalcType{
    ENCPROB_CALCTYPE_NORMAL,  ///< 通常
    ENCPROB_CALCTYPE_MAX,
  };

  //同時にエンカウントするポケモン数
  enum{
    ENC_POKENUM_NORMAL = 1,   ///< 通常エンカウントは同時1体
  };

  //アイテム抽選テーブルの数
  enum{
    ITEM_RANGE_TBL_NUM = 4,
  };

  //アイテム抽選率テーブル
  static const u8 DATA_ItemRangeTable[ITEM_RANGE_TBL_NUM][pml::personal::ITEM_SLOT_NUM];

private:

  //エンカウントポケモンデータ
  EncountPokeParam            m_pokeTbl[ pml::PokeParty::MAX_MEMBERS ];
  ReinforcePokeParam          m_reinforceTbl[ REINFORCE_POKE_NUM ];
  int                         m_enc_num;        ///<エンカウントポケモンの有効数
  int                         m_tbl_num;
  int                         m_element_num;
  int                         m_reinforce_num;  ///<援軍の有効数

  //エンカウントのシチュエーションを保持するメンバ
  Attr                        m_attr;                  ///< アトリビュート
  Encount::Type               m_enc_type;              ///< エンカウントタイプ
  EncProbCalcType             m_prob_calctype;         ///< データ抽選パターン

  btl::bg::bgsys_far_typeID   m_far_bg;
  btl::bg::bgsys_near_typeID  m_near_bg;
  BattleEffectId              m_btl_eff;
  WildEncMode                 m_btl_mode;

  EffectApplyCheckInfo        m_topPokeInfo;            ///< プレイヤーの手持ちのタマゴでない先頭(瀕死はOK)ポケモンのパラメータ
  EffectApplyCheckInfo        m_topPokeInfoIntruder;    ///< 乱入特性アイテム効果適用チェック用

  u32                         m_my_id;                  ///< プレイヤーのトレーナーＩＤ
  u8                          m_real_region_formno;     ///< リアル地域ポケモンフォルムNo.

  EffectApplyCheckFlag        m_flags;                  ///< フラグ
  EffectApplyCheckFlag        m_flagsIntruder;          ///< フラグ乱入用

  ENCOUNT_DATA                m_encount_data;           ///< エンカウントデータ
  bool                        m_is_encount_data_exist;  ///< エンカウントデータが存在しているか(使用可能か)

  u32                         m_intruderIndex;          ///< 乱入ポケモンのブロック内番号

  weather::WeatherKind    m_weather;                ///< 天候定義。乱入用に保存

private:

  //----------------------------------------------------------------------------
  /**
   *  @brief  特性効果発揮チェック 
   *
   *  @param pFlag チェックの反映を行う、特性、道具効果適用チェックフラグのポインタ
   *  @param weather 天候コード 
   *  @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
   *  @param info チェック情報
   */
  //-----------------------------------------------------------------------------
  void CheckMonsSpa( EffectApplyCheckFlag* pFlag, weather::WeatherKind weather, gfl2::math::SFMTRandom* pRandom, const EffectApplyCheckInfo& info);

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウントデータの通常エンカウントポケモンデータブロックを取得
   *  @param now_time_data 現在の時間帯のエンカウントデータ
   *
   *  @return テーブルポインタ
   */
  //-----------------------------------------------------------------------------
  const COMMON_POKE_DATA* GetEncountPokeBlockData( const COMMON_TIME_ZONE_DATA& now_time_data ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  エンカウントデータの乱入エンカウントポケモンデータブロックを取得
   *  @param now_time_data 現在の時間帯のエンカウントデータ
   *  @param intruder_type 乱入種類
   *  @param block_index ブロック番号(通常乱入…0～6、雨・雪・砂嵐乱入…0～1)
   *
   *  @return テーブルポインタ
   */
  //-----------------------------------------------------------------------------
  const COMMON_POKE_DATA* GetEncountIntruderPokeBlockData( const COMMON_TIME_ZONE_DATA& now_time_data, IntruderType intruder_type, int block_index ) const ;
  
  /* -------------------------------------------------------------------------*/
  /**
   * @brief  通常エンカウントデータ抽選
   *
   * @param enc_poke_block エンカウトポケモンデータブロック
   * @param minLevel 最小レベル
   * @param maxLevel 最大レベル
   *
   * @return 抽選に成功した時true
   */
  /* -------------------------------------------------------------------------*/
  bool LotteryEncountPoke( const COMMON_POKE_DATA* enc_poke_block, int minLevel, int maxLevel);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 乱入エンカウントデータ抽選
   *
   * @param p_out_epp エンカウントポケモンパラメータ出力先
   * @param enc_poke_block エンカウトデータ
   * @param intruder_type 乱入種類
   * @param minLevel 最小レベル
   * @param maxLevel 最大レベル
   * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
   *
   * @return 抽選に成功した時true
   */
  /* -------------------------------------------------------------------------*/
  bool LotteryEncountIntruderPoke( EncountPokeParam* p_out_epp, const COMMON_POKE_DATA* enc_poke_block, IntruderType intruder_type, int minLevel, int maxLevel, gfl2::math::SFMTRandom* pRandom);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief  エンカウントポケモンデータ取得
   *
   * @param p_out_ep 格納先
   * @param p_max_level 格納したポケモンの最大レベル
   * @param p_poke_data 格納するエンカウントポケモンデータ
   * @param flags 特性、道具効果適用チェックフラグ
   * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
   * @param minLevel 最小レベル
   * @param maxLevel 最大レベル
   */
  /* -------------------------------------------------------------------------*/
  void GetEncountPokeParam( EncountPokeParam* p_out_epp, int* p_max_level, const COMMON_POKE_DATA* p_poke_data, int minLevel, int maxLevel, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom);

  /* ------------------------------------------------------------------------- */
  /**
   * @brief ビードロレベル補正
   *
   * @param level         [in] レベル
   * @param flags 特性、道具効果適用チェックフラグ
   * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
   *
   * @return 補正後のレベル
   */
  /* ------------------------------------------------------------------------- */
  int CalcLevelFixBiidoro( int level, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom ) const;

  //--------------------------------------------------------------
  /**
   * @brief 隠し特性込みのエンカウントレベル取得
   *
   * @param enc_poke_block エンカウントポケモンデータブロック
   * @param minLevel 最小レベル
   * @param maxLevel 最大レベル
   * @param flags 特性、道具効果適用チェックフラグ
   * @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
   *
   * @return  レベル
   */
  //--------------------------------------------------------------
  int LotEncountLevel( const COMMON_POKE_DATA* enc_poke_block, int minLevel, int maxLevel, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom);

  //--------------------------------------------------------------
  /**
   * @brief 低レベルエンカウント回避チェック
   *
   * @param   max_level 同時にエンカウントするポケモンで一番高いレベル
   * @retval  true  回避する
   * @retval  false 回避しない
   */
  //--------------------------------------------------------------
  bool CheckLowLevelEncountAvoid( int max_level );

  //----------------------------------------------------------------------------
  /**
   *  @brief  特性による特定タイプの優先エンカウント
   *
   *  @retval 0〜 優先エンカウントするテーブルIndex
   *  @retval -1  優先エンカウントする対象が見つからなかった
   */
  //-----------------------------------------------------------------------------
  int LotFixTypeEncount( const COMMON_POKE_DATA* enc_poke_block, int type );

  //--------------------------------------------------------------
  /**
  * @brief テーブル内番号の抽選共通処理
  * @param 確率テーブルポインタ
  * @param 確率テーブル長
  * @return 抽選されたテーブル内番号
  */
  //--------------------------------------------------------------
  int LotRandomEncountCommon( const u8* tbl, u32 tbl_len );

  //--------------------------------------------------------------
  /**
   * @brief 抽選共通処理
   * @param rnd 乱数
   * @param tbl 確率が格納された配列
   * @param tbl_len tblサイズ
   */
  //--------------------------------------------------------------
  s8 LotteryCommon( u32 rnd, const u8* tbl, u32 tbl_len );

  //----------------------------------------------------------------------------
  /**
   *  @brief  通常エンカウントブロック内Index抽選
   *
   *  @retval 0～ エンカウントデータブロック内Index
   */
  //-----------------------------------------------------------------------------
  int LotNormalEncount();

  //----------------------------------------------------------------------------
  /**
   *  @brief  パーソナルから持たせるアイテムを抽選
   *  @param monsno モンスターNo
   *  @param form フォルムNo
   *  @param flags 特性、道具効果適用チェックフラグ
   *  @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
   */
  //-----------------------------------------------------------------------------
  u16 ItemGetFromPersonal( u32 monsno, u8 form, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief ポケモン初期化パラメーターを初期化
   *  @param p_out_spec 初期化するポケモン初期化パラメーター
   *  @param enc_poke_param 元になるエンカウントポケモンパラメーター
   *  @param flags 特性、道具効果適用チェックフラグ
   */
  //-----------------------------------------------------------------------------
  void InitializePokeParamInitialSpec( pml::pokepara::InitialSpec* p_out_spec, const EncountPokeParam* enc_poke_param, const EffectApplyCheckFlag& flags) const ;

  //----------------------------------------------------------------------------
  /**
   *  @brief ポケモン初期化パラメーターを初期化
   *  @param p_out_spec 初期化するポケモン初期化パラメーター
   *  @param enc_poke_param 元になるエンカウントポケモンパラメーター
   *  @param flags 特性、道具効果適用チェックフラグ
   *  @param rare_try_count レアになるまで乱数を引き直す回数
   */
  //-----------------------------------------------------------------------------
  void InitializePokeParamInitialSpecIntruder( pml::pokepara::InitialSpec* p_out_spec, const EncountPokeParam* enc_poke_param, const EffectApplyCheckFlag& flags, u8 rare_try_count) const ;

  //----------------------------------------------------------------------------
  /**
   *  @brief ポケモンパラメーターを初期化
   *  @param p_out_param 生成したポケモンパラメーターの格納先
   *  @param spec ポケモン初期化パラメーター
   *  @param enc_poke_param エンカウントポケモンパラメーター
   *  @param flags 特性、道具効果適用チェックフラグ
   *  @param pRandom 乱数管理インスタンス(NULLならPublicRandを使用する)
   */
  //-----------------------------------------------------------------------------
  void InitializePokeParam( pml::pokepara::PokemonParam* p_out_param, const pml::pokepara::InitialSpec& spec, const EncountPokeParam* enc_poke_param, const EffectApplyCheckFlag& flags, gfl2::math::SFMTRandom* pRandom) const ;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモン追加
   */
  //-----------------------------------------------------------------------------
  void AddPoke( gfl2::heap::HeapBase* p_heap, const EncountPokeParam* param, pml::PokeParty* p_party ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモン追加
   */
  //-----------------------------------------------------------------------------
  void SetPowerUp( const EncountPokeParam* param, DefaultPowerUpDesc* p_powerup ) const;

  /* ------------------------------------------------------------------------- */
  /**
   * @brief バトルモードをセット
   *
   * @param mode WildEncMode
   */
  /* ------------------------------------------------------------------------- */
  void SetBattleMode( WildEncMode mode );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 現在の時間帯のエンカウントデータを取得
   * @param エンカウントデータ
   *
   * @return 現在の時間帯のエンカウントデータ
   */
  /* ------------------------------------------------------------------------- */
  const COMMON_TIME_ZONE_DATA& GetNowTimeZoneEncountData( const ENCOUNT_DATA& data ) const ;

  //-------------------------------------------------------------------------
  /**
   * @brief 乱入ポケモンのブロック番号を抽選
   * @param pOutFixType ブロック番号抽選後の乱入種類
   * @param pRandom 乱数管理インスタンス
   * @type type 乱入種類
   * @return 抽選されたブロック番号
   */
  //-------------------------------------------------------------------------
  s8 LotteryIntruderPokeBlock( IntruderType* pOutFixType, gfl2::math::SFMTRandom* pRandom, IntruderType type);

  //-------------------------------------------------------------------------
  /**
   * @brief 特性、道具効果チェック用情報セット
   * @param pInfo セットするインスタンスのポインタ
   * @param pPokeParam 情報を取得するポケモンパラメーター
   */
  //-------------------------------------------------------------------------
  void SetEffectApplyCheckInfo( EffectApplyCheckInfo* pInfo, const pml::pokepara::PokemonParam& pPokeParam);

  //----------------------------------------------------------------------------
  /**
   *  @brief  特性による特定タイプの優先エンカウント (乱入用)
   *  @param pOutEncPokeParam 抽選したポケモンデータ格納用
   *  @param pNowTimeEncPokeBlock 現在の時間帯のブロック配列
   *  @param type ポケモンタイプ(はがね、でんきとか)
   *  @param pRandom 乱数インスタンス
   *  @retval true 抽選した
   *  @retval false 抽選していない
   */
  //-----------------------------------------------------------------------------
  bool LotFixTypeEncountIntruder( EncountPokeParam* pOutEncPokeParam, const COMMON_TIME_ZONE_DATA& nowTimeEncPokeBlock, int type, gfl2::math::SFMTRandom* pRandom );

  //----------------------------------------------------------------------------
  /**
   *  @brief  特性による特定タイプの優先エンカウント (乱入用、乱入種類指定)
   *  @param pOutEncPokeParam 抽選したポケモンデータ格納用
   *  @param pNowTimeEncPokeBlock 現在の時間帯のブロック配列
   *  @param type ポケモンタイプ(はがね、でんきとか)
   *  @param pRandom 乱数インスタンス
   *  @retval true 抽選した
   *  @retval false 抽選していない
   */
  //-----------------------------------------------------------------------------
  bool LotFixTypeEncountIntruderEx( EncountPokeParam* pOutEncPokeParam, const COMMON_TIME_ZONE_DATA& nowTimeEncPokeBlock, int type, gfl2::math::SFMTRandom* pRandom, IntruderType intruder_type );

  //----------------------------------------------------------------------------
  /**
   *  @brief 乱数取得
   *  @param max 最大値
   *  @param pRandom 乱数インスタンス
   *  @return 乱数 (maxは含まれない, 0～max-1が返る)
   *  @note pRandomがNULLならGetPublicRandを使用する
   */
  //-----------------------------------------------------------------------------
  u32 GetRandom( u32 max, gfl2::math::SFMTRandom* pRandom) const ;

  //----------------------------------------------------------------------------
  /**
   *  @brief ライドごとに調整したエンカウント率を返す
   */
  //-----------------------------------------------------------------------------
  u32 GetRideAdjustEncountProb( const u32 in_prob, const u32 max_prob, MoveModel::FieldMoveModelPlayer * cp_player ) const ;

  //----------------------------------------------------------------------------
  /**
   *  @brief レア抽選回数計算
   *  @param src 元の抽選回数
   *  @param flags 特性、道具効果適用チェックフラグ
   *  @return srcに特性、どうぐの効果を適用したレア抽選回数
   */
  //-----------------------------------------------------------------------------
  u8 CalcRareTryCount( u8 src, const EffectApplyCheckFlag& flags) const ;

  //----------------------------------------------------------------------------
  /**
  *  @brief 100%レア、100%ノーマルの処理をしてよいゾーンか？（momijiひずみ世界専用）
  *  @return true　で対象ゾーン　false　で非対象ゾーン
  */
  //-----------------------------------------------------------------------------
  bool IsMustRareEncZone(void);
};


GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

#endif  // FIELD_ENCOUNT_POKE_SET_H_INCLUDED

