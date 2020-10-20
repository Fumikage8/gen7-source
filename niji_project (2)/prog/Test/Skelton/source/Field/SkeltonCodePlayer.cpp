#if defined(GF_PLATFORM_WIN32)
//=================================================================================================
/**
 @file SkeltonCode_player.cpp
 @brief		動作コード　処理管理　基底クラス
 @date	 2014.03.17
 @author takahashi_tomoya 2012.11.30
 @author tamada@gamefreak.co.jp

 */
//=================================================================================================

#include "math/include/gfl2_Vector4.h"
#include "math/include/gfl2_Matrix.h"


#include "Test/Skelton/include/Field/SkeltonModelDefine.h"

#include "Test/Skelton/include/Field/SkeltonCodePlayer.h"
#include "Test/Skelton/include/Field/SkeltonModel.h"

#include "Test/Skelton/include/Field/SkeltonFieldTransform.h"
#include "Test/Skelton/include/Field/SkeltonPlayerControl.h"
#include "Test/Skelton/include/Field/SkeltonModelManager.h"

#include "Test/Skelton/include/SkeltonDebugInfo.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

	//----------------------------------------------------------------------------
  /**
   *	@brief  動作コード処理　初期化
   *
   *	@param	p_wk  動作コードワーク
   */
  //-----------------------------------------------------------------------------
  void SkeltonCodePlayer::InitProcess( SkeltonCodeWork* p_wk ) const
	{
    p_wk->mv_code = SKELTON_CODE_PLAYER; 

    // たとえば、ここで、共有ワークを取得
    /*
    Work* p_local_work = SkeltonCodeBase::GetSkeltonCodeWork( p_wk, sizeof(Work) );
     */
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  動作コード処理  メイン　
   *
   *	@param	p_wk  動作コードワーク
   */
  //-----------------------------------------------------------------------------
  void SkeltonCodePlayer::MainProcess( SkeltonCodeWork* p_wk ) const
	{
		bool is_move_command = false;
		gfl2::math::Vector4 mv;
		is_move_command = SkeltonModel::IsMoveCommand( p_wk->p_model, mv );
		float rate = SkeltonModelManager::GetSizeRate() * PlayerControl::GetMoveRate();
		{
			u32 anime_id = p_wk->p_model->GetAnimeID();
			u32 next_anime_id = chara::MOTION_ID_WAIT;
      f32 move_length = (mv).Length3();
      f32 move_meter = move_length * 30.0f / 100.0f;
			if ( is_move_command )
			{
         u8 playerMove = p_wk->p_model->GetCommand().data.vec_data.palyerMove;

         if( p_wk->p_model->IsAngleLock() )
         {
           playerMove = PlayerControl::PLAYER_MOVE_WALK;
         }

         SkeltonDebugInfo::PlayerMoveSetting& rPlayerMoveSetting = SkeltonDebugInfo::m_PlayerMoveSettingTable[ playerMove ];
         next_anime_id = rPlayerMoveSetting.motionId;

         if( rPlayerMoveSetting.motionSpeedAutomaticFlag )
         {
           SkeltonDebugInfo::m_DebugInfoPlayerMotionStepFrame = move_meter / rPlayerMoveSetting.motionBaseMoveSpeed;
         }
         else
         {
           SkeltonDebugInfo::m_DebugInfoPlayerMotionStepFrame = rPlayerMoveSetting.motionSpeed;
         }

         SkeltonDebugInfo::m_DebugInfoPlayerMove = p_wk->p_model->GetCommand().data.vec_data.palyerMove;
         SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed = move_meter;
			}
      else
      {
        SkeltonDebugInfo::m_DebugInfoPlayerMotionStepFrame = 1.0f;
        SkeltonDebugInfo::m_DebugInfoPlayerMove = PlayerControl::PLAYER_MOVE_MAX;
        SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed = 0.0f;
      }

			if ( anime_id != next_anime_id )
			{
        if( anime_id == chara::MOTION_ID_SP && next_anime_id == chara::MOTION_ID_WAIT && p_wk->p_model->IsAnimeLastFrame() == false )
        {
          return;
        }

				//p_wk->p_model->SetAnimation( next_anime_id );
        f32 interp_frame = 5.0f;

        if( anime_id == chara::MOTION_ID_SP )
        {
           interp_frame = 0.0f;
        }

        if( next_anime_id == chara::MOTION_ID_WAIT )
        {
          interp_frame = 5.0f;
        }

        f32 animation_t = p_wk->p_model->GetAnimationFrame() / p_wk->p_model->GetAnimationMaxFrame();
				p_wk->p_model->ChangeAnimation( next_anime_id, true, interp_frame );
#if 0
        if( 
          ( anime_id == chara::MOTION_ID_RUN || anime_id == chara::MOTION_ID_WALK || anime_id == chara::MOTION_ID_WALK_FAST ) &&
          ( next_anime_id == chara::MOTION_ID_RUN || next_anime_id == chara::MOTION_ID_WALK || next_anime_id == chara::MOTION_ID_WALK_FAST )
          )
#endif
        {
          p_wk->p_model->SetAnimationFrame( animation_t * p_wk->p_model->GetAnimationMaxFrame() );
        }
			}
		}

		/*
		 p_wk->p_model->UpdateGridPosCurrent(); 
		 */
  } 

  GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32