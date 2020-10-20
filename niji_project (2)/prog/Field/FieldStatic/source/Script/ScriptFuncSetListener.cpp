//=============================================================================
/**
 * @file  ScriptFuncSetListener.cpp
 * @brief スクリプトコマンドが用いるリスナー系常駐クラス配置場所
 */
//=============================================================================

#include <heap/include/gfl2_Heap.h>

#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "SaveData/include/MyItemSave.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptWork.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetListener.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//-----------------------------------------------------------------------------
/**
 *  @brief  PC(ポケモンボックス)呼び出し
 */
//-----------------------------------------------------------------------------
void ScriptBoxEvenListener::OnTermination(u8 outJumpMode, u8 outTray,u8 outPos)
{
  ScriptObject* p_obj = m_pScriptSystem->GetScriptObject();
  Work* p_work = p_obj->GetWork();

  switch(m_Type){
  case App::Box::BOX_CALL_PC:
    p_work->SetTempWorkValue( SCWK_ANSWER, outJumpMode );
    break;
  case App::Box::BOX_CALL_FIELD_TRADE:
    p_work->SetTempWorkValue( SCWK_RET0, outJumpMode );
    p_work->SetTempWorkValue( SCWK_RET1, outTray );
    p_work->SetTempWorkValue( SCWK_RET2, outPos );
    break;
  }
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

