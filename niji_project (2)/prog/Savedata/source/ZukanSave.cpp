//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		sv_zukan.cpp
 *	@brief  図鑑セーブデータ
 *	@author	Toru=Nagihashi
 *	@date		2012.05.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <pml/include/personal/pml_PersonalData.h>
#include "Savedata/include/ZukanSave.h"
#include <System/include/PokemonVersion.h>

GFL_NAMESPACE_BEGIN(Savedata)


//=============================================================================
//@brief ソース内定義
//=============================================================================
// フォルムテーブル  // monsno_def.hのFORMNO_xxxxを参考に作成(フォルム0を含まないときの合計はOTHER_FORM_MAXと同じになるはず)
static const struct
{
  u16 monsno; //対応ポケモン
  u16 form_max; //フォルム最大
}FormTable[] =
{




  {    MONSNO_HUSIGIBANA  ,  FORMNO_HUSIGIBANA_MAX    }    , 
  {    MONSNO_RIZAADON    ,  FORMNO_RIZAADON_MAX      }    , 
  {    MONSNO_KAMEKKUSU   ,  FORMNO_KAMEKKUSU_MAX     }    ,
  {    MONSNO_SUPIAA      ,  FORMNO_SUPIAA_MAX        }    , 
  {    MONSNO_PIZYOTTO    ,  FORMNO_PIZYOTTO_MAX      }    , 
  {    MONSNO_KORATTA     ,  FORMNO_KORATTA_MAX       }    ,
  {    MONSNO_RATTA       ,  FORMNO_RATTA_MAX         }    ,
  {    MONSNO_PIKATYUU    ,  FORMNO_PIKATYUU_MAX      }    ,
  {    MONSNO_RAITYUU     ,  FORMNO_RAITYUU_MAX       }    ,
  {    MONSNO_SANDO       ,  FORMNO_SANDO_MAX         }    ,
  {    MONSNO_SANDOPAN    ,  FORMNO_SANDOPAN_MAX      }    , 
  {    MONSNO_ROKON       ,  FORMNO_ROKON_MAX         }    ,
  {    MONSNO_KYUUKON     ,  FORMNO_KYUUKON_MAX       }    ,
  {    MONSNO_DHIGUDA     ,  FORMNO_DHIGUDA_MAX       }    ,
  {    MONSNO_DAGUTORIO   ,  FORMNO_DAGUTORIO_MAX     }    ,
  {    MONSNO_NYAASU      ,  FORMNO_NYAASU_MAX        }    , 
  {    MONSNO_PERUSIAN    ,  FORMNO_PERUSIAN_MAX      }    , 
  {    MONSNO_HUUDHIN     ,  FORMNO_HUUDHIN_MAX       }    ,
  {    MONSNO_ISITUBUTE   ,  FORMNO_ISITUBUTE_MAX     }    ,
  {    MONSNO_GOROON      ,  FORMNO_GOROON_MAX        }    , 
  {    MONSNO_GOROONYA    ,  FORMNO_GOROONYA_MAX      }    , 
  {    MONSNO_YADORAN     ,  FORMNO_YADORAN_MAX       }    ,
  {    MONSNO_BETOBETAA   ,  FORMNO_BETOBETAA_MAX     }    ,
  {    MONSNO_BETOBETON   ,  FORMNO_BETOBETON_MAX     }    ,
  {    MONSNO_GENGAA      ,  FORMNO_GENGAA_MAX        }    , 
  {    MONSNO_NASSII      ,  FORMNO_NASSII_MAX        }    , 
  {    MONSNO_GARAGARA    ,  FORMNO_GARAGARA_MAX      }    , 
  {    MONSNO_GARUURA     ,  FORMNO_GARUURA_MAX       }    ,
  {    MONSNO_KAIROSU     ,  FORMNO_KAIROSU_MAX       }    ,
  {    MONSNO_GYARADOSU   ,  FORMNO_GYARADOSU_MAX     }    ,
  {    MONSNO_PUTERA      ,  FORMNO_PUTERA_MAX        }    , 
  {    MONSNO_MYUUTUU     ,  FORMNO_MYUUTUU_MAX       }    ,
  {    MONSNO_DENRYUU     ,  FORMNO_DENRYUU_MAX       }    ,
  {    MONSNO_ANNOON      ,  FORMNO_ANNOON_MAX        }    , 
  {    MONSNO_HAGANEERU   ,  FORMNO_HAGANEERU_MAX     }    ,
  {    MONSNO_HASSAMU     ,  FORMNO_HASSAMU_MAX       }    ,
  {    MONSNO_HERAKUROSU  ,  FORMNO_HERAKUROSU_MAX    }    , 
  {    MONSNO_HERUGAA     ,  FORMNO_HERUGAA_MAX       }    ,
  {    MONSNO_BANGIRASU   ,  FORMNO_BANGIRASU_MAX     }    ,
  {    MONSNO_ZYUKAIN     ,  FORMNO_ZYUKAIN_MAX       }    ,
  {    MONSNO_BASYAAMO    ,  FORMNO_BASYAAMO_MAX      }    , 
  {    MONSNO_RAGURAAZI   ,  FORMNO_RAGURAAZI_MAX     }    ,
  {    MONSNO_SAANAITO    ,  FORMNO_SAANAITO_MAX      }    , 
  {    MONSNO_YAMIRAMI    ,  FORMNO_YAMIRAMI_MAX      }    , 
  {    MONSNO_KUTIITO     ,  FORMNO_KUTIITO_MAX       }    ,
  {    MONSNO_BOSUGODORA  ,  FORMNO_BOSUGODORA_MAX    }    , 
  {    MONSNO_TYAAREMU    ,  FORMNO_TYAAREMU_MAX      }    , 
  {    MONSNO_RAIBORUTO   ,  FORMNO_RAIBORUTO_MAX     }    ,
  {    MONSNO_SAMEHADAA   ,  FORMNO_SAMEHADAA_MAX     }    ,
  {    MONSNO_BAKUUDA     ,  FORMNO_BAKUUDA_MAX       }    ,
  {    MONSNO_TIRUTARISU  ,  FORMNO_TIRUTARISU_MAX    }    , 
  {    MONSNO_POWARUN     ,  FORMNO_POWARUN_MAX       }    ,
  {    MONSNO_ZYUPETTA    ,  FORMNO_ZYUPETTA_MAX      }    , 
  {    MONSNO_ABUSORU     ,  FORMNO_ABUSORU_MAX       }    ,
  {    MONSNO_ONIGOORI    ,  FORMNO_ONIGOORI_MAX      }    , 
  {    MONSNO_BOOMANDA    ,  FORMNO_BOOMANDA_MAX      }    , 
  {    MONSNO_METAGUROSU  ,  FORMNO_METAGUROSU_MAX    }    , 
  {    MONSNO_RATHIASU    ,  FORMNO_RATHIASU_MAX      }    , 
  {    MONSNO_RATHIOSU    ,  FORMNO_RATHIOSU_MAX      }    , 
  {    MONSNO_KAIOOGA     ,  FORMNO_KAIOOGA_MAX       }    ,
  {    MONSNO_GURAADON    ,  FORMNO_GURAADON_MAX      }    , 
  {    MONSNO_REKKUUZA    ,  FORMNO_REKKUUZA_MAX      }    , 
  {    MONSNO_DEOKISISU   ,  FORMNO_DEOKISISU_MAX     }    ,
  {    MONSNO_MINOMUTTI   ,  FORMNO_MINOMUTTI_MAX     }    ,
  {    MONSNO_MINOMADAMU  ,  FORMNO_MINOMADAMU_MAX    }    , 
  {    MONSNO_GAAMEIRU    ,  FORMNO_GAAMEIRU_MAX      }    , 
  {    MONSNO_THERIMU     ,  FORMNO_THERIMU_MAX       }    ,
  {    MONSNO_KARANAKUSI  ,  FORMNO_KARANAKUSI_MAX    }    , 
  {    MONSNO_TORITODON   ,  FORMNO_TORITODON_MAX     }    ,
  {    MONSNO_MIMIROPPU   ,  FORMNO_MIMIROPPU_MAX     }    ,
  {    MONSNO_GABURIASU   ,  FORMNO_GABURIASU_MAX     }    ,
  {    MONSNO_RUKARIO     ,  FORMNO_RUKARIO_MAX       }    ,
  {    MONSNO_YUKINOOO    ,  FORMNO_YUKINOOO_MAX      }    , 
  {    MONSNO_ERUREIDO    ,  FORMNO_ERUREIDO_MAX      }    , 
  {    MONSNO_ROTOMU      ,  FORMNO_ROTOMU_MAX        }    , 
  {    MONSNO_GIRATHINA   ,  FORMNO_GIRATHINA_MAX     }    ,
  {    MONSNO_SHEIMI      ,  FORMNO_SHEIMI_MAX        }    , 
  {    MONSNO_ARUSEUSU    ,  FORMNO_ARUSEUSU_MAX      }    , 
  {    MONSNO_TABUNNE     ,  FORMNO_TABUNNE_MAX       }    ,
  {    MONSNO_BASURAO     ,  FORMNO_BASURAO_MAX       }    ,
  {    MONSNO_HIHIDARUMA  ,  FORMNO_HIHIDARUMA_MAX    }    , 
  {    MONSNO_SIKIZIKA    ,  FORMNO_SIKIZIKA_MAX      }    , 
  {    MONSNO_MEBUKIZIKA  ,  FORMNO_MEBUKIZIKA_MAX    }    , 
  {    MONSNO_TORUNEROSU  ,  FORMNO_TORUNEROSU_MAX    }    , 
  {    MONSNO_BORUTOROSU  ,  FORMNO_BORUTOROSU_MAX    }    , 
  {    MONSNO_RANDOROSU   ,  FORMNO_RANDOROSU_MAX     }    ,
  {    MONSNO_KYUREMU     ,  FORMNO_KYUREMU_MAX       }    ,
  {    MONSNO_KERUDHIO    ,  FORMNO_KERUDHIO_MAX      }    , 
  {    MONSNO_MEROETTA    ,  FORMNO_MEROETTA_MAX      }    , 
  {    MONSNO_GENOSEKUTO  ,  FORMNO_GENOSEKUTO_MAX    }    , 
  {    MONSNO_GEKKOUGA    ,  FORMNO_GEKKOUGA_MAX      }    , 
  {    MONSNO_KOHUKIMUSI  ,  FORMNO_KOHUKIMUSI_MAX    }    , 
  {    MONSNO_KOHUURAI    ,  FORMNO_KOHUURAI_MAX      }    , 
  {    MONSNO_BIBIYON     ,  FORMNO_BIBIYON_MAX       }    ,
  {    MONSNO_HURABEBE    ,  FORMNO_HURABEBE_MAX      }    , 
  {    MONSNO_HURAETTE    ,  FORMNO_HURAETTE_MAX      }    , 
  {    MONSNO_HURAAJESU   ,  FORMNO_HURAAJESU_MAX     }    ,
  {    MONSNO_TORIMIAN    ,  FORMNO_TORIMIAN_MAX      }    , 
  {    MONSNO_NYAONIKUSU  ,  FORMNO_NYAONIKUSU_MAX    }    , 
  {    MONSNO_GIRUGARUDO  ,  FORMNO_GIRUGARUDO_MAX    }    , 
  {    MONSNO_BAKETTYA    ,  FORMNO_BAKETTYA_MAX      }    , 
  {    MONSNO_PANPUZIN    ,  FORMNO_PANPUZIN_MAX      }    , 
  {    MONSNO_ZERUNEASU   ,  FORMNO_ZERUNEASU_MAX     }    ,
  {    MONSNO_ZIGARUDE    ,  FORMNO_ZIGARUDE_MAX      }    , 
  {    MONSNO_DHIANSII    ,  FORMNO_DHIANSII_MAX      }    , 
  {    MONSNO_HUUPA       ,  FORMNO_HUUPA_MAX         }    ,
  {    MONSNO_MANGUUSU2   ,  FORMNO_MANGUUSU2_MAX     }    ,
  {    MONSNO_KUWAGATA3   ,  FORMNO_KUWAGATA3_MAX     }    ,
  {    MONSNO_MITUSUI     ,  FORMNO_MITUSUI_MAX       }    ,
  {    MONSNO_ABU2        ,  FORMNO_ABU2_MAX          }    ,  //  momiji
  {    MONSNO_OOKAMI1     ,  FORMNO_OOKAMI1_MAX       }    ,  //  momiji
  {    MONSNO_OOKAMI2     ,  FORMNO_OOKAMI2_MAX       }    ,
  {    MONSNO_YOWASI      ,  FORMNO_YOWASI_MAX        }    ,
  {    MONSNO_MIZUGUMO2   ,  FORMNO_MIZUGUMO2_MAX     }    ,  //  momiji
  {    MONSNO_HANAKAMA2   ,  FORMNO_HANAKAMA2_MAX     }    ,
  {    MONSNO_DOKUTOKAGE2 ,  FORMNO_DOKUTOKAGE2_MAX   }    ,
  {    MONSNO_GURIPUSU2   ,  FORMNO_GURIPUSU2_MAX     }    ,
  {    MONSNO_NAGAREBOSI  ,  FORMNO_NAGAREBOSI_MAX    }    ,
  {    MONSNO_NIZITYUU    ,  FORMNO_NIZITYUU_MAX      }    ,  //  momiji
  {    MONSNO_HORAA       ,  FORMNO_HORAA_MAX         }    ,
  {    MONSNO_UROKO3      ,  FORMNO_UROKO3_MAX        }    ,
  {    MONSNO_PURIZUMU    ,  FORMNO_PURIZUMU_MAX      }    ,  //  momiji
  {    MONSNO_MABOROSI16  ,  FORMNO_MABOROSI16_MAX    }    ,
/*
    MONSNO_BIISUTOd1,     //  momiji  //  no form
    MONSNO_BIISUTOd2,     //  momiji  //  no form
    MONSNO_BIISUTOs,      //  momiji  //  no form
    MONSNO_BIISUTOh,      //  momiji  //  no form
    MONSNO_MABOROSI18,    //  momiji  //  no form
*/
};

// メガシンカするポケモンのテーブル  // monsno_def.hのFORMNO_xxxx_MEGAを参考に作成。【レックウザを入れ忘れないように注意！】
u16 MegaTable[] =  // monsno  //対応ポケモン
{
  MONSNO_HUSIGIBANA  ,
  MONSNO_RIZAADON    ,
  MONSNO_KAMEKKUSU   ,
  MONSNO_SUPIAA      ,
  MONSNO_PIZYOTTO    ,
  MONSNO_HUUDHIN     ,
  MONSNO_YADORAN     ,
  MONSNO_GENGAA      ,
  MONSNO_GARUURA     ,
  MONSNO_KAIROSU     ,
  MONSNO_GYARADOSU   ,
  MONSNO_PUTERA      ,
  MONSNO_MYUUTUU     ,
  MONSNO_DENRYUU     ,
  MONSNO_HAGANEERU   ,
  MONSNO_HASSAMU     ,
  MONSNO_HERAKUROSU  ,
  MONSNO_HERUGAA     ,
  MONSNO_BANGIRASU   ,
  MONSNO_ZYUKAIN     ,
  MONSNO_BASYAAMO    ,
  MONSNO_RAGURAAZI   ,
  MONSNO_SAANAITO    ,
  MONSNO_YAMIRAMI    ,
  MONSNO_KUTIITO     ,
  MONSNO_BOSUGODORA  ,
  MONSNO_TYAAREMU    ,
  MONSNO_RAIBORUTO   ,
  MONSNO_SAMEHADAA   ,
  MONSNO_BAKUUDA     ,
  MONSNO_TIRUTARISU  ,
  MONSNO_ZYUPETTA    ,
  MONSNO_ABUSORU     ,
  MONSNO_ONIGOORI    ,
  MONSNO_BOOMANDA    ,
  MONSNO_METAGUROSU  ,
  MONSNO_RATHIASU    ,
  MONSNO_RATHIOSU    ,
  MONSNO_REKKUUZA    ,  // @attention 名前がMEGAではないので入れ忘れないように注意！  // 将来への注意喚起のためにtodoとして残しておきたい。
  MONSNO_MIMIROPPU   ,
  MONSNO_GABURIASU   ,
  MONSNO_RUKARIO     ,
  MONSNO_YUKINOOO    ,
  MONSNO_ERUREIDO    ,
  MONSNO_TABUNNE     ,
  MONSNO_DHIANSII    ,
};


/**
 * @brief 全国図鑑でチェックしないポケモン一覧
 */
static const u16 ZENKOKU_NOT_CHECK_MONSNO[] = {
  // nijiには全国図鑑がないので、sangoから変更していない。
  MONSNO_MYUU,				// 151 ミュウ
  MONSNO_SEREBHI,			// 251 セレビィ
  MONSNO_ZIRAATI,			// 385 ジラーチ
  MONSNO_FIONE,				// 489 フィオネ
  MONSNO_MANAFI,			// 490 マナフィ
  MONSNO_DAAKURAI,		// 491 ダークライ
  MONSNO_SHEIMI,			// 492 シェイミ
  MONSNO_ARUSEUSU,		// 493 アルセウス
  MONSNO_BIKUTHINI,		// 494 ビクティニ
  MONSNO_KERUDHIO,		// 646 ケルディオ
  MONSNO_MEROETTA,		// 647 メロエッタ
  MONSNO_GENOSEKUTO,	// 648 ゲノセクト
  MONSNO_DHIANSII,    // 719 ディアンシー
  MONSNO_HUUPA,       // 720 フーパ
  MONSNO_BORUKENION,  // 721 ボルケニオン
};

/**
 * @brief 地方図鑑でチェックしないポケモン一覧
 */
static const u16 LOCAL_A_NOT_CHECK_MONSNO[] = {
  // アローラ図鑑用
  MONSNO_MABOROSI16  ,  // 801 マギアナ
  MONSNO_MABOROSI17  ,  // 802 マーシャドー
  MONSNO_MABOROSI18  ,  // 807 [momiji new]
};

const int ZukanData::ZENKOKU_COMP_NOPOKE_NUM = GFL_NELEMS(ZENKOKU_NOT_CHECK_MONSNO);  // 不必要なポケモン数
const int ZukanData::ZENKOKU_COMP_NUM  = ZENKOKU_MONSMAX - ZukanData::ZENKOKU_COMP_NOPOKE_NUM;

//=============================================================================
//@brief コンストラクタ・デストラクタ
//=============================================================================
/**
 * @brief コンストラクタ
 */
ZukanData::ZukanData()
{
#if PM_DEBUG
  {
    u32 cntZero = 0;  // フォルム0を含む
    u32 cntOne  = 0;  // フォルム0を含まない
    for( int i = 0; i < GFL_NELEMS(FormTable); ++ i )
    {
      cntZero += FormTable[i].form_max;
      cntOne  += FormTable[i].form_max -1;
    }
    GFL_PRINT( "zukan save: form max(0)=%d, form max(1)=%d, elem num=%d, ZukanCoreData=%d, OTHER_FORM_MAX=%d\n", cntZero, cntOne, GFL_NELEMS(FormTable), sizeof(ZukanCoreData), OTHER_FORM_MAX );
  }
#endif //PM_DEBUG

  this->Clear(NULL);
}

/**
 * @brief デストラクタ
 */
ZukanData::~ZukanData()
{
}

//=============================================================================
//@brief 取得・操作
//=============================================================================

////////////////////////////////
//
// オープン状態
//
////////////////////////////////

/**
 * @brief		全国図鑑モードをセット
 * @param		is_flag 全国モードかどうか
 * @return	none
 */
void ZukanData::SetZenkokuZukanFlag( bool is_flag )
{
  this->IncorrectZukan();
  m_data.zenkoku_flag = is_flag;
}

/**
 * @brief		全国ずかんモードかどうか？の問い合わせ
 * @retval	"true = 全国図鑑可"
 * @retval	"false = それ以外"
 */
bool ZukanData::GetZenkokuZukanFlag( void ) const
{
  this->IncorrectZukan();
  return m_data.zenkoku_flag;
}

/**
 * @brief 各モードのオープン状態を設定
 *
 * @param mode    モード
 * @param is_flag trueでオープン  falseでオープンしていない
 * 
 * @note modeにMODE_ZENKOKUをいれた場合は　SetZenkokuZukanFlag　と同じ
 */
void ZukanData::SetZukanOpenFlag( Mode mode, bool is_flag )
{
  this->IncorrectZukan();
  switch( mode )
  {
  default:
    GFL_ASSERT(0);  //@check
    break;
  case MODE_LOCAL_A: ///<地方  エリアA
    m_data.area_a_flag = is_flag;
    break;
  case MODE_LOCAL_B: ///<地方  エリアB
    m_data.area_b_flag = is_flag;
    break;
  case MODE_LOCAL_C: ///<地方  エリアC
    m_data.area_c_flag = is_flag;
    break;
  case MODE_LOCAL_D: ///<地方  エリアD
    m_data.area_d_flag = is_flag;
    break;
  case MODE_LOCAL_E: ///<地方  エリアE
    m_data.area_e_flag = is_flag;
    break;
  case MODE_ZENKOKU: ///<全国
    this->SetZenkokuZukanFlag( is_flag );
    break;
  }
}

/**
 * @brief 各モードのオープン状態を取得
 *
 * @param mode  モード
 *
 * @return trueでオープン falseオープンしていない
 * 
 * @note modeにMODE_ZENKOKUをいれた場合は　GetZenkokuZukanFlag　と同じ
 */
bool ZukanData::GetZukanOpenFlag( Mode mode ) const
{
  this->IncorrectZukan();
  switch( mode )
  {
  case MODE_LOCAL_A: ///<地方  エリアA
    return m_data.area_a_flag;
  case MODE_LOCAL_B: ///<地方  エリアB
    return m_data.area_b_flag;
  case MODE_LOCAL_C: ///<地方  エリアC
    return m_data.area_c_flag;
  case MODE_LOCAL_D: ///<地方  エリアD
    return m_data.area_d_flag;
  case MODE_LOCAL_E: ///<地方  エリアE
    return m_data.area_e_flag;
  case MODE_ZENKOKU: ///<全国
    return this->GetZenkokuZukanFlag();
  }
  
  GFL_ASSERT(0);  //@check
  return false;
}

/**
 * @brief 図鑑OSのバージョンを設定
 *
 * @param バージョン
 */
void ZukanData::SetZukanOSVersion(u32 ver)
{
  this->IncorrectZukan();
  m_data.os_ver = ver;
}

/**
 * @brief 図鑑OSのバージョンを取得
 *
 * @return バージョン
 */
u32 ZukanData::GetZukanOSVersion(void) const
{
  this->IncorrectZukan();
  return m_data.os_ver;
}


////////////////////////////////
//
// モード
//
////////////////////////////////

/**
 * @brief		閲覧中の図鑑モードを設定
 * @param		mode	図鑑モード 
 */
void ZukanData::SetZukanMode( ZukanData::Mode mode )
{
  this->IncorrectZukan();
  m_data.zukan_mode = mode;
}

/**
 * @brief		閲覧中の図鑑モードを取得
 * @retval	図鑑モード
 */
ZukanData::Mode ZukanData::GetZukanMode( void ) const
{
  this->IncorrectZukan();
  return static_cast<Mode>(m_data.zukan_mode);
}

/**
 * @brief モードから地方エリアを取得
 *
 * @return 
 */
PokeTool::ExtendData::LocalArea ZukanData::GetLocalAreaMode( void ) const
{
  switch ( this->GetZukanMode() )
  {
  case MODE_LOCAL_A: ///<地方  エリアA
    return PokeTool::ExtendData::LOCAL_AREA_A;
  case MODE_LOCAL_B: ///<地方  エリアB
    return PokeTool::ExtendData::LOCAL_AREA_B;
  case MODE_LOCAL_C: ///<地方  エリアC
    return PokeTool::ExtendData::LOCAL_AREA_C;
  case MODE_LOCAL_D: ///<地方  エリアD
    return PokeTool::ExtendData::LOCAL_AREA_D;
  case MODE_LOCAL_E: ///<地方  エリアE
    return PokeTool::ExtendData::LOCAL_AREA_E;
  case MODE_ZENKOKU: ///<全国
    return PokeTool::ExtendData::LOCAL_AREA_NONE;
  }
  
  GFL_ASSERT(0);  //@check
  return PokeTool::ExtendData::LOCAL_AREA_NONE;
}


////////////////////////////////
//
// 閲覧中
//
////////////////////////////////

/**
 * @brief		閲覧中のポケモン番号を設定
 * @param		mons	ポケモン番号
 */
void ZukanData::SetDefaultMons( MonsNo mons )
{
  this->IncorrectZukan();
	m_data.defaultMonsNo = mons;
}


/**
 * @brief		閲覧中のポケモン番号を取得
 * @return	ポケモン番号
 */
MonsNo ZukanData::GetDefaultMons( void ) const
{
  this->IncorrectZukan();
	return static_cast<MonsNo>(m_data.defaultMonsNo);
}

/**
 * @brief		閲覧中データ設定
 * @param		mons		ポケモン番号
 * @param		sex			性別
 * @param		rare		レアフラグ true = RARE
 * @param		form		フォルム
 */
void ZukanData::SetDrawData( MonsNo mons, pml::Sex sex, bool rare, FormNo form )
{
  if( this->IncorrectMonsNo(mons) ) return;

  this->GetFormNoWhenSetDrawData( mons, form, &form );

  // ポワルンのレアフラグ特別処理【制限】
  if( mons == MONSNO_POWARUN )
  {
    // ポワルンは通常フォルムにはレアカラーが存在するが、
    // それ以外のフォルムにはレアカラーが存在しない。
    if( form != FORMNO_POWARUN_NORMAL )
    {
      rare = false;
    }
  }

	{
    // まずはmonsのdraw_flagを全て落とす
    u32 formMax = this->GetFormMax(mons);
    for(u32 j=0; j<formMax; ++j)
    {
      u16 index = this->GetMonsFormIndex(mons, j);
      if(index == MONS_FORM_INDEX_INVALID)
      {
        continue;
      }

      for(u32 i=0; i<SEE_FLAG_MAX; ++i)
      {
        this->ResetBit( reinterpret_cast<u8 *>(m_data.draw_flag[i]), index, MONS_FORM_ARRAY_LEN*4 );
      }
    }

    // 閲覧中フォルムデータ
    u16 index = this->GetMonsFormIndex(mons, form);
    if(index == MONS_FORM_INDEX_INVALID)
    {
      // 不正な値のときはフォルム0としておく
      index = mons -1;
    }

    // 次に閲覧中のところだけ立てる
    if( sex == pml::SEX_MALE ){
			if( rare != false ){
				this->SetBit( reinterpret_cast<u8 *>(m_data.draw_flag[SEE_FLAG_M_RARE]), index, MONS_FORM_ARRAY_LEN*4 );
			}else{
				this->SetBit( reinterpret_cast<u8 *>(m_data.draw_flag[SEE_FLAG_MALE]), index, MONS_FORM_ARRAY_LEN*4 );
			}
		}else if( sex == pml::SEX_FEMALE ){
			if( rare != false ){
				this->SetBit( reinterpret_cast<u8 *>(m_data.draw_flag[SEE_FLAG_F_RARE]), index, MONS_FORM_ARRAY_LEN*4 );
			}else{
				this->SetBit( reinterpret_cast<u8 *>(m_data.draw_flag[SEE_FLAG_FEMALE]), index, MONS_FORM_ARRAY_LEN*4 );
			}
		}else{
			// 性別がない場合は♂のフラグを立てる
			if( rare != false ){
				this->SetBit( reinterpret_cast<u8 *>(m_data.draw_flag[SEE_FLAG_M_RARE]), index, MONS_FORM_ARRAY_LEN*4 );
			}else{
				this->SetBit( reinterpret_cast<u8 *>(m_data.draw_flag[SEE_FLAG_MALE]), index, MONS_FORM_ARRAY_LEN*4 );
			}
		}
  }	
}

/**
 * @brief		表示するフォルムデータを取得
 * @param		mons		ポケモン番号
 * @param		sex			性別
 * @param		rare		レアフラグ true = RARE
 * @param		form		フォルム
 */
void ZukanData::GetDrawData( MonsNo mons, pml::Sex * sex, bool * rare, FormNo * form ) const
{
  *sex = pml::SEX_MALE;
  *rare = false;
  *form = 0;
  if( this->IncorrectMonsNo(mons) ) return;

  u32 formMax = this->GetFormMax(mons);
  for(u32 j=0; j<formMax; ++j)
  {
    u16 index = this->GetMonsFormIndex(mons, j);
    if(index == MONS_FORM_INDEX_INVALID)
    {
      continue;
    }

    if( this->CheckBit( reinterpret_cast<const u8 *>(m_data.draw_flag[SEE_FLAG_MALE]), index ) ){
      *form = j;
      *sex = pml::SEX_MALE;
      *rare = false;
      break;
    }
    else if( this->CheckBit( reinterpret_cast<const u8 *>(m_data.draw_flag[SEE_FLAG_FEMALE]), index ) ){
      *form = j;
      *sex = pml::SEX_FEMALE;
      *rare = false;
      break;
    }
    else if( this->CheckBit( reinterpret_cast<const u8 *>(m_data.draw_flag[SEE_FLAG_M_RARE]), index ) ){
      *form = j;
      *sex = pml::SEX_MALE;
      *rare = true;
      break;
    }
    else if( this->CheckBit( reinterpret_cast<const u8 *>(m_data.draw_flag[SEE_FLAG_F_RARE]), index ) ){
      *form = j;
      *sex = pml::SEX_FEMALE;
      *rare = true;
      break;
    }
  }

  // 性別不明
  pml::personal::LoadPersonalData( mons, *form );

  if( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ) == pml::personal::SEX_VECTOR_UNKNOWN ) {
		*sex  = pml::SEX_UNKNOWN;
	}
}


////////////////////////////////
//
// フォルム
//
////////////////////////////////

/**
 * @brief		最大フォルム数取得
 * @param		mons		ポケモン番号
 * @return	最大フォルム数
 */
u32	ZukanData::GetFormMax( MonsNo mons ) const
{
	s32	pos = this->GetPokeFormTablePos( mons );

	if( pos != -1 ){
		return FormTable[pos].form_max;
	}
	return 1;
}


/**
 * @brief		メガフォルムもちを取得
 * @param		mons		ポケモン番号
 * @return	trueならばメガフォルム持ち
 */
bool ZukanData::IsMega( MonsNo mons ) const
{
	s32	pos = this->GetPokeMegaTablePos( mons );

	if( pos != -1 ){
    return true;
	}
	return false;
}


////////////////////////////////
//
// 捕まえたフラグ
//
////////////////////////////////

/**
 * @brief		捕獲データセット
 * @param		pp		捕獲ポケモンデータ
 */
void ZukanData::SetPokeGet( const pml::pokepara::CoreParam & param )
{
 	PokeTool::SimpleParam simple_param;

	bool fast = param.StartFastMode();

	simple_param.monsNo  = param.GetMonsNo();
  simple_param.formNo  = param.GetFormNo();
  simple_param.sex     = param.GetSex();
  simple_param.isRare  = param.IsRare();
  simple_param.isEgg   = param.IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  simple_param.perRand = param.GetPersonalRnd();
  
  u32 lang    = param.GetLangId();
  //u32 version = param.GetCassetteVersion();  // バージョン

	param.EndFastMode( fast );

	this->SetPokeGet( simple_param , lang );
}

void ZukanData::SetPokeGet( const PokeTool::SimpleParam & param, u32 lang )  // langはpml::pokepara::CoreParamからGetLangId()で取得できる値
{
  this->IncorrectZukan();

	// タマゴなら登録しない
	if( param.isEgg ){
    GFL_PRINT( "タマゴ異常：%d\n", param.monsNo );
		return;
	}
	MonsNo mons = param.monsNo;
	u32 rand    = param.perRand;
	FormNo form = param.formNo;
  pml::Sex sex  = param.sex;
  bool rare   = param.isRare;

  if( this->IncorrectMonsNo(mons) ){
    return;
  }
  
  bool getForModifyPokeRegisterShowBySetPokeGet = this->GetPokeGetFlag( mons );
  bool seeForModifyPokeRegisterShowBySetPokeGet = this->GetPokeSeeFlag( mons );

	// 見たことない場合
	//if( this->GetPokeSeeFlag( mons ) == false ){
  // 捕まえたことがない場合  // @fix NMCat[2716] フォルム違いを捕獲した時に、図鑑では最初に見たフォルムがメインで表示されている
                             //                  →初捕獲後に図鑑を開いたときに設定されている姿が、初捕獲したときの姿になるようにしておく(nijiから。sangoまでは初見の姿)
	if( this->GetPokeGetFlag( mons ) == false ){
		this->SetDrawData( mons, sex, rare, form );
	}

	// 固体乱数
	this->SetZukanRandom( mons, sex, rare, form, rand, true );  // Getフラグが立つ前に呼ぶこと！
	// 見たフラグ
	this->SetSeeBit( mons, sex, rare, form );
	// 国コードセット
	this->SetZukanTextVersionUp( mons, lang );
  
  //捕獲フラグセット
  this->SetGetBit( mons );

  // 初回のSetPokeGet時に呼んで、PokeRegisterShowを修正する
  if(!getForModifyPokeRegisterShowBySetPokeGet) this->ModifyPokeRegisterShowBySetPokeGet( mons, seeForModifyPokeRegisterShowBySetPokeGet );  // 初ゲット時にしか呼ばない
}

/**
 * @brief		捕獲チェック
 * @param		monsno	ポケモン番号
 * @retval	"true = 捕獲済み"
 * @retval	"false = それ以外"
 */
bool ZukanData::GetPokeGetFlag( MonsNo monsno ) const
{
  this->IncorrectZukan();

  if( this->IncorrectMonsNo(monsno) ) {
    return false;
  }
  
  bool  get   = this->CheckGetBit(monsno);
  
  return get;
}

/**
 * @brief  初ゲット時に出る図鑑登録画面を呼んだ(ログ分しか記録できない)(名前入力画面とかは関係ない)
 * @param  monsno  ポケモン番号
 */
void ZukanData::SetPokeRegisterShow( MonsNo monsno )
{
  this->IncorrectZukan();

  u16 l_monsNo = static_cast<u16>(monsno);
 
  // PokeGetフラグを立てたがまだ図鑑登録画面を呼んでいないリストから消す
  for(s8 i=0; i<m_notSaveData.logNumForNotPokeRegisterShow; ++i)
  {
    if( m_notSaveData.logNotPokeRegisterShowMonsNo[i] == l_monsNo )
    {
      for(s8 j=i; j<m_notSaveData.logNumForNotPokeRegisterShow-1; ++j)
      {
        m_notSaveData.logNotPokeRegisterShowMonsNo[j]  = m_notSaveData.logNotPokeRegisterShowMonsNo[j+1];
        if( ( m_notSaveData.logNotPokeRegisterShowSeeFlag & (1<<(j+1)) ) != 0 )
        {
          m_notSaveData.logNotPokeRegisterShowSeeFlag |= 1<<j;  // フラグを立てる
        }
        else
        {
          m_notSaveData.logNotPokeRegisterShowSeeFlag &= ~(1<<j);  // フラグを落とす
        }
      }
      --m_notSaveData.logNumForNotPokeRegisterShow;
      break;
    }
  }

  // 図鑑登録画面を呼んだがまだPokeGetフラグを立てていないリストに入れる
  bool get = this->GetPokeGetFlag(monsno);
  if(!get)
  {
    // 既にリストに入っているなら、何もしない
    bool inList = false;
    for(s8 i=0; i<m_notSaveData.logNumForNotPokeGet; ++i)
    {
      if( m_notSaveData.logNotPokeGetMonsNo[i] == l_monsNo )
      {
        inList = true;
        break;
      }
    }
    // まだリストに入っていないなら、リストに入れる
    if(!inList)
    {
      s8 i_start = m_notSaveData.logNumForNotPokeGet;
      ++m_notSaveData.logNumForNotPokeGet;
      if(m_notSaveData.logNumForNotPokeGet > ZukanNotSaveData::LOG_NUM_MAX)
      {
        --i_start;
        m_notSaveData.logNumForNotPokeGet = ZukanNotSaveData::LOG_NUM_MAX;
      }
      for(s8 i=i_start; i>0; --i)
      {
        m_notSaveData.logNotPokeGetMonsNo[i] = m_notSaveData.logNotPokeGetMonsNo[i-1];
      }
      m_notSaveData.logNotPokeGetMonsNo[0] = l_monsNo;
    }
  }

#if PM_DEBUG
  this->PrintNotSaveData("SetPokeRegisterShow\n");
#endif  // PM_DEBUG
}

/**
 * @brief  初ゲット時に出る図鑑登録画面を呼んでいるかチェック(名前入力画面とかは関係ない)
 * @param  monsno  ポケモン番号
 * @param[out]  monsno  seeフラグ。戻り値がfalseのときのみ有効。[1]のときはPokeGetフラグを立てたときのseeフラグ。[2]のときは現在のseeフラグ。
 * @retval true   図鑑登録画面を呼んでいる
 * @retval false  [1]PokeGetフラグが立っているのに図鑑登録画面を呼んでいない(ログ分しか正確には判定できない) or [2]PokeGetフラグが立っていない
 */
bool ZukanData::GetPokeRegisterShowFlag( MonsNo monsno, bool* seeFlag ) const
{
  this->IncorrectZukan();

  u16 l_monsNo = static_cast<u16>(monsno);

  // 図鑑登録画面を呼んでいるかチェック(ログ分しか正確には判定できない)
  for(s8 i=0; i<m_notSaveData.logNumForNotPokeGet; ++i)
  {
    if( m_notSaveData.logNotPokeGetMonsNo[i] == l_monsNo )
    {
      return true;
    }
  }

  // [1]
  // PokeGetフラグが立っているのに図鑑登録画面を呼んでいないかチェック(ログ分しか正確には判定できない)
  for(s8 i=0; i<m_notSaveData.logNumForNotPokeRegisterShow; ++i)
  {
    if( m_notSaveData.logNotPokeRegisterShowMonsNo[i] == l_monsNo )
    {
      *seeFlag = ( ( m_notSaveData.logNotPokeRegisterShowSeeFlag & (1<<i) ) != 0);
      return false;
    }
  }

  // [2]
  *seeFlag = this->GetPokeSeeFlag(monsno);
  // PokeGetフラグが立っていないかチェック
  return this->GetPokeGetFlag(monsno);
}

/**
 * @brief		現在の図鑑モードのポケモン捕獲数取得
 * @param		ポケモン番号辞書クラス
 * @return	捕獲数
 */
u16 ZukanData::GetZukanPokeGetCount( const PokeTool::ExtendData * poke_extend ) const
{
  if( this->GetZukanMode() == MODE_ZENKOKU ){
    return this->GetPokeGetCount();
  }
  return this->GetLocalPokeGetCount( this->GetLocalAreaMode(), poke_extend );
}

/**
 * @brief		ポケモン捕獲数取得（全国）
 * @return	捕獲数
 */
u16 ZukanData::GetPokeGetCount(void) const
{
  int count;

  this->IncorrectZukan();

  count= 0;
  for( int i=1; i<=MONSNO_END; i++ ){
    if( this->GetPokeGetFlag( static_cast<MonsNo>(i) ) != false ){
      count++;
    }
  }
  return count;
}

/**
 * @brief ポケモン捕獲数取得（地方エリア別）
 * @param local_area  地方エリア
 * @param poke_extend ポケモンデータ拡張クラス
 * @return 捕獲数
 */
u16 ZukanData::GetLocalPokeGetCount( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const
{
  u16 num=0;

  this->IncorrectZukan();

  num = 0;

  for( int i=1; i<=MONSNO_END; i++ ){
    if( this->GetPokeGetFlag( static_cast<MonsNo>(i) ) != false ){
      // 地方図鑑にいるかチェック
      if( poke_extend->IsExistLocalZukan(i,local_area) ){
        num++;
      }
    }
  }
  return num;
}

u16 ZukanData::GetLocalPokeGetCountStatic( PokeTool::ExtendData::LocalArea local_area ) const
{
  GFL_ASSERT( local_area == PokeTool::ExtendData::LOCAL_AREA_A );  // 開発中に気付かせるためのASSERT。

  u16 num=0;

  this->IncorrectZukan();

  num = 0;

  for( int i=1; i<=MONSNO_END; i++ ){
    if( this->GetPokeGetFlag( static_cast<MonsNo>(i) ) != false ){
      // 地方図鑑にいるかチェック
      if( PokeTool::ExtendData::IsExistLocalZukanStatic(i,local_area) ){
        num++;
      }
    }
  }
  return num;
}


////////////////////////////////
//
// 見たフラグ
//
////////////////////////////////

/**
 * @brief		見たデータセット ( SimpleParam版 )
 * @param		pp		見たポケモンのデータ
 * @note		出会った回数の追加はしていません。
 *          出会った回数の追加はIncrementPokeSeeAmountで行って下さい。
 */
void ZukanData::SetPokeSee( const PokeTool::SimpleParam & param )
{
  this->IncorrectZukan();

	// タマゴなら登録しない
	if( param.isEgg != false ){
		return;
	}

  if( this->IncorrectMonsNo(param.monsNo) ){
    return;
  }
  
	// 見たことない場合
	if( this->GetPokeSeeFlag( param.monsNo ) == false ){
		this->SetDrawData( param.monsNo, param.sex, param.isRare, param.formNo );
	}

	// 固体乱数
	this->SetZukanRandom( param.monsNo, param.sex, param.isRare, param.formNo, param.perRand, false );  // Seeフラグが立つ前に呼ぶこと！
	// 見たフラグ
	this->SetSeeBit( param.monsNo, param.sex, param.isRare, param.formNo );
}

/**
 * @brief		見たデータセット ( CoreParam版 )
 * @param		pp		見たポケモンのデータ
 * @note		出会った回数の追加はしていません。
 *          出会った回数の追加はIncrementPokeSeeAmountで行って下さい。
 */
void ZukanData::SetPokeSee( const pml::pokepara::CoreParam & param )
{
	PokeTool::SimpleParam simple_param;

	bool fast = param.StartFastMode();

	simple_param.monsNo  = param.GetMonsNo();
  simple_param.formNo  = param.GetFormNo();
  simple_param.sex     = param.GetSex();
  simple_param.isRare  = param.IsRare();
  simple_param.isEgg   = param.IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  simple_param.perRand = param.GetPersonalRnd();

	param.EndFastMode( fast );

	this->SetPokeSee( simple_param );
}

/**
 * @brief		見たチェック
 * @param		monsno	ポケモン番号
 * @retval	"true = 見た"
 * @retval	"false = それ以外"
 */
bool ZukanData::GetPokeSeeFlag( MonsNo monsno ) const
{
  this->IncorrectZukan();
  if( this->IncorrectMonsNo(monsno) ){
    return false;
  }

  u32 formMax = this->GetFormMax(monsno);
  for(u32 formNo=0; formNo<formMax; ++formNo)
  {
    if( this->CheckSeeBit( monsno, pml::SEX_MALE  , false, formNo ) ) return true;
    if( this->CheckSeeBit( monsno, pml::SEX_FEMALE, false, formNo ) ) return true;
    if( this->CheckSeeBit( monsno, pml::SEX_MALE  , true , formNo ) ) return true;
    if( this->CheckSeeBit( monsno, pml::SEX_FEMALE, true , formNo ) ) return true;
  }

  return false;
}

/**
 * @brief		見たチェック
 * @param		monsno	ポケモン番号
 * @param		form	  フォルム
 * @retval	"true = 見た"
 * @retval	"false = それ以外"
 * @note    性別とレアは何でもいいから、monsnoとformが一致するものを見ているかを返す
 */
bool ZukanData::GetPokeFormSeeFlag( MonsNo monsno, FormNo form ) const
{
  this->IncorrectZukan();
  if( this->IncorrectMonsNo(monsno) ){
    return false;
  }

  {
    if( this->CheckSeeBit( monsno, pml::SEX_MALE  , false, form ) ) return true;
    if( this->CheckSeeBit( monsno, pml::SEX_FEMALE, false, form ) ) return true;
    if( this->CheckSeeBit( monsno, pml::SEX_MALE  , true , form ) ) return true;
    if( this->CheckSeeBit( monsno, pml::SEX_FEMALE, true , form ) ) return true;
  }

  return false;
}

/**
 * @brief		現在の図鑑モードのポケモンを見た数取得
 * @param		ポケモン番号辞書クラス
 * @return	見た数
 */
u16 ZukanData::GetZukanPokeSeeCount( const PokeTool::ExtendData * poke_extend ) const
{
  if( this->GetZukanMode() == MODE_ZENKOKU ){
    return this->GetPokeSeeCount();
  }
  else
  {
    return this->GetLocalPokeSeeCount( poke_extend, this->GetLocalAreaMode() );
  }
}

/**
 * @brief		ポケモンを見た数取得（全国）
 * @return	見た数
 */
u16 ZukanData::GetPokeSeeCount( void ) const
{
  int count;

  this->IncorrectZukan();

  count= 0;
  for( int i=1; i<=MONSNO_END; i++ ){
    if( this->GetPokeSeeFlag( static_cast<MonsNo>(i) ) != false ){
      count++;
    }
  }
  return count;
}


/**
 * @brief		ポケモンを見た数取得（地方）
 * @param		ポケモン番号辞書クラス
 *
 * @return	見た数
 */
u16 ZukanData::GetLocalPokeSeeCount( const PokeTool::ExtendData * poke_extend, PokeTool::ExtendData::LocalArea local_area ) const
{
  u16 num=0;

  this->IncorrectZukan();

  num = 0;

  for( int i=1; i<=MONSNO_END; i++ ){
    if( this->GetPokeSeeFlag( static_cast<MonsNo>(i) ) != false ){
      // 地方図鑑にいるかチェック
      if( poke_extend->IsExistLocalZukan(i,local_area) ){
        num++;
      }
    }
  }

  return num;
}

u16 ZukanData::GetLocalPokeSeeCountStatic( PokeTool::ExtendData::LocalArea local_area ) const
{
  GFL_ASSERT( local_area == PokeTool::ExtendData::LOCAL_AREA_A );  // 開発中に気付かせるためのASSERT。
  
  u16 num=0;

  this->IncorrectZukan();

  num = 0;

  for( int i=1; i<=MONSNO_END; i++ ){
    if( this->GetPokeSeeFlag( static_cast<MonsNo>(i) ) != false ){
      // 地方図鑑にいるかチェック
      if( PokeTool::ExtendData::IsExistLocalZukanStatic(i,local_area) ){
        num++;
      }
    }
  }

  return num;
}

/**
 * @brief		指定タイプのポケモンを見たか
 * @param		monsno	ポケモン番号
 * @param		sex			性別
 * @param		rare		レア
 * @param		form		フォルム
 * @retval	"true = 見た"
 * @retval	"false = それ以外"
 *
 * @li	性別不明の場合は♂♀のどちらかのフラグが立っていればtrueを返す（図鑑ナビで使用してます！）
 */
bool ZukanData::CheckPokeSeeForm( MonsNo monsno, pml::Sex sex, bool rare, FormNo form ) const
{
  this->IncorrectZukan();

  if( this->IncorrectMonsNo(monsno) ){
    return false;
  }
 
  if( this->CheckSeeBit( monsno, sex, rare, form ) ) return true;

  // 性別不明
  pml::personal::LoadPersonalData( monsno, form );

  if( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ) == pml::personal::SEX_VECTOR_UNKNOWN ) {
    if( this->CheckSeeBit( monsno, pml::SEX_MALE  , rare, form ) ) return true;
    if( this->CheckSeeBit( monsno, pml::SEX_FEMALE, rare, form ) ) return true;
	}

  return false;
}

bool ZukanData::CheckPokeSeeForm( const PokeTool::SimpleParam & simple_param ) const
{
  if( simple_param.isEgg )
  {
    GFL_ASSERT( 0 ); //@fix
    return false;
  }
  return this->CheckPokeSeeForm( simple_param.monsNo, simple_param.sex, simple_param.isRare, simple_param.formNo );
}

void ZukanData::IncrementPokeSeeAmount( MonsNo monsno )
{
  GFL_UNUSED(monsno);
  return;  // nijiではm_data.see_amountは使わないので値が設定されないように本関数の中身をコメントアウトしておく。

  /*
  if( this->IncorrectMonsNo(monsno) ) return;
  
  GFL_PRINT("\n");
  GFL_PRINT("################################################\n");
  GFL_PRINT("###                                          ###\n");
  GFL_PRINT("###   出会った回数アップ MonsNo[%d] %d回目   ###\n", monsno, m_data.see_amount[monsno-1]+1);
  GFL_PRINT("###                                          ###\n");
  GFL_PRINT("################################################\n");
  GFL_PRINT("\n");

  if(1<=monsno && monsno<=POKESEEGET_AMOUNT_LEN)
  {
    if(m_data.see_amount[monsno-1] < POKESEEGET_AMOUNT_VALUE_MAX)
    {
      ++m_data.see_amount[monsno-1];
    }
    else
    {
      GFL_PRINT("見た総数が最大値%dです。\n", m_data.see_amount[monsno-1]);
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "monsno=%dが異常です。\n", monsno);
  }
  */
}

/**
 * @brief monsnoのポケモンを今までに見た総数を取得
 *
 * @param monsno ポケモン番号
 *
 * @return monsnoのポケモンを今までに見た総数
*/
u16 ZukanData::GetPokeSeeAmount( MonsNo monsno ) const
{
  if(1<=monsno && monsno<=POKESEEGET_AMOUNT_LEN)
  {
    if(m_data.see_amount[monsno-1] > POKESEEGET_AMOUNT_VALUE_MAX)
    {
      return POKESEEGET_AMOUNT_VALUE_MAX;
    }
    else
    {
      return m_data.see_amount[monsno-1];
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "monsno=%dが異常です。\n", monsno);
    return 0;
  }
}

////////////////////////////////
//
// 完成度
//
////////////////////////////////

/**
 * @brief		全国図鑑完成チェック
 * @retval	"true = 完成"
 * @retval	"false = それ以外"
 */
bool ZukanData::CheckZenkokuComp(void) const
{
  u16 num;

  // ゼンコク図鑑完成に必要なポケモンを何匹捕まえたか
  num = this->GetZenkokuGetCompCount();

  GFL_PRINT( "全国ずかん完成チェック %d匹", num );  // NAGI_PRINT
  
  if( num >= ZENKOKU_COMP_NUM ){
    
    GFL_PRINT( "complete\n" );  // NAGI_PRINT
    return true;
  }
  GFL_PRINT( "ng\n" );  // NAGI_PRINT
  return false;
}

/**
 * @brief		地方図鑑完成チェック（捕獲数）
 * @param		ポケモン番号辞書クラス
 * @retval	"true = 完成"
 * @retval	"false = それ以外"
 */
bool ZukanData::CheckLocalGetComp( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const
{
  u16 num;

  // 地方図鑑完成に必要なポケモンを何匹捕まえたか
  num = this->GetLocalGetCompCount( local_area, poke_extend );

  GFL_PRINT( "地方ずかん完成チェック %d匹", num );  // NAGI_PRINT
  
  if( num >= this->GetLocalCompNum( poke_extend, local_area ) ){
    GFL_PRINT( "complete\n" );  // NAGI_PRINT
    return true;
  }
  GFL_PRINT( "ng\n" );  // NAGI_PRINT
  return false;
}

/**
 * @brief		地方図鑑完成チェック（見た数）
 * @param		ポケモン番号辞書クラス
 * @retval	"true = 完成"
 * @retval	"false = それ以外"
 */
bool ZukanData::CheckLocalSeeComp( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const
{
  u16 num;

  // 地方図鑑完成に必要なポケモンを何匹捕まえたか
  num = this->GetLocalSeeCompCount( local_area, poke_extend );

  if( num >= this->GetLocalCompNum( poke_extend, local_area ) ){
    return true;
  }
  return false;
}

/**
 * @brief		全国図鑑完成に必要なポケモンの捕獲数
 * @return	捕獲数
 */
u16 ZukanData::GetZenkokuGetCompCount(void) const
{
  u16 num;

  num = 0;
  for( int i=1; i<=ZENKOKU_MONSMAX; i++ ){
    if( this->GetPokeGetFlag( static_cast<MonsNo>(i) ) != false ){
      if( this->CheckZenkokuCompMonsno( static_cast<MonsNo>(i) ) != false ){
        num ++;
      }
    }
  }
  return num;
}


/**
 * @brief		地方図鑑完成に必要なポケモンの捕獲数
 * @param		ポケモン番号辞書クラス
 * @return	捕獲数
 */
u16 ZukanData::GetLocalGetCompCount( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const
{
  u16 num=0;

  for( int i=1; i<=ZENKOKU_MONSMAX; i++ ){
    if( this->GetPokeGetFlag( static_cast<MonsNo>(i) ) != false ){
      if( poke_extend->IsExistLocalZukan(i, local_area) ){
        if( this->CheckLocalCompMonsno( static_cast<MonsNo>(i), local_area ) != false ){
          num++;
        }
      }
    }
  }

  return num;
}

/**
 * @brief		地方図鑑完成に必要なポケモンを見た数
 * @param		ポケモン番号辞書クラス
 * @return	見た数
 */
u16 ZukanData::GetLocalSeeCompCount( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const
{
  u16 num=0;


  for( int i=1; i<=ZENKOKU_MONSMAX; i++ ){
    if( this->GetPokeSeeFlag( static_cast<MonsNo>(i) ) != false ){
      if( poke_extend->IsExistLocalZukan(i, local_area) ){
        if( this->CheckLocalCompMonsno( static_cast<MonsNo>(i), local_area ) != false ){
          num++;
        }
      }
    }
  }

  return num;
}

/**
 * @brief 地方チェック最大数を取得
 *
 * @param local_area 地方エリア
 *
 * @return 地方チェック最大数を取得
 */
u16 ZukanData::GetLocalCompNum( const PokeTool::ExtendData * poke_extend, PokeTool::ExtendData::LocalArea local_area ) const
{
  u16 num;
  this->GetLocalNotCheckMonsNoTable( local_area, &num );
  u16 max = poke_extend->GetLocalAreaMonsMax(local_area);  // @fix NMCat[1022] 完成していないのに王冠マークがついてしまう → 最大番号を返すので-1する必要はないのでそれを止めた。
  GFL_PRINT("地方最大 max%d min%d num%d\n", max, num, max-num);  // NAGI_PRINT
  return max - num;
}


////////////////////////////////
//
// 言語
//
////////////////////////////////

/**
 * @brief		図鑑テキストバージョンチェック
 * @param		monsno				ポケモン番号
 * @param		country_code	国コード
 * @retval	"true = 表示可"
 * @retval	"false = それ以外"
 */
bool ZukanData::GetTextVersionUpFlag( MonsNo monsno, u32 country_code ) const
{
  this->IncorrectZukan();
  
  //テキストバージョンは自分の国は常にTrue
  if( country_code == System::GetLang() )
  {
    return true;
  }

	// 国コード最大 or 欠番コード
	if( (country_code > 10) || (country_code == 6) || (country_code == 0) ){
		return false;
	}

	// 言語６番が欠番なので、スペイン語以降は-1
	if( country_code >= POKEMON_LANG_SPAIN ){
		country_code -= 1;
	}

	return this->CheckBit( m_data.TextVersionUp, (monsno-1)*TEXTVER_VER_MAX+(country_code-1) );
}


////////////////////////////////
//
// 番号
//
////////////////////////////////

/**
 * @brief 現在の図鑑モードの末尾ポケモンを取得
 *
 * @param poke_extend　ポケモン番号辞書クラス
 *
 * @return 末尾ポケモン(全国ならば全国図鑑番号、地方ならば地方図鑑番号で返る)
 */
u16 ZukanData::GetZukanTailMonsNo( const PokeTool::ExtendData * poke_extend ) const
{
  this->IncorrectZukan();

  if( this->GetZukanMode() == MODE_ZENKOKU )
  {
    for( int i= MONSNO_END; i >= 1; --i )
    {
      if( this->GetPokeSeeFlag( static_cast<MonsNo>(i) )
          || this->GetPokeGetFlag( static_cast<MonsNo>(i) ) ){
        return i;
      }
    }
  }
  else
  {
    PokeTool::ExtendData::LocalArea area = this->GetLocalAreaMode();

    for( int i= poke_extend->GetLocalAreaMonsMax( area ); i >= 1; --i )  // @fix NMCat[1022] 完成していないのに王冠マークがついてしまう → 最大番号を返すので-1する必要はないのでそれを止めた。
    {
      if( this->GetPokeSeeFlag( static_cast<MonsNo>(poke_extend->GetZenkokuNumber(i,area)) )
          || this->GetPokeGetFlag( static_cast<MonsNo>(poke_extend->GetZenkokuNumber(i,area)) ) )
      {
        return i;
      }
    }
  }
  
  return 0;
}


////////////////////////////////
//
// 個性乱数
//
////////////////////////////////

/**
 * @brief		特殊ポケモンの個性乱数取得
 * @param		monsno	ポケモン番号
 * @return	個性乱数
 */
/*
u32 ZukanData::GetPokeRandomFlag( MonsNo monsno ) const
{
  this->IncorrectZukan();

	// パッチールのみ
	if( monsno == MONSNO_PATTIIRU ){
		return m_data.PachiRandom[0];
	}
  else
  {
	  return 0;
  }
}
*/
u32 ZukanData::GetPokeRandomFlag( MonsNo monsno, pml::Sex sex, bool rare, FormNo /*form*/ ) const
{
  this->IncorrectZukan();

	// パッチールのみ
	if( monsno == MONSNO_PATTIIRU ){
    u32 i = 0;
    if(sex == pml::SEX_FEMALE)
    {
      if(rare) i = SEE_FLAG_F_RARE;
      else     i = SEE_FLAG_FEMALE;
    }
    else
    {
      if(rare) i = SEE_FLAG_M_RARE;
      else     i = SEE_FLAG_MALE;
    }
		return m_data.PachiRandom[i];
	}
  else
  {
	  return 0;
  }
}


////////////////////////////////
//
// デバッグ機能
//
////////////////////////////////

#if PM_DEBUG
void ZukanData::DebugSetPokeGetForce( const pml::pokepara::CoreParam & param )
{
  //this->DebugSetPokeDeleteForce( param );  // それまでの設定が消えてしまうので消さない
  this->SetPokeGet( param );
}
void ZukanData::DebugSetPokeSeeForce( const pml::pokepara::CoreParam & param )
{
  //this->DebugSetPokeDeleteForce( param );  // それまでの設定が消えてしまうので消さない
  this->SetPokeSee( param );
}
void ZukanData::DebugSetPokeDeleteForce( const pml::pokepara::CoreParam & param )
{
  this->IncorrectZukan();

	bool fast = param.StartFastMode();
	// タマゴなら登録しない
	if( param.IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ){
    GFL_PRINT( "タマゴ異常：%d\n", param.GetMonsNo());
    param.EndFastMode( fast );
		return;
	}
	MonsNo mons = param.GetMonsNo();
	//u32 rand    = param.GetPersonalRnd();
	//FormNo form = param.GetFormNo();
  //u32 lang    = param.GetLangId();
  //pml::Sex sex  = param.GetSex();
  //bool rare   = param.IsRare();
  param.EndFastMode( fast );

  //捕獲フラグリセット
  this->ResetBitMons( reinterpret_cast<u8*>(m_data.get_flag), mons );

  //見たフラグリセット
  u32 formMax = this->GetFormMax(mons);
  for(u32 j=0; j<formMax; ++j)
  {
    u16 index = this->GetMonsFormIndex(mons, j);
    if(index == MONS_FORM_INDEX_INVALID)
    {
      continue;
    }

    for(u32 i=0; i<SEE_FLAG_MAX; ++i)
    {
      this->ResetBit( reinterpret_cast<u8 *>(m_data.see_flag[i]), index, MONS_FORM_ARRAY_LEN*4 );
    }
  }
 
  //言語
  static const u32 POKEMON_LANG_VALID_ARRAY[] =
  {
    POKEMON_LANG_JAPAN  ,
    POKEMON_LANG_ENGLISH,
    POKEMON_LANG_FRANCE ,
    POKEMON_LANG_ITALY  ,
    POKEMON_LANG_GERMANY,
    POKEMON_LANG_SPAIN  ,
    POKEMON_LANG_KOREA  ,
    POKEMON_LANG_CHINA  ,
    POKEMON_LANG_TAIWAN ,
  };
  for(u32 i=0; i<GFL_NELEMS(POKEMON_LANG_VALID_ARRAY); ++i)
  {
    this->ResetZukanTextVersionUp( mons, POKEMON_LANG_VALID_ARRAY[i] );
  }
}
#endif  //PM_DEBUG

#if PM_DEBUG
void ZukanData::Debug_SetPokeSeeAmount( MonsNo monsno, int val )
{
	if( monsno > MONSNO_NULL && monsno <= POKESEEGET_AMOUNT_LEN ){
		// 見たことにしておく
		PokeTool::SimpleParam prm;
		prm.monsNo  = monsno;
		prm.formNo  = 0;
		prm.sex     = pml::SEX_MALE;
		prm.isRare  = false;
		prm.isEgg   = false;
		prm.perRand = 0;
		this->SetPokeSee( prm );

		if( val < 1 ){
			val = 1;
		}else if( val > POKESEEGET_AMOUNT_VALUE_MAX ){
			val = POKESEEGET_AMOUNT_VALUE_MAX;
		}
		m_data.see_amount[monsno-1] = val;
	}else{
		GFL_ASSERT_MSG(0, "monsno=%dが異常です。\n", monsno);
	}
}
#endif	// PM_DEBUG


//=============================================================================
//@brief SaveDataInterface実装
//=============================================================================
/**
 *  @brief    読み込んだデータをセットする関数
  * @param    pData   先頭のポインタ
 */
void ZukanData::SetData(void* data)
{
   gfl2::std::MemCopy(data, &m_data, sizeof(m_data) );
}

/**
 *  @brief    データの先頭ポインタを得る関数
  * @return   先頭のポインタ
 */
void* ZukanData::GetData(void)
{
  return reinterpret_cast<void*>(&m_data);
}

/**
 *  @brief    データサイズ
 *  @retval   バイト
 */
size_t ZukanData::GetDataSize(void)
{
  return sizeof( ZukanCoreData );
}

/**
 * @brief セーブデータのクリア
 * @param heap　クリアに使用するテンポラリヒープ(使用していない)
 */
void ZukanData::Clear( gfl2::heap::HeapBase * heap )
{
GFL_UNUSED(heap);


  // 右目 Loop_01 ->  58
  // 左目 Loop_02 -> 122
  // 右耳 Loop_03 -> 102
  // 左耳 Loop_04 -> 120

  // ０バイト目：左の耳　１バイト目：右の耳　２バイト目：左の目　３バイト目：右の目

  static const u32 PACHI_RANDOM_OFFICIAL = 0x3A7A6678;



  gfl2::std::MemClear( &m_data, this->GetDataSize() );

  m_data.zukan_magic = MAGIC_NUMBER;
  
  // 最初から立っているフラグ 
  m_data.area_a_flag = 1;
  m_data.area_b_flag = 1;

  for(u32 i=0; i<SEE_FLAG_MAX; ++i)
  {
    //m_data.PachiRandom[i] = 0x88888888;  // パッチールの公式イラストは0x88888888ではないらしいが、nijiでは0x88888888でいく。
    m_data.PachiRandom[i] = PACHI_RANDOM_OFFICIAL;
  }

  // m_data.zukan_modeとm_data.defaultMonsNoは0初期化でよい
  
  gfl2::std::MemClear( &m_notSaveData, sizeof(ZukanNotSaveData) );
}

//============================================================================================
//	内部関数
//============================================================================================

////////////////////////////////
//////// 不正チェック
////////////////////////////////
/**
 * @brief		ずかんワークの整合性チェック
 */
void ZukanData::IncorrectZukan( void ) const
{
  GFL_ASSERT_STOP(m_data.zukan_magic == MAGIC_NUMBER);  //@fix
}

/**
 * @brief		ポケモンナンバーの範囲チェック
 * @param		monsno		ポケモン番号
 * @retval	"true = 範囲外"
 * @retval	"false = それ以外"
 */
bool ZukanData::IncorrectMonsNo(MonsNo monsno) const
{
  if (monsno == 0 || monsno > MONSNO_END) {
    GFL_ASSERT_MSG( 0, "ポケモンナンバー異常：%d\n", monsno); //@check
    return true;
  } else {
    return false;
  }
}

////////////////////////////////
//////// 汎用的なビット操作
////////////////////////////////
/**
 * @brief		汎用ビットチェック
 * @param		array			ビットテーブル
 * @param		flag_id		フラグＩＤ
 * @retval	"true = ON"
 * @retval	"false = OFF"
 */
bool ZukanData::CheckBit(const u8 * array, u16 flag_id) const
{
  return 0 != (array[flag_id >> 3] & (1 << (flag_id & 7)));
}

/**
 * @brief		汎用ビットチェック（フラグ = ポケモン番号）
 * @param		array			ビットテーブル
 * @param		flag_id		フラグＩＤ
 * @retval	"true = ON"
 * @retval	"false = OFF"
 */
bool ZukanData::CheckBitMons(const u8 * array, MonsNo flag_id) const
{
	return this->CheckBit( array, static_cast<int>(flag_id)-1 );
}

/**
 * @brief		汎用ビットセット
 * @param		array			ビットテーブル
 * @param		flag_id		フラグＩＤ
 */
void ZukanData::SetBit(u8 * array, u16 flag_id, u32 array_len )
{
  if( static_cast<u32>(( flag_id >> 3 )) >= array_len )
  {
    GFL_ASSERT_MSG( 0, "flag%d flag_ar%d max_ar%d\n",  flag_id, ( flag_id >> 3 ), array_len ); //@fix
    return;
  }
  
  array[flag_id >> 3] |= 1 << (flag_id & 7);
}

/**
 * @brief		汎用ビットセット（フラグ = ポケモン番号）
 * @param		array			ビットテーブル
 * @param		flag_id		フラグＩＤ
 */
void ZukanData::SetBitMons(u8 * array, MonsNo flag_id)
{
	this->SetBit( array, flag_id-1, POKEZUKAN_ARRAY_LEN*4 );
}

/**
 * @brief		汎用ビットリセット
 * @param		array			ビットテーブル
 * @param		flag_id		フラグＩＤ
 * @param array_len 配列
 */
void ZukanData::ResetBit( u8 * array, u16 flag_id, u32 array_len )
{
  if( static_cast<u32>(( flag_id >> 3 )) >= array_len )
  {
    GFL_ASSERT_MSG( 0, "reset flag%d flag_ar%d max_ar%d\n",  flag_id, ( flag_id >> 3 ), array_len );  //@fix
    return;
  }
  array[flag_id >> 3] &= (1 << (flag_id & 7)) ^ 0xff;
}

/**
 * @brief		汎用ビットリセット（フラグ = ポケモン番号）
 * @param		array			ビットテーブル
 * @param		flag_id		フラグＩＤ
 */
void ZukanData::ResetBitMons( u8 * array, MonsNo flag_id )
{
	this->ResetBit( array, flag_id-1, POKEZUKAN_ARRAY_LEN*4 );
}

////////////////////////////////
//////// 見た、捕まえた
////////////////////////////////
/**
 * @brief		つかまえたビット設定
 * @param		flag_id		ポケモン番号
 */
void ZukanData::SetGetBit( MonsNo flag_id )
{
  this->SetBitMons( reinterpret_cast<u8*>(m_data.get_flag), flag_id );
}

/**
 * @brief		みたビット設定
 * @param		mons		ポケモン番号
 * @param		sex			性別
 * @param		rare		レアフラグ true = RARE
 */
void ZukanData::SetSeeBit( MonsNo mons, pml::Sex sex, bool rare, FormNo form )
{
  if( this->IncorrectMonsNo(mons) ) return;

  FormNo form_start = form;
  FormNo form_end   = form;
  this->GetFormStartEndWhenSetSeeBit( mons, form, &form_start, &form_end );

  {
    for(u16 form_i=form_start; form_i<=form_end; ++form_i)
    {
      // ポワルンのレアフラグ特別処理【制限】
      if( mons == MONSNO_POWARUN )
      {
        // ポワルンは通常フォルムにはレアカラーが存在するが、
        // それ以外のフォルムにはレアカラーが存在しない。
        if( form_i != FORMNO_POWARUN_NORMAL )
        {
          rare = false;
        }
      }

      u16 index = this->GetMonsFormIndex(mons, form_i);
      if(index == MONS_FORM_INDEX_INVALID)
      {
        continue;
      }
     
      if( sex == pml::SEX_MALE ){
        if( rare != false ){
          this->SetBit( reinterpret_cast<u8 *>(m_data.see_flag[SEE_FLAG_M_RARE]), index, MONS_FORM_ARRAY_LEN*4 );
        }else{
          this->SetBit( reinterpret_cast<u8 *>(m_data.see_flag[SEE_FLAG_MALE]), index, MONS_FORM_ARRAY_LEN*4 );
        }
      }else if( sex == pml::SEX_FEMALE ){
        if( rare != false ){
          this->SetBit( reinterpret_cast<u8 *>(m_data.see_flag[SEE_FLAG_F_RARE]), index, MONS_FORM_ARRAY_LEN*4 );
        }else{
          this->SetBit( reinterpret_cast<u8 *>(m_data.see_flag[SEE_FLAG_FEMALE]), index, MONS_FORM_ARRAY_LEN*4 );
        }
      }else{
        // 性別がない場合は♂のフラグを立てる
        if( rare != false ){
          this->SetBit( reinterpret_cast<u8 *>(m_data.see_flag[SEE_FLAG_M_RARE]), index, MONS_FORM_ARRAY_LEN*4 );
        }else{
          this->SetBit( reinterpret_cast<u8 *>(m_data.see_flag[SEE_FLAG_MALE]), index, MONS_FORM_ARRAY_LEN*4 );
        }
      }
    }
  }
}

/**
 * @brief		つかまえたビットON/OFFチェック
 * @param		monsno		ポケモン番号
 * @retval	"true = ON"
 * @retval	"false = OFF"
 */
bool ZukanData::CheckGetBit( MonsNo monsno ) const
{
  return this->CheckBitMons(reinterpret_cast<const u8*>(m_data.get_flag), monsno );
}

/**
 * @brief		みたビットON/OFFチェック
 * @param		monsno		ポケモン番号
 * @retval	"true = ON"
 * @retval	"false = OFF"
 */
bool ZukanData::CheckSeeBit( MonsNo mons, pml::Sex sex, bool rare, FormNo form ) const
{
  if( this->IncorrectMonsNo(mons) ) return false;

  {
    {
      u16 index = this->GetMonsFormIndex(mons, form);
      if(index == MONS_FORM_INDEX_INVALID)
      {
        return false;
      }
     
      if( sex == pml::SEX_MALE ){
        if( rare != false ){
          return this->CheckBit( reinterpret_cast<const u8 *>(m_data.see_flag[SEE_FLAG_M_RARE]), index );
        }else{
          return this->CheckBit( reinterpret_cast<const u8 *>(m_data.see_flag[SEE_FLAG_MALE]), index );
        }
      }else if( sex == pml::SEX_FEMALE ){
        if( rare != false ){
          return this->CheckBit( reinterpret_cast<const u8 *>(m_data.see_flag[SEE_FLAG_F_RARE]), index );
        }else{
          return this->CheckBit( reinterpret_cast<const u8 *>(m_data.see_flag[SEE_FLAG_FEMALE]), index );
        }
      }else{
        // 性別がない場合は♂のフラグを立てる
        if( rare != false ){
          return this->CheckBit( reinterpret_cast<const u8 *>(m_data.see_flag[SEE_FLAG_M_RARE]), index );
        }else{
          return this->CheckBit( reinterpret_cast<const u8 *>(m_data.see_flag[SEE_FLAG_MALE]), index );
        }
      }
    }
  }
}

////////////////////////////////
//////// フォルム補正
////////////////////////////////
void ZukanData::GetFormNoWhenSetDrawData( MonsNo mons, FormNo form, FormNo* form_return ) const
{
  FormNo l_form_return = form;

  // ナガレボシ（メテノ）の殻つきのフォルムは必ずFORMNO_NAGAREBOSI_M_REDにする。【制限】
  if( mons == MONSNO_NAGAREBOSI )
  {
    if(FORMNO_NAGAREBOSI_M_RED<=form && form<=FORMNO_NAGAREBOSI_M_PURPLE)
    {
      l_form_return = FORMNO_NAGAREBOSI_M_RED;
    }
  }
  // ゲッコウガの配信専用のフォルムは必ずFORMNO_GEKKOUGA_NORMALにする【制限】
  else if( mons == MONSNO_GEKKOUGA )
  {
    if(FORMNO_GEKKOUGA_GEKKOUGA2<=form && form<=FORMNO_GEKKOUGA_GEKKOUGA2)
    {
      l_form_return = FORMNO_GEKKOUGA_NORMAL;
    }
  }
  // ジガルデの５０％フォルム（ヘビ）と１０％フォルム（イヌ）【制限】
  else if( mons == MONSNO_ZIGARUDE )
  {
    if(form==FORMNO_ZIGARUDE_SW50PER)
    {
      l_form_return = FORMNO_ZIGARUDE_50PER;
    }
    else if(form==FORMNO_ZIGARUDE_SW10PER)
    {
      l_form_return = FORMNO_ZIGARUDE_10PER;
    }
  }
  // ラッタのぬしフォルム【制限】
  else if( mons == MONSNO_RATTA )
  {
    if(form==FORMNO_RATTA_NUSHI)
    {
      l_form_return = FORMNO_RATTA_ALOLA;
    }
  }
  // デカグースのぬしフォルム【制限】
  else if( mons == MONSNO_MANGUUSU2 )
  {
    if(form==FORMNO_MANGUUSU2_NUSHI)
    {
      l_form_return = FORMNO_MANGUUSU2_NORMAL;
    }
  }
  // クワガノンのぬしフォルム【制限】
  else if( mons == MONSNO_KUWAGATA3 )
  {
    if(form==FORMNO_KUWAGATA3_NUSHI)
    {
      l_form_return = FORMNO_KUWAGATA3_NORMAL;
    }
  }
  // ラランテスのぬしフォルム【制限】
  else if( mons == MONSNO_HANAKAMA2 )
  {
    if(form==FORMNO_HANAKAMA2_NUSHI)
    {
      l_form_return = FORMNO_HANAKAMA2_NORMAL;
    }
  }
  // エンニュートのぬしフォルム【制限】
  else if( mons == MONSNO_DOKUTOKAGE2 )
  {
    if(form==FORMNO_DOKUTOKAGE2_NUSHI)
    {
      l_form_return = FORMNO_DOKUTOKAGE2_NORMAL;
    }
  }
  // ミミッキュのぬしフォルム【制限】
  else if( mons == MONSNO_HORAA )
  {
    if(form==FORMNO_HORAA_NORMAL_NUSHI)
    {
      l_form_return = FORMNO_HORAA_NORMAL;
    }
    else if(form==FORMNO_HORAA_KUBIORE_NUSHI)
    {
      l_form_return = FORMNO_HORAA_KUBIORE;
    }
  }
  // ジャラランガのぬしフォルム【制限】
  else if( mons == MONSNO_UROKO3 )
  {
    if(form==FORMNO_UROKO3_NUSHI)
    {
      l_form_return = FORMNO_UROKO3_NORMAL;
    }
  }
  /*
    momiji
  */
  // ガラガラのぬしフォルム【制限】
  else if(mons == MONSNO_GARAGARA)
  {
    if(form == FORMNO_GARAGARA_NUSHI)
    {
      l_form_return = FORMNO_GARAGARA_ALOLA;
    }
  }
  // アブリボンのぬしフォルム【制限】
  else if(mons == MONSNO_ABU2)
  {
    if(form == FORMNO_ABU2_NUSHI)
    {
      l_form_return = FORMNO_ABU2_NORMAL;
    }
  }
  /**
    @fix  MMCat[158]   イワンコ(フォルム1)がフォルム0として登録されない
    イワンコは必ずForm0が表示/登録されるよう修正
  */
  // #744: イワンコ FORMNO_OOKAMI1_TWILIGHT 【制限】
  else if(mons == MONSNO_OOKAMI1)
  {
    if(form == FORMNO_OOKAMI1_TWILIGHT)
    {
      l_form_return = FORMNO_OOKAMI1_NORMAL;
    }
  }
  // #752：オニシズクモ ぬしフォルム【制限】
  else if(mons == MONSNO_MIZUGUMO2)
  {
    if(form == FORMNO_MIZUGUMO2_NUSHI)
    {
      l_form_return = FORMNO_MIZUGUMO2_NORMAL;
    }
  }
  // #777：トゲデマル ぬしフォルム【制限】
  else if(mons == MONSNO_NIZITYUU)
  {
    if(form == FORMNO_NIZITYUU_NUSHI)
    {
      l_form_return = FORMNO_NIZITYUU_NORMAL;
    }
  }

  else
  {
    // 登録できないフォルムの場合
    if( form >= this->GetFormMax( mons ) ){  // @fix NMCat[416] コフキムシをタマゴ孵化するとはまる
      l_form_return = 0;                     // コフキムシ、コフーライはフォルム数以上のフォルム番号が来るので、
    }                                        // 未知のフォルム番号が来ても対応できるようにしておく。
                                             // 2017/04/04 Tue. Yu Muto: momijiにおいて、コフキムシ、コフーライはFormが追加された
  }

  *form_return = l_form_return;
}
void ZukanData::GetFormStartEndWhenSetSeeBit( MonsNo mons, FormNo form, FormNo* form_start, FormNo* form_end ) const
{
  FormNo l_form_start = form;  // form_start<= <=form_end
  FormNo l_form_end   = form;

  // 一部のポケモンは全てのフォルムをONにする
  // ポワルン【全解放】
  if( mons == MONSNO_POWARUN ){
    l_form_start = 0;
    l_form_end   = FORMNO_POWARUN_MAX -1;
  }
  // チェリム【全解放】
  else if( mons == MONSNO_THERIMU ){
    l_form_start = 0;
    l_form_end   = FORMNO_THERIMU_MAX -1;
  }
  // ヒヒダルマ【全解放】
  else if( mons == MONSNO_HIHIDARUMA ){
    l_form_start = 0;
    l_form_end   = FORMNO_HIHIDARUMA_MAX -1;
  }
  // メロディア【全解放】
  else if( mons == MONSNO_MEROETTA ){
    l_form_start = 0;
    l_form_end   = FORMNO_MEROETTA_MAX -1;
  }
  // ギルガルド【全解放】
  else if( mons == MONSNO_GIRUGARUDO ){
    l_form_start = 0;
    l_form_end   = FORMNO_GIRUGARUDO_MAX -1;
  }
  // ヨワシ【全解放】
  else if( mons == MONSNO_YOWASI )
  {
    l_form_start = 0;
    l_form_end   = FORMNO_YOWASI_MAX -1;
  }
  // ミミッキュ【(制限以外)全解放】＆【制限】
  else if( mons == MONSNO_HORAA )  // @fix NMCat[1167] ミミッキュは全開放だが、ぬしフォルムは図鑑登録しない。
  {
    l_form_start = FORMNO_HORAA_NORMAL;
    l_form_end   = FORMNO_HORAA_KUBIORE;
  
    /*
    ぬしフォルムは図鑑登録しないので、以下の記述は不要。
    if(form==FORMNO_HORAA_NORMAL_NUSHI)
    {
      l_form_start = FORMNO_HORAA_NORMAL;
      l_form_end   = FORMNO_HORAA_NORMAL;
    }
    else if(form==FORMNO_HORAA_KUBIORE_NUSHI)
    {
      l_form_start = FORMNO_HORAA_KUBIORE;
      l_form_end   = FORMNO_HORAA_KUBIORE;
    }
    */
  }
  // ナガレボシ（メテノ）の殻つきのフォルムは必ずFORMNO_NAGAREBOSI_M_REDにする。【制限】
  else if( mons == MONSNO_NAGAREBOSI )
  {
    if(FORMNO_NAGAREBOSI_M_RED<=form && form<=FORMNO_NAGAREBOSI_M_PURPLE)
    {
      l_form_start = FORMNO_NAGAREBOSI_M_RED;
      l_form_end   = FORMNO_NAGAREBOSI_M_RED;
    }
  }
  // ゲッコウガの配信専用のフォルムは必ずFORMNO_GEKKOUGA_NORMALにする【制限】
  else if( mons == MONSNO_GEKKOUGA )
  {
    if(FORMNO_GEKKOUGA_GEKKOUGA2<=form && form<=FORMNO_GEKKOUGA_GEKKOUGA2)
    {
      l_form_start = FORMNO_GEKKOUGA_NORMAL;
      l_form_end   = FORMNO_GEKKOUGA_NORMAL;
    }
  }
  // ジガルデの５０％フォルム（ヘビ）と１０％フォルム（イヌ）【制限】
  else if( mons == MONSNO_ZIGARUDE )
  {
    if(form==FORMNO_ZIGARUDE_SW50PER)
    {
      l_form_start = FORMNO_ZIGARUDE_50PER;
      l_form_end   = FORMNO_ZIGARUDE_50PER;
    }
    else if(form==FORMNO_ZIGARUDE_SW10PER)
    {
      l_form_start = FORMNO_ZIGARUDE_10PER;
      l_form_end   = FORMNO_ZIGARUDE_10PER;
    }
  }
  // ラッタのぬしフォルム【制限】
  else if( mons == MONSNO_RATTA )
  {
    if(form==FORMNO_RATTA_NUSHI)
    {
      l_form_start = FORMNO_RATTA_ALOLA;
      l_form_end   = FORMNO_RATTA_ALOLA;
    }
  }
  // デカグースのぬしフォルム【制限】
  else if( mons == MONSNO_MANGUUSU2 )
  {
    if(form==FORMNO_MANGUUSU2_NUSHI)
    {
      l_form_start = FORMNO_MANGUUSU2_NORMAL;
      l_form_end   = FORMNO_MANGUUSU2_NORMAL;
    }
  }
  // クワガノンのぬしフォルム【制限】
  else if( mons == MONSNO_KUWAGATA3 )
  {
    if(form==FORMNO_KUWAGATA3_NUSHI)
    {
      l_form_start = FORMNO_KUWAGATA3_NORMAL;
      l_form_end   = FORMNO_KUWAGATA3_NORMAL;
    }
  }
  // ラランテスのぬしフォルム【制限】
  else if( mons == MONSNO_HANAKAMA2 )
  {
    if(form==FORMNO_HANAKAMA2_NUSHI)
    {
      l_form_start = FORMNO_HANAKAMA2_NORMAL;
      l_form_end   = FORMNO_HANAKAMA2_NORMAL;
    }
  }
  // エンニュートのぬしフォルム【制限】
  else if( mons == MONSNO_DOKUTOKAGE2 )
  {
    if(form==FORMNO_DOKUTOKAGE2_NUSHI)
    {
      l_form_start = FORMNO_DOKUTOKAGE2_NORMAL;
      l_form_end   = FORMNO_DOKUTOKAGE2_NORMAL;
    }
  }
  // ジャラランガのぬしフォルム【制限】
  else if( mons == MONSNO_UROKO3 )
  {
    if(form==FORMNO_UROKO3_NUSHI)
    {
      l_form_start = FORMNO_UROKO3_NORMAL;
      l_form_end   = FORMNO_UROKO3_NORMAL;
    }
  }
  /*
    momiji
  */
  // ガラガラのぬしフォルム【制限】
  else if(mons == MONSNO_GARAGARA)
  {
    if(form == FORMNO_GARAGARA_NUSHI)
    {
      l_form_start = FORMNO_GARAGARA_ALOLA;
      l_form_end   = FORMNO_GARAGARA_ALOLA;
    }
  }
  // アブリボンのぬしフォルム【制限】
  else if(mons == MONSNO_ABU2)
  {
    if(form == FORMNO_ABU2_NUSHI)
    {
      l_form_start = FORMNO_ABU2_NORMAL;
      l_form_end   = FORMNO_ABU2_NORMAL;
    }
  }
  /**
    @fix  MMCat[158]   イワンコ(フォルム1)がフォルム0として登録されない
    イワンコは必ずForm0が表示/登録されるよう修正
  */
  // #744: イワンコ FORMNO_OOKAMI1_TWILIGHT 【制限】
  else if(mons == MONSNO_OOKAMI1)
  {
    if(form == FORMNO_OOKAMI1_TWILIGHT)
    {
      l_form_start = FORMNO_OOKAMI1_NORMAL;
      l_form_end   = FORMNO_OOKAMI1_NORMAL;
    }
  }
  // #752：オニシズクモ ぬしフォルム【制限】
  else if(mons == MONSNO_MIZUGUMO2)
  {
    if(form == FORMNO_MIZUGUMO2_NUSHI)
    {
      l_form_start = FORMNO_MIZUGUMO2_NORMAL;
      l_form_end   = FORMNO_MIZUGUMO2_NORMAL;
    }
  }
  // #777：トゲデマル ぬしフォルム【制限】
  else if(mons == MONSNO_NIZITYUU)
  {
    if(form == FORMNO_NIZITYUU_NUSHI)
    {
      l_form_start = FORMNO_NIZITYUU_NORMAL;
      l_form_end   = FORMNO_NIZITYUU_NORMAL;
    }
  }

  else{
    // 登録できないフォルムの場合
    if( form >= this->GetFormMax( mons ) ){
      l_form_start = 0;
      l_form_end   = 0;
    }
  }

  *form_start = l_form_start;
  *form_end   = l_form_end;
}

////////////////////////////////
//////// 個性乱数
////////////////////////////////
/**
 * @brief		固体乱数を設定
 * @param		monsno		ポケモン番号
 * @param		rand			個性乱数
 * @param   isGet     Get時に(Getフラグを立てる前に)呼んだらtrue、See時に(Seeフラグを立てる前に)呼んだらfalse
 * @note    monsnoのGetフラグ、Seeフラグが立つ前に呼ぶこと！
 *          初めてSeeして登録、初めてGetして登録。最終的に残るのは初めてGetしたときの値。
 * @note    個性乱数で起こり得ない数値はないかもしれないので、特定の値で未入力を示すことはできない。
 */
/*
void ZukanData::SetZukanRandom( MonsNo monsno, u32 rand, bool isGet )
{
  if( monsno == MONSNO_PATTIIRU )
  {
    if(!isGet)
    {
      // See時
      if(!this->GetPokeSeeFlag(monsno))
      {
        // 初See
        m_data.PachiRandom[0] = rand;
      }
    }
    else
    {
      // Get時
      if(!this->GetPokeGetFlag(monsno))
      {
        // 初Get
        m_data.PachiRandom[0] = rand;
      }
    }
  }
}
*/
void ZukanData::SetZukanRandom( MonsNo monsno, pml::Sex sex, bool rare, FormNo /*form*/, u32 rand, bool isGet )
{
  if( monsno == MONSNO_PATTIIRU )
  {
    u32 i = 0;
    if(sex == pml::SEX_FEMALE)
    {
      if(rare)
      {
        i = SEE_FLAG_F_RARE;
      }
      else
      {
        i = SEE_FLAG_FEMALE;
      }
    }
    else
    {
      if(rare)
      {
        i = SEE_FLAG_M_RARE;
      }
      else
      {
        i = SEE_FLAG_MALE;
      }
    }

    if(!isGet)
    {
      // See時
      if( (m_data.pachi_see_get_flag & (1<<i)) == 0 )  // 見たフラグ(捕まえたフラグが立っているときは見たフラグも立っているので、見たフラグだけ見ればいい)
      {
        // 初See
        m_data.PachiRandom[i] = rand;
        m_data.pachi_see_get_flag |= (1<<i);  // 見たフラグ
      }
    }
    else
    {
      // Get時
      if( (m_data.pachi_see_get_flag & (1<<(i+SEE_FLAG_MAX))) == 0 )  // 捕まえたフラグ
      {
        // 初Get
        m_data.PachiRandom[i] = rand;
        m_data.pachi_see_get_flag |= (1<<i);                 // 見たフラグも
        m_data.pachi_see_get_flag |= (1<<(i+SEE_FLAG_MAX));  // 捕まえたフラグ
      }
    }
  }
}

////////////////////////////////
//////// 言語
////////////////////////////////
/**
 * @brief		テキストバージョンフラグ設定
 * @param		monsno		ポケモン番号
 * @param		lang			国コード
 */
void ZukanData::SetZukanTextVersionUp( MonsNo monsno, u32 lang )
{
	// 国コード最大 or 欠番コード
	if( (lang > 10) || (lang == 6) || (lang == 0) ){
    GFL_ASSERT_MSG( 0, "lang err %d\n", lang ); //@check
		return;
	}

	// 言語６番が欠番なので、スペイン語以降は-1
	if( lang >= POKEMON_LANG_SPAIN ){
		lang -= 1;
	}

	this->SetBit( m_data.TextVersionUp, (monsno-1)*TEXTVER_VER_MAX+(lang-1), TEXTVER_ARRAY_LEN );
}

/**
 * @brief		テキストバージョンフラグリセット
 * @param		monsno		ポケモン番号
 * @param		lang			国コード
 */
void ZukanData::ResetZukanTextVersionUp( MonsNo monsno, u32 lang )
{
	// 国コード最大 or 欠番コード
	if( (lang > 10) || (lang == 6) || (lang == 0) ){
    GFL_ASSERT_MSG( 0, "lang err %d\n", lang ); //@check
		return;
	}

	// 言語６番が欠番なので、スペイン語以降は-1
	if( lang >= POKEMON_LANG_SPAIN ){
		lang -= 1;
	}

	this->ResetBit( m_data.TextVersionUp, (monsno-1)*TEXTVER_VER_MAX+(lang-1), TEXTVER_ARRAY_LEN );
}

////////////////////////////////
//////// 完成度
////////////////////////////////
/**
 * @brief		全国図鑑完成に必要なポケモンかチェック
 * @param		monsno		ポケモン番号
 * @retval	"true = 必要"
 * @retval	"false = それ以外"
 */
bool ZukanData::CheckZenkokuCompMonsno( MonsNo monsno ) const
{
  int i;
  // チェック除外ポケモンチェック
  for( i=0; i<GFL_NELEMS(ZENKOKU_NOT_CHECK_MONSNO); i++ ){
    if( ZENKOKU_NOT_CHECK_MONSNO[i] == monsno ){
      return false;
    }
  }
  return true;
}

/**
 * @brief		地方図鑑完成に必要なポケモンかチェック
 * @param		monsno		ポケモン番号
 * @retval	"true = 必要"
 * @retval	"false = それ以外"
 */
bool ZukanData::CheckLocalCompMonsno( MonsNo monsno, PokeTool::ExtendData::LocalArea local_area ) const
{
  int i;

  u16 table_num;
  const u16 *table =  this->GetLocalNotCheckMonsNoTable( local_area, &table_num );

  // チェック除外ポケモンチェック
  for( i=0; i<table_num; i++ ){
    if( table[i] == monsno ){
      return false;
    }
  }
  return true;
}

/**
 * @brief 地方のチェックしないポケモンテーブルを取得
 *
 * @param local_area  地方
 * @param table_num テーブルの個数
 *
 * @return  テーブル
 */
const u16 * ZukanData::GetLocalNotCheckMonsNoTable( PokeTool::ExtendData::LocalArea local_area, u16 *table_num ) const
{
  switch ( local_area )
  {
  case PokeTool::ExtendData::LOCAL_AREA_A:
    *table_num  = GFL_NELEMS(LOCAL_A_NOT_CHECK_MONSNO);
    return LOCAL_A_NOT_CHECK_MONSNO;
  }

  *table_num = 0;
  return NULL;
}

////////////////////////////////
//////// テーブルアクセス
////////////////////////////////
/**
 * @brief		フォルムテーブル位置を取得
 * @param		mons			ポケモン番号
 * @retval	"-1 != フォルムテーブル位置"
 * @retval	"-1 = 別フォルムがないポケモン"
 */
s32 ZukanData::GetPokeFormTablePos( MonsNo mons ) const
{
  for(s32 i=0; i<GFL_NELEMS(FormTable); ++i)
  {
    if(FormTable[i].monsno == mons)
    {
      return i;
    }
  }
  return -1;
}

/**
 * @brief		メガシンカテーブル位置を取得
 * @param		mons			ポケモン番号
 * @retval	"-1 != メガシンカテーブル位置"
 * @retval	"-1 = メガシンカがないポケモン"
 */
s32 ZukanData::GetPokeMegaTablePos( MonsNo mons ) const
{
  u16 mons_u16 = static_cast<u16>(mons);

  for(s32 i=0; i<GFL_NELEMS(MegaTable); ++i)
  {
    if(MegaTable[i] == mons_u16)
    {
      return i;
    }
  }
  return -1;
}

// MonsNo, FormNoを一列に並べたとき、何番目(0始まり)に来るか？
//
// monsNo=1のフォルム0
// monsNo=2のフォルム0
// ...
// monsNo=MONSNO_ENDのフォルム0
// monsNo=3のフォルム1
// monsNo=6のフォルム1
// monsNo=6のフォルム2
// monsNo=9のフォルム1
// ...
// monsNo=フォルム違いのある最後のモンスナンバーの最後のフォルムナンバー
// という順番に並べたとき、monsNo, formNoは何番目(0始まり)に来るか？
//
// 不正なmonsNo, formNoのときは MONS_FORM_INDEX_INVALID を返す。
//
// 例：monsNo=6, formNo=2だったら、(MONSNO_END-1)+(1)+(2)番を返す。
u16 ZukanData::GetMonsFormIndex(MonsNo monsNo, FormNo formNo) const  // この関数が不正値MONS_FORM_INDEX_INVALIDを返したときも、呼び出し元は正常に動くようにして下さい。
{
  u16 index = MONS_FORM_INDEX_INVALID;
  if(1<=monsNo && monsNo<=MONSNO_END)
  {
    if(formNo == 0)
    {
      index = monsNo -1;
    }
    else
    {
      u16 count = MONSNO_END -1;
      for(u32 i=0; i<GFL_NELEMS(FormTable); ++i)
      {
        if(FormTable[i].monsno == monsNo)
        {
          if(formNo < FormTable[i].form_max)
          {
            index = count +formNo;
          }
          //else                                                                 // @fix NMCat[416] コフキムシをタマゴ孵化するとはまる
          //{                                                                    // コフキムシ、コフーライはフォルム数以上のフォルム番号が来るので、ASSERTに掛からないようにする。
          //  GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。formNoが不正。  // MONS_FORM_INDEX_INVALIDを返しても呼び出し元で正常に動くようにしてもらっているので、
          //}                                                                    // フォルム番号が異常なことを許すことにする。ASSERTをコメントアウトした。                        
          break;
        }
        else
        {
          count += FormTable[i].form_max -1;
        }
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。monsNoが不正。
  }
  return index;
} 

////////////////////////////////
//////// ZukanNotSaveData関連
////////////////////////////////
// 初回のSetPokeGet時に呼んで、PokeRegisterShowを修正する。初回のSetPokeGet時のseeフラグを渡す。
void ZukanData::ModifyPokeRegisterShowBySetPokeGet( MonsNo monsno, bool seeFlag )
{
  u16 l_monsNo = static_cast<u16>(monsno);
  
  bool registerShow = false;

  // 図鑑登録画面を呼んだがまだPokeGetフラグを立てていないリストから消す
  for(s8 i=0; i<m_notSaveData.logNumForNotPokeGet; ++i)
  {
    if( m_notSaveData.logNotPokeGetMonsNo[i] == l_monsNo )
    {
      registerShow = true;
      for(s8 j=i; j<m_notSaveData.logNumForNotPokeGet-1; ++j)
      {
        m_notSaveData.logNotPokeGetMonsNo[j] = m_notSaveData.logNotPokeGetMonsNo[j+1];
      }
      --m_notSaveData.logNumForNotPokeGet;
      break;
    }
  }

  // PokeGetフラグを立てたがまだ図鑑登録画面を呼んでいないリストに入れる
  if(!registerShow)
  {
    // 既にリストに入っているなら、何もしない
    bool inList = false;
    for(s8 i=0; i<m_notSaveData.logNumForNotPokeRegisterShow; ++i)
    {
      if( m_notSaveData.logNotPokeRegisterShowMonsNo[i] == l_monsNo )
      {
        inList = true;
        break;
      }
    }
    // まだリストに入っていないなら、リストに入れる 
    if(!inList)
    {
      s8 i_start = m_notSaveData.logNumForNotPokeRegisterShow;
      ++m_notSaveData.logNumForNotPokeRegisterShow;
      if(m_notSaveData.logNumForNotPokeRegisterShow > ZukanNotSaveData::LOG_NUM_MAX)
      {
        --i_start;
        m_notSaveData.logNumForNotPokeRegisterShow = ZukanNotSaveData::LOG_NUM_MAX;
      }
      for(s8 i=i_start; i>0; --i)
      {
        m_notSaveData.logNotPokeRegisterShowMonsNo[i]  = m_notSaveData.logNotPokeRegisterShowMonsNo[i-1];
        if( ( m_notSaveData.logNotPokeRegisterShowSeeFlag & (1<<(i-1)) ) != 0 )
        {
          m_notSaveData.logNotPokeRegisterShowSeeFlag |= 1<<i;  // フラグを立てる
        }
        else
        {
          m_notSaveData.logNotPokeRegisterShowSeeFlag &= ~(1<<i);  // フラグを落とす
        }
      }
      m_notSaveData.logNotPokeRegisterShowMonsNo[0]  = l_monsNo;
      if(seeFlag)
      {
        m_notSaveData.logNotPokeRegisterShowSeeFlag |= 1<<0;  // フラグを立てる
      }
      else
      {
        m_notSaveData.logNotPokeRegisterShowSeeFlag &= ~(1<<0);  // フラグを落とす
      }
    }
  }

#if PM_DEBUG
  this->PrintNotSaveData("ModifyPokeRegisterShowBySetPokeGet\n");
#endif  // PM_DEBUG
}

#if PM_DEBUG
void ZukanData::PrintNotSaveData(const char* mes) const
{
  GFL_PRINT("%s", mes);
  GFL_PRINT("logNotPokeRegisterShowMonsNo\n");
  for(s8 i=0; i<m_notSaveData.logNumForNotPokeRegisterShow; ++i)
  {
    GFL_PRINT("%d(%d), ", m_notSaveData.logNotPokeRegisterShowMonsNo[i], (m_notSaveData.logNotPokeRegisterShowSeeFlag&(1<<i))!=0 ); 
  }
  GFL_PRINT("\n");
  GFL_PRINT("logNotPokeGetMonsNo\n");
  for(s8 i=0; i<m_notSaveData.logNumForNotPokeGet; ++i)
  {
    GFL_PRINT("%d, ", m_notSaveData.logNotPokeGetMonsNo[i]); 
  }
  GFL_PRINT("\n");
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Savedata)




#if 0
フォルム数が増え過ぎ、このテーブルのつくり方では限界が来たので、
整然と番号順に並べたテーブルをつくることにした。

// フォルムテーブル  // monsno_def.hのFORMNO_xxxxを参考に作成。【OTHER_FORM_MAXは参考にしないように！】
static const struct
{
  u16 monsno; //対応ポケモン
  u16 form_max; //フォルム最大
  bool is_mega; //メガフォルム
}FormTable[] =
{
	// ポケモン番号, フォルム数
	{ MONSNO_ANNOON, FORMNO_ANNOON_MAX, false},						// アンノーン
	{ MONSNO_DEOKISISU, FORMNO_DEOKISISU_MAX, false},			// デオキシス
	{ MONSNO_SHEIMI, FORMNO_SHEIMI_MAX, false},						// シェイミ  
	{ MONSNO_GIRATHINA, FORMNO_GIRATHINA_MAX, false},			// ギラティナ
	{ MONSNO_ROTOMU, FORMNO_ROTOMU_MAX, false},						// ロトム    
	{ MONSNO_KARANAKUSI, FORMNO_KARANAKUSI_MAX, false},		// カラナクシ
	{ MONSNO_TORITODON, FORMNO_TORITODON_MAX, false},			// トリトドン
	{ MONSNO_MINOMUTTI, FORMNO_MINOMUTTI_MAX, false},			// ミノムッチ
	{ MONSNO_MINOMADAMU, FORMNO_MINOMADAMU_MAX, false},		// ミノマダム
	{ MONSNO_POWARUN, FORMNO_POWARUN_MAX, false},					// ポワルン ※特殊 
	{ MONSNO_THERIMU, FORMNO_THERIMU_MAX, false},					// チェリム ※特殊
	{ MONSNO_SIKIZIKA, FORMNO_SIKIZIKA_MAX, false},				        // シキジカ ※新規
	{ MONSNO_MEBUKIZIKA, FORMNO_MEBUKIZIKA_MAX, false},				        // アントレス ※新規  // メブキジカ
	{ MONSNO_MEROETTA, FORMNO_MEROETTA_MAX, false},								// メロディア ※新規 特殊  // メロエッタ
	{ MONSNO_HIHIDARUMA, FORMNO_HIHIDARUMA_MAX, false},		// ヒヒダルマ ※新規
	{ MONSNO_BASURAO, FORMNO_BASURAO_MAX, false},					// バスラオ ※新規

  //スワン新規フォルムテーブル
	{ MONSNO_KYUREMU, FORMNO_KYUREMU_MAX, false},					// キュレム ※スワン新規
	{ MONSNO_KERUDHIO, FORMNO_KERUDHIO_MAX, false},				// ケルディオ ※スワン新規
	{ MONSNO_BORUTOROSU, FORMNO_BORUTOROSU_MAX, false},	  // ボルトロス ※スワン新規
	{ MONSNO_TORUNEROSU, FORMNO_TORUNEROSU_MAX, false},		// トルネロス ※スワン新規
	{ MONSNO_RANDOROSU, FORMNO_RANDOROSU_MAX, false},		  // ランドロス ※スワン新規

  //kujira新規フォルムテーブル
  { MONSNO_BIBIYON   , FORMNO_BIBIYON_MAX   , false},  //666 ビビヨン     MONSNO_BIBIYON    ※kujira新規
  { MONSNO_HURABEBE  , FORMNO_HURABEBE_MAX  , false},  //669 フラベベ     MONSNO_HURABEBE
  { MONSNO_HURAETTE  , FORMNO_HURAETTE_MAX  , false},  //670 フラエッテ   MONSNO_HURAETTE
  { MONSNO_HURAAJESU , FORMNO_HURAAJESU_MAX , false},  //671 フラージェス MONSNO_HURAAJESU
  { MONSNO_BAKETTYA  , FORMNO_BAKETTYA_MAX  , false},  //710 バケッチャ   MONSNO_BAKETTYA
  { MONSNO_PANPUZIN  , FORMNO_PANPUZIN_MAX  , false},  //711 パンプジン   MONSNO_PANPUZIN
  { MONSNO_GIRUGARUDO, FORMNO_GIRUGARUDO_MAX, false},  //681 ギルガルド   MONSNO_GIRUGARUDO ※特殊
  { MONSNO_ZERUNEASU , FORMNO_ZERUNEASU_MAX , false},  //716 ゼルネアス   MONSNO_ZERUNEASU
  // { MONSNO_NYAONIKUSU, FORMNO_NYAONIKUSU_MAX, false}, //ニャオニクス 性別違いをFormで表現している ->ニャオニクスはフォルム登録させない ->nijiでフォルムの扱いを簡単にするためにフォルム登録することにした。

  //メガ系新規追加
  { MONSNO_HUSIGIBANA, FORMNO_HUSIGIBANA_MAX, true}, //フシギバナ
  { MONSNO_RIZAADON,   FORMNO_RIZAADON_MAX,   true}, //リザードン
  { MONSNO_KAMEKKUSU,  FORMNO_KAMEKKUSU_MAX,  true}, //カメックス
  { MONSNO_HUUDHIN,    FORMNO_HUUDHIN_MAX,    true}, //フーディン
  { MONSNO_GENGAA,     FORMNO_GENGAA_MAX,     true}, //ゲンガー
  { MONSNO_GARUURA,    FORMNO_GARUURA_MAX,    true}, //ガルーラ
  { MONSNO_KAIROSU,    FORMNO_KAIROSU_MAX,    true}, //カイロス
  { MONSNO_GYARADOSU,  FORMNO_GYARADOSU_MAX,  true}, //ギャラドス
  { MONSNO_PUTERA,     FORMNO_PUTERA_MAX,     true}, //プテラ
  { MONSNO_MYUUTUU,    FORMNO_MYUUTUU_MAX,    true}, //ミュウツー
  { MONSNO_DENRYUU,    FORMNO_DENRYUU_MAX,    true}, //デンリュウ
  { MONSNO_HASSAMU,    FORMNO_HASSAMU_MAX,    true}, //ハッサム
  { MONSNO_HERAKUROSU, FORMNO_HERAKUROSU_MAX, true}, //ヘラクロス
  { MONSNO_HERUGAA,    FORMNO_HERUGAA_MAX,    true}, //ヘルガー
  { MONSNO_BANGIRASU,  FORMNO_BANGIRASU_MAX,  true}, //バンギラス
  { MONSNO_BASYAAMO,   FORMNO_BASYAAMO_MAX,   true}, //バシャーモ
  { MONSNO_SAANAITO,   FORMNO_SAANAITO_MAX,   true}, //サーナイト
  { MONSNO_KUTIITO,    FORMNO_KUTIITO_MAX,    true}, //クチート
  { MONSNO_BOSUGODORA, FORMNO_BOSUGODORA_MAX, true}, //ボスコドラ
  { MONSNO_TYAAREMU,   FORMNO_TYAAREMU_MAX,   true}, //チャーレム
  { MONSNO_RAIBORUTO,  FORMNO_RAIBORUTO_MAX,  true}, //ライボルト
  { MONSNO_ZYUPETTA,   FORMNO_ZYUPETTA_MAX,   true}, //ジュペッタ
  { MONSNO_ABUSORU,    FORMNO_ABUSORU_MAX,    true}, //アブソル
  { MONSNO_RATHIASU,   FORMNO_RATHIASU_MAX,   true}, //ラティアス
  { MONSNO_RATHIOSU,   FORMNO_RATHIOSU_MAX,   true}, //ラティオス
  { MONSNO_GABURIASU,  FORMNO_GABURIASU_MAX,  true}, //ガブリアス
  { MONSNO_RUKARIO,    FORMNO_RUKARIO_MAX,    true}, //ルカリオ
  { MONSNO_YUKINOOO,   FORMNO_YUKINOOO_MAX,   true}, //ユキノオー

  //sango追加フォルム
  { MONSNO_PIKATYUU, FORMNO_PIKATYUU_MAX, false},    //025 ピカチュウ(ピカチュウの1から5を登録するときは、6にして登録するので0と6だけでいいが、全領域用意しておく)
  { MONSNO_HUUPA, FORMNO_HUUPA_MAX, false},          //720 フーパ
  //sango追加メガ
  { MONSNO_SUPIAA, FORMNO_SUPIAA_MAX, true},         //015 スピアー 
  { MONSNO_PIZYOTTO, FORMNO_PIZYOTTO_MAX, true},     //018 ピジョット
  { MONSNO_YADORAN, FORMNO_YADORAN_MAX, true},       //080 ヤドラン
  { MONSNO_HAGANEERU, FORMNO_HAGANEERU_MAX, true},   //208 ハガネール
  { MONSNO_ZYUKAIN, FORMNO_ZYUKAIN_MAX, true},       //254 ジュカイン
  { MONSNO_RAGURAAZI, FORMNO_RAGURAAZI_MAX, true},   //260 ラグラージ
  { MONSNO_YAMIRAMI, FORMNO_YAMIRAMI_MAX, true},     //302 ヤミラミ
  { MONSNO_SAMEHADAA, FORMNO_SAMEHADAA_MAX, true},   //319 サメハダー
  { MONSNO_BAKUUDA, FORMNO_BAKUUDA_MAX, true},       //323 バクーダ
  { MONSNO_TIRUTARISU, FORMNO_TIRUTARISU_MAX, true}, //334 チルタリス
  { MONSNO_ONIGOORI, FORMNO_ONIGOORI_MAX, true},     //362 オニゴーリ
  { MONSNO_BOOMANDA, FORMNO_BOOMANDA_MAX, true},     //373 ボーマンダ
  { MONSNO_METAGUROSU, FORMNO_METAGUROSU_MAX, true}, //376 メタグロス
  { MONSNO_REKKUUZA, FORMNO_REKKUUZA_MAX, true},    //384 レックウザ
  { MONSNO_MIMIROPPU, FORMNO_MIMIROPPU_MAX, true},   //428 ミミロップ
  { MONSNO_ERUREIDO, FORMNO_ERUREIDO_MAX, true},     //475 エルレイド
  { MONSNO_TABUNNE, FORMNO_TABUNNE_MAX, true},       //531 タブンネ
  { MONSNO_DHIANSII, FORMNO_DHIANSII_MAX, true},     //719 ディアンシー
  //sango追加メガ類似
  { MONSNO_KAIOOGA, FORMNO_KAIOOGA_MAX, false},      //382 カイオーガ
  { MONSNO_GURAADON, FORMNO_GURAADON_MAX, false},    //383 グラードン

  //sangoでセーブするようになったポケモン
  { MONSNO_ARUSEUSU, FORMNO_ARUSEUSU_MAX, false },      //493 アルセウス
  { MONSNO_GENOSEKUTO, FORMNO_GENOSEKUTO_MAX, false },  //649 ゲノセクト
  { MONSNO_TORIMIAN, FORMNO_TORIMIAN_MAX, false },      //676 トリミアン

  //niji追加アローラ地方の姿
  { MONSNO_KORATTA  , FORMNO_KORATTA_MAX  , false },      // コラッタ
  { MONSNO_RATTA    , FORMNO_RATTA_MAX    , false },      // ラッタ
  { MONSNO_RAITYUU  , FORMNO_RAITYUU_MAX  , false },      // ライチュウ
  { MONSNO_SANDO    , FORMNO_SANDO_MAX    , false },      // サンド
  { MONSNO_SANDOPAN , FORMNO_SANDOPAN_MAX , false },      // サンドパン
  { MONSNO_ROKON    , FORMNO_ROKON_MAX    , false },      // ロコン
  { MONSNO_KYUUKON  , FORMNO_KYUUKON_MAX  , false },      // キュウコン
  { MONSNO_NYAASU   , FORMNO_NYAASU_MAX   , false },      // ニャース
  { MONSNO_PERUSIAN , FORMNO_PERUSIAN_MAX , false },      // ペルシアン
  { MONSNO_ISITUBUTE, FORMNO_ISITUBUTE_MAX, false },      // イシツブテ
  { MONSNO_GOROON   , FORMNO_GOROON_MAX   , false },      // ゴローン
  { MONSNO_GOROONYA , FORMNO_GOROONYA_MAX , false },      // ゴローニャ
  { MONSNO_BETOBETAA, FORMNO_BETOBETAA_MAX, false },      // ベトベター
  { MONSNO_BETOBETON, FORMNO_BETOBETON_MAX, false },      // ベトベトン
  { MONSNO_NASSII   , FORMNO_NASSII_MAX   , false },      // ナッシー
  { MONSNO_GARAGARA , FORMNO_GARAGARA_MAX , false },      // ガラガラ
  //niji追加フォルム
  { MONSNO_ZIGARUDE , FORMNO_ZIGARUDE_MAX , false },      // ジガルデ
  { MONSNO_ODORIDORI, FORMNO_ODORIDORI_MAX, false },      // オドリドリ
  { MONSNO_YOWASI   , FORMNO_YOWASI_MAX   , false },      // ヨワシ
  { MONSNO_METENO   , FORMNO_METENO_MAX   , false },      // メテノ
  { MONSNO_OOKAMI2  , FORMNO_OOKAMI2_MAX  , false },      // オオカミ２

  //nijiでセーブするようになったポケモン
  { MONSNO_GEKKOUGA  , FORMNO_GEKKOUGA_MAX  , false}, //ゲッコウガ
  { MONSNO_NYAONIKUSU, FORMNO_NYAONIKUSU_MAX, false}, //ニャオニクス 性別違いをFormで表現している

  // 終了
	{ 0, 0 },
};
#endif

