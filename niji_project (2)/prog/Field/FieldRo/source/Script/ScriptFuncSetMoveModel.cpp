//==============================================================================
/**
 * @file        ScriptFuncSetChara.cpp
 * @brief       ScriptFuncSetCommon系 動作モデル制御系統
    
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include <animation/include/gfl2_AnimationId.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"

#include "Savedata/include/SaveData.h"

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldFacialAnime.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetFieldInit.h"

#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Script/ScriptPlayerModelController.h"
#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

#include "Field/FieldRo/include/ActionButton/ActionButtonManager.h"

#include "niji_reference_files/script/FieldPawnTypes.h"
#include "niji_conv_header/field/chara/chara_model_id.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

/// 未実装中のものが warning に含まれるので
//#define UNDER_CONSTRUCTION


/// * @brief       DirToRot  平面(_xx,-_zz) 6時=0度、反時計回り(-PI～PI)
f32 _DIRTOROT( const f32 _xx, const f32 _zz )
{
  f32 fAngle;
  if( _zz == 0.f )  fAngle = ( _xx > 0.f ) ? (PI/2.f) : -(PI/2.f);
  else{
    if( _xx == 0.f )  fAngle = ( _zz > 0.f ) ? 0.f : PI;
    else{
      fAngle = atan( _xx / _zz );
      if( _zz < 0.f ) fAngle += ( _xx > 0.f ) ? PI : -PI;
    }
  }
  return fAngle;
}


/// * @brief      角度の差を得る.
f32 FuncSetCommon::DiffAngle( f32 &_fRadA, f32 &_fRadB )
{
  f32   fRadA = _fRadA;
  f32   fRadB = _fRadB;
  f32   fSa;
  
  while( 0.f > fRadA )  fRadA += PI2;
  while( PI2 < fRadA )  fRadA -= PI2;
  while( 0.f > fRadB )  fRadB += PI2;
  while( PI2 < fRadB )  fRadB -= PI2;
  
  if( fRadA < fRadB )
  {
    fSa = fRadB - fRadA;
    if( PI < fSa )  fRadB -= PI2;
  }else{
    fSa = fRadA - fRadB;
    if( PI < fSa )  fRadA -= PI2;
  }
  
  fSa = fRadA - fRadB;
  _fRadA = fRadA;
  _fRadB = fRadB;
  return fSa;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief WaitFunc:非常駐（＝イベント専用）モーションのパッケージロード
 */
/* ------------------------------------------------------------------------- */
static bool WaitFunc_SpMotionPackLoad( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = p_sys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32 motionPac = label;

  // 非同期読み込み待ち
  if( pFieldMoveModelManager->IsDynamicAnimationLoaded( motionPac ) ) 
  {
    // 読み込んだ非常駐アニメーションパックのセットアップ
    pFieldMoveModelManager->SetupDynamicAnimation( System::GflUse::GetEventDeviceAllocator(), gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ), motionPac );
    return true;
  }

  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief WaitFunc:非常駐（＝イベント専用）モーションのパッケージロード 着せ替え用
 */
/* ------------------------------------------------------------------------- */
static bool WaitFunc_SpMotionPackLoadForKisekae( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = p_sys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  poke_3d::model::DressUpParam* pDressUpParam = p_sys->GetGameManager()->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  u32 motionPac = label;

  // 非同期読み込み待ち
  if( pFieldMoveModelManager->IsDressUpDynamicAnimationLoaded( *pDressUpParam, motionPac ) == false ){
    return false;
  }
  // 読み込んだ非常駐アニメーションパックのセットアップ
  pFieldMoveModelManager->SetupDressUpDynamicAnimation( System::GflUse::GetEventDeviceAllocator(),gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),*pDressUpParam, motionPac );
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief WaitFunc:非常駐（＝イベント専用）モデルのロード
 */
/* ------------------------------------------------------------------------- */
static bool WaitFunc_SpChrLoadWait( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = p_sys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32 characterID = label;

  // 非同期読み込み待ち
  if( pFieldMoveModelManager->IsLoaded( characterID ) == false ){
    return false;
  }
  // 読み込んだ非常駐アニメーションパックのセットアップ
  pFieldMoveModelManager->SetupModel( characterID );
  return true;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief WaitFunc:非常駐（＝イベント専用）モデルの破棄
 */
/* ------------------------------------------------------------------------- */
static bool WaitFunc_ChrDel( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = p_sys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32 EventID = label;

  u32 modelID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( modelID != MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    MoveModel::FieldMoveModel* pModel = pFieldMoveModelManager->GetFieldMoveModel( modelID );

    // 読み込んだ非常駐アニメーションパックのセットアップ
    if( pModel->GetCharaDrawInstance() && pModel->GetCharaDrawInstance()->GetModelInstanceNode()->GetReferenceCnt() == 0 )
    {
      pFieldMoveModelManager->TerminateMoveModelWorkResource( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>(modelID) );
      return true;
    }
  }
  GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        TalkMdlStartInit_
 * @brief       話しかけ開始処理 初期化.
 *    C:/work/sangoScript/sango_project/prog/src/field/script/ScriptFuncSetTalkModel.cpp(24): cell AMX_NATIVE_CALL FuncSetCommon::_TalkMdlStartInit(AMX* amx, const cell * ptr )
 * @author      N.Takeda
 * @date        2015/08/27(木) 18:46:50
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TalkMdlStartInit_(AMX* amx, const cell * ptr )
{
  s32 EventID = ptr[1];
  u16 option  = ptr[2];
//  Dir dir = static_cast<Dir>( ptr[3] );
//  b32 halfSitFlag = ptr[4];

  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_script_work = p_obj->GetWork();
  FieldScriptSystem     *pThis = GameSys::GameManager::GetInstance()->GetFieldScriptSystem();
  GameSys::GameManager  *p_gman = pThis->GetGameManager();

  // 多重起動防止
  u32 old_option  = p_script_work->GetTempWorkValue( SCWK_TALK_OPTION );
  u32 old_eventId = p_script_work->GetTempWorkValue( SCWK_TALK_START_OBJID );
  GFL_ASSERT( old_option == TALK_OPTION_NONE );

  if ( old_eventId != FIELD_EVENTID_NULL )
  {
    GFL_ASSERT( 0 );
    return TALK_OPTION_NOT_ALL;
  }

  // -1 は会話相手
  if ( EventID == -1 )
  {
    EventID = p_script_work->GetTempWorkValue( SCWK_TARGET_OBJID );
  }

  bool turn_ok = true;

  {
    if( !turn_ok )
    {
      GFL_ASSERT_MSG(0,"no set NotTurn");//アサート
      option |=~TALK_OPTION_NOT_TURN;//ケア処理
    }
  }

  // セット
  p_script_work->SetTempWorkValue( SCWK_TALK_OPTION, option );
  p_script_work->SetTempWorkValue( SCWK_TALK_START_OBJID, EventID );
  {
    Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
    u32 chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
    Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

    if( pCharaModel==NULL){
      GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
      return option;
    }
    gfl2::math::Quaternion  cQuat = pCharaModel->GetCharaDrawInstance()->GetRotationQuat();
    
    /// クォータニオンから角を求める.
    gfl2::math::Matrix34  Mtx;
    gfl2::math::Vector3   v1( 0, 0, 1 );
    cQuat.QuaternionToMatrix( Mtx );
    v1 = Mtx * v1;
    v1 = v1.Normalize();
    FuncSetCommon::m_fAngle = gfl2::math::ConvRadToDeg( _DIRTOROT( v1.x, v1.z ) );
  }

  //  ChrRot_
  //  SCWK_TARGET_ANGLE 用意したが使用しなかった. 頃合を見て削除したい

//HOSAKA_PRINT("Set option=%d EventID=%d \n", option, EventID );

  return option;
}

//------------------------------------------------------------------------------
/**
 * @func        TalkMdlEndInit_
 * @brief       話しかけ終了処理 初期化.
 *    C:/work/sangoScript/sango_project/prog/src/field/script/ScriptFuncSetTalkModel.cpp(150): cell AMX_NATIVE_CALL FuncSetCommon::_TalkMdlEndInit(AMX * amx, const cell * ptr)
 * @author      N.Takeda
 * @date        2015/08/27(木) 19:33:31
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TalkMdlEndInit_(AMX * amx, const cell * ptr)
{
  u32 EventID  = ptr[1];
  
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_script_work = p_obj->GetWork();

  u32 option  = p_script_work->GetTempWorkValue( SCWK_TALK_OPTION );

  // いきなりENDはアサート
  if ( EventID == FIELD_EVENTID_NULL )
  {
    //GFL_ASSERT(0); //@note チェックツール修正、チェック、対応後にアサート化する。-> kujiraではアサートにしなかった。
    GFL_PRINT( "Warning : いきなりTalkMdlEndが呼ばれました\n" );
    return TALK_OPTION_NOT_ALL; // 全て無効化してRETURN
  }
  
  // クリア
  p_script_work->SetTempWorkValue( SCWK_TALK_OPTION, TALK_OPTION_NONE );
  p_script_work->SetTempWorkValue( SCWK_TALK_START_OBJID, FIELD_EVENTID_NULL );
  
  return option;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrRotRepair
 * @brief       ChrRotRepair関数.
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TalkMdlGetRepairAngle_(AMX * amx, const cell * ptr)
{
  // @fix GFNMcat[3644]共通会話ルーチン内で向きを戻す際のオプションが、その他の回転と一致していない対処。戻す角度のゲッターを追加し、スクリプトで回転を発効するように変更
  return amx_ftoc(FuncSetCommon::m_fAngle);
}

//------------------------------------------------------------------------------
/**
 * @func        _SpMotionPacLoad
 * @brief       非常駐（＝イベント専用）モーションのパッケージをメモリに展開（プール）します。モーションパッケージはイベントID単位でのネーミングとします.
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpMotionPacLoad( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   uMotionPac    = ptr[1];
  
  // 非常駐モーションの読み込みリクエスト
// note: 引数の値はコンバートヘッダに定義されています(niji_conv_header/field/chara/chara_model_dynamic_motion.h)
  pFieldMoveModelManager->LoadDynamicAnimationAsync( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),uMotionPac );

  pSys->SetWaitFunc( amx, WaitFunc_SpMotionPackLoad, uMotionPac, NULL );
  pSys->Suspend();
  
  return false;
}
cell AMX_NATIVE_CALL FuncSetCommon::SpMotionPacLoad_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   uMotionPac    = ptr[1];

  // 非常駐モーションの読み込みリクエスト
  // note: 引数の値はコンバートヘッダに定義されています(niji_conv_header/field/chara/chara_model_dynamic_motion.h)
  pFieldMoveModelManager->LoadDynamicAnimationAsync( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),uMotionPac );

  pSys->SetWaitFunc( amx, WaitFunc_SpMotionPackLoad, uMotionPac, NULL );
  pSys->Suspend();

  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        _SpMotionPacDel
 * @brief       非常駐モーションパックをメモリから削除します
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:02
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpMotionPacDel( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   uMotionPac    = ptr[1];


  // 非常駐モーションのまま終了しているインスタンスがいないかチェックする。
  if( CheckMoveModelUseDynamicAnimation( pSys, uMotionPac ) )
  {
    GFL_ASSERT_MSG(0, "非常駐モーションのままのキャラがいる。このアサートは抜けることができます。\n");
  }

  
  pFieldMoveModelManager->UnloadDynamicAnimation( uMotionPac );
  
  return false;
}
cell AMX_NATIVE_CALL FuncSetCommon::SpMotionPacDel_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   uMotionPac    = ptr[1];


  // 非常駐モーションのまま終了しているインスタンスがいないかチェックする。
  if( CheckMoveModelUseDynamicAnimation( pSys, uMotionPac ) )
  {
    GFL_ASSERT_MSG(0, "非常駐モーションのままのキャラがいる。このアサートは抜けることができます。\n");
  }


  pFieldMoveModelManager->UnloadDynamicAnimation( uMotionPac );

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        SpMotionPacLoadForKisekae 
 * @brief       非常駐（＝イベント専用）モーションのパッケージをメモリに展開（プール）します。モーションパッケージはイベントID単位でのネーミングとします.
 * @author      Miyuki iwasawa
 * @date        2015/10/12(木) 
 * @note        着せ替えモデル専用　パックIDから識別できないのが難点・・・
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpMotionPacLoadForKisekae( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pSys->GetGameManager()->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  u32   uMotionPac    = ptr[1];
  
  // 非常駐モーションの読み込みリクエスト
// note: 引数の値はコンバートヘッダに定義されています(niji_conv_header/field/chara/chara_model_dynamic_motion.h)
  pFieldMoveModelManager->LoadDressUpDynamicAnimationSync( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),*pDressUpParam, uMotionPac );

  pSys->SetWaitFunc( amx, WaitFunc_SpMotionPackLoadForKisekae, uMotionPac, NULL );
  pSys->Suspend();
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        _SpMotionPacDel
 * @brief       非常駐モーションパックをメモリから削除します
 * @author      Miyuki iwasawa
 * @date        2015/10/12(木) 
 * @note        着せ替えモデル専用　パックIDから識別できないのが難点・・・
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpMotionPacDelForKisekae( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  poke_3d::model::DressUpParam* pDressUpParam = pSys->GetGameManager()->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ
  u32   uMotionPac    = ptr[1];
  
  pFieldMoveModelManager->UnloadDressUpDynamicAnimation( *pDressUpParam, uMotionPac );
  
  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        SpChrLoad
 * @brief       非常駐キャラクターリソースの読み込み
 * @author      takahashi tomoya
 * @date        2015/12/10(木) 
 * @note        メモリの制限で１種類しか読み込むことが出来ないので注意
 * @note        セーブ復帰、アプリ復帰　未対応
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpChrLoad( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   uCharacterID    = ptr[1];

  // すでに読み込み済みでないかチェック
  if( !pFieldMoveModelManager->IsLoaded( uCharacterID ) )
  {
    pFieldMoveModelManager->LoadDynamicAsync( MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_EVENT, uCharacterID );

    pSys->SetWaitFunc( amx, WaitFunc_SpChrLoadWait, uCharacterID, NULL );
    pSys->Suspend();
    pSys->SetSpChrLoadedFlag( true );
  }
  else
  {
    //GFL_ASSERT_MSG( !pFieldMoveModelManager->IsLoaded( uCharacterID ), "SpChrLoad　読み込もうとしたリソースは常駐で管理されています。\n" );
    GFL_PRINT( "SpChrLoad　読み込もうとしたリソースは常駐で管理されています。\n" );
    pSys->SetSpChrLoadedFlag( false );
  }
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        SpChrCreate
 * @brief       非常駐キャラクターの生成
 * @author      takahashi tomoya
 * @date        2015/12/10(木) 
 * @note        一旦１インスタンスしか生成できないという制限をつける。
 * @note        生成したモデルのイベントIDはSP_CHR_EVENT_ID
 * @note        セーブ復帰、アプリ復帰　未対応
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpChrCreate( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  Fieldmap * pFieldmap = pSys->GetGameManager()->GetFieldmap();

  u32   uCharacterID    = ptr[1];

  MoveModel::FieldMoveModelHeaderWork headerWork;
  MoveModel::FieldMoveModelHeaderResource headerResource;

  headerWork.eventId = SP_CHR_EVENT_ID;
  headerWork.zoneId  = pFieldmap->GetZoneID();
  headerResource.characterId  = uCharacterID;

  // まずは、EventIDのモデルがいないことをチェック
  u32 modelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( SP_CHR_EVENT_ID );
  if(modelIndex == MoveModel::FIELD_MOVE_MODEL_MAX)
  {

    // 空きがあるかチェック
    modelIndex = pFieldMoveModelManager->GetFieldMoveEventModelIndexFromFreeSpace();
    if(modelIndex != MoveModel::FIELD_MOVE_MODEL_MAX)
    {
      pFieldMoveModelManager->InitializeMoveModelWork( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>(modelIndex), &headerWork );
      pFieldMoveModelManager->InitializeMoveModelResource( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>(modelIndex), &headerResource, NULL );

      Field::MoveModel::FieldMoveModel *pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>( modelIndex ) );
      pCharaModel->SetCollisionSceneContainerForGround( pFieldmap->GetTerrainManager()->GetCollsionScene() ); // 接地できるように
    }
    else
    {
      GFL_ASSERT_STOP_MSG( modelIndex != MoveModel::FIELD_MOVE_MODEL_MAX, "SpChrCreate MoveModelに空きがありません\n" );
    }
  }
  else
  {
    GFL_ASSERT_STOP_MSG( modelIndex == MoveModel::FIELD_MOVE_MODEL_MAX, "SpChrCreate 古いSpChrが残っています。\n" );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        SpChrDel
 * @brief       非常駐キャラクターの破棄
 * @author      takahashi tomoya
 * @date        2015/12/10(木) 
 * @note        破棄するモデルのイベントIDはSP_CHR_EVENT_ID
 * @note        セーブ復帰、アプリ復帰　未対応
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpChrDel( AMX* amx, const cell* )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32 modelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( SP_CHR_EVENT_ID );
  if( modelIndex != MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    MoveModel::FieldMoveModel * pModel = pFieldMoveModelManager->GetFieldMoveModel( modelIndex );
    pModel->SetVisible(false);

    pSys->SetWaitFunc( amx, WaitFunc_ChrDel, SP_CHR_EVENT_ID, NULL );
    pSys->Suspend();
  }
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        SpChrUnLoad
 * @brief       非常駐キャラクターのリソース破棄
 * @author      takahashi tomoya
 * @date        2015/12/10(木) 
 * @note        セーブ復帰、アプリ復帰　未対応
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::SpChrUnLoad( AMX*, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   uCharacterID    = ptr[1];

  if( pSys->IsSpChrLoaded() )
  {
    pFieldMoveModelManager->Unload( uCharacterID );
    pSys->SetSpChrLoadedFlag( false );
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        _ChrMove
 * @brief       キャラクターを歩数指定で歩行／走行させます
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:03
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMove_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  u32   move_type = ptr[2];             ///< 歩行／走行
  s32   walkCount = ptr[3] - 1;         ///< 歩数
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }

  switch (move_type)
  {
    case CHR_MOVE_CMD_RUN:
    {
      // 1歩走る
      pCharaModel->SetActionCommandStepMove((MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE), false, walkCount, true, 1.0f);
      break;
    }

    case CHR_MOVE_CMD_WALK:
    {
      // 1歩歩く
      pCharaModel->SetActionCommandStepMove( (MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE),true,walkCount,true,1.0f );
      break;
    }
    case CHR_MOVE_CMD_SLOW_WALK:
    {
      // 1歩歩く（遅い）
      pCharaModel->SetActionCommandStepMove( (MoveModel::FieldActionCommandId)(MoveModel::FIELD_ACTION_COMMAND_STEP_MOVE),true,walkCount,true,1.0f,true,true,true );
      break;
    }
  }

  // バミリチェック用フラグ操作
  pCharaModel->EnableFreeMoveForEventScript();
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrMagnetActor_
 * @brief       キャラクターをフィールドアクターに吸着させます
 * @author      Miyuki Iwasawa
 * @date        2015/11/1(日)
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMagnetActor_( AMX* amx, const cell* ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys          = p_gman->GetFieldScriptSystem();
  Work                    *p_script_work = p_obj->GetWork();
  Field::MoveModel::FieldMoveModelManager  *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  Field::FieldResident          *pFieldResident = p_gman->GetFieldResident();
  Field::PlacementDataManager   *pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  u32  EventID = ptr[1];
  u32  MotionID = ptr[2];             //吸着時のモーション
  f32  Distance = amx_ctof( ptr[3] ); //吸着時のアクション切り替え距離
  u32  TargetEventID = ptr[4];        //FIELD_EVENTID_NULLの場合、話しかけターゲット

  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }

  IFieldActorBase* pActor = NULL;

  if( TargetEventID == FIELD_EVENTID_NULL ){
    TargetEventID = p_script_work->GetTargetActorID();
  }
  pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBgEvent( TargetEventID ));
  if( !pActor ) pActor = pPlacementDataManager->SearchZoneMapJump( TargetEventID );
  if( !pActor ) pActor = pPlacementDataManager->SearchZoneItem( TargetEventID );
  if( !pActor ) pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBerryPointFromEventID( TargetEventID ));

  if( pActor == NULL ){
    GFL_ASSERT_MSG(0,"TargetActor NULL or Not Support Type %d\n",TargetEventID);
    return false;
  }

  gfl2::math::Vector3 cpos = pCharaModel->GetCharaDrawInstance()->GetPosition();
  gfl2::math::Vector3 tpos = pActor->GetDataPosition();
  gfl2::math::Vector3 vec = tpos-cpos;
  gfl2::math::Quaternion quat( pActor->GetDataQuaternion() );

  // バミリチェック用フラグ操作
  pCharaModel->EnableFreeMoveForEventScript();

  vec.y = 0; //Y座標差分は無視
  
  if( vec.Length() <= Distance ){
    //一定距離以下だったら、モーションしながら吸着
    pCharaModel->SetActionCommandTargetMoveWithOptionalMotion(Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE_WITH_OPTIONAL_MOTION, MotionID, tpos, quat );
    return false;
  }else{
    //歩いてからアクション
    pCharaModel->SetActionCommandTargetMove(Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, true, tpos, quat, true );
    return true;
  }
}


//------------------------------------------------------------------------------
/**
 * @func        ChrActionCommandIsPlaying
 * @brief       キャラクターのアクションコマンドの再生状態をチェック
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrActionCommandIsPlaying_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  // 動作モデルがアクションコマンドを実行中かどうか
  if( pCharaModel->IsExecuteActionCommand() )
  {
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMotionGetPlayNo
 * @brief       キャラクターが再生しているモーションナンバーを取得する
 * @author      Miyuki Iwasawa 
 * @date        2015/09/14(月) 
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionGetPlayNo( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  
  // 対象となる動作モデルを取得する
  // note: FieldCharaModelManagerはFieldmapクラスに定義されている動作モデルマネージャーインスタンスです
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  //再生中のIDを返す
  return pCharaModel->GetCharaDrawInstance()->GetAnimationId();
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMotionPlayFrameCore
 * @brief       キャラクターにモーションを再生します
 * @author      N.Takeda->Miyuki Iwasawa
 * @date        2015/05/22(金) 21:42:08
 */
//------------------------------------------------------------------------------
cell FuncSetCommon::ChrMotionPlayFrameCore( FieldScriptSystem* pSys, u32 EventID, u32 uMotion, u32 startFrame, s32 interpFrame, s32 joint_type )
{
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  // 対象となる動作モデルを取得する
  // note: FieldCharaModelManagerはFieldmapクラスに定義されている動作モデルマネージャーインスタンスです
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  poke_3d::model::BaseModel* pCharDrawInstanceModel = pCharaModel->GetCharaDrawInstance();

  // モーションの向きをキャラクターの向きとして設定してからモーション再生
  if( joint_type < CHR_LOC_POSITION ){
    // @note 対象モデルのトラバースを実行。対象がカリングされている場合、jointの行列が更新されていない事がある。
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast( pCharDrawInstanceModel->GetModelInstanceNode() );
    gfl2::math::Matrix34 mtx = GetLocaterMatrix( pCharDrawInstanceModel, joint_type );
    gfl2::math::Vector3 FootFront(mtx.GetElemAxisX());  // X軸伝播
    f32 defRad  = FuncSetFieldInit::ConvRotationVector3ToYRadian( FootFront );

    gfl2::math::Quaternion  quat;
    GFL_PRINT("%f\n", gfl2::math::ConvRadToDeg(defRad));
    quat.RadianYToQuaternion(defRad);

    pCharDrawInstanceModel->SetRotationQuat(quat);
  }

  s32 endFrame = -1;

  // プレイヤーのモーションIDをライド状況にあったモーションIDに変換する
  if( EventID == FIELD_EVENTID_PLAYER )
  {
    Field::MoveModel::FieldMoveModelPlayer* pPlayer = reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(pCharaModel);
    if( pPlayer && pPlayer->GetOnMoveModel() )
    {
      pFieldMoveModelManager->ConvertPlayerMotionIdToRideMotionId( uMotion , pPlayer->GetOnMoveModel()->GetCharacterId() , &uMotion , &endFrame );
      if( interpFrame >= 0 ){
        //ライドモーションを補間ありで再生
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimationSmooth(uMotion,interpFrame);
      }else{
        //デザイナー指定のデフォルト補間フレームでライドモーション再生
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation(uMotion);
      }

      // 終了フレームがリソースから設定されている場合
      if( endFrame >= 0 )
      {
        pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationEndFrame((f32)endFrame);
      }
    }
  }

  if( interpFrame >= 0 ){
    //モーションを補間ありで再生
    pCharaModel->GetCharaDrawInstance()->ChangeAnimationSmooth(uMotion,interpFrame);
  }else{
    //デザイナー指定のデフォルト補間フレームで再生
    pCharaModel->GetCharaDrawInstance()->ChangeAnimation(uMotion);
  }

  // 終了フレームがリソースから設定されている場合
  if( endFrame >= 0 )
  {
    pCharaModel->GetCharaDrawInstance()->SetAnimationEndFrame((f32)endFrame);
  }

  // 開始フレームを変更
  pCharaModel->GetCharaDrawInstance()->SetAnimationStartFrame( startFrame );

  return true;
}


cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionPlayFrame_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  u32   EventID = ptr[1];
  u32   uMotion     = ptr[2];
  u32   startFrame  = ptr[3];
  s32   interpFrame  = ptr[4];

  return ChrMotionPlayFrameCore( pSys, EventID, uMotion, startFrame, interpFrame, CHR_JOINT_MAX );
}

//モーション再生前にキャラクターの向きを、モーションの向き(指定ジョイントの向き)に補正する　視線トレーナー用
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionPlayFrameDirCorrect_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  u32   EventID = ptr[1];
  u32   uMotion     = ptr[2];
  u32   startFrame  = ptr[3];
  s32   interpFrame  = ptr[4];
  s32   joint_type  = ptr[5];


  return ChrMotionPlayFrameCore( pSys, EventID, uMotion, startFrame, interpFrame, joint_type );
}


//------------------------------------------------------------------------------
/**
 * @func        ChrMotionIsPlaying
 * @brief       キャラクターに再生されているモーションの終了チェック
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionIsPlaying_( AMX* amx, const cell* ptr )
{
  //ループモーションをウェイト使用とした場合、1loopが終了した時点でtrueが返ります
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1]; 
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }

  // 動作モデルの現在のアニメーションが再生中かどうか
  if( pCharaModel->GetCharaDrawInstance()->GetAnimationId() < 0 )
  {
    return false;
  }

  // 処理負荷対策:停止ビットが立っている場合は再生終了扱い
  if (pCharaModel->IsAnimationStopByUnRegist())
  {
    return false;
  }

  if( pCharaModel->GetCharaDrawInstance()->IsAnimationLastFrame() )
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMotionPause
 * @brief       キャラクターのアニメをポーズします（フィールドが破棄されたら元に戻ります）
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionPause( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  b32   pause = ptr[2];
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  if( pause ){
    pCharaModel->GetCharaDrawInstance()->SetAnimationStepFrame(0.0f);
  }else{
    pCharaModel->GetCharaDrawInstance()->SetAnimationStepFrame(1.0f);
  }
  
  return true;
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrGetDataIndexMax_( AMX* amx, const cell* ptr )
{
  return Field::MoveModel::FIELD_MOVE_MODEL_MAX;
}
cell AMX_NATIVE_CALL FuncSetCommon::ChrGetEventIdByDataIndex_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   index = ptr[1];

  // @note ライドをスクリプトからアクセスするのは禁止
  if (index == Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON)
  {
    return FIELD_EVENTID_NULL;
  }
  
  if (index == Field::MoveModel::FIELD_MOVE_MODEL_PLAYER)
  {
    return FIELD_EVENTID_PLAYER;
  }

  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( index );
  if( pMoveModel && ( pMoveModel->IsSetupWork() || pMoveModel->IsSetupResource() ) )
  {
    // @note 動的枠にはSP_CHR_EVENT_IDが入っている
    return pMoveModel->GetEventId();
  }

  return FIELD_EVENTID_NULL;
}

u32 ConvertRotTypeToFrame(s32 rot_type)
{
  switch (rot_type)
  {
  case CHR_ROT_CMD_1F: return 1;
  case CHR_ROT_CMD_4F: return 4;
  case CHR_ROT_CMD_8F: return 8;
  case CHR_ROT_CMD_12F: return 12;
  case CHR_ROT_CMD_14F: return 14;
  case CHR_ROT_CMD_16F: return 16;
  case CHR_ROT_CMD_32F: return 32;
  case CHR_ROT_CMD_33F: return 33;
  case CHR_ROT_CMD_57F: return 57;
  }

  GFL_ASSERT(0);
  return 1;
}

//------------------------------------------------------------------------------
/**
 * @func        _ChrRot
 * @brief       キャラクターを指定フレームで指定角度に回転させます
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:15
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrRot_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  u32   rot_type  = ptr[2];
  f32   angle     = amx_ctof( ptr[3] );
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }

  pCharaModel->SetActionCommandRotation( Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, angle, 0.0f, 0.0f, ConvertRotTypeToFrame( rot_type ), is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init );
  return true;
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrRotFrame( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  u32   frame = ptr[2];
  f32   angle     = amx_ctof( ptr[3] );
  f32   diff = amx_ctof( ptr[4] );
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[5]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[6]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[7]);
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
    
  /// クォータニオンから角を求める.
  gfl2::math::Quaternion  quatMine = pCharaModel->GetCharaDrawInstance()->GetRotationQuat();
  f32 fNowAngRad = FuncSetFieldInit::ConvRotationQuaternionToYRadian(quatMine);
  f32 fTargetAngRad = gfl2::math::ConvDegToRad( angle );
   
  /// 相対角が範囲内なら回転しない.
  f32 fAngle = gfl2::math::FAbs(gfl2::math::ConvRadToDeg(DiffAngle(fTargetAngRad, fNowAngRad)));

  if (fAngle < diff)
  {
    return false;
  }
  pCharaModel->SetActionCommandRotation( Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, angle, 0.0f, 0.0f, frame, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, true );
  return true;
}
cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRotFrame( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  u32   frame = ptr[2];
  f32   angle     = amx_ctof( ptr[3] );
  f32   diff = amx_ctof( ptr[4] );
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[5]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[6]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[7]);
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
    
  /// クォータニオンから角を求める.
  gfl2::math::Quaternion  quatMine = pCharaModel->GetCharaDrawInstance()->GetRotationQuat();
  f32 fNowAngRad = FuncSetFieldInit::ConvRotationQuaternionToYRadian(quatMine);
  f32 fTargetAngRad = gfl2::math::ConvDegToRad( angle );
   
  /// 相対角が範囲内なら回転しない.
  f32 fAngle = gfl2::math::FAbs(gfl2::math::ConvRadToDeg(DiffAngle(fTargetAngRad, fNowAngRad)));

  if (fAngle < diff)
  {
    return false;
  }
  pCharaModel->SetActionCommandRotation( Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, angle, 0.0f, 0.0f, frame, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, false );
  return true;
}
cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRot_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  u32   rot_type  = ptr[2];
  f32   angle     = amx_ctof( ptr[3] );
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);
  
  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }

  pCharaModel->SetActionCommandRotation( Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, angle, 0.0f, 0.0f, ConvertRotTypeToFrame( rot_type ), is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, false );
  return true;
}


cell AMX_NATIVE_CALL FuncSetCommon::ChrRotReq(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  u32   rot_type = ptr[2];
  f32   angle = amx_ctof(ptr[3]);
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);

  return pSys->ChrReqActionAfterRot(EventID, ConvertRotTypeToFrame(rot_type), angle, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init);
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRotReq(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  u32   rot_type = ptr[2];
  f32   angle = amx_ctof(ptr[3]);
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);

  return pSys->ChrReqActionAfterRot(EventID, ConvertRotTypeToFrame(rot_type), angle, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, false);
}



//------------------------------------------------------------------------------
/**
 * @func        _ChrRotVec3
 * @brief       キャラクターを指定フレームで指定角度に３軸回転させます
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:15
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrRotVec3_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  u32   rot_type  = ptr[2];
  f32   fAngX     = amx_ctof( ptr[3] );
  f32   fAngY     = amx_ctof( ptr[4] );
  f32   fAngZ     = amx_ctof( ptr[5] );
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[6]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[7]);

  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  // @fix cov_ctr[12600]:引数順序ミス修正
  pCharaModel->SetActionCommandRotation(Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, fAngY, fAngX, fAngZ, ConvertRotTypeToFrame(rot_type), is_enable_idle_init, isCompressMotion);
  return true;
}
cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRotVec3_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID   = ptr[1];
  u32   rot_type  = ptr[2];
  f32   fAngX     = amx_ctof( ptr[3] );
  f32   fAngY     = amx_ctof( ptr[4] );
  f32   fAngZ     = amx_ctof( ptr[5] );
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[6]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[7]);

  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  // @fix cov_ctr[12600]:引数順序ミス修正
  pCharaModel->SetActionCommandRotation(Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, fAngY, fAngX, fAngZ, ConvertRotTypeToFrame(rot_type), is_enable_idle_init, isCompressMotion, false);
  return true;
}


b32 FuncSetCommon::ChrRotTargetImpl(s32 event_id, u32 rot_frame, s32 target_event_id, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 isEnableIdleWait )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  static const u32 CHR_ROT_TARGET_RANGE = 15;
  u32   EventID = event_id;
  u32   targetEventId = target_event_id;
  bool  isCompressMotion = is_compress_motion;

  f32   fNowAng = 0;
  f32   fTargetAng = 0;
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);
  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return false;
  }
  poke_3d::model::BaseModel* pCharDrawInstanceModel = pCharaModel->GetCharaDrawInstance();

  /// ポジション
  {
    gfl2::math::Vector3 posMine = pCharDrawInstanceModel->GetPosition();
    gfl2::math::Vector3 posTarget(0, 0, 0);
    gfl2::math::Vector3 tvec(0, 0, 0);

    if (!GetEventIDActorPos(pSys, targetEventId, &posTarget))  // targetのポジションを取得
    {
      return false;
    }

    tvec = posTarget - posMine;
    fTargetAng = FuncSetFieldInit::ConvRotationVector3ToYRadian(tvec);
  }

  {
    gfl2::math::Quaternion  quatMine = pCharDrawInstanceModel->GetRotationQuat();

    /// クォータニオンから角を求める.
    fNowAng = FuncSetFieldInit::ConvRotationQuaternionToYRadian(quatMine);

    /// 相対角が範囲内なら回転しない.
    f32 fAngle = gfl2::math::FAbs(gfl2::math::ConvRadToDeg(DiffAngle(fTargetAng, fNowAng)));
    GFL_PRINT("%f, %f,%f:%f,%f\n", fAngle, fTargetAng, fNowAng, gfl2::math::ConvRadToDeg(fTargetAng), gfl2::math::ConvRadToDeg(fNowAng));

    if (fAngle < static_cast<f32>(CHR_ROT_TARGET_RANGE))
    {
      return false;
    }
  }

  fTargetAng = gfl2::math::ConvRadToDeg(fTargetAng);
  pCharaModel->SetActionCommandRotation(Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, fTargetAng, 0.0f, 0.0f, rot_frame, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, isEnableIdleWait ); 
  return true;
}

b32 FuncSetCommon::ChrRotPosImpl(s32 event_id, u32 rot_frame, const gfl2::math::Vector3& rPos, f32 diff, b32 is_enable_idle_init, b32 is_compress_motion, b32 is_enable_turn_motion_init, b32 isEnableIdleWait )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( event_id );
  f32   fNowAng = 0;
  f32   fTargetAng = 0;

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);
  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", event_id );
    return false;
  }
  poke_3d::model::BaseModel* pCharDrawInstanceModel = pCharaModel->GetCharaDrawInstance();
    /// ポジション
  {
    gfl2::math::Vector3 posMine = pCharDrawInstanceModel->GetPosition();
    gfl2::math::Vector3 posTarget( rPos.x, posMine.y, rPos.z);
    gfl2::math::Vector3 tvec(0, 0, 0);

    tvec = posTarget - posMine;
    fTargetAng = FuncSetFieldInit::ConvRotationVector3ToYRadian(tvec);
  }

  {
    gfl2::math::Quaternion  quatMine = pCharDrawInstanceModel->GetRotationQuat();

    /// クォータニオンから角を求める.
    fNowAng = FuncSetFieldInit::ConvRotationQuaternionToYRadian(quatMine);

    /// 相対角が範囲内なら回転しない.
    f32 fAngle = gfl2::math::FAbs(gfl2::math::ConvRadToDeg(DiffAngle(fTargetAng, fNowAng)));
    GFL_PRINT("%f, %f,%f:%f,%f\n", fAngle, fTargetAng, fNowAng, gfl2::math::ConvRadToDeg(fTargetAng), gfl2::math::ConvRadToDeg(fNowAng));

    if (fAngle < diff)
    {
      return false;
    }
  }

  fTargetAng = gfl2::math::ConvRadToDeg(fTargetAng);
  pCharaModel->SetActionCommandRotation(Field::MoveModel::FIELD_ACTION_COMMAND_ROTATION, fTargetAng, 0.0f, 0.0f, rot_frame, is_enable_idle_init, is_compress_motion, is_enable_turn_motion_init, isEnableIdleWait ); 
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        _ChrRotTarget
 * @brief       キャラクターを指定フレームで指定キャラクターに向かせます
 * @author      N.Takeda
 * @date        2015/05/22(金) 21:42:16
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrRotTarget_( AMX* amx, const cell* ptr )
{
  u32   EventID       = ptr[1];
  u32   targetEventId = ptr[2];
  u32   rot_type      = ptr[3];
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);

  return ChrRotTargetImpl( EventID, ConvertRotTypeToFrame(rot_type), targetEventId, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init );
}
cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRotTarget_( AMX* amx, const cell* ptr )
{
  u32   EventID       = ptr[1];
  u32   targetEventId = ptr[2];
  u32   rot_type      = ptr[3];
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);

  return ChrRotTargetImpl( EventID, ConvertRotTypeToFrame(rot_type), targetEventId, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, false );
}


cell AMX_NATIVE_CALL FuncSetCommon::ChrRotTargetReq(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   EventID = ptr[1];
  u32   targetEventId = ptr[2];
  u32   rot_type = ptr[3];
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);

  return pSys->ChrReqActionAfterRotTarget(EventID, ConvertRotTypeToFrame(rot_type), targetEventId, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init);
}
cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRotTargetReq(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   EventID = ptr[1];
  u32   targetEventId = ptr[2];
  u32   rot_type = ptr[3];
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);

  return pSys->ChrReqActionAfterRotTarget(EventID, ConvertRotTypeToFrame(rot_type), targetEventId, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, false );
}


cell AMX_NATIVE_CALL FuncSetCommon::ChrRotReqClear(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   EventID = ptr[1];
  pSys->ChrClearActionAfterRot( EventID );
  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrRotPos(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   EventID = ptr[1];
	u32   rot_frame = ConvertRotTypeToFrame( ptr[2] );
  f32   fPosX = amx_ctof(ptr[3]);
  f32   fPosZ = amx_ctof(ptr[4]);
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[5]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[6]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[7]);
  f32   fDiff = amx_ctof(ptr[8]);

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  f32   fTargetAng;
  f32   fNowAng;


  gfl2::math::Vector3 pos( fPosX, 0.0f, fPosZ );
  return ChrRotPosImpl(EventID, rot_frame, pos, fDiff, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, false);
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRotPos(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();

  u32   EventID = ptr[1];
  u32   rot_type = ptr[2];
  f32   fPosX = amx_ctof(ptr[3]);
  f32   fPosZ = amx_ctof(ptr[4]);
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[5]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[6]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[7]);
  f32   fDiff = amx_ctof(ptr[8]);

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  f32   fTargetAng;
  f32   fNowAng;
  u32   rot_frame = ConvertRotTypeToFrame(rot_type);


  gfl2::math::Vector3 pos( fPosX, 0.0f, fPosZ );
  return ChrRotPosImpl(EventID, rot_frame, pos, fDiff, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, true );
}


cell AMX_NATIVE_CALL FuncSetCommon::ChrRotFieldActor(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  ScriptObject                  *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager          *p_gman   = p_obj->GetGameManager();
  Fieldmap                      *p_fieldmap = p_gman->GetFieldmap();
  Field::FieldResident          *pFieldResident = p_gman->GetFieldResident();
  Field::PlacementDataManager   *pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  u32   EventID = ptr[1];
  u32   rot_type = ptr[2];
  u32   TargetID = ptr[3];
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);
  f32   fDiff = amx_ctof(ptr[7]);

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  f32   fTargetAng;
  f32   fNowAng;
  u32   rot_frame = ConvertRotTypeToFrame(rot_type);

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);
  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return false;
  }
  poke_3d::model::BaseModel* pCharDrawInstanceModel = pCharaModel->GetCharaDrawInstance();


  IFieldActorBase *pActor = NULL;

  pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBgEvent(TargetID));
  if (!pActor) pActor = pPlacementDataManager->SearchZoneMapJump(TargetID);
  if (!pActor) pActor = pPlacementDataManager->SearchZoneItem(TargetID);
  if (!pActor) pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBerryPointFromEventID(TargetID));
  //  FieldFishingSpotActor
  GFL_ASSERT(pActor);
  if (!pActor) return NULL;

  u32                                             target_event_id = 0xffffffff;
  u32                                             wait_count = 0;
  ActionButton::ActionButtonManager::ICON_FRAME   icon_frame = ActionButton::ActionButtonManager::NONE;
  gfl2::math::Vector3                             actor_pos(0, 0, 0);
  gfl2::math::Vector3                             attention_pos(0, 0, 0);
  b32                                             attention = false;

  if (!p_fieldmap->GetActionButtonManager()->GetActionButtonParam(pActor, &target_event_id, &wait_count, &icon_frame, &actor_pos, &attention_pos, &attention))
  {
    attention_pos = pActor->GetPosision();
  }

  return ChrRotPosImpl(EventID, rot_frame, attention_pos, fDiff, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, true );
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrFastRotFieldActor(AMX* amx, const cell* ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  ScriptObject                  *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager          *p_gman   = p_obj->GetGameManager();
  Fieldmap                      *p_fieldmap = p_gman->GetFieldmap();
  Field::FieldResident          *pFieldResident = p_gman->GetFieldResident();
  Field::PlacementDataManager   *pPlacementDataManager = pFieldResident->GetPlacementDataManager();

  u32   EventID = ptr[1];
  u32   rot_type = ptr[2];
  u32   TargetID = ptr[3];
  bool  is_enable_idle_init = GFL_BOOL_CAST(ptr[4]);
  bool  isCompressMotion = GFL_BOOL_CAST(ptr[5]);
  bool  is_enable_turn_motion_init = GFL_BOOL_CAST(ptr[6]);
  f32   fDiff = amx_ctof(ptr[7]);

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  f32   fTargetAng;
  f32   fNowAng;
  u32   rot_frame = ConvertRotTypeToFrame(rot_type);

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);
  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return false;
  }
  poke_3d::model::BaseModel* pCharDrawInstanceModel = pCharaModel->GetCharaDrawInstance();


  IFieldActorBase *pActor = NULL;

  pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBgEvent(TargetID));
  if (!pActor) pActor = pPlacementDataManager->SearchZoneMapJump(TargetID);
  if (!pActor) pActor = pPlacementDataManager->SearchZoneItem(TargetID);
  if (!pActor) pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBerryPointFromEventID(TargetID));
  //  FieldFishingSpotActor
  GFL_ASSERT(pActor);
  if (!pActor) return NULL;

  u32                                             target_event_id = 0xffffffff;
  u32                                             wait_count = 0;
  ActionButton::ActionButtonManager::ICON_FRAME   icon_frame = ActionButton::ActionButtonManager::NONE;
  gfl2::math::Vector3                             actor_pos(0, 0, 0);
  gfl2::math::Vector3                             attention_pos(0, 0, 0);
  b32                                             attention = false;

  if (!p_fieldmap->GetActionButtonManager()->GetActionButtonParam(pActor, &target_event_id, &wait_count, &icon_frame, &actor_pos, &attention_pos, &attention))
  {
    attention_pos = pActor->GetPosision();
  }

  return ChrRotPosImpl(EventID, rot_frame, attention_pos, fDiff, is_enable_idle_init, isCompressMotion, is_enable_turn_motion_init, false );
}


cell AMX_NATIVE_CALL FuncSetCommon::ChrStepJump_( AMX* amx, const cell* ptr )
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  static const u32 CHR_ROT_TARGET_RANGE = 15;
  u32   EventID       = ptr[1];
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  pCharaModel->SetActionCommand(Field::MoveModel::FIELD_ACTION_COMMAND_STEP_JUMP);
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrChangeEye
 * @brief       ChrChangeEye関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:29
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrChangeEye( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  s32     EyeFacialID = ptr[2];
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pCharaModel->GetFacialAnimeEye();
    pAnimeEye->ChangeAnime( static_cast<poke_3d::model::CharaModel::EyeIndex>(EyeFacialID) );
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrResetEye
 * @brief       ChrResetEye関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrResetEye( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pCharaModel->GetFacialAnimeEye();
    pAnimeEye->ResetAnime();
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrChangeMouth
 * @brief       ChrChangeMouth関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:31
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrChangeMouth( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  s32     MouthFacialID = ptr[2];
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
    pAnimeMouth->ChangeAnime( static_cast<poke_3d::model::CharaModel::MouthIndex>(MouthFacialID) );
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrResetMouth
 * @brief       ChrResetMouth関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrResetMouth( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
    pAnimeMouth->ResetAnime();
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrEyeMotionInit_
 * @brief       ChrEyeMotionInit_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrEyeMotionInit_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pCharaModel->GetFacialAnimeEye();
    pAnimeEye->AnimeCommandInit();
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return true;

}


//------------------------------------------------------------------------------
/**
 * @func        ChrEyeMotionEntry_
 * @brief       ChrEyeMotionEntry_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrEyeMotionEntry_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  s32     EyeFacialID = ptr[2];
  s32     Frame       = ptr[3]; 
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pCharaModel->GetFacialAnimeEye();
    pAnimeEye->AnimeCommandEntry(EyeFacialID,Frame);
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrEyeMotionPlay_
 * @brief       ChrEyeMotionPlay_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrEyeMotionPlay_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pCharaModel->GetFacialAnimeEye();
    pAnimeEye->AnimeCommandPlay();
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrEyeMotionIsPlaying_
 * @brief       ChrEyeMotionIsPlaying_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrEyeMotionIsPlaying_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pCharaModel->GetFacialAnimeEye();
    return pAnimeEye->AnimeCommandPlayCheck();
  }

  GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMotionCommandInit_
 * @brief       ChrMotionCommandPlay ワーク初期化
 * @author      N.Takeda
 * @date        2016/03/02(水) 15:08:12
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionCommandInit_( AMX *amx, const cell *ptr )
{
  ScriptObject*         pScript       = ScriptObject::GetClass<ScriptObject*>(amx);
  Work*                 pScriptWork    = pScript->GetWork();
  MotionCommandParam    *pParam = pScriptWork->GetMotionCommandParamPtr();
  
  pParam->Clear();
  
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMotionCommandEntry_
 * @brief       ChrMotionCommandPlay モーションコマンドの積み込み
 * @author      N.Takeda
 * @date        2016/03/02(水) 15:08:12
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionCommandEntry_( AMX *amx, const cell *ptr )
{
  ScriptObject*         pScript       = ScriptObject::GetClass<ScriptObject*>(amx);
  Work*                 pScriptWork    = pScript->GetWork();
  MotionCommandParam    *pParam = pScriptWork->GetMotionCommandParamPtr();
  
  u32 anime_id = ptr[1];
  s16 frame_start = ptr[2];
  s16 loop_max = ptr[3];
  s16 blend = ptr[4];
  
  GFL_ASSERT( pParam->GetEntryNum() != static_cast<u32>(MotionCommandParam::NO_INITLIZE) );
  if( pParam->GetEntryNum() != static_cast<u32>(MotionCommandParam::NO_INITLIZE) )
  {
    pParam->Entry( anime_id, frame_start, loop_max, blend );
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMotionCommandPlay_
 * @brief       ChrMotionCommandPlay 実行
 * @author      N.Takeda
 * @date        2016/03/02(水) 15:08:12
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionCommandPlay_( AMX *amx, const cell *ptr )
{
  ScriptObject*         pScript       = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager  *p_gman       = pScript->GetGameManager();
  Work*                 pScriptWork   = pScript->GetWork();
  MotionCommandParam    *pParam       = pScriptWork->GetMotionCommandParamPtr();
  
  s32 EventID = ptr[1];
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  
  GFL_ASSERT( pParam->GetEntryNum() != static_cast<u32>(MotionCommandParam::NO_INITLIZE) );
  if( pParam->GetEntryNum() != static_cast<u32>(MotionCommandParam::NO_INITLIZE) )
  {
    pCharaModel->SetActionCommandContinuousPlayBack( Field::MoveModel::FIELD_ACTION_COMMAND_CONTINUOUS_PLAYBACK,
      pParam->GetAnimationIDArrayPtr(),
      pParam->Getm_FrameStartArrayPtr(),
      pParam->Get_LoopMaxArrayPtr(),
      pParam->GetBlendArrayPtr()
    );
  }
  
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMouthMotionInit_
 * @brief       ChrMouthMotionInit_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMouthMotionInit_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
    pAnimeMouth->AnimeCommandInit();
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrMouthMotionEntry_
 * @brief       ChrMouthMotionEntry_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMouthMotionEntry_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  s32     MouthFacialID = ptr[2];
  s32     Frame       = ptr[3]; 
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
    pAnimeMouth->AnimeCommandEntry(MouthFacialID,Frame);
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMouthMotionPlay_
 * @brief       ChrMouthMotionPlay_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMouthMotionPlay_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
    pAnimeMouth->AnimeCommandPlay();
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMouthMotionIsPlaying_
 * @brief       ChrMouthMotionIsPlaying_関数.
 * @author      Miyuki Iwasawa
 * @date        2015/10/31(土)
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMouthMotionIsPlaying_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  s32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
    return pAnimeMouth->AnimeCommandPlayCheck();
  }
  GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrAutoBlink
 * @brief       目パチの設定.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:32
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrAutoBlink( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  bool    isAuto = GFL_BOOL_CAST(ptr[2]);
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pCharaModel->GetFacialAnimeEye();
    pAnimeEye->SetAutoAnime( isAuto );
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrAutoSpeak
 * @brief       口パクの設定.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:33
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrAutoSpeak( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32     EventID     = ptr[1];                   ///< chr_id
  bool    isAuto = GFL_BOOL_CAST(ptr[2]);
  
  Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
  
  if( pCharaModel ){
    Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
    pAnimeMouth->SetAutoAnime( isAuto );
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrNeckRot_
 * @brief       ChrNeckRot関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:34
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrNeckRot_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32     EventID   = ptr[1];                   ///< chr_id
  s32     frame     = ptr[2];                   ///< Frame
  f32     axisYRad = gfl2::math::ConvDegToRad( amx_ctof( ptr[3] ) );  ///< Y軸回転 つまり、水平方向(+で左,-で右)
  f32     axisXRad = gfl2::math::ConvDegToRad( amx_ctof( ptr[4] ) );  ///< X軸回転 つまり、垂直方向(+で上,-で下)

  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  poke_3d::model::CharaModel*  pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID )->GetCharaDrawInstance();

#if DEBUG_NECKLIMIT
  bool isFree = GFL_BOOL_CAST(ptr[5]);
  m_fLimitAngH = 0.f;
  if( isFree ){
    m_fLimitAngH = pCharaModel->GetInterestLimitAngleHigh( 0 );
    m_fLimitAngL = pCharaModel->GetInterestLimitAngleLow( 0 );
    m_fLimitAngH2 = pCharaModel->GetInterestLimitAngleHigh2( 0 );
    m_fLimitAngL2 = pCharaModel->GetInterestLimitAngleLow2( 0 );
    pCharaModel->SetInterestLimitAngleHigh( 0, gfl2::math::ConvDegToRad(90) );
    pCharaModel->SetInterestLimitAngleLow( 0, gfl2::math::ConvDegToRad(-90) );
    pCharaModel->SetInterestLimitAngleHigh2( 0, gfl2::math::ConvDegToRad(45) );
    pCharaModel->SetInterestLimitAngleLow2( 0, gfl2::math::ConvDegToRad(-45) );
  }
#endif

  /*
   * SetNeckAngle関数定義で引数名が SetNeckAngle(f32 angleXRad, f32 angleYRad, u32 frame) 
   * となっており紛らわしいが、
   *
   * angleXRad=左右方向=Y軸回転(axisY)
   * angleYRad=上下方向=X軸回転(axisX)
   *
   * なので、この指定で正しい（「軸」で話すか「方向」で話すかの差）　15/10/30 by iwasawa
   */
  pCharaModel->SetNeckAngle( axisYRad, axisXRad, frame );
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrNeckTarget_
 * @brief       ChrNeckTarget_関数.
 * @author      N.Takeda
 * @date        2015/10/28(水) 13:29:21
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrNeckTarget_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32     EventID   = ptr[1];                   ///< chr_id
  u32     TargetEventID  = ptr[2];
  s32     Frame     = ptr[3];
  s32     LocaterID = ptr[4];
  bool    isFree = GFL_BOOL_CAST(ptr[5]);
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  u32   targetID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( TargetEventID );

  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  if( targetID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", TargetEventID );
    return false;
  }
  poke_3d::model::CharaModel*  pCharaModel = reinterpret_cast<poke_3d::model::CharaModel*>(pFieldMoveModelManager->GetFieldMoveModel( chrID )->GetCharaDrawInstance());
  poke_3d::model::CharaModel*  pTargetModel = reinterpret_cast<poke_3d::model::CharaModel*>(pFieldMoveModelManager->GetFieldMoveModel( targetID )->GetCharaDrawInstance());
#if DEBUG_NECKLIMIT
  m_fLimitAngH = 0.f;
  if( isFree ){
    m_fLimitAngH = pCharaModel->GetInterestLimitAngleHigh( 0 );
    m_fLimitAngL = pCharaModel->GetInterestLimitAngleLow( 0 );
    m_fLimitAngH2 = pCharaModel->GetInterestLimitAngleHigh2( 0 );
    m_fLimitAngL2 = pCharaModel->GetInterestLimitAngleLow2( 0 );
    pCharaModel->SetInterestLimitAngleHigh( 0, gfl2::math::ConvDegToRad(90) );
    pCharaModel->SetInterestLimitAngleLow( 0, gfl2::math::ConvDegToRad(-90) );
    pCharaModel->SetInterestLimitAngleHigh2( 0, gfl2::math::ConvDegToRad(45) );
    pCharaModel->SetInterestLimitAngleLow2( 0, gfl2::math::ConvDegToRad(-45) );
  }
#endif
  
  /// Head から回転のみを Matrix34 に取り出し.
  gfl2::math::Matrix34  targetMtx = GetLocaterMatrix( pTargetModel, LocaterID );
  gfl2::math::Vector3 lookPos = targetMtx.GetElemPosition();
  
  /// プレイヤーの位置と向き.
  gfl2::math::Matrix34  locatorEyeMtx = GetLocaterMatrix( pCharaModel, CHR_LOC_HEAD );
  gfl2::math::Vector3 chrPos = locatorEyeMtx.GetElemPosition();
  gfl2::math::Quaternion  cQuat = pCharaModel->GetRotationQuat();
  /// クォータニオンから角を求める.
  gfl2::math::Matrix34  Mtx;
  gfl2::math::Vector3   v1( 0, 0, 1 );
  cQuat.QuaternionToMatrix( Mtx );
  v1 = Mtx * v1;
  v1 = v1.Normalize();
  f32 fNowAngle = _DIRTOROT( v1.x, v1.z );
  gfl2::math::Vector3 Dir = lookPos - chrPos;
  f32 fTargetAngle = _DIRTOROT( Dir.x, Dir.z );
  f32 fTargetAngleV = _DIRTOROT( Dir.y, gfl2::math::FSqrt(Dir.x*Dir.x+Dir.z*Dir.z) ); ///< 縦角.
  
  /// 相対角を得る.
  f32 fAngle = DiffAngle( fTargetAngle, fNowAngle );
  if( fAngle > PI ) fAngle -= PI2;              ///< 180～359 -> -180～-1

  pCharaModel->SetNeckAngle( fAngle, fTargetAngleV, Frame );
//  GFL_PRINT( "angle = %f, %f  %f, %f\n", gfl2::math::ConvRadToDeg(fAngle), gfl2::math::ConvRadToDeg(fTargetAngleV), gfl2::math::ConvRadToDeg(fNowAngle), gfl2::math::ConvRadToDeg(fTargetAngle) );
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrNeckReset_
 * @brief       ChrNeckReset_関数.
 * @author      N.Takeda
 * @date        2015/10/28(水) 13:29:19
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrNeckReset_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32     EventID   = ptr[1];                   ///< chr_id
  s32     frame     = ptr[2];                   ///< Frame
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  poke_3d::model::CharaModel*  pCharaModel = reinterpret_cast<poke_3d::model::CharaModel*>(pFieldMoveModelManager->GetFieldMoveModel( chrID )->GetCharaDrawInstance());
  pCharaModel->ResetNeckAngle( frame );
  
#if DEBUG_NECKLIMIT
  if( 0.f != m_fLimitAngH ){
    pCharaModel->SetInterestLimitAngleHigh( 0, m_fLimitAngH );
    pCharaModel->SetInterestLimitAngleLow( 0, m_fLimitAngL );
    pCharaModel->SetInterestLimitAngleHigh2( 0, m_fLimitAngH2 );
    pCharaModel->SetInterestLimitAngleLow2( 0, m_fLimitAngL2 );
  }
#endif
  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrNeckIsChanged( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32     EventID   = ptr[1];                   ///< chr_id
  s32     frame     = ptr[2];                   ///< Frame
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;
  }
  poke_3d::model::CharaModel*  pCharaModel = reinterpret_cast<poke_3d::model::CharaModel*>(pFieldMoveModelManager->GetFieldMoveModel( chrID )->GetCharaDrawInstance());
  return pCharaModel->IsChangedNeck();
}


//------------------------------------------------------------------------------
/**
 * @func        ChrNeckRotIsFinish_
 * @brief       ChrNeckRotIsFinish_関数.
 * @author      N.Takeda
 * @date        2015/10/28(水) 13:29:18
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrNeckRotIsFinish_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32     EventID   = ptr[1];                   ///< chr_id
  
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( chrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return true;
  }
  poke_3d::model::CharaModel*  pCharaModel = reinterpret_cast<poke_3d::model::CharaModel*>(pFieldMoveModelManager->GetFieldMoveModel( chrID )->GetCharaDrawInstance());
  
  return pCharaModel->IsFinishNeck();
}


//------------------------------------------------------------------------------
/**
 * @func        ChrLookAtChrEye
 * @brief       ChrLookAtChrEye関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:51
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrLookAtChrEye_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32         EventID = ptr[1];                 ///< chr_id
  u32         TargetEventId = ptr[2];
  u32         Locater = ptr[3];
  s32         frame = ptr[4];                   ///< Frame
  s32         type = ptr[5];
  b32         is_behind_care = ptr[6];
  f32         behind_range_deg   = amx_ctof( ptr[7] );

  u32   TargetChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( TargetEventId );
 
  if( TargetChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", TargetEventId );
    return false;
  }
  poke_3d::model::BaseModel*  pTargetCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( TargetChrID )->GetCharaDrawInstance();
  gfl2::math::Matrix34  locatorEyeMtx = GetLocaterMatrix( pTargetCharaDrawInstance, Locater );
  gfl2::math::Vector3 eyePos = locatorEyeMtx.GetElemPosition();
  
  LookAtPoint( p_gman, EventID, eyePos, frame, GFL_BOOL_CAST(type), is_behind_care, behind_range_deg );
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       指定キャラの間を注目する
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrLookAtChrEyePair_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32         EventID = ptr[1];
  u32         TargetEventId_1 = ptr[2];
  u32         TargetEventId_2 = ptr[3];
  u32         Locater_1 = ptr[4];
  u32         Locater_2 = ptr[5];
  s32         frame = ptr[6];
  f32         t = amx_ctof( ptr[7] );

  u32   TargetChrID_1 = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( TargetEventId_1 );
  u32   TargetChrID_2 = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( TargetEventId_2 );
 
  if( TargetChrID_1 == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", TargetEventId_1 );
    return false;
  }
  if( TargetChrID_2 == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", TargetEventId_1 );
    return false;
  }
  
  gfl2::math::Vector3 eyePos_1 = GetLocaterMatrix( pFieldMoveModelManager->GetFieldMoveModel( TargetChrID_1 )->GetCharaDrawInstance(), Locater_1 ).GetElemPosition();
  gfl2::math::Vector3 eyePos_2 = GetLocaterMatrix( pFieldMoveModelManager->GetFieldMoveModel( TargetChrID_2 )->GetCharaDrawInstance(), Locater_2 ).GetElemPosition();
  gfl2::math::Vector3 lookPos = eyePos_1;

  lookPos.Lerp( eyePos_2, t );
  LookAtPoint( p_gman, EventID, lookPos, frame, true );
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrLookAtFieldObj
 * @brief       ChrLookAtFieldObj関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:47:52
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrLookAtFieldActor_( AMX *amx, const cell *ptr )
{
  ScriptObject                  *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager          *p_gman   = p_obj->GetGameManager();
  Fieldmap                      *p_fieldmap = p_gman->GetFieldmap();
  Field::FieldResident          *pFieldResident = p_gman->GetFieldResident();
  Field::PlacementDataManager   *pPlacementDataManager = pFieldResident->GetPlacementDataManager();
  
  u32         EventID = ptr[1];         ///< chr_id
  u32         TargetID = ptr[2];
  s32         frame = ptr[3];           ///< Frame
  f32         fX = amx_ctof( ptr[4] );
  f32         fY = amx_ctof( ptr[5] );
  f32         fZ = amx_ctof( ptr[6] );
  
  IFieldActorBase *pActor = NULL;
  
  pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBgEvent( TargetID ));
  if( !pActor ) pActor = pPlacementDataManager->SearchZoneMapJump( TargetID );
  if( !pActor ) pActor = pPlacementDataManager->SearchZoneItem( TargetID );
  if( !pActor ) pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBerryPointFromEventID( TargetID ));
//  FieldFishingSpotActor
  GFL_ASSERT( pActor );
  if( !pActor ) return NULL;
  
  u32                                             target_event_id = 0xffffffff;
  u32                                             wait_count = frame;
  ActionButton::ActionButtonManager::ICON_FRAME   icon_frame = ActionButton::ActionButtonManager::NONE;
  gfl2::math::Vector3                             actor_pos(0,0,0);
  gfl2::math::Vector3                             attention_pos(0,0,0);
  b32                                             attention = false;
  
  if( !p_fieldmap->GetActionButtonManager()->GetActionButtonParam( pActor, &target_event_id, &wait_count, &icon_frame, &actor_pos, &attention_pos, &attention ) )
  {
    attention_pos = pActor->GetPosision();
  }
  attention_pos.x += fX;
  attention_pos.y += fY;
  attention_pos.z += fZ;
  
  LookAtPoint( p_gman, EventID, attention_pos, frame, true );
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        ChrLookAtPos
 * @brief       ChrLookAtPos関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:48:25
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrLookAtPos_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u32         EventID = ptr[1];                   ///< chr_id
  f32         posX   = amx_ctof( ptr[2] );
  f32         posY   = amx_ctof( ptr[3] );
  f32         posZ   = amx_ctof( ptr[4] );
  s32         frame = ptr[5];                   ///< Frame
  s32         type = ptr[6];
  
  gfl2::math::Vector3 eyePos = p_sys->CalcEventPositionWorld(posX,posY,posZ);
  LookAtPoint( p_gman, EventID, eyePos, frame, GFL_BOOL_CAST(type) );
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrResetLookAt
 * @brief       ChrResetLookAt関数.
 * @author      N.Takeda
 * @date        2015/08/06(木) 16:48:26
 *
 * @param[in,out] AMX*          未使用.
 * @param[in]     const cell*   未使用.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrResetLookAt_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32         EventID = ptr[1];                   ///< chr_id
  s32         frame = ptr[2];                   ///< Frame
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  Field::MoveModel::FieldMoveModel* pChar = pFieldMoveModelManager->GetFieldMoveModel( ChrID );
  
  if( pChar ){
    pChar->DisableInterest( static_cast<f32>(frame) );
    //  poke_3d_lib/model/source/gfl2_BaseModel.cpp(2106):  * @param resetFrameCount 注目解除にかかるフレーム数
    // なのでここだけはデフォルトフレーム.
  }else{
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return false;  
  }
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrWaitLookAt_
 * @brief       首回転の終了を待つ. デフォルト状態に戻ったか、と限界値に達したかを見ている.
 * @author      N.Takeda
 * @date        2015/09/30(水) 17:38:18
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrWaitLookAt_( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32         EventID = ptr[1];                   ///< chr_id
  bool        bRes = true;
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return true;
  }
  poke_3d::model::BaseModel*  pCharaDrawInstance = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetCharaDrawInstance();
  
  if( pCharaDrawInstance ){
    bRes = pCharaDrawInstance->IsFinishInterest();
  }
  return bRes;
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrLookAtChrKeep( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32         chr_id = ptr[1];                   ///< chr_id
  u32         target_chr_id = ptr[2];            ///< target_chr_id
  u32         frame = ptr[3];                     ///< frame
  s32         target_loc_id = ptr[4];            ///< target_loc_id

  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32 chr_index = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( chr_id );
  u32 target_chr_index = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( target_chr_id );
  
  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel( chr_index );
  Field::MoveModel::FieldMoveModel* pTargetMoveModel = pFieldMoveModelManager->GetFieldMoveModel( target_chr_index );
  
  if (!pMoveModel)
  {
    GFL_ASSERT_MSG(0,"ChrLookAtChrKeep chr_id(%d) is not found\n", chr_id );
    return false;  
  }
  if (!pTargetMoveModel)
  {
    GFL_ASSERT_MSG(0,"ChrLookAtChrKeep target_chr_id(%d) is not found\n", target_chr_id );
    return false;  
  }

  s32 joint_index = ChrGetJointIndex( pTargetMoveModel->GetCharaDrawInstance(), target_loc_id );
  if (joint_index == -1)
  {
    GFL_ASSERT_MSG(0,"ChrLookAtChrKeep target_loc_id(%d) is not found\n", target_loc_id );
    return false;  
  }

  gfl2::math::Vector3 target_pos = pTargetMoveModel->GetCharaDrawInstance()->GetJointWorldPosition(joint_index);
  f32 speed = FuncSetCommon::GetInterestSpeed( pMoveModel->GetCharaDrawInstance(), target_pos, frame );

  poke_3d::model::BaseModel*  pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
  pCharaDrawInstance->SetInterestSpeed( 0, speed );   ///< speed は1回のUpdateで回転させる角度（ラジアン）
  pMoveModel->EnableInterestMoveModel( pTargetMoveModel, joint_index );
  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrLookAtRideKeep(AMX *amx, const cell *ptr)
{
	ScriptObject            *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
	GameSys::GameManager    *p_gman = p_obj->GetGameManager();

	u32         chr_id = ptr[1];                   ///< chr_id
	u32         frame = ptr[3];                     ///< frame
	s32         target_loc_id = ptr[4];            ///< target_loc_id

	Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
	u32 chr_index = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(chr_id);

	Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel(chr_index);
	Field::MoveModel::FieldMoveModel* pTargetMoveModel = p_gman->GetFieldmap()->GetPlayerCharacter()->GetOnMoveModel();

	if (!pMoveModel)
	{
		GFL_ASSERT_MSG(0, "ChrLookAtChrKeep chr_id(%d) is not found\n", chr_id);
		return false;
	}
	if (!pTargetMoveModel)
	{
		GFL_ASSERT_MSG(0, "ChrLookAtChrKeep target is not found\n" );
		return false;
	}

	s32 joint_index = ChrGetJointIndex(pTargetMoveModel->GetCharaDrawInstance(), target_loc_id);
	if (joint_index == -1)
	{
		GFL_ASSERT_MSG(0, "ChrLookAtChrKeep target_loc_id(%d) is not found\n", target_loc_id);
		return false;
	}

	gfl2::math::Vector3 target_pos = pTargetMoveModel->GetCharaDrawInstance()->GetJointWorldPosition(joint_index);
	f32 speed = FuncSetCommon::GetInterestSpeed(pMoveModel->GetCharaDrawInstance(), target_pos, frame);

	poke_3d::model::BaseModel*  pCharaDrawInstance = pMoveModel->GetCharaDrawInstance();
	pCharaDrawInstance->SetInterestSpeed(0, speed);   ///< speed は1回のUpdateで回転させる角度（ラジアン）
	pMoveModel->EnableInterestMoveModel(pTargetMoveModel, joint_index);
	return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::ChrLookAtChrKeepClear( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  
  u32         chr_id = ptr[1];                   ///< chr_id
  s32         is_reset_look = ptr[2];            ///< is_reset_look
  s32         frame = ptr[3];                    ///< frame
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32 chr_index = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(chr_id);
  Field::MoveModel::FieldMoveModel* pMoveModel = pFieldMoveModelManager->GetFieldMoveModel(chr_index);

  if (!pMoveModel)
  {
    GFL_ASSERT_MSG(0, "ChrLookAtChrKeep chr_id(%d) is not found\n", chr_id);
    return false;
  }

  pMoveModel->DisableInterestMoveModel( !is_reset_look, frame );
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CharKindCheck
 * @brief       CharKindCheck関数.
 * @author      N.Takeda
 * @date        2015/12/02(水) 20:22:01
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::CharKindCheck( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  
  u32   EventID  = ptr[1];
  u32   iId[8];
  iId[0] = ptr[2];
  iId[1] = ptr[3];
  iId[2] = ptr[4];
  iId[3] = ptr[5];
  iId[4] = ptr[6];
  iId[5] = ptr[7];
  iId[6] = ptr[8];
  iId[7] = ptr[9];
  
  for( s32 ii=0 ; ii<Field::MoveModel::FIELD_MOVE_MODEL_MAX ; ii++ )
  {
    if( pFieldMoveModelManager->GetFieldMoveModel( ii )->GetEventId() == EventID )
    {
      u32 uCharId = pFieldMoveModelManager->GetFieldMoveModel( ii )->GetCharacterId();
      if( uCharId == iId[0]
       || uCharId == iId[1]
       || uCharId == iId[2]
       || uCharId == iId[3]
       || uCharId == iId[4]
       || uCharId == iId[5]
       || uCharId == iId[6]
       || uCharId == iId[7] ) return true;
      break;
    }
  }
  
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        GetInterestPositionX
 * @brief       ModelSearchByCharacterID関数.
 * @author      N.Takeda
 * @date        2015/11/13(金) 12:46:24
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetInterestPositionX_(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  u32   EventID  = ptr[1];
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  gfl2::math::Vector3 pos = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetInterestPosition();
  
  return amx_ftoc(pos.x);
}

//------------------------------------------------------------------------------
/**
 * @func        GetInterestPositionY
 * @brief       ModelSearchByCharacterID関数.
 * @author      N.Takeda
 * @date        2015/11/13(金) 12:46:24
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetInterestPositionY_(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  u32   EventID  = ptr[1];
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  gfl2::math::Vector3 pos = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetInterestPosition();
  
  return amx_ftoc(pos.y);
}

//------------------------------------------------------------------------------
/**
 * @func        GetInterestPositionZ
 * @brief       ModelSearchByCharacterID関数.
 * @author      N.Takeda
 * @date        2015/11/13(金) 12:46:24
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetInterestPositionZ_(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  u32   EventID  = ptr[1];
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   ChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );

  if( ChrID == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return 0;
  }
  gfl2::math::Vector3 pos = pFieldMoveModelManager->GetFieldMoveModel( ChrID )->GetInterestPosition();
  
  return amx_ftoc(pos.z);
}


//------------------------------------------------------------------------------
/**
 * @func        GetMonsNo
 * @brief       MonsNoの取得.
 * @author      N.Takeda
 * @date        2015/10/15(木) 20:14:04
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::GetMonsNo( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  u32     EventID     = ptr[1];                   ///< chr_id
  
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
  u32   moveModelIndex = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  
  if( moveModelIndex == MoveModel::FIELD_MOVE_MODEL_MAX ){
    GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    return MONSNO_KORATTA;  //当たりさわりのないナンバーを返す
  }
  u32   chrID         = pFieldMoveModelManager->GetFieldMoveModel( moveModelIndex )->GetCharacterId();
  
  Field::MoveModel::ApplicationDataForCommon* pAppData = (Field::MoveModel::ApplicationDataForCommon*)pFieldMoveModelManager->GetApplicationData( chrID );
  if( pAppData )
  {
    if( pAppData->type == Field::MoveModel::TYPE_POKEMON )
    {
      Field::MoveModel::ApplicationDataForPokemon* pNpcData = (Field::MoveModel::ApplicationDataForPokemon*)pAppData;
      return pNpcData->monsNo;
    }
  }
  GFL_ASSERT_MSG( 0, "MonsNo: EventID wrong number" );
  return MONSNO_KORATTA;
}

//------------------------------------------------------------------------------
/**
 * @func        ChrMovePos
 * @brief       指定動作モデルMoveFieldLocator関数.
 * @author      onoue_masayuki
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL
 *
 * @com 相対座標対応コマンド
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ChrMovePos_( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
 
  u32   EventID = ptr[1];                 ///< chr_id
  u32   chrID  = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
  s32   type   = ptr[2];
  f32   posX   = amx_ctof( ptr[3] );
  f32   posY   = amx_ctof( ptr[4] );
  f32   posZ   = amx_ctof( ptr[5] );
  f32   ang    = gfl2::math::ConvDegToRad( amx_ctof( ptr[6] ) );
  bool  ang_enable = GFL_BOOL_CAST( ptr[7] );
  f32   thresholdAngle = amx_ctof( ptr[8] );
  bool  isFiexdPos = GFL_BOOL_CAST( ptr[9] );
  bool  isMoveEndIdleWait = GFL_BOOL_CAST( ptr[10] );
  bool  isEnd = GFL_BOOL_CAST( ptr[11] );
  
  // 対象となる動作モデルを取得する
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );

  if( pCharaModel==NULL){
    GFL_ASSERT_MSG(0,"ChrNotExists\n");
    return false;
  }

  /// 移動
  gfl2::math::Vector3 pos = p_sys->CalcEventPositionWorld(posX,posY,posZ);

  /// 回転.
  gfl2::math::Vector3 axis( 0, 1, 0 );
  gfl2::math::Quaternion quat( axis, ang );
  
  switch (type)
  {
    case CHR_MOVE_CMD_RUN:
    {
      // 指定地点まで走った後、指定した向きへ
      pCharaModel->SetActionCommandTargetMove( Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, false, pos, quat, ang_enable, true, true, thresholdAngle, false, true, isMoveEndIdleWait );
      break;
    }

    case CHR_MOVE_CMD_WALK:
    {
      // 指定地点まで歩いた後、指定した向きへ
      pCharaModel->SetActionCommandTargetMove( Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, true, pos, quat, ang_enable, true, true, thresholdAngle, false, true, isMoveEndIdleWait );
      break;
    }
    case CHR_MOVE_CMD_SLOW_WALK:
    {
      // 指定地点までゆっくり歩いた後、指定した向きへ
      pCharaModel->SetActionCommandTargetMove( Field::MoveModel::FIELD_ACTION_COMMAND_TARGET_MOVE, true, pos, quat, ang_enable, true, true, thresholdAngle, true, true, isMoveEndIdleWait );
      break;
    }
  }

  // バミリチェック用フラグ操作
  pCharaModel->EnableFreeMoveForEventScript();


  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief プレイヤーのフォルムを取得
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::PlayerGetForm( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();

  //判定式を流用している
  return p_gman->GetGameData()->GetFieldSound()->GetPlayerForm();
}

cell AMX_NATIVE_CALL FuncSetCommon::PlayerGetRideMonsNo( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();

  RIDE_POKEMON_ID ride_poke_id = EventPokemonRideTool::GetPokemonRideOnID(p_gman);

  switch (ride_poke_id){
  case Field::RIDE_KENTAROSU:
    return MONSNO_KENTAROSU;
  case Field::RIDE_MUURANDO:
    return MONSNO_MUURANDO;
  case Field::RIDE_SAMEHADA:
    return MONSNO_SAMEHADAA;
  case Field::RIDE_LAPLACE:
    return MONSNO_RAPURASU;
  case Field::RIDE_ROBA2:
    return MONSNO_ROBA2;
  case Field::RIDE_KAIRIKY:
    return MONSNO_KAIRIKII;
  default:
    break;
  }

  return MONSNO_NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief プレイヤーのフォルムをノーマルに戻す
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetCommon::PlayerSetFormToNormalForce( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();

  if( p_gman->GetGameData()->GetFieldSound()->GetPlayerForm() == PLAYER_FORM_NORMAL ){
    return false;
  }
  Field::EventPokemonRideOffWithoutEffect* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOffWithoutEffect>( p_gman->GetGameEventManager() );
  pEvent->SetForce( true );
  
  p_sys->Suspend();
  return true;
}
//------------------------------------------------------------------------------
/**
 * @func        PlayerSetDressUpHair_
 * @brief       プレイヤーの髪型の設定（trueを返すまで実行し続ける事）
 * @author      Ohhira Syo
 * @date        2015/11/09(月) 17:21:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerSetDressUpHair_( AMX * amx, const cell * ptr )
{
  //Param
  u32 hair_style_id = ptr[1];
  u32 hair_color_id = ptr[2];

  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  ScriptObject*                            pObj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                                    pWork = pObj->GetWork();
  u16*                                     seq = pWork->GetSeq();

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  switch(*seq){
  case 0:
    {
      gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

      //モデルコントローラの準備
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = GFL_NEW(p_heap_dev) PlayerModelControllerForScript();
      pPlayerModelControllerForScript->Initialize( pGameManager, pFieldCharaModelManager, pPlayer, pDressUpParam );

      //現在のモデルを隠す
      //- すでにモデルデータがGPUに乗ってるので、まずはモデルを隠してGPUに乗ってない状態にしてから作り直す
      pPlayerModelControllerForScript->HideModel();

      //使い回すのでTempに格納（必ず解放する事）
      pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pPlayerModelControllerForScript;

      *seq = (*seq) + 1;
    }
    break;
  case 1:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //現在のモデルを削除
      pPlayerModelControllerForScript->DeleteModel();

      //新しいモデルの設定
      pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = hair_style_id;
      pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR] = hair_color_id;

      //GameDataに反映
      pGameManager->GetGameData()->GetPlayerStatus()->SetDressup(*pDressUpParam);

      //新しいモデルのロード開始
      pPlayerModelControllerForScript->Load( pDressUpParam );

      *seq = (*seq) + 1;
    }
    break;
  case 2:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //新しいモデルのロード待機
      b32 result_player = pPlayerModelControllerForScript->WaitLoad(); 
      if( result_player )
      {
        //新しいモデルを生成
        pPlayerModelControllerForScript->Create();

        //使い終わったので削除
        GFL_SAFE_DELETE(pPlayerModelControllerForScript);
        pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;  ///< ASSERTチェックにかかるのでクリア Modify. N.Takeda 2016/01/29

        //次に使う場所のためseqを戻す
        *seq = 0;

        //ロード終了＆生成完了を伝える
        return true;
      }
    }
    break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerSetDressUpEye_
 * @brief       プレイヤーの目の設定（trueを返すまで実行し続ける事）
 * @author      Ohhira Syo
 * @date        2015/12/03(木) 11:10:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerSetDressUpEye_( AMX * amx, const cell * ptr )
{
  //Param
  u32 color_id = ptr[1];

  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  ScriptObject*                            pObj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                                    pWork = pObj->GetWork();
  u16*                                     seq = pWork->GetSeq();

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  switch(*seq){
  case 0:
    {
      gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

      //モデルコントローラの準備
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = GFL_NEW(p_heap_dev) PlayerModelControllerForScript();
      pPlayerModelControllerForScript->Initialize( pGameManager, pFieldCharaModelManager, pPlayer, pDressUpParam );

      //現在のモデルを隠す
      //- すでにモデルデータがGPUに乗ってるので、まずはモデルを隠してGPUに乗ってない状態にしてから作り直す
      pPlayerModelControllerForScript->HideModel();

      //使い回すのでTempに格納（必ず解放する事）
      pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pPlayerModelControllerForScript;

      *seq = (*seq) + 1;
    }
    break;
  case 1:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //現在のモデルを削除
      pPlayerModelControllerForScript->DeleteModel();

      //新しいモデルの設定
      pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR] = color_id;

      //GameDataに反映
      pGameManager->GetGameData()->GetPlayerStatus()->SetDressup(*pDressUpParam);

      //新しいモデルのロード開始
      pPlayerModelControllerForScript->Load( pDressUpParam );

      *seq = (*seq) + 1;
    }
    break;
  case 2:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //新しいモデルのロード待機
      b32 result_player = pPlayerModelControllerForScript->WaitLoad(); 
      if( result_player )
      {
        //新しいモデルを生成
        pPlayerModelControllerForScript->Create();

        //使い終わったので削除
        GFL_SAFE_DELETE(pPlayerModelControllerForScript);
        pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;  ///< ASSERTチェックにかかるのでクリア Modify. N.Takeda 2016/01/29

        //次に使う場所のためseqを戻す
        *seq = 0;

        //ロード終了＆生成完了を伝える
        return true;
      }
    }
    break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerSetDressUpLip_
 * @brief       プレイヤーの目の設定（trueを返すまで実行し続ける事）
 * @author      Ohhira Syo
 * @date        2015/12/03(木) 11:10:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerSetDressUpLip_( AMX * amx, const cell * ptr )
{
  //Param
  u32 color_id = ptr[1];

  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  ScriptObject*                            pObj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                                    pWork = pObj->GetWork();
  u16*                                     seq = pWork->GetSeq();

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  switch(*seq){
  case 0:
    {
      gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

      //モデルコントローラの準備
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = GFL_NEW(p_heap_dev) PlayerModelControllerForScript();
      pPlayerModelControllerForScript->Initialize( pGameManager, pFieldCharaModelManager, pPlayer, pDressUpParam );

      //現在のモデルを隠す
      //- すでにモデルデータがGPUに乗ってるので、まずはモデルを隠してGPUに乗ってない状態にしてから作り直す
      pPlayerModelControllerForScript->HideModel();

      //使い回すのでTempに格納（必ず解放する事）
      pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pPlayerModelControllerForScript;

      *seq = (*seq) + 1;
    }
    break;
  case 1:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //現在のモデルを削除
      pPlayerModelControllerForScript->DeleteModel();

      //新しいモデルの設定
      pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = color_id;

      //GameDataに反映
      pGameManager->GetGameData()->GetPlayerStatus()->SetDressup(*pDressUpParam);

      //新しいモデルのロード開始
      pPlayerModelControllerForScript->Load( pDressUpParam );

      *seq = (*seq) + 1;
    }
    break;
  case 2:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //新しいモデルのロード待機
      b32 result_player = pPlayerModelControllerForScript->WaitLoad(); 
      if( result_player )
      {
        //新しいモデルを生成
        pPlayerModelControllerForScript->Create();

        //使い終わったので削除
        GFL_SAFE_DELETE(pPlayerModelControllerForScript);
        pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;  ///< ASSERTチェックにかかるのでクリア Modify. N.Takeda 2016/01/29

        //次に使う場所のためseqを戻す
        *seq = 0;

        //ロード終了＆生成完了を伝える
        return true;
      }
    }
    break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerGetHairStyle
 * @brief       プレイヤーの髪の情報取得：髪型
 * @author      Ohhira Syo
 * @date        2015/11/09(月) 17:21:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerGetHairStyle( AMX * amx, const cell * ptr )
{
  //Alias
  FieldScriptSystem*    pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager = pSys->GetGameManager();

  //パラメータの取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork();

  return pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE];
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerGetHairColor
 * @brief       プレイヤーの髪の情報取得：色
 * @author      Ohhira Syo
 * @date        2015/11/09(月) 17:21:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerGetHairColor( AMX * amx, const cell * ptr )
{
  //Alias
  FieldScriptSystem*    pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager = pSys->GetGameManager();

  //パラメータの取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork();

  return pDressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR];
}


//------------------------------------------------------------------------------
/**
 * @func        PlayerSetRideDressUp
 * @brief       プレイヤーの見た目を陸ライドの着せ替えにする
 * @author      tomoya takahashi
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerSetRideDressUp_( AMX * amx, const cell * ptr )
{
  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  ScriptObject*                            pObj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                                    pWork = pObj->GetWork();
  u16*                                     seq = pWork->GetSeq();

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  switch(*seq){
  case 0:
    {
      gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

      //モデルコントローラの準備
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = GFL_NEW(p_heap_dev) PlayerModelControllerForScript();
      pPlayerModelControllerForScript->Initialize( pGameManager, pFieldCharaModelManager, pPlayer, pDressUpParam );

      //現在のモデルを隠す
      //- すでにモデルデータがGPUに乗ってるので、まずはモデルを隠してGPUに乗ってない状態にしてから作り直す
      pPlayerModelControllerForScript->HideModel();

      //使い回すのでTempに格納（必ず解放する事）
      pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pPlayerModelControllerForScript;

      *seq = (*seq) + 1;
    }
    break;
  case 1:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //現在のモデルを削除
      pPlayerModelControllerForScript->DeleteModel();

      //新しいモデルの設定
      EventPokemonRideOn::GetPokemonRideDressUpParam( pGameManager,  pDressUpParam ); // ライドスーツに置き換え

      //@note GameDataに反映はしてはいけないので注意！！！！！ ライドドレスアップがセーブされてしまいます。
      //pGameManager->GetGameData()->GetPlayerStatus()->SetDressup(*pDressUpParam);

      //新しいモデルのロード開始
      pPlayerModelControllerForScript->Load( pDressUpParam );

      *seq = (*seq) + 1;
    }
    break;
  case 2:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //新しいモデルのロード待機
      b32 result_player = pPlayerModelControllerForScript->WaitLoad(); 
      if( result_player )
      {
        //新しいモデルを生成
        pPlayerModelControllerForScript->Create();

        //使い終わったので削除
        GFL_SAFE_DELETE(pPlayerModelControllerForScript);
        pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;  ///< ASSERTチェックにかかるのでクリア Modify. N.Takeda 2016/01/29

        //次に使う場所のためseqを戻す
        *seq = 0;

        //ロード終了＆生成完了を伝える
        return true;
      }
    }
    break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerSetSaveDressUp
 * @brief       プレイヤーの見た目を現状のセーブデータの物にする
 * @author      tomoya takahashi
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerSetSaveDressUp_( AMX * amx, const cell * ptr )
{
  //Param
  u32 color_id = ptr[1];

  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  ScriptObject*                            pObj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                                    pWork = pObj->GetWork();
  u16*                                     seq = pWork->GetSeq();

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  switch(*seq){
  case 0:
    {
      gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

      //モデルコントローラの準備
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = GFL_NEW(p_heap_dev) PlayerModelControllerForScript();
      pPlayerModelControllerForScript->Initialize( pGameManager, pFieldCharaModelManager, pPlayer, pDressUpParam );

      //現在のモデルを隠す
      //- すでにモデルデータがGPUに乗ってるので、まずはモデルを隠してGPUに乗ってない状態にしてから作り直す
      pPlayerModelControllerForScript->HideModel();

      //使い回すのでTempに格納（必ず解放する事）
      pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pPlayerModelControllerForScript;

      *seq = (*seq) + 1;
    }
    break;
  case 1:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //現在のモデルを削除
      pPlayerModelControllerForScript->DeleteModel();

      //新しいモデルの設定
      *pDressUpParam = pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup();

      //新しいモデルのロード開始
      pPlayerModelControllerForScript->Load( pDressUpParam );

      *seq = (*seq) + 1;
    }
    break;
  case 2:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //新しいモデルのロード待機
      b32 result_player = pPlayerModelControllerForScript->WaitLoad(); 
      if( result_player )
      {
        //新しいモデルを生成
        pPlayerModelControllerForScript->Create();

        //使い終わったので削除
        GFL_SAFE_DELETE(pPlayerModelControllerForScript);
        pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;  ///< ASSERTチェックにかかるのでクリア Modify. N.Takeda 2016/01/29

        //次に使う場所のためseqを戻す
        *seq = 0;

        //ロード終了＆生成完了を伝える
        return true;
      }
    }
    break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerSetSpaceDressUp
 * @brief       プレイヤーの見た目を宇宙服の着せ替えにする
 * @author      tomoya takahashi
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerSetSpaceDressUp_( AMX * amx, const cell * ptr )
{
  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  ScriptObject*                            pObj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                                    pWork = pObj->GetWork();
  u16*                                     seq = pWork->GetSeq();

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  switch(*seq){
  case 0:
    {
      gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

      //モデルコントローラの準備
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = GFL_NEW(p_heap_dev) PlayerModelControllerForScript();
      pPlayerModelControllerForScript->Initialize( pGameManager, pFieldCharaModelManager, pPlayer, pDressUpParam );

      //現在のモデルを隠す
      //- すでにモデルデータがGPUに乗ってるので、まずはモデルを隠してGPUに乗ってない状態にしてから作り直す
      pPlayerModelControllerForScript->HideModel();

      //使い回すのでTempに格納（必ず解放する事）
      pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pPlayerModelControllerForScript;

      *seq = (*seq) + 1;
    }
    break;
  case 1:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //現在のモデルを削除
      pPlayerModelControllerForScript->DeleteModel();

      //新しいモデルの設定
      EventPokemonRideOn::GetSpaceSuitDressUpParam( pGameManager,  pDressUpParam ); // 宇宙服に置き換え

      //@note GameDataに反映はしてはいけないので注意！！！！！ 宇宙服ドレスアップがセーブされてしまいます。
      //pGameManager->GetGameData()->GetPlayerStatus()->SetDressup(*pDressUpParam);

      //新しいモデルのロード開始
      pPlayerModelControllerForScript->Load( pDressUpParam );

      *seq = (*seq) + 1;
    }
    break;
  case 2:
    {
      //ロード時に使ったものを取得
      PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

      //新しいモデルのロード待機
      b32 result_player = pPlayerModelControllerForScript->WaitLoad(); 
      if( result_player )
      {
        //新しいモデルを生成
        pPlayerModelControllerForScript->Create();

        //使い終わったので削除
        GFL_SAFE_DELETE(pPlayerModelControllerForScript);
        pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;  ///< ASSERTチェックにかかるのでクリア Modify. N.Takeda 2016/01/29

        //次に使う場所のためseqを戻す
        *seq = 0;

        //ロード終了＆生成完了を伝える
        return true;
      }
    }
    break;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerGetItemID
 * @brief       プレイヤーの身に着けているアイテムのID取得
 * @author      Ohhira Syo
 * @date        2015/11/11(水) 11:45:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerGetItemID( AMX * amx, const cell * ptr )
{
  //Param
  u32 category_id = ptr[1];

  //Alias
  FieldScriptSystem*    pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager = pSys->GetGameManager();

  //パラメータの取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork();

  return pDressUpParam->itemParams[category_id];
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerPartsEditStart
 * @brief       プレイヤーのパーツの設定開始（必ずEndの方も呼ぶ事）
 * @author      Ohhira Syo
 * @date        2015/11/11(水) 11:22:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerPartsEditStart( AMX * amx, const cell * ptr )
{
  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  {
    gfl2::heap::HeapBase* p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

    //モデルコントローラの準備
    PlayerModelControllerForScript* pPlayerModelControllerForScript  = GFL_NEW(p_heap_dev) PlayerModelControllerForScript();
    pPlayerModelControllerForScript->Initialize( pGameManager, pFieldCharaModelManager, pPlayer, pDressUpParam );

    //使い回すのでTempに格納（必ず解放する事）
    pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pPlayerModelControllerForScript;
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerPartsEditSet
 * @brief       プレイヤーのパーツの設定（Startを呼んでから設定する事。実際の反映はEndで行われる）
 * @author      Ohhira Syo
 * @date        2015/11/11(水) 11:22:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerPartsEditSet( AMX * amx, const cell * ptr )
{
  //Param
  u32 category_id = ptr[1];
  u32 item_id = ptr[2];

  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();

  //設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  {
    //Start時に生成したものを取得
//    PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

    //新しいモデルの設定
//    pDressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = hair_style_id;
    pDressUpParam->itemParams[category_id] = item_id;
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        PlayerPartsEditEnd
 * @brief       プレイヤーのパーツの設定反映（trueを返すまで呼び続ける事）
 * @author      Ohhira Syo
 * @date        2015/11/11(水) 11:22:00
 *
 * @param[in,out] AMX*          .
 * @param[in]     const cell*   .
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PlayerPartsEditEnd( AMX * amx, const cell * ptr )
{
  //Alias
  FieldScriptSystem*                       pSys = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                    pGameManager = pSys->GetGameManager();
//  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
//  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  ScriptObject*                            pObj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                                    pWork = pObj->GetWork();
  u16*                                     seq = pWork->GetSeq();

  //初期設定取得
  poke_3d::model::DressUpParam* pDressUpParam = pGameManager->GetFieldmap()->GetPlayerDressUpParamWork(); // Fieldmapが保持している主人公用のドレスアップパラメータ

  //Start時に作ったものを取得
  PlayerModelControllerForScript* pPlayerModelControllerForScript  = static_cast<PlayerModelControllerForScript*>(pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);

  switch(*seq){
  case 0:
    {
      //現在のモデルを隠す
      //- すでにモデルデータがGPUに乗ってるので、まずはモデルを隠してGPUに乗ってない状態にしてから作り直す
      pPlayerModelControllerForScript->HideModel();

      *seq = (*seq) + 1;
    }
    break;
  case 1:
    {
      //現在のモデルを削除
      pPlayerModelControllerForScript->DeleteModel();

      //新しいモデルのロード開始
      pPlayerModelControllerForScript->Load( pDressUpParam );

      //GameDataに反映
      pGameManager->GetGameData()->GetPlayerStatus()->SetDressup(*pDressUpParam);

      *seq = (*seq) + 1;
    }
    break;
  case 2:
    {
      //新しいモデルのロード待機
      b32 result_player = pPlayerModelControllerForScript->WaitLoad(); 
      if( result_player )
      {
        //新しいモデルを生成
        pPlayerModelControllerForScript->Create();

        //使い終わったので削除
        GFL_SAFE_DELETE(pPlayerModelControllerForScript);
        pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;  ///< ASSERTチェックにかかるのでクリア Modify. N.Takeda 2016/01/29

        //次に使う場所のためseqを戻す
        *seq = 0;

        //ロード終了＆生成完了を伝える
        return true;
      }
    }
    break;
  }

  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        MoveModelResourceLoad
 * @brief       ロードリクエスト.
 * @author      N.Takeda
 * @date        2015/12/09(水) 15:21:59
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MoveModelResourceLoad( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32   ChrId = ptr[1];

  // キャラクターのバイナリリソースの非同期ロード(断片化に対応済み)
  bool  bSuccess = p_sys->GetScriptMoveModelManager()->LoadResourceReq( ChrId );
  GFL_ASSERT( bSuccess );

  return bSuccess;
}

//------------------------------------------------------------------------------
/**
 * @func        MoveModelResourceIsLoaded
 * @brief       ロード待ち.
 * @author      N.Takeda
 * @date        2015/12/09(水) 15:21:57
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MoveModelResourceIsLoaded( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32   ChrId = ptr[1];
  
  bool  bRes = p_sys->GetScriptMoveModelManager()->ResourceIsLoaded( ChrId );
  
  if( bRes ){
    p_sys->GetScriptMoveModelManager()->GetMoveModelManager()->SetupModel( ChrId );
  }
  return bRes;
}

//------------------------------------------------------------------------------
/**
 * @func        MoveModelResourceDel
 * @brief       リソース破棄.
 * @author      N.Takeda
 * @date        2015/12/09(水) 15:21:55
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MoveModelResourceDel( AMX* amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  s32   ChrId = ptr[1];
    
  // バイナリリソースの破棄
  return p_sys->GetScriptMoveModelManager()->UnloadResource( ChrId );
}



/**
 *  @brief 非常駐アニメーションが使用できるか確認する。
 */
b32 FuncSetCommon::CheckMoveModelUseDynamicAnimation( FieldScriptSystem* p_sys, u32 dynamicAnimationId )
{
  b32 result = false; 
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);


  for( u32 i=0; i<Field::MoveModel::FIELD_MOVE_MODEL_MAX; ++i )
  {
    MoveModel::FieldMoveModel* pModel = p_sys->GetGameManager()->GetGameData()->GetFieldCharaModelManager()->GetFieldMoveModel(i);
    if( pModel->IsSetup() )
    {
      if( pModel->GetCharaDrawInstance() )
      {
        u32 animeId = pModel->GetCharaDrawInstance()->GetAnimationId();
        gfl2::animation::PackedAnimationId packedAnimationId(animeId);

        // 破棄する非常駐アニメーションを使っている。
        if( (pModel->GetCharacterId() == packedDynamicAnimationId.charaId) && (packedAnimationId.packId  == packedDynamicAnimationId.packId) )
        {
          GFL_PRINT( "charactorID[%d] EventID[%d]が非常駐アニメーションを参照したままです。\n", pModel->GetCharacterId(), pModel->GetEventId() );
          pModel->GetCharaDrawInstance()->ChangeAnimation( pModel->GetDefaultIdleAnimationId() );
          result = true;
        }
      }
    }
  }

  return result;
}


/**
 *  @brief イベントＩＤからアクター（動作モデルと配置モデル）の座標を返す。
 */
bool FuncSetCommon::GetEventIDActorPos( FieldScriptSystem* p_sys, u32 eventID, gfl2::math::Vector3 * p_out )
{
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = p_sys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  Field::StaticModel::StaticModelManager *pStaticModelManager = p_sys->GetGameManager()->GetFieldmap()->GetStaticModelManager();
  bool result = false;

  // 動作モデルからチェック
  u32   targetChrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( eventID );
  Field::MoveModel::FieldMoveModel* pTargetModel = NULL;
  if( targetChrID < Field::MoveModel::FIELD_MOVE_MODEL_MAX )
  {
    pTargetModel = pFieldMoveModelManager->GetFieldMoveModel( targetChrID );
    *p_out = pTargetModel->GetCharaDrawInstance()->GetPosition();
    result = true;
  }
  else
  {
    // 動作モデルでヒットしなければＳｔａｔｉＭｏｄｅｌ
    Field::StaticModel::StaticModel* pTargetStaticModel = pStaticModelManager->GetEventIDStaticModel( eventID );
    if( pTargetStaticModel )
    {
      *p_out = pTargetStaticModel->GetBaseModel(0)->GetPosition();
      result = true;
    }
  }

  //さらに他のオブジェクトも検索する事になった
  if(! result){
    IFieldActorBase *pActor = NULL;
    {
      Field::PlacementDataManager   *pPlacementDataManager = p_sys->GetGameManager()->GetFieldResident()->GetPlacementDataManager();

      pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBgEvent( eventID ));
      if( !pActor ) pActor = pPlacementDataManager->SearchZoneMapJump( eventID );
      if( !pActor ) pActor = pPlacementDataManager->SearchZoneItem( eventID );
      if( !pActor ) pActor = reinterpret_cast<IFieldActorBase*>(pPlacementDataManager->SearchZoneBerryPointFromEventID( eventID ));
      //FieldFishingSpotActor
    }

    if( pActor ){
      *p_out = pActor->GetDataPosition();
      result = true;
    }
  }

  GFL_ASSERT_MSG( result, "EventID Error %d\n", eventID );

  return result;
}
cell AMX_NATIVE_CALL FuncSetCommon::ChrMotionSetSpeed(AMX* amx, const cell * ptr)
{
  FieldScriptSystem* pSys = FieldScriptSystem::GetInstance();
  Field::MoveModel::FieldMoveModelManager   *pFieldMoveModelManager = pSys->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  u32   EventID = ptr[1];
  f32   speed = amx_ctof( ptr[2] );

  // 対象となる動作モデルを取得する
  u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId(EventID);
  Field::MoveModel::FieldMoveModel* pCharaModel = pFieldMoveModelManager->GetFieldMoveModel(chrID);

  if (pCharaModel == NULL){
    GFL_ASSERT_MSG(0, "EventID Error %d\n", EventID);
    return false;
  }

  pCharaModel->GetCharaDrawInstance()->SetAnimationStepFrame( speed );
	
	// プレイヤーのモーションIDをライド状況にあったモーションIDに変換する
	if (EventID == FIELD_EVENTID_PLAYER)
	{
		Field::MoveModel::FieldMoveModelPlayer* pPlayer = reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(pCharaModel);
		if (pPlayer && pPlayer->GetOnMoveModel())
		{
			pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->SetAnimationStepFrame(speed);
		}
	}
  return NULL;
}



GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
