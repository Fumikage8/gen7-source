#if PM_DEBUG

//======================================================================
/**
 * @file    FinderStudioCaptureDebugAutoCaptureFrame.cpp
 * @date    2017/04/06 12:15:55
 * @author  fukushima_yuya
 * @brief   デバッグ機能フレーム：自動撮影
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureDebugAutoCaptureFrame.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebug.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureUpperView.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>

#include <niji_conv_header/app/data/finder_studio/finder_studio_stamp_s_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_stamp_m_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_stamp_l_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_stamp_text_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_frame_def.h>

#include <System/include/gflUse.h>
#include <util/include/gfl2_std_string.h>


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
DebugAutoCaptureFrame::DebugAutoCaptureFrame( void )
  : m_pViewerManager( NULL )
  , m_pUpperView( NULL )
  , m_pImageDBUtil( NULL )
  , m_pExtSaveData( NULL )
  , m_CaptureCount( 0 )
  , m_CaptureCountMax( 0 )
  , m_MonsNo( MONSNO_NULL )
  , m_bRandomMonsNo( false )
  , m_BackGroundID( 0 )
  , m_BackGroundType( 0 )
  , m_bRandomDressup( false )
  , m_bRandomDeco( false )
  , m_bSaveFlg( false )
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
applib::frame::Result DebugAutoCaptureFrame::InitFunc( void )
{
  /// ViewerManager
  m_pViewerManager = m_pMainWork->viewer;

  /// 上画面View
  Static::ViewList* viewList = m_pMainWork->viewList;
  m_pUpperView = reinterpret_cast<UpperView*>( viewList->GetView( VIEW_ID_UPPER ) );
  m_pUpperView->CurtainOpen();

  /// 自動撮影開始フラグを初期化
  DebugClass::ResetAutoCapture_Start();

  this->SetMainSeq( SEQ_INIT );

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result DebugAutoCaptureFrame::EndFunc( void )
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
applib::frame::Result DebugAutoCaptureFrame::UpdateFunc( void )
{
  void ( DebugAutoCaptureFrame::*UPDATE_FUNC[] )( void ) = {
    &DebugAutoCaptureFrame::_UpdateFunc_Init,
    &DebugAutoCaptureFrame::_UpdateFunc_ViewerSetup,
    &DebugAutoCaptureFrame::_UpdateFunc_ViewerUpdate,
    &DebugAutoCaptureFrame::_UpdateFunc_CheckFull,
    &DebugAutoCaptureFrame::_UpdateFunc_Capture,
    &DebugAutoCaptureFrame::_UpdateFunc_SaveImageDB,
    &DebugAutoCaptureFrame::_UpdateFunc_SaveExtSave,
    &DebugAutoCaptureFrame::_UpdateFunc_FinishCheck,
  };

  if( m_MainSeq >= SEQ_END )
  {
    return applib::frame::RES_FINISH;
  }
  else {
    ( this->*UPDATE_FUNC[m_MainSeq] )( );
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：初期化
 */
void DebugAutoCaptureFrame::_UpdateFunc_Init( void )
{
  /// パラメータの取得
  {
    /// 撮影枚数
    m_CaptureCountMax = DebugClass::GetAutoCapture_CaptureCount();
    /// 開始ポケモンID
    m_MonsNo = DebugClass::GetAutoCapture_StartMonsNo();
    /// ポケモンランダム撮影フラグ
    m_bRandomMonsNo = DebugClass::IsAutoCapture_RandomMonsNo();
    /// 背景ID
    m_BackGroundID = DebugClass::GetAutoCapture_BackGroundID();
    /// 背景表示タイプ
    m_BackGroundType = DebugClass::GetAutoCapture_BackGroundType();
    /// 衣装ランダムフラグ
    m_bRandomDressup = DebugClass::IsAutoCapture_RandomDressup();
    /// デコランダムフラグ
    m_bRandomDeco = DebugClass::IsAutoCapture_RandomDecoration();
    /// 撮影日時ランダムフラグ
    m_bRandomDate = DebugClass::IsAutoCapture_RandomDate();
    /// 保存フラグ
    m_bSaveFlg = DebugClass::IsAutoCapture_Save();
  }

  this->SetMainSeq( SEQ_VIEWER_SETUP );
}

/**
 * @brief   更新：Viewerセットアップ
 */
void DebugAutoCaptureFrame::_UpdateFunc_ViewerSetup( void )
{
  /// 撮影対象のポケモン
  MonsNo monsNo = _GetMonsNo();

  /// ポケモンパラメータ
  PokeTool::SimpleParam simpleParam = _SetupSimpleParam( monsNo );
  /// 背景ID
  u32 backGroundID = _SetupBackGroundID();
  /// 衣装
  poke_3d::model::DressUpParam dressupParam = _SetupDressupParam();
  /// 構図
  Viewer::Composition composition = _SetupComposition();

  /// Viewerリクエストパラメータ
  ViewerManager::RequestViewerParam reqParam = m_DefaultRequestParam;
  {
    reqParam.simpleParam  = simpleParam;
    reqParam.bgID         = backGroundID;
    reqParam.dressupParam = dressupParam;
    reqParam.composition  = composition;
  }

  /// Viewer更新
  m_pViewerManager->RequestViewerUpdate_All( reqParam );
  m_pViewerManager->StartUpdateViewer();

  /// 拡張セーブデータのセットアップ
  _SetupIntegratedData( monsNo );

  this->SetMainSeq( SEQ_VIEWER_UPDATE );
}

/**
 * @brief   更新：Viewer更新
 */
void DebugAutoCaptureFrame::_UpdateFunc_ViewerUpdate( void )
{
  /// Viewer更新完了
  if( m_pViewerManager->IsFinishUpdadteViewer() )
  {
    this->SetMainSeq( SEQ_CHECK_FULL );
  }
}

/**
 * @brief   更新：チェック 保存可能か
 */
void DebugAutoCaptureFrame::_UpdateFunc_CheckFull( void )
{
  u32 saveCount = 0;

  gfl2::heap::HeapBase* heap = m_pMainWork->heap->GetDeviceHeap();
  m_pExtSaveData = GFL_NEW( heap ) Static::FinderStudioExtSaveDataAccessor( heap );
  {
    /// データカウントの設定
    m_pExtSaveData->SetDataIndex();
    /// 保存数の取得
    saveCount = m_pExtSaveData->GetDataCount();
  }
  GFL_SAFE_DELETE( m_pExtSaveData );

  FUKUSHIMA_PRINT( "SaveCount [%d] / 420\n", saveCount );

  if( saveCount == Static::SAVE_PHOTO_MAX )
  {
    this->SetMainSeq( SEQ_FINISH_CHECK );
  }
  else {
    this->SetMainSeq( SEQ_CAPTURE );
  }
}

/**
 * @brief   更新：撮影
 */
void DebugAutoCaptureFrame::_UpdateFunc_Capture( void )
{
  if( m_SubSeq == 0 )
  {
    if( m_bSaveFlg )
    {
      Work* work = m_pMainWork->work;

      m_pViewerManager->RequestCaptureStart( work->viewer.photo_tex, work->viewer.photo_param );
      m_SubSeq = 1;
    }
    else {
      this->SetMainSeq( SEQ_FINISH_CHECK );
    }
  }
  else if( m_SubSeq == 1 )
  {
    if( m_pViewerManager->IsFinishCapture() )
    {
#if defined(GF_PLATFORM_CTR)
      this->SetMainSeq( SEQ_SAVE_IMAGEDB );
#else
      this->SetMainSeq( SEQ_FINISH_CHECK );
#endif
    }
  }
}

/**
 * @brief   更新：ImageDBへの保存
 */
void DebugAutoCaptureFrame::_UpdateFunc_SaveImageDB( void )
{
  if( m_SubSeq == 0 )
  {
    gfl2::heap::HeapBase* heap = m_pMainWork->heap->GetDeviceHeap();
    m_pImageDBUtil = GFL_NEW( heap ) Static::ImageDBUtil();

    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();
    wchar_t* filepath = reinterpret_cast<wchar_t*>( m_ManagementData.filepath );

    // 保存開始
    m_pImageDBUtil->StartSaveImage( heap, filepath, m_pMainWork->work->viewer.photo_tex[0] );

    m_SubSeq++;
  }
  if( m_SubSeq == 2 )
  {
    // 保存完了まで待機
    if( m_pImageDBUtil->IsFinishSaveImage() )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 3 )
  {
    bool bSuccess = m_pImageDBUtil->GetResult() == ErrorConverter::IMGDB_RESULT_SUCCESS;

    GFL_SAFE_DELETE( m_pImageDBUtil );

    if( bSuccess )
    {
      this->SetMainSeq( SEQ_SAVE_EXTSAVE );
    }
    else {
      this->SetMainSeq( SEQ_END );
    }
  }
}

/**
 * @brief   更新：拡張セーブデータへの保存
 */
void DebugAutoCaptureFrame::_UpdateFunc_SaveExtSave( void )
{
  if( m_SubSeq == 0 )
  {
    gfl2::heap::HeapBase* heap = m_pMainWork->heap->GetDeviceHeap();
    m_pExtSaveData = GFL_NEW( heap ) Static::FinderStudioExtSaveDataAccessor( heap );

    m_SubSeq++;
  }
  if( m_SubSeq == 1 )
  {
    /// 管理データ
    m_IntegratedData.management_data = m_ManagementData;
    /// 写真データ
    m_IntegratedData.photo_data.photo_param = m_pMainWork->work->viewer.photo_param[0];
    /// デコデータ
    _SetupDecorationData();

    m_SubSeq++;
  }
  if( m_SubSeq == 2 )
  {
    Static::FinderStudioExtSaveDataAccessor::Result result = m_pExtSaveData->WriteFile( &m_IntegratedData );

    if( result != Static::FinderStudioExtSaveDataAccessor::RESULT_CONTINUE )
    {
      m_SubSeq++;
    }
  }
  if( m_SubSeq == 3 )
  {
    bool bSuccess = ErrorConverter::ConvertSDCardErrorFromExtSaveResult( m_pExtSaveData->GetResult() ) == ErrorConverter::SDCARD_RESULT_SUCCESS;

    GFL_SAFE_DELETE( m_pExtSaveData );

    this->SetMainSeq( SEQ_FINISH_CHECK );
  }
}

/**
 * @brief   更新：終了チェック
 */
void DebugAutoCaptureFrame::_UpdateFunc_FinishCheck( void )
{
  m_CaptureCount++;

  if( m_bSaveFlg )
  {
    GFL_RELEASE_PRINT( "CaptureFinish : [%d / %d]\n", m_CaptureCount, m_CaptureCountMax );
  }
  else {
    GFL_RELEASE_PRINT( "CaptureFinish : [%d]\n", m_CaptureCount );
  }

  if( DebugClass::IsAutoCapture_Stop() )
  {
    DebugClass::ResetAutoCapture_Stop();
    this->SetMainSeq( SEQ_END );
    return;
  }

  if( m_bSaveFlg )
  {
    if( m_CaptureCount >= m_CaptureCountMax )
    {
      this->SetMainSeq( SEQ_END );
    }
    else {
      this->SetMainSeq( SEQ_VIEWER_SETUP );
    }
  }
  else {
    if( m_CaptureCount >= 0xFFFF )
    {
      m_CaptureCount = 0;
    }

    this->SetMainSeq( SEQ_VIEWER_SETUP );
  }
}



/**
 * @brief   PokeTool::SimpleParamのセットアップ
 *
 * @param[in]   monsNo    種族No
 *
 * @retval  PokeTool::SimpleParam
 */
PokeTool::SimpleParam DebugAutoCaptureFrame::_SetupSimpleParam( MonsNo monsNo )
{
  /// ポケモンパラメータ
  PokeTool::SimpleParam simpleParam;
  {
    /// MonsNo
    simpleParam.monsNo = monsNo;
    /// Sex
    simpleParam.sex = _GetSex( monsNo );
    /// FormNo
    simpleParam.formNo = 0;
    /// isEgg
    simpleParam.isEgg = false;
    /// isRare
    simpleParam.isRare = 0;
    /// perRand
    simpleParam.perRand = 0;
  }

  return simpleParam;
}

/**
 * @brief   背景IDのセットアップ
 *
 * @retval  背景ID
 */
u32 DebugAutoCaptureFrame::_SetupBackGroundID( void )
{
  /// [固定]
  if( m_BackGroundType == 0 )
  {
    return m_BackGroundID;
  }
  /// [順番]
  else if( m_BackGroundType == 1 )
  {
    return ( m_BackGroundID + m_CaptureCount ) % BG_ID_NUM;
  }
  /// [ランダム]
  else {
    return _GetRandomNum( BG_ID_NUM );
  }
}

/**
 * @brief   衣装のセットアップ
 *
 * @retval  衣装パラメータ
 */
poke_3d::model::DressUpParam DebugAutoCaptureFrame::_SetupDressupParam( void )
{
  poke_3d::model::DressUpParam dressupParam = DebugClass::GetFieldDressup();

  /// ランダム
  if( m_bRandomDressup )
  {
    /// 男
    if( DebugClass::IsPlayerSexMale() )
    {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT]     = _GetRandomNum( DRESSUP_ITEM_ID_HERO_HAT_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE]  = _GetRandomNum( DRESSUP_ITEM_ID_HERO_ACCEYE_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS]    = _GetRandomNum( DRESSUP_ITEM_ID_HERO_TOPS_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS] = _GetRandomNum( DRESSUP_ITEM_ID_HERO_BOTTOMS_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS]    = _GetRandomNum( DRESSUP_ITEM_ID_HERO_LEGS_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES]   = _GetRandomNum( DRESSUP_ITEM_ID_HERO_SHOES_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG]     = _GetRandomNum( DRESSUP_ITEM_ID_HERO_BAG_COUNT );
    }
    /// 女
    else {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT]     = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_HAT_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_ACCHAIR_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE]  = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_ACCEYE_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS]    = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_TOPS_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS] = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_BOTTOMS_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS]    = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_LEGS_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES]   = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_SHOES_COUNT );
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG]     = _GetRandomNum( DRESSUP_ITEM_ID_HEROINE_BAG_COUNT );
    }
  }
  /// 固定
  else {
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT]     = DebugClass::GetAutoCapture_DressupHat();
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = DebugClass::GetAutoCapture_DressupHairAcce();
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE]  = DebugClass::GetAutoCapture_DressupEyeAcce();
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS]    = DebugClass::GetAutoCapture_DressupTops();
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS] = DebugClass::GetAutoCapture_DressupBottoms();
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS]    = DebugClass::GetAutoCapture_DressupLeg();
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES]   = DebugClass::GetAutoCapture_DressupShoes();
    dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG]     = DebugClass::GetAutoCapture_DressupBag();
  }

  /// ランダムで帽子、アクセサリー、全力リングを外す
  {
    u32 rand = 0;

    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
    }
    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;
    }
    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
    }
    rand = _GetRandomNum();
    if( rand % 2 == 0 )
    {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = -1;
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
      if( DebugClass::IsPlayerSexMale() )
      {
        dressupResourceManagerCore = dressupResourceManager->GetDressUpModelResourceManagerCore( poke_3d::model::DressUpParam::SEX_MALE );
      }
      else {
        dressupResourceManagerCore = dressupResourceManager->GetDressUpModelResourceManagerCore( poke_3d::model::DressUpParam::SEX_FEMALE );
      }
    }

    /// 帽子がかぶれない髪型なら帽子を外す
    if( !dressupResourceManagerCore->CanDressUpHairWearHat( dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE] ) )
    {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
    }
    /// 帽子をかぶっているならヘアアクセサリーは外す
    if( dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] >= 0 )
    {
      dressupParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
    }

    /// 念のため組み合わせチェックをして問題があればフィールド衣装に戻す
    if( !dressupResourceManagerCore->CheckDressUpCombination( dressupParam ) )
    {
      dressupParam = DebugClass::GetFieldDressup();
    }

    /// 破棄
    dressupResourceManager->Finalize();
    GFL_SAFE_DELETE( dressupResourceManager );
  }

  return dressupParam;
}

/**
 * @brief   構図のセットアップ
 *
 * @retval  構図ID
 */
Viewer::Composition DebugAutoCaptureFrame::_SetupComposition( void )
{
  /// 構図IDの取得
  u8 compID = DebugClass::GetAutoCapture_Composition();

  if( compID == 0 )
  {
    return Viewer::COMPOSITION_1;
  }
  else if( compID == 1 )
  {
    return Viewer::COMPOSITION_2;
  }
  else {
    return ( ( _GetRandomNum() % 2 ) == 0 ) ? Viewer::COMPOSITION_1 : Viewer::COMPOSITION_2;
  }
}


/**
 * @brief   IntegratedDataのセットアップ
 */
void DebugAutoCaptureFrame::_SetupIntegratedData( MonsNo monsNo )
{
  /// 初期化
  gfl2::std::MemClear( &m_IntegratedData, sizeof( Static::IntegratedData ) );

  /// ManagementDataのセットアップ
  _SetupManagementData( monsNo );
}

/**
 * @brief   ManagementDataのセットアップ
 *
 * @param[in]   monsNo    種族No
 */
void DebugAutoCaptureFrame::_SetupManagementData( MonsNo monsNo )
{
  /// 初期化
  gfl2::std::MemClear( &m_ManagementData, sizeof( Static::ManagementData ) );

  /// 種族No
  m_ManagementData.mons_no = monsNo;
  /// 編集フラグ
  m_ManagementData.deco_flg = _GetDecorationFlg();
  /// お気に入りフラグ
  m_ManagementData.favorite_flg = 0;
  /// 撮影モード
  m_ManagementData.cap_mode = m_pMainWork->work->prepare.capture_mode;
  /// 背景インデックス
  m_ManagementData.bg_index = m_pMainWork->work->prepare.background_id;
  /// 共有フラグ
  m_ManagementData.joint_flg = 0;
  /// 撮影日時
  m_ManagementData.date = _GetDate();
}

/**
 * @brief   DecorationDataのセットアップ
 */
void DebugAutoCaptureFrame::_SetupDecorationData( void )
{
  /// NULL_DATAをセット
  for( u8 i = 0; i < Static::DECO_ITEM_MAX; ++i )
  {
    m_IntegratedData.photo_data.deco_param.deco_param[i] = Static::STAMP_NULL_DATA;
  }

  /// 編集フラグが１ならデコを配置する
  if( m_ManagementData.deco_flg == 1 )
  {
    /// デコの配置数を取得
    u8 decoCount = _GetRandomNum( Static::DECO_ITEM_MAX );

    for( u8 i = 0; i <= decoCount; ++i )
    {
      Static::StampData* data = &m_IntegratedData.photo_data.deco_param.deco_param[i];
      {
        data->id = _GetRandomNum( DECO_STAMP_S_NUM + DECO_STAMP_M_NUM + DECO_STAMP_L_NUM + DECO_STAMP_TEXT_NUM );
        data->x = _GetRandomNum( 400 );
        data->y = _GetRandomNum( 240 );
        data->frame_id = _GetRandomNum( DECO_FRAME_KIND_NUM );
      }
    }
  }
}




/**
 * @brief   MonsNoの取得
 *
 * @retval  MonsNo
 */
MonsNo DebugAutoCaptureFrame::_GetMonsNo( void )
{
  if( !m_bRandomMonsNo )
  {
    /// ランダムではないので開始ID＋撮影枚数でMonsNoを計算
    u32 nextNo = m_MonsNo + m_CaptureCount;

    if( nextNo >= POKEMON_NUM )
    {
      nextNo = ( nextNo % POKEMON_NUM ) + 1;
    }

    return static_cast<MonsNo>( nextNo );
  }
  else {
    /// ランダムでMonsNoを計算
    return static_cast<MonsNo>( _GetRandomNum( POKEMON_NUM ) + 1 );
  }
}


/**
 * @brief   性別の取得
 *
 * @param[in]   monsNo    種族No
 *
 * @retval  pml::Sex
 */
pml::Sex DebugAutoCaptureFrame::_GetSex( MonsNo monsNo )
{
  pml::Sex sex = pml::SEX_MALE;
  {
    gfl2::heap::HeapBase* heap = m_pMainWork->heap->GetDeviceHeap();

    /// ポケモンパラメータを生成
    pml::pokepara::PokemonParam* param =
      GFL_NEW_LOW( heap ) pml::pokepara::PokemonParam( heap, monsNo, 100, 0 );
    {
      /// 性別の取得
      bool fastmode = param->StartFastMode();
      {
        sex = param->GetSex();
      }
      param->EndFastMode( fastmode );
    }
    GFL_SAFE_DELETE( param );
  }

  return PokeTool::GetDrawSex( monsNo, sex, false, false );
}

/**
 * @brief   フォルムNoの取得
 *
 * @param[in]   monsNo    種族No
 *
 * @retval  フォルムNo
 */
pml::FormNo DebugAutoCaptureFrame::_GetFormNo( MonsNo monsNo )
{
  /// フォルム最大数を取得
  pml::FormNo formMax = _GetFormMax( monsNo );

  /// フォルムNoをランダムで設定
  return static_cast<pml::FormNo>( _GetRandomNum( formMax ) );
}

/**
 * @brief   フォルムMaxを取得
 *
 * @retval  フォルムの最大数
 */
pml::FormNo DebugAutoCaptureFrame::_GetFormMax( MonsNo monsNo )
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
 * @brief   編集フラグの取得
 *
 * @retval  "1"なら編集済み
 * @retval  "0"なら未編集
 */
u8 DebugAutoCaptureFrame::_GetDecorationFlg( void )
{
  if( m_bRandomDeco )
  {
    return _GetRandomNum() % 2;
  }

  return 0;
}

/**
 * @brief   日時の取得
 *
 * @retval  日時
 */
Static::Date DebugAutoCaptureFrame::_GetDate( void )
{
  /// ランダム
  if( m_bRandomDate )
  {
    return _GetDate_Random();
  }
  /// 通常
  else {
    return _GetDate_Now();
  }
}

/**
 * @brief   日時の取得：現在
 */
Static::Date DebugAutoCaptureFrame::_GetDate_Now( void )
{
  // 現在時間
  GameSys::DeviceDate deviceDate;
  gfl2::system::Date* systemDate = deviceDate.GetDate();
  gfl2::system::Date::GetNow( systemDate );

  Static::Date date;
  {
    date.year   = static_cast<u16>( systemDate->GetYear() );
    date.month  = static_cast<u8>( systemDate->GetMonth() );
    date.day    = static_cast<u8>( systemDate->GetDay() );
  }

  return date;
}

/**
 * @brief   日時の取得：ランダム
 */
Static::Date DebugAutoCaptureFrame::_GetDate_Random( void )
{

  Static::Date date;
  {
    date.year  = 2000 + _GetRandomNum( 30 );
    date.month = _GetRandomNum( 12 ) + 1;
    date.day   = _GetRandomNum( _GetDateNum( date.month, _IsUruuDoshi( date.year ) ) ) + 1;
  }

  return date;
}

/**
 * @brief   閏年かどうか
 */
bool DebugAutoCaptureFrame::_IsUruuDoshi( u16 year )
{
  if( ( year % 4 ) != 0 )
  {
    return false;
  }
  if( ( year % 100 ) == 0 )
  {
    if( ( year % 400 ) != 0 )
    {
      return false;
    }
  }

  return true;
}

/**
 * @brief   日数を取得
 */
u8 DebugAutoCaptureFrame::_GetDateNum( u8 month, bool bUruuDoshi )
{
  switch( month )
  {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
    {
      return 31;
    } // no break;

    case 2:
    {
      return ( bUruuDoshi ) ? 29 : 28;
    } // no break;
  }

  return 30;
}



/**
 * @brief   ランダム値の取得
 */
u32 DebugAutoCaptureFrame::_GetRandomNum( void )
{
  return System::GflUse::GetPublicRand();
}

/**
 * @brief   ランダム値の取得：最大値指定
 *
 * @param[in]   max   最大値
 */
u32 DebugAutoCaptureFrame::_GetRandomNum( u32 max )
{
  return System::GflUse::GetPublicRand( max );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif  // PM_DEBUG