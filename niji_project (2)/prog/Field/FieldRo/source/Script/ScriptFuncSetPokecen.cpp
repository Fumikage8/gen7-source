//==============================================================================
/**
 * @file        ScriptFuncSetPokecen.cpp
 * @brief       ScriptFuncSetCommon系 ポケモンセンター系統
    
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

//Pokecen
#include "Field/FieldRos/FieldGimmickPokecen/include/FieldGimmickPokecen.h"
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
 * @func        PokecenInit
 * @brief       ポケセンの複雑な初期化処理
 * @author      Ohhira Syo
 * @date        2015/11/12(木) 16:58:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PokecenInit( AMX* amx, const cell* ptr )
{
  ScriptObject*                    pObj                = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*            pGameManager        = pObj->GetGameManager();

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickPokecen* gimmick_pokecen = static_cast<FieldGimmickPokecen*>(gimmick);

  GFL_ASSERT(gimmick_pokecen);

  gimmick_pokecen->MachineInit();

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        PokecenBallSetVisible
 * @brief       ポケセンのボールの表示オンオフ
 * @author      Ohhira Syo
 * @date        2015/11/13(金) 15:42:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PokecenBallSetVisible( AMX* amx, const cell* ptr )
{
  ScriptObject*                    pObj                = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*            pGameManager        = pObj->GetGameManager();

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickPokecen* gimmick_pokecen = static_cast<FieldGimmickPokecen*>(gimmick);

  GFL_ASSERT(gimmick_pokecen);

  s32 ball_index = ptr[1];
  bool visible = GFL_BOOL_CAST(ptr[2]);

  gimmick_pokecen->MachineBallSetVisible(ball_index, visible);

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        StaticModelMotionByStaticIdPlay_Pokecen
 * @brief       ポケセン用の補間Play
 * @author      Ohhira Syo
 * @date        2015/12/08(火) 17:10:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::StaticModelMotionByStaticIdPlay_Pokecen(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *pFieldmap = p_gman->GetFieldmap();
  
  u32 mode = ptr[1];
  s32 staticId  = ptr[2];

  StaticModel::StaticModel *pStaticModel = pFieldmap->GetStaticModelManager()->GetStaticModelIDStaticModel( staticId );

  GFL_ASSERT( pStaticModel );

  switch(mode){
  case 0:
    {
      s32   AnimId  = ptr[3];
      f32   speed = amx_ctof(ptr[4]);
      u32   lerpTime = ptr[5];

      u32 slotIndex = 0;
      bool isLoop = false;
      
      StaticModel::StaticModel::Handle_StartEventAnimationEx handle( slotIndex, AnimId, speed, isLoop, lerpTime );
      if( pStaticModel )  pStaticModel->Set( &handle);
    }
    break;
  case 1:
    {
      StaticModel::StaticModel::Handle_StartLoopAnimation handle( 0 );
      if( pStaticModel )  pStaticModel->Set( &handle);
    }
    break;
  }
  
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        PokecenFinish
 * @brief       ポケセンのInitで行ったことの後始末
 * @author      Ohhira Syo
 * @date        2015/11/12(木) 16:58:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PokecenFinish( AMX* amx, const cell* ptr )
{
  ScriptObject*                    pObj                = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*            pGameManager        = pObj->GetGameManager();

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickPokecen* gimmick_pokecen = static_cast<FieldGimmickPokecen*>(gimmick);

  GFL_ASSERT(gimmick_pokecen);

  gimmick_pokecen->MachineFin();

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        PokecenGimmickAnimeReq
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PokecenGimmickAnimeReq( AMX* amx, const cell* ptr )
{
  ScriptObject*                    pObj                = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*            pGameManager        = pObj->GetGameManager();

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickPokecen* gimmick_pokecen = static_cast<FieldGimmickPokecen*>(gimmick);

  GFL_ASSERT(gimmick_pokecen);

  u32 model_id = ptr[1];
  bool is_on = GFL_BOOL_CAST(ptr[2]);

  gimmick_pokecen->BirthdayAnimStart(model_id, is_on);

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        PokecenGimmickAnimeIsEnd
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PokecenGimmickAnimeIsEnd( AMX* amx, const cell* ptr )
{
  ScriptObject*                    pObj                = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager*            pGameManager        = pObj->GetGameManager();

  FieldGimmick* gimmick = pGameManager->GetFieldmap()->GetFieldGimmick(Fieldmap::GIMMICK_IDX_ZONE);
  FieldGimmickPokecen* gimmick_pokecen = static_cast<FieldGimmickPokecen*>(gimmick);

  GFL_ASSERT(gimmick_pokecen);

  u32 model_id = ptr[1];

  return gimmick_pokecen->BirthdayAnimIsEnd(model_id);
}


//-----------------------------------------------------------------------------
/**
 *  @brief  てもちのポケルスチェック
 *
 *  @retval true:ポケルスがいる
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PokerusCheckTemoti( AMX *amx, const cell * )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  const pml::PokeParty    *p_party  = p_gman->GetGameData()->GetPlayerPartyConst();

  return p_party->CheckPokerusExist();
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

