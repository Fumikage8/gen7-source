//=============================================================================
/**
 * @brief 技データ
 * @file pml_WazaDataLocal.h
 * @author obata_toshihiro
 * @date 2011.05.06
 */
//=============================================================================
#ifndef __PML_WAZADATALOCAL_H__
#define __PML_WAZADATALOCAL_H__

namespace pml {
  namespace wazadata { 


    struct WAZA_DATA 
    { 
      u8  type;                            ///< 0x00 タイプ（みず、くさ、ほのお…）
      u8  category;                        ///< 0x01 カテゴリ（ enum WazaCategory )
      u8  damageType;                      ///< 0x02 ダメージタイプ（ enum WazaDamageType )
      u8  power;                           ///< 0x03 威力
      u8  hitPer;                          ///< 0x04 命中率
      u8  basePP;                          ///< 0x05 基本PPMax
      s8  priority;                        ///< 0x06 ワザ優先度
      u8  hitCountMin : 4;                 ///< 0x07 最小ヒット回数
      u8  hitCountMax : 4;                 ///< 0x07 最大ヒット回数
      u16 sickID;                          ///< 0x08 状態異常コード（enum WazaSick）
      u8  sickPer;                         ///< 0x0a 状態異常の発生率
      u8  sickCont;                        ///< 0x0b 状態異常継続パターン（enum WazaSickCont）
      u8  sickTurnMin;                     ///< 0x0c 状態異常の継続ターン最小
      u8  sickTurnMax;                     ///< 0x0d 状態異常の継続ターン最大
      u8  criticalRank;                    ///< 0x0e クリティカルランク
      u8  shrinkPer;                       ///< 0x0f ひるみ確率
      u16 AISeqNo;                         ///< 0x10 AI用シーケンスナンバー
      s8  damageRecoverRatio;              ///< 0x12 ダメージ回復率
      s8  HPRecoverRatio;                  ///< 0x13 HP回復率
      u8  target;                          ///< 0x14 ワザ効果範囲( enum WazaTarget )
      u8  rankEffType[ RANKEFF_NUM_MAX ];  ///< 0x15 ステータスランク効果
      s8  rankEffValue[ RANKEFF_NUM_MAX ]; ///< 0x18 ステータスランク増減値
      u8  rankEffPer[ RANKEFF_NUM_MAX ];   ///< 0x1b ステータスランク発生率
      u16 zenryokuWazaNo;                  ///< 0x1e 全力技として使用した場合の、変化後の技No.
      u8  zenryokuPower;                   ///< 0x20 全力技として使用した場合の、変化後の威力
      u8  zenryokuEffect;                  ///< 0x21 全力技として使用した場合の、追加効果
      u8  dirtType;                        ///< 0x22 汚れタイプ( pml::pokepara::DirtType )
      u8  dirtRate;                        ///< 0x23 汚れる確率[%]
      u32 flags;                           ///< 0x24 各種フラグ
    };


  }
}

#endif //__PML_WAZADATALOCAL_H__
