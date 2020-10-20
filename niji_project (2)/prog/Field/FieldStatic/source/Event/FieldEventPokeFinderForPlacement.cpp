//======================================================================
/**
 * @file FieldEventPokeFinderForPlacement.cpp
 * @date 2015/10/06 14:59:18
 * @author miyachi_soichi
 * @brief イベント：ポケファインダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <Fade/include/gfl2_FadeManager.h>
#include <gfx/include/gfl2_GFGL.h>
// GameSys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/DllProc.h"
// Savedata
#include "Savedata/include/Record.h"
#include "Savedata/include/PokeDiarySave.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "App/Finder/include/Field/FinderManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
// placement
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
// event
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
// menu
#include "App/FinderMenu/include/FinderMenuProc.h"
// self
#include "Field/FieldStatic/include/Event/FieldEventPokeFinderForPlacement.h"
// subjectID
#include <arc_ctr/PokeFinderSubjectTable.gaix>
// debug
#if PM_DEBUG
#include <gfx/include/gfl2_PCDebugUtil.h>
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldRo/include/Debug/FieldDebugPokeFinder.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Event)

#define ENABLE_DOF 1
#define ENABLE_UI_INPUT 0
static const u32 CONV_SEC_FROM_FRAME = 30;

/**
 *  @class  EventPokeFinderForPlacement
 *  @brief  イベント：ポケファインダー
 */
/**
 *  @brief  コンストラクタ
 *  @param  pSystemHeap   GameEvent側で利用するヒープ領域
 */
EventPokeFinderForPlacement::EventPokeFinderForPlacement( gfl2::heap::HeapBase *pSystemHeap )
: GameSys::GameEvent( pSystemHeap )
, m_pHeap( NULL )
, m_pWorkHeap( NULL )
, m_pFinderUIModule( NULL )
, m_pFieldmap( NULL )
, m_pManager( NULL )
, m_pOutRes( NULL )
, m_nSpotID( 0 )
, m_nSubjectID( -1 ) 
, m_TrialMonsNo( MONSNO_NULL )
, m_OldTotalValue( 0 )
, m_FadeDelayFrame( 0 )
, m_bNeedMenu( false )
, m_bFirst( true )
{
#if PM_DEBUG
  m_pDebug = NULL;
#endif
}

/**
 *  @brief  デストラクタ
 */
EventPokeFinderForPlacement::~EventPokeFinderForPlacement( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化処理
 *  @note   インスタンス生成直後に実行すること
 *  @param  rData   初期化設定
 */
void EventPokeFinderForPlacement::Initialize( const SetupData &rData )
{
  SetSeqNo( INIT_NONE );
  m_pHeap       = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  m_pWorkHeap   = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD );
  m_nSpotID     = rData.spot_id;
  m_TrialMonsNo = rData.mons_no;
  if( m_TrialMonsNo != MONSNO_NULL )
  {
    // 試練モード以外では使用しない
    m_pOutRes     = rData.out_res;
  }
}

/**
 *  @brief  イベントを動かしてよいかどうかの判定関数
 *  @param  gmgr    ゲームマネージャ
 */
bool EventPokeFinderForPlacement::BootChk( GameSys::GameManager *gmgr )
{
  return true;
}

/**
 *  @brief  BootChkがTrueの場合に呼び出される初期化関数
 *  @param  gmgr    ゲームマネージャ
 */
void EventPokeFinderForPlacement::InitFunc( GameSys::GameManager *gmgr )
{
  m_pFieldmap = gmgr->GetFieldmap();

#if PM_DEBUG
  if( m_nSpotID != 0xFFFFFFFF )
#endif
  {
    Field::FieldResident *pResident = gmgr->GetFieldResident();
    Field::PlacementDataManager *pPlacementDataManager = pResident->GetPlacementDataManager();
    Field::FieldPokeFinderActor *pActor = pPlacementDataManager->SearchZonePokeFinder( m_nSpotID );
    GFL_ASSERT_MSG( pActor, "FinderActor is NULL !!\n" );
    if( pActor )
    {
      m_nSubjectID = pActor->GetSubjectID();
    }
  }

#if PM_DEBUG
  m_pDebug = m_pFieldmap->GetFieldDebugMenu()->GetDebugPokeFinder();
#endif
}

/**
 *  @brief  呼び出される関数
 *  メイン処理
 *  @param  gmgr    ゲームマネージャ
 *  @return GameSys::GMEVENT_RESULT   イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT EventPokeFinderForPlacement::MainFunc( GameSys::GameManager *gmgr )
{
  if( m_pManager )
  {
    m_pManager->Update( gmgr );
  }

  switch( GetSeqNo() )
  {
  case INIT_NONE:
    {
      // 試練モードはスクリプトでフェードする
      if( m_TrialMonsNo == MONSNO_NULL )
      {
        // フェード
        //RequestFadeOut();
      }
      AddSeqNo();
    }
    break;
  case INIT_RO:
    {
      // ファインダーUIモジュールの起動
      gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      {
        m_pFinderUIModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Finder.cro");

        roMgr->StartModule( m_pFinderUIModule, true );
      }

      AddSeqNo();
    }
    /* FALL THROUGH */
  case INIT_FADEOUT_WAIT:
    {
      if( !IsEndFade() )
      {
        // フェードアウト待ち
        break;
      }

      {
        // スコアの退避
        Savedata::PokeFinderSave *pSave = gmgr->GetGameData()->GetPokeFinderSave();
        m_OldTotalValue = pSave->GetTotalValue();
      }

      createPhotoBuffer();
      m_pManager = GFL_NEW( m_pHeap ) Field::Finder::Manager;
      Field::Finder::Manager::Desc desc;
      desc.pHeap = m_pHeap;
      desc.pHeap_FileRead = m_pWorkHeap;
      desc.pHeap_Dof = NULL;
      desc.pFinderMenuParam = &m_rFinderMenuParam;
      desc.nSpotID = m_nSpotID;
      desc.nSubjectID = m_nSubjectID;
      desc.nTrialMonsNo = m_TrialMonsNo;
#if PM_DEBUG
      desc.pDebug = m_pDebug;
      desc.pDebugInfoWin = m_pFieldmap->GetFieldDebugInfoWin();
#endif
      m_pManager->Initialize( desc );

      AddSeqNo();
    }
    /* FALL THROUGH */
  case INIT_FADEIN:
    if( m_pManager->IsInitialized() )
    {
      // キャラクタ表示操作
      setVisibleCharacter( false );

      m_pManager->Execute();

      // フェードイン
      RequestFadeIn();
      AddSeqNo();
    }
    break;
  case INIT_FADEIN_WAIT:
    {
      if( !IsEndFade() )
      {
        // フェードイン待ち
        break;
      }
      AddSeqNo();
    }
    break;
  case MAIN:
    {
      u32 eResult = m_pManager->GetEndResult();
      if( eResult == Field::Finder::Manager::EndResult_Trial )
      {
        if( m_pOutRes ){ *m_pOutRes = FINDER_RESULT_SUCCESS; }
        m_bNeedMenu = false;
        AddSeqNo();
      }
      else if( eResult == Field::Finder::Manager::EndResult_Over )
      {
        if( m_pOutRes ){ *m_pOutRes = FINDER_RESULT_FULL; m_bNeedMenu = false; }
        else{ m_bNeedMenu = true; }
        AddSeqNo();
      }
      else if( eResult == Field::Finder::Manager::EndResult_Cancel )
      {
        if( m_pOutRes ){ *m_pOutRes = FINDER_RESULT_CANCEL; }
        m_bNeedMenu = false;
        AddSeqNo();
      }
    }
    break;
  case TERM_FADEOUT:
    {
      // フェード
      RequestFadeOut();
      AddSeqNo();
    }
    /* FALL THROUGH */
  case TERM_FADEOUT_WAIT:
    {
      if( !IsEndFade() )
      {
        // フェードアウト待ち
        break;
      }
      // 15FPSモードなので完全に描画されることを待つ
      if( m_FadeDelayFrame < 2 )
      {
        ++ m_FadeDelayFrame;
        break;
      }
      // 再利用のためにリセット
      m_FadeDelayFrame = 0;
      AddSeqNo();
    }
    break;
  case TERM_DELETE:
    {
      // キャラクタ表示操作
      setVisibleCharacter( true );
      // 破棄
      m_pManager->Finalize();

      AddSeqNo();
    }
    break;
  case TERM_DELETE_WAIT:
    if( m_pManager->IsFinalized() )
    {
      GFL_SAFE_DELETE( m_pManager );

      // ファインダーUIモジュールの終了
      gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      roMgr->DisposeModule( m_pFinderUIModule );
      m_pFinderUIModule = NULL;

      if( m_bNeedMenu )
      {
        SetSeqNo( MENU_FIELD_CLOSE );
      }
      else
      {
        // 終了へ
        SetSeqNo( TERM_POTAL );
      }
    }
    break;
  case MENU_FIELD_CLOSE:
    {
      // 復帰時に設定するため、現在のロケーションを保持しておく
      m_playerLocation.worldId  = m_pFieldmap->GetWorldID();
      m_playerLocation.zoneId   = m_pFieldmap->GetZoneID();
      m_playerLocation.position = m_pFieldmap->GetPlayerCharacter()->GetLocalSRT().translate;
      m_playerLocation.quaternion = m_pFieldmap->GetPlayerCharacter()->GetLocalSRT().rotation;

      // クローズイベント
      EventFieldmapClose *p_close;
      p_close = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose>( gmgr->GetGameEventManager() );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP;
      p_close->SetDesc( desc );
      p_close->SetFadeCallBack( NULL, NULL, NULL );

      // 不正アドレスへのアクセス防止のため
      m_pFieldmap = NULL;
      AddSeqNo();
    }
    break;
  case MENU_CALL:
    {
      App::FinderMenu::Proc *procFinder = GameSys::CallFinderMenuProc();
      procFinder->SetAppParam( &m_rFinderMenuParam );
      //RequestFadeIn();
      AddSeqNo();
    }
    break;
  case MENU_WAIT:
    if( !gmgr->IsProcessExists() )
    {
      AddSeqNo();
    }
    break;
  case MENU_FIELD_OPEN:
    {
      // オープンイベント
      EventFieldmapOpen *p_open;
      p_open = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen>( gmgr->GetGameEventManager() );
      p_open->SetDesc( FM_OPEN_MODE_APP_RECOVER, m_playerLocation );
      p_open->SetFadeCallBackCustom( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_NONE );

      AddSeqNo();
    }
    break;
  case TERM_POTAL:
    {
      // Fieldmapが再生成されている可能性があるので
      m_pFieldmap = gmgr->GetFieldmap();

      // 写真データの破棄
      destroyPhotoBuffer();

      // レコードの更新
      {
        Savedata::PokeFinderSave *pSave = gmgr->GetGameData()->GetPokeFinderSave();
        if( m_bNeedMenu )
        {
          // 撮影回数の増加
          Savedata::IncRecord( Savedata::Record::RECID_CAMERA_CNT );
          /**
            2017/04/21 Fri. Yu Muto
            RECID_GLOBAL_ATTRACTION_FINDERPOINTS
              ->RECID_GLOBAL_ATTRACTION_FESCOIN
            となったため削除
          */
/*
          u32 totalValue = pSave->GetTotalValue();
          if( totalValue - m_OldTotalValue >= 10000 )
          {
            Savedata::IncRecord( Savedata::Record::RECID_GLOBAL_ATTRACTION_FINDERPOINTS );
          }
*/
        }
        // 最高記録の更新
        u32 hp = pSave->GetHighValue();
        Savedata::SetRecord( Savedata::Record::RECID_FINDER_RECORD, hp );
        // 総得点の更新
        u32 tp = pSave->GetTotalValue();
        Savedata::SetRecord( Savedata::Record::RECID_FINDER_POINT, tp );
      }

#if PM_DEBUG
      if( Field::Debug::DebugTypes::s_DebugFinderParam.bLoop )
      {
        SetSeqNo( INIT_RO );
        m_bFirst = false; // 2回目です
      }
      else
#endif
      if( m_rFinderMenuParam.finder_continue )
      {
        // 続けて撮影する
        SetSeqNo( INIT_RO );
        m_rFinderMenuParam.finder_continue = false;
        m_bFirst = false; // 2回目です
        setTutorialContinue( gmgr );
      }
      else if( m_pOutRes != NULL )
      {
        // 試練なのでフェードインしない
        SetSeqNo( END );
      }
      else
      {
        // 通常処理
        SetSeqNo( TERM_FADEIN );
      }

      // 続けて撮影しない
      if( GetSeqNo() != INIT_RO )
      {
        // 2回目 or キャンセルしていない
        if( !m_bFirst || m_bNeedMenu )
        {
          // ダイアリー
          ZoneID zoneID = m_pFieldmap->GetZoneID();
          Field::ZoneDataLoader *pLoader = gmgr->GetGameData()->GetFieldZoneDataLoader();
          void *zoneData = pLoader->GetZoneData( zoneID );
          Field::ZoneDataAccessor accessor( zoneData );
          u32 placeID = accessor.GetPlaceLabelID();
          u32 cnt = Savedata::GetRecord( Savedata::Record::RECID_CAMERA_CNT );
          PokeDiary::Set( PDID_COUNT_CAMERA_OYAJI, placeID, cnt );
        }
      }
    }
    break;
  case TERM_FADEIN:
    {
      // フェードイン
      RequestFadeIn();
      AddSeqNo();
    }
    /* FALL THROUGH */
  case TERM_FADEIN_WAIT:
    {
      if( !IsEndFade() )
      {
        // フェードイン待ち
        break;
      }
      SetSeqNo( END );
    }
    break;
  case END:
    // イベントを終了する
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
 *  @brief  呼び出される関数
 *  終了処理
 *  @param  gmgr    ゲームマネージャ
 */
void EventPokeFinderForPlacement::EndFunc( GameSys::GameManager *gmgr )
{
#if PM_DEBUG
  if( m_pDebug )
  {
    m_pDebug->m_bExec = false;
  }
#endif
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  フェードアウト
 */
void EventPokeFinderForPlacement::RequestFadeOut( void )
{
  gfl2::math::Vector4 start_col( 0.0f, 0.0f, 0.0f,   0.0f );
  gfl2::math::Vector4 end_col( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col );
}

/**
 *  @brief  フェードイン
 */
void EventPokeFinderForPlacement::RequestFadeIn( void )
{
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
}

/**
 *  @brief  フェード処理待ち
 */
bool EventPokeFinderForPlacement::IsEndFade( void )
{
  return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_DOUBLE );
}

/**
 *  @brief  バッファ作成
 */
void EventPokeFinderForPlacement::createPhotoBuffer( void )
{
  for( u32 i = 0; i < Field::TrialModel::FINDER_PHOTO_MAX ; ++ i )
  {
    m_rFinderMenuParam.photo_data[i] = GFL_NEW( m_pHeap ) Field::TrialModel::FinderPhotoData;
  }
}
/**
 *  @brief  カラーバッファの破棄
 */
void EventPokeFinderForPlacement::destroyPhotoBuffer( void )
{
  for( u32 i = 0; i < Field::TrialModel::FINDER_PHOTO_MAX ; ++ i )
  {
    GFL_SAFE_DELETE( m_rFinderMenuParam.photo_data[i] );
  }
}

/**
 *  @brief  キャラクタ表示操作
 */
void EventPokeFinderForPlacement::setVisibleCharacter( bool flag )
{
  if( !m_pFieldmap ){ return; }

  m_pFieldmap->GetPlayerCharacter()->SetVisible( flag );
  Field::MoveModel::FieldMoveModelManager *pManager = m_pFieldmap->GetMoveModelManager();

  if( flag )
  {
    for( s32 i = 0; i < Field::MoveModel::FIELD_MOVE_MODEL_MAX; ++ i )
    {
      if( !m_bKeepDisp[i] ){ continue; }

      Field::MoveModel::FIELD_MOVE_MODEL_ID id = static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( i );
      Field::MoveModel::FieldMoveModel *pModel = pManager->GetFieldMoveModel( id );
      if( !pModel ){ continue; }
      pModel->SetVisible( true );
    }
  }
  else
  {
    for( s32 i = 0; i < Field::MoveModel::FIELD_MOVE_MODEL_MAX; ++ i )
    {
      m_bKeepDisp[i] = false;

      Field::MoveModel::FIELD_MOVE_MODEL_ID id = static_cast<Field::MoveModel::FIELD_MOVE_MODEL_ID>( i );
      Field::MoveModel::FieldMoveModel *pModel = pManager->GetFieldMoveModel( id );
      if( !pModel ){ continue; }
      m_bKeepDisp[i] = pModel->GetVisible();
      pModel->SetVisible( false );
    }
  }
}

/**
 *  @brief  チュートリアル二回目対策
 */
void EventPokeFinderForPlacement::setTutorialContinue( GameSys::GameManager *pGameManager )
{
  // サブジェクトIDからチュートリアルであることを保証する
  if( m_nSubjectID < 0 ){ return; }
  s32 nArcIdx = m_nSubjectID * 2;
  if( nArcIdx != GARC_PokeFinderSubjectTable_PokeFinderSubject_0_A_BIN ){ return; }

  // サブジェクトIDからActorを取得する
  s32 nSubjectID = GARC_PokeFinderSubjectTable_PokeFinderSubject_01_A_BIN / 2;
  Field::FieldResident *pResident = pGameManager->GetFieldResident();
  Field::PlacementDataManager *pPlacementDataManager = pResident->GetPlacementDataManager();
  Field::FieldPokeFinderActor *pActor = pPlacementDataManager->SearchZonePokeFinderFromSubjectID( nSubjectID );

  // ActorからスポットIDを取得する
  if( pActor )
  {
    // チュートリアルスポットを通常スポットへ差し替える
    m_nSpotID     = pActor->GetSpotID();
    m_nSubjectID  = pActor->GetSubjectID();
  }
}

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(Field)
