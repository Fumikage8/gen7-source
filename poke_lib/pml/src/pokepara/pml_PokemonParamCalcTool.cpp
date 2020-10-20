//=======================================================================================
/**
 * @brief  ポケモンパラメータの計算
 * @file   pml_PokemonParamCalcTool
 * @author obata_toshihiro
 * @date   2010.12.29
 */
//=======================================================================================
#include <debug/include/gfl2_Assert.h>

#include <pml/include/pml_Type.h>
#include <pml/include/pml_Const.h>
#include <pml/include/pml_Personal.h>
#include <pml/include/pml_Waza.h>
#include <pml/include/pml_System.h>
#include "pml_PokemonParamCalcTool.h"


namespace pml {
  namespace pokepara { 


    // 能力値の計算に用いる係数
    static const u16 MIN_HP = 10;
    static const u16 HP_BASE_COEFFICIENT = 2;
    static const u16 HP_EXP_COEFFICIENT = 4;

    static const u16 MIN_ATK = 5;
    static const u16 ATK_BASE_COEFFICIENT = 2;
    static const u16 ATK_EXP_COEFFICIENT = 4;

    static const u16 MIN_DEF = 5;
    static const u16 DEF_BASE_COEFFICIENT = 2;
    static const u16 DEF_EXP_COEFFICIENT = 4;

    static const u16 MIN_SPATK = 5;
    static const u16 SPATK_BASE_COEFFICIENT = 2;
    static const u16 SPATK_EXP_COEFFICIENT = 4;

    static const u16 MIN_SPDEF = 5;
    static const u16 SPDEF_BASE_COEFFICIENT = 2;
    static const u16 SPDEF_EXP_COEFFICIENT = 4;

    static const u16 MIN_AGI = 5; 
    static const u16 AGI_BASE_COEFFICIENT = 2;
    static const u16 AGI_EXP_COEFFICIENT = 4; 


    // ヌケニンの最大HP
    static const u8 NUKENIN_MAX_HP = 1; 



    //-----------------------------------------------------------------------------------
    /**
     * @brief レベルを計算する
     *
     * @param monsno  モンスターNo.
     * @param formno  フォルムNo.
     * @param exp     経験値
     */
    //-----------------------------------------------------------------------------------
    u8 CalcTool::CalcLevel( MonsNo monsno, FormNo formno, u32 exp )
    {
      u8 level = 0;

      // 成長曲線テーブルを読み込む
      pml::personal::LoadGrowTable( monsno, formno );

      // 該当するレベルを検索
      for( level=1; level<=MAX_POKE_LEVEL; level++ )
      {
        if( exp < pml::personal::GetMinExp( level ) ) {
          break;
        }
      } 

      if( ( level - 1 ) <= 0 ) {
        return 1;  // レベル0は不正
      }
      return level - 1;
    } 

    //-----------------------------------------------------------------------------------
    /**
     * @brief HPの最大値を計算する
     *
     * @param monsno  モンスターNo.
     * @param level   レベル
     * @param base    種族値
     * @param rnd     個体値
     * @param exp     努力値
     */
    //-----------------------------------------------------------------------------------
    u16 CalcTool::CalcMaxHp( MonsNo monsno, u8 level, u16 base, u16 rnd, u16 exp )
    {
      u16 value = 0;

      if( monsno == MONSNO_NUKENIN ) // ヌケニンは必ず1
      { 
        value = NUKENIN_MAX_HP;
      }
      else 
      {
        value = ( ( HP_BASE_COEFFICIENT * base + rnd + exp / HP_EXP_COEFFICIENT ) * level / MAX_POKE_LEVEL + level + MIN_HP );
      }

      return value;
    } 

    //-----------------------------------------------------------------------------------
    /**
     * @brief 「こうげき」を計算する
     *
     * @param level    レベル
     * @param base     種族値
     * @param rnd      個体値
     * @param exp      努力値
     * @param seikaku  性格
     */
    //-----------------------------------------------------------------------------------
    u16 CalcTool::CalcAtk( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku )
    { 
      u16 value = 0;
      value = ( ( ATK_BASE_COEFFICIENT * base + rnd + exp / ATK_EXP_COEFFICIENT ) * level / MAX_POKE_LEVEL + MIN_ATK );
      value = CalcCorrectedPowerBySeikaku( value, seikaku, POWER_ATK );
      return value;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 「ぼうぎょ」を計算する
     *
     * @param level    レベル
     * @param base     種族値
     * @param rnd      個体値
     * @param exp      努力値
     * @param seikaku  性格
     */
    //-----------------------------------------------------------------------------------
    u16 CalcTool::CalcDef( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku )
    {
      u32 value = 0;
      value = ( ( DEF_BASE_COEFFICIENT * base + rnd + exp / DEF_EXP_COEFFICIENT ) * level / MAX_POKE_LEVEL + MIN_DEF );
      value = CalcCorrectedPowerBySeikaku( value, seikaku, POWER_DEF );
      return value;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 「とくこう」を計算する
     *
     * @param level    レベル
     * @param base     種族値
     * @param rnd      個体値
     * @param exp      努力値
     * @param seikaku  性格
     */
    //-----------------------------------------------------------------------------------
    u16 CalcTool::CalcSpAtk( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku )
    {
      u32 value = 0;
      value = ( ( SPATK_BASE_COEFFICIENT * base + rnd + exp / SPATK_EXP_COEFFICIENT ) * level / MAX_POKE_LEVEL + MIN_SPATK );
      value = CalcCorrectedPowerBySeikaku( value, seikaku, POWER_SPATK );
      return value;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 「とくこう」を計算する
     *
     * @param level    レベル
     * @param base     種族値
     * @param rnd      個体値
     * @param exp      努力値
     * @param seikaku  性格
     */
    //-----------------------------------------------------------------------------------
    u16 CalcTool::CalcSpDef( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku )
    {
      u32 value = 0;
      value = ( ( SPDEF_BASE_COEFFICIENT * base + rnd + exp / SPDEF_EXP_COEFFICIENT ) * level / MAX_POKE_LEVEL + MIN_SPDEF );
      value = CalcCorrectedPowerBySeikaku( value, seikaku, POWER_SPDEF );
      return value;
    } 

    //-----------------------------------------------------------------------------------
    /**
     * @brief 「すばやさ」を計算する
     *
     * @param level    レベル
     * @param base     種族値
     * @param rnd      個体値
     * @param exp      努力値
     * @param seikaku  性格
     */
    //-----------------------------------------------------------------------------------
    u16 CalcTool::CalcAgi( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku )
    {
      u32 value = 0;
      value = ( ( AGI_BASE_COEFFICIENT * base + rnd + exp / AGI_EXP_COEFFICIENT ) * level / MAX_POKE_LEVEL + MIN_AGI );
      value = CalcCorrectedPowerBySeikaku( value, seikaku, POWER_AGI );
      return value;
    }



    // 性格によるパラメータ変化値テーブル
    static const s8 SeikakuAbiTbl[][5]={
      // atk def sat sdf agi
      {  0,  0,  0,  0,  0, },  // がんばりや
      {  1, -1,  0,  0,  0, },  // さみしがり
      {  1,  0,  0,  0, -1, },  // ゆうかん
      {  1,  0, -1,  0,  0, },  // いじっぱり
      {  1,  0,  0, -1,  0, },  // やんちゃ
      { -1,  1,  0,  0,  0, },  // ずぶとい
      {  0,  0,  0,  0,  0, },  // すなお
      {  0,  1,  0,  0, -1, },  // のんき
      {  0,  1, -1,  0,  0, },  // わんぱく
      {  0,  1,  0, -1,  0, },  // のうてんき
      { -1,  0,  0,  0,  1, },  // おくびょう
      {  0, -1,  0,  0,  1, },  // せっかち
      {  0,  0,  0,  0,  0, },  // まじめ
      {  0,  0, -1,  0,  1, },  // ようき
      {  0,  0,  0, -1,  1, },  // むじゃき
      { -1,  0,  1,  0,  0, },  // ひかえめ
      {  0, -1,  1,  0,  0, },  // おっとり
      {  0,  0,  1,  0, -1, },  // れいせい
      {  0,  0,  0,  0,  0, },  // てれや
      {  0,  0,  1, -1,  0, },  // うっかりや
      { -1,  0,  0,  1,  0, },  // おだやか
      {  0, -1,  0,  1,  0, },  // おとなしい
      {  0,  0,  0,  1, -1, },  // なまいき
      {  0,  0, -1,  1,  0, },  // しんちょう
      {  0,  0,  0,  0,  0, },  // きまぐれ
    };

    
    /**
     * @brief 性格からパワー乱数補正値を取得
     *
     * @param seikaku 性格
     * @param power_id  どのパラメータなのか
     *
     * @return 0ならば変化量なし  -1ならば下降補正  +1ならば上昇補正
     */
    s8 CalcTool::GetPowerTransformBySeikaku( u16 seikaku, PowerID power_id )
    {
      if( ( power_id < POWER_ATK ) || ( POWER_AGI < power_id ) ) {
        GFL_ASSERT(0); //@check iwasawa このまま
        return 0;
      }
      return SeikakuAbiTbl[ seikaku ][ power_id - 1 ];
    }
    /**
     * @brief 性格によって補正を掛けた値を計算する
     *
     * @param value     補正前の値
     * @param seikaku   性格
     * @param power_id  どのパラメータなのか？[POWER_ATK, POWER_AGI]
     *
     * @return 補正後の値
     */
    u16 CalcTool::CalcCorrectedPowerBySeikaku( u16 value, u16 seikaku, PowerID power_id )
    {
      if( ( power_id < POWER_ATK ) || ( POWER_AGI < power_id ) ) {
        GFL_ASSERT(0); //@check iwasawa このまま
        return value;
      }

      u16 ret = 0;
      switch( SeikakuAbiTbl[ seikaku ][ power_id - 1 ] ) {
      case 1:
        ret = value * 110;
        ret /= 100;
        break;
      case -1:
        ret = value * 90;
        ret /= 100;
        break;
      default:
        ret = value;
        break;
      }
      return ret;
    }






    //-----------------------------------------------------------------------------------
    /**
     * @brief 色違いかどうかを調べる
     *
     * @param id   IDNo.
     * @param rnd  色違い乱数
     *
     * @return 色違いならtrue, そうでなければ false
     */
    //-----------------------------------------------------------------------------------
    bool CalcTool::IsRareColor( u32 id, u32 rnd )
    {
      // 2013.04.23 1/8192→1/4096 の確率に変更
      return ( ( ( ( id & 0xffff0000 ) >> 16 ) ^ ( id & 0xffff ) ^ ( ( rnd & 0xffff0000 ) >> 16 ) ^ ( rnd & 0xffff ) ) < 16 );
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 非色違いになるように調整する
     *
     * @param id   IDNo.
     * @param rnd  色違い乱数
     *
     * @return 色違いにならないように調整された乱数
     */
    //-----------------------------------------------------------------------------------
    u32 CalcTool::CorrectColorRndForNormal( u32 id, u32 rnd )
    {
      if( IsRareColor( id, rnd ) )
      {
        u32 new_rnd = rnd ^ 0x10000000;
        GFL_ASSERT( IsRareColor( id, new_rnd ) == false ); //@check iwasawa 計算が正しいかのチェックなので致命的ではない
        return new_rnd;
      }
      else
      {
        return rnd;
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 色違いになるように調整する
     *
     * @param id   IDNo.
     * @param rnd  色違い乱数
     *
     * @return 色違いになるように調整された乱数
     */
    //-----------------------------------------------------------------------------------
    u32 CalcTool::CorrectColorRndForRare( u32 id, u32 rnd )
    {
      if( IsRareColor( id, rnd ) )
      {
        return rnd;
      }
      else
      {
        u32 id_upper = ( id & 0xffff0000 ) >> 16;
        u32 id_lower = ( id & 0x0000ffff );
        u32 rnd_lower = ( rnd & 0x0000ffff );
        u32 rnd_upper = id_upper ^ id_lower ^ rnd_lower;
        u32 new_rnd = ( rnd_upper << 16 ) | rnd_lower;
        GFL_ASSERT( IsRareColor( id, new_rnd ) );  //@check iwasawa 計算が正しいかのチェックなので致命的ではない
        return new_rnd;
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief パーソナルデータと照らし合わせて, 正しい性別を取得する
     *
     * @param monsno     モンスターNo.
     * @param formno     フォルムNo.
     * @param both_case  ♂♀が両方存在する場合の性別
     *
     * @retval SEX_MALE     ♂しか存在しない or ♂♀両方存在する場合
     * @retval SEX_FEMALE   ♀しか存在しない or ♂♀両方存在する場合
     * @retval SEX_UNKNOWN  性別不明の場合
     */
    //-----------------------------------------------------------------------------------
    Sex CalcTool::GetCorrectSexInPersonalData( MonsNo monsno, FormNo formno, Sex both_case )
    {
      // 性別ベクトルを取得
      personal::LoadPersonalData( monsno, formno );
      u8 sex_vector = personal::GetPersonalParam( personal::PARAM_ID_sex );

      // 性別固定のケース
      switch( sex_vector ) {
      case personal::SEX_VECTOR_ONLY_MALE: return SEX_MALE;
      case personal::SEX_VECTOR_ONLY_FEMALE: return SEX_FEMALE;
      case personal::SEX_VECTOR_UNKNOWN: return SEX_UNKNOWN;
      }

      // ♂♀両方が存在する場合
      if( both_case == SEX_UNKNOWN ) {
        return SEX_MALE;  // 不正な場合は強制的に♂にする
      }
      return both_case;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 特性No.を取得する
     * @param monsno          モンスターNo.
     * @param formno          フォルムNo.
     * @param tokusei_index   何番目の特性なのか
     */
    //-----------------------------------------------------------------------------------
    TokuseiNo CalcTool::GetTokuseiNo( MonsNo monsno, FormNo formno, u8 tokusei_index )
    {
      GFL_ASSERT( tokusei_index <= MAX_TOKUSEI_INDEX ); //@check iwasawa

      personal::ParamID param_id;
      switch( tokusei_index ) {
      default:
        GFL_ASSERT(0); //@check iwasawa 不定値だったらtokusei1の値を取得する
        /* fall through */
      case 0: param_id = personal::PARAM_ID_tokusei1; break;
      case 1: param_id = personal::PARAM_ID_tokusei2; break;
      case 2: param_id = personal::PARAM_ID_tokusei3; break;
      } 

      personal::LoadPersonalData( monsno, formno );
      u16 tokusei_no = personal::GetPersonalParam( param_id );
      return static_cast<TokuseiNo>( tokusei_no );
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief「めざめるパワー」のタイプを計算する
     *
     * @param hp    個体値(HP)
     * @param atk   個体値(こうげき)
     * @param def   個体値(ぼうぎょ)
     * @param agi   個体値(すばやさ)
     * @param spatk 個体値(とくこう)
     * @param spdef 個体値(とくぼう)
     */
    //-----------------------------------------------------------------------------------
    PokeType CalcTool::CalcMezamerupawaaType( u8 hp, u8 atk, u8 def, u8 agi, u8 spatk, u8 spdef )
    {
      static const u8 type_table[ ] = {
        POKETYPE_KAKUTOU, POKETYPE_HIKOU, POKETYPE_DOKU,   POKETYPE_JIMEN,
        POKETYPE_IWA,     POKETYPE_MUSHI, POKETYPE_GHOST,  POKETYPE_HAGANE,
        POKETYPE_HONOO,   POKETYPE_MIZU,  POKETYPE_KUSA,   POKETYPE_DENKI,
        POKETYPE_ESPER,   POKETYPE_KOORI, POKETYPE_DRAGON, POKETYPE_AKU,
      };
      static const u8 type_table_size = GFL_NELEMS(type_table);

      u32 val = 0;
      if( hp    & 1 ) { val += 1; }
      if( atk   & 1 ) { val += 2; }
      if( def   & 1 ) { val += 4; }
      if( agi   & 1 ) { val += 8; }
      if( spatk & 1 ) { val += 16; }
      if( spdef & 1 ) { val += 32; }

      u32 idx = val * ( type_table_size - 1 ) / 63;  // valは最大63
      while( type_table_size <= idx ) { // テーブルサイズを越えることは有り得ないハズだが念のため
        idx -= type_table_size;
      } 
      return type_table[ idx ];
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief「めざめるパワー」の威力を計算する
     *
     * @param hp    個体値(HP)
     * @param atk   個体値(こうげき)
     * @param def   個体値(ぼうぎょ)
     * @param agi   個体値(すばやさ)
     * @param spatk 個体値(とくこう)
     * @param spdef 個体値(とくぼう)
     */
    //-----------------------------------------------------------------------------------
    u32 CalcTool::CalcMezamerupawaaPower( u8 hp, u8 atk, u8 def, u8 agi, u8 spatk, u8 spdef )
    {
      u32 val = 0;
      if( (hp    % 4) > 1) { val += 1; }
      if( (atk   % 4) > 1) { val += 2; }
      if( (def   % 4) > 1) { val += 4; }
      if( (agi   % 4) > 1) { val += 8; }
      if( (spatk % 4) > 1) { val += 16; }
      if( (spdef % 4) > 1) { val += 32; }

      u32 pow = 30 + (val * 40 / 63); // valは最大63
      return pow;
    }




#include "taste.cdat"
    //-----------------------------------------------------------------------------------
    /**
     * @brief 味の好みを調べる
     *
     * @param seikaku  ポケモンの性格
     * @param taste    味
     *
     * @retval TASTE_JUDGE_LIKE     好きな場合
     * @retval TASTE_JUDGE_NORMAL   普通の場合
     * @retval TASTE_JUDGE_DISLIKE  嫌いな場合
     */
    //-----------------------------------------------------------------------------------
    TasteJudge CalcTool::JudgeTaste( Seikaku seikaku, Taste taste )
    {
      return desired_taste_tbl[ seikaku ][ taste ];
    }




#include "rotom_waza.cdat"
    //-----------------------------------------------------------------------------------
    /**
     * @brief ロトムの専用技を取得する
     *
     * @param formno  ロトムのフォルムNo.
     *
     * @return 指定したフォルムに対応するロトムの専用技No.
     */
    //-----------------------------------------------------------------------------------
    WazaNo CalcTool::GetRotomuWazaNo( FormNo formno )
    {
      if( FORMNO_ROTOMU_MAX <= formno ) {
        GFL_ASSERT(0); //@check iwasawa 呼び出し側で回避すべき
        return WAZANO_NULL;
      }
      return rotom_waza_tbl[ formno ];
    }




    //-----------------------------------------------------------------------------------
    /**
     * @brief 「アルセウス」に指定したアイテムを持たせた場合のタイプを求める
     *
     * @param itemno 持ち物
     *
     * @return ポケモンのタイプ
     */
    //-----------------------------------------------------------------------------------
    PokeType CalcTool::GetAruseusuType( u32 itemno )
    {
      static const struct 
      {
        u16      item;
        PokeType type;
      }
      ITEM_TYPE_TABLE[] = 
      {
        { ITEM_HINOTAMAPUREETO,   POKETYPE_HONOO   },
        { ITEM_SIZUKUPUREETO,     POKETYPE_MIZU    },
        { ITEM_IKAZUTIPUREETO,    POKETYPE_DENKI   },
        { ITEM_MIDORINOPUREETO,   POKETYPE_KUSA    },
        { ITEM_TURARANOPUREETO,   POKETYPE_KOORI   },
        { ITEM_KOBUSINOPUREETO,   POKETYPE_KAKUTOU },
        { ITEM_MOUDOKUPUREETO,    POKETYPE_DOKU    },
        { ITEM_DAITINOPUREETO,    POKETYPE_JIMEN   },
        { ITEM_AOZORAPUREETO,     POKETYPE_HIKOU   },
        { ITEM_HUSIGINOPUREETO,   POKETYPE_ESPER   },
        { ITEM_TAMAMUSIPUREETO,   POKETYPE_MUSHI   },
        { ITEM_GANSEKIPUREETO,    POKETYPE_IWA     },
        { ITEM_MONONOKEPUREETO,   POKETYPE_GHOST   },
        { ITEM_RYUUNOPUREETO,     POKETYPE_DRAGON  },
        { ITEM_KOWAMOTEPUREETO,   POKETYPE_AKU     },
        { ITEM_KOUTETUPUREETO,    POKETYPE_HAGANE  },
        { ITEM_SEIREIPUREETO,     POKETYPE_FAIRY   }, 
        { ITEM_HONOOBIIZU_1,        POKETYPE_HONOO   },
        { ITEM_MIZUBIIZU_1,         POKETYPE_MIZU    },
        { ITEM_DENKIBIIZU_1,        POKETYPE_DENKI   },
        { ITEM_KUSABIIZU_1,         POKETYPE_KUSA    },
        { ITEM_KOORIBIIZU_1,        POKETYPE_KOORI   },
        { ITEM_KAKUTOUBIIZU_1,      POKETYPE_KAKUTOU },
        { ITEM_DOKUBIIZU_1,         POKETYPE_DOKU    },
        { ITEM_ZIMENBIIZU_1,        POKETYPE_JIMEN   },
        { ITEM_HIKOUBIIZU_1,        POKETYPE_HIKOU   },
        { ITEM_ESUPAABIIZU_1,       POKETYPE_ESPER   },
        { ITEM_MUSIBIIZU_1,         POKETYPE_MUSHI   },
        { ITEM_IWABIIZU_1,          POKETYPE_IWA     },
        { ITEM_GOOSUTOBIIZU_1,      POKETYPE_GHOST   },
        { ITEM_DORAGONBIIZU_1,      POKETYPE_DRAGON  },
        { ITEM_AKUBIIZU_1,          POKETYPE_AKU     },
        { ITEM_HAGANEBIIZU_1,       POKETYPE_HAGANE  },
        { ITEM_FEARIIBIIZU_1,       POKETYPE_FAIRY   },
      };

      for( u32 i=0; i<GFL_NELEMS(ITEM_TYPE_TABLE); ++i )
      {
        if( ITEM_TYPE_TABLE[i].item == itemno ) {
          return ITEM_TYPE_TABLE[i].type;
        }
      }

      return POKETYPE_NORMAL;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 「グリプス２」に指定したアイテムを持たせた場合のタイプを求める
     *
     * @param itemno 持ち物
     *
     * @return ポケモンのタイプ
     */
    //-----------------------------------------------------------------------------------
    PokeType CalcTool::GetGuripusu2Type( u32 itemno )
    {
      static const struct 
      {
        u16      item;
        PokeType type;
      }
      ITEM_TYPE_TABLE[] = 
      {
        { ITEM_FAIYAAMEMORI,    POKETYPE_HONOO   },
        { ITEM_UOOTAAMEMORI,    POKETYPE_MIZU    },
        { ITEM_EREKUTOROMEMORI, POKETYPE_DENKI   },
        { ITEM_GURASUMEMORI,    POKETYPE_KUSA    },
        { ITEM_AISUMEMORI,      POKETYPE_KOORI   },
        { ITEM_FAITOMEMORI,     POKETYPE_KAKUTOU },
        { ITEM_POIZUNMEMORI,    POKETYPE_DOKU    },
        { ITEM_GURAUNDOMEMORI,  POKETYPE_JIMEN   },
        { ITEM_HURAINGUMEMORI,  POKETYPE_HIKOU   },
        { ITEM_SAIKIKKUMEMORI,  POKETYPE_ESPER   },
        { ITEM_BAGUMEMORI,      POKETYPE_MUSHI   },
        { ITEM_ROKKUMEMORI,     POKETYPE_IWA     },
        { ITEM_GOOSUTOMEMORI,   POKETYPE_GHOST   },
        { ITEM_DORAGONMEMORI,   POKETYPE_DRAGON  },
        { ITEM_DAAKUMEMORI,     POKETYPE_AKU     },
        { ITEM_SUTIIRUMEMORI,   POKETYPE_HAGANE  },
        { ITEM_FEARIIMEMORI,    POKETYPE_FAIRY   }, 
      };

      for( u32 i=0; i<GFL_NELEMS(ITEM_TYPE_TABLE); ++i )
      {
        if( ITEM_TYPE_TABLE[i].item == itemno ) {
          return ITEM_TYPE_TABLE[i].type;
        }
      }

      return POKETYPE_NORMAL;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief
     * @param[in]   monsno
     * @param[in]   hold_itemno
     * @param[out]  formno
     */
    //-----------------------------------------------------------------------------------
    bool CalcTool::DecideFormNoFromHoldItem( MonsNo monsno, u32 hold_itemno, FormNo* formno )
    {
      *formno = 0; //デフォルト値はノーマルフォルム
      switch ( monsno )
      {
      case MONSNO_ARUSEUSU:
        *formno = static_cast<FormNo>(GetAruseusuType( hold_itemno ) ); // タイプとフォルム番号は一致
        return true;
      
      case MONSNO_GURIPUSU2:
        *formno = static_cast<FormNo>(GetGuripusu2Type( hold_itemno ) ); // タイプとフォルム番号は一致
        return true;

      case MONSNO_GENOSEKUTO:
        switch ( hold_itemno )
        {
        case ITEM_AKUAKASETTO:
          *formno = FORMNO_GENOSEKUTO_AQUA;
          break;
        case ITEM_INAZUMAKASETTO:
          *formno = FORMNO_GENOSEKUTO_INAZUMA;
          break;
        case ITEM_BUREIZUKASETTO:
          *formno = FORMNO_GENOSEKUTO_BLAZE;
          break;
        case ITEM_HURIIZUKASETTO:
          *formno = FORMNO_GENOSEKUTO_FREEZE;
          break;
				default:
          *formno = FORMNO_GENOSEKUTO_NORMAL;
        }
        return true;

      case MONSNO_GIRATHINA:
        if  ( hold_itemno == ITEM_HAKKINDAMA)
        {
          *formno = FORMNO_GIRATHINA_ORIGIN;
        }
        else
        {
          *formno = FORMNO_GIRATHINA_ANOTHER;
        }
        return true;
      }

      return false; //どうぐによるフォルムチェンジはない
    }

  } // namespace pokepara
} // namespace pml 
