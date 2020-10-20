//======================================================================
/**
 * @file FieldFinderModel.cpp
 * @date 2015/10/07 15:04:15
 * @author miyachi_soichi
 * @brief フィールドで使用するバトルモデル：ファインダー用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// System
#include "System/include/GflUse.h"
// PokeTool
#include "PokeTool/include/PokeModel.h"
// self
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"
// control
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControl.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControlStatue.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControlWalker.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControlOneway.h"
// sound
#include "Sound/include/Sound.h"
#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

static const u32 CONV_SEC2FRAME = 30;
/**
 *  @class  FinderModel
 *  @brief  ファインダーで使用するポケモンモデル
 */
/**
 *  @brief  コンストラクタ
 */
FinderModel::FinderModel( PokeTool::PokeModel *pModel, Field::MyRenderingPipeLine *pPipe, const FinderModelData *pData, u32 idx )
: FieldTrialModel( pModel, pPipe )
, m_pControl( NULL )
, m_eInterestType( INTEREST_TYPE_NONE )
, m_eCtrlType( CTRL_TYPE_None )
, m_FigureAnim( 0 )
, m_Voice( 0 )
, m_InterestResetFrame( 0 )
, m_SlotIndex( idx )
, m_IsAppeal( false )
, m_HasPokeData( false )
{
  m_rModelData = *pData;
  m_eMode = MODE_NORMAL;

  m_CoolTime_NoAppeal = CalcNoAppealVoiceCoolTime();
#if PM_DEBUG
  deb_1 = -1;
  deb_2 = -1;
  deb_3 = -1;
#endif
}

/**
 *  @brief  デストラクタ
 */
FinderModel::~FinderModel( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  移動コンポーネントを生成する
 */
void FinderModel::CreateControlWalker( gfl2::heap::HeapBase *pHeap, const FinderPathData *pathData )
{
  if( m_pControl ){ return; }
  m_pControl = GFL_NEW( pHeap ) FinderModelControlWalker( pathData );
  m_pControl->Initialize( this, pHeap );
  m_eCtrlType = CTRL_TYPE_Walker;
}

/**
 *  @brief  待機コンポーネントを生成する
 */
void FinderModel::CreateControlStatue( gfl2::heap::HeapBase *pHeap, const FinderStatueData *statueData )
{
  if( m_pControl ){ return; }
  m_pControl = GFL_NEW( pHeap ) FinderModelControlStatue( statueData );
  m_pControl->Initialize( this, pHeap );
  m_eCtrlType = CTRL_TYPE_Statue;
}

/**
 *  @brief  一通コンポーネントを生成する
 */
void FinderModel::CreateControlOneway( gfl2::heap::HeapBase *pHeap, const FinderOnewayData *onewayData )
{
  if( m_pControl ){ return; }
  m_pControl = GFL_NEW( pHeap ) FinderModelControlOneway( onewayData );
  m_pControl->Initialize( this, pHeap );
  m_eCtrlType = CTRL_TYPE_Oneway;
}

/**
 *  @brief  操作コンポーネントを破壊する
 */
void FinderModel::DestroyControl( void )
{
  if( m_pControl )
  {
    m_pControl->Terminate();
    GFL_SAFE_DELETE( m_pControl );
  }
}

/**
 *  @brief  安全に破棄できるか調べる
 */
bool FinderModel::IsSafeDelete( void )
{
  if( !m_pModel->CanDelete() )
    return false;

  return true;
}

/**
 *  @brief  更新
 *  @note   アニメーション更新前に行う
 */
void FinderModel::UpdateBeforeAnimation( void )
{
  updateInterestPosition();

  switch( m_eMode )
  {
  case MODE_NORMAL:
    if( m_pControl )
    {
      m_pControl->UpdateBeforeAnimation();
    }
    break;
  default:
    break;
  }
}

/**
 *  @brief  更新
 *  @note   アニメーション更新後に行う
 */
void FinderModel::UpdateAfterAnimation( void )
{
  switch( m_eMode )
  {
  case MODE_NORMAL:
    if( m_pControl )
    {
      m_pControl->UpdateAfterAnimation();
    }
    break;
  case MODE_REQUEST_SUSPEND:
    if( !m_pModel->IsAnimationInterp() )
    {
      // 保管が終わったら停止する
      m_fStepFrame = m_pModel->GetAnimationStepFrame();
      m_pModel->SetAnimationStepFrame( 0.0f );
      m_eMode = MODE_SUSPEND;
    }
    break;
  default:
    break;
  }
}

/**
 *  @brief  更新
 */
void FinderModel::UpdateAfterTraverse( void )
{
  m_pModel->UpdateInterest();
  // ボイスは座標確定後に更新する
  CalcVoiceVolume();
  CalcVoicePan();
}

/**
 *  @brief  待機モーションを取得する
 */
PokeTool::MODEL_ANIME_POKE_FINDER FinderModel::GetWaitAnimNo( u32 anim )
{
  PokeTool::MODEL_ANIME_POKE_FINDER nextAnim = static_cast<PokeTool::MODEL_ANIME_POKE_FINDER>( anim );
  if( nextAnim >= PokeTool::MODEL_ANIME_PF_KW_WAIT && m_pModel->IsAvailableAnimation( PokeTool::MODEL_ANIME_PF_KW_WAIT ) )
  {
    return PokeTool::MODEL_ANIME_PF_KW_WAIT;
  }

  return PokeTool::MODEL_ANIME_PF_BT_WAIT_A;
}

/**
 *  @brief  移動モーションを取得する：ランダムで
 */
PokeTool::MODEL_ANIME_POKE_FINDER FinderModel::GetMoveAnimNo( void )
{
  // 歩きのみ
  if( m_rPokeData.move == 1 )
  {
    return PokeTool::MODEL_ANIME_PF_WALK01;
  }
  // 走りのみ
  if( m_rPokeData.move == 2 )
  {
    return PokeTool::MODEL_ANIME_PF_RUN01;
  }

  u32 rand = System::GflUse::GetPublicRand( 100 );
  PokeTool::MODEL_ANIME_POKE_FINDER anim = PokeTool::MODEL_ANIME_PF_WALK01;
  if( rand < 50 && m_pModel->IsAvailableAnimation( PokeTool::MODEL_ANIME_PF_RUN01 ) )
  {
    anim = PokeTool::MODEL_ANIME_PF_RUN01;
  }
  return anim;
}

/**
 *  @brief  アピール挙動を取得する
 *  @param  out_AnimNo  モーション番号
 */
void FinderModel::GetAppealAction( u32 &out_AnimNo )
{
  // 注視とキョロキョロを追加するので+2する
  u32 tbl[ FINDER_ANIM_MAX +2];
  s32 fgr[ FINDER_ANIM_MAX +2];
  u32 vic[ FINDER_ANIM_MAX +2];
  for( u32 i = 0; i < FINDER_ANIM_MAX+2; ++ i )
  {
    tbl[i] = FINDER_ANIM_ERROR;
    fgr[i] = 0;
    vic[i] = 0;
  }

  // 個数がほしい
  u32 cnt = 0;
  for( u32 i = 0; i < FINDER_ANIM_MAX; ++ i )
  {
    // 未設定は飛ばす
    if( m_rModelData.animNo[i] == FINDER_ANIM_ERROR ){ continue; }
    tbl[cnt] = m_rModelData.animNo[i];
    fgr[cnt] = m_rModelData.figureAnime[i];
    vic[cnt] = m_rModelData.voice[i];
    cnt += 1;
  }

  // カメラ注視とキョロキョロを足す
  if( m_rPokeData.enable )
  {
    tbl[cnt] = Field::TrialModel::FINDER_APPEAL_CUSTOM_CAMERA;
    cnt += 1;
    tbl[cnt] = Field::TrialModel::FINDER_APPEAL_CUSTOM_SEARCH;
    cnt += 1;
  }

  if( cnt == 0 )
  {
    // データ上ここに来てはいけない
    GFL_ASSERT_MSG( 0, "FinderPokemon has NoAppeal\n" );
    out_AnimNo = FINDER_ANIM_ERROR;
    return;
  }

  u32 idx = System::GflUse::GetPublicRand( cnt );
#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_DebugFinderParam.nAppealIdx >= 0 )
  {
    // アピールの固定化
    u32 d_idx = static_cast<u32>( Field::Debug::DebugTypes::s_DebugFinderParam.nAppealIdx );
    if( d_idx < cnt )
    {
      idx = d_idx;
    }
  }
#endif
  out_AnimNo = tbl[idx];
  // アピールの予約
  m_FigureAnim = fgr[idx];
  m_Voice = vic[idx];
}

/**
 *  @brief  個体が持つ得点を取得する
 */
u32 FinderModel::GetBasePoint( void )
{
  return m_rModelData.basePoint;
}

/**
 *  @brief  容姿を取得
 */
u32 FinderModel::GetFigure( void )
{
  return m_rModelData.figure;
}

/**
 *  @brief  アピール容姿を取得
 */
u32 FinderModel::GetFigureAppeal( void )
{
  if( m_IsAppeal )
  {
    return m_FigureAnim;
  }

  return FIGURE_NONE;
}

/**
 *  @brief  停止リクエスト
 */
void FinderModel::RequestSuspend( void )
{
  m_eMode = MODE_REQUEST_SUSPEND;
}

/**
 *  @brief  再生リクエスト
 */
void FinderModel::RequestResume( void )
{
  m_eMode = MODE_NORMAL;
  m_pModel->SetAnimationStepFrame( m_fStepFrame );
}

/**
 *  @brief  停止確認
 */
bool FinderModel::IsPause( void )
{
  return m_eMode == MODE_SUSPEND;
}

/**
 *  @brief  アピール
 */
void FinderModel::SetAppeal( bool flag )
{
  m_IsAppeal = flag;
  if( flag )
  {
    // 開始
    if( m_Voice != 0 )
    {
      PlayVoice( m_Voice );
    }
  }
  else
  {
    // 終了
  }
}

/**
 *  @brief  描画優先度を専用モードに切り替え
 *  @note   ゴース用対応
 */
void FinderModel::SetOptionalRenderKey( void )
{
  if( !IsEnable() ){ return; }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pModelInstanceNode = m_pModel->GetModelInstanceNode();
  pModelInstanceNode->SetOptionalRenderKey();
}

/**
 *  @brief  注視開始
 */
void FinderModel::EnableInterestCamera( void )
{
  m_eInterestType = INTEREST_TYPE_CAMERA;
  m_InterestResetFrame = 0;
  m_pModel->SetInterestPosition( &m_InterestPos_Camera );
  m_pModel->SetInterestAdjustAngleEnable( true );

  m_nAdjustAngle_Life = FINDER_CAMINTEREST_RESET_FRAME;
  m_fAdjustAngle_Now = 0.0f;
  m_fAdjustAngle_End = gfl2::math::ConvDegToRad( m_rModelData.adjustAngle );
  m_fAdjustAngle_Add = (m_fAdjustAngle_End-m_fAdjustAngle_Now) / m_nAdjustAngle_Life;
  m_pModel->SetInterestTagAdjustAngle( m_fAdjustAngle_Now );

  m_pModel->SetInterestSpringVal(1.0f, 1.0f); 
}
void FinderModel::EnableInterestSearch( void )
{
  m_eInterestType = INTEREST_TYPE_SEARCH;
  m_InterestResetFrame = 0;
  m_pModel->SetInterestPosition( &m_InterestPos_Search );

  m_pModel->SetInterestSpringVal(1.0f, 1.0f); 
}

/**
 *  @brief  注視終了
 */
void FinderModel::DisableInterest( u32 frm )
{
  if( !IsInterest() ){ return; }

  if( frm <= 0 )
  {
    m_InterestResetFrame = FINDER_CAMINTEREST_RESET_FRAME;
  }
  else
  {
    m_InterestResetFrame = frm;
  }

  m_eInterestType = INTEREST_TYPE_ENDING;
  m_nAdjustAngle_Life = FINDER_CAMINTEREST_RESET_FRAME;
  m_fAdjustAngle_End = 0.0f;
  m_fAdjustAngle_Add = (m_fAdjustAngle_End-m_fAdjustAngle_Now) / m_nAdjustAngle_Life;
  m_pModel->SetInterestTagAdjustAngle( m_fAdjustAngle_Now );
  m_pModel->SetInterestResetFrameCount( 0, m_InterestResetFrame );
  m_pModel->SetInterestPosition( NULL );
  m_pModel->SetInterestSpringVal(1.0f, 1.0f); 
}

/**
 *  @brief  注視中確認
 */
bool FinderModel::IsInterest( void )
{
  if( m_eInterestType != INTEREST_TYPE_NONE ){ return true; }
  if( m_InterestResetFrame > 0 ){ return true; }
  return false;
}
bool FinderModel::IsFinishInterest( void )
{
  return m_pModel->IsFinishInterest();
}

/**
 *  @brief  注視座標更新
 */
void FinderModel::SetInterestPosition( const gfl2::math::Vector3 &pos )
{
  m_InterestPos_Camera = pos;
}
void FinderModel::SetInterestPositionSearch( const gfl2::math::Vector3 &pos )
{
  m_InterestPos_Search = pos;
}
void FinderModel::SetInterestSpeedSearch( f32 spd )
{
  m_pModel->SetInterestSpeed( 0, spd );
}

/**
 *  @brief  アピールを保持しているか
 */
bool FinderModel::HasAppeal( void )
{
  for( u32 i = 0; i < FINDER_ANIM_MAX; ++ i )
  {
    if( m_rModelData.animNo[i] != FINDER_ANIM_ERROR )
    {
      return true;
    }
  }
  return false;
}

/**
 *  @brief  ポケモンを呼ぶ
 */
void FinderModel::RequestPokeCall( void )
{
  if( m_pControl && m_rPokeData.enable )
  {
    m_pControl->RequestPokeCall();
  }
}

/**
 *  @brief  ボイスを再生する
 */
void FinderModel::PlayVoice( u32 voiceID )
{
  // ボイス再生中
  if( !Sound::IsVoiceFinished( m_SlotIndex ) ){ return; }

  VoiceID eVoice = static_cast<VoiceID>( voiceID );
  Sound::PlayVoice( m_SlotIndex, eVoice );

  CalcVoiceVolume();
  CalcVoicePan();
  // 一之瀬さん要望対応：デバッグ表示
#if PM_DEBUG
  GFL_PRINT("PlayVoice Volume=%0.2f Pan=%0.2f\n", m_voice_volume, m_voice_pan );
#endif
}

/**
 *  @brief  ボイスのボリュームを再計算する
 */
void FinderModel::CalcVoiceVolume( void )
{
  // ボイス再生中
  if( Sound::IsVoiceFinished( m_SlotIndex ) ){ return; }

  gfl2::math::Vector3 self_pos = m_pModel->GetPosition();
  gfl2::math::Vector3 diff = self_pos - m_InterestPos_Camera;

  f32 length_now   = diff.Length();
  // 一之瀬さん要望対応：サウンドパラメータ調整
  f32 length_start = 200.0f;
  f32 length_end   = 700.0f;
  f32 volume_start = 0.45f;
  f32 volume_end   = 0.2f;

#if PM_DEBUG
  // デバッグで置き換え
  if( Field::Debug::DebugTypes::s_DebugFinderParam.bVoice != 0 )
  {
    length_start = Field::Debug::DebugTypes::s_DebugFinderParam.fLenStart;
    length_end   = Field::Debug::DebugTypes::s_DebugFinderParam.fLenEnd;
    volume_start = Field::Debug::DebugTypes::s_DebugFinderParam.fVolStart;
    volume_end   = Field::Debug::DebugTypes::s_DebugFinderParam.fVolEnd;
  }
#endif

  // 単位距離当たりの変化ボリュームを算出する
  f32 vpl = 0.0f;
  if( length_end - length_start >= EPSILON || -EPSILON <= length_end - length_start ) // 安全のため0.0付近は差分なしとする
  {
    vpl = ( volume_end - volume_start ) / ( length_end - length_start );
  }

  // 距離の分岐
  f32 volume_now = volume_start;
  if( length_now <= length_start )
  {
    // 近い
    volume_now = volume_start;
  }
  else if( length_now >= length_end )
  {
    // 遠い
    volume_now = volume_end;
  }
  else
  {
    // 減衰中
    f32 diff_length = length_now - length_start;
    volume_now = vpl * diff_length + volume_start;
  }

  Sound::ChangeVoiceVolume( m_SlotIndex, volume_now );
  // 一之瀬さん要望対応：デバッグ表示
#if PM_DEBUG
  m_voice_volume = volume_now;
#endif
}

/**
 *  @brief  ボイスのパンを再計算する
 */
void FinderModel::CalcVoicePan( void )
{
  // ボイス再生中
  if( Sound::IsVoiceFinished( m_SlotIndex ) ){ return; }

  f32 dir_cos   = 1.0f;
  f32 dir_cross = 1.0f;
  f32 cos_start = 10.0f;
  f32 cos_end   = 170.0f;
  f32 power     = 0.90f;

#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_DebugFinderParam.bVoice != 0 )
  {
    cos_start = Field::Debug::DebugTypes::s_DebugFinderParam.fPanStart;
    cos_end   = Field::Debug::DebugTypes::s_DebugFinderParam.fPanEnd;
    power     = Field::Debug::DebugTypes::s_DebugFinderParam.fPanPow;
  }
#endif

  // カメラ方向ベクトル
  gfl2::math::Vector3 cam_dir( 0.0f, 0.0f, 1.0f );
  gfl2::math::Matrix34 mtx; mtx = gfl2::math::Matrix34::GetRotationY( m_fCameraRotY );
  cam_dir = mtx * cam_dir;
  // カメラからキャラへのベクトル
  gfl2::math::Vector3 chr_dir( 0.0f, 0.0f, 1.0f );
  chr_dir = m_InterestPos_Camera - m_pModel->GetPosition();
  chr_dir.SetY( 0.0f );
  chr_dir = chr_dir.Normalize();

  f32 length = chr_dir.Length();
  // 角度を算出する
  if( length > 0.0f )
  {
    dir_cos = chr_dir.Dot( cam_dir );
    dir_cos /= length;
    gfl2::math::Vector3 cross = chr_dir.Cross( cam_dir );
    dir_cross = cross.GetY();
  }

  // パンの値を求める
  cos_start = gfl2::math::CosDeg( cos_start );
  cos_end   = gfl2::math::CosDeg( cos_end );
  f32 length_max = gfl2::math::Abs( cos_end - cos_start );

  f32 pan = 0.0f;
  if( cos_start >= dir_cos && cos_end <= dir_cos )
  {
    f32 cos_len = gfl2::math::Abs( dir_cos - cos_start );
    pan = gfl2::math::SinDeg( ( cos_len / length_max ) * 180.0f );
  }

  if( dir_cross > 0.0f )
  {
    pan *= power;
  }
  else
  {
    pan *= -power;
  }

  // パンを設定する
  Sound::ChangeVoicePan( m_SlotIndex, pan );
  // 一之瀬さん要望対応：デバッグ表示
#if PM_DEBUG
  m_voice_pan = pan;
#endif
}

/**
 *  @brief  アピールなし鳴き声
 */
void FinderModel::UpdateNoAppealVoice( void )
{
  // アピールがあるならいらない
  if( HasAppeal() ){ return; }
  // ボイス設定されていない
  if( GetCallVoice() == 0 ){ return; }
  // ボイス再生中
  if( !Sound::IsVoiceFinished( m_SlotIndex ) ){ return; }
  // @fix GFNMCat[1740] 非常時では泣かない
  if( !GetVisible() ){ return; }

  // 一之瀬さん要望対応：サウンドパラメータ調整
  u32 rate = 80;
#if PM_DEBUG
  {
    rate = Field::Debug::DebugTypes::s_DebugFinderParam.nNoAppRate;
  }
#endif

  if( m_CoolTime_NoAppeal > 0 )
  {
    // クールタイム
    -- m_CoolTime_NoAppeal;
  }
  else
  {
    // 抽選
    if( System::GflUse::GetPublicRand( 100 ) < rate )
    {
      // なく
      PlayVoice( GetCallVoice() );
    }
    // もう一度
    m_CoolTime_NoAppeal = CalcNoAppealVoiceCoolTime();
  }
}

/**
 *  @brief  クールタイム
 */
u32 FinderModel::CalcNoAppealVoiceCoolTime( void )
{
  u32 result = 0;
  // 一之瀬さん要望対応：サウンドパラメータ調整
  u32 min = 5;
  u32 max = 10;

#if PM_DEBUG
  {
    min = Field::Debug::DebugTypes::s_DebugFinderParam.nNoAppCMin;
    max = Field::Debug::DebugTypes::s_DebugFinderParam.nNoAppCMax;
  }
#endif

  result = ( System::GflUse::GetPublicRand( max - min ) + min ) * CONV_SEC2FRAME;

  return result;
}

/**
 *  @brief  ボイス取得
 */
u32 FinderModel::GetCallVoice( void )
{
  return m_rPokeData.voice;
}

/**
 *  @brief  AABBを取得する
 */
void FinderModel::GetAABB( gfl2::math::AABB *pAABB )
{
  if( !pAABB ){ return; }
  if( !m_pModel ){ return; }

  gfl2::math::AABB dataAABB;

#if 0
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pModelInstanceNode = m_pModel->GetModelInstanceNode();
  dataAABB = pModelInstanceNode->GetAABB();
#else
  dataAABB.SetMin( m_rPokeData.min );
  dataAABB.SetMax( m_rPokeData.max );
#endif

  gfl2::math::Vector4 min = dataAABB.GetMin();
  gfl2::math::Vector4 max = dataAABB.GetMax();

#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_DebugFinderParam.bEnableAABB != 0 )
  {
    min.x = Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_x;
    min.y = Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_y;
    min.z = Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_z;
    max.x = Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_x;
    max.y = Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_y;
    max.z = Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_z;
  }
#endif

  pAABB->SetMinMax( min, max );
}

/**
 *  @brief  AABBをマウントする座標を取得する
 */
gfl2::math::Vector3 FinderModel::GetAABBMountPosition( void )
{
  gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );

  if( m_pModel )
  {
    gfl2::math::Matrix34 mtx;
    mtx =  m_pModel->GetJointWorldMatrix(m_pModel->GetJointIndex( "Waist" ) );
    pos = mtx.GetElemPosition();
  }

  return pos;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  注目更新
 */
void FinderModel::updateInterestPosition( void )
{
  if( m_eInterestType == INTEREST_TYPE_CAMERA )
  {
    m_pModel->SetInterestPosition( &m_InterestPos_Camera );
  }
  else if( m_eInterestType == INTEREST_TYPE_SEARCH )
  {
    m_pModel->SetInterestPosition( &m_InterestPos_Search );
  }
  else if( m_eInterestType == INTEREST_TYPE_ENDING )
  {
    m_pModel->SetInterestPosition( NULL );
  }

  // 補間処理
  if( m_nAdjustAngle_Life > 0 )
  {
    -- m_nAdjustAngle_Life;
    if( m_nAdjustAngle_Life <= 0 )
    {
      m_nAdjustAngle_Life = 0;
      m_fAdjustAngle_Now = m_fAdjustAngle_End;
    }
    else
    {
      m_fAdjustAngle_Now += m_fAdjustAngle_Add;
    }
    m_pModel->SetInterestTagAdjustAngle( m_fAdjustAngle_Now );
  }

  if( m_InterestResetFrame > 0 )
  {
    -- m_InterestResetFrame;
    if( m_InterestResetFrame <= 0 )
    {
      m_pModel->SetInterestAdjustAngleEnable( false );
      m_pModel->SetInterestPosition( NULL );

      m_eInterestType = INTEREST_TYPE_NONE;
    }
  }
}

#if PM_DEBUG
f32 FinderModel::debug_getPosX(void)
{
  return m_pModel->GetPosition().x;
}
f32 FinderModel::debug_getPosY(void)
{
  return m_pModel->GetPosition().y;
}
f32 FinderModel::debug_getPosZ(void)
{
  return m_pModel->GetPosition().z;
}
void FinderModel::debug_setPosX(f32 dst)
{
  gfl2::math::Vector3 pos = m_pModel->GetPosition();
  pos.x = dst;
  m_pModel->SetPosition( pos );
}
void FinderModel::debug_setPosY(f32 dst)
{
  gfl2::math::Vector3 pos = m_pModel->GetPosition();
  pos.y = dst;
  m_pModel->SetPosition( pos );
}
void FinderModel::debug_setPosZ(f32 dst)
{
  gfl2::math::Vector3 pos = m_pModel->GetPosition();
  pos.z = dst;
  m_pModel->SetPosition( pos );
}
f32 FinderModel::debug_getRotY(void)
{
  return gfl2::math::ConvRadToDeg(m_pModel->GetRotation().y);
}
void FinderModel::debug_setRotY(f32 dst)
{
  gfl2::math::Vector3 rot = m_pModel->GetRotation();
  rot.y = gfl2::math::ConvDegToRad(dst);
  m_pModel->SetRotation( rot );
}

void FinderModel::debug_getPrevWayPoint1(void)
{
  if( m_eCtrlType == CTRL_TYPE_Walker )
  {
    FinderModelControlWalker* walker = reinterpret_cast<FinderModelControlWalker*>(m_pControl);
    deb_1 = walker->debug_getPrevWayPoint1( deb_1 );
    deb_2 = -1;
    deb_3 = -1;
  }
}
void FinderModel::debug_getNextWayPoint1(void)
{
  if( m_eCtrlType == CTRL_TYPE_Walker )
  {
    FinderModelControlWalker* walker = reinterpret_cast<FinderModelControlWalker*>(m_pControl);
    deb_1 = walker->debug_getNextWayPoint1( deb_1 );
    deb_2 = -1;
    deb_3 = -1;
  }
}
void FinderModel::debug_getPrevWayPoint2(void)
{
  if( m_eCtrlType == CTRL_TYPE_Walker )
  {
    FinderModelControlWalker* walker = reinterpret_cast<FinderModelControlWalker*>(m_pControl);
    deb_2 = walker->debug_getPrevWayPoint2( deb_1, deb_2 );
    deb_3 = -1;
  }
}
void FinderModel::debug_getNextWayPoint2(void)
{
  if( m_eCtrlType == CTRL_TYPE_Walker )
  {
    FinderModelControlWalker* walker = reinterpret_cast<FinderModelControlWalker*>(m_pControl);
    deb_2 = walker->debug_getNextWayPoint2( deb_1, deb_2 );
    deb_3 = -1;
  }
}
void FinderModel::debug_getPrevWayPoint3(void)
{
  if( m_eCtrlType == CTRL_TYPE_Walker )
  {
    FinderModelControlWalker* walker = reinterpret_cast<FinderModelControlWalker*>(m_pControl);
    deb_3 = walker->debug_getPrevWayPoint3( deb_1, deb_2, deb_3 );
  }
}
void FinderModel::debug_getNextWayPoint3(void)
{
  if( m_eCtrlType == CTRL_TYPE_Walker )
  {
    FinderModelControlWalker* walker = reinterpret_cast<FinderModelControlWalker*>(m_pControl);
    deb_3 = walker->debug_getNextWayPoint3( deb_1, deb_2, deb_3 );
  }
}
void FinderModel::debug_createPath( void )
{
  if( m_eCtrlType == CTRL_TYPE_Walker )
  {
    FinderModelControlWalker* walker = reinterpret_cast<FinderModelControlWalker*>(m_pControl);
    walker->debug_createPath( deb_1, deb_2, deb_3 );
  }
}
void FinderModel::debug_onemore( void )
{
  if( m_eCtrlType == CTRL_TYPE_Oneway )
  {
    FinderModelControlOneway *oneway = reinterpret_cast<FinderModelControlOneway*>( m_pControl );
    oneway->debug_onemore();
  }
}
u32 FinderModel::debug_getOnewayDelay( void )
{
  if( m_eCtrlType == CTRL_TYPE_Oneway )
  {
    FinderModelControlOneway *oneway = reinterpret_cast<FinderModelControlOneway*>( m_pControl );
    return oneway->debug_delay();
  }
  return 0;
}
u32 FinderModel::debug_getStatuePosition( void )
{
  if( m_eCtrlType == CTRL_TYPE_Statue )
  {
    FinderModelControlStatue *statue = reinterpret_cast<FinderModelControlStatue*>( m_pControl );
    return statue->debug_GetPosition();
  }
  return 0;
}
void FinderModel::debug_setStatuePosition( u32 idx )
{
  if( m_eCtrlType == CTRL_TYPE_Statue )
  {
    FinderModelControlStatue *statue = reinterpret_cast<FinderModelControlStatue*>( m_pControl );
    statue->debug_SetPosition( idx );
  }
}
void FinderModel::debug_requestStatueMotion( void )
{
  if( m_eCtrlType == CTRL_TYPE_Statue )
  {
    m_pModel->ChangeAnimation( static_cast<PokeTool::MODEL_ANIME_POKE_FINDER>( m_statue_motion ) );
    m_pModel->SetAnimationLoop( true );
  }
}

#endif

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)
