//======================================================================
/**
 * @file FieldFinderModelControlOneway.cpp
 * @date 2015/12/08 17:46:22
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// PokeTool
#include "PokeTool/include/PokeModel.h"
#include "PokeTool/include/PokeModelPokeFinderMotion.h"
// System
#include "System/include/GflUse.h"
// FinderModel
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"
// self
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControlOneway.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

static const u32 MAX_SEARCH_CNT = 3;
static const f32 SEARCH_OFFSET_Z = 100.0f;

/**
 *  @class  FinderModelControlOneway
 *  @brief  一通挙動
 */
/**
 *  @brief  コンストラクタ
 */
FinderModelControlOneway::FinderModelControlOneway( const Field::TrialModel::FinderOnewayData *pOnewayData )
: FinderModelControl()
, m_pOnewayData( pOnewayData )
, m_fProgress( 0.0f )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void FinderModelControlOneway::Initialize( FinderModel *parent, gfl2::heap::HeapBase *pHeap )
{
  m_pParent = parent;
  m_pHeap = pHeap;
  m_pPathControl = GFL_NEW( m_pHeap ) gfl2::math::PathController();
  m_eState = STATE_WAIT;

  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();

  // 出現までの待機時間の設定
  u32 life_mini = m_pParent->GetExtPrm1();
  u32 life_maxi = m_pParent->GetExtPrm2();
  u32 diff = life_maxi - life_mini;
  if( diff > 0 )
  {
    m_Delay = System::GflUse::GetPublicRand( diff ) + life_mini;
  }
  else
  {
    m_Delay = life_mini;
  }
  m_Delay *= 30; // 秒からフレームへ
#if PM_DEBUG
  m_debug_delayMax = m_Delay;
#endif

  // パスを作成する
  createPath();
  // 座標
  gfl2::math::Vector3 pos = m_pOnewayData->position[0];
  pokeModel->SetPosition( pos );
  // 方向
  gfl2::math::Vector3 next = m_pOnewayData->position[1];
  gfl2::math::Vector3 dir = next - pos;
  f32 fRotY = atan2f( dir.x, dir.z );
  gfl2::math::Vector3 rot( 0.0f, fRotY, 0.0f );
  pokeModel->SetRotation( rot );
  // 待機モーション再生
  pokeModel->ChangeAnimation( PokeTool::MODEL_ANIME_PF_BT_WAIT_A );
  // 描画
  m_pParent->SetDisp( false );
  m_eCall = CALL_NONE;
}

/**
 *  @brief  破棄
 */
void FinderModelControlOneway::Terminate( void )
{
  // 描画
  m_pParent->SetDisp( false );
  // パス
  m_pPathControl->Finalize();
  GFL_SAFE_DELETE( m_pPathControl );
}

/**
 *  @brief  更新
 */
void FinderModelControlOneway::UpdateBeforeAnimation( void )
{
  switch( m_eState )
  {
  case STATE_WAIT:
    updateWait_Before();
    break;
  case STATE_MOVE:
    updateMove_Before();
    break;
  case STATE_END:
    break;
  }
}
void FinderModelControlOneway::UpdateAfterAnimation( void )
{
  switch( m_eState )
  {
  case STATE_WAIT:
    updateWait_After();
    break;
  case STATE_MOVE:
    updateMove_After();
    break;
  case STATE_END:
    break;
  }

  updateCall();
  m_pParent->UpdateNoAppealVoice();
}

/**
 *  @brief  ポケモンを呼ぶ
 */
void FinderModelControlOneway::RequestPokeCall( void )
{
  if( m_eCall == CALL_NONE && m_eState == STATE_MOVE )
  {
    m_eCall = CALL_REQUEST;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  パス作成
 */
void FinderModelControlOneway::createPath( void )
{
  // 曲線+ループなし設定
  m_pPathControl->Initialize( m_pHeap, m_pOnewayData->position, m_pOnewayData->num, false );
  m_fProgress = 0.0f;
}

/**
 *  @brief  更新：待機
 */
void FinderModelControlOneway::updateWait_Before( void )
{
  if( m_Delay > 0 )
  {
    -- m_Delay;
  }
  if( m_Delay <= 0 ) // @fix GFNMCat[1197]
  {
    // 移動モーションの発行
    PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
    pokeModel->ChangeAnimation( m_pParent->GetMoveAnimNo() );
  }
}
void FinderModelControlOneway::updateWait_After( void )
{
  if( m_Delay <= 0 ) // @fix GFNMCat[1197]
  {
    m_eState = STATE_MOVE;
    // 描画
    m_pParent->SetDisp( true );
  }
}


/**
 *  @brief  更新：移動
 */
void FinderModelControlOneway::updateMove_Before( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();

  // 終了
  if( m_fProgress >= 1.0f )
  {
    changeEnd();
  }
}
void FinderModelControlOneway::updateMove_After( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  gfl2::math::Vector3 pos_old = pokeModel->GetPosition();
  f32 spd = pokeModel->GetWalkSpeed( -1.0f ).z;
  f32 len = m_pPathControl->GetTotalLength();
  f32 add = spd / len;
  m_fProgress += add;
  gfl2::math::Vector3 pos_new = m_pPathControl->GetValue( m_fProgress );
  gfl2::math::Vector3 dir = pos_new - pos_old;

  // 座標
  pokeModel->SetPosition( pos_new );
  // 向き
  f32 fRotY = atan2f( dir.x, dir.z );
  gfl2::math::Vector3 rot( 0.0f, fRotY, 0.0f );
  pokeModel->SetRotation( rot );
}


/**
 *  @brief  変更：終了
 */
void FinderModelControlOneway::changeEnd( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  // 終了状態へ
  m_eState = STATE_END;
  // 待機モーションへ
  pokeModel->ChangeAnimation( PokeTool::MODEL_ANIME_PF_BT_WAIT_A );
  // 描画を切る
  m_pParent->SetDisp( false );
}

/**
 *  @brief  ポケモンを呼ぶ
 */
void FinderModelControlOneway::updateCall( void )
{
  switch( m_eCall )
  {
  case CALL_NONE:
    // 何もしない
    break;
  case CALL_REQUEST:
    // 移動の時だけ実行する
    if( m_eState == STATE_MOVE )
    {
      m_eCall = CALL_EXEC;
      m_pParent->EnableInterestCamera();
      // @fix GFNMCat[1343]:ボイス再生を追加
      u32 voice = m_pParent->GetCallVoice();
      if( voice != 0 )
      {
        m_pParent->PlayVoice( voice );
      }
      m_pParent->SetAppeal(true);
    }
    break;
  case CALL_EXEC:
    // 首向け完了まで待つ
    //if( m_pParent->IsFinishInterest() )
    {
      m_eCall = CALL_WAIT;
      m_nCallWait = 0;
    }
    break;
  case CALL_WAIT:
    ++ m_nCallWait;
    // 指定時間待機
    if( m_nCallWait >= CALL_WAIT_FRAME )
    {
      m_eCall = CALL_RESET;
      m_pParent->DisableInterest();
      m_pParent->SetAppeal(false);
    }
    break;
  case CALL_RESET:
    if( !m_pParent->IsInterest() )
    {
      // 完了
      m_eCall = CALL_NONE;
    }
    break;
  }
}

#if PM_DEBUG
void FinderModelControlOneway::debug_onemore( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  m_eState = STATE_WAIT;
  m_Delay = 2;
  m_fProgress = 0.0f;
  m_pPathControl->Finalize();
  createPath();
  // 座標
  gfl2::math::Vector3 pos = m_pOnewayData->position[0];
  pokeModel->SetPosition( pos );
  // 方向
  gfl2::math::Vector3 next = m_pOnewayData->position[1];
  gfl2::math::Vector3 dir = next - pos;
  f32 fRotY = atan2f( dir.x, dir.z );
  gfl2::math::Vector3 rot( 0.0f, fRotY, 0.0f );
  pokeModel->SetRotation( rot );
  // 待機モーション再生
  pokeModel->ChangeAnimation( PokeTool::MODEL_ANIME_PF_BT_WAIT_A );
  // 描画
  m_pParent->SetDisp( false );
}
u32 FinderModelControlOneway::debug_delay( void )
{
  return m_debug_delayMax;
}

#endif
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)
