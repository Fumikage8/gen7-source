//==============================================================================
/**
 * @file	FieldGimmickJoinFesta.cpp
 * @brief	フィールドギミック：ジョインフェスタ
 * @author	ichiraku_katsuhiko
 * @date	2015/06/09 13:08:32
 */
// =============================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include <niji_conv_header/field/zone/zone_id.h> // zoneID

// ジョインフェスタ用カメラ
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/FieldCameraUiDeviceTranslator.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeJoinFesta.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

// ジョインフェスタ動的動作モデルロードシステム
#include <ro/include/gfl2_RoManager.h>
#include "system/include/HeapDefine.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"

// ジョインフェスタエフェクトシステム
#include "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystem.h"
//  ジョインフェスタリソースマネージャ
#include "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"
// ジョインフェスタ施設システム
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesSystem.h"

// 下画面
#include "Field\FieldRo\include\Subscreen\FieldSubScreenProcManager.h"

// mine
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"


#if PM_DEBUG
// デバッグ機能
#include "NetApp/JoinFesta/include/Debug/JoinFestaDebug.h"
#include "NetApp/JoinFesta/include/Debug/JoinFestaFacilitiesSystemDebug.h"
#endif

GFL_NAMESPACE_BEGIN( Field )


// コンストラクタ
FieldGimmickJoinFesta::FieldGimmickJoinFesta( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap ) :
  FieldGimmick(pMap)
  ,m_pFieldDeviceHeap(pHeap)
  ,m_pJFDynamicLoadSystem(NULL)
  ,m_pEffectSystem(NULL)
  ,m_pResourceManager(NULL)
  ,m_pFacilitiesSystem(NULL)
  ,m_pJoinFestaRoModule(NULL)
  ,m_pKisekaeDataRoModule(NULL)
  ,m_ResourceManagerHeap()
  ,m_initializeSeq(INITIALIZE_SEQ_START)
  ,m_updateInitializeSeq(UPDATE_INITIALIZE_SEQ_START)
  ,m_charSetupSeq(CHARA_SETUP_SEQ_LOAD_1)
#if PM_DEBUG
  ,m_pJoinFestaDebugGroup(NULL)
#endif
#ifdef JF_DEBUG_MENU
  ,m_pDebugMenu(NULL)
  ,m_pFacilitiesDebug(NULL)
#endif
{
  // リソースマネージャ用ヒープ作成
  {
    gfl2::heap::HeapBase* pFieldEventHeap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldExtHeap();
    gfl2::heap::NwAllocator*      pNULLNwAllocator      = NULL;
    gfl2::util::GLHeapAllocator*  pNULLGLHeapAllocator  = NULL;
    m_ResourceManagerHeap.Create(
      pFieldEventHeap,     //  gfl2::heap::HeapBase        * heap,
      pFieldEventHeap,     //  gfl2::heap::HeapBase        * devHeap,
      pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * heapAllocator,
      pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * devHeapAllocator,
      pNULLGLHeapAllocator,   //  gfl2::util::GLHeapAllocator * glHeapAllocator,
      pNULLGLHeapAllocator    //  gfl2::util::GLHeapAllocator * devGLHeapAllocator
      );   /*  参照Create  */
  }


}
// デストラクタ
FieldGimmickJoinFesta::~FieldGimmickJoinFesta()
{

  m_ResourceManagerHeap.Delete();
}

/**
 *  @brief  生成
 */
void FieldGimmickJoinFesta::Create( void )
{
  ICHI_PRINT("FieldGimmickJoinFesta::Create\n");

  //
  // DebugGroup作成
  //
#if PM_DEBUG
  {
    m_pJoinFestaDebugGroup  = gfl2::debug::DebugWin_AddGroup( m_pFieldDeviceHeap, GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork )->GetMessageData(), msg_dmenu_joinfes_top );  // "ジョインフェスタ"
  }
#endif


  //
  // JF専用カメラ変更
  //
  {
    // JF用操作コントローラ作成
    Field::Camera::Controller::ControllerTypeJoinFesta*  pCameraController;  // カメラ操作、作るのはこのイベントだが、寿命管理をカメラシステムに任せるので外部扱い
    pCameraController = GFL_NEW( m_pFieldDeviceHeap ) Camera::Controller::ControllerTypeJoinFesta( m_pFieldmap );

    // 操作コントローラーをJF用操作コントローラに変更 @note 通常フィールドのコントローラーは切り替え時に破棄される
    m_pFieldmap->GetCameraManager()->ChangeFieldController( pCameraController );

    // 現在のカメラをスティック入力に即時反映する
    m_pFieldmap->GetCameraUiDeviceTranslator()->SetMode( Camera::UiDeviceTranslator::Mode::DIRECT );
  }

  // ジョインフェスタDLL読み込み
  {
    GameSys::GameManager* pGameManager = m_pFieldmap->GetGameManager();
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    // 着せ替えデータを参照するのにdll読みが必要
    m_pKisekaeDataRoModule = roMgr->LoadModule(pGameManager->GetAsyncFileManager(), "KisekaeData.cro");
    roMgr->StartModule(m_pKisekaeDataRoModule, true);

    // ジョインフェスタDLL
    m_pJoinFestaRoModule = roMgr->LoadModule(pGameManager->GetAsyncFileManager(), "JoinFesta.cro");
    roMgr->StartModule(m_pJoinFestaRoModule, true);
  }


  // 各システムインスタンス作成
  {
    gfl2::heap::HeapBase* heap = m_pFieldDeviceHeap;

    // JFDynamicLoadSystem作成
    m_pJFDynamicLoadSystem = GFL_NEW( heap ) NetApp::JoinFesta::JFDynamicLoadSystem( heap, m_pFieldmap );

    // リソースマネージャ
    m_pResourceManager  = GFL_NEW( heap ) NetApp::JoinFesta::JoinFestaResourceManager( &m_ResourceManagerHeap );

    // エフェクトシステム作成
#ifdef  JF_DEBUG_MENU
    m_pEffectSystem = GFL_NEW(heap) NetApp::JoinFesta::JoinFestaEffectSystem( heap, m_pResourceManager, GetDebugGroup() );
#else
    m_pEffectSystem = GFL_NEW(heap) NetApp::JoinFesta::JoinFestaEffectSystem( heap, m_pResourceManager );
#endif

    // 施設システム
    m_pFacilitiesSystem = GFL_NEW( heap ) NetApp::JoinFesta::JoinFestaFacilitiesSystem( heap, false );
  }

  // 初期化リクエスト関連
  {
    // JFリソース
    m_pResourceManager->RequestLoad();
    m_pResourceManager->ObserveLoad();
  }

#ifdef JF_DEBUG_MENU
  // デバッグメニュー作成
  DebugMenu_Init();
#endif // JF_DEBUG_MENU
}
void FieldGimmickJoinFesta::UpdateCreate( void )
{
  ICHI_PRINT("FieldGimmickJoinFesta::UpdateCreate %d \n", m_initializeSeq);

  switch( m_initializeSeq ){
  case INITIALIZE_SEQ_START:                   //! 初期化最初
    m_initializeSeq = INITIALIZE_SEQ_RESOURCE_LOAD; 
    /* break through */

  case INITIALIZE_SEQ_RESOURCE_LOAD:           //! リソース読み込み待ち
    {
      // リソース読み込み完了待ち
      if( !m_pResourceManager->ObserveLoad() )
      {
        break;
      }

      m_initializeSeq = INITIALIZE_SEQ_MODULE_INIT_1; 
    }
    /* break through */

  case INITIALIZE_SEQ_MODULE_INIT_1:             //! 各システム初期化開始
    {
      // 施設初期化
      m_pFacilitiesSystem->Initialize( m_pResourceManager->GetDataTablePtr() );
      m_initializeSeq = INITIALIZE_SEQ_MODULE_WAIT_1; 
    }
    /* break through */

  case INITIALIZE_SEQ_MODULE_WAIT_1:             //! 各システム初期化待ち
    {
      // 施設システム初期化完了待ち
      if( !m_pFacilitiesSystem->IsInitializeComplete() )
      {
        break;
      }

      // エフェクトシステム初期化
      m_pEffectSystem->Initialize();
      m_initializeSeq = INITIALIZE_SEQ_MODULE_INIT_2; 
    }
    /* break through */
  case INITIALIZE_SEQ_MODULE_INIT_2:           //! 各システム初期化開始
    {
      // 話しかけ中だった場合は、話しかけ人物を表示する必要があるので人物システムも初期化する
      // 話しかけた相手のJFパーソナルデータ
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
      NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJoinFestaPersonalDataManager->GetScriptTalkFieldPersonalData(); 
      if( jfPersonal != NULL )
      {
        m_initializeSeq = INITIALIZE_SEQ_MODULE_WAIT_2; 
      }
      else
      {
        m_initializeSeq = INITIALIZE_SEQ_END; 
      }
    }
    /* break through */

  case INITIALIZE_SEQ_MODULE_WAIT_2:           //! 各システム初期化待ち
    {
      if( !InitializeInUpdate()  )
      {
        break;
      }
      m_initializeSeq = INITIALIZE_SEQ_END; 
    }
    /* break through */

  case INITIALIZE_SEQ_END:                     //! 初期化完了
    // 処理なし
    break;
  }
}
bool FieldGimmickJoinFesta::IsCreated( void )
{
  return ( m_initializeSeq == INITIALIZE_SEQ_END );
}

/**
 *  @biref  破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickJoinFesta::Finalize( void )
{
  // 破棄待ち
  if( m_pJFDynamicLoadSystem->Terminate() == NetApp::JoinFesta::RESULT_CONTINUE )
  {
    return false;
  }
  if( m_pEffectSystem->Terminate() == NetApp::JoinFesta::JoinFestaEffectSystem::RESULT_CONTINUE)
  {
    return false;
  }
  if( m_pFacilitiesSystem->Terminate() == NetApp::JoinFesta::RESULT_CONTINUE )
  {
    return false;
  }

  m_pResourceManager->ReleaseData();

  return true;
}


/**
 *  @brief  破棄
 */
void FieldGimmickJoinFesta::Delete( void )
{
  ICHI_PRINT("FieldGimmickJoinFesta::Delete\n");

#ifdef JF_DEBUG_MENU
  // デバッグメニュー破棄
  DebugMenu_Exit();
#endif // JF_DEBUG_MENU

  GFL_SAFE_DELETE( m_pFacilitiesSystem );
  GFL_SAFE_DELETE( m_pJFDynamicLoadSystem );
  GFL_SAFE_DELETE( m_pEffectSystem );
  GFL_SAFE_DELETE( m_pResourceManager );
  

  //
  // JF専用カメラ変更戻す
  //
  {
    // JF用操作コントローラを破棄 @note この後、フィールドが破棄されずに通常フィールドになる場合はコントローラーを作り直す必要がある。。。
    m_pFieldmap->GetCameraManager()->ChangeFieldController( NULL );
  }
  //
  // DebugGroup
  //
#if PM_DEBUG
  {
    if(m_pJoinFestaDebugGroup)
    {
      gfl2::debug::DebugWin_RemoveGroup(m_pJoinFestaDebugGroup);
      m_pJoinFestaDebugGroup  = NULL;
    }
  }
#endif


  // ジョインフェスタDLLアンロード
  {
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    roMgr->DisposeModule( m_pJoinFestaRoModule );
    roMgr->DisposeModule( m_pKisekaeDataRoModule );
  }
}

/**
 *  @brief  Ready中1度だけ呼び出し
 */
void FieldGimmickJoinFesta::Ready( void )
{
  ICHI_PRINT("FieldGimmickJoinFesta::Ready\n");
}
  
/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  キャラクタ前更新よりも手前で呼び出される
 */
void FieldGimmickJoinFesta::UpdateFirst( void )
{
  // フィールド中の初期化が終わらないと、人物システムを更新できない
  if( InitializeInUpdate()  )
  {
    m_pJFDynamicLoadSystem->Update();
    m_pEffectSystem->Update();
  }

  m_pFacilitiesSystem->UpdateAnime();
}
/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickJoinFesta::UpdateLast( void )
{
}

/**
 *  @brief  更新
 *  Fieldmap::UpdateNormalにて呼び出される
 *  トラバース直前に呼び出される
 */
void FieldGimmickJoinFesta::TraverseBefore( void )
{
  // 処理負荷を池内さんに相談し、下記の方針でいくことになった
  /*
  カリング状態が無効の時に、動作モデルの要素が偶数のものを最初にトラーバス対象からハズす
  次は奇数の物を外し偶数のものをリセットし、トラバース負荷を平均的に下げる
  */

  // フィールド中の初期化が終わらないと、人物システムを更新できない
  if( InitializeInUpdate()  )
  {
    m_pJFDynamicLoadSystem->TraverseBefore();
  }
}

/**
 *  @brief  更新
 *  Fieldmap::UpdateNormalにて呼び出される
 */
void FieldGimmickJoinFesta::CalculateAfter( void )
{
}

/**
 *  @brief  特殊ステップチェック
 */
bool FieldGimmickJoinFesta::MoveRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}

/**
 *  @brief  イベント起動チェック
 */
bool FieldGimmickJoinFesta::EventRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   Updateで初期化
 * @return  初期化完了でtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool FieldGimmickJoinFesta::InitializeInUpdate()
{

  switch( m_updateInitializeSeq ){
  case UPDATE_INITIALIZE_SEQ_START:                   //! 初期化最初
    {
      m_updateInitializeSeq = UPDATE_INITIALIZE_SEQ_MODULE_INIT_1;
    }
    /* break through */
  case UPDATE_INITIALIZE_SEQ_MODULE_INIT_1:           //! 各システム初期化開始
    {
      // 人物システム初期化
      m_pJFDynamicLoadSystem->Initialize( m_pEffectSystem, m_pResourceManager->GetDataTablePtr() );

      m_updateInitializeSeq = UPDATE_INITIALIZE_SEQ_MODULE_WAIT_1;
    }
    /* break through */
  case UPDATE_INITIALIZE_SEQ_MODULE_WAIT_1:           //! 各システム初期化待ち
    {
      bool isInitializeEnd = true;
      // キャラクターリソース
      if( !CharaModelResourceSetup() )
      {
        isInitializeEnd = false;
      }
      if( !m_pJFDynamicLoadSystem->IsInitializeComplete() )
      {
        isInitializeEnd = false;
      }
      if( isInitializeEnd == false )
      {
        break;
      }
      m_updateInitializeSeq = UPDATE_INITIALIZE_SEQ_END;
    }
    /* break through */

  case UPDATE_INITIALIZE_SEQ_END:                     //! 初期化完了
    return true;
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   キャラクターリソースのセットアップ（ロード）
 * @return  セットアップ完了でtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool FieldGimmickJoinFesta::CharaModelResourceSetup()
{
  // 一楽くんへ　フィールドキャラクターのリソースはAreaに必要な物(EventEditorではいちされている）のみ
  // 読み込むように変更しました。　なので、JFで使用しているリソースをここで読み込みます。
  // ジョインフェスタで使用するリソースの読み込みをリクエスト
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = m_pFieldmap->GetGameManager()->GetGameData()->GetFieldCharaModelManager();
  // charcterIdが0と1(=CHARA_MODEL_ID_PC0001_00とCHARA_MODEL_ID_PC0002_00)のモデルリソースノードがないと、
  // その性別の着せ替えがセットアップ出来ない。

  switch( m_charSetupSeq ){
  case CHARA_SETUP_SEQ_LOAD_1:            //! キャラクターリソースロード
    {
      pFieldCharaModelManager->LoadStaticAsync(CHARA_MODEL_ID_PC0001_00);
      m_charSetupSeq = CHARA_SETUP_SEQ_WAIT_1; 
    }
    /* break through */

  case CHARA_SETUP_SEQ_WAIT_1:            //! キャラクターリソースロード待ち
    {
      if( !pFieldCharaModelManager->IsLoaded(CHARA_MODEL_ID_PC0001_00) )
      {
        break;
      }

      // キャラクターリソースノードの初期化
      pFieldCharaModelManager->SetupModel(CHARA_MODEL_ID_PC0001_00);
      m_charSetupSeq = CHARA_SETUP_SEQ_LOAD_2; 
    }
    /* break through */

  case CHARA_SETUP_SEQ_LOAD_2:            //! キャラクターリソースロード
    {
      pFieldCharaModelManager->LoadStaticAsync(CHARA_MODEL_ID_PC0002_00);
      m_charSetupSeq = CHARA_SETUP_SEQ_WAIT_2; 
    }
    /* break through */

  case CHARA_SETUP_SEQ_WAIT_2:            //! キャラクターリソースロード待ち
    {
      if( !pFieldCharaModelManager->IsLoaded(CHARA_MODEL_ID_PC0002_00) )
      {
        break;
      }

      // キャラクターリソースノードの初期化
      pFieldCharaModelManager->SetupModel(CHARA_MODEL_ID_PC0002_00);
      m_charSetupSeq = CHARA_SETUP_SEQ_END; 
    }
    /* break through */

  case CHARA_SETUP_SEQ_END:               //! 初期化完了
    return true;
  default:
    GFL_ASSERT_MSG(0,"%d",m_charSetupSeq);
    return true;
  }

  return false;
}

#ifdef JF_DEBUG_MENU
//=============================================================================
/**
 *    デバッグメニュー
 */
//=============================================================================
// デバッグメニュー作成
void FieldGimmickJoinFesta::DebugMenu_Init()
{
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
  m_pDebugMenu  = GFL_NEW(pHeap) NetApp::JoinFesta::JoinFestaDebug( m_pJFDynamicLoadSystem, m_pResourceManager->GetDataTablePtr(), m_pResourceManager, m_pFacilitiesSystem );
  m_pFacilitiesDebug  = GFL_NEW(pHeap) NetApp::JoinFesta::JoinFestaFacilitiesSystemDebug( m_pFacilitiesSystem, m_pResourceManager );
}
// デバッグメニュー破棄
void FieldGimmickJoinFesta::DebugMenu_Exit()
{
  GFL_SAFE_DELETE(m_pFacilitiesDebug);
  GFL_SAFE_DELETE(m_pDebugMenu);
}
#endif // JF_DEBUG_MENU


GFL_NAMESPACE_END( Field )
