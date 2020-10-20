//======================================================================
/**
* @file DebugStartSettingAccessor.cpp
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief csvファイルのパーサー、CTR/Win32共通。デバッグ用。
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

// gfl
#include <util/include/gfl2_std_string.h>
#include <core/include/gfl2_Utility.h>

// niji
#include "Field/FieldStatic/include/Debug/DebugStartSettingAccessor.h"
#include "Field/FieldStatic/include/Debug/DebugZoneNameLoader.h"

#include <arc_def_index/arc_def.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/PokemonVersion.h"

#include "niji_conv_header/field/zone/zone_id.h"

#define VALID_CHECK( member ) ( m_pSettingData && m_pSettingData->member != sc_InvalidParam.member )


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)





struct StartSettingAccessor::SettingData
{
  u32 startZoneID;                  // 開始ゾーン
  gfl2::math::Vector3 startPositon; // 開始ポジション
  s32 sex;              // 性別

  StartSettingAccessor::TitleSkipMode::Value titleSkipMode; // タイトル画面スキップ機能
  s32 scriptHIOEnable;      // ScriptのHIO機能
  s32 scriptEditModeEnable; // Scriptのシーンエディットモード機能
  s32 scriptAssertEnable;   // Scriptのアサート機能
  s32 scriptEndPositonCheckEnable; // Scriptの終了座標出力機能
  s32 scriptEndPositonOutPutEnable;   // Scriptの終了座標チェック機能

  f32 soundMasterVolum;     // サウンドのマスターボリューム(0.0～1.0)
  s32 hioEnable;            // 常駐ファイルマネージャーのHIO機能
  u8  lang;                 // 言語画面スキップ時の言語ID
  b32 isKanji;
};

// 各パラメータの無効値
static const StartSettingAccessor::SettingData sc_InvalidParam =
{
  ZONE_ID_INVALID,
  gfl2::math::Vector3( FLT_MAX, FLT_MAX, FLT_MAX ),
  PM_NEUTRAL,
  StartSettingAccessor::TitleSkipMode::INVALID,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1.0f,
  -1,
  POKEMON_LANG_NULL,
  false
};


StartSettingAccessor::StartSettingAccessor( gfl2::heap::HeapBase* pHeap ) :
  m_pSettingData( NULL )
{
  m_pSettingData = GFL_NEW( pHeap ) SettingData( sc_InvalidParam );
}

StartSettingAccessor::~StartSettingAccessor()
{
  GFL_SAFE_DELETE( m_pSettingData );
}

/**
* @breaf データをロードする 
*
* @param pHeap ロード先を確保するためのヒープ
* @return 成否
*/
b32 StartSettingAccessor::Load( gfl2::heap::HeapBase* pHeap )
{
  DeleteData();

  Debug::CsvParser* pCsvData = GFL_NEW( pHeap ) Debug::CsvParser( pHeap, "R:/home/momiji/exec/develop_desc/Bin/DebugStartSetting_utf8.csv" ); 
  const s32 headerLineNum = 1;

  if( pCsvData->Exists() )
  {
    if( pCsvData->GetLineNum() <= headerLineNum )
    {
      // データ無し、ロードは成功扱いにする
      // CSV破棄
      GFL_SAFE_DELETE( pCsvData );
      return true;
    }

    // Csvデータからハッシュテーブルを作成
    HASH_DATA hashData;
    hashData.pDataTable = GFL_NEW_ARRAY( pHeap ) Data [ pCsvData->GetLineNum() - headerLineNum ];  // 1行目はヘッダー
    for( u32 i = 0; i < pCsvData->GetLineNum() - headerLineNum; i++ )
    {
      Debug::CsvParser::RowObject* pRowObject = pCsvData->GetRowData( pHeap, i + headerLineNum );
      hashData.pDataTable[ i ].hash = gfl2::util::FnvHash( reinterpret_cast<const u8*>( pRowObject->GetColumnData( 0 ) ) );

      // GFL_PRINT("Make FnvHash %s\n",  pRowObject->GetColumnData( 0 ) );

      s32 textLen = strlen( pRowObject->GetColumnData( 1 ) ) + 1;
      hashData.pDataTable[ i ].pVable = GFL_NEW_ARRAY( pHeap ) c8[ textLen ];
      gfl2::std::MemCopy( pRowObject->GetColumnData( 1 ), hashData.pDataTable[ i ].pVable, textLen );

      GFL_SAFE_DELETE( pRowObject );
    }
    hashData.dataNum = pCsvData->GetLineNum() - 1;

    // CSV破棄
    GFL_SAFE_DELETE( pCsvData );
    
    //
    // ハッシュデータをパラメータ化
    //

    // 開始ゾーン
    c8* pZoneName;
    if( GetValue( hashData, "StartZone", &pZoneName ) )
    {
      Debug::ZoneNameLoader* pZoneNameLoader = GFL_NEW( pHeap ) Debug::ZoneNameLoader;
      if( pZoneNameLoader->Load( pHeap ) )
      {
        u32 zoneId;
        // GFL_PRINT("zone=%s\n", pZoneName );
        if( pZoneNameLoader->GetZoneId( pZoneName, &zoneId ) )
        {
          m_pSettingData->startZoneID = zoneId;
        }

        pZoneNameLoader->Delete();
      }
      GFL_SAFE_DELETE( pZoneNameLoader );
    }

    // 開始ポジション
    c8* startPositon;
    if( GetValue( hashData, "StartPositon", &startPositon ) )
    { 
      Debug::CsvParser::RowObject* pRowObject = GFL_NEW( pHeap ) Debug::CsvParser::RowObject( pHeap, startPositon );
      if( pRowObject->GetSize() >= 2 )
      {
        m_pSettingData->startPositon.x = atof( pRowObject->GetColumnData( 0 ) );
        m_pSettingData->startPositon.z = atof( pRowObject->GetColumnData( 1 ) );
        m_pSettingData->startPositon.y = 0.0f;
      }
      GFL_SAFE_DELETE( pRowObject );
    }
    // startPositon

    // 性別
    c8* pSex;
    if( GetValue( hashData, "Sex", &pSex ) )
    {
      if( atoi( pSex ) == 1 )
      {
        m_pSettingData->sex = PM_FEMALE;
      }
      else
      {
        m_pSettingData->sex = PM_MALE;
      }
    }

    // その他
    c8* pWork;
    if( GetValue( hashData, "TitleSkip", &pWork ) )
    {
      u32 work = atoi( pWork );
      work = (work >= TitleSkipMode::VAL_MAX) ? TitleSkipMode::INVALID : work;
      m_pSettingData->titleSkipMode = static_cast<TitleSkipMode::Value>( work );
    }
    if( GetValue( hashData, "SoundMasterVolum", &pWork ) )
    {
      m_pSettingData->soundMasterVolum = atof( pWork );
    }
    if( GetValue( hashData, "PcFileLoadEnable", &pWork ) )
    {
      m_pSettingData->hioEnable = ( atoi( pWork ) == 1 ) ? 1 : 0;
    }

    // スクリプト関連
    if( GetValue( hashData, "ScriptHIOEnable", &pWork ) )
    {
      m_pSettingData->scriptHIOEnable = ( atoi( pWork ) == 1 ) ? 1 : 0;
    }
    if( GetValue( hashData, "ScriptEditModeEnable", &pWork ) )
    {
      m_pSettingData->scriptEditModeEnable = ( atoi( pWork ) == 1 ) ? 1 : 0;
    }
    if( GetValue( hashData, "ScriptAssertEnable", &pWork ) )
    {
      m_pSettingData->scriptAssertEnable = ( atoi( pWork ) == 1 ) ? 1 : 0;
    }
    if( GetValue( hashData, "ScriptEndPositonCheckEnable", &pWork ) )
    {
      m_pSettingData->scriptEndPositonCheckEnable = (atoi(pWork) == 1) ? 1 : 0;
    }
    if( GetValue( hashData, "ScriptEndPositonOutputEnable", &pWork ) )
    {
      m_pSettingData->scriptEndPositonOutPutEnable = (atoi(pWork) == 1) ? 1 : 0;
    }
    
    if( GetValue( hashData, "Lang", &pWork ) )
    {
      struct LANG_DATA
      {
        const c8* const name;
        u8              value;
        b32             isKanji;
      };

      static const LANG_DATA sc_LangTable[] = 
      {
        { "JAPAN", POKEMON_LANG_JAPAN, false },
        { "KANJI", POKEMON_LANG_JAPAN, true },
        { "ENGLISH", POKEMON_LANG_ENGLISH, false },
        { "FRANCE", POKEMON_LANG_FRANCE, false },
        { "ITALY", POKEMON_LANG_ITALY, false },
        { "GERMANY", POKEMON_LANG_GERMANY, false },
        { "SPAIN", POKEMON_LANG_SPAIN, false },
        { "KOREA", POKEMON_LANG_KOREA, false },
        { "CHINA", POKEMON_LANG_CHINA, false },
        { "TAIWAN", POKEMON_LANG_TAIWAN, false }
      };

      u32 checkLen = ::std::strlen( pWork );
      for (u32 i = 0; i < GFL_NELEMS(sc_LangTable); i++)
      {
        if (gfl2::core::Utility::Strncmp( sc_LangTable[i].name, pWork, checkLen ) == 0)
        {
          m_pSettingData->lang = sc_LangTable[i].value;
          m_pSettingData->isKanji = sc_LangTable[i].isKanji;
          break;
        }
      }
    }

#if 0
    if (gfl2::core::Utility::Strncmp( &m_pNameBuf[i * m_NameLengthLimit], pName, m_NameLengthLimit ) == 0)
    {

#endif

    // ハッシュテーブル破棄
    for( u32 i = 0; i < hashData.dataNum; i++ )
    {
      GFL_SAFE_DELETE_ARRAY( hashData.pDataTable[i].pVable );
    }
    GFL_SAFE_DELETE_ARRAY( hashData.pDataTable );

    return true;

  }
  else
  {
    GFL_SAFE_DELETE( pCsvData );
    return false;
  }
}

/**
* @breaf データを削除
*/
void StartSettingAccessor::DeleteData( void )
{
  // 現在のパラメーターを無効化
  if( m_pSettingData )
  {
    *m_pSettingData = sc_InvalidParam;
  }
}


/**
* @breaf 開始ゾーン取得
*/
b32 StartSettingAccessor::HasStartZone( void ) const
{
  if( m_pSettingData == NULL ) return false;
  return ( m_pSettingData->startZoneID != sc_InvalidParam.startZoneID );
}

u32 StartSettingAccessor::GetStartZone( void ) const
{
  return m_pSettingData->startZoneID;
}

/**
* @breaf 開始座標取得
*/
b32 StartSettingAccessor::HasStartPosition( void ) const
{
  return VALID_CHECK( startPositon );
}

void StartSettingAccessor::GetStartPosition( gfl2::math::Vector3* pOut ) const
{
  *pOut = m_pSettingData->startPositon;
}

/**
* @breaf 性別取得
*/
b32 StartSettingAccessor::HasStartSex( void ) const
{
  if( m_pSettingData == NULL ) return false;
  return ( m_pSettingData->sex != sc_InvalidParam.sex );
}

s32 StartSettingAccessor::GetStartSex( void ) const
{
  return m_pSettingData->sex;
}


/**
* @breaf タイトルスキップ機能
*/
b32 StartSettingAccessor::HasTitleSkipEnable( void ) const
{
  return VALID_CHECK( titleSkipMode );
}
StartSettingAccessor::TitleSkipMode::Value StartSettingAccessor::GetTitleSkipMode( void ) const
{
  return ( m_pSettingData->titleSkipMode );
}

/**
* @breaf ScriptのHIO機能
*/
b32 StartSettingAccessor::HasScriptHioEnable( void ) const
{
  if( m_pSettingData == NULL ) return false;
  return ( m_pSettingData->scriptHIOEnable != sc_InvalidParam.scriptHIOEnable );
}

b32 StartSettingAccessor::IsScriptHioEnable( void ) const
{
  return ( m_pSettingData->scriptHIOEnable == 1 );
}

/**
* @breaf Scriptのシーンエディットモード機能
*/
b32 StartSettingAccessor::HasScriptEditModeEnable( void ) const
{
  if( m_pSettingData == NULL ) return false;
  return ( m_pSettingData->scriptEditModeEnable != sc_InvalidParam.scriptEditModeEnable );
}

b32 StartSettingAccessor::IsScriptEditModeEnable( void ) const
{
  return  ( m_pSettingData->scriptEditModeEnable == 1 );
}

/**
* @breaf Scriptのシーンエディットモード機能
*/
b32 StartSettingAccessor::HasScriptAssertEnable( void ) const
{
  if( m_pSettingData == NULL ) return false;
  return ( m_pSettingData->scriptAssertEnable != sc_InvalidParam.scriptAssertEnable );
}

b32 StartSettingAccessor::IsScriptAssertEnable( void ) const
{
  return m_pSettingData->scriptAssertEnable;
}

/**
* @breaf Scriptの終了座標出力機能
*/
b32 StartSettingAccessor::HasScriptEndPositionOutputEnable(void) const
{
  return VALID_CHECK(scriptEndPositonOutPutEnable);
}
b32 StartSettingAccessor::IsScriptEndPositionOutPutEnable(void) const
{
  return m_pSettingData->scriptEndPositonOutPutEnable;
}

/**
* @breaf Scriptの終了座標チェック機能
*/
b32 StartSettingAccessor::HasScriptEndPositionCheckEnable(void) const
{
  return VALID_CHECK(scriptEndPositonCheckEnable);
}
b32 StartSettingAccessor::IsScriptEndPositionCheckEnable(void) const
{
  return m_pSettingData->scriptEndPositonCheckEnable;
}


/**
* @breaf 常駐ファイルマネージャーのHIO機能
*/
b32 StartSettingAccessor::HasHioEnable(void) const
{
  return VALID_CHECK(hioEnable);
}

b32 StartSettingAccessor::GetHioEnable(void) const
{
  return m_pSettingData->hioEnable;
}

/**
* @breaf サウンドのボリューム(0.0～1.0)
*/
b32 StartSettingAccessor::HasSoundMasterVolume( void ) const
{
  if( m_pSettingData == NULL ) return false;
  return ( m_pSettingData->soundMasterVolum != sc_InvalidParam.soundMasterVolum );
}

f32 StartSettingAccessor::GetSoundMasterVolume( void ) const
{
  return m_pSettingData->soundMasterVolum;
}

b32 StartSettingAccessor::GetValue( const HASH_DATA& rHashData, const c8* dataName, c8** ppOut ) const
{
  gfl2::util::FnvHash hash( reinterpret_cast<const u8*>( dataName ) );
  for( u32 i = 0; i < rHashData.dataNum; i++ )
  {
    if( rHashData.pDataTable[i].hash == hash )
    {
      *ppOut = rHashData.pDataTable[i].pVable;
      return true;
    }
  }

  return false;
}

/**
* @breaf 言語画面スキップ時の言語ID
*/
b32 StartSettingAccessor::HasLang(void) const
{
  return VALID_CHECK(lang);
}

u8 StartSettingAccessor::GetLang(void) const
{
  return m_pSettingData->lang;
}

b32 StartSettingAccessor::IsKanji(void) const
{
  return m_pSettingData->isKanji;
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif
