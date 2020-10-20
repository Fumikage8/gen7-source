//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_EffectHeap.h
 *	@brief  HeapBaseを使用した汎用的なエフェクトヒープ
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __GFL2_EFFECT_HEAP_H__ )
#define __GFL2_EFFECT_HEAP_H__
#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nw/eft.h>
#elif defined(GF_PLATFORM_WIN32)
#include <nw/effect.h>
#endif

#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( Effect )

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
typedef nw::eft::Heap Heap;                       // エフェクトヒープ

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
/**
 * @brief HeapBaseを使用した汎用的なエフェクトヒープ
 */
class GFHeap : public Heap
{
  GFL_FORBID_COPY_AND_ASSIGN(GFHeap); //コピーコンストラクタ＋代入禁止

public:
  /**
   *@brief  コンストラクタ
   */
  GFHeap( void ) : 
    m_pHeapBase(NULL)
  {
  }
  /**
   *@brief  コンストラクタ
   *@param  pHeapBase 使用するヒープベース
   */
  GFHeap( gfl2::heap::HeapBase * pHeapBase ) : 
    m_pHeapBase(pHeapBase)
  {
  }

  //---------------------------------------------------------------------------
  //! @brief        メモリ確保を行います。
  //!               各ご使用の環境に合わせて派生クラス側にて実装をお願いします。
  //! @param[in]    size      確保するメモリサイズ.
  //! @param[in]    alignment メモリアライメント.
  //! @return       確保したメモリへのポインタを返却します。
  //---------------------------------------------------------------------------
  virtual void* Alloc( u32 size, s32 alignment = Heap::EFT_HEAP_DEAFULT_ALIGNMENT )
  {
    return GflHeapAllocMemoryAlign( m_pHeapBase, size, alignment );
  }


  //---------------------------------------------------------------------------
  //! @brief        メモリ破棄を行います。
  //!               各ご使用の環境に合わせて派生クラス側にて実装をお願いします。
  //! @param[in]    ptr       解放するメモリへのポインタ.
  //---------------------------------------------------------------------------
  virtual void Free( void* ptr )
  {
    GflHeapSafeFreeMemory( ptr );
  }

  //---------------------------------------------------------------------------
  //! @brief        保持しているHeapBaseの取得
  //! @retval       HeapBase * 
  //---------------------------------------------------------------------------
  gfl2::heap::HeapBase * GetGFHeapBase() const { return m_pHeapBase; }

private:
  gfl2::heap::HeapBase * m_pHeapBase;
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( gfl2 )

#endif	// __GFL2_EFFECT_HEAP_H__

