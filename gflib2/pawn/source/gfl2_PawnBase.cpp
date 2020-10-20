//==============================================================================
/**
 * @file  gfl_Pawn.cpp
 * @brief 
 * @author  kimura_shin
 * @data  2011/02/02, 12:34
 */
// =============================================================================

#include "pawn/include/gfl2_Pawn.h"
#include "util/include/gfl2_std_string.h"


//==============================================================================
//==============================================================================
#if GFL_PAWN_DEBUG
const c8 * const gfl2::pawn::PawnBase::s_cErrorMessage[] = {
  "", // AMX_ERR_NONE
  "forced exit", // AMX_ERR_EXIT
  "assertion failed", // AMX_ERR_ASSERT,       
  "stack/heap collision", // AMX_ERR_STACKERR,     
  "index out of bounds", // AMX_ERR_BOUNDS,       
  "invalid memory access", // AMX_ERR_MEMACCESS,    
  "invalid instruction", // AMX_ERR_INVINSTR,     
  "stack underflow", // AMX_ERR_STACKLOW,     
  "heap underflow", // AMX_ERR_HEAPLOW,      
  "no callback, or invalid callback", // AMX_ERR_CALLBACK,     
  "native function failed", // AMX_ERR_NATIVE,       
  "divide by zero", // AMX_ERR_DIVIDE,       
  "go into sleepmode - code can be restarted", // AMX_ERR_SLEEP,        
  "no implementation for this state, no fall-back", // AMX_ERR_INVSTATE,     

  "", "", // dummy

  "out of memory", // AMX_ERR_MEMORY = 16,  
  "invalid file format", // AMX_ERR_FORMAT,       
  "file is for a newer version of the AMX", // AMX_ERR_VERSION,      
  "function not found", // AMX_ERR_NOTFOUND,     
  "invalid index parameter (bad entry point)", // AMX_ERR_INDEX,        
  "debugger cannot run", // AMX_ERR_DEBUG,        
  "AMX not initialized (or doubly initialized)", // AMX_ERR_INIT,         
  "unable to set user data field (table full)", // AMX_ERR_USERDATA,     
  "cannot initialize the JIT", // AMX_ERR_INIT_JIT,     
  "parameter error", // AMX_ERR_PARAMS,       
  "domain error, expression result does not fit in range", // AMX_ERR_DOMAIN,       
  "general error (unknown or unspecific error)", // AMX_ERR_GENERAL,      
  "overlays are unsupported (JIT) or uninitialized" // AMX_ERR_OVERLAY,      
};
#endif
static cell AMX_NATIVE_CALL _Suspend( AMX *amx, const cell* val )
{
  gfl2::pawn::PawnBase* pt = gfl2::pawn::PawnBase::GetClass<gfl2::pawn::PawnBase*>( amx );
  pt->SetSuspendFrame( val[1] - 1 );
  return 0;
}

static const GFL_PAWN_FUNC_TABLE s_PawnBaseFuncTable[] = {
  GFL_PAWN_FUNC( _Suspend )
  GFL_PAWN_FUNC_END
};

//==============================================================================
//==============================================================================
gfl2::pawn::PawnBase::PawnBase()
: m_pBuffer( 0 )
, m_BufferSize(0)   ///< Coverity.11361
, m_SuspendFrame(0)
{
  /**
   * @note  N.Takeda
   * PawnMemFillはコンストラクタから呼べないのでやむを得ずここで別途初期化.
    コンスト  InitializeAmx( );
    Load    ClearAmx( );
    Load    InitializeAmx( );
    の順で初期化しているのも妙なので、どうするのが適切なのかはもう少し解析が必要.
   */
  AMX* amx = GetAmx( );
  amx->base = NULL; ///< AMX でbaseが初期化されていないのがまずいと思うが
  
  InitializeAmx( );
}

//==============================================================================
//==============================================================================
gfl2::pawn::PawnBase::~PawnBase( void )
{
  ClearAmx( );
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnBase::InitializeAmx( void )
{
  /// このクリアをしないとエラーになるので絶対必要
  PawnMemFill( (void*)GetAmx(), 0, sizeof(AMX) );
  m_HaltFlag = 0;
  m_ExecuteReturnValue = AMX_ERR_NONE;
}
//==============================================================================
//==============================================================================
void gfl2::pawn::PawnBase::ClearAmx( void )
{
  AMX* amx = GetAmx( );
  if(amx->base){
    amx_Cleanup( amx );
    PawnDel( amx->base );
    amx->base = 0;
    m_SuspendFrame = 0;
  }
}



//==============================================================================
//==============================================================================
void gfl2::pawn::PawnBase::Load(const void* pBuffer, const size_t size, const c8* )
{
  ClearAmx( );
  InitializeAmx( );
  
  AMX* amx = GetAmx( );

  /// ヘッダ情報を取得.
  const AMX_HEADER  *pHeader = reinterpret_cast < const AMX_HEADER* >( pBuffer );

#if 1//def GFL_LITTLE_ENDIAN
#    if GFL_PAWN_DEBUG
  GFL_ASSERT_MSG( pHeader->magic == AMX_MAGIC, "This is not AMX buffer...\n" );
#    endif
  const u32 total_size = pHeader->stp;
#else
#    if GFL_PAWN_DEBUG
  GFL_ASSERT_MSG( EndianSwap16( pHeader->magic ) == AMX_MAGIC, "This is not AMX buffer...\n" );
#    endif
  const u32 total_size = EndianSwap32( pHeader->stp );
#endif

  /// スタックを含めたサイズのメモリを確保する
  u8* pPtr = PawnNew( total_size );
  /// スタッククリア
  PawnMemFill( pPtr + size, 0, total_size - size );
  /// データコピー
  PawnMemCpy( pPtr, pBuffer, size );

  /// 初期化
#if GFL_PAWN_DEBUG
  s32 err =
#endif
    amx_Init( amx, pPtr );

  PAWN_ASSERT( err, "Pawn Initialize Error" );

#if GFL_PAWN_CORE
  amx_CoreInit( amx );
#endif
#if GFL_PAWN_PRINT
  amx_PrintInit( amx );
#endif
  amx_FloatInit( amx );

#if 1
  RegisterPawnFunction( s_PawnBaseFuncTable );
#endif

  m_pBuffer = pBuffer;
  m_BufferSize = size;

//  SetHook();
}

//==============================================================================
//==============================================================================
void gfl2::pawn::PawnBase::LoadShare(const PawnBase* org)
{
  ClearAmx( );
  InitializeAmx( );
  
  AMX* amx = GetAmx( );
  const AMX* org_amx = org->GetAmx();

  /// ヘッダ情報を取得
  const AMX_HEADER* pHeader = reinterpret_cast < const AMX_HEADER* >( org_amx->base );

#if 1//def GFL_LITTLE_ENDIAN
#    if GFL_PAWN_DEBUG
  GFL_ASSERT_MSG( pHeader->magic == AMX_MAGIC, "This is not AMX buffer...\n" );
#    endif
  const u32 total_size = pHeader->stp - pHeader->dat + pHeader->cod;
#else
#    if GFL_PAWN_DEBUG
  GFL_ASSERT_MSG( EndianSwap16( pHeader->magic ) == AMX_MAGIC, "This is not AMX buffer...\n" );
#    endif
  const u32 total_size = EndianSwap32( pHeader->stp ) - EndianSwap32( pHeader->dat ) + EndianSwap32( pHeader->cod );
#endif

  /// データ、ヒープ、スタックのメモリを確保する（つまりコード以外）
  u8* pPtr = PawnNew( total_size );
  /// スタッククリア
  PawnMemFill( pPtr, 0, total_size );

  PawnMemCpy( pPtr, pHeader, pHeader->cod );
  PawnMemCpy( amx, org_amx, sizeof(AMX) );

  /// ベース部分は共有出来無い
  amx->base = pPtr;
  /// データ部分はコピーが必要
  amx->data = pPtr + pHeader->cod;
  /// データの参照ポインタを無理やり変更
  PawnMemCpy( amx->data, (void*)((size_t)pHeader + pHeader->dat), pHeader->hea - pHeader->dat );
  
  m_pBuffer = org->m_pBuffer;
  m_BufferSize = org->m_BufferSize;
//  SetHook();
}


//------------------------------------------------------------------------------
/**
 * @func    RegisterPawnFunction
 * @brief   C のスクリプト関数テーブルを Pawn に登録する.
 * @date    2014/10/23(木) 19:25:52
 *
 * @param[in] const GFL_PAWN_FUNC_TABLE*  native_func_tbl C の関数のテーブル.
 *
 * @return    なし (none)
 */
//------------------------------------------------------------------------------
void  gfl2::pawn::PawnBase::RegisterPawnFunction( const GFL_PAWN_FUNC_TABLE native_func_tbl[] )
{
  amx_Register( GetAmx( ), native_func_tbl, -1 );
}


//==============================================================================
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @func    CallPawnFunction
 * @brief   インデックス番号の関数を実行.
 * @date    2014/10/23(木) 19:51:23
 *
 * @param[in] const s32 index 関数のインデックス.
 *
 * @return    cell  Pawnからの返り値.
 */
//------------------------------------------------------------------------------
cell gfl2::pawn::PawnBase::CallPawnFunction( const s32 index )
{
  if( !IsAvailable( ) ){
    return 0;
  }
  cell ret;
  s32 err;
  err = amx_Exec( GetAmx( ), &ret, index );
  while( err == AMX_ERR_SLEEP ){ 
    err = amx_Exec( GetAmx( ), &ret, AMX_EXEC_CONT );
  }
  PAWN_ASSERT(err,"PawnExec Error");

  return ret;
}


//------------------------------------------------------------------------------
/**
 * @func    GetIndex
 * @brief   関数名から関数のインデックスを取得
 * @date    2014/10/23(木) 19:49:20
 *
 * @param[in] const c8 * const  com 関数名文字列.
 *
 * @return    s32 インデックス(無かったら-1)
 */
//------------------------------------------------------------------------------
s32 gfl2::pawn::PawnBase::GetIndex( const c8 * const com )
{
  int index;  ///< ★★s32 -> int に変更
  
  s32 err = amx_FindPublic( GetAmx( ), com, &index );
//  PAWN_ASSERT(err,"GetIndex Error");
  if( err ) index = -1;
  
  return index;
}

//==============================================================================
//==============================================================================
cell * gfl2::pawn::PawnBase::GetGlobalParameterPointer( const c8 * const str )
{
#if GFL_PAWN_DEBUG
  if(!GetAmx()->base ){
    return 0;
  }
#endif  
  cell amx_addr;
  cell* phys_addr;

  s32 err = amx_FindPubVar( GetAmx( ), str, &amx_addr );
  if( err != AMX_ERR_NONE ){
    return 0;
  }

  //  GFL_ASSERT_MSG( err == AMX_ERR_NONE, "Pawn Could not Find Parameter %s !! Wrong spell or optimized(Pawn removes unused parameter)?\n", str, amx_addr );
  amx_GetAddr( GetAmx( ), amx_addr, &phys_addr );
  return phys_addr;
}

//==============================================================================
//==============================================================================
s32 gfl2::pawn::PawnBase::ExecuteStep( void )
{
  if(m_HaltFlag){
    return AMX_ERR_SLEEP;
  }
  
  s32 ret;
  m_ExecuteReturnValue = ret = amx_Exec( GetAmx( ), /*&m_PawnReturnValue*/0,
                 ( m_ExecuteReturnValue == AMX_ERR_SLEEP ) ? AMX_EXEC_CONT : AMX_EXEC_MAIN );
  
  PAWN_ASSERT(ret,"Execute Error");
  
  return ret;
}
// ----------------------------------------------------------------
/// DebugContinue コンティニュー
// ----------------------------------------------------------------
s32 gfl2::pawn::PawnBase::Execute()
{
  for(;; ){ ///< continue の挙動が変？なので do while が使えない・・・
    s32 ret = ExecuteStep();

    /// １フレームの処理が終了した場合
    if( ret == GFL_PAWN_FINISHED || ret == AMX_ERR_SLEEP ){
      //      gfl2::core::Debug::PrintConsole( "ret %s %s\n", old_func,m_pCurrentFunction);
      return ret;
    }

    /// サスペンドしたフレームをスキップするため
    if(m_SuspendFrame){
      --m_SuspendFrame;
      return GFL_PAWN_SUSPENDING;
    }
  }
}

//==============================================================================
//==============================================================================
s32 gfl2::pawn::PawnBase::Update( void )
{
  if( IsAvailable( ) ){
    s32 f = m_SuspendFrame;
    if( f ){
      m_SuspendFrame = --f;
      return GFL_PAWN_SUSPENDING;
    }
    return Execute();
//    return m_PawnReturnValue;
  }
  return GFL_PAWN_NOT_AVAILABLE;
}
