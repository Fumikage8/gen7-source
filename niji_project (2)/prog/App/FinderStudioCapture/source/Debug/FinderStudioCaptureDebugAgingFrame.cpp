#if PM_DEBUG

//======================================================================
/**
 * @file    FinderStudioCaptureDebugAgingFrame.cpp
 * @date    2017/04/18 10:55:07
 * @author  fukushima_yuya
 * @brief   デバッグ機能フレーム：エージング
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureDebugAgingFrame.h"

#include <App/FinderStudioCapture/source/FinderStudioCaptureViewerManager.h>
#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebug.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>
#include <App/FinderStudioViewer/include/FinderStudioViewer.h>

#include <System/include/gflUse.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )


//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
DebugAgingFrame::DebugAgingFrame( void )
  : m_pViewerManager( NULL )
  , m_AgingLoopCount( 0 )
  , m_PokemonSimpleParam()
  , m_PokemonStartMonsNo( MONSNO_001 )
  , m_PokemonFinishMonsNo( MONSNO_001 )
  , m_PokemonMonsNo( MONSNO_001 )
  , m_PokemonFormNo( 0 )
  , m_PokemonFormMax( 1 )
  , m_PokemonSex( pml::SEX_MALE )
  , m_PokemonSexType( pml::personal::SEX_TYPE_RANDOM )
  , m_bPokemonRare( false )
  , m_PokemonPositionSeq( 0 )
  , m_PokemonPositionID( 0 )
  , m_PokemonRotationID( 0 )
  , m_PokemonMotionSeq( 0 )
  , m_PokemonMotionID( 0 )
  , m_pPokemonMotionList( NULL )
  , m_PokemonMotion( PokeTool::MODEL_ANIME_ERROR )
  , m_MotionListMax( 0 )
  , m_PokemonMotionType( Viewer::POKE_MOTION_TYPE_BATTLE )
  , m_MotionWaitFrame( 0 )
  , m_PokemonSleepMotionSeq( 0 )
  , m_PokemonSleepWaitFrame( 0 )
  , m_TrainerDressUpParamSex( poke_3d::model::DressUpParam::SEX_MALE )
  , m_TrainerZMotionType( Viewer::Z_MOTION_TYPE_NORMAL )
  , m_BackGroundID( BG_ID_00 )
  , m_BackGroundCount( 0 )
  , m_BackGroundWaitTime( 0 )
  , m_BackGroundTimeZone( Viewer::TIME_ZONE_NOON )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
applib::frame::Result DebugAgingFrame::InitFunc( void )
{
  if( m_SubSeq == 0 )
  {
    /// ViewerManagerの取得
    m_pViewerManager = m_pMainWork->viewer;
    /// Viewerをリセットする
    m_pViewerManager->RequestViewerUpdate_AllReset();
    m_pViewerManager->StartUpdateViewer();

    /// 全てのViewの入力を切る
    for( u32 i = 0; i < VIEW_ID_NUM; ++i )
    {
      Static::ViewBase* viewBase = m_pMainWork->viewList->GetView( i );
      {
        viewBase->SetInputEnabled( false );
      }
    }

    this->SetSubSeq( 1 );
  }
  else {
    if( _IsFinishUpdateViewer() )
    {
      this->SetMainSeq( SEQ_INIT );

      return applib::frame::RES_FINISH;
    }
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   終了
 */
applib::frame::Result DebugAgingFrame::EndFunc( void )
{
  this->GetFrameManager()->SetResult( Frame::RES_NEXT );
  return applib::frame::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
applib::frame::Result DebugAgingFrame::UpdateFunc( void )
{
  if( !DebugClass::IsAging_Start() )
  {
    this->SetMainSeq( SEQ_END );
  }

  if( m_MainSeq == SEQ_INIT )
  {
    /// 初期化
    _InitSequence();
  }
  else if( m_MainSeq == SEQ_UPDATE_POKEMON )
  {
    /// 更新：ポケモン
    _UpdateSequence_Pokemon();
  }
  else if( m_MainSeq == SEQ_UPDATE_TRAINER )
  {
    /// 更新：トレーナー
    _UpdateSequence_Trainer();
  }
  else if( m_MainSeq == SEQ_UPDATE_BACKGROUND )
  {
    /// 更新：背景
    _UpdateSequence_BackGround();
  }
  else if( m_MainSeq == SEQ_END )
  {
    if( _IsAgingStart() )
    {
      this->SetMainSeq( SEQ_INIT );
    }
    else {
      return applib::frame::RES_FINISH;
    }
  }

  return applib::frame::RES_CONTINUE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void DebugAgingFrame::_InitSequence( void )
{
  if( _IsAgingStartPokemon() )
  {
    /// 初期化：ポケモン
    _InitSequence_Pokemon();
  }
  else if( _IsAgingStartTrainer() )
  {
    /// 初期化：トレーナー
    _InitSequence_Trainer();
  }
  else if( _IsAgingStartBackGround() )
  {
    /// 初期化：背景
    _InitSequence_BackGround();
  }
  else {
    /// 何も無ければSEQ_ENDへ
    this->SetMainSeq( SEQ_END );
  }
}

/**
 * @brief   初期化：ポケモン
 */
void DebugAgingFrame::_InitSequence_Pokemon( void )
{
  /// 開始ポケモンを取得
  m_PokemonStartMonsNo = DebugClass::GetAging_PokemonStartMonsNo();
  /// 終了ポケモンを取得
  m_PokemonFinishMonsNo = DebugClass::GetAging_PokemonFinishMonsNo();

  /// エージング開始を通知
  m_pViewerManager->GetFinderStudioViewer()->DebugSetupAsing();

  m_AgingLoopCount = 0;

  this->SetMainSeq( SEQ_UPDATE_POKEMON );
}

/**
 * @brief   初期化：トレーナー
 */
void DebugAgingFrame::_InitSequence_Trainer( void )
{
  if( m_SubSeq == 0 )
  {
    /// 性別を初期化
    m_TrainerDressUpParamSex = poke_3d::model::DressUpParam::SEX_MALE;
    /// ZMotionTypeを初期化
    m_TrainerZMotionType = Viewer::Z_MOTION_TYPE_NORMAL;
    
    /// 着せ替えデータの変更
    _RequestChangeDressUp();

    // ループ回数
    _IncrementLoopCount();

    this->SetSubSeq( 1 );
  }
  else if( m_SubSeq == 1 )
  {
    if( _IsEndChangeDressUp() )
    {
      this->SetMainSeq( SEQ_UPDATE_TRAINER );
    }
  }
}

/**
 * @brief   初期化：背景
 */
void DebugAgingFrame::_InitSequence_BackGround( void )
{
  if( m_SubSeq == 0 )
  {
    /// 待機時間を初期化
    m_BackGroundWaitTime = 0;
    /// 時間帯を初期化
    m_BackGroundTimeZone = Viewer::TIME_ZONE_NOON;

    /// 背景の変更
    {
      m_BackGroundID = _GetAgingBackGroundID();
      Viewer::TimeZone timeZone = _GetAgingBackGroundTimeZone( m_BackGroundID );

      m_pViewerManager->RequestViewerUpdate_BackGround( m_BackGroundID, timeZone );
      m_pViewerManager->StartUpdateViewer();

      GFL_RELEASE_PRINT( "BG : ID=[%d] TimeZone=[%d]\n", m_BackGroundID, timeZone );
    }

    // ループ回数
    _IncrementLoopCount();

    this->SetSubSeq( 1 );
  }
  else if( m_SubSeq == 1 )
  {
    if( m_pViewerManager->IsFinishUpdadteViewer() )
    {
      this->SetMainSeq( SEQ_UPDATE_BACKGROUND );
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：ポケモン
 */
void DebugAgingFrame::_UpdateSequence_Pokemon( void )
{
  enum {
    POKEMON_SEQ_INIT,
    POKEMON_SEQ_INIT_SIMPLEPARAM,
    POKEMON_SEQ_INIT_MONSNO,
    POKEMON_SEQ_INIT_PERSONAL_DATA,
    POKEMON_SEQ_INIT_FORMNO,
    POKEMON_SEQ_INIT_SEX,
    POKEMON_SEQ_INIT_RARE,

    POKEMON_SEQ_SETUP_MODEL,
    POKEMON_SEQ_SETUP_MODEL_END,

    POKEMON_SEQ_SETUP_BTL_MOTION,
    POKEMON_SEQ_SETUP_BTL_MOTION_END,

    POKEMON_SEQ_CHECK_BTL_POSITION,
    POKEMON_SEQ_CHECK_BTL_POSITION_END,

    POKEMON_SEQ_CHECK_BTL_MOTION,
    POKEMON_SEQ_CHECK_BTL_MOTION_END,

    POKEMON_SEQ_SETUP_RFR_MOTION,
    POKEMON_SEQ_SETUP_RFR_MOTION_END,

    POKEMON_SEQ_CHECK_RFR_POSITION,
    POKEMON_SEQ_CHECK_RFR_POSITION_END,

    POKEMON_SEQ_CHECK_RFR_MOTION,
    POKEMON_SEQ_CHECK_RFR_MOTION_END,

    POKEMON_SEQ_CHECK_SLEEP_MOTION,
    POKEMON_SEQ_CHECK_SLEEP_MOTION_END,

    POKEMON_SEQ_FIN_RARE,
    POKEMON_SEQ_FIN_SEX,
    POKEMON_SEQ_FIN_FORMNO,
    POKEMON_SEQ_FIN_MONSNO,

    POKEMON_SEQ_END,
  };

  if( m_SubSeq == POKEMON_SEQ_INIT )
  {
    /// MonsNo
    m_PokemonMonsNo = m_PokemonStartMonsNo;
    /// FormNoの初期化
    m_PokemonFormNo = 0;
    /// 性別の初期化
    m_PokemonSex = pml::SEX_MALE;
    /// 色違い
    m_bPokemonRare = false;

    /// ループ回数のインクリメント
    _IncrementLoopCount();

    this->SetSubSeq( POKEMON_SEQ_INIT_SIMPLEPARAM );
  }
  else if( m_SubSeq == POKEMON_SEQ_INIT_SIMPLEPARAM )
  {
    /// PokeTool::SimpleParamの初期化
    m_PokemonSimpleParam = PokeTool::SimpleParam();

    this->SetSubSeq( POKEMON_SEQ_INIT_MONSNO );
  }
  else if( m_SubSeq == POKEMON_SEQ_INIT_MONSNO )
  {
    /// MonsNoの取得
    m_PokemonMonsNo = _GetMonsNo();
    /// MonsNoの設定
    m_PokemonSimpleParam.monsNo = static_cast<MonsNo>( m_PokemonMonsNo );

    FUKUSHIMA_PRINT( "MonsNo : Start=[%d] End=[%d] Now=[%d]\n", m_PokemonStartMonsNo, m_PokemonFinishMonsNo, m_PokemonMonsNo );

    this->SetSubSeq( POKEMON_SEQ_INIT_PERSONAL_DATA );
  }
  else if( m_SubSeq == POKEMON_SEQ_INIT_PERSONAL_DATA )
  {
    MonsNo monsNo = m_PokemonSimpleParam.monsNo;

    /// パーソナルデータの読込み
    _LoadPersonalData( monsNo );
    /// Formの最大値を設定
    m_PokemonFormMax = _GetFormMax( monsNo );
    /// 基本性別を取得
    m_PokemonSex = _GetBaseSex( monsNo );
    /// 性別タイプを取得
    m_PokemonSexType = _GetSexType( monsNo );

    this->SetSubSeq( POKEMON_SEQ_INIT_FORMNO );
  }
  else if( m_SubSeq == POKEMON_SEQ_INIT_FORMNO )
  {
    /// FormNoの設定
    m_PokemonSimpleParam.formNo = static_cast<pml::FormNo>( m_PokemonFormNo );

    this->SetSubSeq( POKEMON_SEQ_INIT_SEX );
  }
  else if( m_SubSeq == POKEMON_SEQ_INIT_SEX )
  {
    /// 性別の設定
    m_PokemonSimpleParam.sex = m_PokemonSex;

    this->SetSubSeq( POKEMON_SEQ_INIT_RARE );
  }
  else if( m_SubSeq == POKEMON_SEQ_INIT_RARE )
  {
    /// 色違いの設定
    m_PokemonSimpleParam.isRare = m_bPokemonRare;

    this->SetSubSeq( POKEMON_SEQ_SETUP_MODEL );
  }
  else if( m_SubSeq == POKEMON_SEQ_SETUP_MODEL )
  {
    /// ポケモンの変更
    _RequestChangePokeModel();

    this->SetSubSeq( POKEMON_SEQ_SETUP_MODEL_END );
  }
  else if( m_SubSeq == POKEMON_SEQ_SETUP_MODEL_END )
  {
    if( _IsEndChangePokeModel() )
    {
      this->SetSubSeq( POKEMON_SEQ_SETUP_BTL_MOTION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_SETUP_BTL_MOTION )
  {
    /// バトルモーションへ変更
    _RequestChanePokeMotionType_Battle();

    this->SetSubSeq( POKEMON_SEQ_SETUP_BTL_MOTION_END );
  }
  else if( m_SubSeq == POKEMON_SEQ_SETUP_BTL_MOTION_END )
  {
    if( _IsEndChangePokeMotionType() )
    {
      this->SetSubSeq( POKEMON_SEQ_CHECK_BTL_POSITION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_BTL_POSITION )
  {
    /// シーケンスの初期化
    m_PokemonPositionSeq = 0;
    /// 位置IDの初期化
    m_PokemonPositionID = 0;
    /// 回転IDの初期化
    m_PokemonRotationID = 0;

    this->SetSubSeq( POKEMON_SEQ_CHECK_BTL_POSITION_END );
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_BTL_POSITION_END )
  {
    /// 再生可能な全モーションが再生できるのかをチェック
    if( _CheckAvailablePokemonMotion() )
    {
      this->SetSubSeq( POKEMON_SEQ_CHECK_BTL_MOTION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_BTL_MOTION )
  {
    /// シーケンスの初期化
    m_PokemonMotionSeq = 0;
    /// モーションIDの初期化
    m_PokemonMotionID = 0;

    this->SetSubSeq( POKEMON_SEQ_CHECK_BTL_MOTION_END );
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_BTL_MOTION_END )
  {
    /// バトルモーション全チェック
    if( _CheckPokemonAllMotion() )
    {
      this->SetSubSeq( POKEMON_SEQ_SETUP_RFR_MOTION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_SETUP_RFR_MOTION )
  {
    /// 可愛がりモーションへ変更
    _RequestChanePokeMotionType_Kawaigari();

    this->SetSubSeq( POKEMON_SEQ_SETUP_RFR_MOTION_END );
  }
  else if( m_SubSeq == POKEMON_SEQ_SETUP_RFR_MOTION_END )
  {
    if( _IsEndChangePokeMotionType() )
    {
      this->SetSubSeq( POKEMON_SEQ_CHECK_RFR_POSITION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_RFR_POSITION )
  {
    /// シーケンスの初期化
    m_PokemonPositionSeq = 0;
    /// 位置IDの初期化
    m_PokemonPositionID = 0;
    /// 回転IDの初期化
    m_PokemonRotationID = 0;

    this->SetSubSeq( POKEMON_SEQ_CHECK_RFR_POSITION_END );
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_RFR_POSITION_END )
  {
    /// 再生可能な全モーションが再生できるのかをチェック
    if( _CheckAvailablePokemonMotion() )
    {
      this->SetSubSeq( POKEMON_SEQ_CHECK_RFR_MOTION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_RFR_MOTION )
  {
    /// シーケンスの初期化
    m_PokemonMotionSeq = 0;
    /// モーションIDの初期化
    m_PokemonMotionID = 0;

    /// 可愛がりモーションがあるか
    if( _IsAvailableKawaigariMotion() )
    {
      this->SetSubSeq( POKEMON_SEQ_CHECK_RFR_MOTION_END );
    }
    else {
      this->SetSubSeq( POKEMON_SEQ_CHECK_SLEEP_MOTION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_RFR_MOTION_END )
  {
    /// 可愛がりモーション全チェック
    if( _CheckPokemonAllMotion() )
    {
      this->SetSubSeq( POKEMON_SEQ_CHECK_SLEEP_MOTION );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_SLEEP_MOTION )
  {
    /// 眠りシーケンスの初期化
    m_PokemonSleepMotionSeq = 0;
    /// 眠り待機フレーム
    m_PokemonSleepWaitFrame = 0;

    /// 眠りモーションがあるか
    if( _IsAvailableSleepMotion() )
    {
      this->SetSubSeq( POKEMON_SEQ_CHECK_SLEEP_MOTION_END );
    }
    else {
      this->SetSubSeq( POKEMON_SEQ_FIN_RARE );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_CHECK_SLEEP_MOTION_END )
  {
    /// 眠りモーションのチェック
    if( _CheckPokemonSleepMotion() )
    {
      this->SetSubSeq( POKEMON_SEQ_FIN_RARE );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_FIN_RARE )
  {
    /// TRUEなら性別の終了チェックへ
    if( m_bPokemonRare )
    {
      this->SetSubSeq( POKEMON_SEQ_FIN_SEX );
    }
    /// FALSEならTRUEに変更してポケモンを変更しなおす
    else {
      m_bPokemonRare = true;
      this->SetSubSeq( POKEMON_SEQ_INIT_RARE );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_FIN_SEX )
  {
    /// 性別タイプが固定か男の子以外ならフォルムの終了チェックへ
    if( m_PokemonSexType == pml::personal::SEX_TYPE_FIX || m_PokemonSex != pml::SEX_MALE )
    {
      this->SetSubSeq( POKEMON_SEQ_FIN_FORMNO );
    }
    /// 男の子なら女の子に変更してポケモンを変更しなおす
    else {
      m_PokemonSex = pml::SEX_FEMALE;
      m_bPokemonRare = false;
      this->SetSubSeq( POKEMON_SEQ_INIT_SEX );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_FIN_FORMNO )
  {
    m_PokemonFormNo++;

    /// フォルムが最大ならMonsNoの終了チェックへ
    if( m_PokemonFormNo >= m_PokemonFormMax )
    {
      this->SetSubSeq( POKEMON_SEQ_FIN_MONSNO );
    }
    /// それ以外ならポケモンを変更しなおす
    else {
      m_PokemonSex = _GetBaseSex( m_PokemonSimpleParam.monsNo );
      m_bPokemonRare = false;
      this->SetSubSeq( POKEMON_SEQ_INIT_FORMNO );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_FIN_MONSNO )
  {
    m_PokemonMonsNo++;

    /// MonsNoが最大値超なら終了処理へ
    if( m_PokemonMonsNo > m_PokemonFinishMonsNo )
    {
      this->SetSubSeq( POKEMON_SEQ_END );
    }
    /// それ以外ならポケモンを変更しなおす
    else {
      m_PokemonFormNo = 0;
      m_PokemonSex = pml::SEX_MALE;
      m_bPokemonRare = false;
      this->SetSubSeq( POKEMON_SEQ_INIT_SIMPLEPARAM );
    }
  }
  else if( m_SubSeq == POKEMON_SEQ_END )
  {
    this->SetSubSeq( POKEMON_SEQ_INIT );
  }
}

/**
 * @brief   更新：トレーナー
 */
void DebugAgingFrame::_UpdateSequence_Trainer( void )
{
  enum {
    TRAINER_SEQ_MOTION_START,
    TRAINER_SEQ_MOTION_WAIT,
    TRAINER_SEQ_MOTION_PLAYING,
    TRAINER_SEQ_MOTION_END,
    TRAINER_SEQ_CHECK_END,
    TRAINER_SEQ_END,
    TRAINER_SEQ_DRESSUP_START,
    TRAINER_SEQ_DRESSUP_END,
  };

  if( m_SubSeq == TRAINER_SEQ_MOTION_START )
  {
    /// Zモーションの再生
    _RequestPlayTrainerMotion();
    
    this->SetSubSeq( TRAINER_SEQ_MOTION_WAIT );
  }
  else if( m_SubSeq == TRAINER_SEQ_MOTION_WAIT )
  {
    this->SetSubSeq( TRAINER_SEQ_MOTION_PLAYING );
  }
  else if( m_SubSeq == TRAINER_SEQ_MOTION_PLAYING )
  {
    if( !_IsEndTrainerMotion() )
    {
      this->SetSubSeq( TRAINER_SEQ_MOTION_END );
    }
  }
  else if( m_SubSeq == TRAINER_SEQ_MOTION_END )
  {
    /// エージングが停止されたので終了する
    if( !_IsAgingStartTrainer() )
    {
      this->SetSubSeq( TRAINER_SEQ_END );
      return;
    }

    m_TrainerZMotionType++;
    
    if( m_TrainerZMotionType < Viewer::Z_MOTION_TYPE_MAX )
    {
      this->SetSubSeq( TRAINER_SEQ_MOTION_START );
    }
    else {
      this->SetSubSeq( TRAINER_SEQ_CHECK_END );
    }
  }
  else if( m_SubSeq == TRAINER_SEQ_CHECK_END )
  {
    u8 mode = _GetAgingTrainerMode();
    
    /// 両方
    if( mode == 0 )
    {
      /// 現在の性別が男の子なら女の子に変更してもう一度モーションを再生させる
      if( m_TrainerDressUpParamSex == poke_3d::model::DressUpParam::SEX_MALE )
      {
        this->SetSubSeq( TRAINER_SEQ_DRESSUP_START );
      }
      else {
        this->SetSubSeq( TRAINER_SEQ_END );
      }
    }
    /// それ以外
    else {
      this->SetSubSeq( TRAINER_SEQ_END );
    }
  }
  else if( m_SubSeq == TRAINER_SEQ_END )
  {
    this->SetMainSeq( SEQ_INIT );
  }
  else if( m_SubSeq == TRAINER_SEQ_DRESSUP_START )
  {
    /// Zモーションタイプの初期化
    m_TrainerZMotionType = Viewer::Z_MOTION_TYPE_NORMAL;
    /// 女の子に変更
    m_TrainerDressUpParamSex = poke_3d::model::DressUpParam::SEX_FEMALE;

    /// 着せ替えデータの変更
    _RequestChangeDressUp();

    this->SetSubSeq( TRAINER_SEQ_DRESSUP_END );
  }
  else if( m_SubSeq == TRAINER_SEQ_DRESSUP_END )
  {
    /// Viewer更新完了待ち
    if( m_pViewerManager->IsFinishUpdadteViewer() )
    {
      this->SetSubSeq( TRAINER_SEQ_MOTION_START );
    }
  }
}

/**
 * @brief   更新：背景
 */
void DebugAgingFrame::_UpdateSequence_BackGround( void )
{
  if( m_SubSeq == 0 )
  {
    /// カメラ操作
    _BackGroundKeyAction();

    /// 終了チェック
    if( _IsEndBackGround() )
    {
      this->SetSubSeq( 1 );
    }
  }
  if( m_SubSeq == 1 )
  {
    /// エージングが停止されたので終了する
    if( !_IsAgingStartBackGround() )
    {
      this->SetSubSeq( 2 );
      return;
    }

    /// 次の時間帯の取得
    m_BackGroundTimeZone = _GetAgingBackGroundNextTimeZone( m_BackGroundTimeZone );
    GFL_RELEASE_PRINT( "TimeZone=[%d]\n", m_BackGroundTimeZone );

    if( m_BackGroundTimeZone == Viewer::TIME_ZONE_MAX )
    {
      this->SetSubSeq( 2 );
    }
    else if( _IsAvailableTimeZone( m_BackGroundID, m_BackGroundTimeZone ) )
    {
      this->SetSubSeq( 3 );
    }
  }
  if( m_SubSeq == 2 )
  {
    m_BackGroundCount++;
    /// 次の背景へ
    this->SetMainSeq( SEQ_END );
  }
  if( m_SubSeq == 3 )
  {
    m_BackGroundWaitTime = 0;
    /// 時間帯変更
    m_pViewerManager->RequestViewerUpdate_TimeZone( m_BackGroundTimeZone );
    /// 次の時間帯へ
    this->SetSubSeq( 0 );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ポケモン：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ポケモンの変更
 */
void DebugAgingFrame::_RequestChangePokeModel( void )
{
  /// リクエスト：ポケモン変更
  _RequestChangePokeModel( m_PokemonSimpleParam );
}

/**
 * @brief   ポケモンの変更
 *
 * @param[in]   param   PokeTool::SimpleParam
 */
void DebugAgingFrame::_RequestChangePokeModel( PokeTool::SimpleParam& param )
{
  /// リクエスト：ポケモン変更
  m_pViewerManager->RequestViewerUpdate_PokeModel( param );
  m_pViewerManager->StartUpdateViewer();

  FUKUSHIMA_PRINT( "Pokemon : MonsNo=[%d] FormNo=[%d] Sex=[%d] Rare=[%d]\n", param.monsNo, param.formNo, param.sex, param.isRare );
}


/**
 * @brief   ポケモンモーションタイプの変更：バトル
 */
void DebugAgingFrame::_RequestChanePokeMotionType_Battle( void )
{
  /// バトルモーションへ変更
  _RequestChanePokeMotionType( Viewer::POKE_MOTION_TYPE_BATTLE );
}

/**
 * @brief   ポケモンモーションタイプの変更：可愛がり
 */
void DebugAgingFrame::_RequestChanePokeMotionType_Kawaigari( void )
{
  /// 可愛がりモーションへ変更
  _RequestChanePokeMotionType( Viewer::POKE_MOTION_TYPE_KAWAIGARI );
}

/**
 * @brief   ポケモンモーションタイプの変更
 *
 * @param[in]   type    モーションタイプ
 */
void DebugAgingFrame::_RequestChanePokeMotionType( Viewer::PokeMotionType type )
{
  /// リクエスト：Viewerリセット
  m_pViewerManager->RequestViewerUpdate_AllReset();
  /// リクエスト：ポケモンモーションタイプの変更
  m_pViewerManager->RequestViewerUpdate_PokeMotionType( type, true );
  m_pViewerManager->StartUpdateViewer();

  m_PokemonMotionType = type;

  FUKUSHIMA_PRINT( "PokemonMotion : Type=[%d]\n", type );
}


/**
 * @brief   再生可能な全モーションが再生できるのかをチェック
 *
 * @note    再生可能割合が１００％の場所を探す。
 *          場所は主にトレーナーを中心に上下左右で探す
 *          場所だけでは難しいポケモンがいるかもしれないので回転も４方向チェック
 */
bool DebugAgingFrame::_CheckAvailablePokemonMotion( void )
{
  /// 移動場所の順番は左、上、右、下
  /// 回転方向は正面、左、上、右
  /// 全部に当てはまらないポケモンがいる場合は、とりあえずASSERTにしておく
  /// 場所、回転を設定したら再生可能割合で割合が１００％かをチェック
  /// １００％でなければ場所、回転を行う
  enum {
    POS_SEQ_POS,
    POS_SEQ_POS_RATE,

    POS_SEQ_ROT,
    POS_SEQ_ROT_END,
    POS_SEQ_ROT_NEXT,
    POS_SEQ_ROT_RATE,

    POS_SEQ_SUCCESS,
  };

  if( m_PokemonPositionSeq == POS_SEQ_POS )
  {
    /// ポケモン配置
    if( _PokemonPlacement() )
    {
      m_PokemonPositionSeq = POS_SEQ_POS_RATE;
    }
    else {
      /// 配置できない 回転チェックへ
      m_PokemonPositionSeq = POS_SEQ_ROT;
    }
  }
  else if( m_PokemonPositionSeq == POS_SEQ_POS_RATE )
  {
    /// モーション再生割合
    if( _GetPlayableRatePokemonMotion() >= 1.0f )
    {
      /// 再生可能
      m_PokemonPositionSeq = POS_SEQ_SUCCESS;
    }
    else {
      /// 再生できないのがある 回転チェックへ
      m_PokemonPositionSeq = POS_SEQ_ROT;
    }
  }
  else if( m_PokemonPositionSeq == POS_SEQ_ROT )
  {
    /// 回転IDの初期化
    m_PokemonRotationID = 0;
    m_PokemonPositionSeq = POS_SEQ_ROT_END;
  }
  else if( m_PokemonPositionSeq == POS_SEQ_ROT_END )
  {
    /// ポケモン回転
    if( _PokemonAvailableRotate() )
    {
      m_PokemonPositionSeq = POS_SEQ_ROT_RATE;
    }
    else {
      /// 次の回転IDへ
      m_PokemonPositionSeq = POS_SEQ_ROT_NEXT;
    }
  }
  else if( m_PokemonPositionSeq == POS_SEQ_ROT_NEXT )
  {
    /// 次の回転IDへ
    m_PokemonRotationID++;
    /// 最大値ならアサートにする
    if( m_PokemonRotationID >= 4 )
    {
      GFL_ASSERT( 0 );
    }
    else {
      m_PokemonPositionSeq = POS_SEQ_ROT_END;
    }
  }
  else if( m_PokemonPositionSeq == POS_SEQ_ROT_RATE )
  {
    /// モーション再生割合
    if( _GetPlayableRatePokemonMotion() >= 1.0f )
    {
      /// 再生可能
      m_PokemonPositionSeq = POS_SEQ_SUCCESS;
    }
    else {
      /// 次の回転IDへ
      m_PokemonPositionSeq = POS_SEQ_ROT_NEXT;
    }
  }
  else if( m_PokemonPositionSeq == POS_SEQ_SUCCESS )
  {
    return true;
  }

  return false;
}

/**
 * @brief   再生可能な全モーションの再生チェック
 */
bool DebugAgingFrame::_CheckPokemonAllMotion( void )
{
  /// 再生するモーションリストを取得
  /// モーションを再生する
  /// モーションの終了をチェック
  /// 全モーション再生が完了したらtrueを返す
  enum {
    MOTION_SEQ_LIST,
    MOTION_SEQ_PLAY,
    MOTION_SEQ_PLAYING,
    MOTION_SEQ_END,
  };

  if( m_PokemonMotionSeq == MOTION_SEQ_LIST )
  {
    /// 再生をするモーションリストを設定
    _SetAvailableMotionList();
    m_PokemonMotionSeq = MOTION_SEQ_PLAY;
  }
  else if( m_PokemonMotionSeq == MOTION_SEQ_PLAY )
  {
    /// 再生するモーションIDを取得
    m_PokemonMotion = _GetPlayMotionID();
    /// 待機フレームを初期化
    m_MotionWaitFrame = 0;

    if( m_PokemonMotion != PokeTool::MODEL_ANIME_ERROR )
    {
      /// ポケモンモーションの再生
      _PlayPokemonMotion( m_PokemonMotion );
      m_PokemonMotionSeq = MOTION_SEQ_PLAYING;
    }
    else {
      m_PokemonMotionID = m_MotionListMax;
      m_PokemonMotionSeq = MOTION_SEQ_END;
    }
  }
  else if( m_PokemonMotionSeq == MOTION_SEQ_PLAYING )
  {
    /// モーション終了チェック
    if( _IsFinishPokeMotion() )
    {
      m_PokemonMotionSeq = MOTION_SEQ_END;
    }
  }
  else if( m_PokemonMotionSeq == MOTION_SEQ_END )
  {
    if( m_PokemonMotionID >= m_MotionListMax )
    {
      return true;
    }
    else {
      m_PokemonMotionSeq = MOTION_SEQ_PLAY;
    }
  }

  return false;
}

/**
 * @brief   眠りモーションのチェック
 */
bool DebugAgingFrame::_CheckPokemonSleepMotion( void )
{
  enum {
    SLEEP_SEQ_PLAY_SLEEP,
    SLEEP_SEQ_WAIT_SLEEP,
    SLEEP_SEQ_PLAY_WAKE,
    SLEEP_SEQ_WAIT_WAKE,
    SLEEP_SEQ_END,
    SLEEP_SEQ_CHANGE_POS,
  };

  if( m_PokemonSleepMotionSeq == SLEEP_SEQ_PLAY_SLEEP )
  {
    /// 眠りモーションの再生
    if( _PlayPokemonMotionSleep() )
    {
      FUKUSHIMA_PRINT("PlaySleep\n");
      m_PokemonSleepMotionSeq = SLEEP_SEQ_WAIT_SLEEP;
    }
    /// 再生できなかった
    else {
      m_PokemonSleepMotionSeq = SLEEP_SEQ_CHANGE_POS;
    }
  }
  else if( m_PokemonSleepMotionSeq == SLEEP_SEQ_WAIT_SLEEP )
  {
    if( m_PokemonSleepWaitFrame < 120 )
    {
      m_PokemonSleepWaitFrame++;
    }
    else {
      /// 起きれるかチェック
      if( _IsAvailableWake() )
      {
        m_PokemonSleepMotionSeq = SLEEP_SEQ_PLAY_WAKE;
      }
    }
  }
  else if( m_PokemonSleepMotionSeq == SLEEP_SEQ_PLAY_WAKE )
  {
    /// 起きる
    _PlayPokemonMotionWake();
    FUKUSHIMA_PRINT( "PlayWake\n" );
    m_PokemonSleepMotionSeq = SLEEP_SEQ_WAIT_WAKE;
  }
  else if( m_PokemonSleepMotionSeq == SLEEP_SEQ_WAIT_WAKE )
  {
    /// 起きるモーションの終了チェック
    if( _IsFinishPokeMotion() )
    {
      m_PokemonSleepMotionSeq = SLEEP_SEQ_END;
    }
  }
  else if( m_PokemonSleepMotionSeq == SLEEP_SEQ_END )
  {
    /// 眠りモーションシーケンス終了
    return true;
  }
  else if( m_PokemonSleepMotionSeq == SLEEP_SEQ_CHANGE_POS )
  {
    /// 位置変更
    if( _PokemonPlacement() )
    {
      m_PokemonSleepMotionSeq = SLEEP_SEQ_PLAY_SLEEP;
    }
  }

  return false;
}


/**
 * @brief   パーソナルデータの読込み
 */
void DebugAgingFrame::_LoadPersonalData( MonsNo monsNo )
{
  // パーソナルデータの読込み
  _LoadPersonalData( monsNo, 0 );
}

/**
 * @brief   パーソナルデータの読込み
 */
void DebugAgingFrame::_LoadPersonalData( MonsNo monsNo, pml::FormNo formNo )
{
  // パーソナルデータの読込み
  pml::personal::LoadPersonalData( monsNo, formNo );
}


/**
 * @brief   MonsNoの取得
 */
u32 DebugAgingFrame::_GetMonsNo( void )
{
  u8 mode = DebugClass::GetAging_PokemonMode();

  if( mode == 0 )
  {
    return m_PokemonMonsNo;
  }
  else {
    return (_GetRandomNum() % (MONSNO_END-1)) + 1;
  }
}

/**
 * @brief   Form最大数の取得
 *
 * @note    予め_LoadPersonalDataで読み込んでおく
 */
pml::FormNo DebugAgingFrame::_GetFormMax( MonsNo monsNo )
{
  const struct FormData
  {
    MonsNo monsNo;
    pml::FormNo formMax;
  } c_FormDataList[] = {
    { MONSNO_KOHUKIMUSI, FORMNO_BIBIYON_MAX },    //!< コフキムシ
    { MONSNO_KOHUURAI, FORMNO_BIBIYON_MAX },      //!< コフーライ
    { MONSNO_GAAMEIRU, FORMNO_MINOMUTTI_MAX },    //!< ガーメイル
  };

  for( u32 i = 0; i < GFL_NELEMS( c_FormDataList ); ++i )
  {
    /// データリストにあるMonsNoが一致していたらそのフォルム最大数を返す
    if( c_FormDataList[i].monsNo == monsNo )
    {
      return c_FormDataList[i].formMax;
    }
  }

  /// データリストに無い場合はパーソナルから値を取得して返す
  return pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max );
}

/**
 * @brief   基本性別の取得
 */
pml::Sex DebugAgingFrame::_GetBaseSex( MonsNo monsNo )
{
  pml::Sex sex = pml::SEX_MALE;

  gfl2::heap::HeapBase* heap = m_pMainWork->heap->GetDeviceHeap()->GetLowerHandle();
  {
    /// PokemonParamを生成
    pml::pokepara::PokemonParam* param = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, monsNo, 100, 0 );
    {
      /// 性別の取得
      sex = param->GetSex();
    }
    GFL_DELETE( param );
  }

  return sex;
}

/**
 * @brief   性別タイプの取得
 *
 * @note    予め_LoadPersonalDataで読み込んでおく
 */
pml::personal::SexType DebugAgingFrame::_GetSexType( MonsNo monsNo )
{
  /// SexTypeの取得
  return pml::personal::GetPersonalSexType();
}


/**
 * @brief   ポケモンの配置
 *
 * @retval  "true"なら配置可能
 * @retval  "false"ならそれ以外
 */
bool DebugAgingFrame::_PokemonPlacement( void )
{
  /// 位置座標テーブル
  const gfl2::math::VEC3 PokemonPositionTable[] = {
    gfl2::math::VEC3( -120.f, 0.f, 0.f ),   // 左
    gfl2::math::VEC3( 0.f, 120.f, 0.f ),    // 上
    gfl2::math::VEC3( 120.f, 0.f, 0.f ),    // 右
    gfl2::math::VEC3( 0.f, -120.f, 0.f ),   // 下
  };

  gfl2::math::VEC3 position = PokemonPositionTable[m_PokemonPositionID];

  return _PokemonPlacement( position );
}

/**
 * @brief   ポケモンの配置
 *
 * @retval  "true"なら配置可能
 * @retval  "false"ならそれ以外
 */
bool DebugAgingFrame::_PokemonPlacement( gfl2::math::VEC3 pos )
{
  /// 位置座標の調整
  _AdjustPokemonPosition( &pos );

  /// スクリーン座標に変換
  gfl2::math::Vector3 vecScreen( pos.x + 160.0f, 120.0f - pos.y, 0.0f );

  /// ポケモンを配置
  if( m_pViewerManager->SetPokemonPosition( vecScreen ) )
  {
    /// 衝突
    FUKUSHIMA_PRINT( "Collision!! : X=[%.1f] Y=[%.1f]\n", vecScreen.x, vecScreen.y );
    return false;
  }

  return true;
}

/**
 * @brief   位置座標の調整
 */
void DebugAgingFrame::_AdjustPokemonPosition( gfl2::math::VEC3* pos )
{
  /// X座標
  if( pos->x < ( Static::UI_MOVE_RADIUS * -1.0f ) )
  {
    pos->x = ( Static::UI_MOVE_RADIUS * -1.0f );
  }
  if( pos->x > Static::UI_MOVE_RADIUS )
  {
    pos->x = Static::UI_MOVE_RADIUS;
  }
  /// Y座標
  if( pos->y < ( Static::UI_MOVE_RADIUS * -1.0f ) )
  {
    pos->y = ( Static::UI_MOVE_RADIUS * -1.0f );
  }
  if( pos->y > Static::UI_MOVE_RADIUS )
  {
    pos->y = Static::UI_MOVE_RADIUS;
  }
}

/**
 * @brief   ポケモンの回転
 */
bool DebugAgingFrame::_PokemonAvailableRotate( void )
{
  /// 現在の角度を取得
  f32 degree = m_pViewerManager->GetPokemonYAxisRotationDegree();

  /// 90度回転させる
  degree += 90.0f;
  if( degree >= 360.0f ) degree -= 360.0f;

  /// 角度をを設定
  if( m_pViewerManager->SetPokemonYAxisRotationDegree( degree ) )
  {
    /// 衝突
    FUKUSHIMA_PRINT( "Collision!! : Deg=[%.1f]\n", degree );
    return false;
  }

  return true;
}


/**
 * @brief   再生可能割合の取得
 */
f32 DebugAgingFrame::_GetPlayableRatePokemonMotion( void )
{
  /// 再生可能割合の取得
  f32 rate = m_pViewerManager->GetPokemonMotionPlayableRate();

  FUKUSHIMA_PRINT( "PlayableRate=[%.1f]\n", rate );

  return rate;
}


/**
 * @brief   再生可能なモーションリストの設定
 */
void DebugAgingFrame::_SetAvailableMotionList( void )
{
  Viewer::FinderStudioViewer* viewer = m_pViewerManager->GetFinderStudioViewer();

  if( m_PokemonMotionType == Viewer::POKE_MOTION_TYPE_BATTLE )
  {
    m_pPokemonMotionList = viewer->DebugGetPokeCanPlayBattleMotionList();
    m_MotionListMax = PokeTool::MODEL_ANIME_BATTLE_NUM;
  }
  else {
    m_pPokemonMotionList = viewer->DebugGetPokeCanPlayKawaigariMotionList();
    m_MotionListMax = PokeTool::MODEL_ANIME_KAWAIGARI_NUM;
  }

  m_PokemonMotion = PokeTool::MODEL_ANIME_ERROR;
}

/**
 * @brief   再生するモーションIDを取得
 */
PokeTool::MODEL_ANIME DebugAgingFrame::_GetPlayMotionID( void )
{
  PokeTool::MODEL_ANIME modelAnim = PokeTool::MODEL_ANIME_ERROR;

  for( u32 i = m_PokemonMotionID; i < m_MotionListMax; ++i )
  {
    modelAnim = m_pPokemonMotionList[i];

    if( modelAnim != PokeTool::MODEL_ANIME_ERROR )
    {
      m_PokemonMotionID = i + 1;
      break;
    }
  }

  return modelAnim;
}

/**
 * @brief   ポケモンモーションを再生
 */
void DebugAgingFrame::_PlayPokemonMotion( PokeTool::MODEL_ANIME modelAnime )
{
  if( m_PokemonMotionType == Viewer::POKE_MOTION_TYPE_BATTLE )
  {
    m_pViewerManager->GetFinderStudioViewer()->DebugPlayPokeBattleMotion( modelAnime );
  }
  else {
    m_pViewerManager->GetFinderStudioViewer()->DebugPlayPokeKawaigariMotion( modelAnime );
  }

  FUKUSHIMA_PRINT( "PlayMotion : ID=[%d]\n", modelAnime );
}


/**
 * @brief   可愛がりが使用可能か
 */
bool DebugAgingFrame::_IsAvailableKawaigariMotion( void )
{
  MonsNo monsNo = static_cast<MonsNo>( m_PokemonMonsNo );
  pml::FormNo formNo = m_PokemonFormNo;

  /// 可愛がり使用可否のチェック
  m_pViewerManager->GetFinderStudioViewer()->DebugCheckMonsNoFormNoCombination( monsNo, &formNo );

  return m_PokemonFormNo == formNo;
}

/**
 * @brief   眠りが使用可能か
 */
bool DebugAgingFrame::_IsAvailableSleepMotion( void )
{
  return m_pViewerManager->GetFinderStudioViewer()->DebugIsPokeCanSleep();
}

/**
 * @brief   ポケモンモーションの終了確認
 */
bool DebugAgingFrame::_IsFinishPokeMotion( void )
{
  enum {
    FIN_MODE_VIEWER,    //!< 終了モード：Viewer
    FIN_MODE_FRAME,     //!< 終了モード：フレーム
    FIN_MODE_NUM,
  };

  struct MOTION_DATA
  {
    Viewer::PokeMotionType type;
    PokeTool::MODEL_ANIME anime;
  } PokeWaitMotionTable[] = {
    { Viewer::POKE_MOTION_TYPE_BATTLE, PokeTool::MODEL_ANIME_BT_WAIT_A },
    { Viewer::POKE_MOTION_TYPE_BATTLE, PokeTool::MODEL_ANIME_BT_WAIT_B },
    { Viewer::POKE_MOTION_TYPE_BATTLE, PokeTool::MODEL_ANIME_BT_WAIT_C },
    { Viewer::POKE_MOTION_TYPE_KAWAIGARI, PokeTool::MODEL_ANIME_KW_WAIT },
  };

  u8 mode = FIN_MODE_VIEWER;

  for( u32 i = 0; i < GFL_NELEMS( PokeWaitMotionTable ); ++i )
  {
    MOTION_DATA data = PokeWaitMotionTable[i];

    if( data.type == m_PokemonMotionType && data.anime == m_PokemonMotion )
    {
      mode = FIN_MODE_FRAME;
    }
  }

  if( mode == FIN_MODE_VIEWER )
  {
    /// モーションの終了をチェック
    return m_pViewerManager->GetFinderStudioViewer()->IsFinishPokeMotion();
  }
  else {
    /// 待機モーションはフレームで終了をチェック
    return ++m_MotionWaitFrame > 20;
  }
}


/**
 * @brief   眠りモーションの再生
 *
 * @retval  "true"なら眠る
 * @retval  "false"なら失敗
 */
bool DebugAgingFrame::_PlayPokemonMotionSleep( void )
{
  return m_pViewerManager->GetFinderStudioViewer()->DebugSleepPoke();
}

/**
 * @brief   起きれるか？
 *
 * @retval  "true"なら起きれる
 * @retval  "false"なら起きれない
 */
bool DebugAgingFrame::_IsAvailableWake( void )
{
  return m_pViewerManager->GetFinderStudioViewer()->DebugIsPokeCanWake();
}

/**
 * @brief   起きる
 */
void DebugAgingFrame::_PlayPokemonMotionWake( void )
{
  m_pViewerManager->GetFinderStudioViewer()->DebugWakePoke();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    トレーナー：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   着せ替えパラメータの取得
 */
poke_3d::model::DressUpParam DebugAgingFrame::_GetDressUpParam( void )
{
  u8 mode = _GetAgingTrainerMode();
  
  if( mode == 0 )
  {
    if( m_TrainerDressUpParamSex == poke_3d::model::DressUpParam::SEX_MALE )
    {
      return _GetDressUpParam_Male();
    }
    else {
      return _GetDressUpParam_Female();
    }
  }
  else if( mode == 1 )
  {
    return _GetDressUpParam_Male();
  }
  else if( mode == 2 )
  {
    return _GetDressUpParam_Female();
  }

  GFL_ASSERT( 0 );
  return _GetDressUpParam_Female();
}

/**
 * @brief   着せ替えパラメータの取得：男の子
 */
poke_3d::model::DressUpParam DebugAgingFrame::_GetDressUpParam_Male( void )
{
  poke_3d::model::DressUpParam param;
  {
    /// デフォルト衣装パラメータの取得
    System::DressUp::GetDressUpParamDefaultHero( &param );
    
    /// 衣装はランダムで設定する
    {
      /// ボディカテゴリ
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR]       = _GetRandomNum( DRESSUP_COLOR_ID_SKIN_COUNT );
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR]        = _GetRandomNum( DRESSUP_COLOR_ID_EYE_COUNT );
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR]       = _GetRandomNum( DRESSUP_COLOR_ID_HAIR_COUNT );
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;

      /// アイテムカテゴリ
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE]  = _GetRandomNum( DRESSUP_ITEM_ID_HERO_HAIR_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE]      = _GetRandomNum( DRESSUP_ITEM_ID_HERO_ACCEYE_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR]     = -1;
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG]         = _GetRandomNum( DRESSUP_ITEM_ID_HERO_BAG_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS]     = _GetRandomNum( DRESSUP_ITEM_ID_HERO_BOTTOMS_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT]         = _GetRandomNum( DRESSUP_ITEM_ID_HERO_HAT_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS]        = _GetRandomNum( DRESSUP_ITEM_ID_HERO_LEGS_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES]       = _GetRandomNum( DRESSUP_ITEM_ID_HERO_SHOES_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS]        = _GetRandomNum( DRESSUP_ITEM_ID_HERO_TOPS_COUNT );
    }
  }
  
  /// 不正チェックを通したパラメータを返す
  return _FraudCheckDressUpParam( param );
}

/**
 * @brief   着せ替えパラメータの取得：女の子
 */
poke_3d::model::DressUpParam DebugAgingFrame::_GetDressUpParam_Female( void )
{
  poke_3d::model::DressUpParam param;
  {
    /// デフォルト衣装パラメータの取得
    System::DressUp::GetDressUpParamDefaultHeroine( &param );
    
    /// 衣装はランダムで設定する
    {
      /// ボディカテゴリ
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR]       = _GetRandomNum( DRESSUP_COLOR_ID_SKIN_COUNT );
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR]        = _GetRandomNum( DRESSUP_COLOR_ID_EYE_COUNT );
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR]       = _GetRandomNum( DRESSUP_COLOR_ID_HAIR_COUNT );
      param.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;

      /// アイテムカテゴリ
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE]  = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_HAIR_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE]      = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_ACCEYE_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR]     = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_ACCHAIR_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG]         = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_BAG_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS]     = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_BOTTOMS_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT]         = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_HAT_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS]        = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_LEGS_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES]       = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_SHOES_COUNT );
      param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS]        = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_TOPS_COUNT );
    }
  }
  
  /// 不正チェックを通したパラメータを返す
  return _FraudCheckDressUpParam( param );
}

/**
 * @brief   不正チェック：着せ替えデータ
 */
poke_3d::model::DressUpParam DebugAgingFrame::_FraudCheckDressUpParam( poke_3d::model::DressUpParam param )
{
  poke_3d::model::DressUpParam dressUpParam = param;

  /// ランダムで帽子、アクセサリー、全力リングを外す
  {
    u32 rand = 0;

    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
    }
    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;
    }
    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
    }
    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = -1;
    }
  }
  /// 不正チェック
  {
    gfl2::heap::HeapBase* heap = m_pMainWork->heap->GetDeviceHeap();

    /// 生成
    poke_3d::model::DressUpModelResourceManager* dressupResourceManager = GFL_NEW_LOW( heap ) poke_3d::model::DressUpModelResourceManager();
    dressupResourceManager->Initialize( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), heap, System::DressUp::GetDressUpArcIdListField() );

    const poke_3d::model::DressUpModelResourceManagerCore* dressupResourceManagerCore = NULL;
    {
      if( dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == poke_3d::model::DressUpParam::SEX_MALE )
      {
        dressupResourceManagerCore = dressupResourceManager->GetDressUpModelResourceManagerCore( poke_3d::model::DressUpParam::SEX_MALE );
      }
      else {
        dressupResourceManagerCore = dressupResourceManager->GetDressUpModelResourceManagerCore( poke_3d::model::DressUpParam::SEX_FEMALE );
      }
    }

    /// 帽子がかぶれない髪型なら帽子を外す
    if( !dressupResourceManagerCore->CanDressUpHairWearHat( dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE] ) )
    {
      dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
    }
    /// 帽子をかぶっているならヘアアクセサリーは外す
    if( dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] >= 0 )
    {
      dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
    }

    /// 念のため組み合わせチェックをして問題があればフィールド衣装に戻す
    if( !dressupResourceManagerCore->CheckDressUpCombination( dressUpParam ) )
    {
      dressUpParam = DebugClass::GetFieldDressup();
    }

    /// 破棄
    dressupResourceManager->Finalize();
    GFL_SAFE_DELETE( dressupResourceManager );
  }
  
  return dressUpParam;
}


/**
 * @brief   着せ替えの変更リクエスト
 */
void DebugAgingFrame::_RequestChangeDressUp( void )
{
  /// 着せ替えデータ
  poke_3d::model::DressUpParam dressupParam = _GetDressUpParam();

  /// 変更リクエスト
  _RequestChangeDressUp( dressupParam );
}

/**
 * @brief   着せ替えの変更リクエスト
 *
 * @param[in]   param   着せ替えデータ
 */
void DebugAgingFrame::_RequestChangeDressUp( poke_3d::model::DressUpParam param )
{
  /// リクエスト：着せ替え
  m_pViewerManager->RequestViewerUpdate_Dressup( param );
  /// Viewer更新の開始
  m_pViewerManager->StartUpdateViewer();

  for( u32 i = 0; i < poke_3d::model::DressUpParam::BODY_CATEGORY_COUNT; ++i )
  {
    GFL_RELEASE_PRINT( "bodyParam[i]=[%d]\n", i, param.bodyParams[i] );
  }
  for( u32 i = 0; i < poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i )
  {
    GFL_RELEASE_PRINT( "itemParam[i]=[%d]\n", i, param.itemParams[i] );
  }
}

/**
 * @brief   トレーナーモーション再生
 */
void DebugAgingFrame::_RequestPlayTrainerMotion( void )
{
  Viewer::ZMotionType motionType = static_cast<Viewer::ZMotionType>( m_TrainerZMotionType );

  /// トレーナーモーションの再生リクエスト
  _RequestPlayTrainerMotion( motionType );

  GFL_RELEASE_PRINT( "ZMotionType=[%d]\n", m_TrainerZMotionType );
}

/**
 * @brief   トレーナーモーション再生
 *
 * @param[in]   type    トレーナーモーションタイプ
 */
void DebugAgingFrame::_RequestPlayTrainerMotion( Viewer::ZMotionType type )
{
  /// トレーナーモーションの再生リクエスト
  m_pViewerManager->RequestTrainerMotionPlay( type );
}

/**
 * @brief   トレーナーモーションが再生中か？
 *
 * @retval  "true"なら再生中
 * @retval  "false"ならそれ以外
 */
bool DebugAgingFrame::_IsEndTrainerMotion( void )
{
  return m_pViewerManager->IsPlayingTrainerMotion();
}


/**
 * @brief   トレーナー：モードの取得
 *
 * @retval  "0"なら両方
 * @retval  "1"なら男の子
 * @retval  "2"なら女の子
 */
u8 DebugAgingFrame::_GetAgingTrainerMode( void )
{
  return DebugClass::GetAging_TrainerMode();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   背景の終了
 */
bool DebugAgingFrame::_IsEndBackGround( void )
{
  if( _GetAgingBackGroundChange() == 0 )
  {
    return _IsEndBackGround_WaitTime();
  }
  else {
    return _IsEndBackGround_InputKey();
  }
}

/**
 * @brief   背景の終了：時間経過
 */
bool DebugAgingFrame::_IsEndBackGround_WaitTime( void )
{
  m_BackGroundWaitTime++;

  if( m_BackGroundWaitTime >= 90 )
  {
    return true;
  }

  return false;
}

/**
 * @brief   背景の終了：キー入力
 */
bool DebugAgingFrame::_IsEndBackGround_InputKey( void )
{
  gfl2::ui::Button* button = _GetUIDevice_Button();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    return true;
  }

  return false;
}


/**
 * @brief   背景：IDの取得
 */
u32 DebugAgingFrame::_GetAgingBackGroundID( void )
{
  if( _GetAgingBackGroundMode() == 0 )
  {
    return ( BG_ID_00 + m_BackGroundCount ) % BG_ID_NUM;
  }
  else {
    return _GetRandomNum( BG_ID_NUM );
  }
}

/**
 * @brief   背景：時間帯の取得
 */
Viewer::TimeZone DebugAgingFrame::_GetAgingBackGroundTimeZone( u32 bgID )
{
  /// UI用データの取得
  BGDataAccessor::UIData data = BGDataAccessor::GetUIData( bgID );

  if( data.timeZoneFlg == TIME_ZONE_FLG_ALL )
  {
    return Viewer::TIME_ZONE_NOON;
  }
  else if( data.timeZoneFlg == TIME_ZONE_FLG_NOON )
  {
    return Viewer::TIME_ZONE_NOON;
  }
  else if( data.timeZoneFlg == TIME_ZONE_FLG_EVENING )
  {
    return Viewer::TIME_ZONE_EVENING;
  }
  else if( data.timeZoneFlg == TIME_ZONE_FLG_NIGHT )
  {
    return Viewer::TIME_ZONE_NIGHT;
  }

  GFL_ASSERT( 0 );
  return Viewer::TIME_ZONE_NOON;
}

/**
 * @brief   背景：次の時間帯の取得
 */
Viewer::TimeZone DebugAgingFrame::_GetAgingBackGroundNextTimeZone( Viewer::TimeZone now )
{
  if( now == Viewer::TIME_ZONE_NOON )
  {
    return Viewer::TIME_ZONE_EVENING;
  }
  else if( now == Viewer::TIME_ZONE_EVENING )
  {
    return Viewer::TIME_ZONE_NIGHT;
  }
  else {
    return Viewer::TIME_ZONE_MAX;
  }
}

/**
 * @brief   背景：利用可能かのチェック
 */
bool DebugAgingFrame::_IsAvailableTimeZone( u32 bgID, Viewer::TimeZone timeZone )
{
  /// UI用データの取得
  BGDataAccessor::UIData data = BGDataAccessor::GetUIData( bgID );

  if( data.timeZoneFlg == TIME_ZONE_FLG_ALL )
  {
    return true;
  }
  else if( data.timeZoneFlg == TIME_ZONE_FLG_NOON && timeZone == Viewer::TIME_ZONE_NOON )
  {
    return true;
  }
  else if( data.timeZoneFlg == TIME_ZONE_FLG_EVENING && timeZone == Viewer::TIME_ZONE_EVENING )
  {
    return true;
  }
  else if( data.timeZoneFlg == TIME_ZONE_FLG_NIGHT && timeZone == Viewer::TIME_ZONE_NIGHT )
  {
    return true;
  }

  return false;
}


/**
 * @brief   背景：モードの取得
 *
 * @retval  "0"なら順番
 * @retval  "1"ならランダム
 */
u8 DebugAgingFrame::_GetAgingBackGroundMode( void )
{
  return DebugClass::GetAging_BackGroundMode();
}

/**
 * @brief   背景：切替方法の取得
 *
 * @retval  "0"なら自動
 * @retval  "1"なら手動
 */
u8 DebugAgingFrame::_GetAgingBackGroundChange( void )
{
  return DebugClass::GetAging_BackGroundChange();
}

/**
 * @brief   背景：カメラモード
 */
u8 DebugAgingFrame::_GetAgingBackGroundCamera( void )
{
  return DebugClass::GetAging_BackGroundCamera();
}


/**
 * @brief   背景：キーアクション
 */
void DebugAgingFrame::_BackGroundKeyAction( void )
{
  u8 cmaeraMode = _GetAgingBackGroundCamera();

  /// 手動操作
  if( cmaeraMode == 1 )
  {
    /// デバッグウィンドウが開いているか
    if( DebugClass::IsDebugWinOpen() )
    {
      /// 開いているなら操作は反映しない
      return;
    }

    gfl2::ui::Button* button = _GetUIDevice_Button();
    gfl2::ui::VectorDevice* stick = _GetUIDevice_Stick();

    /// カメラ：回転
    _BackGroundKeyActionRotate( stick );
    /// カメラ：ズーム
    _BackGroundKeyActionZoom( button );
    /// カメラ：高さ
    _BackGroundKeyActionHeight( button );
    /// カメラ：フォーカス
    _BackGroundKeyActionFocus( button );
  }
  /// 自動回転(Y)
  else if( cmaeraMode == 2 )
  {
    /// カメラ：自動回転
    _BackGroundKeyActionAutoRotate();
  }
}

/**
 * @brief   背景：カメラ回転
 */
void DebugAgingFrame::_BackGroundKeyActionRotate( gfl2::ui::VectorDevice* stick )
{
  f32 stickX = stick->GetX();
  f32 stickY = stick->GetY();

  /// X軸回転
  if( stickY > 0.5f )
  {
    m_pViewerManager->RequestCameraXAxisRotationUp();
  }
  else if( stickY < -0.5f )
  {
    m_pViewerManager->RequestCameraXAxisRotationDown();
  }

  /// Y軸回転
  if( stickX > 0.5f )
  {
    m_pViewerManager->RequestCameraYAxisRotationLeft();
  }
  else if( stickX < -0.5f )
  {
    m_pViewerManager->RequestCameraYAxisRotationRight();
  }
}

/**
 * @brief   背景：カメラズーム
 */
void DebugAgingFrame::_BackGroundKeyActionZoom( gfl2::ui::Button* button )
{
  if( button->IsHold( gfl2::ui::BUTTON_UP ) )
  {
    /// ズームイン
    m_pViewerManager->RequestCameraZoomIn();
  }
  else if( button->IsHold( gfl2::ui::BUTTON_DOWN ) )
  {
    /// ズームアウト
    m_pViewerManager->RequestCameraZoomOut();
  }
}

/**
 * @brief   背景：カメラ高さ
 */
void DebugAgingFrame::_BackGroundKeyActionHeight( gfl2::ui::Button* button )
{
  if( button->IsHold( gfl2::ui::BUTTON_X ) )
  {
    /// カメラの高さ：上
    m_pViewerManager->RequestCameraYAxisTranslationUp();
  }
  else if( button->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    /// カメラの高さ：下
    m_pViewerManager->RequestCameraYAxisTranslationDown();
  }
}

/**
 * @brief   背景：カメラフォーカス
 */
void DebugAgingFrame::_BackGroundKeyActionFocus( gfl2::ui::Button* button )
{
  /// ツーショットの時のみフォーカスを操作できる
  if( button->IsHold( gfl2::ui::BUTTON_LEFT ) )
  {
    /// フォーカス：ポケモン
    m_pViewerManager->RequestCameraFocusPokemon();
  }
  else if( button->IsHold( gfl2::ui::BUTTON_RIGHT ) )
  {
    /// フォーカス：プレイヤー
    m_pViewerManager->RequestCameraFocusTrainer();
  }
}

/**
 * @brief   背景：カメラ自動回転
 */
void DebugAgingFrame::_BackGroundKeyActionAutoRotate( void )
{
  /// カメラ回転
  m_pViewerManager->RequestCameraYAxisRotationLeft();
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    エージングチェック関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   エージングチェック関数：どれかのエージングが開始されているか？
 */
bool DebugAgingFrame::_IsAgingStart( void )
{
  return DebugClass::IsAging_Start();
}

/**
 * @brief   エージングチェック関数：ポケモンエージングを開始するか？
 */
bool DebugAgingFrame::_IsAgingStartPokemon( void )
{
  return DebugClass::IsAging_PokemonStart();
}

/**
 * @brief   エージングチェック関数：トレーナーエージングを開始するか？
 */
bool DebugAgingFrame::_IsAgingStartTrainer( void )
{
  return DebugClass::IsAging_TrainerStart();
}

/**
 * @brief   エージングチェック関数：背景エージングを開始するか？
 */
bool DebugAgingFrame::_IsAgingStartBackGround( void )
{
  return DebugClass::IsAging_BackGroundStart();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    便利関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   便利関数：キーの取得
 */
gfl2::ui::Button* DebugAgingFrame::_GetUIDevice_Button( void )
{
  return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
}

/**
 * @brief   便利関数：スティックの取得
 */
gfl2::ui::VectorDevice* DebugAgingFrame::_GetUIDevice_Stick( void )
{
  return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
}

/**
 * @brief   ランダムの取得
 */
u32 DebugAgingFrame::_GetRandomNum( void )
{
  return System::GflUse::GetPublicRand();
}

/**
 * @brief   ランダムの取得
 */
u32 DebugAgingFrame::_GetRandomNum( u32 max )
{
  return System::GflUse::GetPublicRand( max );
}


/**
 * @brief   Viewer更新完了チェック
 */
bool DebugAgingFrame::_IsFinishUpdateViewer( void )
{
  return m_pViewerManager->IsFinishUpdadteViewer();
}


/**
 * @brief   ループ回数のインクリメント
 */
void DebugAgingFrame::_IncrementLoopCount( void )
{
  GFL_RELEASE_PRINT( "LoopCount=[%d]\n", m_AgingLoopCount );
  m_AgingLoopCount++;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // PM_DEBUG
