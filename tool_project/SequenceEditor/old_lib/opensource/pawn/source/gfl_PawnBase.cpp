//==============================================================================
/**
 * @file	gfl_Pawn.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/02/02, 12:34
 */
// =============================================================================

#include "gfl_PawnBase.h"

//==============================================================================
//==============================================================================
#if GFL_PAWN_DEBUG
const c8 * const gfl::pawn::PawnBase::s_cErrorMessage[] = {
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
	gfl::pawn::PawnBase* pt = gfl::pawn::PawnBase::GetClass<gfl::pawn::PawnBase*>( amx );
	pt->SetSuspendFrame( val[1] - 1 );
	return 0;
}

static const GFL_PAWN_FUNC_TABLE s_PawnBaseFuncTable[] = {
	GFL_PAWN_FUNC( _Suspend )
	GFL_PAWN_FUNC_END
};

//==============================================================================
//==============================================================================
gfl::pawn::PawnBase::PawnBase( gfl::heap::HeapBase* heap )
: m_pBuffer( 0 ), m_pHeap( heap ), m_SuspendFrame(0)
{
	InitializeAmx( );
}

//==============================================================================
//==============================================================================
gfl::pawn::PawnBase::~PawnBase( void )
{
	ClearAmx( );
}

//==============================================================================
//==============================================================================
void gfl::pawn::PawnBase::InitializeAmx( void )
{
	// このクリアをしないとエラーになるので絶対必要
	gfl::std::MemFill( GetAmx( ), 0x00, sizeof(AMX ) );
	m_HaltFlag = 0;
	m_ExecuteReturnValue = AMX_ERR_NONE;
}
//==============================================================================
//==============================================================================
void gfl::pawn::PawnBase::ClearAmx( void )
{
	AMX* amx = GetAmx( );
	if(amx->base){
		amx_Cleanup( amx );
		GFL_DELETE_ARRAY amx->base;
		amx->base = 0;
		m_SuspendFrame = 0;
	}
}

//==============================================================================
//==============================================================================
//void gfl::pawn::PawnBase::Initialize( const size_t size )
//{	
//}

//==============================================================================
//==============================================================================
void gfl::pawn::PawnBase::Load(const void* buffer, const size_t size, const c8* )
{
	ClearAmx( );
	InitializeAmx( );
    
	AMX* amx = GetAmx( );

	// ヘッダ情報を取得
	const AMX_HEADER* hdr = reinterpret_cast < const AMX_HEADER* >( buffer );

#ifdef GFL_LITTLE_ENDIAN
	GFL_ASSERT_MSG( hdr->magic == AMX_MAGIC, "This is not AMX buffer...\n" );
	const u32 total_size = hdr->stp;
#else
#    if GFL_PAWN_DEBUG
	GFL_ASSERT_MSG( EndianSwap16( hdr->magic ) == AMX_MAGIC, "This is not AMX buffer...\n" );
#    endif
	const u32 total_size = EndianSwap32( hdr->stp );
#endif

	gfl::heap::HeapBase* heap = m_pHeap;

	// スタックを含めたサイズのメモリを確保する
	u8* adr = GFL_NEW_ARRAY( heap ) u8 [total_size];
	// スタッククリア
	gfl::std::MemFill( adr + size, 0x00, total_size - size );
	// データコピー
	gfl::std::MemCopy( buffer, adr, size );

	// 初期化
#if GFL_PAWN_DEBUG
	s32 err =
#endif
		amx_Init( amx, adr );

	PAWN_ASSERT( err, "Pawn Initialize Error" );

#if GFL_PAWN_CORE
	amx_CoreInit( amx );
#endif
#if GFL_PAWN_PRINT
	amx_PrintInit( amx );
#endif
	amx_FloatInit( amx );

	RegisterPawnFunction( s_PawnBaseFuncTable );

	m_pBuffer = buffer;
  m_BufferSize = size;

//	SetHook();
}

//==============================================================================
//==============================================================================
void gfl::pawn::PawnBase::LoadShare(const PawnBase* org)
{
	ClearAmx( );
	InitializeAmx( );
    
	AMX* amx = GetAmx( );
	const AMX* org_amx = org->GetAmx();

	// ヘッダ情報を取得
	const AMX_HEADER* hdr = reinterpret_cast < const AMX_HEADER* >( org_amx->base );

#ifdef GFL_LITTLE_ENDIAN
	GFL_ASSERT_MSG( hdr->magic == AMX_MAGIC, "This is not AMX buffer...\n" );
	const u32 total_size = hdr->stp - hdr->dat + hdr->cod;
#else
#    if GFL_PAWN_DEBUG
	GFL_ASSERT_MSG( EndianSwap16( hdr->magic ) == AMX_MAGIC, "This is not AMX buffer...\n" );
#    endif
	const u32 total_size = EndianSwap32( hdr->stp ) - EndianSwap32( hdr->dat ) + EndianSwap32( hdr->cod );
#endif

	gfl::heap::HeapBase* heap = m_pHeap;

	// データ、ヒープ、スタックのメモリを確保する（つまりコード以外）
	u8* adr = GFL_NEW_ARRAY( heap ) u8 [total_size];
	// スタッククリア
	gfl::std::MemFill( adr, 0x00, total_size );

	memcpy( adr, hdr, hdr->cod );
	memcpy( amx, org_amx, sizeof(AMX) );

	// ベース部分は共有出来無い
	amx->base = adr;
	// データ部分はコピーが必要
	amx->data = adr + hdr->cod;
	// データの参照ポインタを無理やり変更
	memcpy( amx->data, (void*)((size_t)hdr + hdr->dat), hdr->hea - hdr->dat );
	
	m_pBuffer = org->m_pBuffer;
	m_BufferSize = org->m_BufferSize;
//	SetHook();
}

//==============================================================================
//==============================================================================
cell gfl::pawn::PawnBase::CallPawnFunction( const s32 index )
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

//==============================================================================
//==============================================================================
s32 gfl::pawn::PawnBase::GetIndex( const c8 * const com )
{
	s32 index;
	s32 err = amx_FindPublic( GetAmx( ), com, &index );
//	PAWN_ASSERT(err,"GetIndex Error");
	if(err){
		index = -1;
	}
	return index;
}

//==============================================================================
//==============================================================================
cell * gfl::pawn::PawnBase::GetGlobalParameterPointer( const c8 * const str )
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

	//	GFL_ASSERT_MSG( err == AMX_ERR_NONE, "Pawn Could not Find Parameter %s !! Wrong spell or optimized(Pawn removes unused parameter)?\n", str, amx_addr );
	amx_GetAddr( GetAmx( ), amx_addr, &phys_addr );
	return phys_addr;
}

//==============================================================================
//==============================================================================
s32 gfl::pawn::PawnBase::ExecuteStep( void )
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
// DebugContinue	コンティニュー
// ----------------------------------------------------------------
s32 gfl::pawn::PawnBase::Execute()
{
	for(;; ){ // continue の挙動が変？なので do while が使えない・・・
		s32 ret = ExecuteStep();

		// １フレームの処理が終了した場合
		if( ret == GFL_PAWN_FINISHED || ret == AMX_ERR_SLEEP ){
			//			gfl::core::Debug::PrintConsole( "ret %s %s\n", old_func,m_pCurrentFunction);
			return ret;
		}

		// サスペンドしたフレームをスキップするため
		if(m_SuspendFrame){
			--m_SuspendFrame;
			return GFL_PAWN_SUSPENDING;
		}
	}
}

//==============================================================================
//==============================================================================
s32 gfl::pawn::PawnBase::Update( void )
{
	if( IsAvailable( ) ){
		s32 f = m_SuspendFrame;
		if( f ){
			m_SuspendFrame = --f;
			return GFL_PAWN_SUSPENDING;
		}
		return Execute();
//		return m_PawnReturnValue;
	}
	return GFL_PAWN_NOT_AVAILABLE;
}
