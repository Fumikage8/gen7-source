#if !defined(GFL2_FSTOOLDEFINE_H_INCLUDED)
#define GFL2_FSTOOLDEFINE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsToolDefine.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステムにおいてツールと共通使用している定義
 */
//==============================================================================

// ツールで使用するときは
// GFL_FS_TOOL_DEFINE_USE_IN_TOOL
// を定義しておくといいです。




#if !defined(GFL_FS_TOOL_DEFINE_USE_IN_TOOL)

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#endif  // GFL_FS_TOOL_DEFINE_USE_IN_TOOL




#if !defined(GFL_FS_TOOL_DEFINE_USE_IN_TOOL)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)

#endif  // GFL_FS_TOOL_DEFINE_USE_IN_TOOL




//==============================================================================
/**
 @class     ToolDefine
 @brief     ファイルシステムで使う定義
 */
//==============================================================================
class ToolDefine
{
  // GFL_FORBID_COPY_AND_ASSIGNなし、デストラクタなし

public:
  //! @brief  garc言語コード
  //! @note   GFArchiver.exeで用いるファイル名の末尾のサフィックスと同じ。
  //!
  //! @note  言語コードはこの他に以下のものがあります。
  //!        pm_versionの言語コード                     (ポケモンに埋め込む言語コード)    ??_project/prog/include/pm_version.h          #define LANG_NULL  #define LANG_JAPAN  #define LANG_ENGLISH  ...
  //!        pm_languageの言語コード                    (メッセージスタジオの言語コード)  ??_project/prog/include/system/pm_language.h  enum MSGLANGID{ MSGLANGID_KANA, MSGLANGID_KANJI, MSGLANGID_ENGLISH, ... };
  //!        nn::cfg::CTR::CfgLanguageCodeの言語コード  (CTR(3DS)本体の言語コード)
  //!
  //! @note  nijiで言語コードは次のようになりました。
  //!        PokemonVersionの言語コード                    (ポケモンに埋め込む言語コード)    R:/home/momiji/git_program/niji_project/prog/System/include/PokemonVersion.h   #define POKEMON_LANG_NULL  #define POKEMON_LANG_JAPAN  ...
  //!        PokemonLanguageのMSGLANGIDの言語コード        (メッセージスタジオの言語コード)  R:/home/momiji/git_program/niji_project/prog/System/include/PokemonLanguage.h  enum MSGLANGID{ MSGLANGID_KANA, MSGLANGID_KANJI, MSGLANGID_ENGLISH, ... };
  //!        PokemonLanguageのCfgLanguageCodeの言語コード  (CTR(3DS)本体の言語コード)        R:/home/momiji/git_program/niji_project/prog/System/include/PokemonLanguage.h  enum CfgLanguageCode{ LANGUAGE_JAPAN = nn::cfg::CTR::CFG_LANGUAGE_JAPANESE, ... };
  //!
  //! @note  ArcLangTypeはPokemonVersionの言語コード(ポケモンに埋め込む言語コード)(#define POKEMON_LANG_NULL  #define POKEMON_LANG_JAPAN  ...)に一致させている。
  class ArcLangType
  {
  public:
    enum Tag
    {
      DEF  =   0,   //!< 専用言語が存在しない場合のデフォルト  // NULL
      JPN  =   1,   //!< 日本語                                // JAPAN
      USA  =   2,   //!< 英語                                  // ENGLISH
      FRA  =   3,   //!< フランス語                            // FRANCE
      ITA  =   4,   //!< イタリア語                            // ITALY
      DEU  =   5,   //!< ドイツ語                              // GERMANY
      ESP  =   7,   //!< スペイン語                            // SPAIN
      KOR  =   8,   //!< 韓国語                                // KOREA
      CHN  =   9,   //!< 簡体字（中国）                        // CHINA
      TWN  =  10,   //!< 繁体字（台湾、香港）                  // TAIWAN

      LANG_NUM      =  11,  //!< 言語数。言語番号の最大値+1。「garcのソースを記録したファイル」にアクセスするarcsrc::Accessorクラスだけで使える定義。

      ALL_LANG      = 254,  //!< AsyncFileManagerクラスのリクエストとArcFileクラスのSetArcLang関数に渡すときにだけ使える定義
      DEFAULT_LANG  = 255,  //!< AsyncFileManagerクラスのリクエストに渡すときにだけ使える定義
    };
  };
};




#if !defined(GFL_FS_TOOL_DEFINE_USE_IN_TOOL)

GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL_FS_TOOL_DEFINE_USE_IN_TOOL




#endif  // GFL2_FSTOOLDEFINE_H_INCLUDED
