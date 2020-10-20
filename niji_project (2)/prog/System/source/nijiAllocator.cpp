//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		nijiAllocator.cpp
 *	@brief  niji用アロケーター   GFHeapAllocator にVRAM管理処理を追加したもの
 *	@author	tomoya takahashi
 *	@date		2015.05.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "System/include/nijiAllocator.h"

GFL_NAMESPACE_BEGIN(System);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------


// 常駐VRAM領域管理
gfl2::util::AreaMan*	nijiAllocator::m_pResidentVramA = NULL;
gfl2::util::AreaMan*	nijiAllocator::m_pResidentVramB = NULL;
gfl2::util::AreaMan::ResultCode* nijiAllocator::m_pResultResidentVramA = NULL;
gfl2::util::AreaMan::ResultCode* nijiAllocator::m_pResultResidentVramB = NULL;
// アプリVRAM領域管理
gfl2::util::AreaMan*	nijiAllocator::m_pApplicationVramA = NULL;
gfl2::util::AreaMan*	nijiAllocator::m_pApplicationVramB = NULL;
gfl2::util::AreaMan::ResultCode* nijiAllocator::m_pResultApplicationVramA = NULL;
gfl2::util::AreaMan::ResultCode* nijiAllocator::m_pResultApplicationVramB = NULL;

#if PM_DEBUG
bool nijiAllocator::m_IsDebugResidentVramAllocCheck = false; //! 常駐VRAM領域からの確保を監視
#endif
/**
*  @brief  初期化
*/
void nijiAllocator::Initialize( gfl2::heap::HeapBase* pHeap )
{
  m_pResidentVramA = GFL_NEW(pHeap) gfl2::util::AreaMan(GetBlockSize(RESIDENT_VRAMA_SIZE), pHeap );
  m_pResidentVramB = GFL_NEW(pHeap) gfl2::util::AreaMan( GetBlockSize(RESIDENT_VRAMB_SIZE), pHeap );
  m_pResultResidentVramA = reinterpret_cast<gfl2::util::AreaMan::ResultCode*>(GflHeapAllocMemory( pHeap, VRAM_ALLOCK_BLOCK_MAX * sizeof(gfl2::util::AreaMan::ResultCode) ));
  m_pResultResidentVramB = reinterpret_cast<gfl2::util::AreaMan::ResultCode*>(GflHeapAllocMemory( pHeap, VRAM_ALLOCK_BLOCK_MAX * sizeof(gfl2::util::AreaMan::ResultCode) ));
  for( u32 i=0; i<VRAM_ALLOCK_BLOCK_MAX; ++i ){ m_pResultResidentVramA[i] = static_cast<gfl2::util::AreaMan::ResultCode>(gfl2::util::AreaMan::RESULT_FAILED); }
  for( u32 i=0; i<VRAM_ALLOCK_BLOCK_MAX; ++i ){ m_pResultResidentVramB[i] = static_cast<gfl2::util::AreaMan::ResultCode>(gfl2::util::AreaMan::RESULT_FAILED); }
 

  u32 vramA_size = GetBlockSize(VRAMA_SIZE);
  u32 vramB_size = GetBlockSize(VRAMB_SIZE);
  m_pApplicationVramA = GFL_NEW(pHeap) gfl2::util::AreaMan( vramA_size - GetBlockSize(RESIDENT_VRAMA_SIZE), pHeap );
  m_pApplicationVramB = GFL_NEW(pHeap) gfl2::util::AreaMan( vramB_size - GetBlockSize(RESIDENT_VRAMB_SIZE), pHeap );
  m_pResultApplicationVramA = reinterpret_cast<gfl2::util::AreaMan::ResultCode*>(GflHeapAllocMemory( pHeap, VRAM_ALLOCK_BLOCK_MAX * sizeof(gfl2::util::AreaMan::ResultCode) ));
  m_pResultApplicationVramB = reinterpret_cast<gfl2::util::AreaMan::ResultCode*>(GflHeapAllocMemory( pHeap, VRAM_ALLOCK_BLOCK_MAX * sizeof(gfl2::util::AreaMan::ResultCode) ));
  for( u32 i=0; i<VRAM_ALLOCK_BLOCK_MAX; ++i ){ m_pResultApplicationVramA[i] = static_cast<gfl2::util::AreaMan::ResultCode>(gfl2::util::AreaMan::RESULT_FAILED); }
  for( u32 i=0; i<VRAM_ALLOCK_BLOCK_MAX; ++i ){ m_pResultApplicationVramB[i] = static_cast<gfl2::util::AreaMan::ResultCode>(gfl2::util::AreaMan::RESULT_FAILED); }
}

/**
 *  @brief  破棄
 */
void nijiAllocator::Terminate( void )
{
#if PM_DEBUG
  u32 vramA_size = GetBlockSize(VRAMA_SIZE);
  u32 vramB_size = GetBlockSize(VRAMB_SIZE);
  // 解放忘れチェック
  GFL_ASSERT( m_pResidentVramA->GetContiniusFreeBlockNum() == GetBlockSize(RESIDENT_VRAMA_SIZE) );
  GFL_ASSERT( m_pResidentVramB->GetContiniusFreeBlockNum() == GetBlockSize(RESIDENT_VRAMB_SIZE) );
  GFL_ASSERT( m_pApplicationVramA->GetContiniusFreeBlockNum() == vramA_size - GetBlockSize(RESIDENT_VRAMA_SIZE) );
  GFL_ASSERT( m_pApplicationVramB->GetContiniusFreeBlockNum() == vramB_size - GetBlockSize(RESIDENT_VRAMB_SIZE) );
#endif // PM_DEBUG

  GFL_DELETE(m_pResidentVramA);
  GFL_DELETE(m_pResidentVramB);
  GflHeapSafeFreeMemory(m_pResultResidentVramA);
  GflHeapSafeFreeMemory(m_pResultResidentVramB);
  GFL_DELETE(m_pApplicationVramA);
  GFL_DELETE(m_pApplicationVramB);
  GflHeapSafeFreeMemory(m_pResultApplicationVramA);
  GflHeapSafeFreeMemory(m_pResultApplicationVramB);
}

/**
 *  @brief  常駐のVRAM領域情報を取得
 */
void nijiAllocator::GetResident( VramArea* desc )			// 常駐で利用するVRAM領域情報
{
#if defined(GF_PLATFORM_CTR)
  desc->startAddrA = reinterpret_cast<void*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMA) + RESIDENT_VRAMA_START);
  desc->startAddrB = reinterpret_cast<void*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMB) + RESIDENT_VRAMB_START);
  desc->vramA      = m_pResidentVramA;
  desc->vramB      = m_pResidentVramB;
  desc->sizeA      = GetBlockSize(RESIDENT_VRAMA_SIZE) * AREA_MAN_BLOCK_SIZE;
  desc->sizeB      = GetBlockSize(RESIDENT_VRAMB_SIZE) * AREA_MAN_BLOCK_SIZE;
  desc->resultA    = m_pResultResidentVramA;
  desc->resultB    = m_pResultResidentVramB;
#elif defined(GF_PLATFORM_WIN32)
  desc->startAddrA = 0;
  desc->startAddrB = 0;
  desc->vramA      = m_pResidentVramA;
  desc->vramB      = m_pResidentVramB;
  desc->sizeA      = GetBlockSize(RESIDENT_VRAMA_SIZE) * AREA_MAN_BLOCK_SIZE;
  desc->sizeB      = GetBlockSize(RESIDENT_VRAMB_SIZE) * AREA_MAN_BLOCK_SIZE;
  desc->resultA    = m_pResultResidentVramA;
  desc->resultB    = m_pResultResidentVramB;
#endif 
}

/**
 *  @brief  アプリ用のVRAM領域情報を取得
 */
void nijiAllocator::GetApplication( VramArea* desc ) // アプリで利用するVRAM領域情報　アプリはこちらのdescで初期化
{
#if defined(GF_PLATFORM_CTR)
  desc->startAddrA = reinterpret_cast<void*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMA) + RESIDENT_VRAMA_START + (GetBlockSize(RESIDENT_VRAMA_SIZE)*AREA_MAN_BLOCK_SIZE));
  desc->startAddrB = reinterpret_cast<void*>(nngxGetVramStartAddr(NN_GX_MEM_VRAMB) + RESIDENT_VRAMB_START + (GetBlockSize(RESIDENT_VRAMB_SIZE)*AREA_MAN_BLOCK_SIZE));
  desc->vramA      = m_pApplicationVramA;
  desc->vramB      = m_pApplicationVramB;
  desc->sizeA      = (GetBlockSize(VRAMA_SIZE)- GetBlockSize(RESIDENT_VRAMA_SIZE)) * AREA_MAN_BLOCK_SIZE;
  desc->sizeB      = (GetBlockSize(VRAMB_SIZE)- GetBlockSize(RESIDENT_VRAMB_SIZE)) * AREA_MAN_BLOCK_SIZE;
  desc->resultA    = m_pResultApplicationVramA;
  desc->resultB    = m_pResultApplicationVramB;
#elif defined(GF_PLATFORM_WIN32)
  desc->startAddrA = 0;
  desc->startAddrB = 0;
  desc->vramA      = m_pApplicationVramA;
  desc->vramB      = m_pApplicationVramB;
  desc->sizeA      = (GetBlockSize(VRAMA_SIZE)- GetBlockSize(RESIDENT_VRAMA_SIZE)) * AREA_MAN_BLOCK_SIZE;
  desc->sizeB      = (GetBlockSize(VRAMB_SIZE)- GetBlockSize(RESIDENT_VRAMB_SIZE)) * AREA_MAN_BLOCK_SIZE;
  desc->resultA    = m_pResultApplicationVramA;
  desc->resultB    = m_pResultApplicationVramB;
#endif 
}

#if PM_DEBUG
//!  VRAMの使用状況をダンプする
void nijiAllocator::DebugVramMemoryInfoDump()
{
  VramArea vramArea;
  GetResident( &vramArea );
  GFL_PRINT( "Resident VRAM A 総サイズ[0x%x] 確保可能 連続領域最大サイズ[0x%x]\n", vramArea.sizeA, vramArea.vramA->GetContiniusFreeBlockNum() * AREA_MAN_BLOCK_SIZE );
  GFL_PRINT( "Resident VRAM B 総サイズ[0x%x] 確保可能 連続領域最大サイズ[0x%x]\n", vramArea.sizeB, vramArea.vramB->GetContiniusFreeBlockNum() * AREA_MAN_BLOCK_SIZE );

  GetApplication( &vramArea );
  GFL_PRINT( "Application VRAM A 総サイズ[0x%x] 確保可能 連続領域最大サイズ[0x%x]\n", vramArea.sizeA, vramArea.vramA->GetContiniusFreeBlockNum() * AREA_MAN_BLOCK_SIZE );
  GFL_PRINT( "Application VRAM B 総サイズ[0x%x] 確保可能 連続領域最大サイズ[0x%x]\n", vramArea.sizeB, vramArea.vramB->GetContiniusFreeBlockNum() * AREA_MAN_BLOCK_SIZE );

}

//!  ApplicationのVRAM領域がクリアな常態かチェックする。
void nijiAllocator::DebugCheckApplicationVramClear()
{
  VramArea vramArea;
  GetApplication( &vramArea );
  GFL_ASSERT_MSG( (vramArea.vramA->GetContiniusFreeBlockNum() == GetBlockSize(vramArea.sizeA)), "\nVRAM A LEAK %d block\n", GetBlockSize(vramArea.sizeA) - vramArea.vramA->GetContiniusFreeBlockNum() );
  GFL_ASSERT_MSG( (vramArea.vramB->GetContiniusFreeBlockNum() == GetBlockSize(vramArea.sizeB)), "\nVRAM B LEAK %d block\n", GetBlockSize(vramArea.sizeB) - vramArea.vramB->GetContiniusFreeBlockNum()  );
}

//! これ以降常駐VRAMからの確保を監視する。
void nijiAllocator::DebugSetResidentVramCheck()
{
  m_IsDebugResidentVramAllocCheck = true;
}
#endif // PM_DEBUG

/**
 * @brief nijiAllocatorコンストラクタ
 */
nijiAllocator::nijiAllocator( gfl2::heap::HeapBase *devHeap, VramControlType controlType, VramMallocMode mode ) : 
  gfl2::util::GLHeapAllocator(devHeap),
  m_VramControlType(controlType),
  m_MallocMode(mode)
{
  if( m_VramControlType == RESIDENT )
  {
    GetResident( &m_Vram );
  }
  else
  {
    GetApplication( &m_Vram );
  }
}

/**
 * @brief nijiAllocatorコンストラクタ
 */
nijiAllocator::nijiAllocator( gfl2::heap::HeapBase *devHeap ) : 
  gfl2::util::GLHeapAllocator(devHeap),
  m_VramControlType(APPLICATION),
  m_MallocMode(VRAM_MALLOC_AB)
{
  if( m_VramControlType == RESIDENT )
  {
    GetResident( &m_Vram );
  }
  else
  {
    GetApplication( &m_Vram );
  }
}

/**
 * @brief nijiAllocatorデストラクタ
 */
nijiAllocator::~nijiAllocator()
{
}

//! VRAMからメモリ確保     外部から確保先指定された場合
void* nijiAllocator::LocalMemoryMalloc(gfl2::gfx::Pool allocation, u32 size, u32 alignment)
{
  switch( allocation )
  {
  case gfl2::gfx::Pool::Default:	//!< 最適なメモリ
    return LocalMemoryMalloc( size, alignment );

  case gfl2::gfx::Pool::System:		 //!< メインメモリ
    GFL_ASSERT_STOP(0); // LocalMemoryなのにmainメモリというのは不明
    break;

  case gfl2::gfx::Pool::Managed:		//!< 必要に応じてメモリ配置を変更します
    return LocalMemoryMalloc( size, alignment );

  case gfl2::gfx::Pool::LocalA:			//!< ３ＤＳ用
    {
      if((m_MallocMode == VRAM_MALLOC_A) || (m_MallocMode == VRAM_MALLOC_AB))
      {
        return AllocVramArea( &m_Vram, size, alignment, true );
      }
      else
      {
        GFL_ASSERT_STOP(0); // VRAM_Aから確保しないモードなのに、VRAMAに対してのMalloc要求が来た。
      }
    }
    break;

  case gfl2::gfx::Pool::LocalB:			//!< ３ＤＳ用
    {
      if((m_MallocMode == VRAM_MALLOC_B) || (m_MallocMode == VRAM_MALLOC_AB))
      {
        return AllocVramArea( &m_Vram, size, alignment, false );
      }
      else
      {
        GFL_ASSERT_STOP(0); // VRAM_Bから確保しないモードなのに、VRAMBに対してのMalloc要求が来た。
      }
    }
    break;

  default:
    GFL_ASSERT_STOP(0); // しらない引数のタイプ
    break;
  }

  return NULL;
}

/**
 *  @brief  VRAMからのメモリ確保
 */
void* nijiAllocator::LocalMemoryMalloc(u32 size, u32 alignment)
{
#if PM_DEBUG
  if( m_IsDebugResidentVramAllocCheck ){
    GFL_ASSERT( m_VramControlType == APPLICATION );  
  }
#endif // PM_DEBUG
  
#if defined(GF_PLATFORM_CTR)

  switch( m_MallocMode )
  {
  case VRAM_MALLOC_A:		//! Aからしか確保しない
    return AllocVramArea( &m_Vram, size, alignment, true );
  case VRAM_MALLOC_B:		//! Bからしか確保しない
    return AllocVramArea( &m_Vram, size, alignment, false );
  case VRAM_MALLOC_AB:		//! ABの空き領域が多いほうから確保
    if( CompVramFreeSize( m_Vram ) ){
      // VRAMAの方が空きが多い
      return AllocVramArea( &m_Vram, size, alignment, true );
    }
    // VRAMBの方が空きが多い
    return AllocVramArea( &m_Vram, size, alignment, false );
  default:
    GFL_ASSERT_STOP(0);
    return NULL;
  }
#elif defined(GF_PLATFORM_WIN32)
  GFL_ASSERT_STOP(0); // 非対応
  return NULL;
#endif 
}

/**
 *  @brief  VRAMからのメモリ解放
 */
void nijiAllocator::LocalMemoryFree(void* const address)
{
#if defined(GF_PLATFORM_CTR)
  // アドレスから解放すべきVRAMを確定
  if( IsVramArea(m_Vram, reinterpret_cast<uptr>(address), true) ){
    FreeVramArea(&m_Vram, address, true);
  }
  else if( IsVramArea(m_Vram, reinterpret_cast<uptr>(address), false) )
  {
    FreeVramArea(&m_Vram, address, false);
  }
  else
  {
    // 現在管理しているVRAMから確保されたアドレスではない。
    GFL_ASSERT_STOP(0);
  }
#elif defined(GF_PLATFORM_WIN32)
  GFL_ASSERT_STOP(0); // 非対応
#endif 
}


/**
 * @brief VRAM確保モードを設定
 * @param mode  VRAM確保モード
 */
void nijiAllocator::SetVramMallocMode( VramMallocMode mode )
{
  m_MallocMode = mode;
}

//! VRAM エリアにアドレスが含まれるかチェック
bool nijiAllocator::IsVramArea( const VramArea& area, uptr address, bool A )
{
  uptr startAddress = 0;
  u32 size;

  if( A )
  {
    startAddress  = reinterpret_cast<uptr>(area.startAddrA);
    size          = area.sizeA;
  }
  else
  {
    startAddress  = reinterpret_cast<uptr>(area.startAddrB);
    size          = area.sizeB;
  }

  address = address - startAddress;
  if( (0 <= address) && (address < size) ){
    return true; 
  }
  return false;
}

//! VRAMの領域確保
void * nijiAllocator::AllocVramArea( VramArea* p_area, u32 size, u32 alignment, bool A )
{
  uptr startAddress = 0;
  u32 vram_size;
  gfl2::util::AreaMan* pAreaMan = NULL;

  if( A )
  {
    startAddress  = reinterpret_cast<uptr>(p_area->startAddrA);
    pAreaMan      = p_area->vramA;
    vram_size     = p_area->sizeA;
  }
  else
  {
    startAddress  = reinterpret_cast<uptr>(p_area->startAddrB);
    pAreaMan      = p_area->vramB;
    vram_size     = p_area->sizeB;
  }

  // 空いているワークを取得
  gfl2::util::AreaMan::ResultCode* p_clearWk = GetClearResultTbl( p_area, A );

  // アライメント設定
  pAreaMan->SetAlign( GetBlockAlign(alignment) );

  // 確保
  *p_clearWk = pAreaMan->ReserveArea( GetBlockSize(size), 0, GetBlockSize(vram_size) );

  uptr addr = pAreaMan->ResCodeToBlockPos( *p_clearWk );
  addr *= AREA_MAN_BLOCK_SIZE;  // blockIndexからbyteアドレスに変更

  if( *p_clearWk == gfl2::util::AreaMan::RESULT_FAILED ){
    // サイズオーバー
    GFL_ASSERT_MSG( 0, "VRAMA over" );
    addr = 0; // 強制的に転送
  }

  addr += startAddress;

  return reinterpret_cast<void*>(addr);
}

//! VRAMの領域解放
void nijiAllocator::FreeVramArea( VramArea* p_area, void* const address, bool A )
{
  uptr startAddress = 0;
  gfl2::util::AreaMan* pAreaMan = NULL;

  if( A )
  {
    startAddress  = reinterpret_cast<uptr>(p_area->startAddrA);
    pAreaMan      = p_area->vramA;
  }
  else
  {
    startAddress  = reinterpret_cast<uptr>(p_area->startAddrB);
    pAreaMan      = p_area->vramB;
  }


  uptr addr = reinterpret_cast<uptr>(address);
  addr -= startAddress;
  addr /= AREA_MAN_BLOCK_SIZE;  // byteアドレスからブロックインデックスに変換

  gfl2::util::AreaMan::ResultCode* p_addrWk = GetAddrResultTbl( p_area, addr, A );
  if( p_addrWk == NULL ){
    // 不明なアドレス。
    GFL_ASSERT( 0 );
  }else{
    pAreaMan->Release( *p_addrWk );
    *p_addrWk = static_cast<gfl2::util::AreaMan::ResultCode>(gfl2::util::AreaMan::RESULT_FAILED); // 初期化
  }

}

//! VRAM空き領域の多いほうを返す
bool nijiAllocator::CompVramFreeSize( const VramArea& area )
{
  // VramA優先
  if( area.vramA->GetContiniusFreeBlockNum() >= area.vramB->GetContiniusFreeBlockNum() )
  {
    return true;
  }
  return false;
}


//! ブロックアライメントを取得
u32 nijiAllocator::GetBlockAlign( u32 alignment )
{
  u32 block_align = 1;

  if( alignment > AREA_MAN_BLOCK_SIZE ){
    block_align = alignment / AREA_MAN_BLOCK_SIZE;
    // あまりがあれば、繰上げ
    if( (alignment % AREA_MAN_BLOCK_SIZE) != 0 ){
      block_align += 1;
    }
  }
  return block_align;

}

//! ブロックサイズを取得
u32 nijiAllocator::GetBlockSize( u32 size )
{
  u32 block_size = 1;

  if( size >= AREA_MAN_BLOCK_SIZE ){
    block_size = size / AREA_MAN_BLOCK_SIZE;
    if( (size % AREA_MAN_BLOCK_SIZE) > 0 ){
      block_size += 1; // 繰上げ
    }
  }
  return block_size;
}

/**
 *  @brief  空いているリザルトテーブルを取得
 */
gfl2::util::AreaMan::ResultCode * nijiAllocator::GetClearResultTbl( VramArea* p_area, bool A )
{
  gfl2::util::AreaMan::ResultCode* pResult = NULL;
  u32 block_size = VRAM_ALLOCK_BLOCK_MAX;

  if( A )
  {
    pResult       = p_area->resultA;
  }
  else
  {
    pResult       = p_area->resultB;
  }

  for( u32 i=0; i<block_size; ++i )
  {
    if( pResult[i] == gfl2::util::AreaMan::RESULT_FAILED )
    {
      return &pResult[i];
    }
  }

  // 確保するためのワークがない＝もう確保できない状況
  GFL_ASSERT_STOP(0);
  return NULL;
}

/**
 *  @brief  アドレスのリザルトテーブルを取得
 */
gfl2::util::AreaMan::ResultCode * nijiAllocator::GetAddrResultTbl( VramArea* p_area, uptr addr, bool A )
{
  u32 start_block = addr;
  u32 block_size = VRAM_ALLOCK_BLOCK_MAX;
  gfl2::util::AreaMan* pAreaMan = NULL;
  gfl2::util::AreaMan::ResultCode* pResult = NULL;

  if( A )
  {
    pAreaMan      = p_area->vramA;
    pResult       = p_area->resultA;
  }
  else
  {
    pAreaMan      = p_area->vramB;
    pResult       = p_area->resultB;
  }

  for( u32 i=0; i<block_size; i++ ){
    if( pAreaMan->ResCodeToBlockPos(pResult[i]) == start_block ){
      return &pResult[i];
    }
  }
  // 確保できるテーブルなし
  GFL_ASSERT_STOP(0);
  return NULL;

}




nijiResidentAllocator::nijiResidentAllocator( gfl2::heap::HeapBase *devHeap, VramControlType controlType, VramMallocMode mode ) : nijiAllocator( devHeap, controlType, mode ),
  m_AllocSize(0)
{

}

nijiResidentAllocator::nijiResidentAllocator( gfl2::heap::HeapBase *devHeap ) : nijiAllocator( devHeap ),
  m_AllocSize(0)
{

}

nijiResidentAllocator::~nijiResidentAllocator()
{
}

#if GFL_DEBUG
void* nijiResidentAllocator::_SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum)
{
  return nijiAllocator::_SystemMemoryMallocAlign( size, alignment, filename, linenum);
}
#else
void* nijiResidentAllocator::SystemMemoryMallocAlign(u32 size, u32 alignment)
{
  return nijiAllocator::SystemMemoryMallocAlign( size , alignment );
}
#endif

void nijiResidentAllocator::SystemMemoryFree(void* const address)
{
  nijiAllocator::SystemMemoryFree(address);
}

GFL_NAMESPACE_END(System);
