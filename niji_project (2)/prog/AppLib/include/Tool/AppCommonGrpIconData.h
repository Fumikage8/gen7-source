//=============================================================================
/**
 * @file    AppCommonGrpIconData.cpp
 * @brief   共通グラフィックスアイコンデータ
 * @author  Yuto Uchida
 * @date    15.06.16
 */
//=============================================================================
#if !defined( APP_COMMON_GRP_ICON_ICON_DATA_H_INCLUDED )
#define APP_COMMON_GRP_ICON_ICON_DATA_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

#include <pml/include/poketype_def.h>
#include <pml/include/pokepara/pml_PokemonParamType.h>

#include "AppLib/include/Tool/AppToolBaseGrpFont.h"

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Tool)

  class CommonGrpIconUVDataTable;

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(AppLib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppUtilFileControlState;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class AppCommonGrpIconData : public AppToolBaseGrpFont
  {
    GFL_FORBID_COPY_AND_ASSIGN( AppCommonGrpIconData );
  public:
    //-----------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-----------------------------------------------------------------------------
    AppCommonGrpIconData();

    //-----------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-----------------------------------------------------------------------------
    ~AppCommonGrpIconData();

    //-----------------------------------------------------------------------------
    /**
     * @brief 初期化
     */
    //-----------------------------------------------------------------------------
    bool Initialize( app::util::Heap* pUtilHeap, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

    //-----------------------------------------------------------------------------
    /**
     * @brief   ポケモン状態異常のid指定してアイコンテクスチャをペインに貼り付け
     *
     * @param   sick      ポケモンの状態異常
     * @param   pane      ペイン
     */
    //-----------------------------------------------------------------------------
    bool ReplacePaneTextureByPokeSickIcon( pml::pokepara::Sick sick, gfl2::lyt::LytPicture * pane );

    //-----------------------------------------------------------------------------
    /**
     * @brief   ポケモンひんし状態アイコンテクスチャをペインに貼り付け
     *
     * @param   pane      ペイン
     */
    //-----------------------------------------------------------------------------
    bool ReplacePaneTextureByPokeHinshiIcon( gfl2::lyt::LytPicture * pane );

    //-----------------------------------------------------------------------------
    /**
     * @brief   ポケモンどくどく状態アイコンテクスチャをペインに貼り付け
     *
     * @param   pane      ペイン
     */
    //-----------------------------------------------------------------------------
    bool ReplacePaneTextureByPokeDokudokuIcon( gfl2::lyt::LytPicture * pane );

    //-----------------------------------------------------------------------------
    /**
     * @brief   ポケモンのわざタイプのid指定してアイコンテクスチャをペインに貼り付け
     *
     * @param   type      ポケモンの技タイプ
     * @param   pane      ペイン
     */
    //-----------------------------------------------------------------------------
    bool ReplacePaneTextureByWazaTypeIcon( PokeType_tag type, gfl2::lyt::LytPicture * pane );

    //-----------------------------------------------------------------------------
    /**
     * @brief   ポケモンのポケルス状態アイコンテクスチャをペインに貼り付け
     *
     * @param   pane      テクスチャー差し替え対象のペイン
     */
    //-----------------------------------------------------------------------------
    bool ReplacePaneTextureByPokerusIcon( gfl2::lyt::LytPicture * pane );

    //-----------------------------------------------------------------------------
    /**
     * @brief   言語のアイコンテクスチャをペインに貼り付け
     *
     * @param   u32        言語タイプ
     * @param   pane      テクスチャー差し替え対象のペイン
     * @note
     *          System/include/PokemonVersion.hに定義している下記のを指定
     *
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
    bool ReplacePaneTextureByLangIcon( const u32 langType, gfl2::lyt::LytPicture * pane );

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
    bool ReplacePaneTextureByPokeParamIcon( const pml::pokepara::PowerID powerId, gfl2::lyt::LytPicture * pane );

    //-----------------------------------------------------------------------------
    /**
     * @brief   newのアイコンテクスチャをペインに貼り付け
     *
     * @param   pane      テクスチャー差し替え対象のペイン
     * @note
     *          newアイコン表示
     */
    //-----------------------------------------------------------------------------
    bool ReplacePaneTextureByNewIcon( gfl2::lyt::LytPicture * pane );

  private:
    void _Clear()
    {
    }
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // APP_COMMON_GRP_ICON_ICON_DATA_H_INCLUDED
