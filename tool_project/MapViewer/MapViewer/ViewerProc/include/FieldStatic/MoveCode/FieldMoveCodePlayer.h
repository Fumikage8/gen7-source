#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.h
*  @brief  フィールド用 動作コード プレイヤー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Quaternion.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodePlayer : public FieldMoveCodeBase
{
public:

  // ステートの種類
  enum PlayerMove
  {
    PLAYER_MOVE_WAIT=0,

    PLAYER_MOVE_WALK_SLOW,        // 忍び歩き

    PLAYER_MOVE_WALK_START,       // 通常歩き
    PLAYER_MOVE_WALK_LOOP,
    PLAYER_MOVE_WALK_END,

    PLAYER_MOVE_RUN_START,        // 走り
    PLAYER_MOVE_RUN_LOOP,
    PLAYER_MOVE_RUN_END,

    PLAYER_MOVE_DEBUG_RUN_START,  // デバッグ走り
    PLAYER_MOVE_DEBUG_RUN_LOOP,
    PLAYER_MOVE_DEBUG_RUN_END,
    PLAYER_MOVE_MAX
  };

  // ステートの内容
  struct PlayerMoveSetting
  {
    c16  name[256];
    f32  moveSpeed;     // 移動速度
    f32  motionSpeed;   // モーション再生速度
    s32  motionId;      // 再生モーション
    f32  stickLevel;    // 入力値がこの値以下なら発動対象
  };

  // ステートの遷移等に必要なワーク
  struct Work
  {
    u32                    oldState;
    u32                    oldoldState;
    gfl2::math::Quaternion targetQua;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodePlayer(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodePlayer(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_PLAYER; }
  
  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void) const;

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief  アナログスティックの倒し具合からステートを取得
  *
  *  @param  stick  アナログスティックの倒し具合
  *  @return ステート
  */
  //-----------------------------------------------------------------------------
  u32 GetStateFromStickLevel( f32 stick ) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual u32 MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートが変わったときに呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const;

private:

  // 反転速度
  f32 m_turnSpeed;
  // 補完フレーム
  u32 m_interpFrameForRunToRunStop;
  u32 m_interpFrameForRunToWalk;
  u32 m_interpFrameForWalkToRun;

protected:

  // 各ステートの内容
  PlayerMoveSetting m_PlayerMoveSettingTable[ PLAYER_MOVE_MAX ];

};  // end of class FieldMoveCodePlayer;
  
}; //end of namespace mmodel
}; //end of namespace fld
