//==============================================================================
/**
 * @file	JFDynamicLoadSystem.cpp
 * @brief	ジョインフェスタモデル動的読み込みシステム
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================

#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "GameSys/include/GameManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "niji_conv_header/field/FieldGimmickTypes.h"


#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
#include "Field/FieldRos/FieldGimmickJoinFestaRoom/include/FieldGimmickJoinFestaRoom.h"

// savedata
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

// JFモジュール
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFPointManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
// スクリプト
#include "niji_reference_files/script/FieldPawnTypes.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// mine
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//------------------------------------------------------------------
/**
  *  @brief    コンストラクタ
  */
//------------------------------------------------------------------
JFDynamicLoadSystem::JFDynamicLoadSystem( gfl2::heap::HeapBase* pHeap, Field::Fieldmap* pFieldmap ) :
  m_heap( pHeap )
  ,JFMODEL_WALK_SEARCH_POINT_Z(300.0f)
  ,JFMODEL_RUN_SEARCH_POINT_Z(550.0f)
  ,JFMODEL_SEARCH_POINT_X(130.0f)
  ,m_JFModelManager(NULL)
  ,m_JFPointManager(NULL)
  ,m_state(STATE_NONE)
  ,m_pFieldmap( pFieldmap )
  ,m_isInitialize(false)
  ,m_initializeSeq(0)
  ,m_isTutorial(false)
  ,m_isRREvent(false)
  ,m_isRoom(false)
  ,m_isDynamicLoadDisable(false)
{
}

// デストラクタ
JFDynamicLoadSystem::~JFDynamicLoadSystem()
{
}

//------------------------------------------------------------------
/**
 * @brief   初期化
 * @param[in]   pJoinFestaEffectSystem  エフェクトシステム
 * @param[in]   pDataTable　データテーブルのポインタ
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::Initialize( JoinFestaEffectSystem* pJoinFestaEffectSystem, const JoinFestaDataTable* pDataTable )
{
  ICHI_PRINT("JFDynamicLoadSystem::Initialize\n");
  GFL_ASSERT(!m_isInitialize);

  if( m_isInitialize == false )
  {
    // 城外か城内か
    m_isRoom = ( m_pFieldmap->GetFieldGimmickID( Field::Fieldmap::GIMMICK_IDX_ZONE ) == Field::GIMMICK_TYPE_JOIN_FESTA_ROOM ) ? true : false;

    //! チュートリアル中かどうか
    m_isTutorial = false;
    {
      Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
      if( *(pEventWork->GetEventWorkAdrs( WK_SYS_JF_TUTORIAL )) < 5 )
      {
        m_isTutorial = true;
      }
    }

    // @fix MMCat[72]:レインボーロケット団のイベント中はP2P人物表示しない 
    // RR団イベント中かどうか
    m_isRREvent = false;
    {
      Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
      if( pEventWork->CheckEventFlag( SYS_FLAG_BFES_ROCKET_NOTTORI ) )
      {
        m_isRREvent = true;
      }
    }

    // 初期値はfalse
    m_isDynamicLoadDisable = false;

    // 管理クラス群
    m_JFModelManager = GFL_NEW( m_heap ) JFModelManager( m_heap, pJoinFestaEffectSystem );
    m_JFPointManager = GFL_NEW( m_heap ) JFPointManager( m_heap, m_isRoom, m_isTutorial, pDataTable );
  }
}

//------------------------------------------------------------------
/**
 * @brief   初期化完了待ち
 * @return  trueで完了
 */
//------------------------------------------------------------------
bool JFDynamicLoadSystem::IsInitializeComplete()
{
  if( m_isInitialize )
  {
    return true;
  }

  // memo:動的動作モデルに話しかけ途中で、アプリ遷移などでフィールドが破棄された場合は、
  //      話しかけていた動作モデル = 一番近い　を表示するまで待つ

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  enum{
    SEQ_SCRIPT_TALK_MODEL_REQUEST,          // 話しかけていたモデルの表示
    SEQ_SCRIPT_TALK_MODEL_REQUEST_WAIT,     // 話しかけていたモデルの表示待ち
    SEQ_SCRIPT_RECRUITER_MODEL_REQUEST,     // リクルーターで呼んだモデルの表示
    SEQ_SCRIPT_RECRUITER_MODEL_REQUEST_WAIT,// リクルーターで呼んだモデルの表示待ち
    SEQ_SCRIPT_P2P_MODEL_REQUEST,           // 最後にP2Pで遊んだモデルの表示
    SEQ_SCRIPT_P2P_MODEL_REQUEST_WAIT,      // 最後にP2Pで遊んだモデルの表示待ち
    SEQ_END, 
  };

  switch( m_initializeSeq ){
  case SEQ_SCRIPT_TALK_MODEL_REQUEST:          // 話しかけていたモデルの表示
    {
      // 話しかけた相手のJFパーソナルデータ
      NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJoinFestaPersonalDataManager->GetScriptTalkFieldPersonalData(); 
      if( jfPersonal != NULL )
      {
        // 話しかけ相手がいた場合は表示リクエスト発行
        JFPoint* jfPoint = m_JFPointManager->GetJFPoint( jfPersonal->GetJoinFestaFriendKey() );
        JFModelRequest( jfPoint );
      }
      m_initializeSeq = SEQ_SCRIPT_TALK_MODEL_REQUEST_WAIT; 
    }
    /* break through */

  case SEQ_SCRIPT_TALK_MODEL_REQUEST_WAIT:     // 話しかけていたモデルの表示待ち
    {
      // 話しかけた相手のJFパーソナルデータ
      NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJoinFestaPersonalDataManager->GetScriptTalkFieldPersonalData(); 
      if( jfPersonal != NULL )
      {
        // エフェクト無しの出現
        if( m_JFModelManager->IsRequestEnd( JF_MODEL_SETUP_NON_EFFECT ) == false )
        {
          return false;
        }
      }
      // @fix GFNMCat[2372]:城外の場合はリクルーターとP2P人物は必要ない
      m_initializeSeq = m_isRoom ? SEQ_SCRIPT_RECRUITER_MODEL_REQUEST : SEQ_END ; 
    }
    break; // break必要

  case SEQ_SCRIPT_RECRUITER_MODEL_REQUEST:     // リクルーターで呼んだモデルの表示
    {
      // リクルーターが表示状態であれば
      if( pJoinFestaPersonalDataManager->IsRecruiterVisible() )
      {
        // リクルーターのJFパーソナルデータ
        NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = pJoinFestaPersonalDataManager->GetRecruiterPersonal(); 
        const JoinFestaFriendKey jfFriendKey = jfPersonal.GetJoinFestaFriendKey();

        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( jfFriendKey ) )
        {
          // 話しかけ相手がいた場合は表示リクエスト発行
          JFPoint* jfPoint = m_JFPointManager->GetJFPoint( jfFriendKey );
          JFModelRequest( jfPoint );
        }
      }
      m_initializeSeq = SEQ_SCRIPT_RECRUITER_MODEL_REQUEST_WAIT; 
    }
    /* break through */

  case SEQ_SCRIPT_RECRUITER_MODEL_REQUEST_WAIT:// リクルーターで呼んだモデルの表示待ち
    {
      // リクルーターが表示状態であれば
      if( pJoinFestaPersonalDataManager->IsRecruiterVisible() )
      {
        // リクルーターのJFパーソナルデータ
        NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = pJoinFestaPersonalDataManager->GetRecruiterPersonal(); 
        const JoinFestaFriendKey jfFriendKey = jfPersonal.GetJoinFestaFriendKey();

        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( jfFriendKey ) )
        {
          // エフェクト無しの出現
          if( m_JFModelManager->IsRequestEnd( JF_MODEL_SETUP_NON_EFFECT ) == false )
          {
            return false;
          }
        }
      }
      m_initializeSeq = SEQ_SCRIPT_P2P_MODEL_REQUEST; 
    }
    /* break through */

  case SEQ_SCRIPT_P2P_MODEL_REQUEST:           // 最後にP2Pで遊んだモデルの表示
    {
      // @fix MMCat[72]:レインボーロケット団のイベント中はP2P人物表示しない 
      if( m_isRREvent == false )
      {
        // P2PのJFパーソナルデータ
        NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = pJoinFestaPersonalDataManager->GetLastP2pPersonal();
        const JoinFestaFriendKey jfFriendKey = jfPersonal.GetJoinFestaFriendKey();

        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( jfFriendKey ) )
        {
          // 話しかけ相手がいた場合は表示リクエスト発行
          JFPoint* jfPoint = m_JFPointManager->GetJFPoint( jfFriendKey );
          JFModelRequest( jfPoint );
        }
      }
      
      m_initializeSeq = SEQ_SCRIPT_P2P_MODEL_REQUEST_WAIT; 
    }
    /* break through */

  case SEQ_SCRIPT_P2P_MODEL_REQUEST_WAIT:      // 最後にP2Pで遊んだモデルの表示待ち
    {
      // P2PのJFパーソナルデータ
      NetAppLib::JoinFesta::JoinFestaPersonalData& jfPersonal = pJoinFestaPersonalDataManager->GetLastP2pPersonal(); 
      const JoinFestaFriendKey jfFriendKey = jfPersonal.GetJoinFestaFriendKey();

      if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( jfFriendKey ) )
      {
        // エフェクト無しの出現
        if( m_JFModelManager->IsRequestEnd( JF_MODEL_SETUP_NON_EFFECT ) == false )
        {
          return false;
        }
      }
      m_initializeSeq = SEQ_END; 
    }
    /* break through */

  case SEQ_END: 
    {
      m_isInitialize = true;
      m_state = STATE_COMMON;

      // ※チュートリアル中は、動的に出さないようにする
      if( m_isTutorial )
      {
        DynamickLoadDisable();
      }

      // アトラクション中の始めは出さないようにする。アナウンスが有るため。
      // 解除はスクリプトで行う
      if( GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->IsPlaying() )
      {
        DynamickLoadDisable();
      }
    }
    return true;
  }
  

  return false;
}

//------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::Update()
{
  // 未初期化なら何もしない
  if( m_isInitialize == false )
  {
    return;
  }

#ifdef JF_DEBUG_MENU
  // デバッグウィンドウが開いている時は何もしない
  if( gfl2::debug::DebugWin_IsOpen() )
  {
    return;
  }
#endif // JF_DEBUG_MENU

  // 動的出現システム無効化フラグがONならば何もしない
  if( m_isDynamicLoadDisable == true )
  {
    return;
  }


  // 状態別更新
  switch( m_state ){
  case STATE_NONE:        /*何もしない*/ break;
  case STATE_COMMON:      State_Common(); break;
  case STATE_DELETING:    State_Deleting(); break;
  case STATE_DISPLAYING:  State_Displaying(); break;
  }
}

//------------------------------------------------------------------
/**
 * @brief   トラバース直前の更新（処理負荷対策用）
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::TraverseBefore()
{
  // 未初期化なら何もしない
  if( m_isInitialize == false )
  {
    return;
  }

  // JF人物に話しかけた場合は何もしない
  {
    // 話しかけた相手のJFパーソナルデータ
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = pJoinFestaPersonalDataManager->GetScriptTalkFieldPersonalData(); 
    if( jfPersonal != NULL )
    {
      return;
    }
  }


  // 処理負荷対策用
  if( m_JFModelManager != NULL )
  {
    m_JFModelManager->TraverseBefore();
  }
}

//------------------------------------------------------------------
/**
 * @brief   破棄
 * @return  RESULT  関数の結果
 */
//------------------------------------------------------------------
NetApp::JoinFesta::RESULT JFDynamicLoadSystem::Terminate()
{
  ICHI_PRINT("JFDynamicLoadSystem::Terminate\n");

  // 未初期化なら何もしない
  if( m_isInitialize == false )
  {
    return RESULT_FINISH;
  }

  // Updateは何もしないようにする
  m_state = STATE_NONE;

  // モデル破棄完了待ち
  if( m_JFModelManager != NULL && m_JFModelManager->Terminate() == false )
  {
    return RESULT_CONTINUE;
  }

  GFL_SAFE_DELETE( m_JFPointManager );
  GFL_SAFE_DELETE( m_JFModelManager );

  m_isInitialize = false;

  return RESULT_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief   動的に主人公の周りに動作モデルを出現させる仕組みを無効化する
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::DynamickLoadDisable()
{
  GFL_PRINT("JFDynamicLoadSystem::DynamickLoadDisable\n");
  GFL_ASSERT(m_isInitialize);
  m_isDynamicLoadDisable = true;
}

//------------------------------------------------------------------
/**
 * @brief   動的に主人公の周りに動作モデルを出現させる仕組みを有効化する
 * @note    DynamickLoadDisableの解除用
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::DynamickLoadEnable()
{
  GFL_PRINT("JFDynamicLoadSystem::DynamickLoadEnable\n");
  GFL_ASSERT(m_isInitialize);

  // 城内は有効化出来ない
  if( m_isRoom == true )
  {
    return;
  }
  m_isDynamicLoadDisable = false;
}

//------------------------------------------------------------------
/**
 * @brief   指定したパーソナルデータの出現リクエストを出す
 *
 * @note 上記のDynamickLoadDisable中に出現させる用 
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::DynamickLoadOneRequest( const u32 idx )
{
  // 通常時には使用できない
  if( m_isDynamicLoadDisable == false )
  {
    GFL_ASSERT(0);
    return;
  }

  // 表示リクエスト発行
  JFPoint* jfPoint = m_JFPointManager->GetJFPoint( idx );
  JFModelRequest( jfPoint );

  // 一時的に無効にする
  m_isDynamicLoadDisable = false;
}
void JFDynamicLoadSystem::DynamickLoadOneRequest( const NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonalData )
{
  // 通常時には使用できない
  if( m_isDynamicLoadDisable == false )
  {
    GFL_ASSERT(0);
    return;
  }

  if( pPersonalData != NULL )
  {
    // 表示リクエスト発行
    JFPoint* jfPoint = m_JFPointManager->GetJFPoint( pPersonalData->GetJoinFestaFriendKey() );
    JFModelRequest( jfPoint );

    // 一時的に無効にする
    m_isDynamicLoadDisable = false;
  }
}

//------------------------------------------------------------------
/**
 * @brief   指定したパーソナルデータの出現リクエストが終了したか
 * @retval  trueで終了した
 */
//------------------------------------------------------------------
bool JFDynamicLoadSystem::IsDynamickLoadOneRequestEnd()
{
  // モデル表示されたら通常状態に戻る
  if( m_JFModelManager->IsRequestEnd( JF_MODEL_SETUP_NORMAL ) )
  {
    // 無効に戻す
    m_isDynamicLoadDisable = true;
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief   指定したパーソナルデータの削除リクエストを出す
 *
 * @note DynamickLoadDisable中用 
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::DynamickUnloadOneRequest( const JoinFestaFriendKey& friendKey )
{
  // 通常時には使用できない
  if( m_isDynamicLoadDisable == false )
  {
    GFL_ASSERT(0);
    return;
  }

  // 削除リクエスト発行
  JFPoint* jfPoint = m_JFPointManager->GetJFPoint( friendKey );
  JFModelDelete( jfPoint );
  // 一時的に無効にする
  m_isDynamicLoadDisable = false;
}

//------------------------------------------------------------------
/**
 * @brief   指定したパーソナルデータの削除リクエストが終了したか
 * @retval  trueで終了した
 */
//------------------------------------------------------------------
bool JFDynamicLoadSystem::IsDynamickUnloadOneRequestEnd()
{
  // 削除待ち
  if( m_JFModelManager->IsDeleteEnd() )
  {
    // 無効に戻す
    m_isDynamicLoadDisable = true;
    return true;
  }
  return false;
}


#if PM_DEBUG
// デバッグ用：削除リクエスト
bool JFDynamicLoadSystem::DEBUG_UnloadRequest( const u32 idx )
{
  // 通常時のみ動作する
  if( m_state != STATE_COMMON )
  {
    return false;
  }

  // 表示リクエスト発行
  JFPoint* jfPoint = m_JFPointManager->GetJFPoint( idx );
  JFModelDelete( jfPoint );
  return true;
}
#endif
  


//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   状態別更新関数：通常時
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::State_Common()
{
  // ①自機周りのポイントをチェックしてモデルが表示されていないポイントがあるか？
  //   ある→②
  //   ない→何もしない
  // ②モデルの最大表示数表示しているか
  //   ※表示しているモデルが全て表示範囲内に入っている場合は、モデル表示してはいけない
  //   している→③
  //   していない→④
  // ③モデル表示している中で一番遠いポイントを取得
  //   そのポイントでモデル非表示リクエスト発行
  //   状態を「モデル非表示中」に変更
  //    →非表示完了で④
  // ④進行方向のポイントでモデル表示リクエスト発行
  //   状態を「モデル表示中」に変更
  //   →表示完了で①


  // ①
  JFPoint* pSearchPoint = GetNearJFPoint();
  {
    // 表示するモデルポイントを検索
    if( pSearchPoint == NULL )
    { // 何もしない
      return;
    }
    //ICHI_PRINT("表示されていないポイント見つかった\n");
  }
  // ②
  {
    // プレイヤー位置
    const gfl2::math::Vector3 playerPosition = m_pFieldmap->GetPlayerPosition();
    if( m_JFPointManager->GetModelViewCount() == JoinFestaDefine::JFMODEL_DISP_MAX )
    {
      // 表示モデルが全て範囲内であれば、新たに表示しない（ちらつくから）
      if( m_JFPointManager->CheckModelDispPointToRange( playerPosition ) == false )
      {
        return;
      }

      // ③
      JFPoint* deletePoint = m_JFPointManager->GetModelDispInFarPoint( playerPosition );
      JFModelDelete( deletePoint );
    }
    else
    {
      // ④
      JFModelRequest( pSearchPoint );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   状態別更新関数：モデル削除中
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::State_Deleting()
{
  if( m_JFModelManager->IsDeleteEnd() )
  {
    // ④
    JFModelRequest( GetNearJFPoint() );
  }
}

//------------------------------------------------------------------
/**
 * @brief   状態別更新関数：モデル表示中
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::State_Displaying()
{
  // モデル表示されたら通常状態に戻る
  if( m_JFModelManager->IsRequestEnd( JF_MODEL_SETUP_NORMAL ) )
  {
    m_state = STATE_COMMON;
    ICHI_PRINT("JFState:STATE_COMMONへ\n");
  }
}

//------------------------------------------------------------------
/**
 * @brief   JFモデル：表示リクエスト
 * @param   表示リクエストする場所
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::JFModelRequest( JFPoint* pJFPoint )
{
  if( pJFPoint == NULL || pJFPoint->IsSetPersonalData() == false )
  { // 何もしない
    m_state = STATE_COMMON;
    return;
  }

  // このポイントでモデル表示リクエスト
  m_JFModelManager->Request( pJFPoint->GetPersonalData(), pJFPoint->GetPointIndex(), pJFPoint->GetPointPosition() );

  // モデル表示ポイントとして登録
  m_JFPointManager->RegistJFModelViewList( pJFPoint );

  // ⑥
  // 表示中状態に変更
  m_state = STATE_DISPLAYING;
  ICHI_PRINT("JFState:STATE_DISPLAYINGへ\n");
}

//------------------------------------------------------------------
/**
 * @brief   JFモデル：非表示リクエスト
 */
//------------------------------------------------------------------
void JFDynamicLoadSystem::JFModelDelete( JFPoint* pJFPoint )
{
  if( pJFPoint == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  // モデル削除
  m_JFModelManager->Delete( pJFPoint->GetPointIndex() );
  // モデル表示ポイント解除
  m_JFPointManager->UnregistJFModelViewList( pJFPoint );
  ICHI_PRINT("モデル削除\n");

  // 削除中状態に変更
  m_state = STATE_DELETING;
  ICHI_PRINT("JFState:STATE_DELETING\n");
}


//------------------------------------------------------------------
/**
 * @brief   プレイヤーに最も近いJFポイントを取得
 */
//------------------------------------------------------------------
JFPoint* JFDynamicLoadSystem::GetNearJFPoint()
{
  // プレイヤー位置
  gfl2::math::Vector3 playerPosition = m_pFieldmap->GetPlayerPosition();


  // D要望：動いている時に前に表示して欲しい
  // プレイヤーが動いているか
  if( m_pFieldmap->GetPlayerCharacter() != NULL && m_pFieldmap->GetPlayerCharacter()->IsMoveState() )
  {
    // 自身の進行方向を取得
    gfl2::math::Vector3 vecZ;
    if( m_pFieldmap->GetPlayerCharacter()->IsRunLoopState() )
    {
      vecZ.SetZ( JFMODEL_RUN_SEARCH_POINT_Z );
    }
    else
    {
      vecZ.SetZ( JFMODEL_WALK_SEARCH_POINT_Z );
    }
    // 外側ばかり出るので内側に寄せる
    vecZ.SetX( JFMODEL_SEARCH_POINT_X );
    playerPosition += m_pFieldmap->GetPlayerQuaternion().Transform( vecZ );
  }

  return m_JFPointManager->GetModelNonDispOfNearPoint( playerPosition );
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

