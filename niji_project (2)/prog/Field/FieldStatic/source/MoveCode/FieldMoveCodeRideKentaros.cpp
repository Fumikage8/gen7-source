//=================================================================================================
/**
*  @file   FieldMoveCodeRideKentaros.cpp
*  @brief  フィールド用 動作コード ケンタロスライド
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideKentaros.h"
// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"

namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeRideKentaros::FieldMoveCodeRideKentaros(void)
: m_hitStopFrame(3)
, m_cameraShakeMass(1.0f)
, m_cameraShakeK(100.0f)
, m_cameraFirstVelocity(30.0f)
, m_cameraDamping(0.7f)
, m_isAcceptJump(true)
{
  // 待機
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionId     = P2_BASE_RI001_WAIT01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].stickLevel   = 0.0f;
  }
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_TOKUSHU    ].moveSpeed    = 0.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_TOKUSHU    ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_TOKUSHU    ].motionId     = P2_BASE_RI012_RUSHWAIT01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_TOKUSHU    ].stickLevel   = -1.0f;
  }
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_DISCOVER    ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_DISCOVER    ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_DISCOVER    ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_DISCOVER    ].stickLevel   = -1.0f;
  }

  // 歩き
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].moveSpeed    =  0.9f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionId     =  P2_BASE_RI004_RUNSTART01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].stickLevel   =  0.4f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].moveSpeed    =  0.9f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionId     =  P2_BASE_RI003_RUN01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionId     =  P2_BASE_RI005_RUNSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].stickLevel   = -1.0f;
  }

  // 走り
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].moveSpeed    = 2.25f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].motionId     = P2_BASE_RI014_RUSHSTART01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].moveSpeed    = 2.25f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].motionId     = P2_BASE_RI013_RUSH01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE  ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE  ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE  ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE  ].stickLevel   =-1.0f;
    
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].moveSpeed    = 1.5f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].motionId     = P2_BASE_RI015_RUSHSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].stickLevel   = -1.0f;
  }

  // その他
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].motionId     =  -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].stickLevel   = -1.0f;
  }

  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].stickLevel   = -1.0f;
  }

  // ぴより(右)
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].stickLevel   = -1.0f;
  }

  // ぴより(左)
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].stickLevel   = -1.0f;
  }

  // アイドル
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].moveSpeed       =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].motionSpeed     =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].motionId        = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].stickLevel      = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].moveSpeed       =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].motionSpeed     =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].motionId        = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].stickLevel      = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].stickLevel   = -1.0f;
  }

  // 躓き
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].stickLevel   = -1.0f;
  }

#if USE_RTTI_DEBUG
  RegistRtti();
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeRideKentaros::~FieldMoveCodeRideKentaros()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKentaros::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodePlayer::InitProcess( pMoveCodeWork );

  //動作コードIDを上書き
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_RIDE_KENTAROS;
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeRideKentaros::RegistRtti(void) const
{
#if USE_RTTI_DEBUG

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(FieldMoveCodeRideKentaros,FieldMoveCodePlayer);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST     ( FieldMoveCodeRideKentaros , m_isAcceptJump        , L"段差ジャンプができる"     );
    RTTI_PARAM_REGIST     ( FieldMoveCodeRideKentaros , m_hitStopFrame        , L"ヒットストップのフレーム" );
    RTTI_PARAM_REGIST_ATTR( FieldMoveCodeRideKentaros , m_cameraShakeMass     , L"カメラ用バネの先端の質量" , gfl2::rttidebug::RttiParamManager::ATTR_ADD_SMALL );
    RTTI_PARAM_REGIST     ( FieldMoveCodeRideKentaros , m_cameraShakeK        , L"カメラ用バネの硬さ"       );
    RTTI_PARAM_REGIST     ( FieldMoveCodeRideKentaros , m_cameraFirstVelocity , L"カメラ用バネの初速度"     );
    RTTI_PARAM_REGIST_ATTR( FieldMoveCodeRideKentaros , m_cameraDamping       , L"カメラ用バネの減衰率"     , gfl2::rttidebug::RttiParamManager::ATTR_ADD_SMALL );
  RTTI_END_REGIST( FieldMoveCodeRideKentaros );

  FieldMoveCodePlayer::RegistRtti();

#endif
}

}; //end of namespace MoveModel
}; //end of namespace Field
