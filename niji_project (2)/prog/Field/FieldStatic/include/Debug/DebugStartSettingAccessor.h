//======================================================================
/**
* @file DebugStartSettingAccessor.h
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief csvファイルのパーサー、CTR/Win32共通。デバッグ用。
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#if !defined __DEBUGSTARTSETTINGACCESSOR_H_INCLUDED__
#define __DEBUGSTARTSETTINGACCESSOR_H_INCLUDED__
#pragma once

// gflib
#include <util/include/gfl2_FnvHash.h>
#include <math/include/gfl2_Vector3.h>

// niji
#include "DebugCsvParser.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
* @class デバッグ開始の設定ファイルへのアクセッサ
*/
class StartSettingAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( StartSettingAccessor );

public:

  struct TitleSkipMode
  {
    enum Value
    {
      INVALID = 0,      // 無効
      START_LAUNCHER,   // タイトルデバッグスタートランチャーから開始
      MODE_DEBUG_FIRST,      // 通常デバッグでフィールド開始
      MODE_DEBUG_DEFAULTNA,  // デフォルト名でフィールド開始
      MODE_DEBUG_NETTEST,    // ネットテストでフィールド開始

      VAL_MAX,
    };
  };

  struct Data
  {
    gfl2::util::FnvHash hash;
    c8*                 pVable;
  };

  struct SettingData;

  StartSettingAccessor( gfl2::heap::HeapBase* pHeap );
  virtual ~StartSettingAccessor();

  /**
  * @breaf データをロードする 
  *
  * @param pHeap ロード先を確保するためのヒープ
  * @return 成否
  */
  b32 Load( gfl2::heap::HeapBase* pHeap );

  /**
  * @breaf データを削除
  */
  void DeleteData( void );


  /**
  * @breaf 開始ゾーン取得
  */
  b32 HasStartZone( void ) const;
  u32 GetStartZone( void ) const;

  /**
  * @breaf 開始座標取得
  */
  b32 HasStartPosition( void ) const;
  void GetStartPosition( gfl2::math::Vector3* pOut ) const;

  /**
  * @breaf 性別取得
  */
  b32 HasStartSex( void ) const;
  s32 GetStartSex( void ) const;

  /**
  * @breaf タイトルスキップ機能
  */
  b32 HasTitleSkipEnable( void ) const;
  TitleSkipMode::Value GetTitleSkipMode( void ) const;

  /**
  * @breaf ScriptのHIO機能
  */
  b32 HasScriptHioEnable( void ) const;
  b32 IsScriptHioEnable( void ) const;
  
  /**
  * @breaf Scriptのシーンエディットモード機能
  */
  b32 HasScriptEditModeEnable( void ) const;
  b32 IsScriptEditModeEnable( void ) const;

  /**
  * @breaf Scriptのアサート機能
  */
  b32 HasScriptAssertEnable( void ) const;
  b32 IsScriptAssertEnable( void ) const;

  /**
  * @breaf Scriptの終了座標出力機能
  */
  b32 HasScriptEndPositionOutputEnable(void) const;
  b32 IsScriptEndPositionOutPutEnable(void) const;
  
  /**
  * @breaf Scriptの終了座標チェック機能
  */
  b32 HasScriptEndPositionCheckEnable(void) const;
  b32 IsScriptEndPositionCheckEnable(void) const;

  /**
  * @breaf サウンドのマスターボリューム(0.0～1.0)
  */
  b32 HasSoundMasterVolume( void ) const;
  f32 GetSoundMasterVolume( void ) const;

  /**
  * @breaf 常駐ファイルマネージャーのHIO機能
  */
  b32 HasHioEnable(void) const;
  b32 GetHioEnable(void) const;

  /**
  * @breaf 言語画面スキップ時の言語ID
  */
  b32 HasLang(void) const;
  u8 GetLang(void) const;
  b32 IsKanji(void) const;

private:

  struct HASH_DATA
  {
    Data*      pDataTable;
    s32        dataNum;
  };
  
  b32 GetValue( const HASH_DATA& rHashData, const c8* dataName, c8** ppOut ) const;


  SettingData* m_pSettingData; // csvから読み込んだ設定           
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __DEBUGSTARTSETTINGACCESSOR_H_INCLUDED__
#endif