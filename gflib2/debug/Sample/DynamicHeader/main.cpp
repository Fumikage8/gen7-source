// gfl2
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


#define GFL_ENABLE_DYNAMIC_HEADER 1

#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

// ヘッダファイルパスのルートパス
#define SAMPLE_DYNAMIC_HEADER_ROOT "R:/home/niji/git_program/gflib2/debug/Sample/DynamicHeader/"


#define SAMPLE_HEADER_FILEPATH "R:/home/niji/git_program/gflib2/debug/Sample/DynamicHeader/dumy_enum.h"

// 動的にロードするヘッダのインクルード
//  動的ロードが有効な場合は指定したファイルを、
//  無効な時はダミーファイルをインクルードする
#include GFL_DYNAMIC_HEADER(SAMPLE_HEADER_FILEPATH)

// ヒープ使用量警告の閾値
enum
{
  AleartHeapLimit = 0x300000
};

void SampleMain()
{
  // ヒープを用意
  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId(gfl2::heap::HEAPID_SYSTEM);

	// 動的ヘッダシステムの初期化
	// ヒープ. 最大同時ロード数. 警告の閾値, ヘッダファイルパスのルートパスを指定
  // ゲーム起動時に一度実行すればよい
  GFL_DYNAMIC_HEADER_MANAGER_INITIALIZE(pHeap, 16, AleartHeapLimit, SAMPLE_DYNAMIC_HEADER_ROOT);

	// ヘッダファイルのロード
	// ファイルパスと同時にテーブルのキャパシティも指定する必要がある
	GFL_LOAD_HEADER_HASH(SAMPLE_HEADER_FILEPATH, 32);

	// 値の取得
	GFL_PRINT("define_value = %d\n",  GFL_VALUE_DEFINE_IN_HEADER(FILE_32_TEST_VAL_2) );

	// ロード済みのファイルパスを指定すると高速化される見込み
	GFL_PRINT("define_value = %d\n",  GFL_VALUE_DEFINE_IN_HEADER_FAST(FILE_32_TEST_VAL_2, SAMPLE_HEADER_FILEPATH));

	// 現在ロードされているヘッダの内容を出力する
	GFL_DYNAMIC_HEADER_DUMP();

	// ファイルのアンロード
	GFL_UNLOAD_HEADER_HASH(SAMPLE_HEADER_FILEPATH);


	// 動的ヘッダシステムの終了処理
	GFL_DYNAMIC_HEADER_MANAGER_FINALIZE();
}


// 以下、直接関係ない事務的処理

#define SYSTEM_HEAP_SIZE 0x0cc0000 + 0x30000
#define HEAP_CHILD_MAX 16

#ifdef GF_PLATFORM_CTR

#include <nn/os.h>
#include <nn/dbg.h>

void HeapInit()
{
  nn::Result result = nn::os::SetHeapSize(SYSTEM_HEAP_SIZE);
  NN_UTIL_PANIC_IF_FAILED( result );

  size_t  systemHeapSize =  nn::os::GetHeapSize();
  void*   systemHeapAdrs = reinterpret_cast<void*>(nn::os::GetHeapAddress());

  gfl2::heap::Manager::Initialize( systemHeapAdrs, (int)systemHeapSize, HEAP_CHILD_MAX );

  gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
}

void HeapFinalize()
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM ));

  gfl2::heap::Manager::Finalize();
}

#else

#include <stdio.h>

void* addr_heap;
void HeapInit()
{
  size_t systemHeapTotalSize = SYSTEM_HEAP_SIZE;

  addr_heap = malloc(systemHeapTotalSize);

  gfl2::heap::Manager::Initialize(addr_heap, systemHeapTotalSize, HEAP_CHILD_MAX);

  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
}

void HeapFinalize()
{
  gfl2::heap::Manager::DeleteHeap(gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM ));

  gfl2::heap::Manager::Finalize();

  free(addr_heap);
  addr_heap = NULL;
}

#endif

#ifdef GF_PLATFORM_CTR

extern "C" void nninitStartUp(void) // 標準のメモリ割り当てをつぶす
{
}

extern "C" void nnMain()
{
  HeapInit();

  SampleMain();

  HeapFinalize();
}

#else  //#ifdef GF_PLATFORM_CTR

extern "C" void main()
{
  HeapInit();

  SampleMain();

  HeapFinalize();
}

#endif //#ifdef GF_PLATFORM_CTR
