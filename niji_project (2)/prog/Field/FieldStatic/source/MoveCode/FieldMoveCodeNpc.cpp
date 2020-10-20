//=================================================================================================
/**
*  @file   FieldMoveCodeNpc.cpp
*  @brief  フィールドNPC用 動作コード
*  @author Junya Ikuta
*  @date   2016.03.23
*/
//=================================================================================================

// gfl2
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Easing.h>

// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"

// Convヘッダー
#include <niji_conv_header/field/chara/p2_base_fi.h>

namespace Field{ namespace MoveModel {


// 挙動設定
FieldMoveCodeNpc::SETTING FieldMoveCodeNpc::m_Setting =
{
  75.0f, // f32 degreeRangeForInterest;             // 注目発動範囲[角度]
  75.0f, // f32 degreeRangeForRotation;             // 回転発動範囲[角度]

  350.0f, // f32 distanceRangeForInterest;          // 注目発動範囲[距離]
  700.0f, // f32 distanceRangeForInterestWide;      // 注目発動範囲(ワイド版)[距離]
  200.0f, // f32 distanceRangeForInterestShort;     // 注目発動範囲(縮小版)[距離]
  200.0f, // f32 distanceRangeForRotation;          // 回転発動範囲[距離]

  // 内側の判定を別に持つ
  160.0f, // f32 inSideDegreeRangeForInterest;      // 注目発動範囲[角度][内側]
  100.0f, // f32 inSideDegreeRangeForRotation;      // 回転発動範囲[角度][内側]
  150.0f, // f32 inSideDistanceRangeForInterest;    // 注目発動範囲[距離][内側]
  150.0f, // f32 inSIdeDistanceRangeForRotation;    // 回転発動範囲[距離][内側]

  // 最近接の判定を別に持つ
  180.0f, // f32 nearestDegreeRangeForInterest;     // 注目発動範囲[角度][最近接]
  180.0f, // f32 nearestDegreeRangeForRotation;     // 回転発動範囲[角度][最近接]
  95.0f, // f32 nearestDistanceRangeForInterest;    // 注目発動範囲[距離][最近接]
  95.0f, // f32 nearestDistanceRangeForRotation;    // 回転発動範囲[距離][最近接]

  400.0f, // f32 talkEndResetDistanceRange;        // 会話終了をリセットする距離
  400.0f, // f32 dirResetDistanceRange;            // 向きをリセットする距離
  30,     // u32 dirResetWaitBase;                 // 向きをリセットする際の待ちフレーム(最低保障)
  30,     // u32 dirResetWaitRand;                 // 向きをリセットする際の待ちフレーム(最低保障に足しこむ乱数値)
  250.0f, // f32 talkEndResetDistanceRangeShort;   // 会話終了をリセットする距離(縮小版)
  250.0f, // f32 dirResetDistanceRangeShort;       // 向きをリセットする距離(縮小版)

  8.0f,   // f32 degreeDiffForRotationChange;      // 回転を実行する角度の差分
  10,     // u32 enterWaitForInterestToRotation;   // 回転発動待ち時間、このフレーム間注目範囲にいたら回転を実行する
  30,     // f32 disableInterestFrame;             // 注目解除にかけるフレーム
  20,     // u32 enterWaitForReturn;               // 元の状態に戻す発動待ち時間、このフレーム間何も反応がなければ実行する
  16,     // u32 rotationFrame;                    // 回転にかけるフレーム数
  false,  // b32 isCullingReset;                   // true:カリング時に再度有効化される
  20.0f,  // f32 hideRangeDeg;                     // 背後扱いの角度(片翼)
};

FieldMoveCodeFureaiTypeA::SETTING FieldMoveCodeFureaiTypeA::m_Setting =
{
  75.0f, // f32 degreeRangeForInterest;             // 注目発動範囲[角度]
  75.0f, // f32 degreeRangeForRotation;             // 回転発動範囲[角度]

  350.0f, // f32 distanceRangeForInterest;          // 注目発動範囲[距離]
  700.0f, // f32 distanceRangeForInterestWide;      // 注目発動範囲(ワイド版)[距離]
  200.0f, // f32 distanceRangeForInterestShort;     // 注目発動範囲(縮小版)[距離]
  200.0f, // f32 distanceRangeForRotation;          // 回転発動範囲[距離]

  // 内側の判定を別に持つ
  160.0f, // f32 inSideDegreeRangeForInterest;      // 注目発動範囲[角度][内側]
  100.0f, // f32 inSideDegreeRangeForRotation;      // 回転発動範囲[角度][内側]
  150.0f, // f32 inSideDistanceRangeForInterest;    // 注目発動範囲[距離][内側]
  150.0f, // f32 inSIdeDistanceRangeForRotation;    // 回転発動範囲[距離][内側]

  // 最近接の判定を別に持つ
  180.0f, // f32 nearestDegreeRangeForInterest;     // 注目発動範囲[角度][最近接]
  180.0f, // f32 nearestDegreeRangeForRotation;     // 回転発動範囲[角度][最近接]
  95.0f, // f32 nearestDistanceRangeForInterest;    // 注目発動範囲[距離][最近接]
  95.0f, // f32 nearestDistanceRangeForRotation;    // 回転発動範囲[距離][最近接]

  400.0f, // f32 talkEndResetDistanceRange;        // 会話終了をリセットする距離
  400.0f, // f32 dirResetDistanceRange;            // 向きをリセットする距離
  30,     // u32 dirResetWaitBase;                 // 向きをリセットする際の待ちフレーム(最低保障)
  30,     // u32 dirResetWaitRand;                 // 向きをリセットする際の待ちフレーム(最低保障に足しこむ乱数値)
  250.0f, // f32 talkEndResetDistanceRangeShort;   // 会話終了をリセットする距離(縮小版)
  250.0f, // f32 dirResetDistanceRangeShort;       // 向きをリセットする距離(縮小版)

  8.0f,   // f32 degreeDiffForRotationChange;      // 回転を実行する角度の差分
  10,     // u32 enterWaitForInterestToRotation;   // 回転発動待ち時間、このフレーム間注目範囲にいたら回転を実行する
  30,     // f32 disableInterestFrame;             // 注目解除にかけるフレーム
  20,     // u32 enterWaitForReturn;               // 元の状態に戻す発動待ち時間、このフレーム間何も反応がなければ実行する
  16,     // u32 rotationFrame;                    // 回転にかけるフレーム数
  false,  // b32 isCullingReset;                   // true:カリング時に再度有効化される
  20.0f,  // f32 hideRangeDeg;                     // 背後扱いの角度(片翼)
  15,     // u32 moveWaitCount;
  30 * 2.5, // u32 moveKeepCount;
  90, // f32 rotRange;
  1.0f,    // f32 basePositionBuffetDistance;       // 基本位置に戻っているとみなすバッファ 
  1250.0f, // f32 sleepIconDispDistance;            // ZZZアイコンを表示する距離
  {
    250,      // f32 startDistance;                  // 開始距離[cm]
    400.0f,   // s32 stopDistance;                   // 停止距離[cm]
    60.0f,    // f32 stopCount;                      // 停止距離[cm]にこの時間いたら停止
    23.0f,   // f32 runToWalkDistance;          // 走り→歩き開始距離[cm]
    220.0f,   // f32 walkToRunDistance;          // 歩き→走り開始距離[cm]
    300.0f,   // f32 endResetDistance;               // 停止終了距離[cm
    true, // b32 runEnable;                      // 走り有効化フラグ
  },
  // FPF_RANK_UNKNOWN
  {
    {
      100.0f + 5.0f,   // s32 baseDistance;                   // 基準距離 
      100.0f + 20.0f,  // baseBuffetDistance;             // 基準距離(+バッファ) 
      100.0f + 20.0f,  // baseMagnetDistance;             // 吸着を発動 

      23.0f,          // baseWalkDistance;               // 歩くを発動 
			100.0f + 5.0f,  // baseDistanceForModorou
    },
    // FPF_RANK_LIKE
    {
      20.0f + 5.0f,   // s32 baseDistance;                   // 基準距離 
      20.0f + 20.0f,  // baseBuffetDistance;             // 基準距離(+バッファ) 
      20.0f + 20.0f,  // baseMagnetDistance;             // 吸着を発動 
      
			23.0, // baseWalkDistance;               // 歩くを発動 
			20.0f + 5.0f,   // baseDistanceForModorou
    },
    // FPF_RANK_LOVE
    {
      5.0f,   // s32 baseDistance;                   // 基準距離 
      20.0f,  // baseBuffetDistance;             // 基準距離(+バッファ)                                                                                                                                                                         
      20.0f,  // baseMagnetDistance;             // 吸着を発動 

      23.0f, // baseWalkDistance;               // 歩くを発動 
      5.0f, // baseDistanceForModorou
    },
  },

  // struct SOUND
  {
    0.3f, // f32 min_volume;
    1.0f, // f32 max_volume;
    550.0f, // f32 min_length;
    50.0f, // f32 max_lggength;
    gfl2::math::Easing::IN_SINE
  },

};

#if PM_DEBUG
// デバッグ用挙動設定
FieldMoveCodeNpc::DEBUG_SETTING FieldMoveCodeNpc::m_DebugSetting =
{
  false, // b32 isUse;                            // デバッグ設定を使うか？
  false, // b32 invalidRotation;                  // 回転無効化
  false, // b32 invalidInterest;                  // 注目無効化
  false, // b32 invalidReturn;                    // 初期状態に戻る、無効化
  false, // b32 enableLookWide;                   // 注目範囲拡大版
  false, // b32 enableLookShort;                  // 注目範囲縮小版

  false, // b32 isMcnfOverWrite;                  // モーションコンフィグファイルを上書きする
  {
    { -20, 20, -3, 5, 200 },
    { -15, 15, -30, 10, 200 }
  }// DEBUG_INTEREST_LIMIT interestLimit[2];  // 0:Neck 1:Head
};

FieldMoveCodeFureaiTypeA::DEBUG_SETTING FieldMoveCodeFureaiTypeA::m_DebugSetting =
{
	false, // b32 isMoveAreaCollison
  0,     // b32 isNadeNadeTest
	60.0f, // f32 nadenade_distance;
	45.0f, // f32 nadenade_angle;
	0.0f,  // f32 nadenade_distance_x;

  false,
  0,
};

#endif



//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeNpc::FieldMoveCodeNpc()
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodeNpc::~FieldMoveCodeNpc()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理　初期化
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // @fix GFNMcat[5702]:Workの初期化に不備がありWorkが使いまわされた際に意図せぬ挙動になる、関連対処。回転実行カウンタが初期化されておらず、「初期化後の通常更新1回目に、回転を実行する至近距離にいた際に回転が行われない」可能があったのでMemClearを追加。
  gfl2::std::MemClear( pWork, sizeof( Work ) );

  pWork->state = State::READY;
  pWork->nextState = 0;
  pWork->option = 0;
  pWork->count = 0;

  pWork->dirResetWaitCount = 0;
  pWork->isDirResetWait = false;
  pWork->rotEnterWaitCount = 0;

  pWork->frontInterestCount = 0;

  pWork->returnRotationDegY = 0.0f;
  pWork->isTalkEnd = false;
  pWork->dirDirtyFlag = 0;
  
  pWork->hideInterestFlag = 0;
  pWork->hideInterestFirstDir = 0;
  pWork->prevMotionId = -1;

  // pWork->rotationWork.nowFrame = 0;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}

//
// NPC挙動 @note 違いはOptionの初期化だけなので、同じファイルに書いてしまいます
//

// NPC挙動   注目[無] 回転[無] 初期配置戻り[有]
FieldMoveCodeNpcReturn::FieldMoveCodeNpcReturn()
{
}

FieldMoveCodeNpcReturn::~FieldMoveCodeNpcReturn()
{
}

void FieldMoveCodeNpcReturn::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::INVALID_ROTATION;
  pWork->option |= Option::INVALID_INTEREST;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}

// NPC挙動    注目[有] 回転[有] 初期配置戻り[無]
FieldMoveCodeNpcMeet::FieldMoveCodeNpcMeet()
{
}

FieldMoveCodeNpcMeet::~FieldMoveCodeNpcMeet()
{
}

void FieldMoveCodeNpcMeet::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::INVALID_RETURN;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}

// NPC挙動    注目[有] 回転[無] 初期配置戻り[無]
FieldMoveCodeNpcLook::FieldMoveCodeNpcLook()
{
}

FieldMoveCodeNpcLook::~FieldMoveCodeNpcLook()
{
}

void FieldMoveCodeNpcLook::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::INVALID_ROTATION;
  pWork->option |= Option::INVALID_RETURN;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}

// NPC挙動    注目[有] 回転[無] 初期配置戻り[有]
FieldMoveCodeNpcLookReturn::FieldMoveCodeNpcLookReturn()
{
}
FieldMoveCodeNpcLookReturn::~FieldMoveCodeNpcLookReturn()
{
}
void FieldMoveCodeNpcLookReturn::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::INVALID_ROTATION;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}

// NPC挙動    注目[有] 回転[有] 初期配置戻り[無] 注目距離縮小
FieldMoveCodeNpcMeetShort::FieldMoveCodeNpcMeetShort()
{
}

FieldMoveCodeNpcMeetShort::~FieldMoveCodeNpcMeetShort()
{
}

void FieldMoveCodeNpcMeetShort::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::INVALID_RETURN;
  pWork->option |= Option::ENABLE_LOOK_SHORT;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}


// NPC挙動    注目[有] 回転[有] 初期配置戻り[有] 注目距離縮小
FieldMoveCodeNpcMeetShortReturn::FieldMoveCodeNpcMeetShortReturn()
{
}
FieldMoveCodeNpcMeetShortReturn::~FieldMoveCodeNpcMeetShortReturn()
{
}
void FieldMoveCodeNpcMeetShortReturn::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::ENABLE_LOOK_SHORT;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}

// NPC挙動    注目[有] 回転[無] 初期配置戻り[無] 注目距離縮小
FieldMoveCodeNpcLookShort::FieldMoveCodeNpcLookShort()
{
}

FieldMoveCodeNpcLookShort::~FieldMoveCodeNpcLookShort()
{
}

void FieldMoveCodeNpcLookShort::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::INVALID_ROTATION;
  pWork->option |= Option::INVALID_RETURN;
  pWork->option |= Option::ENABLE_LOOK_SHORT;
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}

// NPC挙動    注目[有] 回転[無] 初期配置戻り[有] 注目距離縮小
FieldMoveCodeNpcLookShortReturn::FieldMoveCodeNpcLookShortReturn()
{
}

FieldMoveCodeNpcLookShortReturn::~FieldMoveCodeNpcLookShortReturn()
{
}

void FieldMoveCodeNpcLookShortReturn::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= Option::INVALID_ROTATION;
  pWork->option |= Option::ENABLE_LOOK_SHORT;

  // 初期値として記憶
  pWork->defaultOption = pWork->option;
}


// NPC挙動   (預かり屋 ※育て屋前のNPC)
FieldMoveCodeNpcAzukariya::FieldMoveCodeNpcAzukariya(void)
{

}

FieldMoveCodeNpcAzukariya::~FieldMoveCodeNpcAzukariya()
{

}

void FieldMoveCodeNpcAzukariya::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;

  // @note ここでデフォルトモーションを設定しても、FieldMoveModelActorで上書きされる
}


void FieldMoveCodeFureaiTypeA::InitProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  FieldMoveCodeNpc::InitProcess( pMoveCodeWork );

  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  
  // 初期値として記憶
  pWork->defaultOption = pWork->option;

  pWork->subState = 0;
  pWork->subStateCount = 0;
  pWork->isComEventEndFlag = 0;
  pWork->isComEventEndFlagDiscoverAfter = 0;
  pWork->isIconEndFlag = 0;
  pWork->isFirstMoveEndFlag = 0;
  pWork->moveKeepCount = 0;
  pWork->moveWaitCount = m_Setting.moveWaitCount;
  pWork->runIconWaitCount = 0;
  pWork->runIconShowCount = 0;
	pWork->returnWalkWaitCount = 0;
  pWork->isMoveStart = false;
  pWork->randActionWaitCount = 0;
	pWork->randActionCount = 0;
	pWork->isByeByeEnd = 0;
	pWork->isByeByeClear = 0;
	pWork->isFirstRun = 0;
	pWork->isRunUreshii = 0;
	pWork->playerActionReq = 0;

  pWork->renzokuRotCount = 0;
  pWork->renzokuRotWait = 0;
  pWork->renzokuRotIsLeft = 0;
  pWork->playerInterestReqCount = 0;
  // @note ここでデフォルトモーションを設定しても、FieldMoveModelActorで上書きされる
}

}; //end of namespace MoveModel
}; //end of namespace Field

