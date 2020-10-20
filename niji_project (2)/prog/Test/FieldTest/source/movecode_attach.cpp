//=================================================================================================
/**
 *  @file   movecode_attach.h
 *  @brief  動作コード　処理管理　基底クラス
 *  @date   2014.03.17
 *  @author yy-ikeuchi
 */
//=================================================================================================
#include "math/include/gfl2_Vector.h"
#include "math/include/gfl2_Matrix.h"

#include "../include/movemodel_define.h"
#include "../include/movecode_attach.h"
#include "../include/movemodel.h"
#include "../include/field_transform.h"
#include "../include/movemodel_manager.h"
#include "GameSys/include/GameManager.h"

// Field
#include "test/FieldTest/include/FieldSystem.h"
#include "test/FieldTest/include/FieldTestUtility.h"
#include <debug/include/gfl2_DebugPrint.h>

namespace fld{ namespace mmodel {

//=================================================================================================
// パーツアタッチリソース
//=================================================================================================

// プレイヤーのアタッチリソース
static const u32 PLAYER_ATTACH_RESOURCE_CNT = 2;
MoveCodeAttach::AttachResource playerAttachResource[PLAYER_ATTACH_RESOURCE_CNT] =
{
  // テスト釣竿情報
  {
    chara::RSCID_ATTACH_OBJ_TSURIZAO,      // 対象となるリソース
    46,                                   // アタッチする親ジョイント
    gfl2::math::Vector3(0.0f,0.0f,0.0f),  // アタッチする親のジョイントからの相対座標
    gfl2::math::Vector3(0.0f,0.0f,0.0f),  // アタッチする親のジョイントからの相対姿勢
    0,                                    // 同期アニメーションがない場合のデフォルトアニメーション
    {chara::MOTION_ID_TSURI_START,1},     // 同期アニメーション
  },
};

//=================================================================================================
// パーツアタッチ(基礎クラス)
//=================================================================================================

//-----------------------------------------------------------------------------
/**
*  @brief  リソースからアタッチ情報を更新
*/
//-----------------------------------------------------------------------------
void MoveCodeAttach::updateAttachDataFromResource( MoveCodeWork* p_wk, AttachResource* pAttachResource, s32 attachResourceCnt) const
{
  // 共有ワークを取得し初期化
  AttachData* p_local_work   = (AttachData*)MoveCodeBase::GetMoveCodeWork( p_wk, sizeof(AttachData) );
  p_local_work->parentJoint  = -1;
  p_local_work->offsetTrans  = gfl2::math::Vector3::GetZero();
  p_local_work->offsetQua    = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);

  for( s32 i=0 ; i<attachResourceCnt ; i++ )
  {
    // 対象かどうか
    if( pAttachResource[i].rscId != p_wk->p_model->GetHeader().rsc_id )
    {
      continue;
    }

    // オフセット(trans)
    p_local_work->offsetTrans = pAttachResource[i].offsetTrans;

    // オフセット(qua)
    gfl2::math::Vector offsetRot(0.0f,0.0f,0.0f,0.0f);
    offsetRot.x = gfl2::math::ConvRadToDeg( pAttachResource[i].offsetRotYXZ.x ); 
    offsetRot.y = gfl2::math::ConvRadToDeg( pAttachResource[i].offsetRotYXZ.x );
    offsetRot.z = gfl2::math::ConvRadToDeg( pAttachResource[i].offsetRotYXZ.x );
    gfl2::math::Quaternion qua;
    p_local_work->offsetQua.RadianYXZToQuaternion( offsetRot );

    // どこに引っ付けるか
    p_local_work->parentJoint = pAttachResource[i].parentJoint;
    break;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  リソースと親のモーションから自身のアニメーションIDを取得
*/
//-----------------------------------------------------------------------------
s32 MoveCodeAttach::getAnimationIdFromResource( MoveCodeWork* p_wk , AttachResource* pAttachResource, s32 attachResourceCnt) const
{
  s32 animationId = -1;
  if( !p_wk->p_model->GetParent() )
  {
    return animationId;
  }

  for( s32 i=0 ; i<attachResourceCnt ; i++ )
  {
    // 対象かどうか
    if( pAttachResource[i].rscId != p_wk->p_model->GetHeader().rsc_id )
    {
      continue;
    }

    // 同期アニメーションが見つからなかった場合のアニメーションID
    animationId = pAttachResource[i].defaultAnimation;

    // 親で再生中のアニメーションが同期アニメーション対象かどうか
    if( pAttachResource[i].syncAnimationData.parentId == p_wk->p_model->GetParent()->GetAnimeID() )
    {
      animationId = pAttachResource[i].syncAnimationData.childId;
      break;
    }
  }

  return animationId;
}

//=================================================================================================
// パーツアタッチ(プレイヤー)
//=================================================================================================

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
MoveCodeAttachPlayer::MoveCodeAttachPlayer(gfl2::heap::HeapBase * p_heap, c16* pFilePath)
: MoveCodeAttach( p_heap,pFilePath )
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  p_wk  動作コードワーク
*/
//-----------------------------------------------------------------------------
void MoveCodeAttachPlayer::InitProcess( MoveCodeWork* p_wk ) const
{
  MoveCodeAttach::InitProcess(p_wk);
  p_wk->mv_code = MOVE_CODE_ATTACH_PLAYER;
  updateAttachDataFromResource( p_wk , playerAttachResource, PLAYER_ATTACH_RESOURCE_CNT);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン　
*
*  @param  p_wk  動作コードワーク
*/
//-----------------------------------------------------------------------------
void MoveCodeAttachPlayer::MainProcess( MoveCodeWork* p_wk ) const
{
  MoveCodeAttach::MainProcess( p_wk );

  // 共有ワークを取得
  AttachData* p_local_work = (AttachData*)MoveCodeBase::GetMoveCodeWork( p_wk, sizeof(AttachData) );

  // 親にアタッチ処理( 毎フレーム呼んでいるのは親が自身の登録を外すタイミングがあるため )
  if( !p_wk->p_model->GetParent() )
  {
    // 動作モデルマネージャーを取得
    GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    fld::mmodel::MoveModelManager* pMoveModelManager = game_manager->GetFieldSystem()->GetMoveModelManager();

    // 親にアタッチ
    if( pMoveModelManager )
    {
      MoveModel* pPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
      if( pPlayer )
      {
        p_wk->p_model->SetParent( pPlayer,p_local_work->parentJoint );
      }
    }
  }

  // 同期アニメーション
  s32 animationId = getAnimationIdFromResource( p_wk , playerAttachResource, PLAYER_ATTACH_RESOURCE_CNT);
  if( animationId != -1 && p_wk->p_model->GetAnimeID() != animationId )
  {
    p_wk->p_model->ChangeAnimation( animationId , false , 0.0f , 1.0f );
  }
}

}; //end of namespace mmodel
}; //end of namespace fld
