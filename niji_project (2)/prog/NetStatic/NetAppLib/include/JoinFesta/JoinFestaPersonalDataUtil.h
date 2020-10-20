//==============================================================================
/**
 * @file	JoinFestaPersonalDataUtil.h
 * @brief	ジョインフェスタパーソナルデータUtil
 * @author	ichiraku_katsuhiko
 * @date	2016/03/22　12:52
 */
// =============================================================================
#ifndef __JOIN_FESTA_PERSONAL_DATA_UTIL_H__
#define __JOIN_FESTA_PERSONAL_DATA_UTIL_H__
#pragma once

// ライブラリ
#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "System/include/GflUse.h"

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"
// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)
struct JoinFestaDataTable;
GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// 前方宣言
class JoinFestaPersonalData;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaPersonalDataUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaPersonalDataUtil);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

  //! 初期定型文データのMAX値
  static const int INIT_PATTENR_TABLE_NUM = 14;

public:
  // コンストラクタ
  JoinFestaPersonalDataUtil(){};
  // デストラクタ
  virtual ~JoinFestaPersonalDataUtil(){}


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
public:
  //=============================================================================

  //! 初期定型文MSGテーブル取得
  static u32 GetInitPatternMsg( const u32 tblIndex, const u32 patternIndex );
  //! 初期定型文カテゴリMSGテーブル取得
  static u32 GetInitCategoryMsg( const u32 tblIndex, const u32 patternIndex );

  //! 着せ替えパターンで着せ替えをセット（※性別も自動で決まります。）
  static void SetDressUpPattern( NetAppLib::JoinFesta::JoinFestaPersonalData& person, const u32 kisekaeId, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable );

private:
  // あいさつ、さよなら、かんどう、ざんねんの順
  static const u32 PATTERN_MSG_TABLE[ INIT_PATTENR_TABLE_NUM ][ 4 ];     //! 初期定型文MSGのテーブル
  static const u32 CATEGORY_MSG_TABLE[ INIT_PATTENR_TABLE_NUM ][ 4 ];    //! ↑のカテゴリメッセージテーブル

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // __JOIN_FESTA_PERSONAL_DATA_UTIL_H__
