//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.cpp
*  @brief  フィールド用 動作コード プレイヤー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "niji_conv_header/field/chara/pc0002_00_not_parts.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel.h" //for FieldMoveCodeWork
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel_define.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCode_define.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeManager.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodePlayer.h"
///#include "../../include/ActionCommand/FieldActionCommandManager.h"
///#include "../../include/ActionCommand/FieldActionCommand_define.h"
#include "GameSystem/include/GameManager.h"

// RTTIデバッグシステム
///#include "../../include/RttiDebug/RttiEditor.h"
///#include "../../include/RttiDebug/RttiManager.h"

///#include "Field/FieldRo/include/Debug/FieldDebugTypes.h"  // for TEMP_CODE_IKUTA_150423

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
extern f32 g_CameraYaw;
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );


#include <model/include/gfl2_CharaModelFactory.h>


namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveCodePlayer::FieldMoveCodePlayer(void) 
: m_turnSpeed(0.3f)
, m_interpFrameForRunToRunStop(5)
, m_interpFrameForRunToWalk(5)
, m_interpFrameForWalkToRun(5)
{
  // 待機
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].motionId     = PC0002_00_FI001_WAIT01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WAIT            ].stickLevel   = 0.0f;
  }

  // 忍び歩き(無効)
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_SLOW       ].stickLevel   = -1.0f;
  }

  // 歩き
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_START      ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].motionId     = PC0002_00_FI010_WALK01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_LOOP       ].stickLevel   = 0.4f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_WALK_END        ].stickLevel   = -1.0f;
  }

  // 走り
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_START       ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].motionId     = PC0002_00_FI012_RUN01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_LOOP        ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].motionId     = PC0002_00_FI014_RUNSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_RUN_END         ].stickLevel   =-1.0f;
  }

  // デバッグ走り
  {
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].moveSpeed    = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].motionSpeed  = -1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].motionId     = -1;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_START ].stickLevel   = -1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].moveSpeed    = 5.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].motionSpeed  = 5.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].motionId     = PC0002_00_FI012_RUN01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_LOOP  ].stickLevel   =-1.0f;

    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].moveSpeed    = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].motionSpeed  = 1.0f;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].motionId     = PC0002_00_FI014_RUNSTOP01;
    m_PlayerMoveSettingTable[ PLAYER_MOVE_DEBUG_RUN_END   ].stickLevel   =-1.0f;
  }
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
  pMoveCodeWork->moveCodeId = FIELD_MOVE_CODE_PLAYER;

  Work* pWork         = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->oldState     = PLAYER_MOVE_MAX;
  pWork->oldoldState  = PLAYER_MOVE_MAX;
  pWork->targetQua    = gfl2::math::Quaternion(0.0f,0.0f,0.0f,0.0f);

#if USE_RTTI_DEBUG
  RegistRtti();
  FieldMoveCodePlayer* pThis = const_cast<FieldMoveCodePlayer*>(this);
  gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( L"プレイヤーコントロール" , &typeid( FieldMoveCodePlayer ) , pThis );
#endif

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
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // コントローラーの入力を取得
  GameSystem::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::VectorDevice*   pVecDevice     = device_manager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  f32                       stickX         = pVecDevice->GetX();
  f32                       stickY         = pVecDevice->GetY();
  f32                       stickLength    = sqrt( stickX*stickX + stickY*stickY );

  // スティックの倒した量をみてステートを選定
  u32 state = GetStateFromStickLevel( stickLength );

  // ステートを状況をみて上書き
  state = MainProcessSubOverrideState( pMoveCodeWork , state , pWork->oldState , pWork->oldoldState );

  // スティックの倒した方向から目標姿勢の更新
  if( stickLength > 0.0f )
  {
    // スティックの倒した方向を取得
    gfl2::math::Vector raw_move       ( stickX, 0.0f, stickY, 0.0f );
    gfl2::math::Vector normal_raw_move( raw_move.Normalize3() );
    float dir = gfl2::math::NormalizeRadianAngle( atan2( normal_raw_move.z, normal_raw_move.x ) + 0.5f * PI );

///#ifdef TEMP_CODE_IKUTA_150423
    // 仮カメラのY軸回転を元に自機操作のスティック入力値を加工する処理
    dir = gfl2::math::NormalizeRadianAngle( dir + Field::Debug::g_CameraYaw );
///#endif //@saito_del

    // 目標姿勢の更新
    gfl2::math::Quaternion qua; 
    qua.RadianYToQuaternion(dir);
    pWork->targetQua = qua;
  }

  // 目標姿勢の不正チェック
  {
    gfl2::math::Vector temp(pWork->targetQua.x,pWork->targetQua.y,pWork->targetQua.z,pWork->targetQua.w);
    if( temp.Length() <= 0.0f )
    { 
      pWork->targetQua = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
    }
  }

  // レバー入力による動作モデルの回転
  gfl2::math::Quaternion nowQua( pCharaModelInstance->GetRotationQuat() );
  nowQua.Slerp( pWork->targetQua , gfl2::math::Clamp(m_turnSpeed,0.0f,1.0f) );
  pCharaModelInstance->SetRotationQuat( nowQua );

  // Originジョイントによる移動ベクトルを変換するスケール/クォータニオンを設定する
  pMoveCodeWork->pModel->SetCustomValueForOrigin( m_PlayerMoveSettingTable[state].moveSpeed , pWork->targetQua );

  // ステートの変更
  if ( pWork->oldState != state )
  {
    MainProcessSubChangeState(pMoveCodeWork,state,pWork->oldState,pWork->oldoldState);
  }

  pWork->oldoldState = pWork->oldState;
  pWork->oldState    = state;
}

//----------------------------------------------------------------------------
/**
*  @brief  アナログスティックの倒し具合からステートを取得
*
*  @param  stick  アナログスティックの倒し具合
*  @return ステート
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodePlayer::GetStateFromStickLevel( f32 stick ) const
{
  for( s32 i=PLAYER_MOVE_MAX-1 ; i>=0 ; i-- )
  {
    if( m_PlayerMoveSettingTable[i].stickLevel >= 0.0f
     && m_PlayerMoveSettingTable[i].stickLevel <= stick )
    {
      return i;
    }
  }
  return PLAYER_MOVE_WAIT;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodePlayer::MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const
{
  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // ボタン入力状況を取得
  GameSystem::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
  gfl2::ui::DeviceManager*  device_manager = game_manager->GetUiDeviceManager();
  gfl2::ui::Button*         pButton        = device_manager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD); 

  s32 state = nowState;

  // "待機以外→走り開始 or 走りループ"
  if( state != PLAYER_MOVE_WAIT )
  {
    // 通常走りを起動させる(走り始めが無いものは走りループ状態から)
    if( pButton->IsHold( gfl2::ui::BUTTON_B ) )
    {
      state = PLAYER_MOVE_RUN_START;
      if( m_PlayerMoveSettingTable[state].stickLevel < 0.0f )
      {
        state = PLAYER_MOVE_RUN_LOOP;
      }
    }
    // デバッグ走りを起動させる(走り始めが無いものはデバッグ走りループ状態から)
    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      state = PLAYER_MOVE_DEBUG_RUN_START;
      if( m_PlayerMoveSettingTable[state].stickLevel < 0.0f )
      {
        state = PLAYER_MOVE_DEBUG_RUN_LOOP;
      }
    }
  }

  // 前々回まで走っており現在/前回は"待機状態"のときは"走り終了"ステートに上書き
  // note: スティックを走りながら逆方向に入れた際、待機状態を挟まないようにするため前々回もみています。
  {
    if(  ( state       == PLAYER_MOVE_WAIT )
      && ( oldState    == PLAYER_MOVE_WAIT )
      && ( oldoldState == PLAYER_MOVE_RUN_START || oldoldState == PLAYER_MOVE_RUN_LOOP) )
    {
      state = PLAYER_MOVE_RUN_END;
    }
    if(  ( state       == PLAYER_MOVE_WAIT )
      && ( oldState    == PLAYER_MOVE_WAIT )
      && ( oldoldState == PLAYER_MOVE_DEBUG_RUN_START || oldoldState == PLAYER_MOVE_DEBUG_RUN_LOOP ) )
    {
      state = PLAYER_MOVE_DEBUG_RUN_END;
    }
  }

  // "走り始め→走りループ" になるようにステートを上書き
  {
    // 単発再生が終われば"走り始め"ステートが終了
    if( oldState == PLAYER_MOVE_RUN_START )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_RUN_LOOP;
      }
    }
    // 単発再生が終われば"走り始め"ステートが終了
    if( oldState == PLAYER_MOVE_DEBUG_RUN_START )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_DEBUG_RUN_LOOP;
      }
    }
  }

  // "走り終わり→待機" になるようにステートを上書き
  {
    // 単発再生が終われば"走り終わり"ステートが終了
    if( oldState == PLAYER_MOVE_RUN_END )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_RUN_END;
      }
    }
    // 単発再生が終われば"走り終わり"ステートが終了
    if( oldState == PLAYER_MOVE_DEBUG_RUN_END )
    {
      if( !pCharaModelInstance->IsAnimationLastFrame() )
      {
        state = PLAYER_MOVE_DEBUG_RUN_END;
      }
    }
  }

  // Lキーが押されている際は、自機操作をスキップする処理
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    state = PLAYER_MOVE_WAIT;
  }

  return state;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メインのサブ関数(ステートが変わったときに呼ばれる)
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodePlayer::MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const
{
  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // ステートにあったデフォルトモーションを取得
  s32 next_anime_id = m_PlayerMoveSettingTable[nowState].motionId;

  // 待機アニメーションだけは外部から上書き可能
  if( nowState == PLAYER_MOVE_WAIT )
  {
    next_anime_id = (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() );
  }

  // @todo
  u32 interpFrame = 5;
  if     ( nowState == PLAYER_MOVE_RUN_END )                                        { interpFrame=m_interpFrameForRunToRunStop; }
  else if( nowState == PLAYER_MOVE_WALK_LOOP && oldState == PLAYER_MOVE_RUN_LOOP  ) { interpFrame=m_interpFrameForRunToWalk;    }
  else if( nowState == PLAYER_MOVE_RUN_LOOP  && oldState == PLAYER_MOVE_WALK_LOOP ) { interpFrame=m_interpFrameForWalkToRun;    }

  // モーションの変更
  pCharaModelInstance->ChangeAnimationSmooth(next_anime_id,interpFrame);
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
    RTTI_PARAM_REGIST_ATTR( FieldMoveCodePlayer , m_turnSpeed                  , L"反転速度" , gfl2::rttidebug::RttiParamManager::ATTR_ADD_SMALL );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpFrameForRunToRunStop , L"走り->走り終わりの補完フレーム" );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpFrameForRunToWalk    , L"走り->歩き　　　の補完フレーム" );
    RTTI_PARAM_REGIST     ( FieldMoveCodePlayer , m_interpFrameForWalkToRun    , L"歩き->走り　　　の補完フレーム" );
  RTTI_END_REGIST( FieldMoveCodePlayer );

#endif
}

}; //end of namespace MoveModel
}; //end of namespace Field
