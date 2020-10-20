//=================================================================================================
/**
*  @file   ActionCommandRefuse.cpp
*  @brief  フィールド用 アクションコマンド 拒否する
*  @author yy-ikeuchi
*  @date   2015.02.27
*/
//=================================================================================================
// ActionCommand
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommand_define.h"
#include "Field/FieldRo/include/ActionCommand/FieldActionCommandRefuse.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "niji_reference_files/script/FieldPawnTypes.h"
// Sound
#include "Sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief コンストラクタ
*/
//-----------------------------------------------------------------------------
ActionCommandRefuse::ActionCommandRefuse()
{
  m_pFuncTbl[0] = &ActionCommandRefuse::ActionInit;
  m_pFuncTbl[1] = &ActionCommandRefuse::ActionMain;
  m_pFuncTbl[2] = &ActionCommandRefuse::ActionEnd;
  SetSequenceMaxNum(ACMDFUNC_SEQ_MAX);
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの初期化
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandRefuse::ActionInit(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  Work* pWork     = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));    
  pWork->frameMax = 0;
  pWork->frame    = 0;

  // プレイヤーの専用処理
  if( pActionCommandWork->pModel->GetEventId() == Field::FIELD_EVENTID_PLAYER )
  {
    // ライド状態かどうか
    FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( pActionCommandWork->pModel );
    if( pPlayer->GetOnMoveModel() )
    {
      // 自身と自身が乗っている動作モデルにもモーションを流し込む
      const u32 MOTION_SLOT_REFUSE = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+9; // いやいやモーション
      const u32 MOTION_SLOT_OFFSET = FieldMoveModelPlayer::GetRideIndex( pPlayer->GetOnMoveModel()->GetCharacterId() );
      pPlayer->GetCharaDrawInstance()->ChangeAnimation                  ( MOTION_SLOT_REFUSE + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
      pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( MOTION_SLOT_REFUSE + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * MOTION_SLOT_OFFSET ); 
    }
    else
    {
      pPlayer->GetCharaDrawInstance()->ChangeAnimation( P2_BASE_FI110_SHAKY01 ); 
    }
    pWork->frameMax = pActionCommandWork->pModel->GetCharaDrawInstance()->GetAnimationEndFrame();
  }

  // @fix GFNMCat[519] イベント踏んでいる時は壁ドンSEがならなくなったので個別でSEを鳴らす
  Sound::PlaySE(SEQ_SE_FLD_WALL_HIT);

  // 次へ
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの実行
*
*  @param  pActionCommandWork アクションコマンドワーク
*  @return アクションコマンドの状態
*/
//-----------------------------------------------------------------------------
ACTION_COMMAND_RETURN ActionCommandRefuse::ActionMain(Field::MoveModel::ActionCommandWork* pActionCommandWork)
{
  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  Work* pWork = reinterpret_cast<Work*>(FieldActionCommandManager::GetActionCommandWork( pActionCommandWork, sizeof(Work) ));
  pWork->frame++;
  if( pWork->frame <= pWork->frameMax )
  {
    return ACTION_COMMAND_RETURN_ONE_TIME_END;
  }

  // 次へ( アクションコマンドの終了)
  FieldActionCommandManager::AddActionCommandWorkSequence( pActionCommandWork, 1 );
  return ACTION_COMMAND_RETURN_CONTINUE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの終了
*
*  @param  pActionCommandWork  アクションコマンドワーク
*
*  @retval ACTION_COMMAND_RETURN_END
*/
//-----------------------------------------------------------------------------
Field::MoveModel::ACTION_COMMAND_RETURN ActionCommandRefuse::ActionEnd(ActionCommandWork* /*pActionCommandWork*/)
{
  return ACTION_COMMAND_RETURN_END;
}


} // mmodel
} // field
