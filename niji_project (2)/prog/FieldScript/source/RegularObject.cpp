

#include "FieldScript/include/RegularObject.h"
#include "FieldScript/include/SceneChangeControl.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptBufferManager.h"

#include "System/include/HeapDefine.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldScript )

//-----------------------------------------------------------------------------
/**
 * @brief ゾーン／共通スクリプト用Object コンストラクタ
 *
 * @param p_heap ヒープ
 * @param p_system システムへのポインタ
 * @param p_heap_debug デバッグ用ヒープ
 */
//-----------------------------------------------------------------------------
RegularObject::RegularObject( FieldScriptSystem* p_system, gfl2::heap::HeapBase* p_heap, gfl2::heap::HeapBase* p_heap_debug ) :
  ScriptObject( p_heap, p_heap_debug ),
  m_IsLoaded(false),
  m_IsFirstUpdated(false),
  m_RegID(0),
  m_pSystem(p_system),
  m_WaitFunc(NULL),
  m_ParallelObjectList( p_heap ),
  m_ParallelHandle( -1 ),
  m_ParallelHandleBit( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
RegularObject::~RegularObject( void )
{ 
}

//-----------------------------------------------------------------------------
/**
 * @brief レジスト用のデータを設定（Loadの事前に設定する）
 *
 * @param zone_id ゾーンID
 * @param amx_id  AMX ID
 */
//-----------------------------------------------------------------------------
void RegularObject::SetRegistData( ZoneID zone_id, u32 amx_id )
{
  m_RegID = ScriptObject::GetRegistID( zone_id, amx_id );
}

//-----------------------------------------------------------------------------
/**
 * @brief ロード関数
 *
 * @param buffer  ロードする生データ
 * @param size    ロードするサイズ
 * @param amxname ロードするAMXの名前
 *
 * @note  pawn内でPC転送の際に呼ばれる関数。FieldScriptSystem内ではLoadNormal/LoadShareを使う。
 */
//-----------------------------------------------------------------------------
void RegularObject::Load( const void* buffer, const size_t size, const char* amxname )
{
#if PM_DEBUG
  if( this->IsDebug( buffer ) )
  {
    // @note 容量が余っているDEVICEを使う
    gfl2::heap::HeapBase* p_debug = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER)->GetLowerHandle();
    SetHeap( p_debug ); // デバッグヒープを設定
  }
  else
  {
    SetHeap( m_pHeap ); // 通常ヒープを設定
  }
#endif // PM_DEBUG



  // ロード
  ScriptObject::Load( buffer, size, amxname );

  // 命令セットをレジスト
  RegistFunc( m_RegID );
  m_IsLoaded = true;
  m_IsFirstUpdated = false;
  //↓PCから転送した時に表示する
  GFL_PRINT("FieldScript loaded:%s\n", amxname );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 通常ロード
 *
 * @param buffer AMXバッファ
 * @param size AMXバッファサイズ
 * @param amxname AMX名
 */
/* ------------------------------------------------------------------------- */
void RegularObject::LoadNormal( const void* buffer, const size_t size, const char* amxname )
{
//  GFL_DEBUG_TICK_START( SCRIPT, DEBUG_TICK_PRINT_SCRIPT_LOAD, "load" );

  // ロード
  ScriptObject::Load( buffer, size, amxname );

//  GFL_DEBUG_TICK_END( SCRIPT, DEBUG_TICK_PRINT_SCRIPT_LOAD );
//  GFL_DEBUG_TICK_START( SCRIPT, DEBUG_TICK_PRINT_SCRIPT_REGIST, "regist" );

  // 命令セットをレジスト
  RegistFunc( m_RegID );
  m_IsLoaded = true;
  m_IsFirstUpdated = false;
//  GFL_DEBUG_TICK_END( SCRIPT, DEBUG_TICK_PRINT_SCRIPT_REGIST );
}

/* ------------------------------------------------------------------------- */
/**
* @brief スクリプトが完了しているか？
*
* @return true:している, false:していない
*/
/* ------------------------------------------------------------------------- */
bool RegularObject::IsFinished( void )
{
  // 子が生きている
  if( m_ParallelObjectList.Size() > 0 )
  {
    return false;
  }

  return ScriptObject::IsFinished();
}


/* ------------------------------------------------------------------------- */
/**
 * @brief 既にあるバッファからインスタンスを生成
 *
 * @param p_org オリジナルインスタンスへのポインタ
 */
/* ------------------------------------------------------------------------- */
void RegularObject::LoadShare( const gfl2::pawn::PawnBase* p_org )
{
  ScriptObject::LoadShare( p_org );
  // 命令セットをレジスト
  RegistFunc( m_RegID );
  m_IsLoaded = true;
  m_IsFirstUpdated = false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 主処理
 *
 * @return pawn主処理の戻り値
 */
/* ------------------------------------------------------------------------- */
s32 RegularObject::Update( void )
{
  RegularObjectListIterator it = m_ParallelObjectList.Begin();
  RegularObjectListIterator end = m_ParallelObjectList.End();
  while( it != end )
  {
    RegularObject* targetObj = *it;

    if( !(*it)->IsLoaded() )
    {
      // @note momiji運用では同一amxのみ使用可能にしているため、ここにくることは無い
      Work * work = (*it)->GetWork();
      ScriptInfo * scrInfo = work->GetScriptInfo();
      
      //利用できない→ロードされていない
      if ( m_pSystem->GetScriptBufferManager()->IsReadFinished( scrInfo->m_AmxId ) == false )
      { 
        // 共通スクリプトのロード中はスクリプトを走らせない
        
        // @fix GFCHECK[2920]:イテレータ中に削除処理の懸念点、対処。
        it++;
        continue;
        // return ret;
      }

      void * buffer =  m_pSystem->GetScriptBufferManager()->GetBuffer( scrInfo->m_AmxId );
      size_t size   =  m_pSystem->GetScriptBufferManager()->GetBufferSize( scrInfo->m_AmxId );

      targetObj->SetRegistData( scrInfo->m_RegFuncId );
      targetObj->LoadNormal( buffer, size, scrInfo->m_AmxFileName );
      targetObj->SetGlobalParameterInteger( "g_mode", scrInfo->m_ScriptId );
      m_pSystem->GetScriptBufferManager()->ReleaseBuffer( scrInfo->m_AmxId );
    }

    (*it)->Update();

    if( targetObj->IsFinished() )
    {
      SetParallelHandleUsageState( targetObj->m_ParallelHandle, false );
      targetObj->Finalize();
      it = m_ParallelObjectList.Erase( it );
      GFL_SAFE_DELETE( targetObj );
    }
    else
    {
      // @fix GFCHECK[2920]:イテレータ中に削除処理の懸念点、対処。
      it++;
    }
  }
    

  // ウェイト関数がある場合はウェイト
  if( m_WaitFunc )
  {
    if( m_WaitFunc( this->m_pSystem, m_WaitLabel, m_pWaitWork ) )
    {
      // スクリプト再開
      this->Resume();
      m_WaitFunc = NULL;
    }
    else
    {
      return gfl2::pawn::PawnBase::GFL_PAWN_SUSPENDING;
    }
  }


  
  if( m_IsFirstUpdated == false || ScriptObject::IsFinished() == false )
  {
    m_IsFirstUpdated = true;
    return ScriptObject::Update();
  }
  else
  {
    return GFL_PAWN_EXECUTE;
  }
}
/* ------------------------------------------------------------------------- */
/**
 * @brief ウェイト関数を設定
 *
 * @param func ウェイト関数へのポインタ
 * @param label ユーザーラベル
 * @param p_work ユーザーワーク
 */
/* ------------------------------------------------------------------------- */
void RegularObject::SetWaitFunc( WAIT_FUNC func, u32 label, void* p_work ) 
{
  m_WaitFunc  = func; 
  m_WaitLabel = label;
  m_pWaitWork = p_work;
  this->Halt(); // 中断
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 終了処理
 */
/* ------------------------------------------------------------------------- */
void RegularObject::Finalize( void )
{
  RegularObjectListIterator it = m_ParallelObjectList.Begin();
  RegularObjectListIterator end = m_ParallelObjectList.End();
  for( ; it != end; it++ )
  {
    SetParallelHandleUsageState( (*it)->m_ParallelHandle, false );
    (*it)->Finalize();
  }

  ScriptInfo * scrInfo = this->GetWork()->GetScriptInfo();
  SCRIPT_PRINT("ScriptEnd  type(%d):id(%5d):%s\n", scrInfo->m_ScriptType, scrInfo->m_ScriptId, scrInfo->m_AmxFileName );
#if GFL_PAWN_DEBUG
  this->PrintStackHeapMax();
#endif
  if ( scrInfo->m_ScriptType == SCRIPT_TYPE_ZONE )
  {
    // スクリプト：待機モードへ
    this->SetGlobalParameterInteger( "g_mode", SCRID_DEFAULT );
  }
  else 
  {
    this->ClearAmx(); // 破棄
    m_IsLoaded = false;
  }
  
  m_IsFirstUpdated = false;

  // スクリプト用内部変数をクリア
  this->DeleteWork();
}
//-----------------------------------------------------------------------------
/**
 * @brief シーン遷移テーブルのセットアップ
 * @param SceneChangeControl  シーン遷移制御用オブジェクト
 */
//-----------------------------------------------------------------------------
void RegularObject::SetSceneChangeData( SceneChangeControl * sceneControl )
{
  // シーン発動ワーククリア
  sceneControl->Clear();
  
  // シーン発動をセット
  int ret[SCENE_CHANGE_PARAM_MAX];
  int func_id = this->GetIndex("GetSceneChangeData");
  // 関数がPublic宣言されているときのみ処理
  if (func_id != -1) 
  {
    for (u32 i = 0; i < 99999; ++i)
    {
      this->PushInteger(i);
      
      int res = this->CallPawnFunction( func_id );
      int idx = i%SCENE_CHANGE_PARAM_MAX;
      
      if (idx == 0 && res == -1)
      {
        break;
      }
      
      ret[idx] = res;
      
      if (idx == (SCENE_CHANGE_PARAM_MAX-1))
      {
        SCRIPT_PRINT("SceneChangeData: %d %d %d, %d\n", ret[SCENE_CHANGE_PARAM_SCRID], ret[SCENE_CHANGE_PARAM_WORK_ID], ret[SCENE_CHANGE_PARAM_WORK_VALUE], ret[SCENE_CHANGE_PARAM_FADE_MODE] );
        sceneControl->Add( ret[SCENE_CHANGE_PARAM_SCRID], ret[SCENE_CHANGE_PARAM_WORK_ID], ret[SCENE_CHANGE_PARAM_WORK_VALUE], ret[SCENE_CHANGE_PARAM_FADE_MODE] );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
* @brief 並列実行するスクリプトオブジェクトを追加
*
* @param pObject 追加対象
* @param p_heap  管理領域用ヒープ
*
* @return 登録ハンドル( -1で失敗 )
*/
//-----------------------------------------------------------------------------
s32 RegularObject::RegistParallelObject( RegularObject* pObject, gfl2::heap::HeapBase* p_heap )
{
  s32 handle = GetUnusedParallelHandle();
  if( handle == -1 )
  {
    return -1;
  }

  m_ParallelObjectList.PushBack( pObject, p_heap );
  pObject->m_ParallelHandle = handle;
  SetParallelHandleUsageState( handle, true );

  return handle;
}

//-----------------------------------------------------------------------------
/**
* @brief 指定ハンドルの並列スクリプトが存在するか取得
*
* @param handle  指定ハンドル
* @return true 存在する
*/
//-----------------------------------------------------------------------------
b32 RegularObject::IsParallelObjectExists( s32 handle ) 
{
  // @note listクラスがconst対応してない
  RegularObjectListIterator it = m_ParallelObjectList.Begin();
  RegularObjectListIterator end = m_ParallelObjectList.End();
  for( ; it != end; it++ )
  {
    if( (*it)->m_ParallelHandle == handle )
    {
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
* @brief 現在存在する並列スクリプトの数を取得
*
* @return 並列スクリプト数
*/
//-----------------------------------------------------------------------------
u32 RegularObject::GetParallelObjectCount( void ) const
{
  return m_ParallelObjectList.Size();
}

#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
 * @brief デバッグ用シーンエディットパラメータ取得
 */
//-----------------------------------------------------------------------------
int RegularObject::DebugGetSceneEditParam( DebugSceneEditParam * param )
{
  gfl2::std::MemClear(param,sizeof(DebugSceneEditParam));

  int ret[DEBUG_SCENE_EDIT_PARAM_MAX];
  int func_id = this->GetIndex("DebugGetSceneEditParam");
  // 関数がPublic宣言されているときのみ処理
  if (func_id != -1) 
  {
    for (u32 i = 0; i < DEBUG_SCENE_EDIT_PARAM_MAX; ++i)
    {
      this->PushInteger(i);
      
      int res = this->CallPawnFunction( func_id );
      ret[i] = res;
    }
    param->skip_code_start = ret[DEBUG_SCENE_EDIT_PARAM_SKIP_CODE_START];
    param->skip_code_end = ret[DEBUG_SCENE_EDIT_PARAM_SKIP_CODE_END];
    param->skip_point_max = ret[DEBUG_SCENE_EDIT_PARAM_SKIP_POINT_MAX];
    return param->skip_code_start;
  }
  param->skip_code_start = 0;
  param->skip_code_end = 1;
  param->skip_point_max = 1;
  return 0;
}
#endif

//-----------------------------------------------------------------------------
/**
* @brief 使われていないハンドルを取得
*
* @return ハンドル(-1:全て使われている)
*/
//-----------------------------------------------------------------------------
s32 RegularObject::GetUnusedParallelHandle( void ) const
{
  s32 check = 0x01;
  for( u32 i = 0; i < 30; i++ )
  {
    if( !(m_ParallelHandleBit & check) )
    {
      return check;
    }

    check = check << 1;
  }

  return -1;
}

//-----------------------------------------------------------------------------
/**
* @brief ハンドルの使用フラグを操作
*
* @param handle 指定ハンドル
* @param isUse　true 使用
*/
//-----------------------------------------------------------------------------
void RegularObject::SetParallelHandleUsageState( s32 handle, b32 isUse )
{
  if( isUse )
  {
    m_ParallelHandleBit = (m_ParallelHandleBit | handle);
  }
  else
  {
    m_ParallelHandleBit = (m_ParallelHandleBit & (~handle) );
  }
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )