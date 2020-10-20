//==============================================================================
/**
 * @file	JoinFestaFacilitiesSystem.cpp
 * @brief	ジョインフェスタ施設システム
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:47
 */
// =============================================================================
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesSystem.h"

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/TimeZone.h"
#include "System/include/HeapDefine.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptWork.h"
#include "App/Shop/include/ShopAppParam.h"

// save
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

// JFモジュール
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesData.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModel.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModelResourceManager.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesMoveModel.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

#include <niji_conv_header/message/msg_jf_shop.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)


// コンストラクタ
JoinFestaFacilitiesSystem::JoinFestaFacilitiesSystem( gfl2::heap::HeapBase* pHeap, bool isRoom ) :
  m_pHeap( pHeap )
  ,m_pFieldExtHeap( NULL )
  ,m_isRoom( isRoom )
  ,m_isInitialize( false )
  ,m_resourceManager()
  ,m_pCastleModel( NULL )
  ,m_pGroundModel( NULL )
  ,m_pRrSkyModel( NULL )
  ,m_pJoinFestaDataTable(NULL)
{
  m_pFieldExtHeap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldExtHeap();

  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    m_facilities[i].pFacilitiesData       = NULL;
    m_facilities[i].pFacilitiesModel      = NULL;
    m_facilities[i].pFacilitiesMoveModel  = NULL;
  }
}

// デストラクタ
JoinFestaFacilitiesSystem::~JoinFestaFacilitiesSystem()
{
}

//------------------------------------------------------------------
/**
 * @brief   初期化
 * @param[in] pDataTable　データテーブルのポインタ
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::Initialize( const JoinFestaDataTable* pDataTable )
{
  ICHI_PRINT( "JoinFestaFacilitiesSystem::Initialize\n" );

  // momiji追加
  // RR団のっとりイベントの時のフェスサークルは、城をRR団用に変更する。
  // →RR団用見た目はテーマの１つなので、テーマを強制変更する。
  {
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    if( pEventWork->CheckEventFlag( SYS_FLAG_BFES_ROCKET_NOTTORI ) )
    {
      // RR団のテーマにセーブデータを変更する
      Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
      pSave->SetTheme( JoinFestaScript::FES_THEME_RR );
    }
  }


  if( m_isInitialize == false )
  {
    // データテーブル
    m_pJoinFestaDataTable = pDataTable;

    // 施設データ作成
    CreateAllFacilitiesData();

    // 城外のみ
    if( m_isRoom == false )
    {
      // 店員の動作モデルセットアップ
      SetupMoveModel();
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   初期化が完了したか
 * @return  trueで完了した
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::IsInitializeComplete()
{
  if( m_isInitialize )
  {
    return true;
  }

  // 城外のみ
  if( m_isRoom == false )
  {
    //gfl2::heap::HeapBase* pFieldEventHeap = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldExtHeap();
    //bool isResouceLoadFinish = m_resourceManager.Load( pFieldEventHeap );

    bool isResouceLoadFinish = m_resourceManager.Load( m_pHeap ); // fieldDeviceHeap
    bool isMoveModelSetupEnd = IsSetupMoveModel();

    // どちらも終わっているか？
    if( !isResouceLoadFinish || !isMoveModelSetupEnd )
    {
      return false;
    }

    // 城作成
    CreateCastle();

    // 地面作成
    CreateGround();

    // RR団空作成
    CreateRrSky();

    // 全ての施設モデル作成
    CreateAllFacilitiesModel();
  }

  m_isInitialize = true;
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   破棄
 * @return  RESULT  関数の結果
 */
//------------------------------------------------------------------
NetApp::JoinFesta::RESULT JoinFestaFacilitiesSystem::Terminate()
{
  ICHI_PRINT("JoinFestaFacilitiesSystem::Terminate\n");

  // 城外のみ
  if( m_isRoom == false )
  {
    // 店員動作モデルの破棄終了チェック
    if( TerminateMoveModel() == false )
    {
      return RESULT_CONTINUE;
    }

    // 全ての施設モデル削除
    if( DeleteAllFacilitiesModel() == false )
    {
      return RESULT_CONTINUE;
    }

    // 城破棄
    if( DeleteCastle() == false )
    {
      return RESULT_CONTINUE;
    }

    // 地面破棄
    if( DeleteGround() == false )
    {
      return RESULT_CONTINUE;
    }

    // RR団用空破棄
    if( DeleteRrSky() == false )
    {
      return RESULT_CONTINUE;
    }

    // リソース破棄
    m_resourceManager.Terminate();

  }

  // 全ての施設データ削除
  DeleteAllFacilitiesData();

  m_isInitialize = false;

  return RESULT_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief   モデルアニメーション更新
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::UpdateAnime()
{
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    if( m_facilities[i].pFacilitiesModel != NULL )
    {
      m_facilities[i].pFacilitiesModel->UpdateAnime();
    }
  }
  if( m_pCastleModel != NULL )
  {
    m_pCastleModel->UpdateAnime();
  }
  if( m_pRrSkyModel != NULL )
  {
    m_pRrSkyModel->UpdateAnime();
  }
}

//------------------------------------------------------------------
/**
 * @brief   施設データ取得
 * @param[in] facilitiesIndex 施設インデックス
 * @return  施設データのポインタ
 */
//------------------------------------------------------------------
const JoinFestaFacilitiesData* JoinFestaFacilitiesSystem::GetFacilitiesData( JoinFestaScript::FacilitiesIndex facilitiesIndex ) const
{
  return GetFacilitiesData( facilitiesIndex );
}
JoinFestaFacilitiesData* JoinFestaFacilitiesSystem::GetFacilitiesData( JoinFestaScript::FacilitiesIndex facilitiesIndex ) 
{
  //ICHI_PRINT("JoinFestaFacilitiesSystem::GetFacilitiesData %d\n", facilitiesIndex);
  GFL_ASSERT( facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX );
  if( facilitiesIndex < JoinFestaScript::FACILITIES_INDEX_MAX )
  {
    return m_facilities[ facilitiesIndex ].pFacilitiesData;
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief   全ての施設データ作成
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::CreateAllFacilitiesData()
{
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    // 施設データ作成
    m_facilities[i].pFacilitiesData = GFL_NEW( m_pFieldExtHeap ) JoinFestaFacilitiesData( static_cast<JoinFestaScript::FacilitiesIndex>(i), m_pJoinFestaDataTable );
  }
}

//------------------------------------------------------------------
/**
 * @brief   全ての施設データ削除
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::DeleteAllFacilitiesData()
{
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    // データ破棄
    GFL_SAFE_DELETE( m_facilities[i].pFacilitiesData );
  }
}

//------------------------------------------------------------------
/**
 * @brief   全ての施設モデル作成（７つ）
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::CreateAllFacilitiesModel()
{
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    // 施設種類取得
    JoinFestaScript::FacilitiesType facilitiesType = m_facilities[i].pFacilitiesData->GetFacilitiesType(); 
    //JoinFestaScript::FacilitiesType facilitiesType = static_cast<JoinFestaScript::FacilitiesType>(i);
    //JoinFestaScript::FacilitiesType facilitiesType = JoinFestaScript::FACILITIES_TYPE_STALL;
    ICHI_PRINT(">ICHI facilitiesType:%d \n", facilitiesType);

    // モデル作成
    {
      if( m_facilities[i].pFacilitiesModel == NULL )
      {
        m_facilities[i].pFacilitiesModel = GFL_NEW( m_pFieldExtHeap ) JoinFestaFacilitiesModel( m_pFieldExtHeap );
      }

      // 位置設定
      // memo:4200/0/4200に出して45度ずつ回転すると７つキレイにおける
      m_facilities[i].pFacilitiesModel->Create(
                    m_resourceManager.GetModel( facilitiesType ),
                    m_resourceManager.GetAnimationData(),
                    gfl2::math::Vector3( 4200.0f, 0.0f, 4200.0f ),
                    gfl2::math::ConvDegToRad( 45.0f * (i+1) ),
                    m_resourceManager.GetAnimeId( facilitiesType ),
                    m_resourceManager.GetTextureAnimeId( facilitiesType ) );


      // 施設カラー反映
      m_facilities[i].pFacilitiesModel->ChangeTexture( m_facilities[i].pFacilitiesData->GetFacilitiesColor() );
    }

    // チュートリアル中は最初の施設を空き地にしておく
    if( IsTutorial() == true && i == 0 )
    {
      m_facilities[i].pFacilitiesModel->SetVisible( false );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   全ての施設モデル削除（７つ）
 * @return  trueで完了
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::DeleteAllFacilitiesModel()
{
  // 破棄終了チェック
  bool isDelete = true;

  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    if( m_facilities[i].pFacilitiesModel == NULL )
    {
      continue;
    }

    // モデル破棄チェック
    if( m_facilities[i].pFacilitiesModel->Delete() == false )
    {
      isDelete = false;
    }
    else
    {
      GFL_SAFE_DELETE( m_facilities[i].pFacilitiesModel );
    }
  }

  return isDelete;
}

//------------------------------------------------------------------
/**
 * @brief   店員動作モデルのセットアップ
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::SetupMoveModel()
{
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    // チュートリアル中は最初の施設を空き地にしておく
    if( IsTutorial() == true && i == 0 )
    {
      continue;
    }

    JoinFestaFacilitiesData* pData = m_facilities[i].pFacilitiesData;
    GFL_ASSERT( pData );

    // 店員の動作モデル生成
    m_facilities[i].pFacilitiesMoveModel = GFL_NEW( m_pFieldExtHeap ) JoinFestaFacilitiesMoveModel( m_pFieldExtHeap );
    m_facilities[i].pFacilitiesMoveModel->Setup( 
      static_cast<JoinFestaScript::FacilitiesIndex>( i ), // 施設インデックス
      static_cast<JoinFestaScript::FacilitiesType>( pData->GetFacilitiesType() ), // 施設種類
      pData->GetCharaModelId() // キャラモデルID
      );
  }
}

//------------------------------------------------------------------
/**
 * @brief   店員動作モデルのセットアップ完了したか
 * @return  trueで完了
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::IsSetupMoveModel()
{
  bool isMoveModelSetupEnd = true;
  // 店員のセットアップ終了チェック
  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    // チュートリアル中は最初の施設を空き地にしておく
    if( IsTutorial() == true && i == 0 )
    {
      continue;
    }

    if( m_facilities[i].pFacilitiesMoveModel->IsSetup() == false )
    {
      // 1人でもセットアップ完了していないとダメ
      isMoveModelSetupEnd = false;
    }
  }
  return isMoveModelSetupEnd;
}

//------------------------------------------------------------------
/**
 @brief   店員動作モデルの破棄
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::TerminateMoveModel()
{
  // 店員動作モデルの破棄終了チェック
  bool isDelete = true;

  for( u8 i=0; i<JoinFestaScript::FACILITIES_INDEX_MAX; i++ )
  {
    if( m_facilities[i].pFacilitiesMoveModel == NULL )
    {
      continue;
    }

    if( m_facilities[i].pFacilitiesMoveModel->Delete() == false )
    {
      isDelete = false;
    }
    else
    {
      // 動作モデル破棄
      GFL_SAFE_DELETE( m_facilities[i].pFacilitiesMoveModel );
    }
  }

  return isDelete;
}



//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   城作成
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::CreateCastle()
{
  if( m_pCastleModel == NULL )
  {
    Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();


    m_pCastleModel = GFL_NEW( m_pFieldExtHeap ) JoinFestaFacilitiesModel( m_pFieldExtHeap );

    // RR団テーマ設定時
    if( pSave->GetTheme() == JoinFestaScript::FES_THEME_RR )
    {
      m_pCastleModel->Create( m_resourceManager.GetRRCastleModel(),
                              m_resourceManager.GetAnimationData(),
                              m_resourceManager.GetRRCastleAnimeId() );
    }
    // 通常時
    else
    {
      m_pCastleModel->Create( m_resourceManager.GetCastleModel(),
                              m_resourceManager.GetAnimationData(),
                              m_resourceManager.GetCastleAnimeId(),
                              m_resourceManager.GetCastleTextureAnimeId() );

      // セーブデータのテーマを反映する
      m_pCastleModel->ChangeTexture( pSave->GetTheme() );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   城削除
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::DeleteCastle()
{
  if( m_pCastleModel != NULL )
  {
    if( m_pCastleModel->Delete() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pCastleModel );
  }
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   地面作成
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::CreateGround()
{
  if( m_pGroundModel == NULL )
  {
    Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();

    const bool isAttraction = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying(); 
    void* modelBuff;

    // アトラクション時
    if( isAttraction )
    {
      modelBuff = m_resourceManager.GetAttractionGroundModel();
    }
    // RR団テーマ設定時
    else if( pSave->GetTheme() == JoinFestaScript::FES_THEME_RR )
    {
      modelBuff = m_resourceManager.GetRRGroundModel();
    }
    // 通常時
    else
    {
      modelBuff = m_resourceManager.GetNormalGroundModel();
    }

    m_pGroundModel = GFL_NEW( m_pFieldExtHeap ) JoinFestaFacilitiesModel( m_pFieldExtHeap );
    m_pGroundModel->Create( modelBuff,
                            m_resourceManager.GetAnimationData() );
  }
}

//------------------------------------------------------------------
/**
 * @brief   地面削除
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::DeleteGround()
{
  if( m_pGroundModel != NULL )
  {
    // モデル破棄チェック
    if( m_pGroundModel->Delete() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pGroundModel );
  }
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   RR団イベント用空モデル作成
 */
//------------------------------------------------------------------
void JoinFestaFacilitiesSystem::CreateRrSky()
{
  if( m_pRrSkyModel == NULL )
  {
    // フェスのテーマではなく、RR団のっとりイベント中のみこのモデルにする
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
    if( pEventWork->CheckEventFlag( SYS_FLAG_BFES_ROCKET_NOTTORI ) )
    {
      m_pRrSkyModel = GFL_NEW( m_pFieldExtHeap ) JoinFestaFacilitiesModel( m_pFieldExtHeap );
      m_pRrSkyModel->Create( m_resourceManager.GetRRSkyModel(),
                             m_resourceManager.GetAnimationData(),
                             m_resourceManager.GetRRSkyAnimeId() );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   RR団イベント用空モデル削除
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::DeleteRrSky()
{
  if( m_pRrSkyModel != NULL )
  {
    if( m_pRrSkyModel->Delete() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pRrSkyModel );
  }
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   チュートリアル中かどうか
 * @return  trueでチュートリアル中
 */
//------------------------------------------------------------------
bool JoinFestaFacilitiesSystem::IsTutorial()
{
  //! チュートリアル中かどうか
  bool isTutorial = false;
  {
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    if( *(pEventWork->GetEventWorkAdrs( WK_SYS_JF_TUTORIAL )) < 5 )
    {
      isTutorial = true;
    }
  }
  return isTutorial;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaFacilitiesSystem::ShopFeatureDelegate_CreateShopItemList(App::Shop::SHOP_ITEM** ppShopItemList, u32& rShopItemNum, gfl2::heap::HeapBase* pHeapBase, const u32 tableIndex)
{
  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  const ShopItemInfo* pShopItemInfo = m_pJoinFestaDataTable->GetShopItemInfo(tableIndex);


  GFL_ASSERT(pShopItemInfo);

  /*  create  */
  rShopItemNum      = pShopItemInfo->CountAvailableServices();
  *ppShopItemList   = GFL_NEW(pHeapBase)  App::Shop::SHOP_ITEM[rShopItemNum];

  u32 slot  = 0;
  u32 shopItemListIndex = 0;
  for(u32 i = 0; i < rShopItemNum; ++i)
  {
    /*  有効データ数はCountAvailableServices()で確認済み  */
    while(!pShopItemInfo->IsServiceAvailable(slot)) {
      ++slot;
    }

    const u32               serviceID         = pShopItemInfo->GetServiceID(slot);
    const ShopServiceInfo*  pShopServiceInfo  = m_pJoinFestaDataTable->GetShopServiceInfo(serviceID);

    ++slot;

    // プレミアムサービスだが開放していない場合は追加しない
    if( pShopServiceInfo->isHallOfFame == true && pSave->GetRewardState( JoinFestaScript::REWARD_PREMIUM ) != JoinFestaScript::REWARD_STATE_FINISH )
    {
      continue;
    }

    (*ppShopItemList)[shopItemListIndex].id    = pShopServiceInfo->messageID_ServiceName - jf_shop_name_000;    /*  jf_shop_name_000からのoffset値を設定する必要がある  */
    (*ppShopItemList)[shopItemListIndex].price = pShopServiceInfo->coinPrice;

    /*  serviceIDによって設定内容変更  */
    switch(pShopServiceInfo->serviceType)
    {
      /*  アイテム販売  */
      case  ShopServiceInfo::SERVICETYPE_Item:
        (*ppShopItemList)[shopItemListIndex].item  = pShopServiceInfo->serviceArgs[0];
        (*ppShopItemList)[shopItemListIndex].num   = pShopServiceInfo->serviceArgs[1];
        break;

      /*  その他  */
      default:
        (*ppShopItemList)[shopItemListIndex].item  = serviceID;
        (*ppShopItemList)[shopItemListIndex].num   = 0;
        break;
    }

    ++shopItemListIndex;
  }

  // 実際の品数で更新
  rShopItemNum = shopItemListIndex;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaFacilitiesSystem::ShopFeatureDelegate_HandlingResult(const App::Shop::APP_PARAM& rAppParam)
{
//  const u32   WORKID_ServiceType  = SCWK_RET0;
//  const u32   WORKID_ServiceArg0  = SCWK_RET1;
//  const u32   WORKID_ServiceArg1  = SCWK_RET2;
//  const u32   WORKID_CoinCost     = SCWK_RET3;
//  const u32   WORKVALUE_Invalid   = static_cast<u32>(-1);
  const s32   WORKVALUE_Invalid   = -1;
    
  Field::FieldScript::ScriptObject*   pScript       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldScriptSystem()->GetScriptObject();
  Field::FieldScript::Work*           pScriptWork   = pScript->GetWork();


  if(rAppParam.select_action == App::Shop::FC_SELECT_ACTION_CANCEL)
  {
    /*  キャンセル  */
//    pScriptWork->SetWorkValue(WORKID_ServiceType, WORKVALUE_Invalid);
//    pScriptWork->SetWorkValue(WORKID_ServiceArg0, WORKVALUE_Invalid);
//    pScriptWork->SetWorkValue(WORKID_ServiceArg1, WORKVALUE_Invalid);
//    pScriptWork->SetWorkValue(WORKID_CoinCost,    0u);

    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_IsHallOfFame",          WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_MessageID_ServiceName", WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_Price",                 WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_MessageID_Talk_0",      WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_MessageID_Talk_1",      WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_ServiceType",           WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_ServiceArg0",           WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_ServiceArg1",           WORKVALUE_Invalid);
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_SelectPos",             WORKVALUE_Invalid);
  }
  else
  {
    const ShopServiceInfo*      pShopServiceInfo  = m_pJoinFestaDataTable->GetShopServiceInfo(rAppParam.select_action);

    GFL_ASSERT(pShopServiceInfo);

//    pScriptWork->SetWorkValue(WORKID_ServiceType, static_cast<u32>(pShopServiceInfo->serviceType));
//    pScriptWork->SetWorkValue(WORKID_ServiceArg0, static_cast<u32>(pShopServiceInfo->GetArg(0)));
//    pScriptWork->SetWorkValue(WORKID_ServiceArg1, static_cast<u32>(pShopServiceInfo->GetArg(1)));
//    pScriptWork->SetWorkValue(WORKID_CoinCost,    static_cast<u32>(pShopServiceInfo->GetPrice()));

//    pScriptWork->SetWorkValue(WORKID_ServiceType, static_cast<u32>(pShopServiceInfo->serviceType));

    /*    */
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_IsHallOfFame",          static_cast<s32>(pShopServiceInfo->isHallOfFame));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_MessageID_ServiceName", static_cast<s32>(pShopServiceInfo->messageID_ServiceName));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_Price",                 static_cast<s32>(pShopServiceInfo->GetPrice()));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_MessageID_Talk_0",      static_cast<s32>(pShopServiceInfo->messageID_Talk[0]));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_MessageID_Talk_1",      static_cast<s32>(pShopServiceInfo->messageID_Talk[1]));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_ServiceType",           static_cast<s32>(pShopServiceInfo->serviceType));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_ServiceArg0",           static_cast<s32>(pShopServiceInfo->GetArg(0)));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_ServiceArg1",           static_cast<s32>(pShopServiceInfo->GetArg(1)));
    pScript->SetGlobalParameterInteger("gJF_ServiceInfo_SelectPos",             static_cast<s32>(rAppParam.select_pos));
  }
}

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

