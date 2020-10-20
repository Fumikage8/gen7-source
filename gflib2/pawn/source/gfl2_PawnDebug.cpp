//==============================================================================
/**
 * @file  gfl_PawnDebug.cpp
 * @brief 
 * @author  kimura_shin
 * @data  2011/05/25, 15:19
 */
// =============================================================================


// ===========================================================================
// ＰＡＷＮベースタスク＜デバッグ用＞
// ===========================================================================

#include "pawn/include/gfl2_Pawn.h"
#include "pawn/include/gfl2_PawnBase.h"
#include "pawn/include/gfl2_PawnDebug.h"

#if GFL_PAWN_DEBUG

#include <ctype.h>

#ifdef USE_PROPERTY
GFL_RTTI_ROOT_INSTANCE( gfl2::pawn::PawnDebug )
#endif


b32                     gfl2::pawn::PawnDebug::s_Initialized = false;
gfl2::pawn::PawnDebug   *gfl2::pawn::PawnDebug::s_InstanceList[ PAWN_INSTANCE_MAX ];

// ----------------------------------------------------------------
// Construct
// ----------------------------------------------------------------
gfl2::pawn::PawnDebug::PawnDebug( gfl2::heap::HeapBase* heap_debug )
: PawnBase(), m_pDebugData( 0 ), m_pSharedParent(0), m_pHeapDebug(heap_debug), m_AmxHash(0), m_DebugSleep(0)
{
  if( heap_debug )  Initialize( heap_debug );
}


//------------------------------------------------------------------------------
/**
 * @func        Initialize
 * @brief       初期化(デバッグ用).
 * @author      N.Takeda
 * @date        2015/02/23(月) 17:44:59
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  gfl2::pawn::PawnDebug::Initialize( gfl2::heap::HeapBase* heap_debug )
{
  GFL_ASSERT_MSG( heap_debug, "heap_debug" );
  m_pHeapDebug = heap_debug;
  
  m_pBreakPoint = GFL_NEW(m_pHeapDebug) BreakPointList(m_pHeapDebug);
  if( !s_Initialized ){
#ifdef USE_PROPERTY
    GetRtti()->SetPropertyListHeap(m_pHeapDebug);
    GFL_RTTI_REGISTER_FUNCTION( DebugContinue );
    GFL_RTTI_REGISTER_FUNCTION( DebugNext );
    GFL_RTTI_REGISTER_FUNCTION( DebugStep );
    GFL_RTTI_REGISTER_FUNCTION( EnterDebugMode );
    GFL_RTTI_REGISTER_FUNCTION( SetBreakPoint );
    GFL_RTTI_REGISTER_FUNCTION( LoadPcFile );
    GFL_RTTI_REGISTER_FUNCTION( SetPara );
    memset(s_InstanceList,0x00,sizeof(size_t)*PAWN_INSTANCE_MAX);
#endif
    s_Initialized = true;
  }
  InitializePawnDebug( );
  memset(&m_AmxDebug,0x00,sizeof(AMX_DBG));
  
  for( s32 i = 0; i < PAWN_INSTANCE_MAX; ++i ){
    if(!s_InstanceList[i]){
      s_InstanceList[i] = this;
      break;
    }
  }
  
#if 0 ////@attention DEPEND_ON_OLD_MODULE nijiではHIO常時接続非対応LE
#ifdef GF_PLATFORM_CTR
  //  CreateHost( heap );
  m_pHost = gfl2::hio::Host::GetInstance( );

  if( m_pHost ){
    if( !m_pHost->GetHostRead( )->IsAlive( ) ){
      m_pHost->GetHostRead( )->Start( gfl2::hio::Host::READ_THREAD_PRIORITY );
    }

    if( !m_pHost->GetHostWrite( )->IsAlive( ) ){
      m_pHost->GetHostWrite( )->Start( gfl2::hio::Host::WRITE_THREAD_PRIORITY );
    }
  }
#endif
#endif
}

// ----------------------------------------------------------------
// Destruct
// ----------------------------------------------------------------
gfl2::pawn::PawnDebug::~PawnDebug( void )
{
  for( s32 i = 0; i < PAWN_INSTANCE_MAX; ++i ){
    if(s_InstanceList[i] == this){
      s_InstanceList[i] = 0;
    }
  }
  
  ClearPawnDebug( );
  GFL_DELETE m_pBreakPoint;
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnDebug::InitializePawnDebug( void )
{
  m_pOldFileName = m_pCurrentFileName = m_pOldFunction = m_pCurrentFunction = "";
  m_OldLine = m_CurrentLine = 0;
  //  m_DebugMode = 0;
  m_Pause = false;
  m_VariableCount = 0;
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnDebug::ClearPawnDebug( void )
{
  AMX_DBG* ad = GetAmxDebug( );
  if( ad->hdr ){
    dbg_FreeInfo( ad );
    ad->hdr = 0;
    m_pBreakPoint->clear_and_delete<BreakPoint > ( );
    if(m_pDebugData){
#ifdef GF_PLATFORM_CTR
//      GFL_DELETE_ARRAY m_pDebugData;
      GflHeapFreeMemory(m_pDebugData);
#else // 
      GFL_DELETE_ARRAY m_pDebugData;
#endif
      m_pDebugData = 0;
    }
  }
}

//==============================================================================
//==============================================================================

// ----------------------------------------------------------------
// EnterDebugMode Ｐａｗｎのデバッグモードへ入る
// ----------------------------------------------------------------
void gfl2::pawn::PawnDebug::EnterDebugMode( void* )
{
  // デバッグモードを真に設定
  //  m_DebugMode = true;

  // はじめのブレイクを設定するために実行
  m_ExecuteReturnValue = AMX_ERR_NONE;
  //  m_ExecuteReturnValue = AMX_ERR_SLEEP;
  //amx_Exec( GetAmx( ), 0, AMX_EXEC_MAIN );
  m_pOldFunction = m_pCurrentFunction;
  m_pOldFileName = m_pCurrentFileName;
}

// ----------------------------------------------------------------
// DebugBreakInstance デバッグブレイクの実体関数
// ----------------------------------------------------------------
int gfl2::pawn::PawnDebug::DebugBreakInstance( AMX* amx )
{
  //  if( !m_Pause ){
  //    //    GFL_PRINT( "no debug %p\n", this );
  //    return AMX_ERR_NONE;
  //  } else {
  AMX_DBG* amxdbg = GetAmxDebug( );
  // ファイル名を設定
  //    m_pOldFileName = m_pCurrentFileName;
  dbg_LookupFile( amxdbg, amx->cip, &m_pCurrentFileName );

  dbg_LookupFunction( amxdbg, amx->cip, &m_pCurrentFunction );

  // 行を設定
  //    m_OldLine = m_CurrentLine;
  long line;
  dbg_LookupLine( amxdbg, amx->cip, &line );
  m_CurrentLine = line;
  
  m_DebugSleep = true;

  //    GFL_PRINT( "break %s %d\n",m_pCurrentFileName,m_CurrentLine);

  return AMX_ERR_SLEEP;
  //  }
}

// ----------------------------------------------------------------
// DebugBreak フックのためのスタティックな関数
// ----------------------------------------------------------------
int gfl2::pawn::PawnDebug::PawnDebugBreak( AMX* amx )
{
  return GetClass<PawnDebug*>( amx )->DebugBreakInstance( amx );
}
inline bool gfl2::pawn::PawnDebug::CheckBreakPoint( void )
{
  if( !m_pBreakPoint->GetCounter( ) ){
    return false;
  }

  c8 key[0x100];
  
  sprintf( key, "%s:%d", m_pCurrentFileName, m_CurrentLine );
  
  s32 key_len = strlen(key);
  for( s32 i = 0; i < key_len; ++i ){
    if(key[i] == '\\'){
      key[i] = '/';
    }
  }

  //  GFL_PRINT( "check %s\n", key );

  if( m_pBreakPoint->Retrieve( key ) ){
    //    GFL_PRINT( "BREAK! %s\n", key );
    return true;
  }
  return false;
}

// ----------------------------------------------------------------
// DebugContinue  コンティニュー
// ----------------------------------------------------------------
void gfl2::pawn::PawnDebug::DebugContinue( void* )
{
  SetOldLine( 0 );
  m_Pause = false;
}

// ----------------------------------------------------------------
// DebugNext  ネクスト実行
// ----------------------------------------------------------------
void gfl2::pawn::PawnDebug::DebugNext( void* )
{
#if 0 // 
  do{
    ExecuteStep( );
  } while( m_pOldFunction != m_pCurrentFunction || m_OldLine == m_CurrentLine );
  m_pOldFunction = m_pCurrentFunction;
#else // 
  const c8* old_func = m_pCurrentFunction;
  s32 old_line = m_CurrentLine;
  for(;; ){
    s32 old_ret = m_ExecuteReturnValue;
    ExecuteStep( );

    //    GFL_PRINT( "%s ? %s %d ? %d %d\n", old_func ,m_pCurrentFunction ,old_line ,m_CurrentLine, m_ExecuteReturnValue );

    if( !old_ret || (old_func == m_pCurrentFunction && old_line != m_CurrentLine ) ){
//      if( ( ( !old_ret || old_func == m_pCurrentFunction ) && old_line != m_CurrentLine ) || m_ExecuteReturnValue == AMX_ERR_NONE ){
//    if( ( old_func == m_pCurrentFunction && old_line != m_CurrentLine ) || m_ExecuteReturnValue == AMX_ERR_NONE ){
      break;
    }
  }
  m_pOldFunction = m_pCurrentFunction;
#endif
  UpdateVariables( );
  //  GFL_PRINT( "Next out! %d %d\n",m_OldLine, m_CurrentLine);
}

// ----------------------------------------------------------------
// DebugStep  ステップ実行
// ----------------------------------------------------------------
void gfl2::pawn::PawnDebug::DebugStep( void* )
{
  do{
    ExecuteStep( );
    //    GFL_PRINT( "step %s:%d\n", m_pCurrentFileName, m_CurrentLine );
  } while( ( m_OldLine == m_CurrentLine && m_pOldFileName == m_pCurrentFileName ) && m_ExecuteReturnValue != AMX_ERR_NONE );
  UpdateVariables( );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl2::pawn::PawnDebug::SetPara(void* buf)
{
  c8* str = (c8*)buf;
  s32 i = 0;
  for( ; str[i]; ++i ){
    if(str[i] == ' '){
      str[i] = 0x00;
      break;
    }
  }
  Variable* vp = &m_Variables[atoi(str)];
  if(vp->m_Type == Variable::TYPE_INTEGER){
    *((s32*)vp->m_pValue) = atoi(&str[i+1]);
  }else{
    *((f32*)vp->m_pValue) = atof(&str[i+1]);
  }
}


// ----------------------------------------------------------------
// LoadPawnDebugInfo
// ----------------------------------------------------------------
//#include "amxdbg.h"
void gfl2::pawn::PawnDebug::LoadPawnDebugInfo( const void* buffer )
{
  // 3DS にはstdioがないのでメモリー版を作成した
  s32 err = dbg_LoadInfo( buffer );
  //  FILE *fp = fopen("../../../../../romfiles/tutorial/pawn/01_hello.amx","rb");
  //  s32 err = ::dbg_LoadInfo( GetAmxDebug(), fp );
  //  fclose(fp);
  if( err ){
    return;
  }
  //  Assert( err, "Pawn Initialize Error!! No DEBUG data in this pawn file!! Please COMPILE by debug mode\n" );

  // デバッグのフックを設定
  SetDebugHook( );
}

// ----------------------------------------------------------------
// Update
// ----------------------------------------------------------------
s32 gfl2::pawn::PawnDebug::ExecuteDebug( void )
{
  for(;; ){ // continue の挙動が変？なので do while が使えない・・・
    s32 old_line = m_CurrentLine;
    //    const c8* old_func = m_pCurrentFunction;
    const c8* old_fname = m_pCurrentFileName;
    m_DebugSleep = false;
    s32 ret = ExecuteStep( );
    
    // １フレームの処理が終了した場合
    if( ret == GFL_PAWN_FINISHED || (ret == AMX_ERR_SLEEP && !m_DebugSleep) ){
      // GFL_PRINT( "ret %d %s\n", ret,m_pCurrentFunction);
      return ret;
    }

    if( old_line == m_CurrentLine && old_fname == m_pCurrentFileName ){
      continue;
    }

    if( CheckBreakPoint( ) ){
      m_Pause = true;
      //      m_DebugMode = true;
      //      GFL_PRINT( "break\n");
      UpdateVariables( );
      return GFL_PAWN_DEBUGGING;
    }
    if( m_SuspendFrame ){
      --m_SuspendFrame;
      return GFL_PAWN_SUSPENDING;
    }
  }
}

// ----------------------------------------------------------------
// Update
// ----------------------------------------------------------------
s32 gfl2::pawn::PawnDebug::Execute( void )
{
  if( IsAvailable( ) ){
    if( !m_Pause ){
      return ExecuteDebug( );
    }
    return GFL_PAWN_DEBUGGING;
  }
  return GFL_PAWN_NOT_AVAILABLE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdlib.h>
int AMXAPI gfl2::pawn::PawnDebug::dbg_LoadInfo( const void* data_org )
{
  AMX_HEADER amxhdr;
  AMX_DBG_HDR dbghdr;
  unsigned char *ptr;
  int index, dim;
  AMX_DBG_SYMDIM *symdim;
  u8* data = ( u8* )data_org;

  //  GFL_ASSERT_MSG( fp != NULL );
  //  GFL_ASSERT_MSG( amxdbg != NULL );
  AMX_DBG* amxdbg = GetAmxDebug( );

  memset( &amxhdr, 0, sizeof amxhdr );
  //  fseek(fp, 0L, SEEK_SET);
  //  fread(&amxhdr, sizeof amxhdr, 1, fp);
  memcpy( &amxhdr, data, sizeof(amxhdr ) );
#if BYTE_ORDER==PAWN_BIG_ENDIAN
  amx_Align32( ( uint32_t* ) & amxhdr.size );
  amx_Align16( &amxhdr.magic );
  amx_Align16( &dbghdr.flags );
#endif
  if( amxhdr.magic != AMX_MAGIC )
    return AMX_ERR_FORMAT;
  if( ( amxhdr.flags & AMX_FLAG_DEBUG ) == 0 )
    return AMX_ERR_DEBUG;

  //fseek( fp, amxhdr.size, SEEK_SET );
  data = gfl2::core::Utility::AddAddress( data, amxhdr.size );
  memset( &dbghdr, 0, sizeof(AMX_DBG_HDR ) );
  //fread( &dbghdr, sizeof(AMX_DBG_HDR ), 1, fp );
  memcpy( &dbghdr, data, sizeof(AMX_DBG_HDR ) );
  data = gfl2::core::Utility::AddAddress( data, sizeof(AMX_DBG_HDR ) );

#if BYTE_ORDER==PAWN_BIG_ENDIAN
  amx_Align32( ( uint32_t* ) & dbghdr.size );
  amx_Align16( &dbghdr.magic );
  amx_Align16( &dbghdr.files );
  amx_Align16( &dbghdr.lines );
  amx_Align16( &dbghdr.symbols );
  amx_Align16( &dbghdr.tags );
  amx_Align16( &dbghdr.automatons );
  amx_Align16( &dbghdr.states );
#endif
  if( dbghdr.magic != AMX_DBG_MAGIC )
    return AMX_ERR_FORMAT;

  /* allocate all memory */
  memset( amxdbg, 0, sizeof(AMX_DBG ) );
//  amxdbg->hdr = ( AMX_DBG_HDR* )malloc( ( size_t )dbghdr.size );
  amxdbg->hdr = (AMX_DBG_HDR*)(GFL_NEW_ARRAY( m_pHeapDebug ) u8 [dbghdr.size]);
  if( dbghdr.files > 0 )
    amxdbg->filetbl = GFL_NEW_ARRAY( m_pHeapDebug ) AMX_DBG_FILE* [dbghdr.files];
  if( dbghdr.symbols > 0 )
    amxdbg->symboltbl = GFL_NEW_ARRAY( m_pHeapDebug ) AMX_DBG_SYMBOL* [ dbghdr.symbols ];
  if( dbghdr.tags > 0 )
    amxdbg->tagtbl = GFL_NEW_ARRAY( m_pHeapDebug ) AMX_DBG_TAG* [ dbghdr.tags ];
  if( dbghdr.automatons > 0 )
    amxdbg->automatontbl = GFL_NEW_ARRAY( m_pHeapDebug ) AMX_DBG_MACHINE* [ dbghdr.automatons ];
  if( dbghdr.states > 0 )
    amxdbg->statetbl = GFL_NEW_ARRAY( m_pHeapDebug ) AMX_DBG_STATE* [dbghdr.states];
  if( amxdbg->hdr == NULL
    || ( dbghdr.files > 0 && amxdbg->filetbl == NULL )
    || ( dbghdr.symbols > 0 && amxdbg->symboltbl == NULL )
    || ( dbghdr.tags > 0 && amxdbg->tagtbl == NULL )
    || ( dbghdr.states > 0 && amxdbg->statetbl == NULL )
    || ( dbghdr.automatons > 0 && amxdbg->automatontbl == NULL ) ){
    dbg_FreeInfo( amxdbg );
    return AMX_ERR_MEMORY;
  } /* if */

  /* load the entire symbolic information block into memory */
  memcpy( amxdbg->hdr, &dbghdr, sizeof dbghdr );
  //  fread( amxdbg->hdr + 1, 1, ( size_t )( dbghdr.size - sizeof dbghdr ), fp );
  size_t sz = ( size_t )( dbghdr.size - sizeof dbghdr );
  memcpy( amxdbg->hdr + 1, data, sz );
  data = gfl2::core::Utility::AddAddress( data, sz );

  /* run through the file, fix alignment issues and set up table pointers */
  ptr = ( unsigned char * )( amxdbg->hdr + 1 );

  /* file table */
  for( index = 0; index < dbghdr.files; index++ ){
    GFL_ASSERT_MSG( amxdbg->filetbl != NULL, "amxdbg->filetbl != NULL" );
    amxdbg->filetbl[index] = ( AMX_DBG_FILE * )ptr;
#if BYTE_ORDER==PAWN_BIG_ENDIAN
    amx_AlignCell( &amxdbg->filetbl[index]->address );
#endif
    for( ptr = ptr + sizeof(AMX_DBG_FILE ); *ptr != '\0'; ptr++ )
      /* nothing */;
    ptr++; /* skip '\0' too */
  } /* for */

  /* line table */
  amxdbg->linetbl = ( AMX_DBG_LINE* )ptr;
#if BYTE_ORDER==PAWN_BIG_ENDIAN
  for( index = 0; index < dbghdr.lines; index++ ){
    amx_AlignCell( &amxdbg->linetbl[index].address );
    amx_Align32( ( uint32_t* ) & amxdbg->linetbl[index].line );
  } /* for */
#endif
  ptr += dbghdr.lines * sizeof(AMX_DBG_LINE );

  /* symbol table (plus index tags) */
  for( index = 0; index < dbghdr.symbols; index++ ){
    GFL_ASSERT_MSG( amxdbg->symboltbl != NULL, "amxdbg->symboltbl != NULL" );
    amxdbg->symboltbl[index] = ( AMX_DBG_SYMBOL * )ptr;
#if BYTE_ORDER==PAWN_BIG_ENDIAN
    amx_AlignCell( &amxdbg->symboltbl[index]->address );
    amx_Align16( ( uint16_t* ) & amxdbg->symboltbl[index]->tag );
    amx_AlignCell( &amxdbg->symboltbl[index]->codestart );
    amx_AlignCell( &amxdbg->symboltbl[index]->codeend );
    amx_Align16( ( uint16_t* ) & amxdbg->symboltbl[index]->dim );
#endif
    for( ptr = ptr + sizeof(AMX_DBG_SYMBOL ); *ptr != '\0'; ptr++ )
      /* nothing */;
    ptr++; /* skip '\0' too */
    for( dim = 0; dim < amxdbg->symboltbl[index]->dim; dim++ ){
      symdim = ( AMX_DBG_SYMDIM * )ptr;
      amx_Align16( ( uint16_t* ) & symdim->tag );
      amx_AlignCell( &symdim->size );
      ptr += sizeof(AMX_DBG_SYMDIM );
    } /* for */
  } /* for */

  /* tag name table */
  for( index = 0; index < dbghdr.tags; index++ ){
    GFL_ASSERT_MSG( amxdbg->tagtbl != NULL, "amxdbg->tagtbl != NULL" );
    amxdbg->tagtbl[index] = ( AMX_DBG_TAG * )ptr;
#if BYTE_ORDER==PAWN_BIG_ENDIAN
    amx_Align16( &amxdbg->tagtbl[index]->tag );
#endif
    for( ptr = ptr + sizeof(AMX_DBG_TAG ) - 1; *ptr != '\0'; ptr++ )
      /* nothing */;
    ptr++; /* skip '\0' too */
  } /* for */

  /* automaton name table */
  for( index = 0; index < dbghdr.automatons; index++ ){
    GFL_ASSERT_MSG( amxdbg->automatontbl != NULL, "amxdbg->automatontbl != NULL" );
    amxdbg->automatontbl[index] = ( AMX_DBG_MACHINE * )ptr;
#if BYTE_ORDER==PAWN_BIG_ENDIAN
    amx_Align16( &amxdbg->automatontbl[index]->automaton );
    amx_AlignCell( &amxdbg->automatontbl[index]->address );
#endif
    for( ptr = ptr + sizeof(AMX_DBG_MACHINE ) - 1; *ptr != '\0'; ptr++ )
      /* nothing */;
    ptr++; /* skip '\0' too */
  } /* for */

  /* state name table */
  for( index = 0; index < dbghdr.states; index++ ){
    GFL_ASSERT_MSG( amxdbg->statetbl != NULL, "amxdbg->statetbl != NULL" );
    amxdbg->statetbl[index] = ( AMX_DBG_STATE * )ptr;
#if BYTE_ORDER==PAWN_BIG_ENDIAN
    amx_Align16( &amxdbg->statetbl[index]->state );
    amx_Align16( &amxdbg->automatontbl[index]->automaton );
#endif
    for( ptr = ptr + sizeof(AMX_DBG_STATE ) - 1; *ptr != '\0'; ptr++ )
      /* nothing */;
    ptr++; /* skip '\0' too */
  } /* for */

  return AMX_ERR_NONE;
}
void gfl2::pawn::PawnDebug::SetBreakPoint( void* ptr )
{
//  GFL_PRINT( "append %s\n", ( c8* )ptr );
  m_pBreakPoint->clear_and_delete<BreakPoint > ( );

  c8 *bgn, *end;
  bgn = end = ( c8* )ptr;
  s32 cnt = 0;
  while( end[cnt] ){
    if( end[cnt] == ' ' ){
      end[cnt] = 0x00;
      //      GFL_PRINT( "break %s\n",bgn);
      m_pBreakPoint->Append( GFL_NEW(m_pHeapDebug) BreakPoint( bgn, m_pHeapDebug ) );
      bgn = &end[cnt + 1];
    }
    ++cnt;
  }
}

int AMXAPI dbg_FreeInfo(AMX_DBG *amxdbg)
{
  GFL_ASSERT_MSG( amxdbg != NULL, "amxdbg != NULL" );
  if (amxdbg->hdr != NULL)
    GFL_DELETE_ARRAY amxdbg->hdr;
  if (amxdbg->filetbl != NULL)
    GFL_DELETE_ARRAY amxdbg->filetbl;
  if (amxdbg->symboltbl != NULL)
    GFL_DELETE_ARRAY amxdbg->symboltbl;
  if (amxdbg->tagtbl != NULL)
    GFL_DELETE_ARRAY amxdbg->tagtbl;
  if (amxdbg->automatontbl != NULL)
    GFL_DELETE_ARRAY amxdbg->automatontbl;
  if (amxdbg->statetbl != NULL)
    GFL_DELETE_ARRAY amxdbg->statetbl;
  memset(amxdbg, 0, sizeof(AMX_DBG));
  return AMX_ERR_NONE;
}

void gfl2::pawn::PawnDebug::dbg_GetVariables( AMX_DBG *amxdbg, ucell scopeaddr )
{
  ucell codestart, codeend;
  int index;

  GFL_ASSERT_MSG( amxdbg != NULL, "amxdbg != NULL" );

  codestart = codeend = 0;
  index = 0;
  //  m_Variables.clear( );
  m_VariableCount = 0;
  for(;; ){
    if(!amxdbg->hdr){
      return;
    }
    while( index < amxdbg->hdr->symbols
      /*&& (amxdbg->symboltbl[index]->ident == iFUNCTN || strcmp(amxdbg->symboltbl[index]->name, symname) != 0)*/
      && ( amxdbg->symboltbl[index]->codestart > scopeaddr || amxdbg->symboltbl[index]->codeend < scopeaddr ) )
      index++;

    if( index >= amxdbg->hdr->symbols )
      break;

    //    GFL_PRINT( "all %s\n",amxdbg->symboltbl[index]->name);

    /* check the range, keep a pointer to the symbol with the smallest range */
    //    if( ( codestart == 0 && codeend == 0
    //      || amxdbg->symboltbl[index]->codestart >= codestart && amxdbg->symboltbl[index]->codeend <= codeend ) ){
    //    GFL_PRINT( "valname %s(%d) start %d <= %d end %d <= %d\n", amxdbg->symboltbl[index]->name,amxdbg->symboltbl[index]->vclass,
    //                    codestart, amxdbg->symboltbl[index]->codestart,
    //                    amxdbg->symboltbl[index]->codeend, codeend );

    if( amxdbg->symboltbl[index]->ident == iVARIABLE && !amxdbg->symboltbl[index]->vclass ){
      m_Variables[m_VariableCount++].SetVariable( amxdbg->symboltbl[index]->name, amxdbg->symboltbl[index] );
      //      GFL_PRINT( "ok %s\n", amxdbg->symboltbl[index]->name );
      if( VARIABLE_MAX <= m_VariableCount ){
        break;
      }
    } else if( ( codestart == 0 && codeend == 0 )
      || ( amxdbg->symboltbl[index]->codestart >= codestart && amxdbg->symboltbl[index]->codeend <= codeend ) ){
      //      m_Variables.push_back( Variable( amxdbg->symboltbl[index]->name, amxdbg->symboltbl[index] ) );
      if( amxdbg->symboltbl[index]->ident == iVARIABLE ){
        m_Variables[m_VariableCount++].SetVariable( amxdbg->symboltbl[index]->name, amxdbg->symboltbl[index] );
      } else {
        codestart = amxdbg->symboltbl[index]->codestart;
        codeend = amxdbg->symboltbl[index]->codeend;
      }
      //      GFL_PRINT( "ok %s\n", amxdbg->symboltbl[index]->name );

      if( VARIABLE_MAX <= m_VariableCount ){
        break;
      }
    } /* if */
    index++;
  } /* for */
}
void gfl2::pawn::PawnDebug::UpdateVariables( void )
{
  ucell address;

  AMX_DBG* amxdbg = GetAmxDebug( );
  // 選択されている行のアドレスを取ってくる
  dbg_GetLineAddress( amxdbg, m_CurrentLine, m_pCurrentFileName, &address );

  // 変数の情報を取得
  dbg_GetVariables( amxdbg, address );

  //  List<Variable>::iterator it = m_Variables.begin();
  //  GFL_PRINT( "tbl %d\n",m_Variables.size( ));
  //  for( s32 i = 0; i < m_Variables.size( ); ++i ){
  for( s32 i = 0; i < m_VariableCount; ++i ){
    //    const AMX_DBG_SYMBOL *sym = it[i].m_pSym;
    const AMX_DBG_SYMBOL *sym = m_Variables[i].m_pSym;
    // 変数で変数の有効範囲にあるか？
    //    if( sym->ident == iVARIABLE ){
    AMX* amx = GetAmx( );
    // Ｐａｗｎの変数のアドレスを計算
    AMX_HEADER* hdr = ( AMX_HEADER * )amx->base;
    u8* data = ( amx->data != NULL ) ? amx->data : amx->base + ( int )hdr->dat;
    data += sym->address;

//  c8 buf[gfl2::rtti::BUFFER_SIZE];
    //      const c8* para = it[i].m_pName;
    //const c8* para = m_Variables[i].m_pName;

    if( sym->vclass == 1 ){ // ローカル変数の場合
      // 現在実行中のコマンドがローカル変数の有効範囲内にあるか？
      //      if( sym->codestart <= ( ucell )amx->cip && ( ucell )amx->cip <= sym->codeend ){
      data += amx->frm;
      //      } else {
      //        sprintf( buf, "%8s Not Available Now", para );
      //        //        GFL_PRINT( "%s\n", buf );
      //        continue;
      //      }
    }

    // うーん、美しくない。いい方法があれば教えて?
    c8 fval_str[0x20];
    f32 fval_org = *reinterpret_cast < f32* >( data );
    sprintf( fval_str, "%f", fval_org );
    bool fval_flag = true;

    size_t j = 0;
    while( fval_str[j] ){
      if( isalpha( fval_str[j++] ) ){
        fval_flag = false;
        break;
      }
    }
    f32 fval = static_cast < f32 >( atof( fval_str ) );

    s32 val = *reinterpret_cast < s32* >( data );
    m_Variables[i].m_pValue = data;
    if( fval == 0.0f || fval_flag == false ){ // 整数
      //        sprintf( buf, "SetVariable %s %d 0x%08x", para, val, val );
      m_Variables[i].m_S32Value = val;
      m_Variables[i].m_Type = Variable::TYPE_INTEGER;
      //      GFL_PRINT( "[%d] %s %d\n", i, para, val );
    } else {
      m_Variables[i].m_F32Value = fval;
      m_Variables[i].m_Type = Variable::TYPE_FLOAT;
      //      GFL_PRINT( "[%d] %s float %f\n", i, para, fval );
      //        sprintf( buf, "SetVariable %s %s 0x%08x", para, fval_str, val );
    }
    //    }
  }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
u32 gfl2::pawn::PawnDebug::CalcAmxHash(const c8* fname)
{
  s32 len =  strlen(fname);
  s32 bgn=0;
  for(s32 i = len-1; 0 <= i; --i ){
    if(fname[i]=='/'){
      bgn = i+1;
      break;
    }
  }
  return gfl2::core::Utility::CalcHash(&fname[bgn]);
}


//==============================================================================
//==============================================================================
void gfl2::pawn::PawnDebug::Load(const void* buffer, const size_t size, const c8* amxname)
{
  ClearPawnDebug( );
  InitializePawnDebug( );
  PawnBase::Load( buffer, size, amxname );
  LoadPawnDebugInfo( buffer );
  m_AmxHash = CalcAmxHash(amxname);

  EnterDebugMode( );
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnDebug::LoadShare(const PawnBase* org)
{
  ClearPawnDebug( );
  InitializePawnDebug( );

  PawnBase::LoadShare( org );
  LoadPawnDebugInfo( org->GetBuffer() );
  
  m_AmxHash = ((PawnDebug*)org)->m_AmxHash;
  m_pSharedParent = org;

  EnterDebugMode( );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void* gfl2::pawn::PawnDebug::LoadPcFile(void* fname, size_t* sizep)
{
#ifdef GF_PLATFORM_CTR
#if 0 //@attention DEPEND_ON_OLD_MODULE nijiではHIO常時接続非対応
  gfl2::fs::HioFile file;
  wchar_t wfname[0x200];
  gfl2::std::MultiByteStr2WideStr( wfname, ( c8* )fname, 0x200 );
  if( file.Open( wfname, gfl2::fs::OPEN_MODE_READ ) ){
    size_t size = file.GetSize( );
//    u8* buf = GFL_NEW_ARRAY( m_pHeap ) u8 [size + 1];
    u8* buf = (u8*)GflHeapAllocMemoryAlign(m_pHeapDebug,size+1,gfl2::hio::GFL_HIO_ALIGN); // これだとハングしちゃうなんで？
    
    size_t read_size = file.Read( ( void* )buf, size );
    GFL_ASSERT_MSG(size == read_size,"Read HIO Error %s",fname);
    file.Close( );
    GFL_PRINT( "loaded %s %d\n", ( c8* )fname,size );
    *sizep = size;
    return buf;
  }
#endif
#else // 
#if 1
  FILE    *fp;
  fpos_t  size;
  
  if ((fp = fopen( (c8*)fname, "r")) == NULL)  return NULL;
  /// サイズ取得.
  fseek(fp, 0L, SEEK_END);
  fgetpos( fp, &size );
  fseek(fp, 0L, SEEK_SET);
  if( size == 0 ) return NULL;
  
  u8* buf = GFL_NEW_ARRAY( m_pHeapDebug ) u8 [ size + 1 ];
  fread( buf, 1, size, fp);
  fclose(fp);
#else
  QByteArray dat = gfl2::qt::File::LoadBinary( (c8*)fname );
  if( dat.size() )
  {
    u8* buf = DbgNew u8 [dat.size()+1];
    memcpy( buf, dat.data(), dat.size() );
    return buf;
  }
#endif
#endif
  return 0;
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnDebug::LoadPcFile( void* fname )
{
#ifdef GF_PLATFORM_CTR
  if(m_AmxHash){
    u32 hsh = CalcAmxHash((c8*)fname);
    if(hsh != m_AmxHash){
      return;
    }
  }
  
  if(!m_pSharedParent){
    size_t size;
    void* buf = LoadPcFile(fname,&size);
    if(buf){
      Load( ( const void* )buf, size, ( c8* )fname );
      m_pDebugData = (u8*)buf;
      //    EnterDebugMode( );
    }else{
      return;
    }
  }else{
    LoadShare(m_pSharedParent);
  }
  Update();
#endif
}

#if 0 ////@attention DEPEND_ON_OLD_MODULE nijiではHIO常時接続非対応
#if /*GFL_PAWN_DEBUG && */defined(GF_PLATFORM_CTR)
//==============================================================================
//==============================================================================
bool gfl2::pawn::PawnDebug::SendDebugDataToClient( gfl2::hio::HostWrite* hw )
{
  // 書き込み待ちか？
  if( m_Pause && hw && !hw->GetDataSize( ) ){
    c8* buf = hw->GetWriteBuffer( );
    if( strcmp( GetCurrentFileName( ), GetOldFileName( ) ) ){
      strcpy( buf, "LoadFileRequest " );
      strcat( buf, GetCurrentFileName( ) );

//      GFL_PRINT( "file %s\n", buf );
      hw->SetData( buf );

      SetOldFileName( GetCurrentFileName( ) );
      return true;
    } else if( GetOldLine( ) != GetCurrentLine( ) ){
      sprintf( buf, "SetCurrentLine %d", GetCurrentLine( ) );

      hw->SetData( buf );
//      GFL_PRINT( "line changed: %s\n", buf );
      SetOldLine( GetCurrentLine( ) );
      return true;
    } else if( m_VariableCount ){
      sprintf( buf, "SetVariable %d", m_VariableCount );
      //        strcpy( buf, "SetVariable" );
      c8* ptr = &buf[strlen( buf )];
      for( s32 i = 0; i < m_VariableCount; ++i ){
        Variable* v = &m_Variables[i];
        if( v->m_Type == Variable::TYPE_INTEGER ){
          sprintf( ptr, " %s %d", v->m_pName, v->m_S32Value );
        } else {
          sprintf( ptr, " %s %f", v->m_pName, v->m_F32Value );
        }
        ptr = &buf[strlen( buf )];
      }
      hw->SetData( buf );
      m_VariableCount = 0;
      return true;
    }
    return false; // 他のコマンドを受付可
  }
  return true; // 書き込み待ちなので真
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnDebug::RecieveDebugDataFromClient(gfl2::hio::HostRead* hr)
{
  // 読み込みチェック
  if( hr && hr->GetDataSize( ) && *hr->GetData() != '!' ){
//    GFL_PRINT( "!!!!!call %s\n", hr->GetData( ) );
    gfl2::rtti::CallFunction( this, hr->GetData( ) );
  }
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnDebug::UpdateDebug( gfl2::hio::Host* host )
{
  if( host ){
    for( s32 i = 0; i < PAWN_INSTANCE_MAX; ++i ){
      PawnDebug* pd = s_InstanceList[i];
      if(pd && pd->IsAvailable()){
        pd->RecieveDebugDataFromClient( host->GetHostRead( ) );
        pd->SendDebugDataToClient( host->GetHostWrite( ) );
      }
    }
    
    gfl2::hio::HostRead* hr = host->GetHostRead( );
    if( hr && hr->GetDataSize( ) ){
      hr->RestartRead( );
    }
  }
}
#endif  
#endif

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl2::pawn::PawnDebug::PrintStackHeapMax()
{
  const size_t stack = GetStackMax()>>2;
  const size_t heap = GetHeapMax()>>2;
  GFL_PRINT("Stack+Heap:%d(%d:%d:16) cells\n",stack+heap+16,stack,heap);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
gfl2::pawn::PawnDebug::BreakPoint::BreakPoint( c8* key, gfl2::heap::HeapBase* heap ) {
  m_pKeyword = GFL_NEW_ARRAY(heap) c8 [strlen( key ) + 1];
  strcpy( m_pKeyword, key );
}

#endif //GFL_PAWN_DEBUG
