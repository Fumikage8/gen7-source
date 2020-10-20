/**
 * @file btl_AiScript.cpp
 * @brief 戦闘AIスクリプト
 * @author obata_toshihiro
 * @date 2015.04.02
 */
#include <arc_def_index/arc_def.h>
#include <arc_index/BattleAi.gaix>

#include <GameSys/include/GameManager.h>

#include "./btl_BattleAiCommand.h"
#include "./btl_AiScript.h"


GFL_NAMESPACE_BEGIN( btl )

#define LOADING_AI_USE_THREAD (0)


namespace
{
  // スクリプト側で定義している変数の名前
  static const char* SCRIPT_PARAM_COMMAND_HANDLER = "p_AIHandler";
  static const char* SCRIPT_PARAM_SCORE = "p_Score";
  static const char* SCRIPT_PARAM_POKECHANGE_ENABLE = "p_PokeChangeEnable";
}


//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heapForScriptBinary  スクリプトバイナリを確保するヒープ
 * @param heapForFileRead      ファイル読み込みに使用する一時ヒープ
 */
//---------------------------------------------------------------------------
AiScript::AiScript( HeapHandle heapForScriptBinary, HeapHandle heapForFileRead ) : 
  m_heapForScriptBinary( heapForScriptBinary ),
  m_heapForFileRead( heapForFileRead ),
  m_arcHandle( NULL ),
  m_pawn( NULL ),
  m_pawnBuf( NULL ),
  m_pawnBufSize( 0 ),
  m_loadState( LOAD_STATE_NOT_LOADED ),
  m_loadedScriptDataId( GARC_BattleAI_DATA_NUM ),
  m_loadingScriptSize( 0 )
{
  this->Setup();
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
AiScript::~AiScript()
{
  this->Cleanup();
}



/**
 * @brief スクリプトを読み込むことが出来る状態にする
 */
void AiScript::Setup( void )
{
  this->OpenArcFile();
  this->CreatePawnBuffer();
  m_loadedScriptDataId = GARC_BattleAI_DATA_NUM;
  m_loadState = LOAD_STATE_NOT_LOADED;
}

/**
 * @brief アーカイブファイルを開く
 */
void AiScript::OpenArcFile( void )
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId = ARCID_BATTLE_AI;
  openReq.heapForFile = m_heapForFileRead;
  openReq.heapForReq = m_heapForFileRead->GetLowerHandle();

  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  fileManager->SyncArcFileOpen( openReq );
}

/**
 * @brief pawn スクリプトのバッファを確保する
 *
 * @note 最も大きいスクリプトを読み込めるだけのバッファを確保しておく
 */
void AiScript::CreatePawnBuffer()
{  
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  const gfl2::fs::ArcFile* arcFile = fileManager->GetArcFile( ARCID_BATTLE_AI );

  if( arcFile )
  {
    u32 max_size = 0;
    arcFile->GetMaxDataSize( &max_size );

    if( max_size )
    {
      while( max_size % 4 ) {
        ++max_size;
      }

      GflHeapSafeFreeMemory( m_pawnBuf );
      m_pawnBuf = GflHeapAllocMemory( m_heapForScriptBinary, max_size );
      m_pawnBufSize = max_size;

      BTL_PRINT("[TR_AI] alloced heapSize for pawnBuffer=0x%08x\n", max_size );
      BTL_PRINT("[TR_AI] after setup pawn heapFreeSize=0x%08x\n", m_heapForScriptBinary->GetTotalAllocatableSize() );
    }
  }
}




/**
 * @brief インスタンスを破棄できる状態にする
 */
void AiScript::Cleanup( void )
{
  this->UnLoadScript();
  this->DeletePawnBuffer();
  this->CloseArcFile();
  m_loadedScriptDataId = GARC_BattleAI_DATA_NUM;
  m_loadState = LOAD_STATE_NOT_LOADED;
}

/**
 * @brief pawn スクリプトのバッファを破棄する
 */
void AiScript::DeletePawnBuffer( void )
{
  GFL_SAFE_DELETE( m_pawn );
  GflHeapSafeFreeMemory( m_pawnBuf );
  m_pawnBufSize = 0;
}

/**
 * @brief アーカイブファイルを閉じる
 */
void AiScript::CloseArcFile( void )
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = ARCID_BATTLE_AI;
  closeReq.heapForReq = m_heapForFileRead->GetLowerHandle();
  fileManager->SyncArcFileClose( closeReq );
}



#if PM_DEBUG
//---------------------------------------------------------------------------
/**
 * @brief スクリプトを破棄し、アーカイブファイルを開きなおす
 */
//---------------------------------------------------------------------------
void AiScript::Reset( void )
{
  this->Cleanup();
  this->Setup();
}
#endif




//---------------------------------------------------------------------------
/**
 * @brief スクリプトの読み込みを開始する
 * @param heapForScriptExec スクリプトの実行に使用する一時ヒープ
 * @param scriptNo          読み込むスクリプトNo.
 * @retval true  読み込みを開始した or すでに読み込み済み
 * @retval false 読み込みを開始できなかった
 */
//---------------------------------------------------------------------------
bool AiScript::StartLoadScript( HeapHandle heapForScriptExec, BtlAiScriptNo scriptNo )
{
  // バッファが確保されていない
  if( m_pawnBuf == NULL ) {
    GFL_ASSERT(0);
    return false;
  }

  // データが存在しない
  u32 datIdx = this->GetArcDataIndex( scriptNo );
  if( datIdx >= GARC_BattleAI_DATA_NUM ) {
    GFL_ASSERT(0);
    return false;
  }

  // 読み込み中
  if( m_loadState == LOAD_STATE_LOADING ) {
    return false;
  }

  if( m_pawn == NULL )
  {
    m_pawn = GFL_NEW( heapForScriptExec ) gfl2::pawn::Pawn( heapForScriptExec );
  }
  
  // 既に読み込んである
  if( ( m_loadState == LOAD_STATE_LOADED ) &&
      ( m_loadedScriptDataId == datIdx ) ) {
    return true;
  }


  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq loadReq;
  loadReq.arcId = ARCID_BATTLE_AI;
  loadReq.datId = datIdx;
  loadReq.pBuf = m_pawnBuf;
  loadReq.bufSize = m_pawnBufSize;
  loadReq.pRealReadSize = &m_loadingScriptSize;
  loadReq.heapForReq = m_heapForFileRead->GetLowerHandle();

#if LOADING_AI_USE_THREAD
  fileManager->AddArcFileLoadDataBufReq( loadReq );
#else
  fileManager->SyncArcFileLoadDataBuf( loadReq );
#endif

  m_loadedScriptDataId = datIdx;
  m_loadState = LOAD_STATE_LOADING;

  BTL_PRINT("[AI] === Start Loading Script %d === \n", datIdx );
  return true;
}

/**
 * @brief スクリプトNo. から 対象となるデータのアーカイブ内Indexを取得する
 * @param scriptNo  スクリプトNo.
 */
u32 AiScript::GetArcDataIndex( u32 scriptNo )
{
  switch( scriptNo )
  {
  case BTL_AISCRIPT_NO_WAZA_BASIC:        return GARC_BattleAI_btl_ai_basic_AMX;
  case BTL_AISCRIPT_NO_WAZA_DOUBLE:       return GARC_BattleAI_btl_ai_double_AMX;
  case BTL_AISCRIPT_NO_WAZA_EXPERT:       return GARC_BattleAI_btl_ai_expert_AMX;
  case BTL_AISCRIPT_NO_WAZA_STRONG:       return GARC_BattleAI_btl_ai_strong_AMX;
  case BTL_AISCRIPT_NO_WAZA_ALLOWANCE:    return GARC_BattleAI_btl_ai_allowance_AMX;
  case BTL_AISCRIPT_NO_WAZA_ROYAL:        return GARC_BattleAI_btl_ai_royal_AMX;
  case BTL_AISCRIPT_NO_WAZA_INTRUDE:      return GARC_BattleAI_btl_ai_intrude_AMX;
  case BTL_AISCRIPT_NO_ITEM_BASIC:        return GARC_BattleAI_btl_ai_item_AMX;
  case BTL_AISCRIPT_NO_POKECHANGE_BASIC:  return GARC_BattleAI_btl_ai_pokechange_AMX;
  default:                          return GARC_BattleAI_DATA_NUM;
  }
}



//---------------------------------------------------------------------------
/**
 * @brief スクリプトの読み込み終了を待つ
 * @retval true   読み込みが終了した
 * @retval false  読み込みが終了していない
 */
//---------------------------------------------------------------------------
bool AiScript::WaitLoadScript( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] = 
  {
    { "AI_CMD", BattleAiCommand::AI_CMD },
    GFL_PAWN_FUNC_END
  };

  // 読み込んでいない
  if( m_loadState == LOAD_STATE_NOT_LOADED ) {
    return true;
  }

#if LOADING_AI_USE_THREAD
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  if( !( fileManager->IsArcFileLoadDataBufFinished( m_pawnBuf ) ) ) {
    return false;
  }
#endif

  m_pawn->Load( m_pawnBuf, m_loadingScriptSize );
  m_pawn->RegisterPawnFunction( s_FuncTable );
  m_loadState = LOAD_STATE_LOADED;
  return true;
}


//---------------------------------------------------------------------------
/**
 * @brief スクリプトの実行に必要なパラメータを設定する
 * @param commandHandler  関連づけるコマンドハンドラ
 */
//---------------------------------------------------------------------------
void AiScript::SetExecParameter( AiScriptCommandHandler* commandHandler )
{
  if( m_loadState != LOAD_STATE_LOADED ) {
    GFL_ASSERT(0);
    return;
  }
  m_pawn->SetGlobalParameterInteger( SCRIPT_PARAM_COMMAND_HANDLER, (uptr)(commandHandler) );
  m_pawn->SetGlobalParameterInteger( SCRIPT_PARAM_SCORE, 0 );
  m_pawn->SetGlobalParameterInteger( SCRIPT_PARAM_POKECHANGE_ENABLE, 0 );
}


//---------------------------------------------------------------------------
/**
 * @brief スクリプトを実行する
 * @retval true   実行が終了した
 * @retval false  実行が終了していない
 */
//---------------------------------------------------------------------------
bool AiScript::Execute( void )
{
  if( m_loadState != LOAD_STATE_LOADED ) {
    GFL_ASSERT(0);
    return true;
  }
  s32 result = m_pawn->Execute();
  return ( result == gfl2::pawn::PawnBase::GFL_PAWN_FINISHED );
}

//---------------------------------------------------------------------------
/**
 * @brief 実行結果を取得する
 * @param[out] dest  実行結果の格納先
 */
//---------------------------------------------------------------------------
void AiScript::GetResult( Result* dest )
{
  dest->score = m_pawn->GetGlobalParameterInteger( SCRIPT_PARAM_SCORE );
  dest->isPokeChangeEnable = ( m_pawn->GetGlobalParameterInteger( SCRIPT_PARAM_POKECHANGE_ENABLE ) != 0 );
}

//---------------------------------------------------------------------------
/**
 * @brief 読み込んだスクリプトを破棄する
 */
//---------------------------------------------------------------------------
void AiScript::UnLoadScript( void )
{
  GFL_SAFE_DELETE( m_pawn );
}




GFL_NAMESPACE_END( btl )
