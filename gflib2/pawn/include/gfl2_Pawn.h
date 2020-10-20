//==============================================================================
/**
 * @file    gfl2_Pawn.h
 * @brief   Pawnクラス.
 * @author    kimura_shin
 * @date    2014/10/23(木) 14:46:06 N.Takeda  更新.
 */
//==============================================================================

#if !defined(__GFL2_PAWN_H__) // 重複定義防止
#define __GFL2_PAWN_H__ // 重複定義防止マクロ
#pragma once


/// GFLIBのヒープを使用するかフラグ. 基本切り離す事はないが、Pawnの場合、コマンドラインで動作テストのみしたい場合があるので残す.
#define USE_GFLHEAP       (1)

#ifdef GF_PLATFORM_CTR
  #include <nn/types.h>
#endif

#ifdef  __cplusplus
  #ifdef USE_GFLHEAP
    #include "heap/include/gfl2_Heap.h"
  #endif
  #include "debug/include/gfl2_assert.h"
#endif


#ifdef  __cplusplus
#include "gfl2_PawnBase.h"

#if GFL_PAWN_DEBUG
  #include "gfl2_PawnDebug.h"
#endif  //GFL_PAWN_DEBUG

namespace gfl2 {
namespace pawn {


#if GFL_PAWN_DEBUG
  #define _pawnBase PawnDebug
#else
  #define _pawnBase PawnBase
#endif  // GFL_PAWN_DEBUG


#ifdef USE_GFLHEAP
  #define PAWN_CONPARAM     gfl2::heap::HeapBase* heap = 0, gfl2::heap::HeapBase* heap_debug = 0
#else
  #define PAWN_CONPARAM
#endif


//------------------------------------------------------------------------------
/**
 * @class   Pawn
 * @brief   PAWN管理クラス.
 * @author    kimura_shin
 * @date    2014/10/23(木) 14:45:38
 */
//------------------------------------------------------------------------------
class Pawn : public _pawnBase
{
public:
#ifdef USE_GFLHEAP
  /// ------- setters ---------
  void  SetHeap( gfl2::heap::HeapBase* _pHeap, gfl2::heap::HeapBase* _pHeapDebug = NULL ){
    m_pHeap = _pHeap;
#if GFL_PAWN_DEBUG
    if( _pHeap )  PawnDebug::Initialize( _pHeapDebug?_pHeapDebug:_pHeap );
#endif
  }
  /// ------- getters ---------
  gfl2::heap::HeapBase* GetHeap() const { return m_pHeap; }
#endif

  /// コンストラクタ.
  Pawn( PAWN_CONPARAM ) : _pawnBase()
  {
#ifdef USE_GFLHEAP
    SetHeap( heap, heap_debug );
#endif
  }

  /// デストラクタ.
  virtual ~Pawn( void ) {}

  virtual void  Load( const void* buffer, const size_t size, const c8* const amxname="" )
  {
    _pawnBase::Load( buffer, size, amxname );
    GetAmx( )->userdata[ GFL_AMX_THIS ] = this;   ///< ここで入れないとPawnのポインタにならない.
  }
  
public:
  
  /**
   * コードをシェアしてロード
     * @param org　コピー元となるPawnクラスインスタンス
     */
  virtual void  LoadShare( const PawnBase* org )
  {
    _pawnBase::LoadShare( org );
    GetAmx( )->userdata[GFL_AMX_THIS] = this;   ///< ここで入れないとPawnのポインタにならない.
  }

#if 0 // 使用されていないのでクローズ. glf の gfl_CoreFile.cpp の移植が必要
  void  LoadByteArrayPointer( gfl::core::ByteArrayPointer dat, const c8* const amxname )
  {
    Load( dat->GetData( ), dat->GetSize( ), amxname );
  }

  void  LoadFile( const c8 * const filename )
  {
    LoadByteArrayPointer( gfl::core::File::LoadSmart( filename, m_pHeap ), filename );
  }
#endif

protected:
  /**
   * @note  N.Takeda
   * メモリ関連カスタマイズ.
   */
  virtual u8* PawnNew( const s32 iSize, const s32 iCategory );
  virtual void  PawnDel( u8 *pPtr );
  
  /**
   * @note  N.Takeda
   * メモリ操作関連.
   */
  virtual void  PawnMemCpy( void *pD, const void *pS, const s32 iLength );
  virtual void  PawnMemFill( void *pPtr, const u8 _uVal, const s32 iLength );
  
#ifdef USE_GFLHEAP
    gfl2::heap::HeapBase  *m_pHeap;
#endif
};


}
}
#endif //__cplusplus

#endif // __GFL2_PAWN_H__ 重複定義防止
