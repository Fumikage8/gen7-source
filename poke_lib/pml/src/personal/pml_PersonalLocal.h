//=============================================================================
/**
 * @file   pml_PersonalLocal.h
 * @brief  ポケモンパーソナルデータ
 * @author obata_toshihiro
 * @date   2010.12.20
 */
//=============================================================================
#ifndef __PML_PERSONALLOCAL_H__
#define __PML_PERSONALLOCAL_H__

namespace pml { 
  namespace personal { 

    /**
     * @brief ポケモンパーソナルデータの基本要素
     */
    struct BasicParams 
    {
      u8  basic_hp;              // 0x00 基本ＨＰ
      u8  basic_atk;             // 0x01 基本攻撃力
      u8  basic_def;             // 0x02 基本防御力
      u8  basic_agi;             // 0x03 基本素早さ
      u8  basic_spatk;           // 0x04 基本特殊攻撃力
      u8  basic_spdef;           // 0x05 基本特殊防御力
      u8  type1;                 // 0x06 属性1
      u8  type2;                 // 0x07 属性2 
      u8  get_rate;              // 0x08 捕獲率
      u8  rank;                  // 0x09 ポケモンランク
      u16 pains_hp     :2;       // 0x0a 贈与努力値ＨＰ
      u16 pains_atk    :2;       // 0x0a 贈与努力値攻撃力
      u16 pains_def    :2;       // 0x0a 贈与努力値防御力
      u16 pains_agi    :2;       // 0x0a 贈与努力値素早さ
      u16 pains_spatk  :2;       // 0x0a 贈与努力値特殊攻撃力
      u16 pains_spdef  :2;       // 0x0a 贈与努力値特殊防御力
      u16 no_jump      :1;       // 0x0a はねる禁止フラグ
      u16              :3;       // 0x0a 贈与努力値予備
      u16 item1;                 // 0x0c アイテム1
      u16 item2;                 // 0x0e アイテム2
      u16 item3;                 // 0x10 アイテム3
      u8  sex;                   // 0x12 性別ベクトル
      u8  egg_birth;             // 0x13 タマゴの孵化歩数
      u8  initial_familiarity;   // 0x14 なつき度初期値
      u8  grow;                  // 0x15 成長曲線識別
      u8  egg_group1;            // 0x16 こづくりグループ1
      u8  egg_group2;            // 0x17 こづくりグループ2 
      u8  tokusei1;              // 0x18 とくせい1
      u8  tokusei2;              // 0x19 とくせい2
      u8  tokusei3;              // 0x1a とくせい3
      u8  sos_rate;              // 0x1b 助けを呼ぶ割合
      u16 form_index;            // 0x1c 別フォルムパーソナルデータ開始位置
      u16 form_gra_index;        // 0x1e 別フォルムグラフィックデータ開始位置
      u8  form_max;              // 0x20 フォルムの最大値( ノーマルを含むフォルムの数 )
      u8  color     :6;          // 0x21 色（図鑑で使用）
      u8  padding_1 :2;          // 0x21 
      u16 give_exp;              // 0x22 贈与経験値
      u16 height;                // 0x24 高さ[cm]
      u16 weight;                // 0x26 重さ
      u32 machine1;              // 0x28 技マシンフラグ1
      u32 machine2;              // 0x2c 技マシンフラグ2
      u32 machine3;              // 0x30 技マシンフラグ3
      u32 machine4;              // 0x34 技マシンフラグ4 
      u32 waza_oshie_kyukyoku;   // 0x38 技教え究極
      u32 waza_oshie_momiji1;    // 0x3c 教え技フラグ1(momiji追加分)
      u32 waza_oshie_momiji2;    // 0x40 教え技フラグ2(momiji追加分)
      u32 waza_oshie_momiji3;    // 0x44 教え技フラグ3(momiji追加分)
      u32 padding_5;             // 0x48 教え技フラグ4
      u16 zenryoku_waza_item;    // 0x4c ポケモン専用全力技を使用するための道具No.
      u16 zenryoku_waza_before;  // 0x4e ポケモン専用全力技の元技の技No.
      u16 zenryoku_waza_after;   // 0x50 ポケモン専用全力技の技No.
      u8  region;                // 0x52 地方種ID( pml::personal::RegionID )
      u8  padding_6;             // 0x53
    };

  } // namespace personal
} // namespace pml

#endif //__PML_PERSONALLOCAL_H__
