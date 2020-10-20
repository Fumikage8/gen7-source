//===================================================================
/**
 * @file    GameData.cpp
 * @brief   ゲーム進行データ管理
 * @author  Miyuki Iwasawa 
 * @date    2015.02.28
*/
//===================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <System/include/GflUse.h>
#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"

#include <PokeTool/include/PokeMemo.h>
#include <PokeTool/include/PokeToolMega.h>
#include <PokeTool/include/PokeToolForm.h>

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/ZukanSave.h"
#include "Savedata/include/MiscSave.h"
#include <Savedata/include/ISaveDataTotal.h>

#include <model/include/gfl2_CharaModelFactory.h>


#include "App/AppEvent/include/Bag/BagGamedata.h"
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#include "App/AppEvent/include/TrainerPass/TrainerPassGamedata.h"
#include "App/FieldMenu/include/FieldMenuFrame.h"

// キャラクターシステム
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/ActionCommand/FieldActionCommandManager.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldStatic/include/FieldWarpData.h"

#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"

// Zone
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h" 

// World
#include "Field/FieldStatic/include/FieldWorld.h"

// Area
#include "Field/FieldStatic/include/FieldArea.h"

// FieldItem
#include "Field/FieldStatic/include/Item/FieldItemManager.h"

// きのみ
#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"

// dressup
#include <niji_conv_header/field/warpdata/FieldWarpDataType.h>
#include <System/include/DressUp.h>

// 育て屋
#include <pml/include/pml_Const.h>
#include "Field/FieldStatic/include/Sodateya/FieldSodateyaWalkCheckData.h"

// ジョインフェスタセーブ
#include "Savedata/include/JoinFestaVipSave.h"
#include "Savedata/include/JoinFestaGuestSave.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>

#include <niji_conv_header/poke_lib/item/monsball_id.h>

// Finder
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"

// camera
#include "Field/FieldRo/include/Camera/FieldCameraTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"

// trainer
#include "Trainer/Trainer/include/TrainerPairData.h"

// Debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldStatic/include/Debug/DebugStartSettingAccessor.h"

// script
#include "FieldScript/include/FieldScriptSystem.h"

// バトルフェスマネージャー
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"

GFL_NAMESPACE_BEGIN(GameSys)

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief セーブデータに生成するファイル数
 * //@note  kujiraではリアル地域ポケモン情報、通常進行データの2件
 * @note  2013.03.20  通常進行データの1件のみに修正
 */
//--------------------------------------------------------------
enum {
  //SAVE_FILES_MAX = 1 + 1,
  SAVE_FILES_MAX = 1,
};

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------
GameData::GameData( gfl2::heap::HeapBase* deviceHeap, gfl2::ui::DeviceManager* uiDeviceManager ) : 
  m_JoinFestaSwitchFieldData()
#if PM_DEBUG
  , m_pDebugStartSettingAccessor( NULL )
#endif
{
  // アロケータ作成
  m_pDeviceHeapAllocator = GFL_NEW(deviceHeap) gfl2::util::GLHeapAllocator(deviceHeap);

  mIsLoaded = false;   //セーブデータを読み込んでいない
  mFieldMenuMode = App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL; //メニューを閉じた状態

  gfl2::heap::HeapBase* pSaveDataHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_SAVE_DATA );
  mSaveData = GFL_NEW(pSaveDataHeap) Savedata::SaveData(pSaveDataHeap);
  // ボックスポケモンのセーブデータにボックス画面のセーブデータをセット
  mSaveData->GetBoxPokemon()->SetBoxSaveData( mSaveData->GetBOX() );

#if   defined(GF_PLATFORM_CTR)
  mSaveControl = GFL_NEW(deviceHeap) Savedata::CTRSaveLoadControl(deviceHeap, SAVE_FILES_MAX );
#else
  mSaveControl = GFL_NEW(deviceHeap) Savedata::Win32SaveLoadControl();
#endif
  if(mSaveControl)
  {
    mSaveControl->AddBackupEntry( mSaveData, L"main" );
  }


  // 初期状態はセーブ可能
  mSaveLock = false;

  // プレイヤーパーティ生成（実セーブデータはSaveDataのメンバで保持）
  mPlayerParty  = GFL_NEW(deviceHeap)  pml::PokeParty( deviceHeap );

  // QRパーティ生成
  mQRParty  = GFL_NEW( deviceHeap)  pml::PokeParty( deviceHeap );
  mQRPartyName = GFL_NEW_ARRAY( deviceHeap ) gfl2::str::STRCODE[Savedata::BOX::TEAM_NAME_BUFSIZE];

  memset( m_QRBattleTeamID , 0 , sizeof(m_QRBattleTeamID) );

  mQRParam1 = 0;
  mQRParam2 = 0;

#if   defined(GF_PLATFORM_CTR)
  mSignatureBuffer = GFL_NEW_ARRAY( deviceHeap ) u8[NetLib::Validation::PokemonValidation::SIGNATURE_SIZE];
  ::std::memset( mSignatureBuffer , 0 , sizeof(NetLib::Validation::PokemonValidation::SIGNATURE_SIZE) );
#else
  mSignatureBuffer = NULL;
#endif
  // プレイ時間　生成（実セーブデータはSaveDataのメンバで保持）
  mPlayTime = GFL_NEW(deviceHeap) Savedata::PlayTime();

  // ゲーム中に保持し続けるバッグ画面のデータ
  mBagPlayData = GFL_NEW( deviceHeap ) app::bag::PlayData();
  // ゲーム中に保持し続けるフィールド下画面のデータ
  mFieldMenuPlayData = GFL_NEW( deviceHeap ) App::FieldMenu::PlayData();
  // ゲーム中に保持し続けるトレーナーパス画面のデータ
  mTrainerPassPlayData = GFL_NEW( deviceHeap ) App::TrainerPass::PlayData();

  // 動作モデル用
  m_pFieldRootNode            = GFL_NEW(deviceHeap) Field::FieldRootNode();
  m_pFieldCharaModelManager   = GFL_NEW(deviceHeap) Field::MoveModel::FieldMoveModelManager();
  m_pFieldMoveCodeManager     = GFL_NEW(deviceHeap) Field::MoveModel::FieldMoveCodeManager();

  // 動作モデルマネージャーのリソース部以外を初期化
  // *補足* 動作モデルマネージャーはフィールドプロック内でメモリを動的に確保する箇所があるので、フィールドプロック用ヒープを割り当てて別途初期化する必要があります
  m_pFieldRootNode->Initialize( m_pDeviceHeapAllocator );
  m_pFieldCharaModelManager->InitializeManagerWork(m_pFieldRootNode->GetInstanceRootNode(),deviceHeap,m_pDeviceHeapAllocator);
  m_pFieldMoveCodeManager->Initialize(deviceHeap);

  // ゾーン
  mZoneData = GFL_NEW(deviceHeap) Field::ZoneDataLoader();
  mZoneData->Initialize(deviceHeap);
  mZoneData->SyncLoad();    // ゾーンの情報は一括で読み込んでおく。

  // ワールド
  mWorld = GFL_NEW(deviceHeap) Field::World( HEAPID_RESIDENT_DEVICE );

  // エリア
  mArea = GFL_NEW(deviceHeap) Field::Area( deviceHeap );

  // エンカウント制御ワーク
  m_pEncountWork = GFL_NEW(deviceHeap) Field::Encount::EncountWork();
  // フィールドサウンドコントローラーワーク
  m_pFieldSound = GFL_NEW(deviceHeap) Field::FieldSound( this );
  // ワープデータコントローラーワーク
  m_pWarpDataTools = GFL_NEW(deviceHeap) Field::WarpDataTools();


  // 配置アイテム
  {
    m_pFieldItemManager = GFL_NEW(deviceHeap) Field::FieldItemManager();
    m_pFieldItemManager->Initialize();
  }

  // きのみ
  m_pFieldBerryManager = GFL_NEW(deviceHeap) Field::FieldBerryManager();

  // 育て屋
  m_pSodateyaWalkCheckData = GFL_NEW(deviceHeap) Field::FieldSodateyaWalkCheckData; 

  //セーブ書き込み状態を初期化
  mWriteState = WRITE_STATE_NULL;

  // Finder
  for( u32 i = 0; i < Savedata::PokeFinderSave::MAX_POKE_FINDER_SPOT; ++ i )
  {
    m_pFinderSpotReserveData[i] = GFL_NEW(deviceHeap) Field::TrialModel::FinderSpotReserveData();
  }

  m_pFieldCameraData = GFL_NEW(deviceHeap)Field::Camera::SaveData();
  ClearFieldCameraData();

  // 視線トレーナーペアデータ
  m_pTrainerPairData = GFL_NEW(deviceHeap) trainer::TrainerPairData( this );

  // スクリプト常駐データ
  m_pFieldScriptData = GFL_NEW(deviceHeap) Field::FieldScript::GameData();

  // バトルフェスマネージャー
  m_pBattleFesManager = GFL_NEW(deviceHeap) BattleFes::BattleFesManager(deviceHeap);

  //ショート版デモ再生フラグ
  m_FlyDemoShowed = false;
  m_ResortDemoShowed = false;

  m_SkyboxDarkFlag = false;//@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　Mcatコメント内有るaの案件対処

#if PM_DEBUG
  // フィールドデバッグ変数に初期値を設定　C++にはstaticコンストラクタが無いので
  Field::Debug::DebugTypes::Initialize();
  //デバッグ天候リセット
  m_DebugWeather = Field::weather::FORCE_WEATHER_NONE;
#endif
}


//--------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------
GameData::~GameData()
{
#if PM_DEBUG
  // 開発用設定ファイル
  m_pDebugStartSettingAccessor->DeleteData();
  GFL_SAFE_DELETE( m_pDebugStartSettingAccessor );
#endif

  // バトルフェスマネージャー
  GFL_SAFE_DELETE(m_pBattleFesManager);

  // スクリプト常駐データ
  GFL_SAFE_DELETE(m_pFieldScriptData);

  //シセントレーナーペアデータ
  GFL_SAFE_DELETE(m_pTrainerPairData);

  //フィールドカメラ
  GFL_SAFE_DELETE(m_pFieldCameraData);

  // 育て屋
  GFL_SAFE_DELETE(m_pSodateyaWalkCheckData);

  // きのみ
  GFL_SAFE_DELETE(m_pFieldBerryManager);

  // エンカウント制御ワーク
  GFL_DELETE(m_pEncountWork);
  // フィールドサウンドコントローラーワーク
  GFL_DELETE(m_pFieldSound);
  // ワープデータコントローラーワーク
  GFL_DELETE(m_pWarpDataTools);

  // Zone Area Worldの破棄
  mZoneData->Terminate();
  GFL_DELETE(mZoneData);
  GFL_DELETE(mArea);
  GFL_DELETE(mWorld);

  // ゲーム中に保持し続けるトレーナーパス画面のデータ
  GFL_DELETE(mTrainerPassPlayData);
  // ゲーム中に保持し続けるフィールド下画面のデータ
  GFL_DELETE(mFieldMenuPlayData);
  // ゲーム中に保持し続けるバッグ画面のデータ
  GFL_DELETE(mBagPlayData);

  GFL_DELETE(mPlayTime);
  GFL_DELETE(mPlayerParty);
  GFL_SAFE_DELETE(mQRParty);
  GFL_SAFE_DELETE_ARRAY(mQRPartyName);
  GFL_SAFE_DELETE_ARRAY(mSignatureBuffer);

  GFL_DELETE(mSaveControl);
  GFL_DELETE(mSaveData);

  // 動作モデル用
  GFL_SAFE_DELETE(m_pFieldCharaModelManager);
  GFL_SAFE_DELETE(m_pFieldMoveCodeManager);
  GFL_SAFE_DELETE(m_pFieldRootNode);

  // アロケータ削除
  GFL_SAFE_DELETE(m_pDeviceHeapAllocator);
}



//============================================================================================
//
//
//      初期化処理
//
//
//============================================================================================
//----------------------------------------------------------------------------
  /**
   * @brief 「さいしょから」ゲーム開始用データ設定
   */
//----------------------------------------------------------------------------
void GameData::SetFirstData( gfl2::heap::HeapBase * pTempDevHeap )
{
  GFL_PRINT("GameData::SetFirstData\n");

  //pml::pokepara::PokemonParam* pp;
  Savedata::MyStatus * myStatus = this->GetPlayerStatus();
  //Savedata::ZukanData * zukan = this->GetZukanData();
  u32 my_id = System::GflUse::GetPublicRand();

  // ランダムでIDを設定
  myStatus->SetID( my_id );

  // 手持ちポケモン（９月ROM版に更新 2015/09/14 ichiraku）
  {
    mPlayerParty->Clear();  //パーティを初期化する
    mQRParty->Clear();
  }
  //初期戻り先
  SetWarpID(WP_F0102R0101);

  // AlolaTimeを管理するワークを通常に設定
  GameSys::AlolaTime::ClearAlolaOtherWorldOffset(this);
 
  // アローラ時間のセットアップ
  {
    s64 alola_offset = AlolaTime::GetAlolaOffset( GET_VERSION(), GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_GAMESTART );
    this->GetGameTime()->SetAlolaOffset( alola_offset );

    // @fix NMCat[710] ゲーム開始時に時間を固定する。
    GameSys::AlolaTime  cAlolaTime;
    cAlolaTime.Initialize();
    cAlolaTime.FixAlolaOnlyFakeTime( cAlolaTime.GetHour(), 0 );
  }


  // JoinFesta情報を反映
  Savedata::JoinFestaGuest::ApplyJoinFestaGuestSaveData();
  Savedata::JoinFestaVip::ApplyJoinFestaVipSaveData();

  // カメラ
  ClearFieldCameraData();
  mSaveData->GetMisc()->SetFieldCameraData( m_pFieldCameraData->areaId, m_pFieldCameraData->areaDataNo, m_pFieldCameraData->oldAreaDataNo );

  // バトル施設
  this->setFirstDataBattleInst();

  // @fix ディレクターさん要望　バトルのショートカットボタンで最後に使ったボールアイテムidを初期化
  {
    m_battleData.mLastShortCutUseBallItemId = LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE;
  }
}

#if PM_DEBUG
//----------------------------------------------------------------------------
  /**
   * @brief デバッグ開始用データ設定
   */
//----------------------------------------------------------------------------
void GameData::SetDebugData( DebugDataType type, gfl2::heap::HeapBase * pTempDevHeap, int sex )
{
  GFL_PRINT("GameData::SetDebugData\n");

  gfl2::heap::HeapBase * heap = pTempDevHeap;
  //デバッグ開始時は一旦セーブデータを初期化する
  this->ClearSaveData( heap );

//#if 0
  pml::pokepara::PokemonParam* pp;
  Savedata::MyStatus * myStatus = this->GetPlayerStatus();
  Savedata::ZukanData * zukan = this->GetZukanData();
  u32 my_id = System::GflUse::GetPublicRand();

  //@caution  ポケモンを生成する前にMyStatusの初期設定を行う必要がある
  switch(type)
  {
  case DEBUG_DATA_TYPE_NETWORK_RAND:
  case DEBUG_DATA_TYPE_TOP_PFM_DRESS:
    {
      GameSys::DeviceDate date;

      gfl2::math::SFMTRandom * pSfmtRand = GFL_NEW( heap ) gfl2::math::SFMTRandom();
      u32 seed = ((u32)(myStatus->GetTransferdID()) ^ (u32)(myStatus->GetTransferdID() >> 32)) + ((s32)(date.GetDate()->GetDateTimeToDay()) % 365);
      pSfmtRand->Initialize(seed);
      GFL_PRINT( "transferdID [0x%lx] ", myStatus->GetTransferdID() );
      GFL_PRINT( "day[%d] seed [%d]\n", ((s32)(date.GetDate()->GetDateTimeToDay()) % 365), seed );

      u32 sex_rand = pSfmtRand->Next64();
      poke_3d::model::DressUpModelResourceManager* pDressUpResourceManager = GFL_NEW( heap ) poke_3d::model::DressUpModelResourceManager();
      pDressUpResourceManager->Initialize( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), heap, System::DressUp::GetDressUpArcIdListField() );

      myStatus->SetDebugName(true);
      { // ニックネーム
        Savedata::FieldMenu * menu = this->GetFieldMenu();
        gfl2::str::StrBuf * tmp_name = GFL_NEW_LOW(heap) gfl2::str::StrBuf( System::STRLEN_PLAYER_NAME+System::EOM_LEN, heap );
        myStatus->GetNameString( tmp_name );
        menu->SetNickNameString( tmp_name );
        GFL_DELETE tmp_name;
      }

      TOMOYA_PRINT( "sex_rand [%d]\n", sex_rand );

      // デフォルトの服装設定
      poke_3d::model::DressUpParam dressUpParam;

      if (type == DEBUG_DATA_TYPE_NETWORK_RAND)
      {
        // 性別
        if( (sex_rand % 2) == 0 )
        {
          sex = PM_MALE;
          myStatus->SetSex( PM_MALE );
        }
        else
        {
          sex = PM_FEMALE;
          myStatus->SetSex( PM_FEMALE );
        }

        // 各性別のデフォルト着せ替えパラメータの設定
        if (sex == PM_MALE)
        {
          System::DressUp::Debug_GetDressUpParamRandom(sex, pDressUpResourceManager->GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::SEX_MALE), &dressUpParam, pSfmtRand);
        }
        else if (sex == PM_FEMALE)
        {
          System::DressUp::Debug_GetDressUpParamRandom(sex, pDressUpResourceManager->GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::SEX_FEMALE), &dressUpParam, pSfmtRand);
        }
        else
        {
          GFL_ASSERT_STOP(0);
        }
      }
      else
      {
        sex = PM_FEMALE;
        myStatus->SetSex(PM_FEMALE);
        System::DressUp::Debug_GetDressUpParamTopPfm(&dressUpParam);
      }

      myStatus->SetDressup( dressUpParam );


      //  きせかえデフォルト設定 @fix GFNMCat[2108]：着せ替えを設定したあとに移動
      GetFashion()->InitializeDefaultFashionItem( sex == PM_FEMALE );
      // 着せ替えデフォルト設定
      GetFinderStudioSave()->InitDressupParam( dressUpParam );

      // ランダムでIDを設定
      myStatus->SetID( my_id );
      // 全力リング追加
      myStatus->SetZenryokuRing(true);

      pDressUpResourceManager->Finalize();
      GFL_DELETE pDressUpResourceManager; pDressUpResourceManager = NULL;
      GFL_DELETE pSfmtRand; pSfmtRand = NULL;
    }
    break;

  case DEBUG_DATA_TYPE_CTRNAME:
  case DEBUG_DATA_TYPE_DEFAULTNAME:
  default:
    {
      myStatus->SetDebugName(true);
      { // ニックネーム
        Savedata::FieldMenu * menu = this->GetFieldMenu();
        gfl2::str::StrBuf * tmp_name = GFL_NEW_LOW(heap) gfl2::str::StrBuf( System::STRLEN_PLAYER_NAME+System::EOM_LEN, heap );
        myStatus->GetNameString( tmp_name );
        menu->SetNickNameString( tmp_name );
        GFL_DELETE tmp_name;
      }
      // 性別
      myStatus->SetSex(sex);

      // デフォルトの服装設定
      poke_3d::model::DressUpParam dressUpParam;
      // 各性別のデフォルト着せ替えパラメータの設定
      if( sex == PM_MALE )
      {
        System::DressUp::GetDressUpParamDefaultHero( &dressUpParam );
      }
      else if( sex == PM_FEMALE )
      {
        System::DressUp::GetDressUpParamDefaultHeroine( &dressUpParam );
      }
      myStatus->SetDressup( dressUpParam );


      //  きせかえデフォルト設定 @fix GFNMCat[2108]：着せ替えを設定したあとに移動
      GetFashion()->InitializeDefaultFashionItem( sex == PM_FEMALE );
      // 着せ替えデフォルト設定
      GetFinderStudioSave()->InitDressupParam( dressUpParam );

      // ランダムでIDを設定
      myStatus->SetID( my_id );
      // 全力リング追加
      myStatus->SetZenryokuRing(true);
    }
    break;

  }


  // JF定型文初期値
  GetJoinFestaData()->InitializeDefaultPattern( sex );

  //初期戻り先
  SetWarpID(WP_F0102PC0201);

  // アローラ時間のセットアップ
  {
    s64 alola_offset = AlolaTime::GetAlolaOffset( GET_VERSION(), GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_NORMAL  ); // @fix ゲーム開始時間固定にあわせデバッグ開始は通常アローラタイムに変更
    this->GetGameTime()->SetAlolaOffset( alola_offset );
    // フェイク時間をクリア
    AlolaTime alolaTime;
    alolaTime.Initialize();
    alolaTime.ClearAlolaOnlyFakeTime();
    // AlolaTimeを管理するワークを通常に設定
    GameSys::AlolaTime::ClearAlolaOtherWorldOffset(this);
  }

  // JoinFesta情報を反映
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->DEBUG_CheckMaxHeap();
  Savedata::JoinFestaGuest::ApplyJoinFestaGuestSaveData();
  Savedata::JoinFestaVip::ApplyJoinFestaVipSaveData();


  // 手持ちポケモン（momiji版に更新 2017/2/7 ichiraku）
  {
    mPlayerParty->Clear();  //パーティを初期化する

    // ミミッキュ Lv99
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_HORAA, 99, pml::pokepara::INIT_SPEC_ID_RANDOM );
    PokeTool::PokeMemo::SetFromCapture( pp, myStatus, 6, heap->GetLowerHandle() );
    pp->SetWaza( 0, WAZANO_UDDOHANMAA );      // ウッドハンマー
    pp->SetWaza( 1, WAZANO_KAGEUTI );         // かげうち
    pp->SetWaza( 2, WAZANO_ZYARETUKU );       // じゃれつく
    pp->SetWaza( 3, WAZANO_MAMORU );          // まもる
    pp->SetItem( ITEM_MIMIKKYUz_1 );            // ミミッキュZ
    pp->SetLangId(System::GetLang());
    pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
    pp->SetDebugEditFlag(true);
    zukan->SetPokeSee(*pp);
    zukan->SetPokeGet(*pp);
    mPlayerParty->AddMember( *pp );
    GFL_DELETE pp;

    // ガラガラヌシ Lv99
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_GARAGARA, 99, pml::pokepara::INIT_SPEC_ID_RANDOM );
    PokeTool::PokeMemo::SetFromCapture( pp, myStatus, 6, heap->GetLowerHandle() );
    pp->ChangeFormNo( FORMNO_GARAGARA_NUSHI );
    pp->SetWaza( 0, WAZANO_SYADOOBOON );    // シャドーボーン
    pp->SetWaza( 1, WAZANO_HUREADORAIBU );  // フレアドライブ
    pp->SetWaza( 2, WAZANO_HONEBUUMERAN );  // ホネブーメラン
    pp->SetWaza( 3, WAZANO_ONIBI );         // おにび
    pp->SetItem( ITEM_HUTOIHONE );          // ふといホネ
    pp->SetLangId(System::GetLang());
    pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
    pp->SetDebugEditFlag(true);
    zukan->SetPokeSee(*pp);
    zukan->SetPokeGet(*pp);
    mPlayerParty->AddMember( *pp );
    GFL_DELETE pp;

    // ネクロズマ Lv99
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_PURIZUMU, 99, pml::pokepara::INIT_SPEC_ID_RANDOM );
    PokeTool::PokeMemo::SetFromCapture( pp, myStatus, 6, heap->GetLowerHandle() );
    pp->SetWaza( 0, WAZANO_PURIZUMUREEZAA );  // プリズムレーザー
    pp->SetWaza( 1, WAZANO_AKUNOHADOU );      // あくのはどう
    pp->SetWaza( 2, WAZANO_MEISOU );          // めいそう
    pp->SetWaza( 3, WAZANO_HIKARIDORAGON );   // ひかりドラゴン
    pp->SetItem( ITEM_NEKUROZUMAz_1 );    // ネクロZ
    pp->SetLangId(System::GetLang());
    pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
    pp->SetDebugEditFlag(true);
    zukan->SetPokeSee(*pp);
    zukan->SetPokeGet(*pp);
    mPlayerParty->AddMember( *pp );
    GFL_DELETE pp;

    // ソルガレオ Lv99
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_NIKKOU, 99, pml::pokepara::INIT_SPEC_ID_RANDOM );
    PokeTool::PokeMemo::SetFromCapture( pp, myStatus, 6, heap->GetLowerHandle() );
    pp->SetWaza( 0, WAZANO_AIANHEDDO );     // アイアンヘッド
    pp->SetWaza( 1, WAZANO_METEODORAIBU );  // メテオドライブ
    pp->SetWaza( 2, WAZANO_IWANADARE );     // いわなだれ
    pp->SetWaza( 3, WAZANO_KAENHOUSYA );    // かえんほうしゃ
    pp->SetItem( ITEM_SORUGAREOz_1 );       // ソルガレオZ
    pp->SetLangId(System::GetLang());
    pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
    pp->SetDebugEditFlag(true);
    zukan->SetPokeSee(*pp);
    zukan->SetPokeGet(*pp);
    mPlayerParty->AddMember( *pp );
    GFL_DELETE pp;

    // ルナアーラ Lv99
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_GEKKOU, 99, pml::pokepara::INIT_SPEC_ID_RANDOM );
    PokeTool::PokeMemo::SetFromCapture( pp, myStatus, 6, heap->GetLowerHandle() );
    pp->SetWaza( 0, WAZANO_MAZIKARUSYAIN );   // マジカルシャイン
    pp->SetWaza( 1, WAZANO_SYADOOREI );       // シャドーレイ
    pp->SetWaza( 2, WAZANO_REITOUBIIMU );     // れいとうビーム
    pp->SetWaza( 3, WAZANO_10MANBORUTO );     // １０まんボルト
    pp->SetItem( ITEM_RUNAAARAz_1 );          // ルナアーラZ
    pp->SetLangId(System::GetLang());
    pp->SetGetBall( item::BALLID_MONSUTAABOORU ); // 捕獲ボール
    pp->SetDebugEditFlag(true);
    zukan->SetPokeSee(*pp);
    zukan->SetPokeGet(*pp);
    mPlayerParty->AddMember( *pp );
    GFL_DELETE pp;
  }

  // アイテム追加
  Savedata::MyItem * myitem = mSaveData->GetMyItem();

  myitem->Add( ITEM_MASUTAABOORU, 10, true );
  myitem->Add( ITEM_IIKIZUGUSURI, 10, true );
  myitem->Add( ITEM_HUSIGINAAME, 999, true );
//#endif

  // フィールドメニュー
  {
    Savedata::FieldMenu * fieldmenu = this->GetFieldMenu();
    // 追加
    for( int i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
    {
      fieldmenu->SetMenuPageEnable( i );
      for( int j=0; j<Savedata::FieldMenu::PAGE_ICON_MAX; j++ )
      {
        Savedata::FieldMenu::IconID id = static_cast<Savedata::FieldMenu::IconID>( i*Savedata::FieldMenu::PAGE_ICON_MAX+j );
        if( fieldmenu->IsMenuIconEnable(id) == false )
        {
          fieldmenu->SetMenuIconID( i, j, id );
        }
      }
    }
    // 初期データの位置を変更
    fieldmenu->DebugInitMenuSort();

    // 各ボタン表示のためイベントフラグをセット
    Field::EventWork * evwk = GetEventWork();
    evwk->SetEventFlag( SYS_FLAG_BOUKEN_NAVI_GET );
    evwk->SetEventFlag( SYS_FLAG_POKE_FINDER_GET );
    evwk->SetEventFlag( SYS_FLAG_POKE_RIDE_OPEN );

    // 図鑑のイベントフラグをセット
    evwk->SetEventFlag(SYS_FLAG_ZUKAN_GET);  // 図鑑ゲット
    evwk->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND1_OPEN);  // 島１オープン

    // ライドポケモン
    fieldmenu->SetRideShortcut( Savedata::FieldMenu::KEY_ID_UP, Savedata::FieldMenu::RIDE_ID_RUSH );
    evwk->SetEventFlag( SYS_FLAG_KENTAROSU_RIDE_OPEN );
    evwk->SetEventFlag( SYS_FLAG_MUURANDO_RIDE_OPEN );
    evwk->SetEventFlag( SYS_FLAG_KAIRIKII_RIDE_OPEN );
    evwk->SetEventFlag( SYS_FLAG_ROBA2_RIDE_OPEN );
    evwk->SetEventFlag( SYS_FLAG_RAPURASU_RIDE_OPEN );
    evwk->SetEventFlag( SYS_FLAG_SAMEHADAA_RIDE_OPEN );
    evwk->SetEventFlag( SYS_FLAG_RIZAADON_RIDE_OPEN );
  }
  // 到着フラグ系をセット
  {
    Field::EventWork * evwk = GetEventWork();

    evwk->SetEventFlag( SYS_FLAG_FLY_F0102S0201 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102C0102 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102S0401 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102T0101 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102R010102 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102D0301 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102D0501 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102H0101 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102D0201 );
    evwk->SetEventFlag( SYS_FLAG_FLY_F0102D0401 );
  }
  // エンカウントフラグをセット
  {
    Field::EventWork * evwk = GetEventWork();
    evwk->SetEventFlag( SYS_FLAG_FIELD_ENCOUNT_OPEN );
  }
  //  デバッグ用にフィールド設定をしておく
  {
    gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
    gfl2::math::Quaternion  que;
    mStartLoc.Set( 0, 0, pos, que );
  }
  // ジョインフェスタのチュートリアル終わらせる
  {
    Field::EventWork * evwk = GetEventWork();
    u16* p_work = evwk->GetEventWorkAdrs( WK_SYS_JF_TUTORIAL );
    *p_work = 5;
    evwk->SetEventFlag( SYS_FLAG_JF_ROOM_TUTORIAL );
  }

  // カメラ
  ClearFieldCameraData();
  mSaveData->GetMisc()->SetFieldCameraData( m_pFieldCameraData->areaId, m_pFieldCameraData->areaDataNo, m_pFieldCameraData->oldAreaDataNo );

  // バトル施設
  this->setFirstDataBattleInst();

  // @fix ディレクターさん要望　バトルのショートカットボタンで最後に使ったボールアイテムidを初期化
  {
    m_battleData.mLastShortCutUseBallItemId = LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE;
  }
}
//----------------------------------------------------------------------------
  /**
   * @brief デバッグバトル開始用データ設定
   * @param dontClearZukan  true なら、図鑑データを維持する
   */
//----------------------------------------------------------------------------
void GameData::SetDebugBattleData( gfl2::heap::HeapBase * pTempHeap, bool dontClearZukan )
{
  GFL_PRINT("GameData::SetDebugBattleData\n");

  gfl2::heap::HeapBase * heap = pTempHeap->GetLowerHandle();

  // 図鑑データを保存しておく
  void* zukanData = NULL;
  if( dontClearZukan )
  {
    Savedata::ZukanData* zukan = this->GetZukanData();
    const void* data = zukan->GetData();
    const u32 datasize = zukan->GetDataSize();
    zukanData = GflHeapAllocMemory( heap, datasize );
    gfl2::std::MemCopy( data, zukanData, datasize );
  }

  //デバッグ開始時は一旦セーブデータを初期化する
  this->ClearSaveData( heap );

  // 保存しておいた図鑑データを復元
  if( dontClearZukan )
  {
    Savedata::ZukanData* zukan = this->GetZukanData();
    zukan->SetData( zukanData );
    GflHeapFreeMemory( zukanData );
  }

  // バトルデバッグを便利にするため、戦闘中に使用可能な全アイテムを100個ずつ持たせる
  Savedata::MyItem * myitem = mSaveData->GetMyItem();
  for( u32 itemId=0; itemId<=ITEM_DATA_MAX; ++itemId )
  {
    item::ITEM_DATA* itemData = GFL_NEW( pTempHeap ) item::ITEM_DATA( itemId, pTempHeap );
    if( itemData->GetParam( item::ITEM_DATA::PRM_ID_B_POCKET ) != 0 )
    {
      myitem->Add( itemId, 100, true );
    }
    GFL_SAFE_DELETE( itemData );
  }

  // ジガルデの「スワームチェンジ」のため「ジガルデキューブ」を持っていることにする
  myitem->Add( ITEM_ZIGARUDEKYUUBU, 1 );

  // アローラ時間のセットアップ
  {
    s64 alola_offset = GameSys::AlolaTime::GetAlolaOffset( GET_VERSION(), GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_NORMAL );
    this->GetGameTime()->SetAlolaOffset( alola_offset );
    // フェイク時間をクリア
    AlolaTime alolaTime;
    alolaTime.Initialize();
    alolaTime.ClearAlolaOnlyFakeTime();
    GameSys::AlolaTime::ClearAlolaOtherWorldOffset( this );
  }
}

//----------------------------------------------------------------------------
/**
 * @brief デバッグ用天候セット
 * @param weather_kind  天候種類
 */
//----------------------------------------------------------------------------
void GameData::SetDebugWeather( Field::weather::WeatherKind weather_kind)
{
  m_DebugWeather = weather_kind;
}

//----------------------------------------------------------------------------
/**
 * @brief デバッグ用天候セット
 * @return 天候種類
 */
//----------------------------------------------------------------------------
Field::weather::WeatherKind GameData::GetDebugWeather(void)
{
  return m_DebugWeather;
}

#endif //PM_DEBUG


//--------------------------------------------------------------
/**
 * @brief FileエラーをSystemエラーに変換
 * @note  Unmount系はゲーム開始ではじくので失敗はすべて致命的としている
 */
//--------------------------------------------------------------
static gfl2::fs::FsCtrBackupSystem::Result fileErr2sysErr(
    gfl2::fs::FsCtrBackupFile::Result file_result )
{
  if (file_result == gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK )
  {
    return gfl2::fs::FsCtrBackupSystem::RESULT_OK;
  }
  return gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR;
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータの初期化開始
 * @retval  gfl::fs::BackupSystem::RESULT_OK       初期化開始
 */
//--------------------------------------------------------------
void GameData::FormatStart( gfl2::heap::HeapBase * pTempHeap )
{
  if( mSaveControl )
  {
    mSaveControl->FormatStart(pTempHeap);
    mWriteState = WRITE_STATE_FORMAT;
  }

}

//--------------------------------------------------------------
/**
 * @brief   セーブデータの初期化メイン
 * @retval  gfl::fs::BackupSystem::RESULT_OK       初期化処理完了
 * @retval  gfl::fs::BackupSystem::RESULT_CONTINUE 初期化処理継続中
 * @retval  gfl::fs::BackupSystem::RESULT_NG_xxxx  失敗
 */
//--------------------------------------------------------------
bool GameData::FormatMain( gfl2::fs::FsCtrBackupSystem::Result * result )
{
  if( mSaveControl )
  {
    if( mSaveControl->FormatMain( result ) ){
      mWriteState = WRITE_STATE_NULL;
      return true;
    }
    else{
      return false;
    }
  }

  return true;
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータワークからゲームデータが持つ情報をロードする
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupSystem::Result GameData::LoadStart( gfl2::heap::HeapBase * pTempHeap )
{
  if( mSaveControl == NULL )
    return gfl2::fs::FsCtrBackupSystem::RESULT_OK;

  if ( mSaveControl->LoadStart( pTempHeap, mSaveData ) )
  {
    return gfl2::fs::FsCtrBackupSystem::RESULT_OK;
  }
  return gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR;
}


//--------------------------------------------------------------
/**
 * @brief   ゲームデータが持つ情報を元にロードを実行
 * @retval  ロード結果  RESULT_OKで終了
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupSystem::Result GameData::LoadMain(void)
{
  if( mSaveControl == NULL )
    return gfl2::fs::FsCtrBackupSystem::RESULT_OK;

  gfl2::fs::FsCtrBackupFile::Result result;
  if ( !mSaveControl->LoadMain( &result ) )
  {
    return gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;
  }
  if (result == gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK)
  {
    if( mSaveData->GetStatus() != Savedata::ISaveDataTotal::STATE_OK)
    {
      GFL_PRINT(" Invalid Save Data \n");
      return gfl2::fs::FsCtrBackupSystem::RESULT_NG_BROKEN_DATA;
    }
    DeserializeSaveData();
    mIsLoaded = true;
    return gfl2::fs::FsCtrBackupSystem::RESULT_OK;
  }
  return gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR;
}

Savedata::ISaveDataTotal::State GameData::LoadAtOnce( gfl2::heap::HeapBase * pTempHeap )
{
  gfl2::fs::FsCtrBackupFile::Result result;
  result = mSaveControl->LoadAtOnce( pTempHeap, mSaveData );
  if (result == gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK)
  {
    Savedata::ISaveDataTotal::State st = mSaveData->GetStatus();
    if ( st ==  Savedata::ISaveDataTotal::STATE_OK )
    {
      DeserializeSaveData();
      mIsLoaded = true;
    }
    return st;
  }
  return  Savedata::ISaveDataTotal::STATE_NG;
}

//--------------------------------------------------------------
/**
 * @brief   ゲームデータが持つ情報を元にセーブを実行
 * @param   gamedata    ゲームデータへのポインタ
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
void GameData::SaveStart( gfl2::heap::HeapBase * pTempHeap )
{
  if( mSaveControl )
  {
    mTempHeap = pTempHeap;
    this->SerializeSaveData();
    this->RenewSaveDataSecureValue();   ///巻き戻し防止情報を更新
    mSaveControl->SaveStart( mTempHeap, mSaveData );
    mWriteState = WRITE_STATE_NORMAL;
  }
}


//--------------------------------------------------------------
/**
 * @brief   ゲームデータが持つ情報を元にセーブを実行
 * @retval  セーブ結果  RESULT_OKで終了
 */
//--------------------------------------------------------------
gfl2::fs::FsCtrBackupSystem::Result GameData::SaveMain(void)
{
  if( mSaveControl == NULL )
    return gfl2::fs::FsCtrBackupSystem::RESULT_OK;

  gfl2::fs::FsCtrBackupFile::Result result;
  if ( !mSaveControl->SaveMain( &result ) )
  {
    return gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;
  }
  if (result == gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK)
  {
    mWriteState = WRITE_STATE_NULL;
    this->Update3DSSecureValue();       //巻き戻し防止を本体に反映
    return gfl2::fs::FsCtrBackupSystem::RESULT_OK;
  }
  else
  {
    mWriteState = WRITE_STATE_NULL;
    this->CancelSaveDataSecureValue();  //巻き戻し防止を戻す
    return gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR;
  }
}

  //--------------------------------------------------------------
/**
  * @brief   セーブを途中まで実行：開始
  * @param[in]   pTempHeap         作業用ヒープ
  *
  * @note  LastSave～と組みにして使用します。
  * @attention   セーブ失敗は致命的なので内部でFATALにします
  */
//--------------------------------------------------------------
void GameData::FirstSaveStart( gfl2::heap::HeapBase * pTempHeap )
{
  mTempHeap = pTempHeap;
  this->SerializeSaveData();
  this->RenewSaveDataSecureValue();   ///巻き戻し防止情報を更新
  mSaveControl->FirstSaveStart( mTempHeap, mSaveData );
  mWriteState = WRITE_STATE_FIRST;
}

//--------------------------------------------------------------
/**
  * @brief   セーブを途中まで実行：メイン
  * @param[out]  result  結果を受け取るポインタ
  * @return  bool  trueのとき、完了  falseのとき実行中
  *
  * -  gfl::fs::BackupSystem::RESULT_OK       セーブ完了
  * -  gfl::fs::BackupSystem::RESULT_NG_xxxx  セーブ失敗
  */
//--------------------------------------------------------------
bool GameData::FirstSaveMain( gfl2::fs::FsCtrBackupSystem::Result * result )
{
  gfl2::fs::FsCtrBackupFile::Result file_result;
  if ( !mSaveControl->FirstSaveMain( &file_result ) )
  {
    *result = gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;
    return false;
  }
  if (file_result == gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK)
  {
    mWriteState = WRITE_STATE_FIRST_END;
    *result = gfl2::fs::FsCtrBackupSystem::RESULT_OK;
    return true;
  }
  else
  {
    mWriteState = WRITE_STATE_NULL;     //失敗→セーブ終了
    this->CancelSaveDataSecureValue();  //巻き戻し防止を戻す
    *result = gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR;
    return true;
  }
}

//--------------------------------------------------------------
/**
  * @brief   セーブを途中まで実行：解除
  */
//--------------------------------------------------------------
void GameData::FirstSaveCancel( void )
{
  if ( mWriteState == WRITE_STATE_FIRST_END )
  {
    mSaveControl->FirstSaveCancel();    //セーブ中書き込み禁止処理等の解除
    this->CancelSaveDataSecureValue();  //巻き戻し防止を戻す
  }
  else
  {
    GFL_ASSERT( 0 );  // FirstSave中でない場合、Assert
  }
}

//--------------------------------------------------------------
/**
  * @brief   セーブを途中から完了処理
  * @note    エラーはすべて製品版では発生してはならないもの
  */
//--------------------------------------------------------------
void GameData::LastSave( void )
{
  //LastSaveに失敗した場合は内部で致命的エラーにする
  mSaveControl->LastSave( mSaveData );
  this->Update3DSSecureValue();       //巻き戻し防止を本体に反映
}

//--------------------------------------------------------------
/**
  * @brief   セーブ途中から完了処理：開始
  * @param[in]   pTempHeap         作業用ヒープ
  *
  * @note  FirstSave～と組みにして使用します。
  * @note  FirstSaveMain完了後、通信同期をとって呼び出すことで
  * @note  人為的なデータの増殖などを予防するための機能です。
  */
//--------------------------------------------------------------
bool GameData::LastSaveStart( gfl2::heap::HeapBase * pTempHeap )
{
  if (mWriteState != WRITE_STATE_FIRST_END)
  { // FirstSaveを終えていない状態はNG
    GFL_ASSERT(0);
    return false;
  }
  mWriteState = WRITE_STATE_LAST;
  return mSaveControl->LastSaveStart( pTempHeap, mSaveData );
}

//--------------------------------------------------------------
/**
  * @brief   セーブ途中から完了処理：メイン
  * @param[out]  result  結果を受け取るポインタ
  * @return      bool  trueのとき、完了  falseのとき実行中
  *
  * -  gfl::fs::BackupSystem::RESULT_OK       セーブ完了
  * -  gfl::fs::BackupSystem::RESULT_NG_xxxx  セーブ失敗
  */
//--------------------------------------------------------------
bool GameData::LastSaveMain( gfl2::fs::FsCtrBackupSystem::Result * result )
{
  gfl2::fs::FsCtrBackupFile::Result file_result;
  bool flag = mSaveControl->LastSaveMain( &file_result );
  if ( flag )
  {
    mWriteState = WRITE_STATE_NULL;
    *result = fileErr2sysErr( file_result );
    if (*result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
    {
      this->Update3DSSecureValue();       //巻き戻し防止を本体に反映
    }
    else
    {
      this->CancelSaveDataSecureValue();  //巻き戻し防止を戻す
    }
  }
  return flag;
}



//-----------------------------------------------------------------------------
/**
* @brief セーブデータのクリア
*/
//-----------------------------------------------------------------------------
void GameData::ClearSaveData( gfl2::heap::HeapBase * pTempHeap )
{
  mSaveData->Clear( pTempHeap );

  mPlayTime->Initialize();

  // AlolaTimeを管理するワークを通常に設定
  GameSys::AlolaTime::ClearAlolaOtherWorldOffset( this );


  mIsLoaded = false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  セーブ前にSaveDataへの反映処理
 */
//-----------------------------------------------------------------------------
void GameData::SerializeSaveData(void)
{
  //シードを保存する
  //mSaveData->GetMisc()->SetPokemonCreateRandomSeed( System::GflUse::GetPokemonCreateRand() );

  // 動作モデルデータの反映
  // JFの場合、ダミーのセーブ領域に書き込み、セーブ領域は更新させない。
  m_pFieldCharaModelManager->SerializeSaveData( this->GetFieldMoveModelSave() );  

  // 手持ちポケモンデータの反映
  mSaveData->SerializePokeParty( mPlayerParty );
  // プレイ時間を反映
  mSaveData->GetPlayTime()->SetData( mPlayTime->GetData() );
  // アシストパワーのパワーポイントを反映
  //mSaveData->GetAssistPowerData()->SerializePowerPoint();

  // ロケーション
  // ロケーションは、オリジナルのロケーションデータを書き込むことで問題はない。
  Savedata::Situation* pSituation = mSaveData->GetSituation();
  mStartLoc.Get(pSituation->GetStartLocation());
  mSpecialLoc.Get(pSituation->GetSpecialLocation());
  mEscapeLoc.Get(pSituation->GetEscapeLocation());

  //JF用に2重で保持しているセーブデータの書き戻し
  if( IsNowJoinFestaFieldData() )
  {
    serializeJoinFestaFieldData();
  }

  // Alola時間のFakeTime保存
  {
    u8 alola_hour, alola_minute, alola_second;
    b8 alola_flag = AlolaTime::GetFixAlolaOnlyFakeTimeSaveParam( &alola_hour, &alola_minute, &alola_second );
    mSaveData->GetMisc()->SetAlolaOnlyFakeTime( alola_flag, alola_hour, alola_minute, alola_second );
    TOMOYA_PRINT( "hour %d  minute %d second %d\n", alola_hour, alola_minute, alola_second );
  }

  // @caution 本来はJFにジャンプする前に保存しておき、serializeJoinFestaFieldData内で書き戻すべき内容です。
  // nijiではフェス内でCameraAreaを使用していないため、安全に挙動しています。
  // カメラ
  {
    mSaveData->GetMisc()->SetFieldCameraData( m_pFieldCameraData->areaId, m_pFieldCameraData->areaDataNo, m_pFieldCameraData->oldAreaDataNo );
  }

  /*
  // PlayerWork
  pSituation->SetPlayerWork(&mPlayerWork);
  */
  //mSaveData->GetMisc()->SetCameraSaveData( *mFieldCameraSave );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ロード後にGameDataへの展開処理
 */
//-----------------------------------------------------------------------------
void GameData::DeserializeSaveData(void)
{
  Savedata::MyStatus * myStatus = this->GetPlayerStatus();

  // 基本情報を初期化
  {//@fix [#4134 【フェスサークル】本体設定の地域設定を変更してもプレイヤーリスト各種モードに表示される地域名に反映されないことについて]
    gfl2::heap::HeapBase* heapWork = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT ); // 一時領域として利用
    myStatus->SetupBasicInfo( heapWork );
  }

  // 本体を変更をチェック
  myStatus->CheckChangeCTR();

  // 本体を変更した！
  //if( myStatus->IsChangedCTR() )
  {
    //savedata::pss_favorite::RemoveFriendPersonalFromPssFavoriteSaveData();  // お気に入り情報から友達を削除
    //savedata::pss_friend::ClearPssFriendSaveData();                         // PSS友達情報をリセット
  }
  //mFieldMenuRegident->SetChangedCTR( myStatus->IsChangedCTR() );

  // 着せ替え情報が壊れていたら復元する
  //myStatus->CorrectSaveFashionBuffer();

  // 2013.05.09 着せ替え情報→メガリング保持フラグ反映処理を追加
  // アッパーバージョンの兼ね合いで着せ替え情報を書き換える可能性があるため、
  // メガリングの有無判定を着せ替え情報ではなく独立したメガリング保持フラグを使うように修正。
  // 以前のセーブデータから正しく移行できるように更新処理をここで呼び出す
  //myStatus->UpdateMegaRingStatus();

  // Alola時間のFakeTime復帰
  {
    AlolaTime::SetFixAlolaOnlyFakeTimeSaveParam( mSaveData->GetMisc()->IsAlolaOnlyFakeTime(), mSaveData->GetMisc()->GetAlolaOnlyFakeTimeHour(), mSaveData->GetMisc()->GetAlolaOnlyFakeTimeMinute(), mSaveData->GetMisc()->GetAlolaOnlyFakeTimeSecond() );
  }

  mSaveData->DeserializePokeParty( mPlayerParty );

  // @fix GFNMcat[5307]:バトル外でメガ進化ポケモンがいた場合は不正、ノーマルフォルムに書き戻す
  PokeTool::Mega::NormalizeMegaForm( mPlayerParty );
  // momiji対応：追加でバトルフォルムだった場合は不正としてノーマルフォルムに戻す
  PokeTool::form::NormalizeBattleForm( mPlayerParty );

  mPlayTime->SetData( mSaveData->GetPlayTime()->GetData() );
  //mSaveData->GetAssistPowerData()->DeserializePowerPoint();

  // ペアレンタルによるボイスチャット禁止時
  /*if( nn::cfg::CTR::IsRestrictPhotoExchange() ){
    myStatus->SetRejectVoicechat( true );
  }*/
  // COPPACS制限時
  //if( myStatus->IsCoppacsRestriction() )
  {
    //myStatus->SetDefaultPssIcon( true, false );
    //myStatus->SetRejectVoicechat( true );
  }

  // すれ違い通信禁止時はPSS通信の停止設定を解除
  /*
  if( nn::cfg::CTR::IsRestrictP2pCec() ){
    mSaveData->GetConfig()->SetPssEnable( true );
  }
  */

  // JoinFesta情報を反映
  Savedata::JoinFestaGuest::ApplyJoinFestaGuestSaveData();
  Savedata::JoinFestaVip::ApplyJoinFestaVipSaveData();

  //シードを与えて初期化
  //System::GflUse::PokemonCreateRandomInit( mSaveData->GetMisc()->GetPokemonCreateRandomSeed() );

  // @fix GFBTS[226] : 不思議な贈り物などのセーブでNPCがきえる
  // 動作モデルデータの反映
  m_pFieldCharaModelManager->DeserializeSaveData( mSaveData->GetFieldMoveModelSave() );

  // シチュエーション更新
  Savedata::Situation* pSituation = mSaveData->GetSituation();
  mStartLoc.Set(*(pSituation->GetStartLocation()));
  mSpecialLoc.Set(*(pSituation->GetSpecialLocation()));
  mEscapeLoc.Set(*(pSituation->GetEscapeLocation()));
  /*
  // PlayerWork
  mPlayerWork = *(pSituation->GetPlayerWork());
  *mFieldCameraSave = mSaveData->GetMisc()->GetCameraSaveData();
  */

  /*
  // セーブデータから写真アイコンを適応
  if( mSaveData->GetMyPhotoIconSaveData()->IsSaved() )
  {
    netapp::pss::PssFriendKey myFriendKey;
    myStatus->GetPssFriendKey( &myFriendKey );

    mPlayerPhotoIconManager->AddPhoto( GetMyPhotoIconSaveData()->GetPictIcon(),
                                       xy_system::player_icon::PicIconMaker::PIC_ICON_BUFFER_SIZE,
                                       myFriendKey, true );
  }

  //GameSync未登録アラートフラグが立ってたら、PSSのボタンを光らせる
  if( GetGameSync()->GetIsNoRegistPGL() == 1 )
  {
    GFL_SINGLETON_INSTANCE(gamesystem::GameManager)->GetInterfareGuideManager()->SetPssMainMenuLightupOff( field::subscreen::pss::interfare_guide::InterfareGuideManager::E_LT_GAMESYNC );
  }
  */

  // 下画面設定
  //mFieldMenuRegident->SetFieldSubWindowDefault( mSaveData->GetFieldMenu()->GetSubScreenPage() );
	//mPmnPlayData->InitTopNavi( mSaveData->GetFieldMenu()->GetSubScreenPage() );

  // メッセージスピードをMsgWinクラスに伝える
  print::MessageWindow::SetDefaultMessageSpeed( mSaveData->GetConfig()->GetTalkingSpeed() );

  //ボタン設定を反映させる
  {
    gfl2::ui::DeviceManager * pDeviceManager =  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    GFL_ASSERT( pDeviceManager );
    
    gfl2::ui::Button* pButtonStandard = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
    GFL_ASSERT( pButtonStandard );

    gfl2::ui::Button* pButtonStickEmu = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
    GFL_ASSERT( pButtonStickEmu );

    mSaveData->GetConfig()->SetButtonMode( mSaveData->GetConfig()->GetButtonMode(), pButtonStandard, pButtonStickEmu );
  }

  // カメラ
  {
    mSaveData->GetMisc()->GetFieldCameraData( &m_pFieldCameraData->areaId, &m_pFieldCameraData->areaDataNo, &m_pFieldCameraData->oldAreaDataNo );
  }

  // @fix ディレクターさん要望　バトルのショートカットボタンで最後に使ったボールアイテムidを初期化
  {
    m_battleData.mLastShortCutUseBallItemId = LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE;
  }

  //リゾート用RTC処理
  GetResortSave()->ResortRTCCheckFunc();

}


//-----------------------------------------------------------------------------
/// 巻き戻し防止用保存値を更新
//-----------------------------------------------------------------------------
void GameData::RenewSaveDataSecureValue( void )
{
  mSaveData->RenewSecureValue();
}

//-----------------------------------------------------------------------------
/// 巻き戻し防止用保存値の更新をキャンセル
//-----------------------------------------------------------------------------
void GameData::CancelSaveDataSecureValue( void )
{
  mSaveData->CancelSecureValue();
}

//-----------------------------------------------------------------------------
/// 巻き戻し防止用保存値を3DS本体に反映
//-----------------------------------------------------------------------------
void GameData::Update3DSSecureValue( void )
{
  GFL_PRINT("SecureValue:Set\n");
#if defined(GF_PLATFORM_CTR)
  nn::fs::SetSaveDataSecureValue( mSaveData->GetSecureValue() );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief 巻き戻し防止機能の状態取得
 * @retval  true    問題なし
 * @retval  false   巻き戻りが発生している
 */
//-----------------------------------------------------------------------------
bool GameData::VerifySecureValue( void )
{
  GFL_PRINT("SecureValue:Verify\n");
#if defined(GF_PLATFORM_CTR)
  return nn::fs::VerifySaveDataSecureValueLoosely( mSaveData->GetSecureValue() );
#else
  return true;
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief 巻き戻し防止機能の状態取得（ひとつ前のSecureValueとの比較）
 * @retval  true    問題なし
 * @retval  false   巻き戻りが発生している
 */
//-----------------------------------------------------------------------------
bool GameData::VerifyPreiousvSecureValue( void )
{
#if defined(GF_PLATFORM_CTR)
   return nn::fs::VerifySaveDataSecureValueLoosely( mSaveData->GetPreviousSecureValue() );
#else
   return true;
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief　巻き戻し防止機能：本体保存SecureValueを現在のセーブデータに一致させる
 */
//-----------------------------------------------------------------------------
void GameData::RepairCTRSecureValue( void )
{
  if ( VerifySecureValue() == true )
  { //現在のSecureValueと一致→修復する必要がない
    GFL_ASSERT( 0 );
    return;
  }
  if ( VerifyPreiousvSecureValue() == false )
  { //ひとつ前のSecureValueと一致しない→修復してはいけない
    GFL_ASSERT( 0 );
    return;
  }
  Update3DSSecureValue();
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータを格納するために必要なバッファサイズを取得する(PGL通信用）
 *
 * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
 *
 * @return  セーブデータを格納するために必要なバッファサイズ
 */
//--------------------------------------------------------------
u32 GameData::GerSaveDataBufferSize( bool isEnablePokemonBox )
{
  return mSaveControl->GetSaveDataBufferSize( mSaveData, isEnablePokemonBox );
}

//--------------------------------------------------------------
/**
 * @brief   ゲームデータをバッファに書き込み(PGL通信用）
 * @param   pBuffer            格納先バッファ
 * @param   pHeap              ワーク用ヒープ
 * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
 */
//--------------------------------------------------------------
void GameData::WriteSaveDataToBuffer( u8* pBuffer, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox )
{
  mSaveControl->SaveToBuffer( pBuffer, mSaveData, pHeap, isEnablePokemonBox );
}

#if PM_DEBUG

//--------------------------------------------------------------
/**
 * @brief   ゲームデータをバッファから読み込む(PGLデバッグ用）
 * @param   pBuffer            読み込み先バッファ
 * @param   pHeap              ワーク用ヒープ
 * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
 */
//--------------------------------------------------------------
void GameData::LoadSaveDataFromBuffer( const u8* pBuffer, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox )
{
  mSaveControl->LoadFromBuffer( pBuffer, mSaveData, pHeap, isEnablePokemonBox );
}

#endif
//


//============================================================================================
//
//
//    アクセス関数
//
//
//============================================================================================

//-----------------------------------------------------------------------------
/**
 * @brief　ジョインフェスタモード開始
 */
//-----------------------------------------------------------------------------
void GameData::StartJoinFestaFieldData( void )
{
  // Misc コピー
  m_JoinFestaSwitchFieldData.mMisc.CopyData( *this->GetMisc(), false );
  // Situation  コピー
  m_JoinFestaSwitchFieldData.mSituation.CopyData( *this->GetSituation(), false );

  // Locationのコピー
  m_JoinFestaSwitchFieldData.mStartLoc    = mStartLoc;
  m_JoinFestaSwitchFieldData.mSpecialLoc  = mSpecialLoc;
  m_JoinFestaSwitchFieldData.mEscapeLoc   = mEscapeLoc;

  // MoveModelManagerの状態をMoveModelSaveDataで取っておく
  m_pFieldCharaModelManager->SerializeSaveData( mSaveData->GetFieldMoveModelSave() );


  // 処理順重要　フラグ操作は最後
  // JoinFestaモード開始
  m_JoinFestaSwitchFieldData.mIsJoinFesta = true;
}


//-----------------------------------------------------------------------------
/**
 * @brief　ジョインフェスタモード終了
 */
//-----------------------------------------------------------------------------
void GameData::EndJoinFestaFieldData( void )
{
  // 処理順重要　フラグ操作は最初
  // JoinFestaモード終了
  m_JoinFestaSwitchFieldData.mIsJoinFesta = false;

  // JF用の2重化しているセーブデータ領域で引き継ぐべきデータは引き継ぐ
  serializeJoinFestaFieldData();

  // MoveModelManagerの状態を元のMoveModelSaveDataで復元する。
  m_pFieldCharaModelManager->TerminateMoveModelWorkResource( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  m_pFieldCharaModelManager->DeserializeSaveData( mSaveData->GetFieldMoveModelSave() );
}

//----------------------------------------------------------------------------
/**
*  @brief      カメラ保存情報　設定・取得
*/
//-----------------------------------------------------------------------------
void GameData::SetFieldCameraData( const Field::Camera::SaveData& rData )
{
  *m_pFieldCameraData = rData;
}

void GameData::GetFieldCameraData( Field::Camera::SaveData* pData )
{
  *pData = *m_pFieldCameraData;
}

void GameData::ClearFieldCameraData( void )
{
  m_pFieldCameraData->areaId = Field::AREAID_ERROR;
  m_pFieldCameraData->areaDataNo = Field::Camera::Area::AreaDataNo::INVALID;
  m_pFieldCameraData->oldAreaDataNo = Field::Camera::Area::AreaDataNo::INVALID;
  m_pFieldCameraData->focusStopBit = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワープIDの保存
 */
//-----------------------------------------------------------------------------
void GameData::SetWarpID( int warp_id )
{
  mSaveData->GetSituation()->SetWarpID( warp_id );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ワープIDの取得
 */
//-----------------------------------------------------------------------------
int GameData::GetWarpID( void ) const
{
  return mSaveData->GetSituation()->GetWarpID();
}

//----------------------------------------------------------------------------
/**
 *	@brief  現在地のZoneIDの取得
 */
//-----------------------------------------------------------------------------
Field::ZoneID GameData::GetNowZoneID( void ) const
{
  return mStartLoc.GetZoneID();
}


//-----------------------------------------------------------------------------
// private
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * @brief　ジョインフェスタで2重化しているセーブ領域の反映させるべきものをオリジナルに下記戻す。
 */
//-----------------------------------------------------------------------------
void GameData::serializeJoinFestaFieldData( void )
{
  // Misc内のカメラエリアデータ番号以外を引き継ぐ
  mSaveData->GetMisc()->CopyData( m_JoinFestaSwitchFieldData.mMisc, true );

  // Situationのカウント系は引き継ぐ
  mSaveData->GetSituation()->CopyData( m_JoinFestaSwitchFieldData.mSituation, true );

  // ロケーションのデータは全て捨てデータ
}

//-----------------------------------------------------------------------------
/**
 * @brief さいしょからはじめる、ときのバトル施設データ初期化
 */
//-----------------------------------------------------------------------------
void GameData::setFirstDataBattleInst( void )
{
  Savedata::BattleInstSave* pBattleInstSave = mSaveData->GetBattleInstSave();

  pBattleInstSave->AddScoutTrainerData( 80, 0, 1, 0, 0 );
}


//--------------------------------------------------------------
/**
 * @brief   セーブ処理（電源切断時用）
 * @retval  true  セーブ処理終了
 * @retval  false セーブ処理継続中
 *
 * @note    セーブの状態にあわせてなるべく適切に終えられるように分岐する
 */
//--------------------------------------------------------------
bool GameData::FinalizeSaveForShutdown( void )
{
  gfl2::fs::FsCtrBackupSystem::Result result;
  switch ( mWriteState )
  {
  case WRITE_STATE_NULL:
    return true;

  case WRITE_STATE_FORMAT:
    return FormatMain(&result);
    
  case WRITE_STATE_NORMAL:
    //通常セーブ中：単に終了を待つ
    result = SaveMain();
    return ( result != gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE );

  case WRITE_STATE_FIRST:
  case WRITE_STATE_FIRST_END:
    // 通信同期セーブ途中と思われるので失敗終了にする
    return true;

  case WRITE_STATE_LAST:
    //Lastセーブ中単に終了を待つ
    if ( LastSaveMain( &result ) == false ) return false;
    return true;

  default:
    GFL_ASSERT( 0 );
    return true;
  }
}



GFL_NAMESPACE_END(GameSys)


