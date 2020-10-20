//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		nijiAllocator.h
 *	@brief  niji用アロケーター   GFHeapAllocator にVRAM管理処理を追加したもの
 *	@author	tomoya takahashi
 *	@date		2015.05.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __NIJI_ALLOCATOR_H__ )
#define __NIJI_ALLOCATOR_H__
#pragma once

#include <util/include/gfl2_AreaMan.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(System);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 *  @class nijiアロケーター
 *  GLHeapAllocatorにVRAMからもメモリ確保処理を追加したものです。
 *  nijiではこちらのアロケーターを使用してください。
 */
class nijiAllocator : public gfl2::util::GLHeapAllocator
{

  GFL_FORBID_COPY_AND_ASSIGN(nijiAllocator); //コピーコンストラクタ＋代入禁止
public:
  /// VRAM領域データ
  struct VramArea
  {
    void * startAddrA;
    void * startAddrB;
    u32    sizeA;
    u32    sizeB;
    gfl2::util::AreaMan * vramA;
    gfl2::util::AreaMan * vramB;
    gfl2::util::AreaMan::ResultCode* resultA;
    gfl2::util::AreaMan::ResultCode* resultB;
  };

  enum VramControlType
  {
    RESIDENT = 0,   //! 常駐VRAM領域を使用
    APPLICATION,    //! アプリケーションVRAM領域を使用
  };
  
  // @todo 不要となる為、廃止予定です。
  enum VramMallocMode
  {
    VRAM_MALLOC_A,		//! Aからしか確保しない
    VRAM_MALLOC_B,		//! Bからしか確保しない
    VRAM_MALLOC_AB,		//! ABの空き領域が多いほうから確保
  };

  
  static const u32 RESIDENT_VRAMA_START = 0;  //! 常駐VRAMA　開始オフセット
  static const u32 RESIDENT_VRAMB_START = 0;  //! 常駐VRAMB　開始オフセット
  static const u32 RESIDENT_VRAMA_SIZE = 0x100000;  //!常駐VRAMA　使用サイズ
  static const u32 RESIDENT_VRAMB_SIZE = 0x100000;  //!常駐VRAMB　使用サイズ
  static const u32 VRAMA_SIZE = 0x300000;  //!VRAMA　全体サイズ
  static const u32 VRAMB_SIZE = 0x300000;  //!VRAMB　全体サイズ
  static const u32 AREA_MAN_BLOCK_SIZE = 512;//! ブロックサイズ (16x16 pixcel　の 16bitカラーのsize）
  static const u32 VRAM_ALLOCK_BLOCK_MAX = 256; //! VRAM管理ブロック数最大

public:
  /************************************************************************/
  /*            VRAM管理領域の管理                                        */
  /************************************************************************/
  static void Initialize( gfl2::heap::HeapBase* pHeap ); //! 初期化
  static void Terminate( void );                    //! 破棄
  static void GetResident( VramArea* desc );			//! 常駐で利用するVRAM領域情報
  static void GetApplication( VramArea* desc );		//! アプリで利用するVRAM領域情報　アプリはこちらのdescで初期化
#if PM_DEBUG
  static void DebugCheckApplicationVramClear();  //! アプリケーションVRAMが完全に解放されているかチェックする。
  static void DebugVramMemoryInfoDump();  //! VRAMの使用状況をLOG出力する。
  static void DebugSetResidentVramCheck(); //! これ以降常駐VRAMからの確保を監視する。
#endif // PM_DEBUG
  /************************************************************************/
  /************************************************************************/

  /**
   * @brief nijiAllocatorコンストラクタ
   * @param devHeap     メモリ確保するヒープ
   * @param controlType VRAM領域コントロールタイプ
   * @param MallocMode  VRAM確保モード @todo 今後廃止予定　これは指定できないようにしました。
   *
   * controlTypeで、RESIDENTを使用するのはGflUseで生成される描画エンジンのみです。
   * その他のシステムでは、APPLICATIONを使用しRESIDENTは使用しないでください。
   */
  nijiAllocator( gfl2::heap::HeapBase *devHeap, VramControlType controlType );
  nijiAllocator( gfl2::heap::HeapBase *devHeap );
  virtual ~nijiAllocator();

  /*
   GLHeapAllocatorの物を使用
  virtual void* SystemMemoryMalloc(u32 size, u32 alignment);
  virtual void SystemMemoryFree(void* const address);
  */


  //! VRAMからメモリ確保
  virtual void* LocalMemoryMalloc(u32 size, u32 alignment);
  //! VRAMからメモリ解放
  virtual void LocalMemoryFree(void* const address);

  //! VRAM確保モードを設定
  void SetVramMallocMode( VramMallocMode mode );
  //! VRAM確保モードを取得
  inline VramMallocMode GetVramMallocMode() const {return m_MallocMode;}

  //! VRAMコントロールタイプを取得
  inline VramControlType GetVramControlType() const {return m_VramControlType;}

private:

  //! VRAM エリアにアドレスが含まれるかチェック
  static bool IsVramArea( const VramArea& area, uptr address, bool A );

  //! VRAMの領域確保
  static void * AllocVramArea( VramArea* p_area, u32 size, u32 alignment, bool A );

  //! VRAMの領域解放
  static void FreeVramArea( VramArea* p_area, void* const address, bool A );

  //! VRAM空き領域の多いほうを返す
  static bool CompVramFreeSize( const VramArea& area );

  static u32 GetBlockAlign( u32 alignment );
  static u32 GetBlockSize( u32 size );
  static gfl2::util::AreaMan::ResultCode * GetClearResultTbl( VramArea* p_area, bool A );
  static gfl2::util::AreaMan::ResultCode * GetAddrResultTbl( VramArea* p_area, uptr addr, bool A );

private:
  // VRAM管理領域 
  VramArea 		m_Vram;           //! 現在使用しているVRAM領域情報(Resident or Application)
  VramControlType m_VramControlType;  //! VRAMコントロールタイプ
  VramMallocMode	m_MallocMode; //! VRAM確保モード

#if PM_DEBUG
  static bool m_IsDebugResidentVramAllocCheck; //! 常駐VRAM領域からの確保を監視
#endif

  // 常駐VRAM領域管理
  static gfl2::util::AreaMan*	m_pResidentVramA;
  static gfl2::util::AreaMan*	m_pResidentVramB;
  static gfl2::util::AreaMan::ResultCode* m_pResultResidentVramA;
  static gfl2::util::AreaMan::ResultCode* m_pResultResidentVramB;
  // アプリVRAM領域管理
  static gfl2::util::AreaMan*	m_pApplicationVramA;
  static gfl2::util::AreaMan*	m_pApplicationVramB;
  static gfl2::util::AreaMan::ResultCode* m_pResultApplicationVramA;
  static gfl2::util::AreaMan::ResultCode* m_pResultApplicationVramB;
};

GFL_NAMESPACE_END(System);

#endif	// __NIJI_ALLOCATOR_H__

