// ============================================================================
/*
 * @file GTSUtility.h
 * @brief GTSアプリで使用する共通処理をまとめたクラスです。
 * @date 2015.04.16
 */
// ============================================================================
#if !defined( GTSUTILITY_H_INCLUDE )
#define GTSUTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


class GTSUtility
{
public:
  static const u32* GetGenderMessageIDTable();
  static u32 GetGenderMessageIDTableSize();
  static u32 GetGenderMessageID( u8 gender );
  static u8 ConvertPmlSexToGender( pml::Sex sex );
  static pml::Sex ConvertGenderToPmlSex( u8 gender );

  static u8 GetDrawGender( MonsNo monsno, u8 gender, bool haveNickName );

  static const u32* GetLevelBandMessageIDTable();
  static u32 GetLevelBandMessageIDTableSize();
  static u32 GetLevelBandMessageID( u8 levelBand );
  static u8 ConvertLevelToLevelBand( u32 level );
  static u16 ConvertLevelBandToLevel( u8 levelBand );

  static void SetupUploadGTSIndexData( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork, const u64 dataStoreID );

  static void BackupUploadPokemon( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork );
  static void RestoreUploadPokemon( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork );


  static void SetCriticalSectionFlag( bool bFlag );

  // niji通信互換対応用
  // アップロードするデータ検査してniji表示禁止フラグを立てる
  static void CheckNijiNgFlag( NetApp::GTS::ApplicationSystem::GTSWork& gtsWork );


};


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUTILITY_H_INCLUDE
