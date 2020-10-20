//=================================================================================================
/**
*  @file   FieldMoveCodeRideKairiky.cpp
*  @brief  フィールド用 動作コード カイリキーライド
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideKairiky.h"
// Sound
#include "Sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKairiky::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::MainProcess(pMoveCodeWork);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  ポスト
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKairiky::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::PostProcess( pMoveCodeWork );

  // 泣き声
  if( IsTrgInputDashButton() )
  {
    // @fix GFNMCat[965] 泣き声SEを後勝ちにする
    if( Sound::IsSEPlaying( SEQ_SE_FLD_0018 ) ){ Sound::StopSE( SEQ_SE_FLD_0018 ); }
    Sound::PlaySE(SEQ_SE_FLD_0018);
  }
};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  再開
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKairiky::Resume(FieldMoveCodeWork* pMoveCodeWork) const 
{
  FieldMoveCodePlayer::Resume(pMoveCodeWork);
};

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  中断
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKairiky::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::Suspend( pMoveCodeWork );

  // 子キャラ(ライドポケモン)も待機状態に変更
  if( !pMoveCodeWork->pModel->CheckStateBit( FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND ) )
  {
    Field::MoveModel::FieldMoveModelPlayer* pPlayer   = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
    Field::MoveModel::FieldMoveModel*       pKairiky = pPlayer->GetOnMoveModel();
    if( pKairiky )
    {
      if(pKairiky->GetCharaDrawInstance()->GetAnimationId() != (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ))
      {
        pKairiky->GetCharaDrawInstance()->ChangeAnimation( (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() ) );
      }
    }
  }
};

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodeRideKairiky::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  return FieldMoveCodePlayer::MainProcessSubOverrideState(pMoveCodeWork,state,nowState,oldState);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メインのサブ関数(ステートが変わったときに呼ばれる)
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKairiky::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 state,u32 nowState,u32 oldState) const
{
  FieldMoveCodePlayer::MainProcessSubChangeState(pMoveCodeWork,state,nowState,oldState);

  // 子キャラ(ライドポケモン)もモーションを変更
  Field::MoveModel::FieldMoveModelPlayer* pPlayer      = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveCodeWork->pModel);
  Field::MoveModel::FieldMoveModel*       pKairiky     = pPlayer->GetOnMoveModel();
  KairikyWork*                            pKairikyWork = (KairikyWork*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(KairikyWork) );

  if( pKairiky )
  {
    pKairiky->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
    if( !pKairikyWork->playerWork.isContinueMotion )
    {
      pKairiky->GetCharaDrawInstance()->ChangeAnimation(m_PlayerMoveSettingTable[state].motionId);
    }
  }
}

}; //end of namespace MoveModel
}; //end of namespace Field
