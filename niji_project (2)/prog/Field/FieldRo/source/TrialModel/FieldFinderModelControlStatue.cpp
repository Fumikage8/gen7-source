//======================================================================
/**
 * @file FieldFinderModelControlStatue.cpp
 * @date 2015/10/08 14:40:08
 * @author miyachi_soichi
 * @brief 挙動：移動なしファインダーモデル
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
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControlStatue.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

static const u32 MAX_SEARCH_CNT = 3;
static const f32 SEARCH_OFFSET_Z = 100.0f;

/**
 *  @class  FinderModelControlStatue
 *  @brief  移動なし挙動
 */
/**
 *  @brief  コンストラクタ
 */
FinderModelControlStatue::FinderModelControlStatue( const Field::TrialModel::FinderStatueData *pStatueData )
: FinderModelControl()
, m_pStatueData( pStatueData )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void FinderModelControlStatue::Initialize( FinderModel *parent, gfl2::heap::HeapBase *pHeap )
{
  m_pParent = parent;
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  // 乱数で配置位置を決める
  m_Position = System::GflUse::GetPublicRand( m_pStatueData->num );
  // 座標設定
  pokeModel->SetPosition( m_pStatueData->data[m_Position].position );
  // 向き設定
  pokeModel->SetRotation( 0.0f, m_pStatueData->data[m_Position].rotation, 0.0f );
  // モーション設定
  switch( m_pParent->GetExtPrm1() )
  {
  case 0:
    m_eWaitAnim = PokeTool::MODEL_ANIME_PF_KW_WAIT;
    break;
  case 1:
    m_eWaitAnim = PokeTool::MODEL_ANIME_PF_BT_WAIT_A;
    break;
  case 2:
    m_eWaitAnim = PokeTool::MODEL_ANIME_PF_SLEEP_A;
    break;
  default:
    GFL_ASSERT_MSG( 0, "Invalid Motion" );
    m_eWaitAnim = PokeTool::MODEL_ANIME_PF_KW_WAIT;
    break;
  }
  pokeModel->ChangeAnimation( m_eWaitAnim );
  // 描画
  m_pParent->SetDisp( true );

  m_CoolTime = calcCoolTime();
  m_bAppeal = m_pParent->GetExtPrm2() != 0;
  m_eCall = CALL_NONE;
}

/**
 *  @brief  破棄
 */
void FinderModelControlStatue::Terminate( void )
{
  // 描画
  m_pParent->SetDisp( false );
}

/**
 *  @brief  更新
 */
void FinderModelControlStatue::UpdateBeforeAnimation( void )
{
  switch( m_eState )
  {
  case STATE_WAIT:
    updateWait();
    break;
  case STATE_APPEAL:
    updateAppeal();
    break;
  case STATE_CUSTOM:
    updateCustom();
    break;
  }

  updateCall();
}

/**
 *  @brief  ポケモンを呼ぶ
 */
void FinderModelControlStatue::RequestPokeCall( void )
{
  // アピールできること
  if( m_bAppeal && m_eCall == CALL_NONE )
  {
    // 即時実行できない場合もあるので予約方式で
    m_eCall = CALL_REQUEST;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新：待機挙動
 */
void FinderModelControlStatue::updateWait( void )
{
  // アピールチェックを通過していない場合はアピールに遷移させない
  if( !m_bAppeal ){ return; }
  if( !m_pParent->HasAppeal() ){ return; }

  // 首向け実行中でないこと
  if( m_eCall == CALL_NONE )
  {
    // @fix[2307] 最小値0がありうるのでオーバーフロー対策
    if( m_CoolTime > 0 )
    {
      -- m_CoolTime;
    }
    if( m_CoolTime <= 0 )
    {
      changeAppeal();
      m_CoolTime = calcCoolTime();
    }
  }
}

/**
 *  @brief  更新：アピール挙動
 */
void FinderModelControlStatue::updateAppeal( void )
{
  PokeTool::PokeModel*     pokeModel      = m_pParent->GetPokeModel();

  enum
  {
    STEP_APPEAL,
    STEP_AFTER_WAIT,
  };
  switch( m_nStep )
  {
  case STEP_APPEAL:
    {
      m_nStep += 1;
      PokeTool::MODEL_ANIME_POKE_FINDER anim = static_cast<PokeTool::MODEL_ANIME_POKE_FINDER>( m_nAppealAnimNo );
      pokeModel->ChangeAnimation( anim );
      m_pParent->SetAppeal( true );
    }
    break;
  case STEP_AFTER_WAIT:
    if( pokeModel->IsAnimationLastFrame() )
    {
      m_eState = STATE_WAIT;
      m_nStep += 1;
      pokeModel->ChangeAnimation( m_pParent->GetWaitAnimNo( m_nAppealAnimNo ) );
      m_pParent->SetAppeal( false );
    }
    break;
  }
}

/**
 *  @brief  更新：カスタム挙動
 */
void FinderModelControlStatue::updateCustom( void )
{
  switch( m_nAppealAnimNo )
  {
  case FINDER_APPEAL_CUSTOM_DROWSE:
    updateCustomDrowse();
    break;
  case FINDER_APPEAL_CUSTOM_CAMERA:
    updateCustomCamera();
    break;
  case FINDER_APPEAL_CUSTOM_SEARCH:
    updateCustomSearch();
    break;
  }
}

/**
 *  @brief  アピール抽選
 */
void FinderModelControlStatue::changeAppeal( void )
{
  m_pParent->GetAppealAction( m_nAppealAnimNo );
  if( m_nAppealAnimNo == FINDER_ANIM_ERROR )
  {
    // State移行しない＝アピールをなかったことに
  }
  else if( m_nAppealAnimNo >= FINDER_APPEAL_CUSTOM_START )
  {
    m_eState = STATE_CUSTOM;
    m_nStep = 0;
  }
  else
  {
    m_eState = STATE_APPEAL;
    m_nStep = 0;
  }
}

/**
 *  @brief  カスタムアピール：うたたね
 */
void FinderModelControlStatue::updateCustomDrowse( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();

  enum
  {
    STEP_DROWSE_A,
    STEP_DROWSE_A_WAIT,
    STEP_DROWSE_B = 30 + STEP_DROWSE_A_WAIT,
    STEP_DROWSE_C,
  };
  switch( m_nStep )
  {
  case STEP_DROWSE_A:
    {
      pokeModel->ChangeAnimation( PokeTool::MODEL_ANIME_PF_DROWSE_A );
      ++ m_nStep;
    }
    break;
  case STEP_DROWSE_A_WAIT:
    if( pokeModel->IsAnimationLastFrame() )
    {
      pokeModel->ChangeAnimation( PokeTool::MODEL_ANIME_PF_DROWSE_B );
      ++ m_nStep;
    }
    break;
  case STEP_DROWSE_B:
    {
      pokeModel->ChangeAnimation( PokeTool::MODEL_ANIME_PF_DROWSE_C );
      ++ m_nStep;
    }
    break;
  case STEP_DROWSE_C:
    if( pokeModel->IsAnimationLastFrame() )
    {
      pokeModel->ChangeAnimation( m_pParent->GetWaitAnimNo( PokeTool::MODEL_ANIME_PF_DROWSE_C ) );
      m_pParent->SetAppeal( false );
      m_eState = STATE_WAIT;
      ++ m_nStep;
    }
    break;
  default:
    {
      ++ m_nStep;
    }
    break;
  }
}

/**
 *  @brief  カスタムアピール：カメラ注視
 */
void FinderModelControlStatue::updateCustomCamera( void )
{
  enum
  {
    STEP_SET,
    STEP_RESET = 60 + STEP_SET,
    STEP_RESET_WAIT,
  };

  switch( m_nStep )
  {
  case STEP_SET:
    m_pParent->EnableInterestCamera();
    ++ m_nStep;
    break;
  case STEP_RESET:
    m_pParent->DisableInterest();
    ++ m_nStep;
    break;
  case STEP_RESET_WAIT:
    if( m_pParent->IsInterest() )
    {
      ++ m_nStep;
      m_eState = STATE_WAIT;
    }
    break;
  default:
    ++ m_nStep;
    break;
  }
}

/**
 *  @brief  カスタムアピール：キョロキョロ
 */
void FinderModelControlStatue::updateCustomSearch( void )
{
  enum
  {
    STEP_INIT,
    STEP_START,
    STEP_INC,
    STEP_INC_WAIT,
    STEP_DEC,
    STEP_RESET_WAIT,
    STEP_END,
  };

  const FinderPokeData *pData = m_pParent->GetPokeData();

  switch( m_nStep )
  {
  case STEP_INIT:
    if( pData )
    {
      m_searchRad = 0.0f;
      calcSearchPosition();
      m_pParent->EnableInterestSearch();
      m_pParent->SetInterestSpeedSearch( gfl2::math::ConvDegToRad( pData->deg ) );
      ++ m_nStep;
    }
    else
    {
      m_nStep = STEP_END;
      break;
    }
    // no break;
  case STEP_START:
    {
      s32 dst = ( m_SearchCnt % 2 ) * 2 - 1;
      m_searchLife = pData->frame;
      m_searchRad = 0.0f;
      m_searchRad_Max = gfl2::math::ConvDegToRad( pData->deg ) * dst;
      m_searchRad_add = ( m_searchRad_Max - m_searchRad ) / m_searchLife;
      ++ m_SearchCnt;

      calcSearchPosition();
      m_pParent->SetInterestPositionSearch( m_vSearchPos );
      ++ m_nStep;
    }
    break;
  case STEP_INC:
    {
      if( m_searchLife > 0 )
      {
        -- m_searchLife;
        m_searchRad += m_searchRad_add;
      }
      else
      {
        m_searchRad = m_searchRad_Max;
        ++ m_nStep;
        m_nWait = pData->wait;
      }

      calcSearchPosition();
      m_pParent->SetInterestPositionSearch( m_vSearchPos );
    }
    break;
  case STEP_INC_WAIT:
    if( m_nWait > 0 )
    {
      -- m_nWait;
    }
    else
    {
      if( m_SearchCnt < MAX_SEARCH_CNT )
      {
        m_searchLife = pData->frame;
        m_searchRad_Max = 0.0f;
        m_searchRad_add = ( m_searchRad_Max - m_searchRad ) / m_searchLife;
        ++ m_nStep;
      }
      else
      {
        m_pParent->DisableInterest( pData->frame );
        m_nStep = STEP_RESET_WAIT;
      }
    }
    break;
  case STEP_DEC:
    {
      if( m_searchLife > 0 )
      {
        -- m_searchLife;
        m_searchRad += m_searchRad_add;
      }
      else
      {
        m_searchRad = m_searchRad_Max;
        m_nStep = STEP_START;
      }

      calcSearchPosition();
      m_pParent->SetInterestPositionSearch( m_vSearchPos );
    }
    break;
  case STEP_RESET_WAIT:
    if( !m_pParent->IsInterest() )
    {
      ++ m_nStep;
    }
    break;
  case STEP_END:
    m_SearchCnt = 0;
    m_eState = STATE_WAIT;
    break;
  }
}

void FinderModelControlStatue::calcSearchPosition( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();

  f32 radY = m_searchRad;

  gfl2::math::Vector3 vec_pos = pokeModel->GetPosition();
  gfl2::math::Vector3 vec_rot = pokeModel->GetRotation();
  radY = gfl2::math::NormalizeRadianAngle( vec_rot.y + radY );
  gfl2::math::Vector3 vec_off( 0.0f, 0.0f, SEARCH_OFFSET_Z );
  gfl2::math::Matrix34 mtx_rot = gfl2::math::Matrix34::GetRotationZYX( 0.0f, radY, 0.0f );
  m_vSearchPos = mtx_rot * vec_off + vec_pos;
}

u32 FinderModelControlStatue::calcCoolTime( void )
{
  u32 value = m_pStatueData->data[m_Position].cool_max - m_pStatueData->data[m_Position].cool_min;
  if( value > 0 )
  {
    return System::GflUse::GetPublicRand( value ) + m_pStatueData->data[m_Position].cool_min;
  }
  return m_pStatueData->data[m_Position].cool_min;
}

/**
 *  @brief  ポケモンを呼ぶ
 */
void FinderModelControlStatue::updateCall( void )
{
  switch( m_eCall )
  {
  case CALL_NONE:
    // 何もしない
    break;
  case CALL_REQUEST:
    // 移動の時だけ実行する
    if( m_eState == STATE_WAIT )
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
void FinderModelControlStatue::debug_SetPosition( u32 idx )
{
  // あふれ
  if( idx >= m_pStatueData->num ){ return; }

  m_Position = idx;
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  // 座標設定
  pokeModel->SetPosition( m_pStatueData->data[m_Position].position );
  // 向き設定
  pokeModel->SetRotation( 0.0f, m_pStatueData->data[m_Position].rotation, 0.0f );
}

#endif
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)
