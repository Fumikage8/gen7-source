//==============================================================================
/**
 * @file	FieldGimmickJoinFestaRoom.cpp
 * @brief	フィールドギミック：ジョインフェスタ室内
 * @author	ichiraku_katsuhiko
 * @date	2015/06/20 13:09:29
 */
// =============================================================================
#include "Field/FieldRos/FieldGimmickJoinFestaRoom/include/FieldGimmickJoinFestaRoom.h"

#include <debug/include/gfl2_DebugPrint.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include <niji_conv_header/field/zone/zone_id.h> // zoneID

// ジョインフェスタ動的動作モデルロードシステム
#include <ro/include/gfl2_RoManager.h>
#include "system/include/HeapDefine.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"
//  ジョインフェスタリソースマネージャ
#include "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"
// ジョインフェスタ施設システム
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesSystem.h"
// ジョインフェスタエフェクトシステム
#include "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystem.h"
// バトルフェス
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesPokeLotterySystem.h"
#include <niji_conv_header\field\static_model\static_model_id.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include "Savedata/include/EventWork.h"

GFL_NAMESPACE_BEGIN( Field )

// コンストラクタ
FieldGimmickJoinFestaRoom::FieldGimmickJoinFestaRoom( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap ) :
  FieldGimmick(pMap)
  ,m_pFieldDeviceHeap(pHeap)
  ,m_pJFDynamicLoadSystem(NULL)
  ,m_pEffectSystem(NULL)
  ,m_pResourceManager(NULL)
  ,m_pFacilitiesSystem(NULL)
  ,m_pJoinFestaRoModule(NULL)
  ,m_pKisekaeDataRoModule(NULL)
  ,m_initializeSeq(INITIALIZE_SEQ_START)
  ,m_charSetupSeq(CHARA_SETUP_SEQ_LOAD_1)
#if PM_DEBUG
  ,m_pJoinFestaDebugGroup(NULL)
#endif
{
  // リソースマネージャ用ヒープ作成
  {
    gfl2::heap::NwAllocator*      pNULLNwAllocator      = NULL;
    gfl2::util::GLHeapAllocator*  pNULLGLHeapAllocator  = NULL;
    m_ResourceManagerHeap.Create(
      m_pFieldDeviceHeap,     //  gfl2::heap::HeapBase        * heap,
      m_pFieldDeviceHeap,     //  gfl2::heap::HeapBase        * devHeap,
      pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * heapAllocator,
      pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * devHeapAllocator,
      pNULLGLHeapAllocator,   //  gfl2::util::GLHeapAllocator * glHeapAllocator,
      pNULLGLHeapAllocator    //  gfl2::util::GLHeapAllocator * devGLHeapAllocator
      );   /*  参照Create  */
  }

  m_pAllocator = GFL_NEW(pHeap) System::nijiAllocator(pHeap);

  for (u32 index(0); index < BattleFes::BattleFesDefine::POKEMON_MAX; ++index)
  {
    //キャプチャ画像
    m_pCapResourceNode[index] = NULL;
    m_pCapturePoke[index] = NULL;
    m_pMaterialNodeRef[index] = NULL;
  }
  m_ResourceManagerHeap.SetFileReadHeap(m_pFieldDeviceHeap);

  //m_pPokeCapAccesser
  {
    m_pPokeCapAccesser = GFL_NEW(m_pFieldDeviceHeap) AppLib::Tool::PokeCapAccesser(&m_ResourceManagerHeap, pMap->GetGameManager()->GetAsyncFileManager());
  }
}
// デストラクタ
FieldGimmickJoinFestaRoom::~FieldGimmickJoinFestaRoom()
{
  //キャプチャ
  if (m_pPokeCapAccesser)
  {
    {
      GFL_SAFE_DELETE(m_pPokeCapAccesser);
    }

    {
      for (u32 index(0); index < BattleFes::BattleFesDefine::POKEMON_MAX; ++index)
      {
        GflHeapFreeMemory(m_pCapturePoke[index]);
        GFL_SAFE_DELETE(m_pCapResourceNode[index]);
        //解除
        if (m_pMaterialNodeRef[index] != NULL)
        {
          m_pMaterialNodeRef[index]->SetTextureResourceNode(0, NULL);
          m_pMaterialNodeRef[index] = NULL;
        }
      }
    }
  }

  GFL_SAFE_DELETE(m_pAllocator);

  m_ResourceManagerHeap.Delete();
}

/**
 *  @brief  生成
 */
void FieldGimmickJoinFestaRoom::Create( void )
{
  ICHI_PRINT("FieldGimmickJoinFestaRoom::Create\n");

  //
  // DebugGroup作成
  //
#if PM_DEBUG
  {
    m_pJoinFestaDebugGroup  = gfl2::debug::DebugWin_AddGroup( m_pFieldDeviceHeap, GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork )->GetMessageData(), msg_dmenu_joinfes_top );  // "ジョインフェスタ"
  }
#endif

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
    m_pFacilitiesSystem = GFL_NEW( heap ) NetApp::JoinFesta::JoinFestaFacilitiesSystem( heap, true );

    // バトル抽選システム作成
    // @fix MMCat[663] 日付を跨いだ直後にバトルエージェントのレンタルポケモンの「つよさをみる」と、ラインナップポケモンの更新が発生してつよさをみる前後でのラインナップに相違が発生する
    BattleFes::BattleFesManager* pBattleFesManager = m_pFieldmap->GetGameManager()->GetGameData()->GetBattleFesManager();
    pBattleFesManager->CreateBattleFesPokeLotterySystem(heap);
    u32 randSeed = m_pFieldmap->GetGameManager()->GetGameData()->GetSaveData()->GetRandomGroup()->GetTodayRandSeed();
    if (randSeed != pBattleFesManager->GetMonitorRandSeed())
    {
      pBattleFesManager->SetMonitorRandSeed(randSeed);
    }
  }

  // 初期化リクエスト関連
  {
    // JFリソース
    m_pResourceManager->RequestLoad();
    m_pResourceManager->ObserveLoad();
  }

  //キャプチャ画像
  {
    // 開放前は画像表示しない対応
    Field::EventWork* ev = m_pFieldmap->GetGameManager()->GetGameData()->GetEventWork();
    bool isOpen = ev->CheckEventFlag(FE_BATTLE_FES_OPEN);
    if (isOpen)
    {
      m_pPokeCapAccesser->OpenPokemonCapture();
      BattleFes::BattleFesPokeLotterySystem* pBattleFesPokeLotterySystem = m_pFieldmap->GetGameManager()->GetGameData()->GetBattleFesManager()->GetBattleFesPokeLotterySystem();
      for (u32 index(0); index < BattleFes::BattleFesDefine::POKEMON_MAX; ++index)
      {
        //キャプチャデータの取得～ノード化
        {
          //pCapturePoke
          {
            int arci = m_pPokeCapAccesser->GetArcFileIndex(pBattleFesPokeLotterySystem->GetPokemon(index));
            m_pPokeCapAccesser->PokemonCaptureRead(arci, &m_pCapturePoke[index]);
          }

          //pCapResourceNode
          {
            gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
            {
              texData.SetTextureData(static_cast<c8*>(m_pCapturePoke[index]));
            }

            m_pCapResourceNode[index] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &texData);
          }
        }

        //実際の表示への反映
        {
          const char* MaterialNameArr[] = {
            "f0601jf0102_poke01",
            "f0601jf0102_poke02",
            "f0601jf0102_poke03",
          };

          //StaticModel
          StaticModel::StaticModelManager* pStaticModelManager = m_pFieldmap->GetStaticModelManager();
          StaticModel::StaticModel*        pStaticModel = pStaticModelManager->GetStaticModelIDStaticModel(STATIC_MODEL_ID_F0601JF0102_BM_COUNTER01);
          GFL_ASSERT_STOP(pStaticModel);
          //BaseModel
          poke_3d::model::BaseModel* pBaseModel = pStaticModel->GetBaseModel(pStaticModel->GetNowModelType());
          GFL_ASSERT(pBaseModel);

          int material_index = pBaseModel->GetMaterialIndex(MaterialNameArr[index]);

          gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pBaseModel->GetModelInstanceNode()->GetMaterialInstanceNode(material_index);

          gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTexNode = m_TexGetter.GetTextureResourceNode(m_pCapResourceNode[index]);
          pMaterialNode->SetTextureResourceNode(0, pTexNode);

          //解除時に使うために記憶
          m_pMaterialNodeRef[index] = pMaterialNode;
        }
      }
      m_pPokeCapAccesser->ClosePokemonCapture();
    }
  }
}


void FieldGimmickJoinFestaRoom::UpdateCreate( void )
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
      // 人物システム初期化
      m_pJFDynamicLoadSystem->Initialize( m_pEffectSystem, m_pResourceManager->GetDataTablePtr() );
      m_initializeSeq = INITIALIZE_SEQ_MODULE_WAIT_2; 
    }
    /* break through */

  case INITIALIZE_SEQ_MODULE_WAIT_2:           //! 各システム初期化待ち
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
      m_initializeSeq = INITIALIZE_SEQ_END; 
    }
    /* break through */

  case INITIALIZE_SEQ_END:                     //! 初期化完了
    // 処理なし
    break;
  }
}
bool FieldGimmickJoinFestaRoom::IsCreated( void )
{
  return ( m_initializeSeq == INITIALIZE_SEQ_END );
}

/**
 *  @biref  破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickJoinFestaRoom::Finalize( void )
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
void FieldGimmickJoinFestaRoom::Delete( void )
{
  ICHI_PRINT("FieldGimmickJoinFestaRoom::Delete\n");

  // バトル抽選システム破棄
  m_pFieldmap->GetGameManager()->GetGameData()->GetBattleFesManager()->DeleteBattleFesPokeLotterySystem();
  GFL_SAFE_DELETE( m_pFacilitiesSystem );
  GFL_SAFE_DELETE( m_pJFDynamicLoadSystem );
  GFL_SAFE_DELETE( m_pEffectSystem );
  GFL_SAFE_DELETE( m_pResourceManager );

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
void FieldGimmickJoinFestaRoom::Ready( void )
{
  ICHI_PRINT("FieldGimmickJoinFestaRoom::Ready\n");
  // 動的に出現させる仕組みは無効化させておく
  m_pJFDynamicLoadSystem->DynamickLoadDisable();
}
  
/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  キャラクタ前更新よりも手前で呼び出される
 */
void FieldGimmickJoinFestaRoom::UpdateFirst( void )
{
  m_pJFDynamicLoadSystem->Update();
  m_pEffectSystem->Update();
}
/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickJoinFestaRoom::UpdateLast( void )
{
}
/**
 *  @brief  更新
 *  Fieldmap::UpdateNormalにて呼び出される
 */
void FieldGimmickJoinFestaRoom::CalculateAfter( void )
{
}

/**
 *  @brief  特殊ステップチェック
 */
bool FieldGimmickJoinFestaRoom::MoveRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}

/**
 *  @brief  イベント起動チェック
 */
bool FieldGimmickJoinFestaRoom::EventRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   キャラクターリソースのセットアップ（ロード）
 * @return  セットアップ完了でtrue
 */
/* ----------------------------------------------------------------------------------------- */
bool FieldGimmickJoinFestaRoom::CharaModelResourceSetup()
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

GFL_NAMESPACE_END( Field )
