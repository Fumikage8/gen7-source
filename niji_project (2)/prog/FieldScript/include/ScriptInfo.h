//=============================================================================
/**
 * @file    ScriptInfo.h
 * @brief   フィールドスクリプトの情報管理クラス
 * @author  iwasawa GAMEFREAK
 * @date    2015.02.15
 */
//=============================================================================

#ifndef __SCRIPTINFO_H__
#define __SCRIPTINFO_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "FieldScript/include/ScriptTypes.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//--------------------------------------------------------------
/// スクリプトARCテーブル
//==============================================================
struct SCRIPT_ARC_TABLE
{ 
  u16 scrid_start;      ///< 対象スクリプトIDの開始位置
  u16 scrid_num;        ///< 対象スクリプトIDの終了位置(含まない)
  u16 message_arc_id;   ///< 対象スクリプトで使用するメッセージアーカイブ
  u16 message_dat_id;   ///< メッセージアーカイブ内のファイルID
  u16 amx_id;           ///< 対象スクリプト含むamxのID
};

//--------------------------------------------------------------
/**
 * @class ScriptInfo
 * @brief スクリプトの情報管理用クラス
 */
//--------------------------------------------------------------
class ScriptInfo {

public:
  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ScriptInfo();
  /* ------------------------------------------------------------------------- */
  /**
   * @brief デストラクタ
   */
  /* ------------------------------------------------------------------------- */
  ~ScriptInfo();

  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト情報をセット
   *
   * @param zoneId ゾーンID
   * @param scrid スクリプトID
   * @param zone ゾーンデータローダーへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  void SetScriptInfo( u16 zoneId, SCRIPTID scrid, const ZoneDataAccessor* zone );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief AMX名を取得
   *
   * @param scrType スクリプトタイプ
   * @param index スクリプトインデックス
   *
   * @return AMX名
   *
   * @note 現在、"" を返すだけの関数になっている
   */
  /* ------------------------------------------------------------------------- */
  static const char * GetAmxName( SCRIPT_TYPE scrType, u16 index );
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 共通スクリプトの数を取得
   *
   * @return 共通スクリプトの数
   */
  /* ------------------------------------------------------------------------- */
  static u32 GetCommonAmxNum( void );

public:
  SCRIPT_TYPE m_ScriptType;       ///<スクリプトの種類
  SCRIPTID m_ScriptId;            ///<スクリプトID
  u32 m_MsgArcId;                 ///<使用するMSGアーカイブ指定
  u32 m_MsgDatId;                 ///<使用するMSGアーカイブ内インデックス指定
  u32 m_RegFuncId;                ///<登録するコマンドの指定
  u16 m_ZoneID;                   ///<ゾーンID
  u16 m_AmxId;                    ///<Amxのインデックス
  const char * m_AmxFileName;     ///<Amxファイル名（デバッグ用）

  u16 m_NextTblId;  ///< 次のテーブルID
};


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif  // __SCRIPTINFO_H__
