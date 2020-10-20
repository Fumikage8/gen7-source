//=================================================================================================
/**
*  @file   FieldMoveCodeTemplate.cpp
*  @brief  フィールド用 動作コード テンプレート(こちらをコピーして量産してください)
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../../include/MoveModel/FieldMoveModel_define.h"
#include "../../include/MoveCode/FieldMoveCode_define.h"
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodePlayer.h"
#include "../../include/ActionCommand/FieldActionCommandManager.h"
#include "../../include/ActionCommand/FieldActionCommand_define.h"
#include "GameSys/include/GameManager.h"

namespace Field{ namespace MoveModel {

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
  f32  moveSpeed;     // 移動速度
  f32  motionSpeed;   // モーション再生速度
  s32  motionId;      // 再生モーション
  f32  stickLevel;    // 入力値がこの値以下なら発動対象
};

PlayerMoveSetting m_PlayerMoveSettingTable[ PLAYER_MOVE_MAX ] =
{
  // PLAYER_MOVE_WAIT
  {
    L"待機",
    0.0f,
    1.0f,
    0,
    0.0f
  },

  // PLAYER_MOVE_WALK_SLOW
  {
    L"低速歩き",
    0.9f * 100.0f / 30.0f,    // m/sをフレームレートに合わせた値に加工
    0.9f / 1.6f,              // 実際の移動量(m/s) / モーションが想定している移動量(m/s)
    0,
    0.1f
  },

  // PLAYER_MOVE_WALK
  {
    L"歩き", 
    1.6f * 100.0f / 30.0f, 
    1.0f,
    0,
    0.4f
  },

  // PLAYER_MOVE_WALK_FAST
  {
    L"早歩き", 
    2.7f * 100.0f / 30.0f, 
    1.0f,
    0,
    0.8f
  },

  // PLAYER_MOVE_RUN
  {
    L"走り", 
    5.8f * 100.0f / 30.0f, 
    1.0f,
    0,
    2.0f
  },

  // PLAYER_MOVE_DEBUG_RUN
  {
    L"デバッグ走り", 
    5.8f * 2.0f * 100.0f / 30.0f,   // 自転車速度を想定、走りの2倍
    5.8f * 2.0f / 5.8f,             // 実際の移動量(m/s) / モーションが想定している移動量(m/s)
    0,
    3.0f
  },
};


//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_TEMPLATE; 

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
    // コントローラーによる入力を取得(UIの入力システムに乗っかっています 要修正)
  GameSys::GameManager*     game_manager    = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager*  device_manager  = game_manager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*   pVecDevice      = device_manager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button*         pButton         = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
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

  // 入力情報をみてステートを選定
  u32 state = getStateFromStickLevel( length );

  // srtの更新
  if( length > 0.0f )
  {
    // アナログスティックの倒し具合からキャラの向きを決定
    gfl2::math::Vector raw_move( x, 0, y, 0 );
    gfl2::math::Vector normal_raw_move = raw_move.Normalize3();

    float dir = gfl2::math::NormalizeRadianAngle( atan2( normal_raw_move.z, normal_raw_move.x ) + 0.5f * PI );
    gfl2::math::Quaternion qua;
    qua.RadianYToQuaternion(dir);

    // キャラの向きから移動方向・移動量を決定
    gfl2::math::Matrix  mat;
    qua.QuaternionToMatrix(mat);
    gfl2::math::Vector vec = mat.TransformNormal( gfl2::math::Vector(0.0f,0.0f,1.0f) ) * m_PlayerMoveSettingTable[state].moveSpeed;

    // 更新
    pMoveCodeWork->pModel->SetRotationQuat( qua );
    pMoveCodeWork->pModel->SetPosition( gfl2::math::Vector3(vec.x,vec.y,vec.z) + pMoveCodeWork->pModel->GetPosition() );
  }

}

//----------------------------------------------------------------------------
/**
*  @brief  アナログスティックの倒し具合からステートを取得
*
*  @param  stick  アナログスティックの倒し具合
*  @return ステート
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodePlayer::getStateFromStickLevel( f32 stick ) const
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

}; //end of namespace MoveModel
}; //end of namespace Field
