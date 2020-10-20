//=================================================================================================
/**
 *  @file  movecode_player.cpp
 *  @brief  動作コード　処理管理　基底クラス
 *  @date  2014.03.17
 *  @author takahashi_tomoya 2012.11.30
 *  @author tamada@gamefreak.co.jp

 */
//=================================================================================================
#include "math/include/gfl2_Vector.h"
#include "math/include/gfl2_Matrix.h"

#include "../include/movemodel_define.h"
#include "../include/movecode_npc.h"
#include "../include/movemodel.h"
#include "../include/field_transform.h"
#include "../include/movemodel_manager.h"
#include "GameSys/include/GameManager.h"

// Field
#include "test/FieldTest/include/FieldSystem.h"
#include "test/FieldTest/include/Camera/FieldCamera.h"
#include "test/FieldTest/include/Debug/FieldDebugTypes.h"


namespace fld{ namespace mmodel {

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  p_wk  動作コードワーク
*/
//-----------------------------------------------------------------------------
void MoveCodeNpc::InitProcess( MoveCodeWork* p_wk ) const
{
  p_wk->mv_code = MOVE_CODE_NPC; 
  p_wk->p_model->ChangeAnimation( chara::MOTION_ID_WAIT, true, 5.0f, 1.0f );

  // @todo 仮
  WORK* pWork = static_cast<WORK*>(reinterpret_cast<void*>(p_wk->work));
  pWork->animationCount = 0;

  // たとえば、ここで、共有ワークを取得
  /*
  Work* p_local_work = MoveCodeBase::GetMoveCodeWork( p_wk, sizeof(Work) );
  */
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン　
*
*  @param  p_wk  動作コードワーク
*/
//-----------------------------------------------------------------------------
void MoveCodeNpc::MainProcess( MoveCodeWork* p_wk ) const
{
  GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  fld::mmodel::MoveModelManager* pMoveModelManager = game_manager->GetFieldSystem()->GetMoveModelManager();

  if( pMoveModelManager )
  {
    const MoveModel* pPlayer = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_PLAYER );
    if( pPlayer && pPlayer->GetTransform() )
    {
      gfl2::math::Vector  targetVec ( pPlayer->GetTransform()->GetTranslation() );
      gfl2::math::Vector3 targetVec3( targetVec.x,targetVec.y,targetVec.z );
      p_wk->p_model->setInterestTarget( targetVec3 );

      if( p_wk->p_model->m_IsInterpLoop )
      {
        WORK* pWork = static_cast<WORK*>(reinterpret_cast<void*>(p_wk->work));
        pWork->animationCount++;
        u32 frameInter = 5;

        if( pWork->animationCount > frameInter )
        {
          pWork->animationCount = 0;
          u32 motionId = chara::MOTION_ID_WAIT;
          switch( p_wk->p_model->GetAnimeID() )
          {
          case chara::MOTION_ID_WAIT:
            {
              motionId = chara::MOTION_ID_WALK;
              break;
            }
          }
          p_wk->p_model->ChangeAnimation( motionId, true, frameInter, 1.0f );
        }
      }


#if 1
      // 自機に同期
      if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::ENABLE_POKE_RIDE ] )
      {
        if( p_wk->p_model->GetHeader().mmdl_id == MMDL_TEST_KENTAUROS )
        {
          gfl2::math::Vector offset( 0.0f, 0.0f, -30.0f );
          gfl2::math::Matrix mat;
          pPlayer->GetTransform()->GetQuaternion().QuaternionToMatrix( mat );
          offset = mat.TransformCoord( offset );

          p_wk->p_model->GetTransform()->SetTranslation( pPlayer->GetTransform()->GetTranslation() + offset );
          p_wk->p_model->GetTransform()->SetQuaternion( pPlayer->GetTransform()->GetQuaternion() );
          //offset = offset * pPlayer->GetTransform()->GetQuaternion();

          if( p_wk->p_model->GetAnimeID() != pPlayer->GetAnimeID() )
          {
            p_wk->p_model->ChangeAnimation( pPlayer->GetAnimeID(), true, 5.0f, 1.0f );
          }
        }
      }
#endif
    }
  }
}

}; //end of namespace mmodel
}; //end of namespace fld
