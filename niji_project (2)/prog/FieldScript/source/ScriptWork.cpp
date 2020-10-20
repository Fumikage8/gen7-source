//=============================================================================
/**
 *
 *  @file   ScriptWork.cpp
 *  @brief  スクリプト用ワーク
 *  @author Miyuki Iwasawa 
 *  @data   2015.02.28
 *
 */
//=============================================================================

#include <util/include/gfl2_std_string.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "System/include/GameStrLen.h"
#include "FieldScript/include/ScriptWork.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

//#include "../subscreen/fieldmenu.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

#include "prog/Savedata/include/EventWorkDef.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )


//------------------------------------------------------------------------------
/**
 * @func        Entry
 * @brief       Entry関数.
 * @author      N.Takeda
 * @date        2016/03/07(月) 17:41:58
 *
 * @param[in] int anime_id    アニメID.
 * @param[in] int frame_start 開始フレーム.
 * @param[in] int loop_max    ループ回数.
 * @param[in] int blend       ブレンド値.
 */
//------------------------------------------------------------------------------
void  MotionCommandParam::Entry( int anime_id, int frame_start, int loop_max, int blend )
{
  GFL_ASSERT( m_Num < MoveModel::ActionCommandContinuousPlayback::REGIST_MAX_CNT );
  if( m_Num < MoveModel::ActionCommandContinuousPlayback::REGIST_MAX_CNT )
  {
    m_AnimationIDArray[ m_Num ] = anime_id;
    m_FrameStartArray[ m_Num ] = frame_start;
    m_LoopMaxArray[ m_Num ] = loop_max;
    m_BlendArray[ m_Num ] = blend;
    m_Num++;
  }
}


//------------------------------------------------------------------------------
/**
 * @func        Clear
 * @brief       クリア.
 * @author      N.Takeda
 * @date        2016/03/07(月) 17:41:56
 */
//------------------------------------------------------------------------------
void  MotionCommandParam::Clear()
{
  for( u32 ii=0 ; ii<Field::MoveModel::ActionCommandContinuousPlayback::REGIST_MAX_CNT ; ii++ )
  {
    m_AnimationIDArray[ii] = Field::MoveModel::ActionCommandContinuousPlayback::INVALID_ANIMATION_ID;
    m_FrameStartArray[ii] = m_LoopMaxArray[ii] = m_BlendArray[ii] = 0;
  }
  m_Num = 0;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 *
 *  @param  GameSys::GameEvent* event イベントが起動された元のイベント
 */
//-----------------------------------------------------------------------------
Work::Work( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap, u16 acmdlist_max ) :
  m_MagicNo( MAGIC_NO ),
  m_Seq(0),
  m_pGameManager( p_gman ),
  m_pBaseEvent( NULL ),
  m_pTalkModel( NULL ),
  m_TargetActorID( TARGET_ACTOR_ID_NONE ),
  m_TargetActorType( FIELD_ACTOR_TYPE_MAX ),
  m_pInputFreeWord( NULL ),
  m_bEventPosEnable( false ),
  m_bFadeFlag( false ),
  m_bPlaceNameNotCloseFlag( false )
{
  // テンポラリワークの初期値設定
  SetTempWorkValue( SCWK_TALK_START_OBJID, FIELD_EVENTID_NULL );
  // ExtraMsg初期化
  ClearExtraMsgInstant();

  gfl2::std::MemClear( m_ParentGlobalParams, sizeof( cell )*GLOBAL_PARAM_NUM );
  gfl2::std::MemClear( m_ChildGlobalParams, sizeof( cell )*GLOBAL_PARAM_NUM );
  gfl2::std::MemClear( m_ScrTempWork, sizeof( u32 )*TEMP_WORK_SIZE );
  gfl2::std::MemClear( m_TrainerHitData, sizeof( TRAINER_HITDATA )*TRAINER_EYE_HITMAX );
  m_EventPos.Set(0,0,0);

#if PM_DEBUG
  gfl2::std::MemClear( m_DebugChrEditList, sizeof( DebugChrEditList )*DEBUG_CHR_EDIT_LIST_MAX );
  m_DebugChrEditListNum = 0;
#endif
}

//-----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
Work::~Work( void )
{
  DeleteFreeWord();
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 話しかけ対象のモデルを設定
 *
 * @param p_mdl 動作モデル
 */
/* ------------------------------------------------------------------------- */
void Work::SetTargetModel( MoveModel::FieldMoveModel* p_mdl )
{
  m_pTalkModel = p_mdl;
  if( p_mdl )
  {
    SetTempWorkValue( SCWK_TARGET_OBJID, p_mdl->GetEventId() );
  }else{
    SetTempWorkValue( SCWK_TARGET_OBJID, static_cast<u32>(TARGET_OBJID_NONE) );
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 起動ターゲットアクターを設定
 *
 * @param p_mdl 動作モデル
 */
/* ------------------------------------------------------------------------- */
void Work::SetTargetActor( u32 eventID, FieldActorType type )
{
  m_TargetActorID = eventID;
  m_TargetActorType = type;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  ワークを取得(イベントワーク/一時記憶ワーク両対応)
 *
 *  @param  u16 work_no  ワークナンバー
 *
 *  @retval ワークのアドレス
 */
//-----------------------------------------------------------------------------
u32 Work::GetWorkValue( u16 work_no )
{
  if( work_no < SVWK_START )
  {
    GFL_ASSERT(0);
    return 0;
  }
  if( work_no < SCWK_START )
  {
    return GetEventWorkValue( work_no );
  }
  if( work_no < SCWK_END )
  {
    return GetTempWorkValue( work_no );
  }
  
  GFL_ASSERT(0);
  return 0;
}
//-----------------------------------------------------------------------------
/**
 *  @brief  ワークをセット(イベントワーク/一時記憶ワーク両対応)
 *
 *  @param  u16 work_no  ワークナンバー
 *  @param  value
 */
//-----------------------------------------------------------------------------
bool Work::SetWorkValue( u16 work_no, u32 value )
{
  if( work_no < SVWK_START )
  {
    GFL_ASSERT(0);
    return false;
  }
  if( work_no < SCWK_START )
  {
    return SetEventWorkValue( work_no, value );
  }
  if( work_no < SCWK_END )
  {
    return SetTempWorkValue( work_no, value );
  }
  
  GFL_ASSERT(0);
  return false;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  イベントワークを取得
 *
 *  @param  u16 work_no  ワークナンバー
 *
 *  @retval ワークのアドレス
 *
 *  @li work_no < 0x8000  通常のセーブワーク
 *  @li work_no >= 0x8000 スクリプト制御ワークの中に確保しているワーク
 */
//-----------------------------------------------------------------------------
u16* Work::GetEventWork( u16 work_no )
{
  EventWork* ev = m_pGameManager->GetGameData()->GetEventWork();
  
  if( work_no < SVWK_START )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  
  if( work_no < SCWK_START )
  {
    checkBanWorkID(work_no);
    return ev->GetEventWorkAdrs( work_no );
  }
  
  GFL_ASSERT(0);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントワークの値を取得
 *
 *  @param  u16 work_no  ワークナンバー
 *
 *  @retval ワークの値
 */
//-----------------------------------------------------------------------------
u16 Work::GetEventWorkValue( u16 work_no )
{
  u16 *res = GetEventWork( work_no );
  if( res == NULL ){ GFL_ASSERT(0); return work_no; }
  return *res;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントワークの値をセット
 *
 *  @param  u16 work_no  ワークナンバー
 *  @param  value  セットする値
 *
 *  @retval true:セットできた, false:できなかった
 */
//-----------------------------------------------------------------------------
bool Work::SetEventWorkValue( u16 work_no, u32 value )
{
  if( work_no < SCWK_START ){
    checkBanWorkID(work_no);

    u16 *res = GetEventWork( work_no );
    if( res == NULL ){ return false; }
    if( *res == static_cast<u16>(value) ){ return false; }

    *res = static_cast<u16>(value);
  }else{
    GFL_ASSERT(0);
    return false;
  }

  if( m_pGameManager->GetFieldmap() != NULL )
  {
    m_pGameManager->GetFieldResident()->GetPlacementDataManager()->RegistZonePos( work_no );
    m_pGameManager->GetFieldResident()->GetPlacementDataManager()->UnRegistZonePos( work_no );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントフラグを取得
 *
 *  @param  u16 flag_no  フラグナンバー
 *  
 *  @retval フラグ値
 */
//-----------------------------------------------------------------------------
bool Work::GetEventFlag( u16 flag_no ) const
{
  EventWork* ev = m_pGameManager->GetGameData()->GetEventWork();
  return ev->CheckEventFlag( flag_no );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  イベントフラグを設定
 *
 *  @param  u16 flag_no  フラグナンバー
 *  @param  val フラグ値
 */
//-----------------------------------------------------------------------------
void Work::SetEventFlag( u16 flag_no, bool val )
{
  EventWork* ev = m_pGameManager->GetGameData()->GetEventWork();

  if( ev->CheckEventFlag( flag_no ) == val ){
    return;
  }
  if( val )
  {
    ev->SetEventFlag( flag_no );
  }
  else 
  {
    ev->ResetEventFlag( flag_no );
  }

  if( m_pGameManager->GetFieldmap() != NULL )
  {
    m_pGameManager->GetFieldResident()->GetPlacementDataManager()->RegistZonePos( flag_no );
    m_pGameManager->GetFieldResident()->GetPlacementDataManager()->UnRegistZonePos( flag_no );
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトで操作を禁止するworkIdでないかチェック
 */
/* ------------------------------------------------------------------------- */
void Work::checkBanWorkID( u16 work_no )const
{
  switch(work_no)
  {
  case WK_SYS_ALOLA_TIME_TYPE:
    GFL_ASSERT_MSG(0, "スクリプトで指定を禁止しているWORKIDです。no[%d]\n", work_no);
    break;
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief  一時記憶ワークを取得
 *
 *  @param  u16 work_no 一時ワークNO
 *
 *  @return ワークへのポインタ
 */
//-----------------------------------------------------------------------------
u32* Work::GetTempWork( u16 work_no )
{
  if( work_no < TEMP_WORK_START || work_no >= TEMP_WORK_END ){
    GFL_ASSERT(0);
    return NULL;
  }
  return &( m_ScrTempWork[ work_no - TEMP_WORK_START ] );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  一時記憶ワークの値を取得
 *
 *  @param  u16 work_no 一時ワークNO
 *
 *  @return ワークの値
 */
//-----------------------------------------------------------------------------
u32 Work::GetTempWorkValue( u16 work_no )
{
  u32* p_temp = GetTempWork( work_no );
  if( p_temp == NULL ) return 0;

  //話しかけターゲットが存在しないのにワークを参照しようとした場合のセキュアコード
  if( work_no == SCWK_TARGET_OBJID ){
    if( *p_temp == TARGET_OBJID_NONE ){
      SCRIPT_ASSERT_MSG( 0, "話しかけターゲットが不明です\n");
      return 0; //EventIDの無効値を返す
    }
  }
  return *p_temp;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  一時記憶ワークの値をセット
 *
 *  @param  u16 work_no 一時ワークNO
 *  @param  u32 value ワークにセットする値
 */
//-----------------------------------------------------------------------------
bool Work::SetTempWorkValue( u16 work_no, u32 value )
{
  u32* p_temp = GetTempWork( work_no );
  if( p_temp == NULL ) return false;

  *p_temp = value;
  return true;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  話しかけターゲット（SCWK_TARGET_OBJID）の存在チェック
 *
 *  @retval true：話しかけターゲットが存在しない
 *  @retval false：話しかけターゲットが存在する
 */
//-----------------------------------------------------------------------------
bool Work::IsTargetObjidNone()
{
  u32* p_temp = GetTempWork( SCWK_TARGET_OBJID );
  if( p_temp == NULL ) return true;

  if( *p_temp == TARGET_OBJID_NONE )
  {
    //話しかけターゲットが存在しない
    return true;
  }
  return false;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief パラメータを設定
 *
 * @param p0 パラメータ0
 * @param p1 パラメータ1
 * @param p2 パラメータ2
 * @param p3 パラメータ3
 */
/* ------------------------------------------------------------------------- */
void Work::SetParameters( u32 p0, u32 p1, u32 p2, u32 p3 )
{
  SetTempWorkValue( SCWK_PARAM0, p0 );
  SetTempWorkValue( SCWK_PARAM1, p1 );
  SetTempWorkValue( SCWK_PARAM2, p2 );
  SetTempWorkValue( SCWK_PARAM3, p3 );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief BGIDを設定
 *
 * @param bgid BGID
 */
/* ------------------------------------------------------------------------- */
void Work::SetBGID( u32 bgid )
{
  SetTempWorkValue( SCWK_TARGET_BGID, bgid );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief POSIDを設定
 *
 * @param posid POSID
 */
/* ------------------------------------------------------------------------- */
void Work::SetPosID( u32 posid )
{
  SetTempWorkValue( SCWK_TARGET_POSID, posid );
}
  
/* ------------------------------------------------------------------------- */
/**
 * @brief POSIDを取得
 *
 * @return POSID
 */
/* ------------------------------------------------------------------------- */
u32 Work::GetPosID( void )
{
  return GetTempWorkValue( SCWK_TARGET_POSID ); 
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 子グローバルパラメータを取得
 *
 * @param index インデックス
 *
 * @return 子グローバルパラメータ
 *
 * @note グローバルコール時に子に渡すパラメータ
 */
/* ------------------------------------------------------------------------- */
cell Work::GetChildGlobalParam( u32 index )
{
  GFL_ASSERT( index < GLOBAL_PARAM_NUM );
  return m_ChildGlobalParams[ index ];
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 子グローバルパラメータを設定
 *
 * @param index インデックス
 * @param value 値
 *
 * @note グローバルコール時に子に渡すパラメータ
 */
/* ------------------------------------------------------------------------- */
void Work::SetChildGlobalParam( u32 index, cell value )
{
  GFL_ASSERT( index < GLOBAL_PARAM_NUM );
  m_ChildGlobalParams[ index ] = value;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 親グローバルパラメータを取得
 *
 * @param index インデックス
 *
 * @return 親グローバルパラメータ
 *
 * @note グローバルコール時に親に渡すパラメータ
 */
/* ------------------------------------------------------------------------- */
cell Work::GetParentGlobalParam( u32 index )
{
  GFL_ASSERT( index < GLOBAL_PARAM_NUM );
  return m_ParentGlobalParams[ index ];
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 親グローバルパラメータを設定
 *
 * @param index インデックス
 * @param value 値
 
 * @note グローバルコール時に親に渡すパラメータ
 */
/* ------------------------------------------------------------------------- */
void Work::SetParentGlobalParam( u32 index, cell value )
{
  GFL_ASSERT( index < GLOBAL_PARAM_NUM );
  m_ParentGlobalParams[ index ] = value;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 親のワークの子グローバルパラメータに親グローバルパラメータをコピー
 *
 * @param parent 親のワーク
 */
/* ------------------------------------------------------------------------- */
void Work::CopyGlobalParamsToParent( Work * parent )
{
  for (int i = 0; i < GLOBAL_PARAM_NUM; i ++ )
  {
    parent->m_ChildGlobalParams[i] = this->m_ParentGlobalParams[i];
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 子ワークの親グローバルパラメータに子グローバルパラメータをコピー
 *
 * @param child 子のワーク
 */
/* ------------------------------------------------------------------------- */
void Work::CopyGlobalParamsToChild( Work * child )
{
  for (int i = 0; i < GLOBAL_PARAM_NUM; i ++ )
  {
    child->m_ParentGlobalParams[i] = this->m_ChildGlobalParams[i];
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief フリーワード生成
 */
/* ------------------------------------------------------------------------- */
void Work::CreateFreeWord( void )
{
  if (m_pInputFreeWord)
  {
    DeleteFreeWord();
    GFL_ASSERT(0);
  }
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  m_pInputFreeWord = GFL_NEW(p_heap) gfl2::str::StrBuf(System::STRLEN_FREEWORD_MAX + System::EOM_LEN, p_heap);
}

/* ------------------------------------------------------------------------- */
/**
 * @brief フリーワード破棄
 */
/* ------------------------------------------------------------------------- */
void Work::DeleteFreeWord( void )
{
  GFL_SAFE_DELETE( m_pInputFreeWord );
}

//==================================================================================
// ChrEditList
//==================================================================================
#if PM_DEBUG
/* ------------------------------------------------------------------------- */
/**
 * @brief キャラリスト登録
 */
/* ------------------------------------------------------------------------- */
int Work::DebugChrEditListEntry( const char* p_name, u32 event_id, int dynamic_mot_id )
{
  if( m_DebugChrEditListNum >= DEBUG_CHR_EDIT_LIST_MAX ){
    GFL_ASSERT_MSG( 0, "ChrEditListNumOver %d\n", m_DebugChrEditListNum );
    return 0;
  }
  DebugChrEditList* p_list = &m_DebugChrEditList[m_DebugChrEditListNum];
  gfl2::std::MemCopy( p_name, p_list->name, sizeof(char)* DEBUG_CHR_EDIT_NAME_LEN-1 );
  p_list->event_id = event_id;
  p_list->dynamic_mot_id = dynamic_mot_id;
  
  GFL_PRINT( "ChrEditList Entry %d\n", m_DebugChrEditListNum );
  return ++m_DebugChrEditListNum;
}

#endif  //PM_DEBUG

//==================================================================================
// SystemWork
//==================================================================================

/* ------------------------------------------------------------------------- */
/**
 * @brief コンストラクタ
 *
 * @param p_gman ゲームマネージャーへのポインタ
 * @param p_heap ヒープ
 */ 
/* ------------------------------------------------------------------------- */
SystemWork::SystemWork( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* p_heap )
{
  // WordSetに渡すヒープは、メッセージデータを取得するための一時バッファ用のヒープID
  // 大きなサイズを要求するメッセージもあるため、イベントヒープを割り当てる。
  gfl2::heap::HeapBase* p_event_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
  m_pWordSet = GFL_NEW( p_heap ) print::WordSet( BUF_ID_MAX, print::WordSet::DEFAULT_BUF_LEN, p_event_heap );

#if PM_DEBUG
  m_EndCheckID = -1;

  //デバッグの時は見た目で分かりやすい数字文字列でクリアしておく
  for( int i = 0;i < BUF_ID_MAX; i++ ){
    ;//m_pWordSet->RegisterNumber( i, 4936, 4, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  }  
#endif

  SystemSingletonesInit( &m_Singletones );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief デストラクタ
 */
/* ------------------------------------------------------------------------- */
SystemWork::~SystemWork()
{
  GFL_SAFE_DELETE( m_pWordSet );
}


/* ------------------------------------------------------------------------- */
/**
 * @brief チェックビットを立てる
 *
 * @param bit チェックビット
 */
/* ------------------------------------------------------------------------- */
void SystemWork::OnCheckBit( SCR_END_CHECK_BIT bit )
{
  if( CheckBit( bit ) )
  {
    GFL_PRINT("%d::already bit on \n", bit );
    return;
  }

  m_CheckBit |= bit; 
}

/* ------------------------------------------------------------------------- */
/**
 * @brief チェックビットを折る
 *
 * @param bit チェックビット
 */
/* ------------------------------------------------------------------------- */
void SystemWork::OffCheckBit( SCR_END_CHECK_BIT bit )
{ 
  if( !CheckBit( bit ) )
  {
    GFL_PRINT("%d::already bit off \n", bit );
    return;
  }
  
  m_CheckBit &= ~bit; 
}

/* ------------------------------------------------------------------------- */
/**
 * @brief チェックビットが立っているかチェック
 *
 * @param bit チェックビット
 *
 * @return true:立っている
 */
/* ------------------------------------------------------------------------- */
bool SystemWork::CheckBit( SCR_END_CHECK_BIT bit ) const
{
  return ( ( bit & m_CheckBit ) == static_cast<u32>(bit) );
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

