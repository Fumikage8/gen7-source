//==============================================================================
/**
 * @file        ScriptFuncSetEvCamera.cpp
 * @brief       ScriptFuncSetCommon系 イベントカメラ制御系統
    
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

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"
#include "Field/FieldRo/include/Camera/FieldCameraSystemAnimationResource.h"
#include "Field/FieldRo/include/Camera/FieldCameraAnimationLoader.h"
#if defined(GF_PLATFORM_WIN32)
#include "Field/FieldRo/include/Camera/FieldCameraRecorder.h"
#endif

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
 * @func        SCREND_CheckEndEventCamera
 * @brief       SCREND_CheckEndEventCamera関数.
 * @author      N.Takeda
 * @date        2015/07/02(木) 19:12:39
 *
 * @param[in,out] GameSys::GameManager* p_gman  .
 *
 * @return      bool  TRUE:成 / FALSE:否
 */
//------------------------------------------------------------------------------
bool FuncSetCommon::SCREND_CheckEndEventCamera( GameSys::GameManager* p_gman, s32* /*seq*/ )
{
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  Fieldmap*              pFieldmap      = p_gman->GetFieldmap();

  if( !pFieldmap ){
    p_sys->SetCameraController(NULL);
    p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_EVENT_CAMERA ); 
    return true;
  }
  
  Camera::CameraManager* pCameraManager = pFieldmap->GetCameraManager();

  // @note FieldUnitは必ず存在します
  Camera::CameraUnit* pCameraUnitEvent = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT );
  //Camera::CameraUnit* pCameraUnitField = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD );

  // コントローラー破棄
  pCameraUnitEvent->ChangeController( NULL );

  // フィールドに戻す
  pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );

  p_sys->SetCameraController(NULL);

  p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_EVENT_CAMERA ); 
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraStart
 * @brief       EvCameraStart関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraStart(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();

  Fieldmap*               pFieldMap     = p_gman->GetFieldmap();
  Camera::CameraManager*  pCameraManager = pFieldMap->GetCameraManager();
  gfl2::heap::HeapBase*   pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
        
  // FieldUnitの基本パラメーターをEventUnitにコピー
  // イベントスクリプト用コントローラー内部で値を使用する可能性があるので先に行う
  pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT )->CopyBaseParam(
    *pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )
    );
        
  // 使用カメラをField->Eventに変更( Fieldはスリープ状態になる )
  pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::EVENT );

  // イベントスクリプト用コントローラーを生成
  // EventUnitはポケファインダーなどで使いまわすので、毎回使用開始時点で作ります
  // downcastせずアクセスする為に、作成したコントローラーはScriptWorkなどで保持してください
  {
    Camera::Controller::ControllerTypeEventScript::InitializeDescription initDesc;
    Camera::Controller::ControllerTypeEventScript* pCameraController;
    initDesc.dummy = 0; // 引数はまだ仮です
    pCameraController = GFL_NEW( pCameraManager->GetHeapForEvCamera() )Camera::Controller::ControllerTypeEventScript( initDesc );

    // 操作コントローラーをイベントスクリプト用コントローラに変更
    {
      // ↓ChangeMainCameraByUnitNo()で指定したカメラが返ります
      Camera::CameraUnit* pCameraUnit = pCameraManager->GetMainGamePlayCamera();

      // ↓描画に使うUnitを上書き設定している場合は、違うUnitが返ります、用途はデバッグ用で基本はGetMainGamePlayCameraと同じ
      // Camera::CameraUnit* pCameraUnit = pCameraManager->GetMainViewCamera();
          
      // この時点でフィールドシステムが更新してくれるようになります
      pCameraUnit->ChangeController( pCameraController );
      p_sys->SetCameraController(pCameraController);
    }
  }
  
  p_sys->GetSystemWork()->OnCheckBit( SCR_END_CHECK_BIT_EVENT_CAMERA );
  
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraEnd
 * @brief       EvCameraEnd関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraEnd(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<

  Fieldmap*              pFieldmap      = p_gman->GetFieldmap();
  if( pFieldmap )
  {
    Camera::CameraManager* pCameraManager = pFieldmap->GetCameraManager();
  //  Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();

    // @note FieldUnitは必ず存在します
    Camera::CameraUnit* pCameraUnitEvent = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT );
    Camera::CameraUnit* pCameraUnitField = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD );


    if( frame > 0 )
    {
      // 補間
      gfl2::math::Vector3 targetPosition;
      gfl2::math::Vector3 cameraPosition;
      targetPosition = pCameraUnitEvent->GetBaseCamera()->GetTargetPosition();
      cameraPosition = pCameraUnitEvent->GetBaseCamera()->GetPosition();
      
      // コントローラー破棄
      pCameraUnitEvent->ChangeController( NULL );

      // フィールドに戻す
      pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );
      
      // @fix GFNMCat[1817]:イベントカメラがインゲームカメラに戻るときの補間がなくなっている、対処。カメラ切り替え時に補間をクリアする対応の入れバグ。切り替え後に補間を設定するように変更
      pCameraUnitField->SetInterpolation( targetPosition, cameraPosition, frame, easing );
    }
    else
    {
      // コントローラー破棄
      pCameraUnitEvent->ChangeController( NULL );

      // フィールドに戻す
      pCameraManager->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );
    }
  }
  p_sys->SetCameraController(NULL);
  
  p_sys->Suspend();
  
  p_sys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_EVENT_CAMERA );
 
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraMoveWait
 * @brief       EvCameraMoveWait_関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveWait_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  //Fieldmap*               pFieldmap     = p_gman->GetFieldmap();
  //Camera::CameraManager* pCameraManager = pFieldmap->GetCameraManager();
  Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();

  if( pCameraController == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return true;
  }


  if( pCameraController->IsMoving() == false )
  {
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraRecordStart_
 * @brief       EvCameraRecordStart_関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraRecordStart_(AMX * amx, const cell * ptr)
{
#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_EV_CAMERA_RECORD ) )
  {
    ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
    GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
    FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();

    cell* buf  = NULL;
    c8  fileName[ 128 ];
    amx_GetAddr( amx, ptr[ 1 ], &buf );             
    amx_GetString( &fileName[ 0 ], buf, 0, sizeof( fileName ) );

    Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();
    if( pCameraController == NULL )
    {
      GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
      return NULL;
    }

    Camera::CameraRecorder* pCameraRecorder = pCameraController->GetCameraRecorder();
    pCameraRecorder->Start( Camera::CameraRecorder::RecType::FOR_MAYA, fileName );
  }

#endif 
#endif 
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraRecordEnd_
 * @brief       EvCameraRecordEnd_関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraRecordEnd_(AMX * amx, const cell * ptr)
{
#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_EV_CAMERA_RECORD ) )
  {
    ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
    GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
    FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();

    Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();

    if( pCameraController == NULL )
    {
      GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
      return NULL;
    }

    Camera::CameraRecorder* pCameraRecorder = pCameraController->GetCameraRecorder();
    pCameraRecorder->End();
  }
#endif 
#endif 
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        EvCameraMovePos
 * @brief       EvCameraMovePos関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL 
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMovePos(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<
  f32 posX   = amx_ctof( ptr[3] );
  f32 posY   = amx_ctof( ptr[4] );
  f32 posZ   = amx_ctof( ptr[5] );
  f32 distance  = amx_ctof( ptr[6] );
  f32 angX   = gfl2::math::ConvDegToRad( amx_ctof( ptr[7] ) );
  f32 angY   = gfl2::math::ConvDegToRad( amx_ctof( ptr[8] ) );
  f32 degZ   = amx_ctof( ptr[9] );
  f32 angZ   = gfl2::math::ConvDegToRad( degZ );
  //f32 zoom   = amx_ctof( ptr[10] );

  Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();

  if( pCameraController == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  // ジョイントの位置を取得
  gfl2::math::Vector3 eyePos = p_sys->CalcEventPositionWorld( posX, posY, posZ );
  gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, distance );
  gfl2::math::Vector3 cameraPos( 0.0f, 0.0f, 0.0f );

  gfl2::math::Matrix34 rotMat = gfl2::math::Matrix34::GetRotationZYX(angZ,angY,angX);

  cameraOffset = rotMat.TransformCoord( cameraOffset );
  cameraPos = eyePos + cameraOffset;

  p_sys->GetCameraController()->Move( cameraPos, eyePos, frame, easing, degZ );

  pCameraController->Move( cameraPos, eyePos, frame, easing, degZ );
  p_sys->Suspend();

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraMoveDist
 * @brief       EvCameraMoveDist関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL 

 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveDist(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<
  f32 posX   = amx_ctof( ptr[3] );
  f32 posY   = amx_ctof( ptr[4] );
  f32 posZ   = amx_ctof( ptr[5] );
  f32 distance  = amx_ctof( ptr[6] );
  f32 angX   = gfl2::math::ConvDegToRad( amx_ctof( ptr[7] ) );
  f32 angY   = gfl2::math::ConvDegToRad( amx_ctof( ptr[8] ) );
  f32 degZ   = amx_ctof( ptr[9] );
  f32 angZ   = gfl2::math::ConvDegToRad( degZ );
  //f32 zoom   = amx_ctof( ptr[10] );

  Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();
  if( pCameraController == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  // ジョイントの位置を取得
  gfl2::math::Vector3 eyePos = p_sys->CalcEventPositionWorld( posX, posY, posZ );
  gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, distance );
  gfl2::math::Vector3 cameraPos( 0.0f, 0.0f, 0.0f );

  gfl2::math::Matrix34 rotMat = gfl2::math::Matrix34::GetRotationZYX(angZ,angY,angX);

  cameraOffset = rotMat.TransformCoord( cameraOffset );
  cameraPos = eyePos + cameraOffset;

  p_sys->GetCameraController()->Move( cameraPos, eyePos, frame, easing, degZ );

  pCameraController->Move( cameraPos, eyePos, frame, easing, degZ );
  p_sys->Suspend();

  return false;
}

cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveOffset(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys = p_gman->GetFieldScriptSystem();

  u32 frame = ptr[1];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[2]);     ///<
  f32 offsetX = amx_ctof(ptr[3]);
  f32 offsetY = amx_ctof(ptr[4]);
  f32 offsetZ = amx_ctof(ptr[5]);

  Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();
  if (pCameraController == NULL)
  {
    GFL_ASSERT_MSG(0, "EvCamera is not started\n");
    return false;
  }

  // 現在の情報を取得
  gfl2::math::Vector3 nowCameraPos(0.0f, 0.0f, 0.0f);
  gfl2::math::Vector3 nowCameraTargetPos(0.0f, 0.0f, 0.0f);

  // @note アニメの結果を適用しない
  p_sys->GetCameraController()->GetCameraPosition(&nowCameraPos, false);
  p_sys->GetCameraController()->GetTargetPosition(&nowCameraTargetPos);

  gfl2::math::Vector3 offset(offsetX, offsetY, offsetZ);
  p_sys->GetCameraController()->Move(nowCameraPos+offset, nowCameraTargetPos+offset, frame, easing );
  p_sys->Suspend();

  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        EvCameraMoveLookAt
 * @brief       EvCameraMoveLookAt関数.
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveLookAt(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<
  f32 posX   = amx_ctof( ptr[3] );
  f32 posY   = amx_ctof( ptr[4] );
  f32 posZ   = amx_ctof( ptr[5] );
  f32 targetX   = amx_ctof( ptr[6] );
  f32 targetY   = amx_ctof( ptr[7] );
  f32 targetZ   = amx_ctof( ptr[8] );
  f32 bankDeg = amx_ctof( ptr[9] );
  f32 fovY = amx_ctof( ptr[10] );

  //Fieldmap*               pFieldmap      = p_gman->GetFieldmap();
  //Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();
  Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();
  if( pCameraController == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  gfl2::math::Vector3 rCameraPosition = p_sys->CalcEventPositionWorld(posX,posY,posZ);
  gfl2::math::Vector3 rCameraTarget =  p_sys->CalcEventPositionWorld(targetX,targetY,targetZ);

  // -1.0で画角遷移無効
  if( fovY <= 0.0f )
  {
    fovY = -1.0f;
  }

  pCameraController->Move( rCameraPosition, rCameraTarget, frame, easing, bankDeg, fovY );
  p_sys->Suspend();

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraMoveChr
 * @brief       指定キャラをターゲットに、距離と角度でカメラ位置を指定
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveLookAtChr(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<
  u32 EventID = ptr[3];
  u32 locType = ptr[4];
  f32 distance  = amx_ctof( ptr[5] );
  f32 angX      = gfl2::math::ConvDegToRad( amx_ctof( ptr[6] ) );
  f32 angY      = gfl2::math::ConvDegToRad( amx_ctof( ptr[7] ) );
  f32 degZ   = amx_ctof( ptr[8] );
  f32 angZ   = gfl2::math::ConvDegToRad( degZ );
  //f32 zoom      = amx_ctof( ptr[9] );

  Camera::Controller::ControllerTypeEventScript* pCameraController = p_sys->GetCameraController();
  if( pCameraController == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false; 
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( chrID )->GetCharaDrawInstance();
  gfl2::math::Matrix34  locatorEyeMtx = GetLocaterMatrix( pCharaDrawInstance, locType );

  // ジョイントの位置を取得
  gfl2::math::Vector3 eyePos = locatorEyeMtx.GetElemPosition();
  gfl2::math::Vector3 eyeRot = locatorEyeMtx.GetElemRotationZYX();
  gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, distance );
  gfl2::math::Vector3 cameraPos( 0.0f, 0.0f, 0.0f );
#if 0
  gfl2::math::Quaternion quat ( pCharaDrawInstance->GetRotationQuat() );
  gfl2::math::Matrix34 rotChr;
  quat.QuaternionToMatrix( rotChr );
  gfl2::math::Vector3 chrRot = rotChr.GetElemRotationZYX();
#endif
  gfl2::math::Matrix34 rotMat = gfl2::math::Matrix34::GetIdentity();
  
  if( locType < CHR_LOC_POSITION ){
    rotMat = gfl2::math::Matrix34::GetRotationZYX(eyeRot.z,eyeRot.y,eyeRot.x); //ロケーター相対
  }
  rotMat *=  gfl2::math::Matrix34::GetRotationZYX(angZ,angY,angX);
  cameraOffset = rotMat.TransformCoord( cameraOffset );

  eyePos = eyePos;
  cameraPos = eyePos + cameraOffset;

  pCameraController->Move( cameraPos, eyePos, frame, easing, degZ );

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraMoveLookAtCharEasy
 * @brief       指定トレーナーをキャラをターゲットにするカメラ位置を指定(オフセット固定版)
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveLookAtChrEasy(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<
  u32 EventID = ptr[3];
  u32 locType = ptr[4];
  f32 distance_rate = amx_ctof( ptr[5] );

  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false; 
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( chrID )->GetCharaDrawInstance();
  gfl2::math::Matrix34  locatorEyeMtx = GetLocaterMatrix( pCharaDrawInstance, locType );

  // ジョイントの位置を取得
  gfl2::math::Vector3 eyePos = locatorEyeMtx.GetElemPosition();
  gfl2::math::Vector3 eyeRot = locatorEyeMtx.GetElemRotationZYX();

  // 現在の情報を取得
  gfl2::math::Vector3 nowCameraPos( 0.0f, 0.0f, 0.0f );
  gfl2::math::Vector3 nowCameraTargetPos( 0.0f, 0.0f, 0.0f );

  // @note アニメの結果を適用しない
  p_sys->GetCameraController()->GetCameraPosition( &nowCameraPos, false );
  p_sys->GetCameraController()->GetTargetPosition( &nowCameraTargetPos );

  // 新しい情報を計算
  gfl2::math::Vector3 newCameraOffset( ( nowCameraPos - nowCameraTargetPos ) * distance_rate );
  gfl2::math::Vector3 newCameraTargetPos( eyePos );

  p_sys->GetCameraController()->Move( newCameraTargetPos + newCameraOffset, newCameraTargetPos, frame, easing );
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraMoveLookAtCharEasyPair
 * @brief       指定キャラ間の中間をターゲットにするカメラ位置を指定(オフセット固定版)
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveLookAtChrEasyPair(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<
  u32 EventID_01 = ptr[3];
  u32 locType_01 = ptr[4];
  u32 EventID_02 = ptr[5];
  u32 locType_02 = ptr[6];
  f32 distance_rate = amx_ctof( ptr[7] );
  f32 target_rate = amx_ctof( ptr[8] );

  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID_01 = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID_01 );
  u32   chrID_02 = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID_02 );

  if( chrID_01 == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID_01 );
    return false; 
  }
  if( chrID_02 == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID_02 );
    return false; 
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance_01 = pFieldMoveModelManager->GetFieldMoveModel( chrID_01 )->GetCharaDrawInstance();
  gfl2::math::Matrix34  locatorEyeMtx_01 = GetLocaterMatrix( pCharaDrawInstance_01, locType_01 );
  poke_3d::model::BaseModel*  pCharaDrawInstance_02 = pFieldMoveModelManager->GetFieldMoveModel( chrID_02 )->GetCharaDrawInstance();
  gfl2::math::Matrix34  locatorEyeMtx_02 = GetLocaterMatrix( pCharaDrawInstance_02, locType_02 );

  // ジョイントの位置を取得
  gfl2::math::Vector3 eyePos_01 = locatorEyeMtx_01.GetElemPosition();
  gfl2::math::Vector3 eyePos_02 = locatorEyeMtx_02.GetElemPosition();

  // 現在の情報を取得
  gfl2::math::Vector3 nowCameraPos( 0.0f, 0.0f, 0.0f );
  gfl2::math::Vector3 nowCameraTargetPos( 0.0f, 0.0f, 0.0f );

  // @note アニメの結果を適用しない
  p_sys->GetCameraController()->GetCameraPosition( &nowCameraPos, false );
  p_sys->GetCameraController()->GetTargetPosition( &nowCameraTargetPos );
  
  // 新しい情報を計算
  gfl2::math::Vector3 newCameraOffset( ( nowCameraPos - nowCameraTargetPos ) * distance_rate );
  gfl2::math::Vector3 newCameraTargetPos;
  newCameraTargetPos = eyePos_01 + ( eyePos_02 - eyePos_01 ) * target_rate;

  p_sys->GetCameraController()->Move( newCameraTargetPos + newCameraOffset, newCameraTargetPos, frame, easing );
  return false;
}
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraMoveLookAtChrEasyPairDistance(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  u32 frame  = ptr[ 1 ];     ///<
  gfl2::math::Easing::EaseFunc easing = static_cast<gfl2::math::Easing::EaseFunc>(ptr[ 2 ]);     ///<
  u32 EventID_01 = ptr[3];
  u32 locType_01 = ptr[4];
  u32 EventID_02 = ptr[5];
  u32 locType_02 = ptr[6];
  f32 distance = amx_ctof( ptr[7] );
  f32 target_rate = amx_ctof( ptr[8] );

  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID_01 = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID_01 );
  u32   chrID_02 = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID_02 );

  if( chrID_01 == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID_01 );
    return false; 
  }
  if( chrID_02 == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID_02 );
    return false; 
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance_01 = pFieldMoveModelManager->GetFieldMoveModel( chrID_01 )->GetCharaDrawInstance();
  gfl2::math::Matrix34  locatorEyeMtx_01 = GetLocaterMatrix( pCharaDrawInstance_01, locType_01 );
  poke_3d::model::BaseModel*  pCharaDrawInstance_02 = pFieldMoveModelManager->GetFieldMoveModel( chrID_02 )->GetCharaDrawInstance();
  gfl2::math::Matrix34  locatorEyeMtx_02 = GetLocaterMatrix( pCharaDrawInstance_02, locType_02 );

  // ジョイントの位置を取得
  gfl2::math::Vector3 eyePos_01 = locatorEyeMtx_01.GetElemPosition();
  gfl2::math::Vector3 eyePos_02 = locatorEyeMtx_02.GetElemPosition();

  // 現在の情報を取得
  gfl2::math::Vector3 nowCameraPos( 0.0f, 0.0f, 0.0f );
  gfl2::math::Vector3 nowCameraTargetPos( 0.0f, 0.0f, 0.0f );

  // @note アニメの結果を適用しない
  p_sys->GetCameraController()->GetCameraPosition( &nowCameraPos, false );
  p_sys->GetCameraController()->GetTargetPosition( &nowCameraTargetPos );

  f32 length = (nowCameraPos - nowCameraTargetPos).Length();

  // 新しい情報を計算
  gfl2::math::Vector3 newCameraOffset( ( nowCameraPos - nowCameraTargetPos ).Normalize() * distance );
  gfl2::math::Vector3 newCameraTargetPos;
  newCameraTargetPos = eyePos_01 + ( eyePos_02 - eyePos_01 ) * target_rate;

  p_sys->GetCameraController()->Move( newCameraTargetPos + newCameraOffset, newCameraTargetPos, frame, easing );
  return false;
}


// スクリプト定義からカメラアニメデータindexを取得
s32 GetShakeAnimeDataIndex( u32 shakeType )
{
  // 今はデータ順をあわせている
  return shakeType;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraShake
 * @brief       EvCameraShake関数.
 * @author      N.Takeda
 * @date        2015/08/05(水) 16:56:16
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraShake( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  Fieldmap                *pFieldmap      = p_gman->GetFieldmap();
  if( !pFieldmap )  return NULL;
  
  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();

  u32 endFrame = static_cast<u32>(ptr[1]);
  u32 shakeType = static_cast<u32>(ptr[2]);
  GFL_ASSERT( shakeType < SCR_CAMERA_SHAKE_MAX );

  s32 shakeDataIndex = GetShakeAnimeDataIndex( shakeType );
  f32 scale = amx_ctof(ptr[3]);
  f32 rate  = amx_ctof(ptr[4]);

  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return NULL;
  }

  // シェイク開始
  p_sys->GetCameraController()->GetShakeController()->Begin(
    pCameraManager->GetSystemAnimationResource()->GetResource( shakeDataIndex ),
    true,
    rate,
    scale
    );

  // 同時に終了フレームを指定
  p_sys->GetCameraController()->GetShakeController()->End( endFrame );
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraShakeLoop
 * @brief       EvCameraShakeLoop関数.
 * @author      N.Takeda
 * @date        2015/08/05(水) 16:56:20
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraShakeLoop( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  Fieldmap                *pFieldmap      = p_gman->GetFieldmap();
  if( !pFieldmap )  return NULL;

  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();
  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return NULL;
  }


  u32 shakeType = static_cast<u32>(ptr[1]);
  GFL_ASSERT( shakeType < SCR_CAMERA_SHAKE_MAX );

  s32 shakeDataIndex = GetShakeAnimeDataIndex( shakeType );
  f32 scale = amx_ctof(ptr[2]);
  f32 rate  = amx_ctof(ptr[3]);

  // シェイク開始
  p_sys->GetCameraController()->GetShakeController()->Begin(
    pCameraManager->GetSystemAnimationResource()->GetResource( shakeDataIndex ),
    true,
    rate,
    scale
    );

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        EvCameraShakeEnd
 * @brief       EvCameraShakeEnd関数.
 * @author      N.Takeda
 * @date        2015/08/05(水) 16:56:18
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraShakeEnd( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  
  f32 endFrame  = ptr[1];

  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return NULL;
  }

  if( p_sys->GetCameraController()->GetShakeController()->IsShake() ){
    p_sys->GetCameraController()->GetShakeController()->End( endFrame );
  }
#if 0
  else{
    GFL_ASSERT(0); //スクリプトコーディングミスに気づくためのアサート
  }
#endif
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       EvCameraAnimationLoad関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraAnimationLoad( AMX* amx, const cell* ptr )
{
  ScriptObject                *p_obj              = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager        *p_gman             = p_obj->GetGameManager();
  Fieldmap                    *pFieldmap          = p_gman->GetFieldmap();
  gfl2::fs::AsyncFileManager  *pAsyncFileManager  = p_gman->GetAsyncFileManager();
  gfl2::heap::HeapBase        *p_heap             = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  u32 dataIndex  = ptr[1];  

  // データのロードを開始
  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();
  {
    Camera::CameraAnimationLoader::LoadDescription desc;
    desc.m_ArcNo = ARCID_FIELD_EVENT_CAMERA_ANIME;
    desc.m_ArcDataNo = dataIndex;
    desc.m_pFileManager = pAsyncFileManager;
    desc.m_pHeap = p_heap;
    desc.m_pAllocator = System::GflUse::GetEventDeviceAllocator();

    pCameraManager->GetEventCameraAnimationLoader()->Load( desc );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       EvCameraAnimationUnLoad関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraAnimationUnLoad( AMX* amx, const cell* ptr )
{
  ScriptObject                *p_obj              = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager        *p_gman             = p_obj->GetGameManager();
  Fieldmap                    *pFieldmap          = p_gman->GetFieldmap();

  // データのアンロードを実行
  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();
  {
    pCameraManager->GetEventCameraAnimationLoader()->UnLoad();
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       EvCameraAnimationIsLoading関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraAnimationIsLoading( AMX* amx, const cell* ptr )
{
  ScriptObject                *p_obj              = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager        *p_gman             = p_obj->GetGameManager();
  Fieldmap                    *pFieldmap          = p_gman->GetFieldmap();

  // データをロード中か取得
  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();
  return pCameraManager->GetEventCameraAnimationLoader()->IsLoading();
}

//------------------------------------------------------------------------------
/**
 * @brief       EvCameraAnimationIsLoading関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraAnimationPlay( AMX* amx, const cell* ptr )
{
  ScriptObject                *p_obj              = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager        *p_gman             = p_obj->GetGameManager();
  FieldScriptSystem           *p_sys              = p_gman->GetFieldScriptSystem();
  Fieldmap                    *pFieldmap          = p_gman->GetFieldmap();

  u32 dataIndex  = ptr[1];

  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return NULL;
  }

  // アニメーションデータ取得
  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pData = pCameraManager->GetEventCameraAnimationLoader()->GetData( dataIndex );

  // アニメーションデータ適用
  p_sys->GetCameraController()->SetAnimation( pData );
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       EvCameraAnimationIsPlaying関数.
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::EvCameraAnimationIsPlaying( AMX* amx, const cell* ptr )
{
  ScriptObject                *p_obj              = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager        *p_gman             = p_obj->GetGameManager();
  FieldScriptSystem           *p_sys              = p_gman->GetFieldScriptSystem();

  if( p_sys->GetCameraController() == NULL )
  {
    GFL_ASSERT_MSG( 0, "EvCamera is not started\n" );
    return false;
  }

  // アニメーション中か取得
  return p_sys->GetCameraController()->IsAnimation();
}

cell AMX_NATIVE_CALL FuncSetCommon::FieldCameraShake( AMX* amx, const cell* ptr )
{
  ScriptObject                *p_obj              = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager        *p_gman             = p_obj->GetGameManager();
  FieldScriptSystem           *p_sys              = p_gman->GetFieldScriptSystem();
  Fieldmap                    *pFieldmap          = p_gman->GetFieldmap();

  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();

  f32 vertical_mass = amx_ctof(ptr[1]);
  f32 vertical_k = amx_ctof(ptr[2]);
  f32 vertical_firstVelocity = amx_ctof(ptr[3]);
  f32 vertical_damping = amx_ctof(ptr[4]);
  u32 vertical_wait = ptr[5];
  f32 horizon_mass = amx_ctof(ptr[6]);
  f32 horizon_k = amx_ctof(ptr[7]);
  f32 horizon_firstVelocity = amx_ctof(ptr[8]);
  f32 horizon_damping = amx_ctof(ptr[9]);
  u32 horizon_wait = ptr[10];

  // 縦ゆれをリクエストする
  if (vertical_firstVelocity != 0.0f)
  {
    pCameraManager->GetMainViewCamera()->GetControllerBase()->ShakeVerticalRequest(
      vertical_mass,
      vertical_k,
      vertical_firstVelocity,
      vertical_damping
      );
  }

  if (horizon_firstVelocity != 0.0f)
  {
    // 横ゆれをリクエストする
    pCameraManager->GetMainViewCamera()->GetControllerBase()->ShakeHorizonRequest(
      horizon_mass,
      horizon_k,
      horizon_firstVelocity,
      horizon_damping
      );
  }

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetCommon::GlobalCameraShake( AMX* amx, const cell* ptr )
{
  ScriptObject                *p_obj              = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager        *p_gman             = p_obj->GetGameManager();
  FieldScriptSystem           *p_sys              = p_gman->GetFieldScriptSystem();
  Fieldmap                    *pFieldmap          = p_gman->GetFieldmap();

  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();

  f32 vertical_mass = amx_ctof(ptr[1]);
  f32 vertical_k = amx_ctof(ptr[2]);
  f32 vertical_firstVelocity = amx_ctof(ptr[3]);
  f32 vertical_damping = amx_ctof(ptr[4]);
  u32 vertical_wait = ptr[5];
  f32 horizon_mass = amx_ctof(ptr[6]);
  f32 horizon_k = amx_ctof(ptr[7]);
  f32 horizon_firstVelocity = amx_ctof(ptr[8]);
  f32 horizon_damping = amx_ctof(ptr[9]);
  u32 horizon_wait = ptr[10];

  // 縦ゆれをリクエストする
  if (vertical_firstVelocity != 0.0f)
  {
    pCameraManager->GlobalShakeVerticalRequest(
      vertical_mass,
      vertical_k,
      vertical_firstVelocity,
      vertical_damping
      );
  }

  if (horizon_firstVelocity != 0.0f)
  {
    // 横ゆれをリクエストする
    pCameraManager->GlobalShakeHorizonRequest(
      horizon_mass,
      horizon_k,
      horizon_firstVelocity,
      horizon_damping
      );
  }

  return NULL;
}

cell AMX_NATIVE_CALL FuncSetCommon::FieldCameraReset(AMX* amx, const cell* ptr)
{
  ScriptObject                *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager        *p_gman = p_obj->GetGameManager();
  FieldScriptSystem           *p_sys = p_gman->GetFieldScriptSystem();
  Fieldmap                    *pFieldmap = p_gman->GetFieldmap();

  Camera::CameraManager*  pCameraManager = pFieldmap->GetCameraManager();
  if (pCameraManager->GetTarget())
  {
    pCameraManager->GetTarget()->Fix();
  }
  if (pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD ))
  {
    pCameraManager->GetCameraByUnitNo( Camera::UnitNo::FIELD )->Fix();
  }

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        MayaCameraMotionPlay
 * @brief       MayaCameraMotionPlay関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:46
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MayaCameraMotionPlay_( AMX *amx, const cell *ptr )
{
#ifdef UNDER_CONSTRUCTION
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32         MayaCameraMOtionDefine = ptr[1];                   ///< chr_id
  
  
#endif
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        MayaCameraMotionWait_
 * @brief       MayaCameraMotion待ち関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 18:50:03
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MayaCameraMotionWait_( AMX *amx, const cell *ptr )
{
#ifdef UNDER_CONSTRUCTION
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  
#endif
  return true;
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
