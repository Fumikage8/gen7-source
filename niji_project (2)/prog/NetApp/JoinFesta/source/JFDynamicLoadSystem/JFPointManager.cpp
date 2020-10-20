//==============================================================================
/**
 * @file	JFPointManager.cpp
 * @brief	ジョインフェスタモデル設置ポイントマネージャ
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFPointManager.h"

#include <debug/include/gfl2_DebugPrint.h>

#include <system/include/Date/gfl2_Date.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// savedata
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include "Savedata/include/JoinFestaDataSave.h"

// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"
// Excelのデータ
#include "NetApp/JoinFesta/include/JoinFestaDataTable/JoinFestaDataTable.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)



// コンストラクタ
JFPointManager::JFPointManager( gfl2::heap::HeapBase* pHeap, bool isRoom, bool isTutorial, const JoinFestaDataTable* pDataTable ) :
  m_JFPointList()
  ,m_isRoom( isRoom )
  ,m_pDataTable( pDataTable )
{
  m_JFModelViewOnPointList = GFL_NEW( pHeap ) gfl2::util::List<JFPoint*>( pHeap, JoinFestaDefine::JFMODEL_DISP_MAX );

  gfl2::math::Vector3 posList[ JoinFestaDefine::JFPOINT_MAX ] = {
    gfl2::math::Vector3(4555.0f, 0.0f, 5735.1f),
    gfl2::math::Vector3(4715.9f, 0.0f, 5348.3f),
    gfl2::math::Vector3(5141.1f, 0.0f, 5452.7f),
    gfl2::math::Vector3(5524.3f, 0.0f, 5095.5f),
    gfl2::math::Vector3(5247.6f, 0.0f, 4851.8f),
    gfl2::math::Vector3(5724.2f, 0.0f, 4698.6f),
    gfl2::math::Vector3(5773.8f, 0.0f, 3951.3f),
    gfl2::math::Vector3(5433.6f, 0.0f, 3922.0f),
    gfl2::math::Vector3(5249.8f, 0.0f, 3540.1f),
    gfl2::math::Vector3(5644.4f, 0.0f, 3491.2f),
    gfl2::math::Vector3(5455.3f, 0.0f, 3197.6f),
    gfl2::math::Vector3(5130.0f, 0.0f, 2906.7f),
    gfl2::math::Vector3(4778.2f, 0.0f, 2712.3f),
    gfl2::math::Vector3(4402.5f, 0.0f, 2605.1f),
    gfl2::math::Vector3(4893.7f, 0.0f, 3098.0f),
    gfl2::math::Vector3(4051.7f, 0.0f, 2611.3f),
    gfl2::math::Vector3(4496.5f, 0.0f, 2984.9f),
    gfl2::math::Vector3(3723.0f, 0.0f, 2673.2f),
    gfl2::math::Vector3(3902.9f, 0.0f, 3025.0f),
    gfl2::math::Vector3(3404.9f, 0.0f, 2808.2f),
    gfl2::math::Vector3(3528.2f, 0.0f, 3163.6f),
    gfl2::math::Vector3(2944.6f, 0.0f, 3181.8f),
    gfl2::math::Vector3(3150.3f, 0.0f, 3542.2f),
    gfl2::math::Vector3(2748.4f, 0.0f, 3589.3f),
    gfl2::math::Vector3(2967.8f, 0.0f, 3905.9f),
    gfl2::math::Vector3(2591.1f, 0.0f, 4267.3f),
    gfl2::math::Vector3(2886.1f, 0.0f, 4384.8f),
    gfl2::math::Vector3(2686.1f, 0.0f, 4743.8f),
    gfl2::math::Vector3(3132.8f, 0.0f, 4826.3f),
    gfl2::math::Vector3(3209.0f, 0.0f, 5443.8f),
    gfl2::math::Vector3(3531.9f, 0.0f, 5280.1f),
    gfl2::math::Vector3(3733.9f, 0.0f, 5729.0f),
  };

  // 城内の場合は2つだけポイント使う
  if( m_isRoom == true )
  {
    // リクルーターで呼んだ人
    posList[0] = gfl2::math::Vector3( 1850.0f, 0.0f, 2254.0f );

    // P2P直後の人
    posList[1] = gfl2::math::Vector3( 2358.0f, 0.0f, 2232.0f );

    // 後のポイントは0にしておく
    for( int i=2; i<JoinFestaDefine::JFPOINT_MAX; i++ )
    {
      posList[i] = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
    }
  }
  else if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
  {
    // 奇数・偶数パターン
    posList[0] = gfl2::math::Vector3(4606.7f, 0.0f, 5711.7f);
    posList[1] = gfl2::math::Vector3(4854.9f, 0.0f, 5596.7f);
    posList[2] = gfl2::math::Vector3(4868.5f, 0.0f, 5248.8f);
    posList[3] = gfl2::math::Vector3(5186.6f, 0.0f, 4948.1f);
    posList[4] = gfl2::math::Vector3(5492.1f, 0.0f, 5103.6f);
    posList[5] = gfl2::math::Vector3(5667.9f, 0.0f, 4801.2f);
    posList[6] = gfl2::math::Vector3(5397.7f, 0.0f, 4664.6f);
    posList[7] = gfl2::math::Vector3(5518.1f, 0.0f, 4338.8f);
    posList[8] = gfl2::math::Vector3(5800.2f, 0.0f, 4016.6f);
    posList[9] = gfl2::math::Vector3(5695.8f, 0.0f, 3749.8f);
    posList[10] = gfl2::math::Vector3(5573.4f, 0.0f, 3631.9f);
    posList[11] = gfl2::math::Vector3(5270.7f, 0.0f, 3591.0f);
    posList[12] = gfl2::math::Vector3(4783.9f, 0.0f, 3135.0f);
    posList[13] = gfl2::math::Vector3(5348.2f, 0.0f, 3108.8f);
    posList[14] = gfl2::math::Vector3(5145.7f, 0.0f, 2984.9f);
    posList[15] = gfl2::math::Vector3(4980.7f, 0.0f, 3222.1f);
    posList[16] = gfl2::math::Vector3(4336.0f, 0.0f, 2649.1f);
    posList[17] = gfl2::math::Vector3(4849.3f, 0.0f, 2727.9f);
    posList[18] = gfl2::math::Vector3(3909.3f, 0.0f, 2907.4f);
    posList[19] = gfl2::math::Vector3(4586.2f, 0.0f, 2729.4f);
    posList[20] = gfl2::math::Vector3(3180.5f, 0.0f, 2987.6f);
    posList[21] = gfl2::math::Vector3(3899.7f, 0.0f, 2682.4f);
    posList[22] = gfl2::math::Vector3(3023.5f, 0.0f, 3746.5f);
    posList[23] = gfl2::math::Vector3(3561.6f, 0.0f, 3063.0f);
    posList[24] = gfl2::math::Vector3(2655.7f, 0.0f, 4206.6f);
    posList[25] = gfl2::math::Vector3(2936.4f, 0.0f, 3270.1f);
    posList[26] = gfl2::math::Vector3(3054.6f, 0.0f, 4645.2f);
    posList[27] = gfl2::math::Vector3(2904.3f, 0.0f, 3792.8f);
    posList[28] = gfl2::math::Vector3(2820.6f, 0.0f, 4912.2f);
    posList[29] = gfl2::math::Vector3(2636.0f, 0.0f, 4426.8f);
    posList[30] = gfl2::math::Vector3(3404.1f, 0.0f, 5233.4f);
    posList[31] = gfl2::math::Vector3(3077.3f, 0.0f, 5231.2f);
  }

  // @fix GFNMcat[2364]：城外と城内のY座標設定する
  for( int i=0; i<JoinFestaDefine::JFPOINT_MAX; i++ )
  {
    posList[i].y = (m_isRoom==true) ? 17.0f : 42.0f;
  }

  // 配置ポイントリスト生成
  for( int i=0; i<JoinFestaDefine::JFPOINT_MAX; i++ )
  {
    JFPoint* jfPoint = GFL_NEW( pHeap ) JFPoint();
    jfPoint->SetPointIndex(i);
    jfPoint->SetPointPosition( posList[i] ); 
    m_JFPointList.push_back( jfPoint );
  }

  // 城外は、基本的には出ない為別セットアップ
  if( m_isRoom )
  {
    SetupPersonalDataForRoom();
  }
  // チュートリアル中は、ダミーを出すため別セットアップ
  else if( isTutorial )
  {
    SetupPersonalDataForTutorial();
  }
  // アトラクション中はNPCも入れる為、別セットアップ
  else if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
  {
    // アトラクション専用パーソナルセットアップ
    SetupPersonalDataForAttraction( pHeap );
  }
  else
  {
    // パーソナルデータのセットアップ
    SetupPersonalData();

#if defined(GF_PLATFORM_CTR)
    // パーソナルデータ通知リスナー登録
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->RegistPersonalListener( this );
#endif // GF_PLATFORM_CTR
  }

  ICHI_PRINT("m_JFPointList size %d\n", m_JFPointList.size() );
}

// デストラクタ
JFPointManager::~JFPointManager()
{
#if defined(GF_PLATFORM_CTR)
  // パーソナルデータ通知リスナー解除
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->RemovePersonalListener( this );
#endif // GF_PLATFORM_CTR

  // 配置ポイントリスト破棄
  for( u32 i=0; i<m_JFPointList.size(); i++ )
  {
    GFL_SAFE_DELETE( m_JFPointList[i] );
  }
  m_JFPointList.clear();
  GFL_SAFE_DELETE( m_JFModelViewOnPointList );
}


//------------------------------------------------------------------
/**
 * @brief   周囲のポイントでモデルが非表示のポイントを取得
 * @param   [in]pos 調査中心座標
 * @retval  見つかった場合：モデル表示していないJFPointのポインタ
 * @retval  見つからない場合:NULL
 */
//------------------------------------------------------------------
JFPoint* JFPointManager::GetModelNonDispOfNearPoint( const gfl2::math::Vector3 pos ) const 
{
  JFPoint* saveJFPoint = NULL;
  f32 saveLength = JoinFestaDefine::JFMODEL_DISP_DISTANCE;

  // ポイントリストをチェック
  for( u32 i=0; i<m_JFPointList.size(); i++ )
  {
    // パーソナルデータがセットされていない場合はスキップ
    if( m_JFPointList[i]->IsSetPersonalData() != true )
    {
      continue;
    }

    // 調査位置とポイントとの距離を計算
    gfl2::math::Vector3 def = m_JFPointList[i]->GetPointPosition() - pos;
    f32                 defLength = def.Length();
    //ICHI_PRINT("defLength[%d] %f\n", m_JFPointList[i]->GetPointIndex(), defLength );

    // 距離が近くてモデル表示していない場合は更新
    if( defLength < saveLength && m_JFPointList[i]->IsModelView() == false )
    {
      saveLength = defLength;
      saveJFPoint = m_JFPointList[i];
    }
  }

  return saveJFPoint;
}

//------------------------------------------------------------------
/**
 * @brief   モデル表示しているポイントの中で一番遠いポイントを取得
 *
 * @param   [in]pos 調査中心座標
 * @retval  見つかった場合：JFPointのポインタ
 * @retval  見つからない場合:NULL
 */
//------------------------------------------------------------------
JFPoint* JFPointManager::GetModelDispInFarPoint( const gfl2::math::Vector3 pos ) const
{
  JFPoint* saveJFPoint = NULL;
  f32 saveLength = 0.0f;

  // ポイントリストをチェック
  for( gfl2::util::List<JFPoint*>::Iterator it = m_JFModelViewOnPointList->Begin(); it != m_JFModelViewOnPointList->End(); it++ )
  {
    // モデル表示ポイントのみチェック
    if( (*it)->IsModelView() == true )
    {
      // 調査位置とポイントとの距離を計算
      gfl2::math::Vector3 def = (*it)->GetPointPosition() - pos;
      f32                 defLength = def.Length();

      // 距離が遠い場合は更新
      if( defLength > saveLength )
      {
        saveJFPoint= (*it);
        saveLength = defLength;
      }
    }
  }

  return saveJFPoint;
}

//------------------------------------------------------------------
/**
 * @brief   モデル表示しているポイントが表示範囲内かチェック
 *
 * @param   [in]pos 調査中心座標
 * @retval  ture:全て範囲内
 * @retval  false:範囲内じゃない
 */
//------------------------------------------------------------------
bool JFPointManager::CheckModelDispPointToRange( const gfl2::math::Vector3 pos ) const
{
  // ポイントリストをチェック
  for( gfl2::util::List<JFPoint*>::Iterator it = m_JFModelViewOnPointList->Begin(); it != m_JFModelViewOnPointList->End(); it++ )
  {
    // 調査位置とポイントとの距離を計算
    gfl2::math::Vector3 def = (*it)->GetPointPosition() - pos;
    f32                 defLength = def.Length();

    // 範囲外の場合は終了
    if( defLength > JoinFestaDefine::JFMODEL_DISP_DISTANCE )
    {
      return true;
    }
  }

  return false;
}


//------------------------------------------------------------------
/**
 * @brief   モデル表示済みポイントリストに登録
 *
 * @param   [in]jfPoint   モデル表示したJFPoint
 */
//------------------------------------------------------------------
void JFPointManager::RegistJFModelViewList( JFPoint* jfPoint )
{
  // 最大数登録チェック
  if( m_JFModelViewOnPointList->Size() < m_JFModelViewOnPointList->MaxSize() )
  {
    jfPoint->ModelViewOn();
    m_JFModelViewOnPointList->PushBack( jfPoint );
    ICHI_PRINT("表示済みモデル登録 ID:%d\n", jfPoint->GetPointIndex() );
  }
}

//------------------------------------------------------------------
/**
 * @brief   モデル表示済みポイントリストを解除
 *
 * @param   [in]jfPoint   解除したいJFPoint
 */
//------------------------------------------------------------------
void JFPointManager::UnregistJFModelViewList( JFPoint* jfPoint )
{
  for( gfl2::util::List<JFPoint*>::Iterator it = m_JFModelViewOnPointList->Begin(); it != m_JFModelViewOnPointList->End(); it++ )
  {
    // インデックス値が同じのを見つけた場合削除
    if( (*it)->GetPointIndex() == jfPoint->GetPointIndex() )
    {
      jfPoint->ModelViewOff();
      m_JFModelViewOnPointList->Erase( it );
      ICHI_PRINT("表示済みモデル解除 ID:%d\n", jfPoint->GetPointIndex() );
      break;
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief   モデル表示済みポイント数を取得
 * @return  モデル表示済み数
 */
//------------------------------------------------------------------
u32 JFPointManager::GetModelViewCount()
{
  return m_JFModelViewOnPointList->Size();
}


//------------------------------------------------------------------
/**
 * @brief   ポイントを取得
 * @param   [in]index ポイントインデックス値
 * @return  JFPointのポインタ
 */
//------------------------------------------------------------------
JFPoint* JFPointManager::GetJFPoint( const u32 index )
{
  if( index < m_JFPointList.size() )
  {
    return m_JFPointList[ index ];
  }
  GFL_ASSERT(0);
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ポイントを取得
 * @param   [in]targetFriendKey 対象のフレンドキー
 * @return  JFPoint（無い場合はNULL）
 */
//------------------------------------------------------------------
JFPoint* JFPointManager::GetJFPoint( JoinFestaFriendKey targetFriendKey )
{
  // ポイントリストをチェック
  for( u32 i=0; i<m_JFPointList.size(); i++ )
  {
    // パーソナルデータがセットされていない場合はスキップ
    if( m_JFPointList[i]->IsSetPersonalData() != true )
    {
      continue;
    }

    // フレンドキー一致チェック
    if( m_JFPointList[i]->GetPersonalData()->IsSameFriendKey( targetFriendKey ) )
    {
      return m_JFPointList[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief ジョインフェスタパーソナルデータ追加イベント通知
 *
 * @param[in]  person  追加されたパーソナル情報
 */
//------------------------------------------------------------------
void JFPointManager::OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  // @fix NMCat[3592]：【フェスサークル】フェスサークル外周に同じNPCを複数人配置できる対処
  // フィールドリストに同一人物がいた場合は追加しない
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* jfPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = jfPersonalManager->GetFieldPersonalList();
  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* targetPersonal = (*it);

    if( targetPersonal )
    {
      if( targetPersonal->IsSameFriendKey( person.GetJoinFestaFriendKey() ) )
      {
        GFL_PRINT("OnAddJoinFestaPerson -> 同一人物がいるので追加しない\n");
        return;
      }
    }
  }

  // 空いているポイントがあれば追加するだけ
  for( u32 i=0; i<m_JFPointList.size(); i++ )
  {
    if( m_JFPointList[i]->IsSetPersonalData() != true )
    {
      // 常駐の出現リストの方に追加
      NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->AddFieldPersonalList( person );

      // 空いていたので追加
      if( pPersonal != NULL )
      {
        m_JFPointList[i]->SetPersonalData( pPersonal );
        GFL_PRINT("通信で来た人追加[%d]\n", i);
      }
      break;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief パーソナルデータのセットアップ
 */
//------------------------------------------------------------------
void JFPointManager::SetupPersonalData()
{
  // フィールド出現用パーソナルリスト
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* jfPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = jfPersonalManager->GetFieldPersonalList();
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  // フィールド出現用パーソナルリストに入っているパーソナルをそのまま使う
  u32 pointIndex = 0;
  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* targetPersonal = (*it);

    if( targetPersonal )
    {
      // @fix NMCat[1152]：プリセットはフィールドリスト作成時に決めた為、このタイミングでは着せ替えだけ設定する
      if( targetPersonal->IsPresetNpcData() )
      {
        targetPersonal->SetupPresetNpcDressUp( m_pDataTable );
      }

      m_JFPointList[ pointIndex ]->SetPersonalData( targetPersonal );

      NetAppLib::JoinFesta::JoinFestaPersonalData::Date date = m_JFPointList[ pointIndex ]->GetPersonalData()->GetUpdateDate();
      ICHI_PRINT(">ICHI %d:更新日時 %2d/%2d/%2d %2d:%2d:%2d\n",
         pointIndex, date.year, date.month, date.day, date.hour, date.minute, date.half_sec );
      pointIndex++;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief アトラクション専用パーソナルデータのセットアップ
 */
//------------------------------------------------------------------
void JFPointManager::SetupPersonalDataForAttraction( gfl2::heap::HeapBase* pHeap )
{
  JoinFestaScript::AttractionId attractionId = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetPlayAttractionId();

  // アトラクション毎の出現リスト作成
  NetAppLib::JoinFesta::JoinFestaAttractionUtil::CreateFieldPersonalListForAttraction( attractionId, m_pDataTable );


  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* jfPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  /************************************************************/
  //  アトラクション中は半分しか設置しない
  //  偶数か奇数ポイントのみ置くことにする
  //
  //  フィールドのリストはここで作り直す
  /************************************************************/
  u8 pointIndexTable[ JoinFestaDefine::JFPOINT_MAX/2 ] = { 0 };
  u8 tableIndex = 0;

  // 偶数ポイントか奇数ポイントかはランダム
  {
    u8 startCount = System::GflUse::GetPublicRand(2);
    for( u8 i=0; i<JoinFestaDefine::JFPOINT_MAX/2; i++ )
    {
      pointIndexTable[ i ] = startCount;
      startCount += 2;
    }
  }

  // 多少シャッフルする
  if( attractionId != JoinFestaScript::ATTRACTION_RIDE_KENTAROSU_1 )
  {
    // とりあえず15回くらい
    for( int i=0; i<15; i++ )
    {
      u8 changeIndex1 = System::GflUse::GetPublicRand(JoinFestaDefine::JFPOINT_MAX/2);
      u8 changeIndex2 = System::GflUse::GetPublicRand(JoinFestaDefine::JFPOINT_MAX/2);
      u8 tmp;

      tmp = pointIndexTable[ changeIndex1 ];
      pointIndexTable[ changeIndex1 ] = pointIndexTable[ changeIndex2 ];
      pointIndexTable[ changeIndex2 ] = tmp;
    }
  }

  // フィールド出現用パーソナルリストに入っているパーソナルをそのまま使う
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList = jfPersonalManager->GetFieldPersonalList();
  for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it=pPersonalList->Begin(); it!=pPersonalList->End(); it++ )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* targetPersonal = (*it);

    if( targetPersonal )
    {
      if( tableIndex >= JoinFestaDefine::JFPOINT_MAX/2 )
      {
        // 最大値来ていたら終了
        GFL_ASSERT(0); // 来ないはず
        break;
      }

      const u8 pointIndex = pointIndexTable[ tableIndex ];
      m_JFPointList[ pointIndex ]->SetPersonalData( targetPersonal );

      tableIndex++;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief チュートリアル専用パーソナルデータのセットアップ
 */
//------------------------------------------------------------------
void JFPointManager::SetupPersonalDataForTutorial()
{
  // 既存のリストは解放
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ReleaseFieldPersonalList();

  // 3人だけダミーで出す
  NetAppLib::JoinFesta::JoinFestaPersonalData dummyData;

  for( u32 i=0; i<3; i++ )
  {
    // ダミーデータ作成
    dummyData.SetupTutorialDummyData( i, m_pDataTable );

    // 常駐の出現リストの方に追加
    NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->AddFieldPersonalList( dummyData );
    if( pPersonal != NULL )
    {
      // 一言ウィンドウの色を変えたいので、イベント発動状態にしておく
      pPersonal->SetupTutorialDummyEvent();

      m_JFPointList[ i ]->SetPersonalData( pPersonal );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief 城内専用パーソナルデータのセットアップ
 */
//------------------------------------------------------------------
void JFPointManager::SetupPersonalDataForRoom()
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

  NetAppLib::JoinFesta::JoinFestaPersonalData* pLastP2pPersonal = NULL;

  // P2Pで遊んだ人がいた場合
  {
    pLastP2pPersonal = &pJoinFestaPersonalDataManager->GetLastP2pPersonal();
    pLastP2pPersonal->Copy( pJoinFestaDataSave->GetLastP2pPersonal() );

    if( pLastP2pPersonal->IsEnable() )
    {
      // もし常駐にいたらデータ更新する
      {
        // @fix[1447]　通信経路までコピーしない
        const JoinFestaScript::CommRouteType saveCommRoute = pLastP2pPersonal->GetCommRoute();

        NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = pJoinFestaPersonalDataManager->GetPersonalData( pLastP2pPersonal->GetJoinFestaFriendKey() );
        if( pPersonal != NULL )
        {
          ICHI_PRINT(">ICHI 常駐にいたのでデータ更新");
          pLastP2pPersonal->Copy( *pPersonal );

          // 必ず通信経路は上書き
          pLastP2pPersonal->SetCommRouteForce( saveCommRoute );
        }
      }


      // パーソナルイベント初期化
      pLastP2pPersonal->ResetPersonalEvent();

      // 専用イベント終了していなければ発動
      if( !pJoinFestaDataSave->IsFinishLastP2pPersonalEvent() )
      {
        pLastP2pPersonal->SetLastP2PPersonalEvent();
      }

      m_JFPointList[ 1 ]->SetPersonalData( pLastP2pPersonal );
    }
  }

  // リクルーターで選んだ人がいた場合
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = &pJoinFestaPersonalDataManager->GetRecruiterPersonal();

    if( pPersonal != NULL && pPersonal->IsEnable() )
    {
      // P2Pと同じ人は出さない 
      if( !pLastP2pPersonal->IsSameFriendKey( pPersonal->GetJoinFestaFriendKey() ) )
      {
        // パーソナルイベント初期化
        pPersonal->ResetPersonalEvent();
        m_JFPointList[ 0 ]->SetPersonalData( pPersonal );
      }
    }
  }

  
}

#if PM_DEBUG
// 位置をDump
void JFPointManager::Dump()
{
  GFL_PRINT("========================================\n");
  GFL_PRINT("JFPointDump\n");
  GFL_PRINT("----------------------------------------\n");
  for( u32 i=0; i<m_JFPointList.size(); i++ )
  {
    gfl2::math::Vector3 pos = m_JFPointList[i]->GetPointPosition();
    GFL_PRINT(" gfl2::math::Vector3(%.1ff, 0.0f, %.1ff),\n", pos.x, pos.z );
    //calc( i, pos );
  }
  GFL_PRINT("----------------------------------------\n");
}

void JFPointManager::calc( int i, gfl2::math::Vector3 pos )
{
  const gfl2::math::Vector3 centerPosition( 4200.0f, 0.0f, 4200.f ); // マップの真ん中
  const gfl2::math::Vector3 playerPosition = pos;

  gfl2::math::Vector3 targetPosition; // 注視点
  gfl2::math::Vector3 cameraPosition; // カメラ位置

  f32 CameraDistance;
  f32 CameraHeight;
  f32 TargetDistance;
  f32 TargetHeight;

  if( i%2 == 0 )
  {
    CameraDistance = 1870.0f; 
    CameraHeight = 210.0f; 
    TargetDistance = -110.0f; 
    TargetHeight = -1980.0f; 
  }
  else
  {
    CameraDistance = 1960.0f; 
    CameraHeight = 405.0f; 
    TargetDistance = -90.0f; 
    TargetHeight = -2200.0f; 
  }

  //  
  {
    // 中心点からプレイヤーへのベクトル
    gfl2::math::Vector3 vec = playerPosition - centerPosition;
    vec.y = CameraHeight;
    vec = vec.Normalize();

    // 距離
    gfl2::math::Vector3 cameraOffset = vec * CameraDistance;

    cameraPosition.x = centerPosition.x + cameraOffset.x;
    cameraPosition.y = centerPosition.y + cameraOffset.y;
    cameraPosition.z = centerPosition.z + cameraOffset.z;

    // 注視点
    // カメラからプレイヤーへのベクトル
    vec = cameraPosition - centerPosition;
    vec.y = TargetHeight;
    vec = vec.Normalize();
    gfl2::math::Vector3 targetOffset = vec * TargetDistance;
    targetPosition.x = playerPosition.x + targetOffset.x;
    targetPosition.y = playerPosition.y + targetOffset.y;
    targetPosition.z = playerPosition.z + targetOffset.z;
  }

  GFL_PRINT("--------------------------\n");
  GFL_PRINT(" [%d]\n", i);
  GFL_PRINT("%.2f,\n%.2f,\n%.2f,\n", cameraPosition.x, cameraPosition.y, cameraPosition.z );
  GFL_PRINT("%.2f,\n%.2f,\n%.2f\n", targetPosition.x, targetPosition.y, targetPosition.z );
}
#endif

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

