#ifndef __GFL_HEAPBASE_H__
#define __GFL_HEAPBASE_H__
//======================================================================
/**
 * @file  gfl_HeapBase.h
 * @brief GFLヒープ基底クラス
 * @author  ariizumi
 * @data  10/10/05
 */
//======================================================================
#pragma once
#include <nn/os.h>

#include <gfl_Common.h>
#include <heap/gfl_HeapManager.h>
#include <fs/gfl_HioSwitch.h>

#if GFL_HIO_ENABLE
#include  <debug/gfl_HioSharedMemory.h>
#endif

namespace gfl
{
namespace heap
{

class GflHeapBaseLower;

//メモリの頭につくヘッダー
//12末緊急対応！
static const int MEMORY_HEADER_SIZE = 32;
//static const int MEMORY_HEADER_SIZE = 16;
static const int MEMORY_HEADER_USER_SIZE = MEMORY_HEADER_SIZE-6;
typedef struct
{
  //ヒープID
  HEAPID  heapId;
  //メモリ先頭位置までのオフセット
  u16     offset;
  //ユーザーデータ
  u8      userData[MEMORY_HEADER_USER_SIZE];
}MEMORY_HEADER;

//メモリの頭につくヘッダーのデフォルトユーザーヘッダー
static const int MEMORY_USER_HEADER_SIZE = MEMORY_HEADER_USER_SIZE;
#if GFL_HIO_ENABLE
static const int MEMORY_USER_HEADER_FILE_BUFLEN = MEMORY_USER_HEADER_SIZE - 7;
#else
static const int MEMORY_USER_HEADER_FILE_BUFLEN = MEMORY_USER_HEADER_SIZE-3;
#endif
typedef struct
{
#if GFL_HIO_ENABLE
  //アライメント
  s16 align;
  //ラインインデックス(HIO経由のPCツールでライン削除に使うインデックス）
  u16 line_index;
#endif
  //ソースの行
  u16 line;
  //ユーザー指定ID
  u8  id;
  //ソース名
  char fileName[MEMORY_USER_HEADER_FILE_BUFLEN];
}MEMORY_USER_HEADER;

//メモリ作成時に生成されるデータ
typedef struct {
  MEMORY_USER_HEADER  sourceFile;   // 作成したソースファイル
  bool  parentFlag;                 // 親がいるか
}MEMORY_USER_CREATE_DATA;

//----------------------------------------------------------------------------
/**
 *  @brief  メモリを確保
 *          メモリの確保はこのマクロを使って行ってください。
 *          AllocMemoryBlockとAllocMemoryBlockAlignを直接使用しないでください。
 *
 *  @param  heap             HeapBaseクラス(推奨) もしくは HEAPID(呼び出す関数をオーバーロードしています)
 *  @param  const int size   確保サイズ
 *  @param  const int align  アライメント(マイナスで後方から確保)
 *
 *  @return void* 確保メモリ(0クリアされています)
 */
//-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
#define GflHeapAllocMemory(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlock(heapClass,size,__FILE__,__LINE__))
#define GflHeapAllocMemoryAlign(heapClass,size,align) (gfl::heap::GflHeapAllocMemoryBlockAlign(heapClass,size,align,__FILE__,__LINE__))
#define GflHeapAllocMemoryLow(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlockAlign(heapClass,size,(-heapClass->GetDefaultAlign()),__FILE__,__LINE__))
#define GflHeapAllocMemoryLowAlign(heapClass,size,align) (gfl::heap::GflHeapAllocMemoryBlockAlign(heapClass,size,(-align),__FILE__,__LINE__))
#define GflHeapAllocMemoryAlign4096(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlockAlign4096(heapClass,size,__FILE__,__LINE__))
#define GflHeapAllocMemoryAlign4096Lower(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlockAlign4096Lower(heapClass,size,__FILE__,__LINE__))
#else
#define GflHeapAllocMemory(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlock(heapClass,size))
#define GflHeapAllocMemoryAlign(heapClass,size,align) (gfl::heap::GflHeapAllocMemoryBlockAlign(heapClass,size,align))
#define GflHeapAllocMemoryLow(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlockAlign(heapClass,size,(-heapClass->GetDefaultAlign())))
#define GflHeapAllocMemoryLowAlign(heapClass,size,align) (gfl::heap::GflHeapAllocMemoryBlockAlign(heapClass,size,(-align)))
#define GflHeapAllocMemoryAlign4096(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlockAlign4096(heapClass,size))
#define GflHeapAllocMemoryAlign4096Lower(heapClass,size) (gfl::heap::GflHeapAllocMemoryBlockAlign4096Lower(heapClass,size))
#endif

//----------------------------------------------------------------------------
/**
 *  @brief  メモリを開放
 *          関数名統一のため用意しています
 *
 *  @param  void  開放メモリ
 */
//-----------------------------------------------------------------------------
#define GflHeapFreeMemory(buf) gfl::heap::GflHeapFreeMemoryBlock(buf)
#define GflHeapSafeFreeMemory(buf) { if(buf) { GflHeapFreeMemory(buf); (buf)=0; } }


#if GFL_HEAP_DEBUG
extern void* GflHeapAllocMemoryBlock( HeapBase *heapBase ,  const int size , const char *filename , const int linenum );
extern void* GflHeapAllocMemoryBlock( HEAPID heapId ,  const int size , const char *filename , const int linenum );
extern void* GflHeapAllocMemoryBlockAlign( HeapBase *heapBase ,  const int size , const int align , const char *filename , const int linenum );
extern void* GflHeapAllocMemoryBlockAlign( HEAPID heapId ,  const int size , const int align , const char *filename , const int linenum );
extern void* GflHeapAllocMemoryBlockAlign4096( HeapBase *heapBase ,  const int size , const char *filename , const int linenum );
extern void* GflHeapAllocMemoryBlockAlign4096Lower( HeapBase *heapBase ,  const int size , const char *filename , const int linenum );
#else
extern void* GflHeapAllocMemoryBlock( HeapBase *heapBase ,  const int size );
extern void* GflHeapAllocMemoryBlock( HEAPID heapId ,  const int size );
extern void* GflHeapAllocMemoryBlockAlign( HeapBase *heapBase ,  const int size , const int align );
extern void* GflHeapAllocMemoryBlockAlign( HEAPID heapId ,  const int size , const int align );
extern void* GflHeapAllocMemoryBlockAlign4096( HeapBase *heapBase ,  const int size );
extern void* GflHeapAllocMemoryBlockAlign4096Lower( HeapBase *heapBase ,  const int size );
#endif
extern void GflHeapFreeMemoryBlock(void* buf);

//#pragma mark [> HeapBase
class GflHeapBase
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(GflHeapBase);

  friend class Manager;
  friend class GflHeapBaseLower;
public:
  GflHeapBase();
  virtual ~GflHeapBase();

  //----------------------------------------------------------------------------
  /**
   *  @brief  メモリを開放
   *          GflHeapFreeMemoryBlockを使用してください
   *          確保を行ったHeapBaseを使って開放してください
   *
   *  @param  void  開放メモリ
   */
  //-----------------------------------------------------------------------------
  void FreeMemory( void* buf );

  //----------------------------------------------------------------------------
  /**
   *  @brief  自動で親ヒープを判断しメモリを開放
   *          GflHeapFreeMemoryBlockを使用してください
   *
   *  @param  void  開放メモリ
   */
  //-----------------------------------------------------------------------------
  static void FreeMemoryAuto( void* buf );

  //----------------------------------------------------------------------------
  /**
   *  @brief  確保したメモリのリサイズ
   *
   *  @param  void* buf リサイズするメモリ
   *  @param  int       変更後のサイズ
   */
  //-----------------------------------------------------------------------------
  virtual void ResizeMemory( void* buf , const int newSize){GFL_UNUSED(buf);GFL_UNUSED(newSize);};

  //----------------------------------------------------------------------------
  /**
   *  @brief  メモリ状況の表示
   *          独自で用意したMemoryHeaderを元にダンプします
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void Dump(void){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  メモリ状況の表示
   *          独自で用意したMemoryHeaderを元にダンプします。　指定したIDの情報のみダンプする版
   *
   *  @param  id   SetDebugUserID でしていていたID
   */
  //-----------------------------------------------------------------------------
  virtual void DumpId(u8 /*id*/){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  メモリ状況の表示
   *          ライブラリなどで用意された関数を使いダンプします
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void SystemDump(void){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープ破壊チェック
   *
   *  @param  none
   *  @return  bool true壊れていない false壊れている
   */
  //-----------------------------------------------------------------------------
  virtual bool Check(void){return true;};

  //----------------------------------------------------------------------------
  /**
   *  @brief  デフォルトアライメント値の取得
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual int GetDefaultAlign(void) = 0;

  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPIDの取得
   *
   *  @return  HEAPID
   */
  //-----------------------------------------------------------------------------
  virtual HEAPID GetHeapId(void) const{ return m_heapId; };

  //----------------------------------------------------------------------------
  /**
   *  @brief  アライメントを渡しヘッダー込みで実際のメモリ確保量を計算
   *          32バイト以上のアライメントを設定した場合、そのサイズ分ヘッダーが
   *          確保されます。
   *
   *  @return  int 実確保サイズ
   */
  //-----------------------------------------------------------------------------
  int CalcAllocateAdjustSize( const int align ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープのサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalSize(void){return 0;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの合計空きサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalFreeSize(void){return 0;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープで確保できる最大のサイズを取得します
   *
   *  @return  int サイズ
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalAllocatableSize(void){return 0;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの先頭アドレスを返します(管理領域を含まない
   *
   *  @return  void*
   */
  //-----------------------------------------------------------------------------
  virtual void* GetStartAddress(void){return NULL;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープを常に後方確保するハンドルを取得します
   */
  //-----------------------------------------------------------------------------
  virtual HeapBase* GetLowerHandle();

  //----------------------------------------------------------------------------
  /**
   *  @brief  確保された領域のサイズを取得(HeapManagerにもあります)
   *          継承していなければ非対応
   *
   *  @param  const void mem 調べるメモリ
   *  @return int メモリサイズ(-1は失敗)
   */
  //-----------------------------------------------------------------------------
  virtual int GetMemorySize( const void *mem ){return -1;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  渡されたメモリが正当かどうかをチェックします。
   *
   *  @param  const void mem 調べるメモリ
   *  @return bool チェック失敗でfalse
   */
  //-----------------------------------------------------------------------------
  virtual bool CheckMemoryBlock( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  すべてのメモリを解放します。
   *
   *  @return bool 失敗したらfalse
   */
  //-----------------------------------------------------------------------------
  virtual bool FreeAll(void){return false;}


#if GFL_HEAP_DEBUG
  void SetDebugModeFlg( bool flg );
  u32 GetMaxLeastSize(void){return mMinLeast;}
  void SetFilename( char * buf, const char * filename );
  void SetCreateData( const char * filename, const int linenum, bool parent );
  const char * GetCreateDataFilename(void);
  int GetCreateDataLine(void);
  bool GetCreateParentFlag(void);
  void SetDebugUserID( u8 id );

  // メモリ解放時のメモリクリア設定
  enum DebugFreeMemClearType
  {
    DEBUG_FREE_MEMC_NONE = 0,     ///< クリアしない
    DEBUG_FREE_MEMC_ZERO,     ///< ゼロでクリア
    DEBUG_FREE_MEMC_FF,       ///< 0xFFでクリア

    DEBUG_FREE_MEMC_TYPE_MAX,
  };
  static inline void SetDebugFreeMemClear( DebugFreeMemClearType type ){ mDebugFreeMemClear = type; }
  static inline DebugFreeMemClearType GetDebugFreeMemClear( void ){ return (DebugFreeMemClearType)mDebugFreeMemClear; }
#endif

//Heapの確保は上のマクロを使って行ってください。
//AllocMemoryBlockとAllocMemoryBlockAlignを直接使用しないでください。
#if GFL_HEAP_DEBUG
  void* AllocMemory( const int size , const char *filename , const int linenum );
  void* AllocMemoryAlign( const int size , const int align , const char *filename , const int linenum );
#else
  void* AllocMemory( const int size );
  void* AllocMemoryAlign( const int size , const int align );
#endif

#if GFL_HEAP_DEBUG
  void* AllocMemory4096(  const int size , const bool isClearMemory , const char *filename , const u16 linenum );
  void* AllocMemory4096Lower(  const int size , const bool isClearMemory , const char *filename , const u16 linenum );
#else
  void* AllocMemory4096(  const int size , const bool isClearMemory );
  void* AllocMemory4096Lower(  const int size , const bool isClearMemory );
#endif

#if GFL_HIO_ENABLE
virtual void HioDump( bool flag ){};
void  HioLineAdd( void* adrs, const int size, const char* filename, const int linenum, const int align );
#endif

protected:

#if GFL_HEAP_DEBUG
  void* AllocMemory4096Core(  const int size , const bool isClearMemory ,  bool isLower , const char *filename , const u16 linenum );
#else
  void* AllocMemory4096Core(  const int size , const bool isClearMemory , bool isLower );
#endif

  //実際のメモリ先頭取得
  //FreeMemoryAutoから呼ぶのでstatic
  static void* GetMemoryTop( const void *buf );
  //ヘッダー位置取得
  //FreeMemoryAutoから呼ぶのでstatic
  static MEMORY_HEADER* GetMemoryHeader( const void *buf );
  //ヘッダーサイズ取得(GetMemoryTopの前のアドレスを渡す
  int GetMemoryHeaderSize( const void *buf );

  //AllocCountの操作(HeapBaseLowerから操作があるのでこっちを使う！)
  virtual int GetAllocCount(void) const{ return m_allocCount; };
  virtual void IncAllocCount(void);
  virtual void DecAllocCount(void);

#if GFL_HIO_ENABLE
  bool  mHioDump;
  u32   mHioLineIndex;
#endif

private:

  //HeapBaseLowerから操作があるので直接使わないこと！
  HEAPID m_heapId;
  int    m_allocCount;

  //後方から確保するためのハンドル
  GflHeapBaseLower *m_lowerHandle;

  nn::os::CriticalSection mAllocCntLock;


#if GFL_HEAP_DEBUG
  u32  mMinLeast;
  MEMORY_USER_CREATE_DATA createData;
  bool mDebugFlg;
  u8   mDebugUserID;
  u8   mPadding[2];

  static u8 mDebugFreeMemClear;
#endif

  //以下純粋仮想関数
  //継承先で実装してください。
  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの作成
   *          friendでgfl::heap::Managerからのみ呼び出し
   *
   *  @param  void *buf 確保用バッファ
   *  @param  const int size バッファサイズ
   *  @param  void* param ユーザーパラメーター
   */
  //-----------------------------------------------------------------------------
  virtual void CreateHeap( void *buf , const int size , void *param ) = 0;

  //----------------------------------------------------------------------------
  /**
   *  @brief  ヒープの削除
   *          friendでgfl::heap::Managerからのみ呼び出し
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void DeleteHeap(void) = 0;

  //実際のメモリ確保・開放処理
  virtual void* AllocAlign( const int size , const int align ) = 0;
  virtual void Free( void* buf ) = 0;
  //ここまで純粋仮想関数


  //friendでgfl::heap::Managerからのみ呼び出し.HeapBaseの初期化
  void InitHeapBase( const HEAPID heapId );
  //friendでgfl::heap::Managerからのみ呼び出し.HeapBaseの開放
  //isCheckはソフトリセットの時などにfalseになる。残っていても解放する
  void TermHeapBase( bool isCheck = true );

  //friendでgfl::heap::Managerからのみ呼び出しLowerHandleの設定
  virtual void SetLowerHandle( GflHeapBaseLower *handle );

#if GFL_HEAP_DEBUG
  //メモリ確保コア処理
  void* AllocMemoryCore(  const int size , const int align , const bool isClearMemory , const char *filename , const u16 linenum );
#if GFL_HIO_ENABLE
  //ヘッダーセット
  void SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset , const char *filename , const u16 linenum, const int align );
  //ユーザーヘッダーセット(デフォルト)
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum, const int align );
#else
  //ヘッダーセット
  void SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset , const char *filename , const u16 linenum );
  //ユーザーヘッダーセット(デフォルト)
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum );
#endif
#else
  //メモリ確保コア処理
  void* AllocMemoryCore(  const int size , const int align , const bool isClearMemory );
  //ヘッダーセット
  void SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset );
  //ユーザーヘッダーセット(デフォルト)
  virtual void SetMemoryHeader_UserData( void *userHeader );
#endif

#if GFL_HIO_ENABLE
void  HioLineDel( HEAPID heapId, void* userHeader );
#endif

};

} //namespace heap
} //namespace gfl

#endif // __GFL_HEAPBASE_H__
