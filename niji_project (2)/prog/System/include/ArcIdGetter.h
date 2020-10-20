#if !defined(NIJI_PROJECT_SYSTEM_ARCIDGETTER_H_INCLUDED)
#define NIJI_PROJECT_SYSTEM_ARCIDGETTER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ArcIdGetter.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.26
 @brief   ARCIDゲッター
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <fs/include/gfl2_Fs.h>
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>




////////////////////////////////////////////////////////////////////////////////
//
// 公開
//
////////////////////////////////////////////////////////////////////////////////

//! @brief  ARCIDの番号を取得する
//! @param[in]  baseARCID  arc_tool.lstに書いたARCID
//! @note   カセットバージョン(BLACK/WHITEとか)、コンフィギュレーション(Debug/Releaseとか)に
//!         応じたARCIDの番号を返す。
//!         Win32版、HIOを使用しているCTR版では、カセットバージョンの実行時変更にも対応できる。
#if PM_DEBUG
#define NIJI_ARCID_NUM(baseARCID)  __NIJI_ARCID_NUM(baseARCID)
#else  // PM_DEBUG
#define NIJI_ARCID_NUM(baseARCID)  (baseARCID)
#endif  // PM_DEBUG

/*
使っていないのでコメントアウト。

//! @brief  ARCIDの文字列を取得する
//! @param[in]  baseARCID  arc_tool.lstに書いたARCID
//! @note   カセットバージョン(BLACK/WHITEとか)、コンフィギュレーション(Debug/Releaseとか)に
//!         応じたARCIDの文字列を返す。
//!         Win32版、HIOを使用しているCTR版では、カセットバージョンの実行時変更にも対応できる。
#if PM_DEBUG
#define NIJI_ARCID_STR(baseARCID)  __NIJI_ARCID_STR(baseARCID)
#endif  // PM_DEBUG
*/




#if PM_DEBUG

#if GFL_ENABLE_DYNAMIC_HEADER


//! @brief  ARCIDの番号を取得する(ダイナミックヘッダー利用版)
//! @param[in]  baseARCID  arc_tool.lstに書いたARCID
//! @note   カセットバージョン(BLACK/WHITEとか)、コンフィギュレーション(Debug/Releaseとか)に
//!         応じたARCIDの番号を返す。
//!         Win32版、HIOを使用しているCTR版では、カセットバージョンの実行時変更にも対応できる。
//! @note   GFL_VALUE_DEFINE_IN_HEADER参照
#define NIJI_ARCID_DEFINE_IN_HEADER(baseARCID) gfl2::debug::DynamicHeader::HeaderHashManager::GetValue(__NIJI_ARCID_STR(baseARCID), NIJI_ARCID_NUM(baseARCID))

//! @brief  ARCIDの番号を取得する(ヘッダファイル指定版)(ダイナミックヘッダー利用版)
//! @param[in]  baseARCID  arc_tool.lstに書いたARCID
//! @param[in]  filePath   arc_def.hのパス
//! @note   カセットバージョン(BLACK/WHITEとか)、コンフィギュレーション(Debug/Releaseとか)に
//!         応じたARCIDの番号を返す。
//!         Win32版、HIOを使用しているCTR版では、カセットバージョンの実行時変更にも対応できる。
//! @note   GFL_VALUE_DEFINE_IN_HEADER_FAST参照
#define NIJI_ARCID_DEFINE_IN_HEADER_FAST(baseARCID, filePath) gfl2::debug::DynamicHeader::HeaderHashManager::GetValue(__NIJI_ARCID_STR(baseARCID), NIJI_ARCID_NUM(baseARCID), filePath)


#else  // GFL_ENABLE_DYNAMIC_HEADER


#define NIJI_ARCID_DEFINE_IN_HEADER(baseARCID) NIJI_ARCID_NUM(baseARCID)

#define NIJI_ARCID_DEFINE_IN_HEADER_FAST(baseARCID, filePath) NIJI_ARCID_NUM(baseARCID)


#endif  // GFL_ENABLE_DYNAMIC_HEADER






////////////////////////////////////////////////////////////////////////////////
//
// 非公開
//
////////////////////////////////////////////////////////////////////////////////

// これは使わないで下さい。必ずNIJI_ARCID_NUMのほうを使って下さい。
#define __NIJI_ARCID_NUM(baseARCID) \
  System::ArcIdGetter::getArcIdNum( \
    baseARCID, \
    baseARCID##_VERSION_MOON_DEBUG_NO, \
    baseARCID##_VERSION_MOON_DEBUG_YES, \
    baseARCID##_VERSION_SUN_DEBUG_NO, \
    baseARCID##_VERSION_SUN_DEBUG_YES \
  )


// これは使わないで下さい。必ずNIJI_ARCID_STRのほうを使って下さい。
#define __NIJI_ARCID_STR(baseARCID) \
  System::ArcIdGetter::getArcIdStr( \
    #baseARCID, \
    #baseARCID"_VERSION_MOON_DEBUG_NO", \
    #baseARCID"_VERSION_MOON_DEBUG_YES", \
    #baseARCID"_VERSION_SUN_DEBUG_NO", \
    #baseARCID"_VERSION_SUN_DEBUG_YES" \
  ) 


GFL_NAMESPACE_BEGIN(System)

//==============================================================================
/**
 @class     ArcIdGetter
 @brief     ARCIDゲッター
 @note      これは使わないで下さい。必ずNIJI_ARCID_NUM, NIJI_ARCID_STRのほうを使って下さい。
 */
//==============================================================================
class ArcIdGetter
{
public:
  // これは使わないで下さい。必ずNIJI_ARCID_NUMのほうを使って下さい。
  static gfl2::fs::ArcFile::ARCID getArcIdNum(
      gfl2::fs::ArcFile::ARCID baseARCID,
      gfl2::fs::ArcFile::ARCID arcId_VERSION_A_DEBUG_NO,
      gfl2::fs::ArcFile::ARCID arcId_VERSION_A_DEBUG_YES,
      gfl2::fs::ArcFile::ARCID arcId_VERSION_B_DEBUG_NO,
      gfl2::fs::ArcFile::ARCID arcId_VERSION_B_DEBUG_YES
  );

  // これは使わないで下さい。必ずNIJI_ARCID_STRのほうを使って下さい。
  static const char* getArcIdStr(
      const char* baseARCID,
      const char* arcId_VERSION_A_DEBUG_NO,
      const char* arcId_VERSION_A_DEBUG_YES,
      const char* arcId_VERSION_B_DEBUG_NO,
      const char* arcId_VERSION_B_DEBUG_YES
  );
};


GFL_NAMESPACE_END(System)

#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_SYSTEM_ARCIDGETTER_H_INCLUDED

