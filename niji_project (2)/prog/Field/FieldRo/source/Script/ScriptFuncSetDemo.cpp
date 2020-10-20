//==============================================================================
/**
 * @file        ScriptFuncSetDemo.cpp
 * @brief       ScriptFuncSetCommon系 デモ系統
    
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/SaveData.h"
#include <arc_def_index/arc_def.h>

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Event/FieldEventFieldDemo.h"

#include "App/AppEvent/include/FieldDemo/FieldDemoEvent.h"



#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

//------------------------------------------------------------------------------
/**
* @brief       フィールドデモイベントコール
*
* @param[in,out] AMX*          amx .
* @param[in]     const cell *  ptr ポインタ.
*
* @return      イベント結果(今は0固定)
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CallFieldDemoEvent_( AMX *amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();

  u32                     demoID          = static_cast<u32>( ptr[1]);
  bool                    isFieldCapture  = GFL_BOOL_CAST( ptr[2]);

  // テロップが再生できる設定にしました
  // 他にスクリプトから呼ばれるものがあるのであればdemoIDごとにnative側で設定するのが良いかもです
  {
    EventFieldDemo* p_event;
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldDemo > ( p_gman->GetGameEventManager() );

    u32 option = EventFieldDemo::OPTION_NONE;
    if( isFieldCapture)
    {
      option |= EventFieldDemo::OPTION_USE_FIELD_DRAW_CAPTURE;
    }

    u32 unUseOption = EventFieldDemo::UN_USE_OPTION_CHARA_MODEL | EventFieldDemo::UN_USE_OPTION_POKEMON;

    DemoLib::Sequence::SequenceRenderingPipeLine::UseOption renderUseOption;
    renderUseOption.isUseModelRenderPathAfterEdge = false;
    renderUseOption.isUseSkyRenderPath = false;
    renderUseOption.isUseEdgeMapPath = false;
    renderUseOption.isUseModelOutLinePath = false;
    renderUseOption.isUseLayoutRenderPath = true;
    renderUseOption.isUseScreenEffectRenderPath = false;
    renderUseOption.isUseBloomRenderPath = false;

    EventFieldDemo::Description desc;
    desc.pFieldmap        = p_gman->GetFieldmap();
    desc.demoID           = demoID;
    desc.option           = option;
    desc.unUseOption      = unUseOption;
    desc.renderUseOption  = renderUseOption;
    desc.isForceOneStep   = true;
    desc.textureIndex     = 0;
    desc.pTextureNode     = NULL;

    p_event->Initialize( desc);
  }

  // ウェイト
  pSys->Suspend();

  return 0;
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
