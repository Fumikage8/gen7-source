//=================================================================================================
/**
 *	@file	movecode_player.cpp
 *	@brief	動作コード　処理管理　基底クラス
 *	@date	2014.03.17
 *	@author takahashi_tomoya 2012.11.30
 *	@author tamada@gamefreak.co.jp

 */
//=================================================================================================
#include "math/include/gfl2_Vector.h"
#include "math/include/gfl2_Matrix.h"

#include "../include/movemodel_define.h"
#include "../include/movecode_player.h"
#include "../include/movemodel.h"
#include "../include/field_transform.h"
#include "../include/movemodel_manager.h"
#include "GameSys/include/GameManager.h"

// Field
#include "test/FieldTest/include/FieldSystem.h"
#include "test/FieldTest/include/Camera/FieldCamera.h"

enum PlayerMove
{
	PLAYER_MOVE_WAIT=0,
	PLAYER_MOVE_WALK_SLOW,
	PLAYER_MOVE_WALK,
	PLAYER_MOVE_WALK_FAST,
	PLAYER_MOVE_RUN,
	PLAYER_MOVE_DEBUG_RUN,
	PLAYER_MOVE_MAX
};

struct PlayerMoveSetting
{
	c16  name[256];
    f32  moveSpeed;			// 移動速度
    f32  motionSpeed;		// モーション再生速度
    s32  motionId;			// 再生モーション
    f32  stickLevel;		// 入力値がこの値以下なら発動対象
};

PlayerMoveSetting m_PlayerMoveSettingTable[ PLAYER_MOVE_MAX ] =
{
	// PLAYER_MOVE_WAIT
	{
		L"待機",
		0.0f,
		1.0f,
		chara::MOTION_ID_WAIT,
		0.0f
	},

	// PLAYER_MOVE_WALK_SLOW
	{
		L"低速歩き",
		0.9f * 100.0f / 30.0f,    // m/sをフレームレートに合わせた値に加工
		0.9f / 1.6f,              // 実際の移動量(m/s) / モーションが想定している移動量(m/s)
		chara::MOTION_ID_WALK,
		0.1f
	},

	// PLAYER_MOVE_WALK
	{
		L"歩き", 
		1.6f * 100.0f / 30.0f, 
		1.0f,
		chara::MOTION_ID_WALK,
		0.4f
	},

  // PLAYER_MOVE_WALK_FAST
	{
		L"早歩き", 
		2.7f * 100.0f / 30.0f, 
		1.0f,
		chara::MOTION_ID_WALK_FAST,
		0.8f
	},

	// PLAYER_MOVE_RUN
	{
		L"走り", 
		5.8f * 100.0f / 30.0f, 
		1.0f,
		chara::MOTION_ID_RUN,
		2.0f
	},

	// PLAYER_MOVE_DEBUG_RUN
	{
		L"デバッグ走り", 
		5.8f * 2.0f * 100.0f / 30.0f,   // 自転車速度を想定、走りの2倍
		5.8f * 2.0f / 5.8f,             // 実際の移動量(m/s) / モーションが想定している移動量(m/s)
		chara::MOTION_ID_RUN,
		3.0f
	},
};

namespace fld{ namespace mmodel {

//----------------------------------------------------------------------------
/**
*	@brief  動作コード処理　初期化
*
*	@param	p_wk  動作コードワーク
*/
//-----------------------------------------------------------------------------
void MoveCodePlayer::InitProcess( MoveCodeWork* p_wk ) const
{
	p_wk->mv_code = MOVE_CODE_PLAYER; 

	// たとえば、ここで、共有ワークを取得
	/*
	Work* p_local_work = MoveCodeBase::GetMoveCodeWork( p_wk, sizeof(Work) );
	*/
}

//----------------------------------------------------------------------------
/**
*	@brief  動作コード処理  メイン　
*
*	@param	p_wk  動作コードワーク
*/
//-----------------------------------------------------------------------------
void MoveCodePlayer::MainProcess( MoveCodeWork* p_wk ) const
{
	// コントローラーによる入力を取得(UIの入力システムに乗っかっています 要修正)
	GameSys::GameManager*		game_manager	= GFL_SINGLETON_INSTANCE( GameSys::GameManager );
	gfl2::ui::DeviceManager*	device_manager	= game_manager->GetUiDeviceManager();
	gfl2::ui::VectorDevice*		pVecDevice		= device_manager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
	gfl2::ui::Button*			pButton			= device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
	f32 x = pVecDevice->GetX();
	f32 y = pVecDevice->GetY();
	f32 length = sqrt( x*x + y*y );

	if( length > 0.0f )
	{
		// 通常走りを起動させる
	#if defined(GF_PLATFORM_CTR)
		if( pButton->IsHold( gfl2::ui::BUTTON_B ) )
	#else
		if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
	#endif
		{
			length = 2.f;
		}
		
    // デバッグ走りを起動させる
		if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
		{
			length = 3.f;
		}
	}

  // カメラ操作中は自機操作を止める
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    length = 0.0f;
  }

	// 入力情報をみてステートを選定
	u32 state = getStateFromStickLevel( length );

	// srtの更新
	if( length > 0.0f )
	{
		// アナログスティックの倒し具合からキャラの向きを決定
		gfl2::math::Vector raw_move( x, 0, y, 0 );
		gfl2::math::Vector normal_raw_move = raw_move.Normalize3();

		//
		// カメラ方向を元に、移動方向を加工する処理
		//     
	    f32 cameraEffect = 0.0f;
		Field::FieldSystem* pFieldSystem = game_manager->GetFieldSystem();
		// 無いはずはない
		if( pFieldSystem )
		{
			Field::Camera::FieldCamera* pPlayerCamera = pFieldSystem->GetPlayerCamera();
			// 無いことがあるかも
			if( pPlayerCamera )
			{
				cameraEffect = pPlayerCamera->GetYaw();
			}
		}

		float dir = gfl2::math::NormalizeRadianAngle( atan2( normal_raw_move.z, normal_raw_move.x ) + 0.5f * PI + cameraEffect );
		gfl2::math::Quaternion qua;
		qua.RadianYToQuaternion(dir);

		// キャラの向きから移動方向・移動量を決定
		gfl2::math::Matrix mat;
		qua.QuaternionToMatrix(mat);
		gfl2::math::Vector vec = mat.TransformNormal( gfl2::math::Vector(0.0f,0.0f,1.0f) ) * m_PlayerMoveSettingTable[state].moveSpeed;

		// 更新
		p_wk->p_model->GetTransform()->SetQuaternion( qua );
		p_wk->p_model->GetTransform()->SetTranslation( vec + p_wk->p_model->GetTransform()->GetTranslation() );
	}

	// モーションの変更
	u32  anime_id	   = p_wk->p_model->GetAnimeID();
	u32  next_anime_id = m_PlayerMoveSettingTable[state].motionId;
	if ( anime_id != next_anime_id )
	{
		p_wk->p_model->ChangeAnimation( next_anime_id, true, 5.0f, m_PlayerMoveSettingTable[state].motionSpeed );
	}

	// アクションコマンドテスト
	if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
	{
		gfl2::math::Quaternion qua;
		qua.RadianYToQuaternion( PI );
		p_wk->p_model->SetAcmd( AC_TSURI_TEST , qua );
	}

  // LODテスト
#if 0
#if defined(GF_PLATFORM_CTR)
	if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
#else
	if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
#endif
  {
    if( p_wk->p_model->getLod() == MoveModel::LOD_HIGH )
    {
      p_wk->p_model->ChangeLod( MoveModel::LOD_LOW );
    }
    else
    {
      p_wk->p_model->ChangeLod( MoveModel::LOD_HIGH );
    }
    p_wk->p_model->ChangeAnimation( chara::MOTION_ID_WAIT , true , 0.0f, 1.0f );
  }
#endif

  // 注視テスト
  fld::mmodel::MoveModelManager* pMoveModelManager = game_manager->GetFieldSystem()->GetMoveModelManager();
  const MoveModel* pNpc = pMoveModelManager->GetMoveModel( fld::mmodel::MMDL_NPC_0 );
  if( pNpc && pNpc->GetTransform() )
  {
    gfl2::math::Vector  def = p_wk->p_model->GetTransform()->GetTranslation() - pNpc->GetTransform()->GetTranslation();
    if( def.Length() < 600.0f )
    {
      gfl2::math::Vector  targetVec ( pNpc->GetTransform()->GetTranslation() );
      gfl2::math::Vector3 targetVec3( targetVec.x,targetVec.y,targetVec.z );
      p_wk->p_model->setInterestTarget( targetVec3 );
      p_wk->p_model->setInterest(true);
    }
    else
    {
      p_wk->p_model->setInterest(false);
    }
  }

}

//----------------------------------------------------------------------------
/**
*	@brief  アナログスティックの倒し具合からステートを取得
*
*	@param	stick  アナログスティックの倒し具合
*	@return ステート
*/
//-----------------------------------------------------------------------------
u32 MoveCodePlayer::getStateFromStickLevel( f32 stick ) const
{
	for( s32 i=PLAYER_MOVE_MAX-1 ; i>=0 ; i-- )
	{
		if( m_PlayerMoveSettingTable[i].stickLevel <= stick )
		{
			return i;
		}
	}
	return PLAYER_MOVE_WALK_FAST;
}

}; //end of namespace mmodel
}; //end of namespace fld
