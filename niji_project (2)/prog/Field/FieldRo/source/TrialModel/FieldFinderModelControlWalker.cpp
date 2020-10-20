//======================================================================
/**
 * @file FieldFinderModelControlWalker.cpp
 * @date 2015/10/08 15:31:18
 * @author miyachi_soichi
 * @brief 挙動：移動ありファインダーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <debug/include/gfl2_Assert.h>
// PokeTool
#include "PokeTool/include/PokeModel.h"
#include "PokeTool/include/PokeModelPokeFinderMotion.h"
// system
#include "System/include/GflUse.h"
// FinderModel
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"
// self
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControlWalker.h"
#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

static const u32 MAX_SEARCH_CNT = 3;
static const f32 SEARCH_OFFSET_Z = 100.0f;


/**
 *  @class  FinderModelControlWalker
 *  @brief  移動なし挙動
 */
/**
 *  @brief  コンストラクタ
 */
FinderModelControlWalker::FinderModelControlWalker( const FinderPathData *pPathData )
: FinderModelControl()
, m_pHeap( NULL )
, m_pPathControl( NULL )
, m_pPathData( pPathData )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void FinderModelControlWalker::Initialize( FinderModel *parent, gfl2::heap::HeapBase *pHeap )
{
  m_pHeap = pHeap;
  m_pPathControl = GFL_NEW( pHeap ) gfl2::math::PathController();

  m_eState = STATE_WALK;
  for( s32 i = 0; i < TARGET_MAX; ++ i )
  {
    m_nTarget[ i ] = -1;
  }
  m_fSpdRatio = 1.0f;
  m_bAppeal = false;
  m_pParent = parent;

  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  calcNextNext();
  calcNextNext();
  calcNextNext();

  // パスを作る
  createPath();

  s32 curt = m_nTarget[ TARGET_CURRENT ];
  // 現在位置の設定
  pokeModel->SetPosition( m_pPathData->route[curt].pos );
  // モーション設定
  pokeModel->ChangeAnimation( m_pParent->GetMoveAnimNo() );
  // 描画
  m_pParent->SetDisp( true );
  m_eCall = CALL_NONE;
}

/**
 *  @brief  破棄
 */
void FinderModelControlWalker::Terminate( void )
{
  // 描画
  m_pParent->SetDisp( false );
  // パス破棄
  m_pPathControl->Finalize();
  GFL_SAFE_DELETE( m_pPathControl );
}

/**
 *  @brief  更新
 */
void FinderModelControlWalker::UpdateBeforeAnimation( void )
{
  switch( m_eState )
  {
  case STATE_WALK:
    {
      updateMoveTypePath_Before();
    }
    break;
  case STATE_APPEAL:
    {
      updateAppeal();
    }
    break;
  case STATE_CUSTOM:
    {
      updateCustom();
    }
    break;
  }
}
void FinderModelControlWalker::UpdateAfterAnimation( void )
{
  switch( m_eState )
  {
  case STATE_WALK:
    {
      updateMoveTypePath_After();
    }
    break;
  }

  updateCall();
  m_pParent->UpdateNoAppealVoice();
}

/**
 *  @brief  ポケモンを呼ぶ
 */
void FinderModelControlWalker::RequestPokeCall( void )
{
  if( m_eCall == CALL_NONE )
  {
    m_eCall = CALL_REQUEST;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  ネクネクを設定する
 */
void FinderModelControlWalker::calcNextNext( void )
{
  // 押し出し
  m_nTarget[TARGET_CURRENT] = m_nTarget[TARGET_NEXT];
  m_nTarget[TARGET_NEXT] = m_nTarget[TARGET_NEXTNEXT];
  // ネクネクを抽選
  s32 target_next = m_nTarget[TARGET_NEXT];
  if( target_next == -1 )
  {
    // -1の場合：ぜんぶから
    m_nTarget[TARGET_NEXTNEXT] = System::GflUse::GetPublicRand( m_pPathData->pos_num );
  }
  else
  {
    // 設定されている場合
    s32 num = 0;
    s32 target_list[ FINDER_CONECTION_MAX ] = { -1, -1, -1, -1, -1 };
#if 0
    for( s32 i = 0; i < FINDER_CONECTION_MAX; ++ i )
    {
      s32 dst = m_pPathData->route[ target_next ].next_idx[ i ];
      if( dst == -1 ){ continue; }
      if( dst == m_nTarget[TARGET_CURRENT] ){ continue; }

      target_list[ num ] = dst;
      ++ num;
    }
#else
    {
      s32 dst = 0;
      // かっこ悪いので治したいがコンパイラ最適化でループが変になるのでその対策
      dst = m_pPathData->route[ target_next ].next_idx[ 0 ];
      if( dst != -1 && dst != m_nTarget[TARGET_CURRENT] )
      {
        target_list[ num ] = dst;
        ++ num;
      }
      dst = m_pPathData->route[ target_next ].next_idx[ 1 ];
      if( dst != -1 && dst != m_nTarget[TARGET_CURRENT] )
      {
        target_list[ num ] = dst;
        ++ num;
      }
      dst = m_pPathData->route[ target_next ].next_idx[ 2 ];
      if( dst != -1 && dst != m_nTarget[TARGET_CURRENT] )
      {
        target_list[ num ] = dst;
        ++ num;
      }
      dst = m_pPathData->route[ target_next ].next_idx[ 3 ];
      if( dst != -1 && dst != m_nTarget[TARGET_CURRENT] )
      {
        target_list[ num ] = dst;
        ++ num;
      }
      dst = m_pPathData->route[ target_next ].next_idx[ 4 ];
      if( dst != -1 && dst != m_nTarget[TARGET_CURRENT] )
      {
        target_list[ num ] = dst;
        ++ num;
      }
    }
#endif
    GFL_ASSERT_STOP( num > 0 );
    s32 result = System::GflUse::GetPublicRand( num );
    m_nTarget[TARGET_NEXTNEXT] = target_list[ result ];
  }
}

/**
 *  @brief  パスを生成
 */
void FinderModelControlWalker::createPath( void )
{
  // 内部でFinalizeを行っているので明示的に実行する必要はない

  // パス用座標リストを作成
  for( s32 i = 0; i < TARGET_MAX; ++ i )
  {
    s32 idx = m_nTarget[i];
    if( idx == -1 )
    {
      GFL_ASSERT_MSG( false, "FinderModelControlWalker::createPath error idx" );
      return;
    }
    m_PathPos[i] = m_pPathData->route[idx].pos;
  }

  // 曲線+るーぷなし
  m_pPathControl->Initialize( m_pHeap, m_PathPos, TARGET_MAX, false );
  m_fProgress = 0.0f;
}

/**
 *  @brief  移動：パス使用
 */
void FinderModelControlWalker::updateMoveTypePath_Before( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();

  if( m_bAppeal && pokeModel->IsAnimationLastFrame() )
  {
    // アピールへ
    changeAppeal();
  }
}
void FinderModelControlWalker::updateMoveTypePath_After( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  s32 next = m_nTarget[ TARGET_NEXT ];
  gfl2::math::Vector3 pos_chara = pokeModel->GetPosition();
  gfl2::math::Vector3 pos_target = m_pPathData->route[ next ].pos;
  gfl2::math::Vector3 diff = pos_target - pos_chara;
  f32 spd = pokeModel->GetWalkSpeed( -1.0f ).z;
  f32 length = m_pPathControl->GetTotalLength();
  f32 addProg = spd / length;
  m_fProgress += addProg;
  gfl2::math::Vector3 pos_next = m_pPathControl->GetValue( m_fProgress );
  gfl2::math::Vector3 dir = pos_next - pos_chara;
  checkAppealPoint( pos_chara, pos_next );

  if( diff.Length() <= spd )
  {
    // 到着
    pokeModel->SetPosition( pos_target );
    calcNextNext();
    createPath();
  }
  else
  {
    pokeModel->SetPosition( pos_next );
  }

  // rotate
  {
    f32 fRotY = atan2f( dir.x, dir.z );
    gfl2::math::Vector3 rot( 0.0f, fRotY, 0.0f );
    pokeModel->SetRotation( rot );
  }
}

/**
 *  @brief  アピール：モーション
 */
void FinderModelControlWalker::updateAppeal( void )
{
  PokeTool::PokeModel*     pokeModel      = m_pParent->GetPokeModel();

  enum
  {
    STEP_BEFOR_WAIT,
    STEP_APPEAL,
    STEP_AFTER_WAIT,
    STEP_MOVE,
    STEP_CONNECT_MOVE,
  };
  switch( m_nStep )
  {
  case STEP_BEFOR_WAIT:
    {
      m_nStep += 1;
      pokeModel->ChangeAnimation( m_pParent->GetWaitAnimNo( m_nAppealAnimNo ) );
    }
    break;
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
      m_nStep += 1;
      pokeModel->ChangeAnimation( m_pParent->GetWaitAnimNo( m_nAppealAnimNo ) );
      m_pParent->SetAppeal( false );
    }
    break;
  case STEP_MOVE:
    {
      PokeTool::MODEL_ANIME_POKE_FINDER moveAnim = m_pParent->GetMoveAnimNo();
      PokeTool::MODEL_ANIME_POKE_FINDER moveConnect;
      if( moveAnim == PokeTool::MODEL_ANIME_PF_WALK01 )
      {
        // 待機->歩き
        moveConnect = PokeTool::MODEL_ANIME_PF_WAIT_WALK01;
      }
      else
      {
        // 待機->走り
        moveConnect = PokeTool::MODEL_ANIME_PF_WAIT_RUN01;
      }

      if( pokeModel->IsAvailableAnimation( moveConnect ) )
      {
        m_nStep += 1;
        pokeModel->ChangeAnimation( moveConnect );
      }
      else
      {
        // 接続がない
        m_eState = STATE_WALK;
        pokeModel->ChangeAnimation( m_pParent->GetMoveAnimNo() );
      }
    }
    break;
  case STEP_CONNECT_MOVE:
    if( pokeModel->IsAnimationLastFrame() )
    {
      m_eState = STATE_WALK;
      pokeModel->ChangeAnimation( m_pParent->GetMoveAnimNo() );
    }
    break;
  }
}

/**
 *  @brief  アピール：カスタム
 */
void FinderModelControlWalker::updateCustom( void )
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
 *  @brief  アピール地点の計算
 */
bool FinderModelControlWalker::checkAppealPoint( const gfl2::math::Vector3 &old_pos, const gfl2::math::Vector3 &new_pos )
{
  // アピールを所有しないキャラクタなので抽選しない
  if( !m_pParent->HasAppeal() ){ return false; }
  // ポケモンを呼ぶ予約中
  if( m_eCall != CALL_NONE ){ return false; }

  // 現在のルート
  s32 start = m_nTarget[TARGET_CURRENT];
  s32 end = m_nTarget[TARGET_NEXT];
  gfl2::math::Vector3 vStart = m_pPathData->route[start].pos;
  gfl2::math::Vector3 vEnd = m_pPathData->route[end].pos;

  f32 fTotalLength, fOldProgress, fNewProgress;
  {
    gfl2::math::Vector3 diff;
    // 総距離
    diff = vEnd - vStart;
    fTotalLength = diff.Length();
    // 旧割合
    diff = old_pos - vStart;
    fOldProgress = diff.Length() / fTotalLength;
    // 新割合
    diff = new_pos - vStart;
    fNewProgress = diff.Length() / fTotalLength;
  }

  // 該当するアピール地点があるか検索
  s32 max = static_cast<s32>( FINDER_APPEAL_MAX );
  for( s32 i = 0; i < max; ++ i )
  {
    const FinderAppealData &data = m_pPathData->appeal[i];
    s32 dst_start = static_cast<s32>( data.start );
    s32 dst_end = static_cast<s32>( data.end );
    // 同一ルート上にあること
    if( dst_start != start ){ continue; }
    if( dst_end != end ){ continue; }
    // 割合
    if( data.progress <= fOldProgress ){ continue; }
    if( data.progress > fNewProgress ){ continue; }
    // 確率
    f32 rand = static_cast<f32>( System::GflUse::GetPublicRand( 100 ) );
    f32 ratio = data.ratio;
#if PM_DEBUG
    if( Field::Debug::DebugTypes::s_DebugFinderParam.nAppealRatio >= 0 )
    {
      ratio = static_cast<f32>( Field::Debug::DebugTypes::s_DebugFinderParam.nAppealRatio );
    }
#endif
    if( ratio <= rand ){ continue; }

    // 条件に一致する物件を見つけた
    m_bAppeal = true;

    return true;
  }

  // アピール地点を見つけられなかった
  return false;
}

/**
 *  @brief  アピール挙動へ
 */
void FinderModelControlWalker::changeAppeal( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();

  m_pParent->GetAppealAction( m_nAppealAnimNo );

  if( m_nAppealAnimNo == FINDER_ANIM_ERROR )
  {
    // State移行しない＝アピールをなかったことに
  }
  else if( m_nAppealAnimNo >= FINDER_APPEAL_CUSTOM_START )
  {
    m_eState = STATE_CUSTOM;
    m_nStep = 0;
    m_bAppeal = false;
  }
  else
  {
    m_eState = STATE_APPEAL;
    m_nStep = 0;
    m_bAppeal = false;
  }
}

void FinderModelControlWalker::updateCustomDrowse( void )
{
  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  PokeTool::MODEL_ANIME_POKE_FINDER anim;

  enum
  {
    STEP_WAIT,
    STEP_DROWSE_A,
    STEP_DROWSE_A_WAIT,
    STEP_DROWSE_B = 30 + STEP_DROWSE_A_WAIT,
    STEP_DROWSE_C,
    STEP_MOVE,
  };
  switch( m_nStep )
  {
  case STEP_WAIT:
    {
      pokeModel->ChangeAnimation( m_pParent->GetWaitAnimNo( PokeTool::MODEL_ANIME_PF_DROWSE_A ) );
      m_pParent->SetAppeal( true );
      ++ m_nStep;
    }
    break;
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
      ++ m_nStep;
    }
    break;
  case STEP_MOVE:
    {
      m_eState = STATE_WALK;
      pokeModel->ChangeAnimation( m_pParent->GetMoveAnimNo() );
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
void FinderModelControlWalker::updateCustomCamera( void )
{
  enum
  {
    STEP_WAIT,
    STEP_SET,
    STEP_RESET = 60 + STEP_SET,
    STEP_RESET_WAIT,
  };

  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();

  switch( m_nStep )
  {
  case STEP_WAIT:
    pokeModel->ChangeAnimation( m_pParent->GetWaitAnimNo( PokeTool::MODEL_ANIME_PF_DROWSE_A ) );
    ++ m_nStep;
    break;
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
      m_eState = STATE_WALK;
      pokeModel->ChangeAnimation( m_pParent->GetMoveAnimNo() );
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
void FinderModelControlWalker::updateCustomSearch( void )
{
  enum
  {
    STEP_WAIT,
    STEP_INIT,
    STEP_START,
    STEP_INC,
    STEP_INC_WAIT,
    STEP_DEC,
    STEP_RESET_WAIT,
    STEP_END,
  };

  PokeTool::PokeModel *pokeModel = m_pParent->GetPokeModel();
  const FinderPokeData *pData = m_pParent->GetPokeData();

  switch( m_nStep )
  {
  case STEP_WAIT:
    pokeModel->ChangeAnimation( m_pParent->GetWaitAnimNo( PokeTool::MODEL_ANIME_PF_DROWSE_A ) );
    ++ m_nStep;
    break;
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
    pokeModel->ChangeAnimation( m_pParent->GetMoveAnimNo() );
    m_SearchCnt = 0;
    m_eState = STATE_WALK;
    break;
  }
}

/**
 *  @brief  キョロキョロの位置計算
 */
void FinderModelControlWalker::calcSearchPosition( void )
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

/**
 *  @brief  ポケモンを呼ぶ
 */
void FinderModelControlWalker::updateCall( void )
{
  switch( m_eCall )
  {
  case CALL_NONE:
    // 何もしない
    break;
  case CALL_REQUEST:
    // 移動の時だけ実行する＆アピール予約されていないこと
    if( m_eState == STATE_WALK && !m_bAppeal )
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
s32 FinderModelControlWalker::debug_getPrevWayPoint1( s32 old_idx )
{
  if( old_idx - 1 >= -1 )
  {
    -- old_idx;
  }

  return old_idx;
}
s32 FinderModelControlWalker::debug_getNextWayPoint1( s32 old_idx )
{
  if( old_idx + 1 < m_pPathData->pos_num )
  {
    ++ old_idx;
  }

  return old_idx;
}
s32 FinderModelControlWalker::debug_getPrevWayPoint2( s32 wp1, s32 wp2 )
{
  if( wp1 <= -1 ){ return -1; }
  if( wp1 >= m_pPathData->pos_num ){ return -1; }
  if( wp2 <= -1 ){ return -1; }

  const FinderRouteData *pRouteData = &(m_pPathData->route[wp1]);
  s32 prev = -1;
  s32 dst_idx = -1;
  for( s32 idx = 0; idx < FINDER_CONECTION_MAX; ++ idx )
  {
    s32 wp = pRouteData->next_idx[idx];
    if( wp == -1 ){ continue; }

    if( wp == wp2 )
    {
      prev = dst_idx;
    }
    // つぎへ
    dst_idx = wp;
  }

  return prev;
}
s32 FinderModelControlWalker::debug_getNextWayPoint2( s32 wp1, s32 wp2 )
{
  if( wp1 <= -1 ){ return -1; }
  if( wp1 >= m_pPathData->pos_num ){ return -1; }

  const FinderRouteData *pRouteData = &(m_pPathData->route[wp1]);
  s32 next = pRouteData->next_idx[0];
  for( s32 idx = 0; idx < FINDER_CONECTION_MAX -1; ++ idx )
  {
    s32 wp = pRouteData->next_idx[idx];
    s32 np = pRouteData->next_idx[idx+1];
    if( wp == -1 ){ continue; }
    if( wp == wp2 )
    {
      if( np != -1 )
      {
        next = np;
      }
      else
      {
        next = wp;
      }
    }
  }

  return next;
}

s32 FinderModelControlWalker::debug_getPrevWayPoint3( s32 wp1, s32 wp2, s32 wp3 )
{
  if( wp1 <= -1 ){ return -1; }
  if( wp1 >= m_pPathData->pos_num ){ return -1; }
  if( wp2 <= -1 ){ return -1; }
  if( wp2 >= m_pPathData->pos_num ){ return -1; }
  if( wp3 <= -1 ){ return -1; }

  const FinderRouteData *pRouteData = &(m_pPathData->route[wp2]);
  s32 prev = -1;
  s32 dst_idx = -1;
  for( s32 idx = 0; idx < FINDER_CONECTION_MAX; ++ idx )
  {
    s32 wp = pRouteData->next_idx[idx];
    if( wp == -1 ){ continue; }

    if( wp == wp3 && wp != wp1 )
    {
      prev = dst_idx;
    }
    // つぎへ
    dst_idx = wp;
  }

  return prev;
}
s32 FinderModelControlWalker::debug_getNextWayPoint3( s32 wp1, s32 wp2, s32 wp3 )
{
  if( wp1 <= -1 ){ return -1; }
  if( wp1 >= m_pPathData->pos_num ){ return -1; }
  if( wp2 <= -1 ){ return -1; }
  if( wp2 >= m_pPathData->pos_num ){ return -1; }

  const FinderRouteData *pRouteData = &(m_pPathData->route[wp2]);
  s32 next = pRouteData->next_idx[0];
  for( s32 idx = 0; idx < FINDER_CONECTION_MAX -1; ++ idx )
  {
    s32 wp = pRouteData->next_idx[idx];
    s32 np = pRouteData->next_idx[idx+1];
    if( wp == -1 ){ continue; }
    if( wp == wp3 && np != wp1 )
    {
      if( np != -1 )
      {
        next = np;
      }
      else
      {
        next = wp;
      }
    }
  }

  return next;
}
void FinderModelControlWalker::debug_createPath( s32 wp1, s32 wp2, s32 wp3 )
{
  m_nTarget[TARGET_CURRENT]   = wp1;
  m_nTarget[TARGET_NEXT]      = wp2;
  m_nTarget[TARGET_NEXTNEXT]  = wp3;

  createPath();
}

#endif
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)
