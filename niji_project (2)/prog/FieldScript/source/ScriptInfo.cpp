//=============================================================================
/**
 * @file    ScriptInfo.cpp
 * @brief   フィールドスクリプトの情報管理クラス
 * @author  iwasawa GAMEFREAK
 * @date    2015.02.28
 */
//=============================================================================

#include <debug/include/gfl2_Assert.h>
#include "Print/include/PrintSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptInfo.h"
#include "FieldScript/include/FieldScriptSystem.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//--------------------------------------------------------------
/// メッセージARCの種類
//==============================================================
enum
{
  MSGARC_MESSAGE, ///< メッセージARC
  MSGARC_SCRIPT,  ///< スクリプトARC
  MSGARC_DEBUG,   ///< デバッグARC
};

/**
 * @note  N.Takeda
    R:/home/momiji/exec/arc/arc_index/FieldScript.gaix
    R:/home/momiji/exec/arc/arc_index/FieldScriptD.gaix

    R:/home/momiji/exec/conv_header/niji_conv_header/field/script/ScrOffset.cdat
    R:/home/momiji/exec/conv_header/niji_conv_header/field/script/ScrOffsetId.h
 */

#include "niji_conv_header/field/script/ScrOffsetId.h"
#include "niji_conv_header/field/script/FileNameList.cdat" // for sc_ScriptFileName??? 
#include "niji_reference_files/script/sp_script_define.inc" // for SCRID_SP_START

/* ------------------------------------------------------------------------- */
/**
 * @brief コンストラクタ
 */
/* ------------------------------------------------------------------------- */
ScriptInfo::ScriptInfo() :
  m_ScriptType(SCRIPT_TYPE_NONE),
  m_ScriptId(SCRID_DEFAULT),
  m_MsgArcId(print::GetMessageArcId_Script()),
  m_MsgDatId(0),
  m_RegFuncId(0),
  m_ZoneID(0),
  m_AmxId(ScriptObject::AMX_ID_NULL),
  m_AmxFileName(NULL),

  m_NextTblId(0)
{
}

/* ------------------------------------------------------------------------- */
/**
 * @brief デストラクタ
 */
/* ------------------------------------------------------------------------- */
ScriptInfo::~ScriptInfo()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デフォルトアーカイブ指定→言語別アーカイブ指定への振り分け処理
 *
 * @param org_arc_id オリジナルARCID
 */
//-----------------------------------------------------------------------------
static u32 getMessageArcId( u16 org_arc_id )
{
  switch ( org_arc_id )
  {
#if PM_DEBUG
  case MSGARC_DEBUG:
    return print::GetMessageArcId_Debug();
#endif
  case MSGARC_SCRIPT:
    return print::GetMessageArcId_Script();
  case MSGARC_MESSAGE:
  default:
    return print::GetMessageArcId();
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト情報をセット
 *
 * @param zoneId ゾーンID
 * @param scrid スクリプトID
 * @param zone ゾーンデータローダーへのポインタ
 */
/* ------------------------------------------------------------------------- */
void ScriptInfo::SetScriptInfo( u16 zoneId, SCRIPTID scrid, const ZoneDataAccessor* zone)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  
  SCRIPT_ARC_TABLE  *pScriptArcTable = pSys->GetScriptArcTable();
  s32               iEntry = pSys->GetScriptArcTableSize() / sizeof( SCRIPT_ARC_TABLE );
  
  
  // 共通スクリプト読み込み
  for( s32 ii=iEntry-1; ii>=0; --ii )
  {
    // SPはゾーンスクリプト
    if( scrid >= SCRID_SP_START && scrid < SCRID_SP_END )
    {
      break;
    }
    const SCRIPT_ARC_TABLE * tbl = pScriptArcTable;
    if( scrid >= tbl[ii].scrid_start )
    {
      GFL_ASSERT_STOP_MSG( scrid <= tbl[ii].scrid_start+tbl[ii].scrid_num, "id=%d end=%d", scrid, tbl[ii].scrid_start+tbl[ii].scrid_num );
      m_NextTblId   = ii;
      m_ScriptId    = scrid;
      m_ScriptType  = SCRIPT_TYPE_COMMON;
      m_MsgArcId    = getMessageArcId( tbl[ii].message_arc_id );
      m_MsgDatId    = tbl[ii].message_dat_id;
      m_ZoneID      = zoneId;
      m_AmxId       = tbl[ii].amx_id;
      m_AmxFileName = sc_ScriptFileNameCommon[ m_AmxId ];
      m_RegFuncId   = ScriptObject::GetRegistID( m_ZoneID, m_AmxId );
      GFL_PRINT( "#SetScriptInfo: Common %d: zone=%d, amx=%d, msg_arc=%d, msg=%d %s\n", m_ScriptId, m_ZoneID, m_AmxId, m_MsgArcId, m_MsgDatId, m_AmxFileName);
      //tbl[ii].b_resident 
      return;
    }
  }
  
  // ゾーンスクリプト読み込み
  if( scrid >= ID_START_SCR_OFFSET )
  {
    m_ScriptId    = scrid;
    m_ScriptType  = SCRIPT_TYPE_ZONE;
    m_MsgArcId    = print::GetMessageArcId_Script();
    m_MsgDatId    = zone->GetMessageArcID();
    m_ZoneID      = zoneId;
    m_AmxId       = ScriptObject::AMX_ID_NULL;
    m_AmxFileName = "test.amx"; //System::GetInstance()->GetAmxName( SCRIPT_TYPE_ZONE, m_ZoneID );
    m_RegFuncId   = ScriptObject::GetRegistID( m_ZoneID, ScriptObject::AMX_ID_NULL );
    GFL_PRINT( "#SetScriptInfo: Zone %d: zone=%d\n", scrid, m_ZoneID);
    return;
  }
}

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
const char * ScriptInfo::GetAmxName( SCRIPT_TYPE scrType, u16 index )
{
  static const char * dummy_string = "";
#if 0
  switch ( scrType )
  {
  case SCRIPT_TYPE_ZONE:
    GFL_ASSERT( index < GFL_NELEMS(sc_ScriptFileNameZone) );
    return sc_ScriptFileNameZone[ index ];

  case SCRIPT_TYPE_COMMON:
    GFL_ASSERT( index < GFL_NELEMS(sc_ScriptFileNameCommon) );
    return sc_ScriptFileNameCommon[ index ];

  case SCRIPT_TYPE_AI:
    GFL_ASSERT( index < GFL_NELEMS(sc_ScriptFileNameAI) );
    return sc_ScriptFileNameAI[ index ];

  case SCRIPT_TYPE_ZONEINIT:
    GFL_ASSERT( index < GFL_NELEMS(sc_ScriptFileNameZoneInit) );
    return sc_ScriptFileNameZoneInit[ index ];
  }
  GFL_ASSERT_MSG(0, "scrType=%d, index=%d\n", scrType, index);
#endif
  return dummy_string;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 共通スクリプトの数を取得
 *
 * @return 共通スクリプトの数
 */
/* ------------------------------------------------------------------------- */
u32 ScriptInfo::GetCommonAmxNum( void )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  return pSys->GetScriptArcTableSize() / sizeof( SCRIPT_ARC_TABLE );
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )


