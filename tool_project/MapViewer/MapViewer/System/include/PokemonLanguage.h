//===================================================================
/**
 * @file PokemonLanguage.h
 * @brief 言語コード管理クラス＆ツール関数
 * @date  2012.11.17
 * @author  tamada@gamefreak.co.jp
 *
 * @note  kujiraでは言語を扱うのに3種類のコードがある
 * - PM_LANG                        従来のポケモン内言語コード
 * - nn::cfg::CTR::CfgLanguageCode  3DS本体設定言語コード
 * - savedata::config::MsgLangId    言語別メッセージ指定ID
 */
//===================================================================
#if !defined( PM_LANGUAGE_H_INCLUDED )
#define PM_LANGUAGE_H_INCLUDED

#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nn/cfg.h>
#endif

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(System)

//-----------------------------------------------------------------------------
/**
 * @brief 言語別メッセージ指定ID
 *
 * @note  表示メッセージを指定するために使用するID
 * @attention PM_LANG定義（PokemonVersion.hのLANG_～）とは違い、
 * @attention かな・漢字の判別も入っている。
 * @attention 言語の判別に使用しないこと。
 */
//-----------------------------------------------------------------------------
enum MSGLANGID{
  MSGLANGID_KANA = 0,
  MSGLANGID_KANJI,
  MSGLANGID_ENGLISH,
  MSGLANGID_FRENCH,
  MSGLANGID_ITALIAN,
  MSGLANGID_GERMAN,
  MSGLANGID_SPANISH,
  MSGLANGID_KOREAN,

  MSGLANGID_MAX,
};

//! CTRの言語コード定義
enum CfgLanguageCode
{
#if defined(GF_PLATFORM_CTR)
  LANGUAGE_JAPAN        = nn::cfg::CTR::CFG_LANGUAGE_JAPANESE,      //!< 日本語
  LANGUAGE_ENGLISH      = nn::cfg::CTR::CFG_LANGUAGE_ENGLISH,       //!< 英語
  LANGUAGE_FRANCE       = nn::cfg::CTR::CFG_LANGUAGE_FRENCH,        //!< フランス語
  LANGUAGE_GERMANY      = nn::cfg::CTR::CFG_LANGUAGE_GERMAN,        //!< ドイツ語
  LANGUAGE_ITALY        = nn::cfg::CTR::CFG_LANGUAGE_ITALIAN,       //!< イタリア語
  LANGUAGE_SPAIN        = nn::cfg::CTR::CFG_LANGUAGE_SPANISH,       //!< スペイン語
  LANGUAGE_SIMP_CHINESE = nn::cfg::CTR::CFG_LANGUAGE_SIMP_CHINESE,  //!< 中国語（簡体字）
  LANGUAGE_KOREA        = nn::cfg::CTR::CFG_LANGUAGE_KOREAN,        //!< 韓国語
  LANGUAGE_NETHERLANDS  = nn::cfg::CTR::CFG_LANGUAGE_DUTCH,         //!< オランダ語
  LANGUAGE_PORTUGAL     = nn::cfg::CTR::CFG_LANGUAGE_PORTUGUESE,    //!< ポルトガル語
  LANGUAGE_RUSSIA       = nn::cfg::CTR::CFG_LANGUAGE_RUSSIAN,       //!< ロシア語
  LANGUAGE_TRAD_CHINESE = nn::cfg::CTR::CFG_LANGUAGE_TRAD_CHINESE,  //!< 中国語（繁体字）
  LANGUAGE_MAX          = nn::cfg::CTR::CFG_LANGUAGE_CODE_MAX,
#elif defined(GF_PLATFORM_WIN32)
  LANGUAGE_JAPAN,         //!< 日本語
  LANGUAGE_ENGLISH,       //!< 英語
  LANGUAGE_FRANCE,        //!< フランス語
  LANGUAGE_GERMANY,       //!< ドイツ語
  LANGUAGE_ITALY,         //!< イタリア語
  LANGUAGE_SPAIN,         //!< スペイン語
  LANGUAGE_SIMP_CHINESE,  //!< 中国語（簡体字）
  LANGUAGE_KOREA,         //!< 韓国語
  LANGUAGE_NETHERLANDS,   //!< オランダ語
  LANGUAGE_PORTUGAL,      //!< ポルトガル語
  LANGUAGE_RUSSIA,        //!< ロシア語
  LANGUAGE_TRAD_CHINESE,  //!< 中国語（繁体字）
  LANGUAGE_MAX,
#endif
};

//-----------------------------------------------------------------------------
/**
 * @brief 言語IDの設定
 * @param[in]   lang_id   言語指定ID
 * @param[in]   pTempHeap 作業用ヒープID(256KB=0x40000必要）
 *
 * @attention   内部で下記SetupLangResourceを呼び出す。みだりに使わないこと！！
 */
//-----------------------------------------------------------------------------
extern void SetLang( u8 lang_id, gfl2::heap::HeapBase* pTempHeap, bool is_kanji = false );

//-----------------------------------------------------------------------------
/**
 * @brief 言語関連リソースの（再）設定
 * @param[in]   pTempHeap 作業用ヒープID(256KB=0x40000必要）
 */
//-----------------------------------------------------------------------------
extern void SetupLangResource( gfl2::heap::HeapBase* pTempHeap );

//-----------------------------------------------------------------------------
/**
 * @brief 3DS言語コードの取得
 * @return  nn::cfg::CTR::CfgLanguageCode
 * @attention  ゲーム内の言語ID→本体言語コードに変換するので、本体情報ではない
 */
//-----------------------------------------------------------------------------
extern CfgLanguageCode GetCTRLanguageCode( void );


//-----------------------------------------------------------------------------
/**
 * @brief 言語管理クラス
 * @attention 初期化時にnn::cfg::CTR::GetLanguage()を呼んでいる
 */
//-----------------------------------------------------------------------------
class Language : public gfl2::base::Singleton<Language>
{
  GFL_FORBID_COPY_AND_ASSIGN(Language); //コピーコンストラクタ＋代入禁止

  friend void SetLang( u8 lang_id, gfl2::heap::HeapBase* pTempHeap, bool is_kanji );

public:
  /// constructor
  Language();

  /// destructor
  ~Language();

  //--------------------------------------------------------------
  /**
   * @brief 言語IDを取得する
   * @return  u8 言語ID（LANG_～,include/PokemonVersion.hを参照）
   */
  //--------------------------------------------------------------
  u8 GetLangId( void ) const;

  //--------------------------------------------------------------
  /**
   * @brief 対応する3DS本体言語指定コードを取得する
   * @return CfgLanguageCode
   */
  //--------------------------------------------------------------
  CfgLanguageCode GetCTRLanguageCode( void ) const;

  //--------------------------------------------------------------
  /**
   * @brief   対応するMSGLANGIDを取得する
   * @return  MSGLANGID
   */
  //--------------------------------------------------------------
  MSGLANGID GetMsgLangId( void ) const;

  //--------------------------------------------------------------
  /**
   * @brief 3DS本体言語コード→言語IDへの変換
   */
  //--------------------------------------------------------------
  static u8 ConvertCTR2PM( CfgLanguageCode code );

  //--------------------------------------------------------------
  /**
   * @brief 言語ID→3DS本体言語コードへの変換
   */
  //--------------------------------------------------------------
  static CfgLanguageCode ConvertPM2CTR( u8 poke_lang_id );

  //--------------------------------------------------------------
  /**
   * @brief 言語ID→MSGLANGIDへの変換
   */
  //--------------------------------------------------------------
  static MSGLANGID ConvertPM2MSG( u8 lang_id, bool is_kanji = false );

  //--------------------------------------------------------------
  /**
   * @brief MSGLANGID→言語IDへの変換
   */
  //--------------------------------------------------------------
  static u8 ConvertMSG2PM( MSGLANGID msg_lang_id );

private:
  //--------------------------------------------------------------
  /**
   * @brief 言語IDを設定する
   * @param[in]   lang_id   言語指定ID
   */
  //--------------------------------------------------------------
  void SetLangId( u8 lang_id, bool is_kanji );


  //--------------------------------------------------------------
  /**
   * @brief 現在のゲーム内言語指定
   */
  //--------------------------------------------------------------
  u8 m_LangId;
};

GFL_NAMESPACE_END(System)

#endif // PM_LANGUAGE_H_INCLUDED
