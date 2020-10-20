//======================================================================
/**
 * @file    FinderStudioCaptureViewerManager.cpp
 * @date    2016/12/22 20:13:30
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewer管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewerManager.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioViewer/include/FinderStudioViewer.h>
#include <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>

#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <Savedata/include/MyStatus.h>
#include <System/include/gflUse.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ グローバル変数宣言 begin ]---
PokeTool::SimpleParam ViewerManager::SIMPLE_PARAM_NULL;
poke_3d::model::DressUpParam ViewerManager::DRESSUP_PARAM_NULL;
/// ---[ グローバル変数宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// ---[ ViewerManagerクラス begin ]---


//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 *
 * @param[in]   heap    Viewer専用ヒープ
 */
ViewerManager::ViewerManager( gfl2::heap::HeapBase* heap )
  : m_pHeap( heap )
  , m_ReqParam()
  , m_UpdateParam()
  , m_MainSeq( 0 )
  , m_ReqSeq( 0 )
  , m_bUpdateViewer( false )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    生成・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成：FinderStudioViewer
 *
 * @param[in]   simpleParam   ポケモン簡易パラメータ
 * @param[in]   dressupParam  衣装データ
 *
 * @retval  "true"なら生成完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManager::CreateViewer( const PokeTool::SimpleParam& simpleParam, const poke_3d::model::DressUpParam& dressupParam )
{
  if( m_MainSeq == 0 )
  {
    // Viewer生成
    this->_CreateViewer( m_pHeap, simpleParam, dressupParam );
    m_MainSeq++;
  }
  if( m_MainSeq == 1 )
  {
    // 初期化完了チェック
    if( this->_IsFinishInitialize() )
    {
      m_MainSeq++;
    }
  }
  if( m_MainSeq == 2 )
  {
    m_MainSeq = 0;
    return true;
  }

  return false;
}

/**
 * @brief   生成：FinderStudioViewer(アプリ復帰版)
 *
 * @param[in]   photoParam    再現パラメータ
 * @param[in]   viewerMode    Viewer起動モード
 *
 * @retval  "true"なら生成完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManager::CreateViewer( const Viewer::PhotoParam& photoParam, Viewer::ViewerMode viewerMode )
{
  if( m_MainSeq == 0 )
  {
    // Viewer生成
    this->_CreateViewer( m_pHeap, photoParam, viewerMode );
    m_MainSeq++;
  }
  if( m_MainSeq == 1 )
  {
    // 初期化完了チェック
    if( this->_IsFinishInitialize() )
    {
      m_MainSeq++;
    }
  }
  if( m_MainSeq == 2 )
  {
    m_MainSeq = 0;
    return true;
  }

  return false;
}

/**
 * @brief   破棄：FinderStudioViewer
 *
 * @retval  "true"なら破棄完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManager::DeleteViewer( void )
{
  if( m_MainSeq == 0 )
  {
    // 終了処理
    if( this->_IsFinishTerminate() )
    {
      m_MainSeq++;
    }
  }
  if( m_MainSeq == 1 )
  {
    // Viewerの破棄
    this->_DeleteViewer();
    m_MainSeq++;
  }
  if( m_MainSeq == 2 )
  {
    m_MainSeq = 0;
    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void ViewerManager::Update( void )
{
  // Viewerの更新
  this->_Update();
  // Viewerリクエスト
  _UpdateRequestViewer();
}

/**
 * @brief   更新：Viewerリクエスト
 */
void ViewerManager::_UpdateRequestViewer( void )
{
  if( m_ReqSeq == 0 )
  {
    if( _IsStartUpdateViewer() )
    {
      FUKUSHIMA_PRINT("StartUpdateViewer\n");

      m_UpdateParam = m_ReqParam;
      m_ReqParam = DEFAULT_REQUEST_VIEWER_PARAM;

      m_ReqSeq++;
    }
  }
  // カーテンが締め切られたかをチェック
  if( m_ReqSeq == 1 )
  {
    //// Viewer更新が開始されたか？
    //// 更新の開始はViewerManagerの外で行われる
    //if( _IsStartUpdateViewer() )
    //{
    //  m_ReqSeq++;
    //}
    m_ReqSeq++;
  }
  // 全てのリセット
  if( m_ReqSeq == 2 )
  {
    bool reqAllReset = m_UpdateParam.bViewerReset;

    if( reqAllReset )
    {
      // 全てのリセット
      this->_ViewerReset();
    }

    m_ReqSeq++;
  }
  // トレーナーモーションのリセット
  if( m_ReqSeq == 3 )
  {
    bool reqTrainerMotionReset = m_UpdateParam.bTrainerMotionReset;

    if( reqTrainerMotionReset )
    {
      // トレーナーモーションリセット
      this->_TrainerMotionReset();
    }

    m_ReqSeq++;
  }
  // 撮影モードの変更：Viewer::CaptureMode
  if( m_ReqSeq == 4 )
  {
    Viewer::CaptureMode reqMode = m_UpdateParam.captureMode;

    // パラメータが初期値と違うなら変更する
    if( reqMode != CAPTURE_MODE_NULL )
    {
      this->_ChangeCaptureMode( reqMode );
    }

    m_ReqSeq++;
  }
  // ポケモンの変更
  if( m_ReqSeq == 5 )
  {
    PokeTool::SimpleParam reqSimpleParam = m_UpdateParam.simpleParam;

    FUKUSHIMA_PRINT( "ChangePokemon : monsNo=[%d] formNo=[%d]\n", reqSimpleParam.monsNo, reqSimpleParam.formNo );

    // パラメータが初期値と違うなら変更する
    if( !_IsCompareSimpleParam( reqSimpleParam, SIMPLE_PARAM_NULL ) )
    {
      // ポケモンの変更開始
      this->_ChangePokemon( reqSimpleParam );

      m_ReqSeq++;
    }
    else {
      m_ReqSeq++;
      m_ReqSeq++;
    }
  }
  // ポケモンの変更
  if( m_ReqSeq == 6 )
  {
    // ポケモンの変更完了待ち
    if( this->_IsFinishChangePokemon() )
    {
      m_ReqSeq++;
    }
  }
  // 背景の変更
  if( m_ReqSeq == 7 )
  {
    u32 reqBGID = m_UpdateParam.bgID;

    // パラメータが初期値と違うなら変更する
    if( reqBGID != BACKGROUND_ID_NULL )
    {
      // 背景の変更開始
      this->_ChangeBackGround( reqBGID );

      m_ReqSeq++;
    }
    else {
      m_ReqSeq++;
      m_ReqSeq++;
    }
  }
  // 背景の変更
  if( m_ReqSeq == 8 )
  {
    // 背景の変更完了待ち
    if( this->_IsFinishChangeBackGround() )
    {
      m_ReqSeq++;
    }
  }
  // 時間帯の変更
  if( m_ReqSeq == 9 )
  {
    Viewer::TimeZone reqTimeZone = m_UpdateParam.timeZone;

    // パラメータが初期値と違うなら変更する
    if( reqTimeZone != TIME_ZONE_NULL )
    {
      // 時間帯の変更
      RequestViewerUpdate_TimeZone( reqTimeZone );
    }

    m_ReqSeq++;
  }
  // 衣装の変更
  if( m_ReqSeq == 10 )
  {
    poke_3d::model::DressUpParam reqDressupParam = m_UpdateParam.dressupParam;

    // パラメータが初期値と違うなら変更する
    if( !_IsCompareDressupParam( reqDressupParam, DRESSUP_PARAM_NULL ) )
    {
      // 衣装の変更開始
      this->_ChangeDressup( reqDressupParam );

      m_ReqSeq++;
    }
    else {
      m_ReqSeq++;
      m_ReqSeq++;
    }
  }
  // 衣装の変更
  if( m_ReqSeq == 11 )
  {
    // 衣装の変更完了待ち
    if( this->_IsFinishChangeDressup() )
    {
      m_ReqSeq++;
    }
  }
  // ポケモンのモーションタイプの変更
  if( m_ReqSeq == 12 )
  {
    Viewer::PokeMotionType reqMotionType = m_UpdateParam.pokeMotionType;
    bool reqMotionTypeReset = m_UpdateParam.bPokeMotionTypeReset;

    // パラメータが初期値と違うなら変更する
    if( reqMotionType != POKE_MOTION_TYPE_NULL )
    {
      // ポケモンのモーションタイプの変更開始
      this->_ChangePokemonMotionType( reqMotionType, reqMotionTypeReset );

      m_ReqSeq++;
    }
    else {
      m_ReqSeq++;
      m_ReqSeq++;
    }
  }
  // ポケモンのモーションタイプの変更
  if( m_ReqSeq == 13 )
  {
    // ポケモンのモーションタイプの変更完了待ち
    if( this->_IsFinishChangePokemonMotionType() )
    {
      m_ReqSeq++;
    }
  }
  // 構図の変更
  if( m_ReqSeq == 14 )
  {
    Viewer::Composition reqComposition = m_UpdateParam.composition;

    // パラメータが初期値と違うなら変更する
    if( reqComposition != COMPOSITION_NULL )
    {
      // 構図が使用可能かどうか
      if( _IsAvailableComposition( reqComposition ) )
      {
        // 構図の変更開始
        this->_ChangeComposition( reqComposition );
      }
    }

    m_ReqSeq++;
  }
  // 終了
  if( m_ReqSeq == 15 )
  {
    RequestViewerParam defaultParam = DEFAULT_REQUEST_VIEWER_PARAM;

    // Viewer更新パラメータの比較チェック
    // 不一致の場合はViewerを更新する。
    if( !_IsCompareRequestParam( m_ReqParam, defaultParam ) )
    {
      FUKUSHIMA_PRINT( "RetryUpdateViewer\n" );

      m_UpdateParam = m_ReqParam;
      m_ReqParam = defaultParam;

      m_ReqSeq = 1;
    }
    else {
      FUKUSHIMA_PRINT( "FinishUpdateViewer\n" );

      m_bUpdateViewer = false;
      m_ReqSeq = 0;
    }
  }
}

/**
 * @brief   描画
 *
 * @param[in]   dsipNo    ディスプレイNo
 */
void ViewerManager::Draw( gfl2::gfx::CtrDisplayNo dispNo )
{
  // Viewerの描画
  this->_Draw( dispNo );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リクエスト
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   リクエスト：Viewer更新
 *
 * @param[in]   param   Viewer更新リクエストパラメータ構造体
 */
void ViewerManager::RequestViewerUpdate_All( RequestViewerParam param )
{
  m_ReqParam = param;
}

/**
 * @brief   リクエスト：撮影モードの変更
 *
 * @param[in]   mode    撮影モード
 */
void ViewerManager::RequestViewerUpdate_CaptureMode( Viewer::CaptureMode mode )
{
  m_ReqParam.captureMode = mode;
}

/**
 * @brief   リクエスト：ポケモンの変更
 *
 * @param[in]   simpleParam   ポケモン簡易パラメータ
 */
void ViewerManager::RequestViewerUpdate_PokeModel( PokeTool::SimpleParam simpleParam )
{
  m_ReqParam.simpleParam = simpleParam;
}

/**
 * @brief   リクエスト：背景の変更
 *
 * @param[in]   bgID        背景ID
 * @param[in]   timeZone    時間帯
 */
void ViewerManager::RequestViewerUpdate_BackGround( u32 bgID, Viewer::TimeZone timeZone )
{
  m_ReqParam.bgID = bgID;
  m_ReqParam.timeZone = timeZone;

  FUKUSHIMA_PRINT( "RequestViewerUpdate_BackGround : bgID=[%d], timeZone=[%d]\n", bgID, timeZone );
}

/**
 * @brief   リクエスト：衣装の変更
 *
 * @param[in]   dressupParam    衣装データ
 */
void ViewerManager::RequestViewerUpdate_Dressup( poke_3d::model::DressUpParam dressupParam )
{
  m_ReqParam.dressupParam = dressupParam;
}

/**
 * @brief   リクエスト：ポケモンモーションタイプの変更
 *
 * @param[in]   pokeMotionType    ポケモンモーションタイプ
 * @param[in]   bReset            リセットフラグ
 */
void ViewerManager::RequestViewerUpdate_PokeMotionType( Viewer::PokeMotionType pokeMotionType, bool bReset )
{
  m_ReqParam.pokeMotionType = pokeMotionType;
  m_ReqParam.bPokeMotionTypeReset = bReset;
}

/**
 * @brief   リクエスト：構図の変更
 *
 * @param[in]   composition    構図ID
 */
void ViewerManager::RequestViewerUpdate_Composition( Viewer::Composition composition )
{
  m_ReqParam.composition = composition;
}

/**
 * @brief   リクエスト：全てのリセット
 */
void ViewerManager::RequestViewerUpdate_AllReset( void )
{
  m_ReqParam.bViewerReset = true;
}

/**
 * @brief   リクエスト：トレーナーモーションのリセット
 */
void ViewerManager::RequestViewerUpdate_TrainerMotionReset( void )
{
  m_ReqParam.bTrainerMotionReset = true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新 開始・終了：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：開始
 */
void ViewerManager::_StartUpdateViewer( void )
{
  m_bUpdateViewer = true;

  FUKUSHIMA_PRINT("StartUpdateViewer\n");
}

/**
 * @brief   更新：開始するかどうか
 *
 * @retval  "true"なら開始
 * @retval  "false"ならそれ以外
 */
bool ViewerManager::_IsStartUpdateViewer( void )
{
  return m_bUpdateViewer;
}

/**
 * @brief   更新：終了したかどうか
 *
 * @retval  "true"なら終了
 * @retval  "false"ならそれ以外
 */
bool ViewerManager::_IsFinishUpdateViewer( void )
{
  return (!m_bUpdateViewer && m_ReqSeq == 0);
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック：公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   リクエストされているかどうか
 *
 * @retval  "true"ならリクエストされている
 * @retval  "false"ならリクエストされていない
 */
bool ViewerManager::IsRequestUpdateViewer( void )
{
  RequestViewerParam defaultParam = DEFAULT_REQUEST_VIEWER_PARAM;

  // Viewer更新パラメータの比較チェック
  // 不一致の場合はViewerを更新する。
  return !_IsCompareRequestParam( m_ReqParam, defaultParam );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   チェック：リクエストパラメータの比較チェック
 *
 * @param[in]   param1    比較対象１
 * @param[in]   param2    比較対象２
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool ViewerManager::_IsCompareRequestParam( const RequestViewerParam param1, const RequestViewerParam param2 )
{
  bool result = true;

  result &= ( param1.captureMode == param2.captureMode );
  result &= _IsCompareSimpleParam( param1.simpleParam, param2.simpleParam );
  result &= ( param1.bgID == param2.bgID );
  result &= ( param1.timeZone == param2.timeZone );
  result &= _IsCompareDressupParam( param1.dressupParam, param2.dressupParam );
  result &= ( param1.pokeMotionType == param2.pokeMotionType );
  result &= ( param1.composition == param2.composition );
  result &= ( param1.bViewerReset == param2.bViewerReset );
  result &= ( param1.bTrainerMotionReset == param2.bTrainerMotionReset );

  return result;
}

/**
 * @brief   チェック：ポケモン簡易パラメータの比較チェック
 *
 * @param[in]   param1    比較対象１
 * @param[in]   param2    比較対象２
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool ViewerManager::_IsCompareSimpleParam( const PokeTool::SimpleParam& param1, const PokeTool::SimpleParam& param2 )
{
  return PokeTool::CompareSimpleParam( param1, param2 );
}

/**
 * @brief   チェック：衣装データの比較チェック
 *
 * @param[in]   param1    比較対象１
 * @param[in]   param2    比較対象２
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool ViewerManager::_IsCompareDressupParam( const poke_3d::model::DressUpParam param1, const poke_3d::model::DressUpParam param2 )
{
  bool result = true;

  /// 身体パーツ
  for( u32 i = 0; i < poke_3d::model::DressUpParam::BODY_CATEGORY_COUNT; ++i )
  {
    result &= ( param1.bodyParams[i] == param2.bodyParams[i] );
  }
  /// 衣装パーツ
  for( u32 i = 0; i < poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i )
  {
    result &= ( param1.itemParams[i] == param2.itemParams[i] );
  }

  return result;
}

/// ---[ ViewerManagerクラス end ]---
//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////
/// ---[ ViewerManagerBaseクラス begin ]---


//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
ViewerManagerBase::ViewerManagerBase( void )
  : m_pViewer( NULL )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    生成・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   Viewer生成
 *
 * @param[in]   heap          ヒープ
 * @param[in]   simpleParam   ポケモン簡易パラメータ
 * @param[in]   dressupParam  着せ替えデータ
 */
void ViewerManagerBase::_CreateViewer( gfl2::heap::HeapBase* heap, const PokeTool::SimpleParam& simpleParam, const poke_3d::model::DressUpParam& dressupParam )
{
  GFL_ASSERT( m_pViewer == NULL );
  GFL_ASSERT( heap );
  GFL_ASSERT( simpleParam.monsNo != MONSNO_NULL );

  // Viewer生成
  m_pViewer = GFL_NEW( heap ) Viewer::FinderStudioViewer( heap, simpleParam, dressupParam );
}

/**
 * @brief   Viewer生成
 *
 * @param[in]   heap          ヒープ
 * @param[in]   photoParam    写真復帰パラメータ
 * @param[in]   viewerMode    Viewerモード
 */
void ViewerManagerBase::_CreateViewer( gfl2::heap::HeapBase* heap, const Viewer::PhotoParam& photoParam, Viewer::ViewerMode viewerMode )
{
  GFL_ASSERT( m_pViewer == NULL );
  GFL_ASSERT( heap );

  // Viewer生成
  m_pViewer = GFL_NEW( heap ) Viewer::FinderStudioViewer( heap, photoParam, viewerMode );
}

/**
 * @brief   Viewer破棄
 */
void ViewerManagerBase::_DeleteViewer( void )
{
  GFL_SAFE_DELETE( m_pViewer );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化処理完了待ち
 *
 * @retval  "true"なら完了
 * @retval  "false"なら継続
 */
bool ViewerManagerBase::_IsFinishInitialize( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->InitFunc();
}

/**
 * @brief   終了処理完了待ち
 */
bool ViewerManagerBase::_IsFinishTerminate( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->EndFunc();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void ViewerManagerBase::_Update( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->Update();
}

/**
 * @brief   描画
 *
 * @param[in]   dispNo  ディスプレイNo
 */
void ViewerManagerBase::_Draw( gfl2::gfx::CtrDisplayNo dispNo )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->Draw( dispNo );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    Viewerリクエスト関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   リクエスト関数：Viewerリセット
 */
void ViewerManagerBase::_ViewerReset( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->ResetViewer();
}

/**
 * @brief   リクエスト関数：撮影モードの変更
 *
 * @param[in]   mode    撮影モード
 */
void ViewerManagerBase::_ChangeCaptureMode( Viewer::CaptureMode mode )
{
  GFL_ASSERT( m_pViewer );
  GFL_ASSERT( mode < Viewer::CAPTURE_MODE_MAX );

  m_pViewer->ChangeCaptureMode( mode );
}

/**
 * @brief   リクエスト関数：ポケモンの変更開始
 *
 * @param[in]   simpleParam   ポケモン簡易パラメータ
 */
void ViewerManagerBase::_ChangePokemon( PokeTool::SimpleParam simpleParam )
{
  GFL_ASSERT( m_pViewer );
  GFL_ASSERT( simpleParam.monsNo != MONSNO_NULL );

  m_pViewer->StartChangePokeModel( simpleParam );
}

/**
 * @brief   リクエスト関数：ポケモンの変更完了待ち
 *
 * @retval  "true"なら完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsFinishChangePokemon( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsFinishChangePokeModel();
}

/**
 * @brief   リクエスト関数：背景の変更開始
 *
 * @param[in]   bgID    背景ID
 */
void ViewerManagerBase::_ChangeBackGround( u32 bgID )
{
  GFL_ASSERT( m_pViewer );
  GFL_ASSERT( bgID < BG_ID_NUM );

  m_pViewer->StartChangeBackGround( bgID );
}

/**
 * @brief   リクエスト関数：背景の変更完了待ち
 *
 * @retval  "true"なら完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsFinishChangeBackGround( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsFinishChangeBackGround();
}

/**
 * @brief   リクエスト関数：時間帯変更
 *
 * @param[in]   timeZone    時間帯
 */
void ViewerManagerBase::_ChangeTimeZone( Viewer::TimeZone timeZone )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->ChangeTimeZone( timeZone );
}

/**
 * @brief   リクエスト関数：構図変更
 *
 * @param[in]   composition   構図ID
 */
void ViewerManagerBase::_ChangeComposition( Viewer::Composition composition )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->ChangeComposition( composition );
}

/**
 * @brief   リクエスト関数：指定の構図が使用可能かどうか
 *
 * @param[in]   composition   構図ID
 *
 * @retval  "true"なら使用可能
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsAvailableComposition( Viewer::Composition composition )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsCanUseComposition( composition );
}

/**
 * @brief   リクエスト関数：構図の取得
 *
 * @retval  構図ID
 */
Viewer::Composition ViewerManagerBase::_GetComposition( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetComposition();
}

/**
 * @brief   リクエスト関数：衣装変更開始
 *
 * @param[in]   dressupParam    衣装データ
 */
void ViewerManagerBase::_ChangeDressup( poke_3d::model::DressUpParam dressupParam )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->StartChangeDressUp( dressupParam );
}

/**
 * @brief   リクエスト関数：衣装変更完了待ち
 *
 * @retval  "true"なら完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsFinishChangeDressup( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsFinishChangeDressUp();
}


/**
 * @brief   リクエスト関数：トレーナーのＹ軸回転 左回り
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_TrainerYAxisRotationLeft( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->TrainerRotateLeft();
}

/**
 * @brief   リクエスト関数：トレーナーのＹ軸回転 右回り
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_TrainerYAxisRotationRight( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->TrainerRotateRight();
}

/**
 * @brief   リクエスト関数：トレーナーのＹ軸回転値を取得(Viewer上の回転値)
 *
 * @retval  回転角度：0～360
 */
f32  ViewerManagerBase::_GetTrainerYAxisRotationOriginalDegree( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetTrainerDegY();
}

/**
 * @brief   リクエスト関数：トレーナーのＹ軸回転値を設定(UI上の回転値)
 *
 * @param[in]   degree    角度
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_SetTrainerYAxisRotationDegree( f32 degree )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->SetTrainerRotate( degree );
}

/**
 * @brief   リクエスト関数：トレーナーのＹ軸回転値を取得(UI上の回転値)
 *
 * @retval  回転角度：0～360
 */
f32  ViewerManagerBase::_GetTrainerYAxisRotationDegree( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetTrainerDegYIgnoreCollision();
}

/**
 * @brief   リクエスト関数：トレーナーAABBの取得
 *
 * @retval  AABB
 */
gfl2::math::AABB ViewerManagerBase::_GetTrainerAABB( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetTrainerAABB();
}

/**
 * @brief   リクエスト関数：トレーナーモーションの再生
 *
 * @param[in]   type    モーションタイプ
 */
void ViewerManagerBase::_TrainerMotionPlay( Viewer::ZMotionType type )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->PlayZMotion( type );
}

/**
 * @brief   リクエスト関数：トレーナーモーションの停止
 */
void ViewerManagerBase::_TrainerMotionStop( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->StopZMotion();
}

/**
 * @brief   リクエスト関数：トレーナーモーションのリセット
 *
 * @note    強制的に待機モーションにする。
 *          モーションの繋ぎがないので上画面はカーテンで見えないようにする必要がある。
 */
void ViewerManagerBase::_TrainerMotionReset( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->ChangeTrainerWaitState();
}

/**
 * @brief   リクエスト関数：トレーナーのモーションが再生中かどうか
 *
 * @retval  "true"なら再生中
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsPlayingTrainerMotion( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsPlayingZMotion();
}


/**
 * @brief   リクエスト関数：ポケモンの位置を取得(Viewer上の位置座標)
 *
 * @retval  ポケモンの位置座標
 */
gfl2::math::Vector3 ViewerManagerBase::_GetPokemonOriginalPosition( void )
{
  GFL_ASSERT( m_pViewer );

  gfl2::math::Vector3 vec = m_pViewer->GetPokePosition();
  gfl2::math::Vector3 position( vec.x, vec.z, 0 );

  return position;
}

/**
 * @brief   リクエスト関数：ポケモンの位置を設定(UI上の位置座標)
 *
 * @param[in]   position    ポケモンの位置座標(スクリーン座標)
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_SetPokemonPosition( const gfl2::math::Vector3 position )
{
  GFL_ASSERT( m_pViewer );

  gfl2::math::Vector2 vec( position.x, position.y );

  return m_pViewer->SetPokePosition( vec );
}

/**
 * @brief   リクエスト関数：ポケモンの位置を取得(UI上の位置座標)
 *
 * @retval  ポケモンの位置座標(スクリーン座標)
 */
gfl2::math::Vector3 ViewerManagerBase::_GetPokemonPosition( void )
{
  GFL_ASSERT( m_pViewer );

  gfl2::math::Vector2 vec = m_pViewer->GetPokePositionScreen();
  gfl2::math::Vector3 position( vec.x, vec.y, 0 );

  return position;
}

/**
 * @brief   リクエスト関数：ポケモンのＹ軸回転 左回り
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_PokemonYAxisRotationLeft( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->PokemonRotateLeft();
}

/**
 * @brief   リクエスト関数：ポケモンのＹ軸回転 右回り
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_PokemonYAxisRotationRight( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->PokemonRotateRight();
}

/**
 * @brief   リクエスト関数：ポケモンのＹ軸回転値の取得(Viewer上の回転値)
 *
 * @retval  回転角度：0～360
 */
f32  ViewerManagerBase::_GetPokemonYAxisRotationOriginalDegree( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetPokemonDegY();
}

/**
 * @brief   リクエスト関数：ポケモンのＹ軸回転値の設定(UI上の回転値)
 *
 * @param[in]   degree    角度
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_SetPokemonYAxisRotationDegree( f32 degree )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->SetPokeRotate( degree );
}

/**
 * @brief   リクエスト関数：ポケモンのＹ軸回転値の取得(UI上の回転値)
 *
 * @retval  回転角度：0～360
 */
f32  ViewerManagerBase::_GetPokemonYAxisRotationDegree( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetPokemonDegYIgnoreCollision();
}


/**
 * @brief   リクエスト関数：ポケモンのAABBの取得
 *
 * @retval  AABB
 */
gfl2::math::AABB ViewerManagerBase::_GetPokemonAABB( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetPokemonAABB();
}


/**
 * @brief   リクエスト関数：ポケモンのモーション再生
 */
void ViewerManagerBase::_PokemonMotionPlay( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->PlayPokeMotion();
}

/**
 * @brief   リクエスト関数：ポケモンのモーションオート再生の有効設定
 *
 * @param[in]   bAuto   有効フラグ
 */
void ViewerManagerBase::_SetPokemonMotionAutoPlayEnable( bool bAuto )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->SetPokeEnableAutoMotion( bAuto );
}

/**
 * @brief   リクエスト関数：ポケモンのモーションタイプ変更の開始
 *
 * @param[in]   type    モーションタイプ
 * @param[in]   bReset  trueならリセットする、falseならリセットしない
 */
void ViewerManagerBase::_ChangePokemonMotionType( Viewer::PokeMotionType type, bool bReset )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->ChangePokeMotionType( type, bReset );
}

/**
 * @brief   リクエスト関数：ポケモンのモーションタイプ変更の完了
 *
 * @retval  "true"なら完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsFinishChangePokemonMotionType( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsChangedPokeMotionType();
}

/**
 * @brief   リクエスト関数：ポケモンのモーションタイプ変更時にリセットが必要か？
 *
 * @param[in]   type    現在のモーションタイプ
 *
 * @retval  "true"なら必要
 * @retval  "false"なら不必要
 */
bool ViewerManagerBase::_IsNeedResetPokemonMotionType( Viewer::PokeMotionType type )
{
  GFL_ASSERT( m_pViewer );
  GFL_ASSERT( type < Viewer::POKE_MOTION_TYPE_MAX );

  Viewer::PokeMotionType motionType = Viewer::POKE_MOTION_TYPE_BATTLE;

  // バトルモーションの場合は可愛がりモーションで調べ、
  // 可愛がりモーションの場合はバトルモーションで調べる。
  if( type == Viewer::POKE_MOTION_TYPE_BATTLE )
  {
    motionType = Viewer::POKE_MOTION_TYPE_KAWAIGARI;
  }

  return !m_pViewer->IsCanChangePokeMotionTypeNoReset( motionType );
}

/**
 * @brief   リクエスト関数：再生可能なポケモンのモーションの割合を取得
 *
 * @retval  割合：0～1
 */
f32 ViewerManagerBase::_GetPokemonMotionPlayableRate( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetRatePokeCanPlayMotion();
}



/**
 * @brief   リクエスト関数：トレーナーとポケモンの衝突判定
 *
 * @retval  "true"なら衝突
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsCollisionTrainerAndPokemon( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsHitPokemonToTrainer();
}



/**
 * @brief   リクエスト関数：カメラのＸ軸回転 上方向
 */
void ViewerManagerBase::_CameraXAxisRotationUp( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraRotateXUp();
}

/**
 * @brief   リクエスト関数：カメラのＸ軸回転 下方向
 */
void ViewerManagerBase::_CameraXAxisRotationDown( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraRotateXDown();
}

/**
 * @brief   リクエスト関数：カメラのＹ軸回転 左周り
 */
void ViewerManagerBase::_CameraYAxisRotationLeft( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraRotateYLeft();
}

/**
 * @brief   リクエスト関数：カメラのＹ軸回転 右周り
 */
void ViewerManagerBase::_CameraYAxisRotationRight( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraRotateYRight();
}

/**
 * @brief   リクエスト関数：カメラのＹ軸回転角度の取得
 *
 * @retval  角度：0～360
 */
f32  ViewerManagerBase::_GetCameraYAxisRotationDegree( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetCameraDegreeY();
}


/**
 * @brief   リクエスト関数：カメラのＹ軸平行移動　上方向
 */
void ViewerManagerBase::_CameraYAxisTranslationUp( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraHeightUp();
}

/**
 * @brief   リクエスト関数：カメラのＹ軸平行移動　下方向
 */
void ViewerManagerBase::_CameraYAxisTranslationDown( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraHeightDown();
}

/**
 * @brief   リクエスト関数：カメラのＹ軸位置の割合の設定
 *
 * @param[in]   rate    割合：0～1
 */
void ViewerManagerBase::_SetCameraYAxisTranslationRate( f32 rate )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->SetCameraHeightRate( rate );
}

/**
 * @brief   リクエスト関数：カメラのＹ軸位置の割合の取得
 *
 * @retval  割合：0～1
 */
f32  ViewerManagerBase::_GetCameraYAxisTranslationRate( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetCameraHeightRate();
}


/**
 * @brief   リクエスト関数：カメラのズームイン
 */
void ViewerManagerBase::_CameraZoomIn( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraZoomIn();
}

/**
 * @brief   リクエスト関数：カメラのズームアウト
 */
void ViewerManagerBase::_CameraZoomOut( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraZoomOut();
}

/**
 * @brief   リクエスト関数：カメラのズーム割合の設定
 *
 * @param[in]   rate    割合：0～1
 */
void ViewerManagerBase::_SetCameraZoomRate( f32 rate )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->SetCameraZoomRate( rate );
}

/**
 * @brief   リクエスト関数：カメラのズーム割合の取得
 *
 * @retval  割合：0～1
 */
f32  ViewerManagerBase::_GetCameraZoomRate( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetCameraZoomRate();
}


/**
 * @brief   リクエスト関数：カメラのフォーカス設定 トレーナーへ
 */
void ViewerManagerBase::_CameraFocusTrainer( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraFocusToPlayer();
}

/**
 * @brief   リクエスト関数：カメラのフォーカス設定 ポケモンへ
 */
void ViewerManagerBase::_CameraFocusPokemon( void )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->CameraFocusToPokemon();
}

/**
 * @brief   リクエスト関数：カメラのフォーカス割合の設定
 *
 * @param[in]   rate    割合：0～1
 */
void ViewerManagerBase::_SetCameraFocusRate( f32 rate )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->SetCameraFocusRate( rate );
}

/**
 * @brief   リクエスト関数：カメラのフォーカス割合の取得
 *
 * @retval  割合：0～1
 */
f32  ViewerManagerBase::_GetCameraFocusRate( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->GetCameraFocusRate();
}



/**
 * @brief   リクエスト関数：写真再現パラメータの取得
 *
 * @param[out]  dstParam    再現パラメータの格納先
 */
void ViewerManagerBase::_GetPhotoParam( Viewer::PhotoParam* dstParam )
{
  GFL_ASSERT( m_pViewer );
  GFL_ASSERT( dstParam );

  m_pViewer->SavePhotoParam( dstParam );
}



/**
 * @brief   リクエスト関数：撮影開始
 *
 * @param[out]  dstBufferArray        写真データバッファの格納先
 * @param[out]  dstPhotoParamArray    写真再現パラメータの格納先
 */
void ViewerManagerBase::_CaptureStart( void** dstBufferArray, Viewer::PhotoParam* dstPhotoParamArray )
{
  GFL_ASSERT( m_pViewer );

  m_pViewer->StartCapture( dstBufferArray, dstPhotoParamArray );
}

/**
 * @brief   リクエスト関数：撮影完了待ち
 *
 * @retval  "true"なら完了
 * @retval  "false"ならそれ以外
 */
bool ViewerManagerBase::_IsFinishCapture( void )
{
  GFL_ASSERT( m_pViewer );

  return m_pViewer->IsFinishCapture();
}


/// ---[ ViewerManagerBaseクラス end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
