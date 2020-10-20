//==============================================================================
/**
 * @file        ScriptFuncSetEffect.cpp
 * @brief       ScriptFuncSetCommon系 エフェクト制御系統
    
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"

#include "Savedata/include/SaveData.h"
#include <arc_def_index/arc_def.h>

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#include "Field/FieldRo/include/Fieldmap.h"

// Effect
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFade.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFadeSpot.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRotom.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFestivalFire.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectGetItem.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrial3.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrial5.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5ForModel.h"

// MotionBlur
#include "System/include/MotionBlur.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

// Env
#include "Field/FieldRo/include/FieldEnvManager.h"

#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// fade
#include <Fade/include/gfl2_FadeSuper.h>

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )


//------------------------------------------------------------------------------
/**
 * @func        EffectSetPos
 * @brief       位置に付随するエフェクトを作成.
 * @author      N.Takeda
 * @date        2015/06/11(木) 19:01:01
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectSetPos( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32   id          = ptr[1];
  s32   effect_type = ptr[2];
  f32   fX          = amx_ctof(ptr[3]);
  f32   fY          = amx_ctof(ptr[4]);
  f32   fZ          = amx_ctof(ptr[5]);
  bool  se_play     = GFL_BOOL_CAST(ptr[6]);
  s32   iScale      = ptr[7];
  
  gfl2::math::Vector3 pos = p_sys->CalcEventPositionWorld( fX, fY, fZ );
  p_sys->GetScriptEffectManager()->SetPos( id, effect_type, pos.x, pos.y, pos.z, se_play );
  
  if( iScale != 0 ){
    p_sys->GetScriptEffectManager()->SetScale( id, static_cast<f32>(iScale) / 100.f );
  }
  return 0;
}

//------------------------------------------------------------------------------
/**
* @func        EffectSetPosScaleRot
* @brief       位置に付随するエフェクトを作成. + スケール/回転データ受け渡し機能
* @author      kawazoe_shinichi
* @date        2017/01/31(火) 16:15:39
*
* @param[in,out] AMX*          amx .
* @param[in]     const cell *  ptr ポインタ.
*
* @return      cell AMX_NATIVE_CALL
*
* @com 相対座標対応コマンド
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectSetPosScaleRot(AMX* amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys = p_gman->GetFieldScriptSystem();

  s32   id = ptr[1];
  s32   effect_type = ptr[2];
  f32   fX = amx_ctof(ptr[3]);
  f32   fY = amx_ctof(ptr[4]);
  f32   fZ = amx_ctof(ptr[5]);
  bool  se_play = GFL_BOOL_CAST(ptr[6]);
  f32   fScale = amx_ctof(ptr[7]);
  f32   fRotX = amx_ctof(ptr[8]);
  f32   fRotY = amx_ctof(ptr[9]);
  f32   fRotZ = amx_ctof(ptr[10]);

  gfl2::math::Vector3 pos = p_sys->CalcEventPositionWorld(fX, fY, fZ);
  p_sys->GetScriptEffectManager()->SetPosScaleRot(id, effect_type, pos.x, pos.y, pos.z, fScale, se_play, fRotX, fRotY, fRotZ);

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectSetChr
 * @brief       キャラに付随するエフェクトを作成.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:28:39
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectSetChr( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();

  s32     id          = ptr[1];
  s32     effect_type = ptr[2];
  u32     EventID     = ptr[3];                   ///< chr_id
  bool    se_play = GFL_BOOL_CAST(ptr[4]);
  f32 step_frame = amx_ctof( ptr[5] );
  f32 scale = amx_ctof( ptr[6] );


  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false; 
  }
  pSys->GetScriptEffectManager()->SetChr( id, effect_type, chrID, se_play, step_frame, scale );
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectSetTrainerEye
 * @brief       キャラに付随するエフェクトを作成.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:28:39
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectSetTrainerEye( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();

  s32  id       = ptr[1];
  u32  EventID = ptr[2];                 ///< chr_id
  u32  TargetEventId = ptr[3];
  u32  EyeSeID = ptr[4];

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  u32   targetChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(TargetEventId);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  Field::MoveModel::FieldMoveModel* pTargetModel = pFieldMoveModelManager->GetFieldMoveModel( targetChrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  if( pTargetModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", TargetEventId );
    return false;
  }

  {
    //カメラ取得
    Camera::CameraManager* camera = p_gman->GetFieldmap()->GetCameraManager();
    //ターゲット１の目の位置を取得
    gfl2::math::Vector3 t_pos1 = GetLocaterPos( pCharaModel->GetCharaDrawInstance(), CHR_LOC_EYE_W );
    //ターゲット２の目の位置を取得
    gfl2::math::Vector3 t_pos2 = GetLocaterPos( pTargetModel->GetCharaDrawInstance(), CHR_LOC_EYE_W );
    //射影変換
    gfl2::math::Vector3 t1 = GetProjPosition( camera, t_pos1 );
    gfl2::math::Vector3 t2 = GetProjPosition( camera, t_pos2 );
      
    f32 rot = 0;
    gfl2::math::VEC2 pos(0,0);

    if (t1 != t2)
    {
      //方向ベクトルを求める
      gfl2::math::Vector3 dir = t2 - t1;
      gfl2::math::Vector3 base_dir(1,0,0);
      dir = dir.Normalize();
      //回転量を求める
      f32 cos = base_dir.Dot( dir );
      {
        gfl2::math::Vector2 dir_2d;
        dir_2d.x = dir.x;
        dir_2d.y = dir.y;
        gfl2::math::Vector2 base_dir_2d(1, 0);
        f32 cross = base_dir_2d.Cross(dir_2d);
        if (cross < 0)  cos *= -1;
      }
      f32 rad = gfl2::math::FACos(cos);
      rot = gfl2::math::ConvRadToDeg( rad );
      //原点から直線への垂線の交点を求め、原点からこの点へのベクトルを求める
      gfl2::math::Vector3 vec1 = dir;
      gfl2::math::Vector3 vec2 = -t1;
      f32 dot = vec1.Dot(vec2);
      gfl2::math::Vector3 p = t1 + dir * dot;
      pos.x = p.x;
      pos.y = p.y;
    }
    pSys->GetScriptEffectManager()->SetTrainerEye( id, pos, rot, EyeSeID );
  }
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectWait
 * @brief       エフェクトの終了状態を得る.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:28:38
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectWait_( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  
  s32                     id        = ptr[1];
  
  return pSys->GetScriptEffectManager()->IsEnd( id );
}

//------------------------------------------------------------------------------
/**
 * @func        EffectDel
 * @brief       エフェクトの削除.
 * @author      N.Takeda
 * @date        2015/06/12(金) 14:28:36
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectDel( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  
  s32                     id        = ptr[1];
  pSys->GetScriptEffectManager()->Delete( id );
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectNextRotom
 * @brief       ロトム図鑑1枚絵エフェクトを次の絵に進める.
 * @author      saita_kazuki
 * @date        2015/11/10(火) 16:35
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectNextRotom( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();

  s32                     id        = ptr[1];
  
  Field::Effect::IEffectBase* pEffect = pSys->GetScriptEffectManager()->GetEffect( id );
  GFL_ASSERT_STOP_MSG( pEffect, "ロトム図鑑説明1枚絵エフェクトを生成してください\n");

  Field::Effect::EffectRotom *pEffectRotom = reinterpret_cast<Field::Effect::EffectRotom*>( pEffect );
  pEffectRotom->NextAnimation();

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectAllDel
 * @brief       全てのエフェクトを削除.
 * @author      N.Takeda
 * @date        2015/11/12(木) 13:40:03
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectAllDel(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  
  pSys->GetScriptEffectManager()->AllDelete();
  
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        EffectAllDel
 * @brief       エフェクトリソースを開放.
 * @author      N.Takeda
 * @date        2016/03/10(木) 14:58:13
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectUnloadResource(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  Field::Effect::EffectManager  *pEffMgr = p_gman->GetFieldmap()->GetEffectManager();
  
  s32   iEffectType = ptr[1];
  
  s32   iCnt = pEffMgr->GetEffectNum( pSys->GetScriptEffectManager()->TranslateId( iEffectType ) );
  if( iCnt != 0 ) return false;
  
  // エフェクトの非常駐リソースを破棄
  pSys->GetScriptEffectManager()->ResourceReleaseRequest( iEffectType );  ///< Mcat#1560:試練４：キアヴェの試練開始直後にハング 対応でリクエスト形式に
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        EffectFestivalFireStart
 * @brief       EffectFestivalFireStart関数.
 * @author      N.Takeda
 * @date        2015/12/08(火) 19:00:39
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectFestivalFireStart(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  s32   iId = ptr[1];
  
  Field::Effect::IEffectBase  *pp = pSys->GetScriptEffectManager()->GetEffect( iId );
  if( pp )  reinterpret_cast<Field::Effect::EffectFestivalFire*>(pp)->Start();
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectFestivalFireStart
 * @brief       EffectFestivalFireStart関数.
 * @author      N.Takeda
 * @date        2015/12/08(火) 19:00:39
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectFestivalFireEnd(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  s32   iId = ptr[1];
  
  Field::Effect::IEffectBase  *pp = pSys->GetScriptEffectManager()->GetEffect( iId );
  if( pp )  reinterpret_cast<Field::Effect::EffectFestivalFire*>(pp)->End();
  pSys->GetScriptEffectManager()->Delete( iId );
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectFestivalFireStart
 * @brief       EffectFestivalFireStart関数.
 * @author      N.Takeda
 * @date        2015/12/08(火) 19:00:39
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectFestivalFireIsAlive(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  s32   iId = ptr[1];
  
  Field::Effect::IEffectBase  *pp = pSys->GetScriptEffectManager()->GetEffect( iId );
  if( pp )  return reinterpret_cast<Field::Effect::EffectFestivalFire*>(pp)->IsAlive();
  
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectGetItemStart
 * @brief       ゲットアイテム(1枚絵表示)のエフェクト生成
 * @author      N.Takeda
 * @date        2015/12/08(火) 19:00:39
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectGetItemStart(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();

  u32 id = ptr[1];
  GetItemID getItemID = static_cast<GetItemID>( ptr[2] );

  if( getItemID >= GET_ITEM_MAX)
  {
    GFL_ASSERT_MSG( 0, "Invalid get item ID\n");
    getItemID = static_cast<GetItemID>( 0);
  }

  s32 effect_type = FuncSetCommon::ConvertGetItemIDToScriptEffectID( getItemID );

  gfl2::math::Vector3 pos = pSys->CalcEventPositionWorld( 0.0f, 0.0f, 0.0f );
  pSys->GetScriptEffectManager()->SetPos( id, effect_type, pos.x, pos.y, pos.z, false );
  
  Effect::IEffectBase  *pEffect = pSys->GetScriptEffectManager()->GetEffect( id );
  if( pEffect == NULL)
  {
    GFL_ASSERT_MSG( 0, "GetItem Create Failed\n");
    return NULL;
  }

  Effect::EffectGetItem* pGetItem = reinterpret_cast<Effect::EffectGetItem*>( pEffect);
  s32 index = FuncSetCommon::ConvertGetItemIDToGetItemIndex( getItemID);
  pGetItem->Start( index);

  return NULL;
}

/**
 *  @brief  試練3エフェクト
 */
cell AMX_NATIVE_CALL FuncSetCommon::EffectTrial3Start(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  s32   iId = ptr[1];
  s32   kind = ptr[2];
  
  Field::Effect::IEffectBase *pEffect = pSys->GetScriptEffectManager()->GetEffect( iId );
  if( pEffect )
  {
    Field::Effect::EffectTrial3 *pTrial = reinterpret_cast<Field::Effect::EffectTrial3*>(pEffect);
    pTrial->CreateEffect( kind );
  }
  
  return NULL;
}

/**
 *  @brief  試練5エフェクト
 */
cell AMX_NATIVE_CALL FuncSetCommon::EffectTrial5Start(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  s32   iId = ptr[1];
  s32   kind = ptr[2];
  
  Field::Effect::IEffectBase *pEffect = pSys->GetScriptEffectManager()->GetEffect( iId );
  if( pEffect )
  {
    Field::Effect::EffectTrial5 *pTrial = reinterpret_cast<Field::Effect::EffectTrial5*>(pEffect);
    pTrial->CreateEffect( kind );
  }
  
  return NULL;
}


/**
*  @brief  新電気試練エフェクト（試練5）
*/
cell AMX_NATIVE_CALL FuncSetCommon::S5_EffectChange(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   iId = ptr[1];
  s32   kind = ptr[2];

  Field::Effect::IEffectBase *pEffect = pSys->GetScriptEffectManager()->GetEffect(iId);
  if (pEffect)
  {
    if (kind >= EFF_S5_MDL_KUWA_BEAM1)
    {
      // モデルタイプ
      Field::Effect::EffectNewTrial5ForModel *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5ForModel*>(pEffect);
      pTrial->CreateEffect(kind);
    }
    else
    {
      // パーティクルタイプ
      Field::Effect::EffectNewTrial5 *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5*>(pEffect);
      pTrial->CreateEffect(kind);
    }
   
  }

  return NULL;
}


cell AMX_NATIVE_CALL FuncSetCommon::S5_EffectFade(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *pSys = p_gman->GetFieldScriptSystem();
  s32   iId = ptr[1];

  Field::Effect::IEffectBase *pEffect = pSys->GetScriptEffectManager()->GetEffect(iId);
  if (pEffect)
  {
    if ( pEffect->GetType() == Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_02 )
    {
      // モデルタイプ
      Field::Effect::EffectNewTrial5ForModel *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5ForModel*>(pEffect);
    }
    else if ( pEffect->GetType() == Field::Effect::EFFECT_TYPE_DEMO_NEW_TRIAL5_01 )
    {
      // パーティクルタイプ
      Field::Effect::EffectNewTrial5 *pTrial = reinterpret_cast<Field::Effect::EffectNewTrial5*>(pEffect);
      pTrial->Fade();
    }
  }

  return NULL;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief レイアウトフェードの呼び出し（IN）
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::LayoutFadeIn( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  s32                     sLife    = ptr[1];

  Field::Effect::EffectFade* pFade = reinterpret_cast<Field::Effect::EffectFade*>( pSys->GetEffect() );
  if( pFade ) pFade->RequestIn( sLife );
  
  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief レイアウトフェードの呼び出し(OUT)
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::LayoutFadeOut( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  
  Field::Effect::EffectManager  *pEffMgr = p_gman->GetFieldmap()->GetEffectManager();
  s32                     sLife    = ptr[1];
  s32                     iAlpha   = ptr[2];
  
#if PM_DEBUG
//  if( iAlpha == 255 ) iAlpha = 200;
#endif  // PM_DEBUG
  if( iAlpha == 0 ) iAlpha = 255;                   ///< フェードかけない事はないはずなので.
  if( pSys->GetEffect() )
  {
    pEffMgr->DeleteEffect( pSys->GetEffect() );
    pSys->SetEffect( NULL );
  }
  
  gfl2::math::Vector3 vPos_head;
  {
    Field::MoveModel::FieldMoveModel *pPlayer = p_gman->GetFieldmap()->GetPlayerCharacter();
    poke_3d::model::BaseModel *pBaseModel = pPlayer->GetCharaDrawInstance();
    s32 joint_index = pBaseModel->GetJointIndex( "CamTgtHead" );
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode  *pJointNode = pNode->GetJointInstanceNode( joint_index );
    vPos_head = pJointNode->GetWorldMatrix().GetElemPosition();
  }
  Field::Effect::IEffectBase    *pEffect = pEffMgr->CreateEffect( Field::Effect::EFFECT_TYPE_FADE, vPos_head );
  if( pEffect ) pSys->SetEffect( pEffect );
  
  gfl2::math::Vector4 colS( 0.f, 0.f, 0.f, 0.f );
  gfl2::math::Vector4 colE( 0.f, 0.f, 0.f, static_cast<f32>(iAlpha) );
  
  Field::Effect::EffectFade* pFade = reinterpret_cast<Field::Effect::EffectFade*>( pSys->GetEffect() );
  if( pFade ) pFade->RequestOut( &colS, &colE, sLife );
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        LayoutFadeIsEnd
 * @brief       LayoutFadeIsEnd関数.
 * @author      N.Takeda
 * @date        2015/09/01(火) 15:00:09
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::LayoutFadeIsEnd( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  
  Field::Effect::EffectFade* pFade = reinterpret_cast<Field::Effect::EffectFade*>( pSys->GetEffect() );
  return (( pFade ) ? pFade->IsAnimationLastFrame() : true);
}

cell AMX_NATIVE_CALL FuncSetCommon::FadeStateSave( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj      = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman     = p_obj->GetGameManager();
  GameSys::GameData       *p_gamedata = p_gman->GetGameData();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  
  Field::FieldScript::GameData  *p_script_gamedata = p_gamedata->GetFieldScriptGameData();
  
  
  // ゲームデータに保存

  // 上画面
  p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_UPPER )->result = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade( gfl2::Fade::DISP_UPPER )->GetFadeResult();
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade( gfl2::Fade::DISP_UPPER )->GetColor(
    &p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_UPPER )->start_col,
    &p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_UPPER )->end_col
    );

  
  // 下画面
  p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_LOWER )->result = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade( gfl2::Fade::DISP_LOWER )->GetFadeResult();
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade( gfl2::Fade::DISP_LOWER )->GetColor(
    &p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_LOWER )->start_col,
    &p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_LOWER )->end_col
    );

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetCommon::FadeStateRecover( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj      = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman     = p_obj->GetGameManager();
  GameSys::GameData       *p_gamedata = p_gman->GetGameData();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  
  Field::FieldScript::GameData  *p_script_gamedata = p_gamedata->GetFieldScriptGameData();

  u32                     sync    = ptr[1];

  // 上画面
  if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade( gfl2::Fade::DISP_UPPER )->GetFadeResult() != p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_UPPER )->result )
  {
    if( p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_UPPER )->result == gfl2::Fade::FADE_RESULT_IN )
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, sync);
    }
    else
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, NULL, &p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_UPPER )->end_col, sync );
    }
  }

  // 下画面
  if( GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade( gfl2::Fade::DISP_LOWER )->GetFadeResult() != p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_LOWER )->result )
  {
    if( p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_LOWER )->result == gfl2::Fade::FADE_RESULT_IN )
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, sync);
    }
    else
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, NULL, &p_script_gamedata->GetFadeStateSaveData( gfl2::Fade::DISP_LOWER )->end_col, sync );
    }
  }


  // ゲームデータに保存されている情報を使用してフェードを発行

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectSetLoc
 * @brief       EffectSetLoc関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:44
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectSetLoc( AMX *amx, const cell *ptr )
{
#ifdef UNDER_CONSTRUCTION
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32         effect_id = ptr[1];                   ///< chr_id
  s32         FieldLocatorID = ptr[2];
  s32         genelate_fade_flame = ptr[3];                   ///< Frame
  
#endif
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        EffectDisp
 * @brief       EffectDisp関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:45
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EffectDisp( AMX *amx, const cell *ptr )
{
#ifdef UNDER_CONSTRUCTION
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32         effect_id = ptr[1];                   ///< chr_id
  bool        disp = GFL_BOOL_CAST(ptr[2]);
  
#endif
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        BloomChange
 * @brief       BloomChange関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:50
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   intensity 輝度の割合
 * @param[in]     const cell*   frame 変化にかけるフレーム数
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::BloomChange( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32         intensity = ptr[1];   ///< Intensity
  s32         frame = ptr[2];       ///< Frame

  Fieldmap* pFieldmap = p_gman->GetFieldmap();
  EnvManager* pEnvManager = pFieldmap->GetFieldEnvManager();

  pEnvManager->ChangeBloom( intensity, frame);

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラーシステムのデストラクタ
 */
//-----------------------------------------------------------------------------
bool FuncSetCommon::SCREND_CheckEndMotionBlur(  GameSys::GameManager* p_gman, s32* seq )
{
  FieldScriptSystem *p_sys     = p_gman->GetFieldScriptSystem();
  Fieldmap          *pFieldmap = p_gman->GetFieldmap();

  if( !pFieldmap ){
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_MOTION_BLUR ); 
    return true;
  }

  System::MotionBlur      *pMotionBlur = pFieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  switch( *seq ){
  case 0:
    // モーションブラー終了
    pMotionBlur->RenderOff();  //複数回呼ばれても副作用はない
    (*seq)++;
    return false;
  case 1:
    pMotionBlur->Terminate();
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_MOTION_BLUR ); 
    (*seq)++;
    break;
  default:
    break;
  }

  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラーシステムを初期化する
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::InitMotionBlur( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  pMotionBlur->InitializeMotionBlur(
    p_gman->GetFieldResident()->GetFieldDeviceHeapAllocator(),
    p_gman->GetFieldResident()->GetFieldDeviceHeap(),
    p_gman->GetAsyncFileManager(),
    gfl2::Fade::FadeManager::COLOR_BUFFER_LOWER
  );
  p_sys->GetSystemWork()->OnCheckBit( SCR_END_CHECK_BIT_MOTION_BLUR );

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラーシステムの初期化を待つ
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::WaitInitMotionBlur( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  return pMotionBlur->InitializeMotionBlurUpdate();
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラーシステムを破棄する
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EndMotionBlur( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  pMotionBlur->Terminate();

  p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_MOTION_BLUR );

  //@attention アッパーでSuspend候補
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラー開始
 *
 *  @param blend_type ブレンドタイプ
 *  @param anim_type  アニメーションタイプ
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::OnMotionBlur( AMX * amx, const cell * ptr )
{
  u32 blend_type = ptr[1];
  u32 anim_type = ptr[2];

  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  // モーションブラー開始
  pMotionBlur->RenderStart(
    static_cast<System::MotionBlur::BlendType>(blend_type),
    static_cast<System::MotionBlur::AnimeType>(anim_type),
    System::GflUse::GetRenderTargetColor( gfl2::gfx::CtrDisplayNo::LEFT ),//gfl2::grp::DISPLAY_UPPER,
    true
  );

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラー終了
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::OffMotionBlur( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  // モーションブラー終了
  pMotionBlur->RenderOff();

  //@attention アッパーでSuspend候補
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラー　ブレンド比コントロール開始
 *
 *  @param  end   終了ブレンド値 -0.5～0.5
 *  @param  type  補間タイプ
 *  @param  frame 補間フレーム
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ControlMotionBlur( AMX * amx, const cell * ptr )
{
  f32 end = amx_ctof( ptr[1] );
  u32 type = ptr[2];
  u32 frame = ptr[3];

  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  {
    System::MotionBlur::Param param;

    param.end = end;
    param.type = static_cast<System::MotionBlur::Type>(type);
    param.frame = frame;

    pMotionBlur->SetBlendControl(param);
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラー　ブレンド比コントロール終了待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::WaitControlMotionBlur( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  return pMotionBlur->IsEndBlendControl();
}


//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラー　カラーフィルター開始
 *
 *  @param  r   終了カラー加減算(float) -1.0～1.0
 *  @param  g   終了カラー加減算(float) -1.0～1.0
 *  @param  b   終了カラー加減算(float) -1.0～1.0
 *  @param  type  補間タイプ
              SCR_MB_CONT_SET       = 0,    ///< 設定
              SCR_MB_CONT_LINEAR,           ///< 線形補間
              SCR_MB_CONT_SPLINE,           ///< スプライン補間
              SCR_MB_CONT_QUAD,              ///< 2次関数グラフ補間
              SCR_MB_CONT_LOG10,            ///< 常用対数グラフ補間
 *  @param  frame 補間フレーム
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ControlMotionBlurColor( AMX * amx, const cell * ptr )
{
  f32 r = amx_ctof( ptr[1] );
  f32 g = amx_ctof( ptr[2] );
  f32 b = amx_ctof( ptr[3] );
  u32 type = ptr[4];
  u32 frame = ptr[5];

  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  {
    System::MotionBlur::ColorParam param;

    param.end = gfl2::math::Vector3( r,g,b );
    param.type = static_cast<System::MotionBlur::Type>(type);
    param.frame = frame;

    pMotionBlur->SetColorFilter(param);
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  モーションブラー　カラーフィルターアニメ終了待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::WaitControlMotionBlurColor( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Fieldmap                *p_fieldmap = p_gman->GetFieldmap();
  System::MotionBlur      *pMotionBlur = p_fieldmap->GetUpperRenderingPipeLine()->GetMotionBlurPath();

  return pMotionBlur->IsEndColorFilter();
}

//------------------------------------------------------------------------------
/**
 * @func        CircleSpotFade_
 * @brief       CircleSpotFade関数.
 * @author      N.Takeda
 * @date        2015/11/06(金) 20:15:30
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CircleSpotFade_( AMX * amx, const cell * ptr )
{
  ScriptObject                  *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager          *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem             *pSys     = p_gman->GetFieldScriptSystem();
  Field::Effect::EffectManager  *pEffMgr  = p_gman->GetFieldmap()->GetEffectManager();
  
  bool spot_type  = GFL_BOOL_CAST(ptr[1]);  ///< 0:In  1:Out
  u32 frame       = ptr[2];
  u32 ScreenPosX  = ptr[3];
  u32 ScreenPosY  = ptr[4];
  
  gfl2::math::VEC2    pos( ScreenPosX, ScreenPosY );
  
  if( spot_type == 0 )
  {
    Field::Effect::EffectFadeSpot *pFadeSpot = reinterpret_cast<Field::Effect::EffectFadeSpot*>( pSys->GetEffect() );
    if( !pFadeSpot )  return NULL;
    pFadeSpot->StartAnimation( pos, frame, spot_type );
  }else{
    
    if( pSys->GetEffect() )
    {
      pEffMgr->DeleteEffect( pSys->GetEffect() );
      pSys->SetEffect( NULL );
    }
    
    gfl2::math::Vector3 vPos_zero = gfl2::math::Vector3::GetZero();
    
    Field::Effect::IEffectBase    *pEffect = pEffMgr->CreateEffect( Field::Effect::EFFECT_TYPE_FADE_SPOT, vPos_zero );
    if( pEffect ){
      Field::Effect::EffectFadeSpot *pFadeSpot = reinterpret_cast<Field::Effect::EffectFadeSpot*>( pEffect );
      pFadeSpot->StartAnimation( pos, frame, spot_type );
      pSys->SetEffect( pEffect );
    }
  }
  
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        ConvertGetItemIDToEffectID
 * @brief       ゲットアイテムIDからスクリプトエフェクトIDへの変換
 * @param       getItemID FieldPawnTypes.hのGetItemIDから選択
 * @author      saita_kazuki
 * @date        2016/01/20
 */
//------------------------------------------------------------------------------
s32 FuncSetCommon::ConvertGetItemIDToScriptEffectID( GetItemID getItemID)
{
  s32 offset = FuncSetCommon::CalcGetItemIDOffset( getItemID);

  return ( EFF_POS_GET_ITEM_1 + offset );
}

//------------------------------------------------------------------------------
/**
 * @func        ConvertGetItemIDToGetItemIndex
 * @brief       ゲットアイテムIDからGetItemクラスに渡すindexへの変換
 * @param       getItemID FieldPawnTypes.hのGetItemIDから選択
 * @author      saita_kazuki
 * @date        2016/01/20
 */
//------------------------------------------------------------------------------
s32 FuncSetCommon::ConvertGetItemIDToGetItemIndex( GetItemID getItemID)
{
  s32 offset = FuncSetCommon::CalcGetItemIDOffset( getItemID);

  return getItemID - ( Effect::EffectGetItem::INDEX_MAX * offset );
}

//------------------------------------------------------------------------------
/**
 * @func        CalcGetItemIDOffset
 * @brief       ゲットアイテムIDからエフェクトIDのオフセット値を求める
 * @param       getItemID FieldPawnTypes.hのGetItemIDから選択
 * @author      saita_kazuki
 * @date        2016/01/20
 */
//------------------------------------------------------------------------------
s32 FuncSetCommon::CalcGetItemIDOffset( GetItemID getItemID)
{
  s32 offset = 0;
  if( getItemID != 0)
  {
    offset = getItemID / Effect::EffectGetItem::INDEX_MAX;
  }
  return offset;
}


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )


