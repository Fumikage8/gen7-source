//==============================================================================
/**
 * @brief  ポケモンパラメータ ローカル定義ヘッダ
 * @file   pml_PokemonParamLocal.h
 * @author obata_toshihiro
 * @date   2010.12.22
 */
//==============================================================================
#ifndef __PML_POKEPARAM_LOCAL_H__
#define __PML_POKEPARAM_LOCAL_H__
#include <pml/include/pml_Type.h>
#include <pml/include/pml_Const.h>


namespace pml {
  namespace pokepara {



    /**
     * @brief コアデータのヘッダ情報
     *
     * @note 
     * 高速処理モード中は常に復号化された状態を維持する.
     * 暗号・復号化を行わないことで処理の高速化を図る.
     */
    struct CoreDataHeader {
      u32 personal_rnd;           // 04h 個性乱数
      u16 pp_fast_mode      :1;   // 06h 高速処理モード( 不使用 )
      u16 ppp_fast_mode     :1;   // 06h 高速処理モード( 不使用 )
      u16 fusei_tamago_flag :1;   // 06h ダメタマゴフラグ
      u16                   :13;  // 06h
      u16 checksum;               // 08h チェックサム
    };


    /**
     * @brief コアデータ( ブロックＡ )
     */
    struct CoreDataBlockA {
      u16 monsno;            // 02h モンスターNo.
      u16 itemno;            // 04h 所持アイテムNo.
      u32 id;                // 08h IDNo.
      u32 exp;               // 0ch 経験値
      u8 tokuseino;          // 0dh 特性No.
      u8 tokusei_1_flag :1;  // 0eh 第1特性フラグ
      u8 tokusei_2_flag :1;  // 0eh 第2特性フラグ
      u8 tokusei_3_flag :1;  // 0eh 第3特性フラグ
#if PML_DEBUG
      u8 debug_edit_flag:1;  // 0eh デバッグで生成されたか？
      u8                :4;  // 0eh あまり
#else
      u8                :5;  // 0eh あまり
#endif
      u16 box_mark;          // 10h ボックスマーク
      u32 color_rnd;         // 14h 色違い乱数
      u8 seikaku;            // 15h 性格
      u8 event_get_flag :1;  // 16h イベントで配布したことを示すフラグ
      u8 sex            :2;  // 16h ポケモンの性別
      u8 form_no        :5;  // 16h フォルムNo.
      u8 effort_hp;          // 17h 努力値( HP )
      u8 effort_atk;         // 18h 努力値( こうげき )
      u8 effort_def;         // 19h 努力値( ぼうぎょ )
      u8 effort_agi;         // 1ah 努力値( すばやさ )
      u8 effort_spatk;       // 1bh 努力値( とくこう )
      u8 effort_spdef;       // 1ch 努力値( とくぼう )
      u8 style;              // 1dh かっこよさ
      u8 beautiful;          // 1eh うつくしさ
      u8 cute;               // 1fh かわいさ
      u8 clever;             // 20h かしこさ
      u8 strong;             // 21h たくましさ
      u8 fur;                // 22h 毛艶
      u8 resort_event_status;// 23h ポケモンリゾートのイベント状態
      u8 pokerus;            // 24h ポケルス
      u32 training_badge1;   // 28h 新規・トレーニング勲章（32個分）
      u32 ribbon_a;          // 2ch リボンフィールドA
      u32 ribbon_b;          // 30h リボンフィールドB
      u8 lumping_ribbon_a;   // 31h ひとまとめリボンフィールドA
      u8 lumping_ribbon_b;   // 32h ひとまとめリボンフィールドB
      u16 training_badge2;   // 34h トレーニング勲章（16個分）
      u32 multi_work;        // 38h 多目的ワーク（各種族ごとで目的が違う）
    };


    /**
     * @brief コアデータ( ブロックＢ )
     */
    struct CoreDataBlockB {
      STRCODE nickname[ MONS_NAME_BUFFER_SIZE ]; // 1ah ニックネーム
      u16 waza[ MAX_WAZA_NUM ];                  // 22h 所持技
      u8 pp[ MAX_WAZA_NUM ];                     // 26h 所持技の残りPP
      u8 pointup_used_count[ MAX_WAZA_NUM ];     // 2ah 所持技にポイントアップを使用した回数
      u16 tamago_waza[ MAX_WAZA_NUM ];           // 32h タマゴ技ストック
      u16 pokedeco;                              // 34h ポケデコ
      u32 talent_hp     :5;                      // 38h 個体値( HP )
      u32 talent_atk    :5;                      // 38h 個体値( こうげき )
      u32 talent_def    :5;                      // 38h 個体値( ぼうぎょ )
      u32 talent_agi    :5;                      // 38h 個体値( すばやさ )
      u32 talent_spatk  :5;                      // 38h 個体値( とくこう )
      u32 talent_spdef  :5;                      // 38h 個体値( とくぼう )
      u32 tamago_flag   :1;                      // 38h タマゴフラグ ( 0:タマゴじゃない, 1:タマゴだよ )
      u32 nickname_flag :1;                      // 38h ニックネームをつけたかどうかフラグ ( 0:つけていない, 1:つけた )
    };


    /**
     * @brief コアデータ( ブロックＣ )
     */
    struct CoreDataBlockC {
      STRCODE past_parents_name[ PERSON_NAME_BUFFER_SIZE ];  // 1ah 過去の親の名前
      u8 past_parents_sex;                                   // 1bh 過去の親の性別
      u8 owned_by_others;                                    // 1ch （新規）現在親以外に所有されているか
      u16 padding_0[ MAX_PAST_PARENTS_COUNT ];               // 26h 
      u16 familiar_trainer_id;                               // 28h 親以外のなつきトレーナーのID
      u16 friendly_trainer_id;                               // 2ah 親以外の仲良しトレーナーのID 
      u8 others_familiarity;                                 // 2bh 親以外のトレーナーとのなつき度
      u8 others_friendship;                                  // 2ch 親以外のトレーナーとの仲良し度
      u8 others_memories_level;                              // 2dh 親以外のトレーナーとの思い出レベル
      u8 others_memories_code;                               // 2eh 親以外のトレーナーとの思い出事象コード
      u8 others_memories_feel;                               // 2fh 親以外のトレーナーとの思い出気持ちデータ
      u8 padding_1;                                          // 30h
      u16 others_memories_data;                              // 32h 親以外のトレーナーとの思い出内容データ
      u16 padding_2;                                         // 34h 
      u16 padding_3;                                         // 36h 
      u8 feed;                                               // 37h 満腹度
      u8 nade_nade_value;                                    // 38h なでなで値
    };


    /**
     * @brief コアデータ( ブロックＤ )
     */
    struct CoreDataBlockD {
      STRCODE parents_name[ PERSON_NAME_BUFFER_SIZE ];  // 1ah 親の名前
      u8 familiarity;                                   // 1bh 親とのなつき度
      u8 friendship;                                    // 1ch 親との仲良し度
      u8 memories_level;                                // 1dh 親との思い出レベル
      u8 memories_code;                                 // 1eh 親との思い出事象コード
      u16 memories_data;                                // 20h 親との思い出内容データ
      u8 memories_feel;                                 // 21h 親との思い出気持ちデータ
      u8 egg_get_year;                                  // 22h タマゴをもらった年
      u8 egg_get_month;                                 // 23h タマゴをもらった月
      u8 egg_get_day;                                   // 24h タマゴをもらった日
      u8 first_contact_year;                            // 25h 捕獲した or 生まれた年
      u8 first_contact_month;                           // 26h 捕獲した or 生まれた月
      u8 first_contact_day;                             // 27h 捕獲した or 生まれた日
      u8 padding_0;                                     // 28h
      u16 get_place;                                    // 2ah 捕まえた場所
      u16 birth_place;                                  // 2ch 生まれた場所
      u8 get_ball;                                      // 2dh 捕まえたボール
      u8 get_level   :7;                                // 2eh 捕まえたレベル
      u8 parents_sex :1;                                // 2eh 親の性別
      u8 training_flag;                                 // 2fh すごい特訓をしたか？フラグ
      u8 get_cassette;                                  // 30h 捕まえたカセットバージョン
      u8 user_country_code;                             // 31h 国コード(DS以前では言語コードだが今作は違う）
      u8 user_region_code;                              // 32h 地域コード（DS以前のpref_codeは未使用）
      u8 hw_region_code;                                // 33h 3DS本体リージョンコード
      u8 lang_id;                                       // 34h 言語コード（LANG_JAPAN等、DS以前でのcountry_code）
      u32 padding_2;                                    // 38h 
    };





    /**
     * @brief コアデータブロックのＩＤ
     */
    enum CoreDataBlockId {
      CORE_DATA_BLOCK_A,
      CORE_DATA_BLOCK_B,
      CORE_DATA_BLOCK_C,
      CORE_DATA_BLOCK_D,
      CORE_DATA_BLOCK_NUM,
    }; 

    /**
     * @brief コアデータブロックのいずれかを格納する共用体
     */
    union CoreDataBlock {
      CoreDataBlockA block_a;
      CoreDataBlockB block_b;
      CoreDataBlockC block_c;
      CoreDataBlockD block_d;
    }; 


    //--------------------------------------------------------------------------
    /**
     * @brief コアデータ
     */
    //--------------------------------------------------------------------------
    struct CoreData {
      CoreDataHeader header;
      CoreDataBlock blocks[ CORE_DATA_BLOCK_NUM ];
    }; 


    //--------------------------------------------------------------------------
    /**
     * @brief コアデータから計算可能なデータ
     */
    //--------------------------------------------------------------------------
    struct CalcData {
      u32 sick;               ///< 04h 状態異常
      u8 level;               ///< 05h レベル
      u8 dirt_type;           ///< 06h 汚れの種類
      u8 dirt_pos;            ///< 07h 汚れの発生位置
      u8 padding_1;           ///< 08h 
      u16 hp;                 ///< 0ah HP
      u16 max_hp;             ///< 0ch HP最大値
      u16 atk;                ///< 0eh 攻撃力
      u16 def;                ///< 10h 防御力
      u16 agi;                ///< 12h 素早さ
      u16 spatk;              ///< 14h 特攻
      u16 spdef;              ///< 16h 特防
      u16 padding_2;          ///< 18h 
      u8 is_mega;             ///< 19h true:メガ進化だよ false:メガ進化じゃないよ
      u8 mega_before_formno;  ///< 1ah メガ進化する前のフォルムNo.
      u16 padding_3;          ///< 1ch
    };



  }  // namespace pokepara
}  // namespace pml


#endif  // __PML_POKEPARAM_LOCAL_H__
