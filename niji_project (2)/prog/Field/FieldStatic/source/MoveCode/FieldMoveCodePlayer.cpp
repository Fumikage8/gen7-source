//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.cpp
*  @brief  フィールド用 動作コード プレイヤー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// gfl2
#include <types/include/gfl2_Typedef.h>
// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodePlayer.h"
// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"
// Convヘッダー
#include "niji_conv_header/field/chara/p2_base_fi.h"

namespace Field{ namespace MoveModel {

const f32 FieldMoveCodePlayer::WATER_DEPTH = 20.0f;

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodePlayer::FieldMoveCodePlayer(void) 
: m_interpStickDefault  (100)
, m_interpStickRun      ( 30)
, m_runTurnDeg          (120.0f)
, m_piyoriMaxValue      (30)
, m_piyoriAngle         (10)
, m_neutralRange        (20)
, m_slopeDampingRate    (6.00f)
{
  // 待機
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionId     = P2_BASE_FI001_WAIT01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].stickLevel   = 0.0f;
  }
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_TOKUSHU    ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_TOKUSHU    ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT_TOKUSHU    ].motionId     = -1;
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
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].moveSpeed    = 1.2f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionId     = P2_BASE_FI010_WALK01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].stickLevel   = 0.4f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].moveSpeed    = 1.2f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].motionId     = P2_BASE_FI071_SLOPEWALK01_UP;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_SLOPE ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].moveSpeed    = 1.2f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].motionId     = P2_BASE_FI018_WALKGRASS01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP_KUSAMURA ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionId     = P2_BASE_FI011_WALKSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].stickLevel   = -1.0f;
  }

  // 走り
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START         ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START         ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START         ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START         ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP          ].moveSpeed    = 1.2f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP          ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP          ].motionId     = P2_BASE_FI012_RUN01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP          ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE    ].moveSpeed    = 1.2f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE    ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE    ].motionId     = P2_BASE_FI073_SLOPERUN01_UP;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_SLOPE    ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].moveSpeed    = 1.2f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].motionId     = P2_BASE_FI019_RUNGRASS01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_KUSAMURA ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP_DISCOVER ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END           ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END           ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END           ].motionId     = P2_BASE_FI014_RUNSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END           ].stickLevel   =-1.0f;
  }

  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].moveSpeed    =  0.5f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].motionSpeed  =  0.75f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].motionId     =  P2_BASE_FI015_RUNTURN01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_TURN        ].stickLevel   = -1.0f;
  }

  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALLHIT         ].stickLevel   = -1.0f;
  }

  // ぴより(右)
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].motionId     = P2_BASE_FI026_POSERIGHT01_LOOP;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_START ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].motionId     = P2_BASE_FI026_POSERIGHT01_LOOP;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_LOOP  ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].motionId     = P2_BASE_FI028_POSERIGHT01_END;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_RIGHT_END   ].stickLevel   = -1.0f;
  }

  // ぴより(左)
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].motionId     = P2_BASE_FI027_POSELEFT01_LOOP;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_START ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].motionId     = P2_BASE_FI027_POSELEFT01_LOOP;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_LOOP  ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].motionId     = P2_BASE_FI029_POSELEFT01_END;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_PIYORI_LEFT_END   ].stickLevel   = -1.0f;
  }

  // アイドル
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].moveSpeed       =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].motionSpeed     =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].motionId        = P2_BASE_FI002_WAIT02;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_A ].stickLevel      = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].moveSpeed       =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].motionSpeed     =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].motionId        = P2_BASE_FI003_WAIT03;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_B ].stickLevel      = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].motionId     = P2_BASE_FI004_WAIT04;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_WARM ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].motionId     = P2_BASE_FI005_WAIT05;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_IDLE_COOL ].stickLevel   = -1.0f;
  }

  // 躓き
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].moveSpeed    =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].motionSpeed  =  1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].motionId     = P2_BASE_FI110_SHAKY01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_TSUMADUKI ].stickLevel   = -1.0f;
  }

#if USE_RTTI_DEBUG
  RegistRtti();
#endif
};

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodePlayer::~FieldMoveCodePlayer()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_PLAYER;

  u32 START_TURN_FRAME = 4;

  // 共有ワークの初期化
  Work* pWork                  = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  memset( pWork, 0 , sizeof(Work) );
  pWork->nextFoot              = 0;
  pWork->nowState              = PLAYER_MOVE_MAX;
  pWork->oldState              = PLAYER_MOVE_MAX;
  pWork->targetQua             = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
  pWork->angleToTarget         = 0.0f;
  pWork->isTargetRightSide     = false;
  pWork->piyoriNowValue        = 0;
  pWork->inputDir              = 0.0f;
  pWork->inputDirPrev          = 0.0f;
  pWork->isLockDir             = false;
  pWork->isLockFromOutside     = false;
  pWork->lockDir               = 0.0f;
  pWork->moveStartTurnFrame    = START_TURN_FRAME;
  pWork->isMoveSlope           = false;
  pWork->moveSlopeCnt          = 0;
  pWork->isSetupFromAppData    = false;
  pWork->isDiscover            = false;
  pWork->waitFrame             = 0;
  pWork->idleFrame             = 0;
  pWork->idleState             = 0;
  pWork->isContinueMotion      = false;
  pWork->repeatDashButtonCnt   = 0;
  pWork->timeLeftForRepeat     = 0;
  pWork->isMoveKusamura        = false;
  pWork->moveKusamuraCnt       = 0;
  pWork->dashDisableFrame      = 0;
  pWork->lockDirDisableFrame   = 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::RegistRtti(void) const
{
#if USE_RTTI_DEBUG

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpStickDefault    , L"デフォルトのレバー回転補完(パーセント)" );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpStickRun        , L"走り時　　のレバー回転補完(パーセント)" );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_runTurnDeg            , L"走り旋回の再生角度"                     );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_neutralRange          , L"スティックをニュートラルとみなす範囲"   );
  RTTI_END_REGIST( FieldMoveCodePlayer );

#endif
}

}; //end of namespace MoveModel
}; //end of namespace Field
