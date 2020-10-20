/**
 *  GAME Freak inc.
 *
 *  @file   FieldTrafficManager.cpp
 *  @brief  往来管理
 *  @author miyachi_soichi
 *  @date   2015/06/25
 */

// Field
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficActor.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficDataAccessor.h"
// Script
#include "niji_reference_files/script/FieldPawnTypes.h"
// system
#include "System/include/GflUse.h"
#include "GameSys/include/TimeZone.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// savedata
#include "Savedata/include/EventWork.h"
// place
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
// weather
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
#include "niji_conv_header/field/WeatherKindDef.h"
// version
#include "niji_conv_header/field/FieldPlacementPMVersion.h"

GFL_NAMESPACE_BEGIN(Field)

static const s32 FIELD_TRAFFIC_HEAP_SIZE = 0x2000;

/**
 *  @brief  コンストラクタ
 */
TrafficManager::TrafficManager( void )
: m_pHeap( NULL )
, m_pMoveModelManager( NULL )
, m_pWeatherControl( NULL )
, m_pResource( NULL )
, m_ReloadList()
, m_IsInZone( false )
, m_ZoneId( ZONEID_ERROR )
{
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficPath[ i ] = NULL;
    m_pTrafficActor[ i ] = NULL;
  }
}

/**
 *  @brief  デストラクタ
 */
TrafficManager::~TrafficManager( void )
{
  Terminate();
}

/**
 *  @brief  初期化
 *  @param  pDeviceHeap         デバイスヒープ
 *  @param  pMoveModelManager   動作モデル管理
 */
void TrafficManager::Initialize( gfl2::heap::HeapBase *pDeviceHeap, Field::MoveModel::FieldMoveModelManager *pMoveModelManager )
{
  m_pMoveModelManager = pMoveModelManager;
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( pDeviceHeap, FIELD_TRAFFIC_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FieldTrafficManager" );
  m_ReloadList.CreateBuffer( pDeviceHeap, Traffic::FIELD_TRAFFIC_MAX );
  
  // パスとアクターの入れ物はこちらで生成
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficPath[ i ] = GFL_NEW( m_pHeap ) TrafficPath();
  }
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    GFL_SAFE_DELETE( m_pTrafficActor[ i ] );
    m_pTrafficActor[ i ] = GFL_NEW( m_pHeap ) TrafficActor();
    m_pTrafficActor[ i ]->Initialize( m_pMoveModelManager, this );
  }
}

/**
 *  @brief  コリジョンシーンの設定
 *  @param  setup      初期化パラメータ
 */
void TrafficManager::SetupCollisionScene( const Traffic::SetupCollision &setup )
{
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    if( !m_pTrafficActor[ i ] ){ continue; }

    m_pTrafficActor[ i ]->SetupCollisionScene( setup );
  }
}

/**
 *  @brief  破棄
 */
void TrafficManager::Terminate( void )
{
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    if( !m_pTrafficActor[ i ] ){ continue; }
    m_pTrafficActor[ i ]->DeleteActor();
    GFL_SAFE_DELETE( m_pTrafficActor[ i ] );
  }
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    GFL_SAFE_DELETE( m_pTrafficPath[ i ] );
  }
  m_ReloadList.Clear();
  if( m_pHeap )
  {
    GFL_DELETE_HEAP( m_pHeap );
    m_pHeap = NULL;
  }
}

/**
 *  @brief  更新
 */
void TrafficManager::Update( void )
{
  // リロード
  updateReloadList();
  // 終了検知
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    if( !m_pTrafficActor[ i ] ) { continue; }
    if( m_pTrafficActor[ i ]->IsEndTraffic() )
    {
      // 往来挙動を終了する
      u32 actor_idx = i;
      u32 path_idx = m_pTrafficActor[ i ]->GetPathIdx();
      endTrafficNpc( actor_idx, path_idx );
    }
    else if( m_pTrafficActor[ i ]->GetEscapeFlag() )
    {
      m_pTrafficActor[ i ]->NotifyEndTraffic();
    }
  }
}

/**
 *  @brief  パスを生成する
 *  @param  zoneID    生成するゾーンID
 */
void TrafficManager::CreateZonePath( gfl2::heap::HeapBase *pFieldHeap, ZoneID zoneID, PlacementDataManager *pPlaceMgr )
{
  m_ZoneId            = zoneID;

  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficPath[ i ]->Initialize( pFieldHeap );
  }
  // パスデータをリソースから取得する
  m_pResource = pPlaceMgr->GetZonePlacementData( zoneID, PlacementDataManager::PACK_IDX_TRAFFIC );
  TrafficDataAccessor accessor;
  accessor.Initialize( m_pResource );
  // パスを生成する
  u32 path_num = accessor.GetDataMax();
  for( u32 i = 0; i < path_num; ++ i )
  {
    const Traffic::TrafficPathData *pTrafficPathData = accessor.GetTrafficPathData( i );
    // 生成チェック
    if( !checkPathCreate( pTrafficPathData ) ){ continue; }

    const Traffic::PathCoreData *pCoreData = accessor.GetPathCoreData( *pTrafficPathData );
    m_pTrafficPath[ i ]->CreatePath( pCoreData );
  }
  // リロード禁止解除
  m_IsInZone = true;
}

/**
 *  @brief  パスの破棄
 */
void TrafficManager::DeleteZonePath()
{
  m_ReloadList.Erase();
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficPath[ i ]->DeletePath();
    m_pTrafficPath[ i ]->Terminate();
  }
}

/**
 *  @brief  演者のリソース復帰
 */
void TrafficManager::CreateZoneActorResource()
{
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficActor[ i ]->CreateActorResource();
  }
}

/**
 *  @brief  演者のリソース破棄
 */
void TrafficManager::DeleteZoneActorResource()
{
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficActor[ i ]->DeleteActorResource();
  }
}

/**
 *  @brief  演者の破棄
 */
void TrafficManager::DeleteZoneActor()
{
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficActor[ i ]->DeleteActor();
  }
}

void TrafficManager::UnRegistActor()
{
  // リロード禁止
  m_IsInZone = false;

  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    m_pTrafficActor[ i ]->UnRegistActor();
  }

  // リクエストの全破棄
  m_ReloadList.Erase();
}

/**
 *  @brief  ゾーン変更後のリロード対応
 */
void TrafficManager::Request_ReloadActor( void )
{
  m_ReloadList.Erase();
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    // 未初期化は飛ばす
    if( !m_pTrafficPath[ i ]->CheckFlags( TrafficPath::FLAG_INITIALIZED ) ){ continue; }
    // 使用済みは飛ばす
    if( m_pTrafficPath[ i ]->CheckFlags( TrafficPath::FLAG_USED ) ){ continue; }

    // NPCさくせい登録
    if( !m_IsInZone ){ continue; }

    // ランダム作成:１回目
    if( reloadTrafficNpc( i, true ) ){ continue; }
    // ランダム作成:２回目
    if( reloadTrafficNpc( i, true ) ){ continue; }

    // 正規作成
    ReloadData data;
    data.path_idx = i;
    data.cool_time = 0.0f;
    data.bNormal = true;
    data.reload_type = RELOAD_TYPE_NORMAL;
    m_ReloadList.PushBack( data );
  }
}

/**
 *  @brief  プレイヤーがワープしてくるから解消を予約する
 *  @param  pos   プレイヤーのワープ先の座標
 *  @note   あくまで予約を行うだけなので解消が完了したかどうかは
 *          CheckEscape関数にて行うこと
 */
void TrafficManager::RequestPlayerWarp( const gfl2::math::Vector3 &pos )
{
  for( u32 idx = 0; idx < Traffic::FIELD_TRAFFIC_MAX; ++ idx )
  {
    if( !m_pTrafficActor[idx]->IsUsed() ){ continue; }
    Field::MoveModel::FieldMoveModel *pModel = m_pTrafficActor[idx]->GetMoveModel();
    if( !pModel ){ continue; }
    if( !pModel->GetCharaDrawInstance() ){ continue; }
    if( !pModel->GetVisible() ){ continue; }

    gfl2::math::Vector3 act_pos = pModel->GetCharaDrawInstance()->GetPosition();
    gfl2::math::Vector3 diff = act_pos - pos;
    diff.y = 0.0f;
    
    // チェック距離：NPCと最大移動量*2とライド最大サイズ
    f32 checkLength = 37.5f + 40.0f + 40.0f + 100.0f;
    if( diff.Length() <= checkLength )
    {
      // 退避命令発行
      m_pTrafficActor[idx]->RequestEscape();
    }
  }
}

/**
 *  @brief  プレイヤーワープ先の往来が退避完了したか調べる関数
 *  @param  true  退避が完了した＝ワープしていい
 *  @param  false 退避が未完了＝ワープしてはいけない
 */
bool TrafficManager::CheckEscape( void )
{
  for( u32 idx = 0; idx < Traffic::FIELD_TRAFFIC_MAX; ++ idx )
  {
    if( !m_pTrafficActor[idx]->IsUsed() ){ continue; }
    if( m_pTrafficActor[idx]->GetEscapeFlag() )
    {
      // 一人でも退避が完了していないなら
      return false;
    }
  }

  return true;
}

/**
 *  @brief  スクリプトで表示をコントロールする
 *  @param  bDisp   表示状態
 */
void TrafficManager::RequestScriptDisp( bool bDisp )
{
  for( u32 idx = 0; idx < Traffic::FIELD_TRAFFIC_MAX; ++ idx )
  {
    if( !m_pTrafficActor[idx]->IsUsed() ){ continue; }
    m_pTrafficActor[idx]->RequestScriptDisp( bDisp );
  }
}

/**
*  @brief  スクリプトで表示をコントロールする
*  @param  bDisp   表示状態
*/
s32 TrafficManager::GetPathCount( void ) const
{
  s32 count = 0;
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    if( m_pTrafficPath[ i ] && m_pTrafficPath[ i ]->CheckFlags( TrafficPath::FLAG_INITIALIZED ) )
    {
      count++;
    }
  }
  return count;
}

#if PM_DEBUG
void TrafficManager::Debug_CreateActor( void )
{
}
void TrafficManager::Debug_VectorPos( u32 num, gfl2::math::Vector3 *vec_list, bool bCurve )
{
}
#endif // PM_DEBUG

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  パス生成チェック
 */
bool TrafficManager::checkPathCreate( const Traffic::TrafficPathData *data )
{
  const Field::EventWork *cpEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
  // フラグ
  if( EventWork::CheckIDFlagRenge( data->flagwork ) )
  {
    if( data->flagwork != 0 )
    {
      if( cpEventWork->CheckEventFlag( data->flagwork ) ){ return false; }
    }
  }
  else
  {
    const u16 *pwork = cpEventWork->GetEventWorkAdrs( data->flagwork );
    if( pwork )
    {
      if( (*pwork ) != data->flagwork_num ){ return false; }
    }
  }

  return true;
}

/**
 *  @brief  リロードリストの更新
 */
void TrafficManager::updateReloadList( void )
{
  // イベント中なら
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameEventManager()->IsExists() )
    return;

  ReloadList::Iterator it = m_ReloadList.Begin();
  while( it != m_ReloadList.End() )
  {
    if( !it->bNormal ){ ++ it; continue; }

    bool bRandom = ( it->reload_type != RELOAD_TYPE_NORMAL );
    if( it->cool_time > 0.0f )
    {
      // 冷却中
      it->cool_time -= 1.0f;
    }
    else if( reloadTrafficNpc( it->path_idx, bRandom ) )
    {
      it = m_ReloadList.Erase( it );
      // 一人作成したので次回へ回す
      break;
    }
    else if( bRandom )
    {
      it->reload_type += 1;
    }

    ++ it;
  }
}

/**
 *  @brief  往来NPCの生成
 *  @param  path_idx    パス番号
 *  @return 生成の成否を返却
 */
bool TrafficManager::reloadTrafficNpc( u32 path_idx, bool bRandom )
{
  // 範囲外
  if( Traffic::FIELD_TRAFFIC_MAX <= path_idx ) { return false; }
  // 使用中
  if( m_pTrafficPath[ path_idx ]->CheckFlags( TrafficPath::FLAG_USED ) ) { return false; }
  // アクターの空きがない
  u32 actor_idx = getEmptyActorIndex();
  if( actor_idx == Traffic::FIELD_TRAFFIC_MAX ) { return false; }
  // NPCデータを取得
  const Traffic::NpcCoreData *pCoreData = selectNpcCoreData( path_idx );
  // NPCが選出できなかった
  if( !pCoreData ) { return false; }
  f32 fProgress = 0.0f;
  if( bRandom )
  {
    f32 rand = static_cast<f32>( System::GflUse::GetPublicRand( 1000 ) );
    fProgress = rand / 1000.0f;
  }
  // 座標のチェック
  m_pTrafficPath[ path_idx ]->SetProgress( fProgress );
  gfl2::math::Vector3 vPos = m_pTrafficPath[ path_idx ]->GetCurrentPosition();
  if( isInPlayer( vPos ) ){ return false; }
  // 生成
  m_pTrafficActor[ actor_idx ]->CreateActorWork( path_idx, *pCoreData );
  m_pTrafficActor[ actor_idx ]->SetProgress( fProgress );
  m_pTrafficActor[ actor_idx ]->CreateActorResource();

  return true;
}

/**
 *  @brief  Actorの空きを取得する
 *  @return 未使用のActor配列番号を返却する
 */
u32 TrafficManager::getEmptyActorIndex( void )
{
  for( u32 i = 0; i < Traffic::FIELD_TRAFFIC_MAX; ++ i )
  {
    if( m_pTrafficActor[ i ]->IsUsed() ) { continue; }

    // 未使用配列番号の返却
    return i;
  }

  // 発見できなかった
  return Traffic::FIELD_TRAFFIC_MAX;
}

/**
 *  @brief  NPCを選出する
 */
const Traffic::NpcCoreData *TrafficManager::selectNpcCoreData( u32 path_idx )
{
  SelectList select_list;
  select_list.CreateBuffer( m_pHeap, 6 );

  // NPCテーブルを取得する
  TrafficDataAccessor trafficAccessor; trafficAccessor.Initialize( m_pResource );
  const Traffic::TrafficPathData *pTrafficPathData = trafficAccessor.GetTrafficPathData( path_idx );
  const void *dataHeader = trafficAccessor.GetNpcCoreData( *pTrafficPathData );
  TrafficNpcDataAccessor npcAccessor; npcAccessor.Initialize( dataHeader );

  u32 total_ratio = 0;
  u32 maxi = npcAccessor.GetDataMax();
  for( u32 i = 0; i < maxi; ++ i )
  {
    const Traffic::NpcCoreData *data = npcAccessor.GetNpcCoreData( i );

    if( isEvent( data ) )
    {
      total_ratio += data->appearance;
      SelectData select_data;
      select_data.idx = i;
      select_data.ratio = data->appearance;
      select_data.data = data;
      select_list.PushBack( select_data );
    }
  }

  if( total_ratio <= 0 ){ return NULL; }

  // 乱数
  u32 rand = System::GflUse::GetPublicRand( total_ratio );

  const Traffic::NpcCoreData *pResult = NULL;
  SelectList::Iterator it = select_list.Begin();
  while( it != select_list.End() )
  {
    if( rand < it->ratio )
    {
      pResult = it->data;
    }

    // 次へ
    rand -= it->ratio;
    ++ it;
  }
  select_list.Clear();
  return pResult;
}

/**
 *  @brief  条件チェック
 */
bool TrafficManager::isEvent( const Traffic::NpcCoreData *data )
{
#if 1
  // ROMチェック
  if( ( data->romVersion != Field::PLACEMENT_ROM_VERSION_NONE ) && (GET_VERSION() != data->romVersion ) ){ return false; }

#if 0 // nijiでは使わない
  // @caution m_pWeatherControlを往来より先に作成する必要があるのでUpperでは移植の際に注意すること
  // 天候チェック
  if( m_pWeatherControl )
  {
    Field::weather::WeatherKind eKind = m_pWeatherControl->GetNowWeatherKind();
    u32 nWeatherBit = 0x01 << eKind;
    // 0：登場できる/1：登場できない
    if( ( data->weatherBit & nWeatherBit ) == 1 ){ return false; }
  }
#endif
#endif

  // 時間帯
  GameSys::TimeZone::Zone eZone = GameSys::TimeZone::GetNowTimeZone();
  u32 nTimeZoneBit = 0x01 << eZone;
  if( ( data->timezoneBit & nTimeZoneBit ) == 0 ){ return false;}

  return true;
}

/**
 *  @brief  往来NPCが到着したので片付ける
 */
void TrafficManager::endTrafficNpc( u32 actor_idx, u32 path_idx )
{
  // アクターを片す
  m_pTrafficActor[ actor_idx ]->DeleteActor();
  // パス解放
  m_pTrafficPath[ path_idx ]->EndUse();

  TrafficDataAccessor accessor; accessor.Initialize( m_pResource );
  const Traffic::TrafficPathData *pTrafficPathData = accessor.GetTrafficPathData( path_idx );
  // 不正データ
  if( !pTrafficPathData ){ return; }
  f32 coolTime = pTrafficPathData->coolTime;
  // 待ち時間が不正
  if( coolTime < 0.0f ) { return; }
  // 予約禁止
  if( !m_IsInZone ){ return; }

  // リロード予約
  ReloadData data;
  data.path_idx = path_idx;
  data.cool_time = coolTime * 30.0f;  // sec -> frame
  data.reload_type = RELOAD_TYPE_NORMAL;
  data.bNormal = true;
  m_ReloadList.PushBack( data );
}

/**
 *  @brief  生成位置をプレイヤーが占拠しているかチェック
 */
bool TrafficManager::isInPlayer( const gfl2::math::Vector3 &vPos )
{
  if( !m_pMoveModelManager ){ return true; }
  Field::MoveModel::FieldMoveModel *pPlayer = m_pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
  if( !pPlayer ){ return true; }

  gfl2::math::Vector3 player_pos = pPlayer->GetLocalSRT().translate;
  gfl2::math::Vector3 diff = player_pos - vPos;
  diff.y = 0.0f; // @fix NMCat[3364]往来とプレイヤーの重なり対策

  // チェック距離：NPCと最大移動量*2とライド最大サイズ
  f32 checkLength = 37.5f + 40.0f + 40.0f + 100.0f;
  if( diff.Length() <= checkLength )
  {
    return true;
  }

  // 誰も塞いでいない
  return false;
}

GFL_NAMESPACE_END(Field)
