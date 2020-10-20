//=============================================================================
/**
 * @file    AppCommonGrpIconData.cpp
 * @brief   共通グラフィックスアイコンデータ
 * @author  Yuto Uchida
 * @date    15.06.16
 */
//=============================================================================
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

//  gfl2のインクルード
#include <system/include/PokemonVersion.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  AppCommonGrpIconData::AppCommonGrpIconData()
  {
    _Clear();
  }

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  AppCommonGrpIconData::~AppCommonGrpIconData()
  {
  }

  //  初期化
  bool AppCommonGrpIconData::Initialize( app::util::Heap* pUtilHeap, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
  {
    GFL_ASSERT( pUtilHeap );

    return  AppToolBaseGrpFont::Initialize( pUtilHeap, AppToolBaseGrpFont::FONT_TYPE_COMMON, lang );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   ポケモンひんし状態アイコンテクスチャをペインに貼り付け
    *
    * @param   pane      ペイン
    */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByPokeHinshiIcon( gfl2::lyt::LytPicture * pane )
  {
    return  AppToolBaseGrpFont::ReplacePaneTexture( CommonGrpIconUVDataTable::TYPE_DEAD_ICON, pane, 0, 0 );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   ポケモンどくどく状態アイコンテクスチャをペインに貼り付け
    *
    * @param   pane      ペイン
    */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByPokeDokudokuIcon( gfl2::lyt::LytPicture * pane )
  {
    return  AppToolBaseGrpFont::ReplacePaneTexture( CommonGrpIconUVDataTable::TYPE_SICK_ICON_06_DOKUDOKU, pane, 0, 0 );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   ポケモン状態異常のid指定してアイコンテクスチャをペインに貼り付け
    *
    * @param   uvID      ポケモンの状態異常
    * @param   pane      ペイン
    */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByPokeSickIcon( pml::pokepara::Sick sick, gfl2::lyt::LytPicture * pane )
  {
    //  定義しているpml::pokepara::Sickとuvデータindexの対応テーブル
    static const s32  aIdTable[][ 2 ]  =
    {
      ///< まひ
      { CommonGrpIconUVDataTable::TYPE_SICK_ICON_01_MAHI, pml::pokepara::SICK_MAHI },
      ///< ねむり
      { CommonGrpIconUVDataTable::TYPE_SICK_ICON_02_NEMURI, pml::pokepara::SICK_NEMURI },
      ///< こおり
      { CommonGrpIconUVDataTable::TYPE_SICK_ICON_03_KOORI, pml::pokepara::SICK_KOORI },
      ///< やけど
      { CommonGrpIconUVDataTable::TYPE_SICK_ICON_04_YAKEDO, pml::pokepara::SICK_YAKEDO },
       ///< どく
      { CommonGrpIconUVDataTable::TYPE_SICK_ICON_05_DOKU, pml::pokepara::SICK_DOKU },

      { -1, -1 }
    };

    for( u32 i = 0; aIdTable[ i ][ 0 ] != -1; ++i )
    {
      if( aIdTable[ i ][ 1 ] == (s32)sick )
      {
        return  AppToolBaseGrpFont::ReplacePaneTexture( aIdTable[ i ][ 0 ], pane, 0, 0 );
      }
    }

    return  false;
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   ポケモンのわざタイプのid指定してアイコンテクスチャをペインに貼り付け
    *
    * @param   uvID      ポケモンのタイプ
    * @param   pane      ペイン
    */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByWazaTypeIcon( PokeType_tag type, gfl2::lyt::LytPicture * pane )
  {
    //  PokeType_tag定義との対応テーブル
    static const s32  aIdTable[][ 2 ]  =
    {
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_00_NORMAL,   POKETYPE_NORMAL  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_01_KAKUTOU,  POKETYPE_KAKUTOU  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_02_HIKOU,    POKETYPE_HIKOU  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_03_DOKU,     POKETYPE_DOKU  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_04_JIMEN,    POKETYPE_JIMEN  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_05_IWA,      POKETYPE_IWA  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_06_MUSHI,    POKETYPE_MUSHI  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_07_GHOST,    POKETYPE_GHOST  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_08_HAGANE,   POKETYPE_HAGANE  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_09_HONOO,    POKETYPE_HONOO  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_10_MIZU,     POKETYPE_MIZU  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_11_KUSA,     POKETYPE_KUSA  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_12_DENKI,    POKETYPE_DENKI  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_13_ESPER,    POKETYPE_ESPER  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_14_KOORI,    POKETYPE_KOORI  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_15_DRAGON,   POKETYPE_DRAGON  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_16_AKU,      POKETYPE_AKU  },
      { CommonGrpIconUVDataTable::TYPE_TYPE_ICON_17_YOUSEI,   POKETYPE_FAIRY  },
      { -1, -1 }
    };

    for( u32 i = 0; aIdTable[ i ][ 0 ] != -1; ++i )
    {
      if( aIdTable[ i ][ 1 ] == (s32)type )
      {
        return  AppToolBaseGrpFont::ReplacePaneTexture( aIdTable[ i ][ 0 ], pane, 0, 0 );
      }
    }

    return  false;
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   ポケモンのポケルス状態アイコンテクスチャをペインに貼り付け
    *
    * @param   pane      テクスチャー差し替え対象のペイン
    */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByPokerusIcon( gfl2::lyt::LytPicture * pane )
  {
    return  AppToolBaseGrpFont::ReplacePaneTexture( CommonGrpIconUVDataTable::TYPE_POKERUS_ICON, pane, 0, 0 );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   言語のアイコンテクスチャをペインに貼り付け
    *
    * @param   u32       言語タイプ
    * @param   pane      テクスチャー差し替え対象のペイン
    * @note
    *          System/include/PokemonVersion.hに定義している下記のを指定
    *
    *   #define POKEMON_LANG_NULL          0  ///<  言語コード：無効な値
    *   #define POKEMON_LANG_JAPAN         1  ///<  言語コード：日本        JPN
    *   #define POKEMON_LANG_ENGLISH       2  ///<  言語コード：英語        USA
    *   #define POKEMON_LANG_FRANCE        3  ///<  言語コード：フランス語  FRA
    *   #define POKEMON_LANG_ITALY         4  ///<  言語コード：イタリア語  ITA
    *   #define POKEMON_LANG_GERMANY       5  ///<  言語コード：ドイツ語    DEU
    *   #define POKEMON_LANG_SPAIN         7  ///<  言語コード：スペイン語  ESP
    *   #define POKEMON_LANG_KOREA         8  ///<  言語コード：韓国語      KOR
    *   #define POKEMON_LANG_CHINA         9  ///<  言語コード：簡体字（中国）      CHN
    *   #define POKEMON_LANG_TAIWAN        10 ///<  言語コード：繁体字（台湾、香港）    TWN
  */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByLangIcon( const u32 langType, gfl2::lyt::LytPicture * pane )
  {
    static const s32  aIdTable[][ 2 ] =
    {
      { CommonGrpIconUVDataTable::TYPE_JPN, POKEMON_LANG_JAPAN,   }, ///<  言語コード：日本        JPN
      { CommonGrpIconUVDataTable::TYPE_ENG, POKEMON_LANG_ENGLISH  }, ///<  言語コード：英語        USA
      { CommonGrpIconUVDataTable::TYPE_FRA, POKEMON_LANG_FRANCE   }, ///<  言語コード：フランス語  FRA
      { CommonGrpIconUVDataTable::TYPE_ITA, POKEMON_LANG_ITALY    }, ///<  言語コード：イタリア語  ITA
      { CommonGrpIconUVDataTable::TYPE_GER, POKEMON_LANG_GERMANY  }, ///<  言語コード：ドイツ語    DEU
      { CommonGrpIconUVDataTable::TYPE_SPA, POKEMON_LANG_SPAIN    }, ///<  言語コード：スペイン語  ESP
      { CommonGrpIconUVDataTable::TYPE_KOR, POKEMON_LANG_KOREA    }, ///<  言語コード：韓国語      KOR
      { CommonGrpIconUVDataTable::TYPE_CHS, POKEMON_LANG_CHINA    }, ///<  言語コード：簡体字（中国）      CHN
      { CommonGrpIconUVDataTable::TYPE_CHT, POKEMON_LANG_TAIWAN   }, ///<  言語コード：繁体字（台湾、香港）    TWN
      { -1, -1 }
    };

    for( u32 i = 0; aIdTable[ i ][ 0 ] != -1; ++i )
    {
      if( aIdTable[ i ][ 1 ] == (s32)langType )
      {
        return  AppToolBaseGrpFont::ReplacePaneTexture( aIdTable[ i ][ 0 ], pane, 0, 0 );
      }
    }

    return  false;
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   ポケモンパラメータ項目のアイコンテクスチャをペインに貼り付け
    *
    * @param   powerId   パラメータid
    * @param   pane      テクスチャー差し替え対象のペイン
    * @note
    *          レーダーチャートで表示するポケモンの下記パラメータのアイコン差し替え
    *          HP・こうげき・ぼうぎょ・とくこう・とくぼう
    */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByPokeParamIcon( const pml::pokepara::PowerID powerId, gfl2::lyt::LytPicture * pane )
  {
    static const s32  aIdTable[][ 2 ] =
    {
      { CommonGrpIconUVDataTable::TYPE_PARAMNAME_00, pml::pokepara::POWER_HP     }, ///<  HP
      { CommonGrpIconUVDataTable::TYPE_PARAMNAME_01, pml::pokepara::POWER_ATK    }, ///<  こうげき
      { CommonGrpIconUVDataTable::TYPE_PARAMNAME_02, pml::pokepara::POWER_DEF    }, ///<  防御
      { CommonGrpIconUVDataTable::TYPE_PARAMNAME_03, pml::pokepara::POWER_SPATK  }, ///<  とくこう
      { CommonGrpIconUVDataTable::TYPE_PARAMNAME_04, pml::pokepara::POWER_SPDEF  }, ///<  とくぼう
      { CommonGrpIconUVDataTable::TYPE_PARAMNAME_05, pml::pokepara::POWER_AGI    }, ///<  すばやさ
      { -1, -1 }
    };

    for( u32 i = 0; aIdTable[ i ][ 0 ] != -1; ++i )
    {
      if( aIdTable[ i ][ 1 ] == (s32)powerId )
      {
        return  AppToolBaseGrpFont::ReplacePaneTexture( aIdTable[ i ][ 0 ], pane, 0, 0 );
      }
    }
    
    return  false;
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   newのアイコンテクスチャをペインに貼り付け
    *
    * @param   pane      テクスチャー差し替え対象のペイン
    * @note
    *          newアイコン表示
    */
  //-----------------------------------------------------------------------------
  bool AppCommonGrpIconData::ReplacePaneTextureByNewIcon( gfl2::lyt::LytPicture * pane )
  {
    if( pane == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    return  AppToolBaseGrpFont::ReplacePaneTexture( CommonGrpIconUVDataTable::TYPE_EVENT_NEW_ICON_00, pane, 0, 0 );
  }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
