//===================================================================
/**
 * @file pm_language.cpp
 * @brief 言語コードを扱うツール関数
 * @date  2012.11.17
 * @author  tamada@gamefreak.co.jp
 */
//===================================================================
#include "System/include/PokemonVersion.h"
#include "System/include/PokemonLanguage.h"
#include "System/include/HeapDefine.h"
#include "System/include/PmlUse.h"
#include "Print/include/PrintSystem.h"
#include "Print/include/SystemFont.h"
#include "Print/include/GrammaticalAttributeProvider.h"
#include "GameSys/include/GameManager.h"

#include "Print/include/WordsetLoader.h"

#if PM_DEBUG
#include "debug/include/gfl2_DebugwinSystem.h"
#include  <debug/include/gfl2_DebugLogWindow.h>
#endif

//-----------------------------------------------------------------------------
/*
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(System::Language);
#if GFL_RO_ENABLE
template class gfl2::base::SingletonAccessor<System::Language>;
#endif


GFL_NAMESPACE_BEGIN(System)

//===================================================================
//===================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
u8 GetLang(void)
{
  return GFL_SINGLETON_INSTANCE(Language)->GetLangId();
}
//--------------------------------------------------------------
//--------------------------------------------------------------
CfgLanguageCode GetCTRLanguageCode( void )
{
  Language * lang = GFL_SINGLETON_INSTANCE(Language);
  return lang->GetCTRLanguageCode();
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void SetLang( u8 lang_id, gfl2::heap::HeapBase * pTempHeap, bool is_kanji )
{
  GFL_SINGLETON_INSTANCE(Language)->SetLangId( lang_id, is_kanji );
  SetupLangResource( pTempHeap );
}

//--------------------------------------------------------------
/**
 * @note 言語を切り替えた際に実行するべきリソース設定はここに集約
 */
//--------------------------------------------------------------
void SetupLangResource( gfl2::heap::HeapBase* pTempHeap )
{
  WordSetLoader::ReleaseGlobal();

  {
    GameSys::GameManager * gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    if( gman != NULL )
    {
      // 言語ごとのアークファイル読み込み設定
      gfl2::fs::AsyncFileManager * fman = gman->GetAsyncFileManager();
      if( fman != NULL )
      {
        fman->SetDefaultArcLang( Language::ConvertPM2ARC(GetLang()) );
      }
    }
  }

  //フォントを切り替える
  gfl2::heap::HeapBase* fontHeap    = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SYSTEM_FONT );
  gfl2::heap::HeapBase* fontdevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SYSTEM_FONT_DEVICE );
  print::SystemFont * system_font = GFL_SINGLETON_INSTANCE(print::SystemFont);
  system_font->SyncClose();
  system_font->SyncOpen( fontHeap );
  system_font->ChangeMainFont( GFL_SINGLETON_INSTANCE(Language)->GetMsgLangId(), fontHeap , fontdevHeap );
  print::SystemFont_SetLytSystemDefaultFont();

  // PMLIB内のメッセージ関連リソースを作り直す
  System::PmlUse::Finalize();
  System::PmlUse::Initialize();

  {
    WordSetLoader::LoadGlobal(fontdevHeap,PRELOAD_ITEMNAME);  // 前作まではフィールド側に役割が存在していた 言語はまとめて整理
  }

  { // 文法特性データを読み直す
    print::GrammaticalAttributeProvider *grmAttrProvider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
    if( grmAttrProvider )
    {
      grmAttrProvider->ChangeLanguage( GetLang() );
    }
  }

#if PM_DEBUG
  gfl2::debug::DebugWin_ReSetFont(print::SystemFont_GetFont());
  gfl2::debug::LogWindow_SetFont(print::SystemFont_GetFont());
#endif

}



//===================================================================
//
//
//          言語管理クラス
//
//
//===================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
Language::Language()
{
#if defined(GF_PLATFORM_CTR)
  SetLangId( ConvertCTR2PM(static_cast<CfgLanguageCode>(nn::cfg::CTR::GetLanguage())), false );
#elif defined(GF_PLATFORM_WIN32)
  SetLangId( ConvertCTR2PM(LANGUAGE_JAPAN), false );
#endif
}

//--------------------------------------------------------------
//--------------------------------------------------------------
Language::~Language()
{
}

//--------------------------------------------------------------
//--------------------------------------------------------------
u8 Language::GetLangId( void ) const
{
  return m_LangId;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void Language::SetLangId( u8 lang_id, bool is_kanji )
{
  m_LangId = lang_id;
  MSGLANGID msgLangId = ConvertPM2MSG( lang_id, is_kanji );
  print::SetMessageLangId( msgLangId );
//  TAMADA_PRINT("Language::SetLangId(%d)\n", lang_id );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
CfgLanguageCode Language::GetCTRLanguageCode( void ) const
{
  return ConvertPM2CTR( m_LangId );
}
//--------------------------------------------------------------
/**
 * @brief   対応するMSGLANGIDを取得する
 * @return  MSGLANGID
 */
//--------------------------------------------------------------
MSGLANGID Language::GetMsgLangId( void ) const
{
  return ConvertPM2MSG( m_LangId );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
u8 Language::ConvertCTR2PM( CfgLanguageCode code )
{
  u8 lang_id = POKEMON_LANG_NULL;
  switch ( code )
  {
  case LANGUAGE_JAPAN:        lang_id = POKEMON_LANG_JAPAN;   break;	//日本語
  case LANGUAGE_ENGLISH:	    lang_id = POKEMON_LANG_ENGLISH; break;  //英語
  case LANGUAGE_FRANCE:	      lang_id = POKEMON_LANG_FRANCE;  break;  //フランス語
  case LANGUAGE_GERMANY:	    lang_id = POKEMON_LANG_GERMANY; break;  //ドイツ語
  case LANGUAGE_ITALY:	      lang_id = POKEMON_LANG_ITALY;   break;  //イタリア語
  case LANGUAGE_SPAIN:	      lang_id = POKEMON_LANG_SPAIN;   break;  //スペイン語
  case LANGUAGE_SIMP_CHINESE: lang_id = POKEMON_LANG_CHINA; break;  //中国語（簡体字）
  case LANGUAGE_KOREA:	      lang_id = POKEMON_LANG_KOREA;   break;  //韓国語
  case LANGUAGE_NETHERLANDS:  lang_id = POKEMON_LANG_ENGLISH; break;  //オランダ語
  case LANGUAGE_PORTUGAL: 	  lang_id = POKEMON_LANG_ENGLISH; break;  //ポルトガル語
  case LANGUAGE_RUSSIA: 	    lang_id = POKEMON_LANG_ENGLISH; break;  //ロシア語
  case LANGUAGE_TRAD_CHINESE: lang_id = POKEMON_LANG_TAIWAN; break;  //中国語（繁体字）
  //case nn::cfg::CTR::CFG_LANGUAGE_CODE_MAX:	
  }
  if ( lang_id == POKEMON_LANG_NULL )
  {
    GFL_ASSERT( 0 );
    lang_id = POKEMON_LANG_ENGLISH;
  }
  return lang_id;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
CfgLanguageCode Language::ConvertPM2CTR( u8 poke_lang_id )
{
  switch( poke_lang_id )
  {
    case  POKEMON_LANG_JAPAN:
      return LANGUAGE_JAPAN;
    case  POKEMON_LANG_FRANCE:
      return LANGUAGE_FRANCE;
    case  POKEMON_LANG_GERMANY:
      return LANGUAGE_GERMANY;
    case  POKEMON_LANG_ITALY:
      return LANGUAGE_ITALY;
    case  POKEMON_LANG_SPAIN:
      return LANGUAGE_SPAIN;
    case  POKEMON_LANG_KOREA:
      return LANGUAGE_KOREA;
    case  POKEMON_LANG_CHINA:
      return LANGUAGE_SIMP_CHINESE;
    case  POKEMON_LANG_TAIWAN:
      return LANGUAGE_TRAD_CHINESE;

    case  POKEMON_LANG_ENGLISH:
    default:
      return LANGUAGE_ENGLISH;
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
MSGLANGID Language::ConvertPM2MSG( u8 lang_id, bool is_kanji )
{
  switch (lang_id)
  {
  case POKEMON_LANG_JAPAN:
    if  (is_kanji ){
                      return MSGLANGID_KANJI;
    }else{
                      return MSGLANGID_KANA;
    }
  default:
  case POKEMON_LANG_ENGLISH:  return MSGLANGID_ENGLISH;
  case POKEMON_LANG_FRANCE:   return MSGLANGID_FRENCH;
  case POKEMON_LANG_ITALY:    return MSGLANGID_ITALIAN;
  case POKEMON_LANG_GERMANY:  return MSGLANGID_GERMAN;
  case POKEMON_LANG_SPAIN:    return MSGLANGID_SPANISH;
  case POKEMON_LANG_KOREA:    return MSGLANGID_KOREAN;
  case POKEMON_LANG_CHINA:    return MSGLANGID_CHINA;
  case POKEMON_LANG_TAIWAN:    return MSGLANGID_TAIWAN;
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
u8 Language::ConvertMSG2PM( MSGLANGID msg_lang_id )
{
  GFL_ASSERT( msg_lang_id < MSGLANGID_MAX );
  switch ( msg_lang_id )
  {
  case MSGLANGID_KANA:
  case MSGLANGID_KANJI:
    return POKEMON_LANG_JAPAN;
  default:
  case MSGLANGID_ENGLISH:
    return POKEMON_LANG_ENGLISH;
  case MSGLANGID_FRENCH:
    return POKEMON_LANG_FRANCE;
  case MSGLANGID_ITALIAN:
    return POKEMON_LANG_ITALY;
  case MSGLANGID_GERMAN:
    return POKEMON_LANG_GERMANY;
  case MSGLANGID_SPANISH:
    return POKEMON_LANG_SPAIN;
  case MSGLANGID_KOREAN:
    return POKEMON_LANG_KOREA;
  case MSGLANGID_CHINA:
    return POKEMON_LANG_CHINA;
  case MSGLANGID_TAIWAN:
    return POKEMON_LANG_TAIWAN;
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
gfl2::fs::ToolDefine::ArcLangType::Tag Language::ConvertPM2ARC( u8 lang_id )
{
  switch( lang_id )
  {
  case POKEMON_LANG_JAPAN:
    return gfl2::fs::ToolDefine::ArcLangType::JPN;
  case POKEMON_LANG_ENGLISH:
    return gfl2::fs::ToolDefine::ArcLangType::USA;
  case POKEMON_LANG_FRANCE:
    return gfl2::fs::ToolDefine::ArcLangType::FRA;
  case POKEMON_LANG_ITALY:
    return gfl2::fs::ToolDefine::ArcLangType::ITA;
  case POKEMON_LANG_GERMANY:
    return gfl2::fs::ToolDefine::ArcLangType::DEU;
  case POKEMON_LANG_SPAIN:
    return gfl2::fs::ToolDefine::ArcLangType::ESP;
  case POKEMON_LANG_KOREA:
    return gfl2::fs::ToolDefine::ArcLangType::KOR;
  case POKEMON_LANG_CHINA:
    return gfl2::fs::ToolDefine::ArcLangType::CHN;
  case POKEMON_LANG_TAIWAN:
    return gfl2::fs::ToolDefine::ArcLangType::TWN;
  }
  GFL_ASSERT( 0 );
  return gfl2::fs::ToolDefine::ArcLangType::DEF;
}


GFL_NAMESPACE_END(System)
