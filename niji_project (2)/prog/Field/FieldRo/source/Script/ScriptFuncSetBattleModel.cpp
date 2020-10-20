//==============================================================================
/**
 * @file        ScriptFuncSetBattleModel.cpp
 * @brief       ScriptFuncSetCommon系 バトルモデル系統
    
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "Savedata/include/SaveData.h"


#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptTrialModel.h"

#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )


//------------------------------------------------------------------------------
/**
 * @func        BtlPokeMdlLoad
 * @brief       MonsNoで指定されたバトルポケモンのモデルリソースをメモリに読み込む.
 * @author      N.Takeda
 * @date        2015/09/09(水) 15:28:04
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeMdlLoad_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 ModelDefine = ptr[1];
  s32 MonsNo      = ptr[2];
  s32 FormNo      = ptr[3];
  s32 EventMotionPac = ptr[4];
  
  return p_sys->GetScriptTrialModel()->Load( ModelDefine, EventMotionPac, MonsNo, FormNo );
}

//------------------------------------------------------------------------------
/**
 * @func        BtlPokeMdlDel
 * @brief       指定されたIDのフィールド用バトルポケモンモデルを削除する.
 * @author      N.Takeda
 * @date        2015/09/09(水) 15:28:33
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeMdlDel_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 ModelDefine = ptr[1];
  
  p_sys->GetScriptTrialModel()->Del( ModelDefine );
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        BtlPokeResorceUnload
 * @brief       BtlPokeMdlLoad, BtlPokeMotLoad でメモリにバッファされたバトルポケモン関連のリソースをすべて破棄する.
 * @author      N.Takeda
 * @date        2015/09/09(水) 15:28:35
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeResorceUnload_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  p_sys->GetScriptTrialModel()->UnLoad();
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        BtlPokeMdlIsDone_
 * @brief       指定されたIDのフィールド用バトルポケモンモデルのモーション再生を終了待ちする.
 * @author      N.Takeda
 * @date        2015/09/09(水) 15:28:39
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeMdlIsDone_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  return p_sys->GetScriptTrialModel()->IsDone();
}

//------------------------------------------------------------------------------
/**
 * @func        BtlPokeMotionPlay
 * @brief       指定されたIDのフィールド用バトルポケモンモデルに、モーションを再生する.
 * @author      N.Takeda
 * @date        2015/09/09(水) 15:28:37
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeMotionPlay_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 ModelDefine = ptr[1];
  s32 MotionID = ptr[2];
//  s32 BlendFrame = ptr[3];
  
  p_sys->GetScriptTrialModel()->PlayAnim( ModelDefine, MotionID );
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        BtlPokeMotionPlayIsEnd_
 * @brief       BtlPokeMotionPlayIsEnd_関数.
 * @author      N.Takeda
 * @date        2015/09/11(金) 20:05:01
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeMotionPlayIsEnd_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 ModelDefine = ptr[1];
  
  return p_sys->GetScriptTrialModel()->PlayIsEnd( ModelDefine );
}

//------------------------------------------------------------------------------
/**
 * @func        BtlPokeDisp
 * @brief       指定されたIDのフィールド用バトルポケモンモデルを表示／非表示にする.
 * @author      N.Takeda
 * @date        2015/09/09(水) 15:28:42
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeDisp_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 ModelDefine = ptr[1];
  bool disp = GFL_BOOL_CAST(ptr[2]);
  
  p_sys->GetScriptTrialModel()->Disp( ModelDefine, disp );
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        BtlPokeShadowDisp
 * @brief       指定されたIDのフィールド用バトルポケモンモデルの足元影を表示／非表示にする.
 * @author      N.Takeda
 * @date        2015/09/09(水) 15:28:44
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeShadowDisp_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32 ModelDefine = ptr[1];
  bool disp = GFL_BOOL_CAST(ptr[2]);
  
  p_sys->GetScriptTrialModel()->SetShadowVisible( ModelDefine, disp );
  
  return true;
}

cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeAmbientLightOverWrite( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  gfl2::math::Vector4 color;
  color.x = amx_ctof( ptr[1] );
  color.y = amx_ctof( ptr[2] );
  color.z = amx_ctof( ptr[3] );
  color.w = amx_ctof( ptr[4] );

  p_gman->GetFieldmap()->GetTrialModelFactory()->SetOverWriteAmbientColor( color );
  return true;
}

cell AMX_NATIVE_CALL FuncSetCommon::BtlPokeAmbientLightRestore( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  p_gman->GetFieldmap()->GetTrialModelFactory()->ClearOverWriteAmbientColor();
  return true;
}




GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
