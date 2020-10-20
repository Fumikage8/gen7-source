#ifndef GFLIB2_GRX_GLMEMORY_H_INCLUDED
#define GFLIB2_GRX_GLMEMORY_H_INCLUDED

#include <stdio.h>
#include <new>
#include <types/include/gfl2_Typedef.h>
#include <Debug/include/gfl2_Assert.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <gfx/include/gfl2_types.h>

namespace gfl2 { namespace gfx {

class Pool;

#if GFL_DEBUG
#define SystemMemoryMalloc(size) _SystemMemoryMalloc(size,__FILE__,__LINE__)
#define SystemMemoryMallocAlign(size,align) _SystemMemoryMallocAlign(size,align,__FILE__,__LINE__)
#endif
//----------------------------------------------------------------------------
/**
 *	@brief  アロケーターインターフェイス
 */
//-----------------------------------------------------------------------------
class IGLAllocator
{
public:
	IGLAllocator(){}
	virtual ~IGLAllocator(){}

	//! @name システム(メイン)メモリ用関数
	//@{
#if GFL_DEBUG
    virtual void* _SystemMemoryMalloc(u32 size,const char *filename , const int linenum){return _SystemMemoryMallocAlign(size,4,filename,linenum);}
		virtual void* _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum) = 0;
#else
    virtual void* SystemMemoryMalloc(u32 size){return SystemMemoryMallocAlign(size,4);}
		virtual void* SystemMemoryMallocAlign(u32 size, u32 alignment) = 0;
#endif
    virtual void SystemMemoryFree(void* const address) = 0;
	//@}

	//! @name ローカル(ビデオ)メモリ用関数
	//@{
		virtual void* LocalMemoryMalloc( Pool allocation, u32 size, u32 alignment ){ return NULL; }
		virtual void* LocalMemoryMalloc(u32 size, u32 alignment = 4) = 0;
		virtual void LocalMemoryFree(void* const address) = 0;
	//@}
};

//----------------------------------------------------------------------------
/**
 *	@brief  描画ライブラリで用いるアロケーター
 */
//-----------------------------------------------------------------------------
class GLMemory
{
public:

  /*static void SetAllocator( IGLAllocator* pObj )
  {
  	s_pUserAllocator = pObj;
  }*/

	static IGLAllocator* GetAllocator()
	{
    if( s_pNewParameterAllocator ) return s_pNewParameterAllocator;
    if( s_pInstanceAllocator ) return s_pInstanceAllocator;

    GFL_ASSERT_MSG( false, "ＧＬアロケーターが渡されていないのに取得しようとしました。\n" );

    return s_pUserAllocator;
	}

	static bool IsAllocatorEmpty()
	{
    if( s_pNewParameterAllocator ) return false;
    if( s_pInstanceAllocator ) return false;

    return ( s_pUserAllocator == NULL );
	}

	static void* Malloc(u32 size, u32 alignment = 4)
	{
		return GetAllocator()->SystemMemoryMallocAlign(size, alignment);
	}

	static void Free(void* const address)
	{
		GetAllocator()->SystemMemoryFree(address);
	}

	inline void* operator new(size_t size) throw(){
    static int count = 0;
    count ++;
		return GetAllocator()->SystemMemoryMalloc(static_cast<u32>(size));
	}

  inline void* operator new(size_t size, u32 alignment) throw()
  {
    return GetAllocator()->SystemMemoryMallocAlign(static_cast<u32>(size), alignment);
  }
		
	inline void * operator new[](size_t size) throw(){
		return GetAllocator()->SystemMemoryMalloc(static_cast<u32>(size));
	}
		
	inline void *operator new(size_t, void* p) throw(){
		return p;
	}

#if GFL_DEBUG
  inline void* operator new (size_t size, gfl2::gfx::IGLAllocator *pAllocator,const char *filename , const int linenum) throw()
#else
  inline void* operator new (size_t size, gfl2::gfx::IGLAllocator *pAllocator) throw()
#endif
  {
    void * mem;
    SetNewParameterAllocator(pAllocator);//Instance生成用アロケーター設定
#if GFL_DEBUG
    mem = pAllocator->_SystemMemoryMalloc(static_cast<u32>(size),filename,linenum);
#else
    mem = pAllocator->SystemMemoryMalloc(static_cast<u32>(size));
#endif
    return mem;
  }

#if GFL_DEBUG
  inline void* operator new[] (size_t size, gfl2::gfx::IGLAllocator *pAllocator,const char *filename , const int linenum) throw()
#else
  inline void * operator new[](size_t size, gfl2::gfx::IGLAllocator *pAllocator) throw()
#endif
  {
    void * mem;
    SetNewParameterAllocator(pAllocator);//Instance生成用アロケーター設定
#if GFL_DEBUG
    mem = pAllocator->_SystemMemoryMalloc(static_cast<u32>(size),filename,linenum);
#else
    mem = pAllocator->SystemMemoryMalloc(static_cast<u32>(size));
#endif
    return mem;
  }

	inline void operator delete(void* p) throw(){
    GLMemory * pThis = static_cast<GLMemory*>(p);
    pThis->GetInstanceAllocator()->SystemMemoryFree(p);
	}
		
	inline void operator delete[](void* p){
    GLMemory * pThis = static_cast<GLMemory*>((void*)((uptr)(p) + NEW_ARRAY_MEMORYCONTORL_SECTION_SIZE));
		pThis->GetInstanceAllocator()->SystemMemoryFree(p);
	}

#if GFL_DEBUG
  inline void operator delete (void*, gfl2::gfx::IGLAllocator *pAllocator,const char *filename , const int linenum)
#else
  inline void operator delete (void*, gfl2::gfx::IGLAllocator *pAllocator)
#endif
  {
    GFL_ASSERT( 0 );  // new( size_t IGLAllocator*)と対にするために定義、実際には呼ばれない
  }

#if GFL_DEBUG
  inline void operator delete[](void*,gfl2::gfx::IGLAllocator *,const char *filename , const int linenum) 
#else
  inline void operator delete[](void*,gfl2::gfx::IGLAllocator *) 
#endif
  {
    GFL_ASSERT( 0 );  // new[]( size_t IGLAllocator*)と対にするために定義、実際には呼ばれない
  }

  /**
   *    Instanceが確保されたアロケーター
   */
  inline IGLAllocator* GetInstanceAllocator()
  {
    return m_pInstanceAllocator;
  }

	virtual ~GLMemory(){}

  /**
   *  @brief  各Instance生成用のアロケーターを設定( これをコールしてよいのは、Create処理のみ ) 
   */
	static void SetInstanceAllocator( IGLAllocator* pObj )
	{
    if( s_InstanceAllocatorCount == 0 ){
      GFL_ASSERT( s_pInstanceAllocator == NULL );
      GFL_ASSERT( pObj != NULL ); // クリアする処理は、ClearInstanceAllocatorを使用する。
      s_pInstanceAllocator = pObj;
    }

    ++s_InstanceAllocatorCount;
	}

  /**
   *  @brief  各Instance生成用のアロケーターをクリア
   */
	static void ClearInstanceAllocator()
	{
    --s_InstanceAllocatorCount;
    if( s_InstanceAllocatorCount == 0 ){
		  s_pInstanceAllocator = NULL;
    }

    GFL_ASSERT( s_InstanceAllocatorCount >= 0 );
	}

  /**
   *  @brief  各Instance生成用のアロケーターを設定( これをコールしてよいのは、Create処理のみ ) 
   */
	static void SetNewParameterAllocator( IGLAllocator* pObj )
  {
    s_pNewParameterAllocator = pObj;
  }

protected:
  GLMemory():m_pInstanceAllocator(NULL)
  {
    if( GLMemory::s_pNewParameterAllocator || GLMemory::s_pInstanceAllocator )
      m_pInstanceAllocator = GetAllocator(); 
    s_pNewParameterAllocator =NULL;
  }



private:
	static IGLAllocator				*s_pUserAllocator;
	static IGLAllocator				*s_pInstanceAllocator;
  static int                s_InstanceAllocatorCount;
	static IGLAllocator				*s_pNewParameterAllocator;  // 最終的には、これだけで済むはず。

	IGLAllocator				*     m_pInstanceAllocator; // Instanceで保持するアロケーター
};

template <typename TElement>
class GLFixedSizeContainer : public gfl2::util::FixedSizeContainerBase< TElement >, protected GLMemory
{
public:
	GLFixedSizeContainer() : GLMemory(), gfl2::util::FixedSizeContainerBase< TElement >()
	{
		m_pGLAllocator = NULL;
	}

	void CreateBuffer( u32 bufferSize, IGLAllocator *pGLAllocator = NULL )
	{
		u32					size = 0;

		if ( pGLAllocator )
			m_pGLAllocator = pGLAllocator;
		else
			m_pGLAllocator = GetAllocator();

		{
			size = sizeof(TElement) * bufferSize;
			gfl2::util::FixedSizeContainerBase< TElement >::m_pBuffer = (u8*)m_pGLAllocator->SystemMemoryMalloc(size);
			std::memset( gfl2::util::FixedSizeContainerBase< TElement >::m_pBuffer, 0, size );
		}
		{
			size = sizeof( gfl2::util::LinkData<TElement> ) * bufferSize;
			gfl2::util::FixedSizeContainerBase< TElement >::m_pLinkList = (gfl2::util::LinkData<TElement>*)m_pGLAllocator->SystemMemoryMalloc( size );
			std::memset( gfl2::util::FixedSizeContainerBase< TElement >::m_pLinkList, 0, size );
		}
		{
			size = sizeof( gfl2::util::LinkData<TElement>* ) * bufferSize;
			gfl2::util::FixedSizeContainerBase< TElement >::m_ppIndexer = (gfl2::util::LinkData<TElement>**)m_pGLAllocator->SystemMemoryMalloc( size );
			std::memset( gfl2::util::FixedSizeContainerBase< TElement >::m_ppIndexer, 0, size );
		}
		gfl2::util::FixedSizeContainerBase< TElement >::m_BufferSize = bufferSize;
		gfl2::util::FixedSizeContainerBase< TElement >::initialize();
	}

	virtual ~GLFixedSizeContainer()
	{
		if ( gfl2::util::FixedSizeContainerBase< TElement >::m_pBuffer )
			m_pGLAllocator->SystemMemoryFree( gfl2::util::FixedSizeContainerBase< TElement >::m_pBuffer );

		if ( gfl2::util::FixedSizeContainerBase< TElement >::m_pLinkList )
			m_pGLAllocator->SystemMemoryFree( gfl2::util::FixedSizeContainerBase< TElement >::m_pLinkList );

		if ( gfl2::util::FixedSizeContainerBase< TElement >::m_ppIndexer )
			m_pGLAllocator->SystemMemoryFree( gfl2::util::FixedSizeContainerBase< TElement >::m_ppIndexer );
	}

	IGLAllocator					*m_pGLAllocator;
};

}}

#if GFL_DEBUG
inline void* operator new (size_t size, gfl2::gfx::IGLAllocator *pAllocator,const char *filename , const int linenum) throw()
{
	return pAllocator->_SystemMemoryMalloc(static_cast<u32>(size),filename,linenum);
}

inline void * operator new[](size_t size, gfl2::gfx::IGLAllocator *pAllocator,const char *filename , const int linenum) throw()
{
	return pAllocator->_SystemMemoryMalloc(static_cast<u32>(size),filename,linenum);
}
#else
inline void* operator new (size_t size, gfl2::gfx::IGLAllocator *pAllocator) throw()
{
	return pAllocator->SystemMemoryMalloc(static_cast<u32>(size));
}

inline void * operator new[](size_t size, gfl2::gfx::IGLAllocator *pAllocator) throw()
{
	return pAllocator->SystemMemoryMalloc(static_cast<u32>(size));
}
#endif

inline void operator delete(void* p, gfl2::gfx::IGLAllocator *pAllocator)
{
	pAllocator->SystemMemoryFree(p);
}
		
inline void operator delete[](void* p, gfl2::gfx::IGLAllocator *pAllocator)
{
	pAllocator->SystemMemoryFree(p);
}

/**
 *  GLMemoryを生成するためのマクロ
 *  破棄は、GFL_DELETEでOK
 */
#if GFL_DEBUG
#define GLNew(allocator) new( allocator,__FILE__,__LINE__ )
#define GLNewArray(allocator) new( allocator,__FILE__,__LINE__ )
#else
#define GLNew(allocator) new( allocator )
#define GLNewArray(allocator) new( allocator )
#endif

#define GFX_SAFE_DELETE( p )       { if( p ) { delete( p );		(p)=NULL; } }
#define GFX_SAFE_DELETE_ARRAY( p ) { if( p ) { delete[]( p );		(p)=NULL; } }

#endif
