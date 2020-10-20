//======================================================================
/**
 * @file QRDefine.h
 * @brief QR系定義
 */
//======================================================================

#if !defined __QR_DEFINE_H_INCLUDED__
#define __QR_DEFINE_H_INCLUDED__
#pragma once

#include <Savedata/include/MyStatus.h>

#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_StrSys.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QR)

/**
 * @brief　定数
 */
enum
{
  BINARY_SIZE_TYPE_1 = 88,
  BINARY_SIZE_TYPE_2 = 486,
  MODULE_BITS_LEN = 768,
  SIGN_VERIFY_BUFFER_LEN = (MODULE_BITS_LEN / 8) * 3,
  CRYPTO_OVERHEAD = 8,
  FOOTER_SIZE = 10
};

/**
 * @brief 共通ヘッダ(暗号化される領域)
 */
struct QR_HEADER
{
  u32 romVersion;       //ＲＯＭ識別
  u16 region;           //リージョン識別
  u8 padding[2];      
};

/**
 * @brief 共通ヘッダ(平分領域)
 */
struct QR_FOOTER
{
  u8 type[10];          //ＱＲコードのタイプ識別
};

/**
 * @brief 共通フォーマット１（ZOO）
 */
struct QR_TYPE_MIN
{
  u8 cryptoBinary[96];//暗号化済みバイナリ
  QR_FOOTER footer;
};



/**
 * @brief 共通フォーマット２
 */
struct QR_TYPE_MAX
{
  u8 cryptoBinary[494];//暗号化済みバイナリ
  QR_FOOTER footer;
};

/**
 * @brief 図鑑用
 */
struct QR_ZUKAN
{
  enum
  {
    SEE_FLAG_MALE = 0,		// ♂or性別なし
    SEE_FLAG_FEMALE,			// ♀
    SEE_FLAG_M_RARE,			// ♂or性別なしレア
    SEE_FLAG_F_RARE,			// ♀レア
    SEE_FLAG_MAX,
  };

  QR_HEADER header;           //ヘッダ
  u32 formBit[SEE_FLAG_MAX];  //フォルム３２bit
  u32 rand[SEE_FLAG_MAX];     //個性乱数
  u16 monsNo;                 //全国図鑑番号
  u8 dispForm;                //表示ポケモンのフォルム
  u8 dispSex;                 //表示ポケモンの性別
  u8 dispRare;                //表示ポケモンのレア判定
  u8 dispSexSame;             //表示ポケモンがオスメスで同じ姿のとき1、そうでないとき(オスメスで姿が違う、オスしかない、メスしかない、性別不明のとき)0  // @fix NMCat[831] 性別による違いがないポケモンの図鑑QRについての質問
  u8 padding[42];             //パディング
  u8 signOverhead[8];         //暗号化用増加分   
  u8 padding2[2];             //アライメント調整用
  QR_FOOTER footer;           //フッタ
};                       



/**
 * @brief ZOO用
 */
struct QR_ZOO
{
  u32 romVersion;       //ＲＯＭ識別
  u16 region;           //リージョン識別
  u16 monsNo;
  u8  form;
  u8  rare;
  u8  sex;
};




/**
 * @brief バトルチーム用
 */
struct QR_POKE
{
  u32 personal_rnd;
  u8  training_flag;
  u8  training_padding[3];
  u32 pointup_used_count;
  u32 talent_hp      : 5;
  u32 talent_atk     : 5;
  u32 talent_def     : 5;
  u32 talent_agi     : 5;
  u32 talent_spatk   : 5;
  u32 talent_spdef   : 5;
  u32 tamago_flag    : 1;     //無視
  u32 nickname_flag  : 1;     //無視
  u16 pp_fast_mode      : 1;  //無視
  u16 ppp_fast_mode     : 1;  //無視
  u16 fusei_tamago_flag : 1;  //無視
  u16 padding0          : 13;
  u16 checkSum;               //無視
  u16 monsno;
  u16 itemno;
  u16 waza[4];
  u8 tokuseino;               //無視
  u8 tokusei_1_flag : 1;
  u8 tokusei_2_flag : 1;
  u8 tokusei_3_flag : 1;
  u8 padding1       : 5;
  u8 seikaku;
  u8 event_get_flag : 1;
  u8 sex            : 2;
  u8 form_no        : 5;
  u8 effort_hp;
  u8 effort_atk;
  u8 effort_def;
  u8 effort_agi;
  u8 effort_spatk;
  u8 effort_spdef;
  u8 multi_work;              //無視
  u8 familiarity;             //無視
  u8 get_ball;
  u8 level;
  u8 rom_version;
  u8 pokemon_language_code;
};


struct QR_BATTLE_TEAM_BODY
{
  QR_POKE pokeData[6];                    //ポケモンデータ
  u64 nexUniqueID;                        //NEXユニークID
  u8 battleTeamID[12];                    //バトルチームID
  gfl2::str::STRCODE playerName[13];      //プレイヤー名
  gfl2::str::STRCODE teamName[11];        //チーム名
  Savedata::MyStatus::ICON_DATA iconData; //顔アイコン
  u8 padding[70];                         //パディング
};


struct QR_BATTLE_TEAM
{
  QR_HEADER header;           //ヘッダ

  u8 initializeVector[16];    //暗号化用カウンタ初期値
  QR_BATTLE_TEAM_BODY body;   //QRバトルチーム本体
  u8 signature[8]; //署名による増加分

  QR_FOOTER footer;
};


/**
 * @brief 仲間大会レギュレーション用
 */
struct QR_FRIEND_REGULATION
{
  QR_HEADER header;           //ヘッダ

  u8 regulation[456];
  u32 param1;
  u32 param2;
  u8 padding[14];
  u8 signature[8];

  QR_FOOTER footer;
};

/**
 * @brief ライブ大会レギュレーション用
 */
struct QR_LIVE_REGULATION
{
  QR_HEADER header;           //ヘッダ

  u8 regulation[456];
  u8 padding[22];
  u8 signature[8];

  QR_FOOTER footer;
};


GFL_NAMESPACE_END(QR)
GFL_NAMESPACE_END(NetApp)

#endif // __QR_DEFINE_H_INCLUDED__
