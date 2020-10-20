#ifndef GFLIB2_UTIL_FIXEDSIZECONTAINER_H_INCLUDED
#define GFLIB2_UTIL_FIXEDSIZECONTAINER_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4345)//POD型構造体へのplacement newは初期化されない。が、そんなものわかってるので無効。
#pragma warning(disable:4324)//__declspec(align(#))でパディングされたという警告。意図している挙動なので無効にする。
#endif

namespace gfl2 { namespace util {

// aligned 修飾子
#if defined( _MSC_VER )
	#define GF_ALIGNED( SIZE ) __declspec(align(SIZE))
#else
	#define GF_ALIGNED( SIZE ) __attribute__((aligned(SIZE)))
#endif

template <typename T> struct LinkData
{
	LinkData(): m_pData(NULL), m_pPrev(NULL), m_pNext(NULL), m_Index(-1){}

	T							*m_pData;
	LinkData<T>		*m_pPrev;
	LinkData<T>		*m_pNext;
	s32						m_Index;
};

template <typename TElement>
class FixedSizeContainerBase{
public:

	FixedSizeContainerBase() :
		m_pBuffer(NULL),
		m_pLinkList(NULL),
		m_ppIndexer(NULL),
		m_BufferSize(0),
		m_pEmptyTop(NULL),
		m_pDataTop(NULL),
		m_UsedSize(0)
	{
	}

	virtual ~FixedSizeContainerBase(){}

	//! @brief 使用サイズ取得
	inline u32 size() const
	{
		return m_UsedSize;
	}

	//! @brief 空きサイズ取得
	inline u32 free_size() const
	{
		return m_BufferSize - m_UsedSize;
	}

	//! @brief 全体サイズ取得
	inline u32 buffer_size() const
	{
		return m_BufferSize;
	}

	//! @brief インデクサ
	inline TElement& operator[](u32 index)
	{
		GFL_ASSERT( m_ppIndexer[index] );
		return *(m_ppIndexer[index]->m_pData);
	}

	//! @brief インデクサ
	inline const TElement& operator[](u32 index) const
	{
		GFL_ASSERT( m_ppIndexer[index] );
		return *(m_ppIndexer[index]->m_pData);
	}

	//! @brief 要素を末尾に追加
  b32 push_back(TElement& element)
	{
		//GFL_ASSERT( isCyclic() == false );
		GFL_ASSERT( m_pEmptyTop );

		LinkData<TElement>			*pNext = m_pEmptyTop->m_pNext;

		{
			LinkData<TElement>			*pTarget = m_pEmptyTop;

			//isData( element );

			//バッファをTElement配列で持つということはしない。
			//そうしてしまうと初期化時に、配列分のコンストラクタが呼ばれるということ。それは想定した挙動ではない。
			//本来はプッシュした際に変数が生成され積まれるのが正しい挙動。
			new(pTarget->m_pData) TElement();//このタイミングで生成が正しい。

			//コピーコンストラクタがないものでも使えるようにしておく。よってデフォルトコンストラクタ呼び出し->代入。代入くらいは実装してもらおう。
			*(pTarget->m_pData) = element;

			pTarget->m_pNext = NULL;
			pTarget->m_pPrev = NULL;
			pTarget->m_Index = m_UsedSize;//末端を示す仕様

			if ( m_pDataTop )
			{
				m_pDataTop->m_pPrev = pTarget;
				pTarget->m_pNext = m_pDataTop;
				GFL_ASSERT( pTarget != pTarget->m_pNext );
			}
			m_pDataTop = pTarget;

			m_ppIndexer[m_UsedSize] = pTarget;
			++m_UsedSize;
		}

		m_pEmptyTop = pNext;
		if ( m_pEmptyTop )
			m_pEmptyTop->m_pPrev = NULL;

    //GFL_ASSERT( isCyclic() == false );
		return true;
	}

	//! @brief 末尾の要素を削除
	void pop_back()
	{
		//GFL_ASSERT( isCyclic() == false );
		GFL_ASSERT( m_pDataTop );

		LinkData<TElement>			*pNext = m_pDataTop->m_pNext;

		addEmpty( m_pDataTop );

		m_pDataTop = pNext;
		if ( m_pDataTop )
			m_pDataTop->m_pPrev = NULL;
    //GFL_ASSERT( isCyclic() == false );
	}

#if 0
  bool isCyclic( void ) const
  {
		LinkData<TElement>			*pData1 = m_pDataTop;
		LinkData<TElement>			*pData2 = m_pDataTop;
    while ( pData2 )
    {
      pData1 = pData1->m_pNext;
      pData2 = pData2->m_pNext;
      if ( pData2 == NULL ) break;
      pData2 = pData2->m_pNext;
      if ( pData2 == pData1 ) return true;
    }
    return false;
  }

	bool isData( TElement element )
  {
		LinkData<TElement>			*pData = m_pDataTop;
    while ( pData )
    {
			if ( *(pData->m_pData) == element )
			{
				removeData( pData );
				return true;
			}
      pData = pData->m_pNext;
    }
    return false;
  }
#endif

	//! @brief 一致するものすべてを削除
	void remove( TElement element )
	{
		//GFL_ASSERT( isCyclic() == false );

		LinkData<TElement>			*pData = m_pDataTop;

    //GFL_ASSERT( isCyclic() == false );
		while( pData )
		{
			LinkData<TElement>			*pNext = pData->m_pNext;
			if ( *(pData->m_pData) == element )
			{
				remove( pData );
			}
			pData = pNext;
		}

		//GFL_ASSERT( isData(element) == false );
		//GFL_ASSERT( isCyclic() == false );
	}

	//! @brief 要素を削除
	void remove( u32 index )
	{
		remove( &m_pLinkList[index] );
	}

	//! @brief 全要素を削除
	void clear()
	{
		for( u32 pos = 0; pos < m_UsedSize; ++pos )
		{
			destroy( m_ppIndexer[pos]->m_pData );
		}
		initialize();
	}

protected:

	void initialize()
	{
		TElement								*pBuffer = reinterpret_cast<TElement*>(m_pBuffer);

		for( u32 i = 0; i < m_BufferSize; ++i )
		{
			m_pLinkList[i].m_pData = &pBuffer[i];
			m_pLinkList[i].m_pNext = NULL;

			if ( i - 1 >= 0 )
				m_pLinkList[i].m_pPrev = &m_pLinkList[i-1];

			

			if ( (i+1) < m_BufferSize )
				m_pLinkList[i].m_pNext = &m_pLinkList[i+1];

			m_ppIndexer[i] = NULL;
		}

		m_pEmptyTop = &m_pLinkList[0];
		m_pDataTop = NULL;
		m_UsedSize = 0;
	}

	inline void destroy(TElement* element)
	{
		element->~TElement();
	}

	void remove( LinkData<TElement> *pTarget )
	{
		GFL_ASSERT( m_pDataTop );

		removeData( pTarget );
		addEmpty( pTarget );

		//GFL_ASSERT( isCyclic() == false );
	}

	void removeData( LinkData<TElement> *pTarget )
	{
		if ( (pTarget == m_pDataTop) && (pTarget->m_pPrev == NULL) )
		{//m_pDataTopだと言える。
			m_pDataTop = pTarget->m_pNext;
			if ( m_pDataTop )
				m_pDataTop->m_pPrev = NULL;
		}
		else
		{
			pTarget->m_pPrev->m_pNext = pTarget->m_pNext;
			GFL_ASSERT( pTarget->m_pPrev != pTarget->m_pPrev->m_pNext );
			if ( pTarget->m_pNext )
				pTarget->m_pNext->m_pPrev = pTarget->m_pPrev;
		}
	}

	void addEmpty( LinkData<TElement> *pTarget )
	{
		u32					deletePos;

		destroy( pTarget->m_pData );
		pTarget->m_pNext = NULL;
		pTarget->m_pPrev = NULL;
		deletePos = pTarget->m_Index;

		if ( m_pEmptyTop )
		{
			m_pEmptyTop->m_pPrev = pTarget;
			pTarget->m_pNext = m_pEmptyTop;
		}
		m_pEmptyTop = pTarget;

		{//vector仕様とする。よって詰める。
			for( u32 pos = deletePos; pos < (m_UsedSize-1); ++pos )
			{
				m_ppIndexer[pos] = m_ppIndexer[pos+1];
				m_ppIndexer[pos]->m_Index = pos;
			}
			m_ppIndexer[m_UsedSize-1] = NULL;
		}

		--m_UsedSize;
	}

protected:
	u8												*m_pBuffer;
	LinkData<TElement>				*m_pLinkList;
	LinkData<TElement>				**m_ppIndexer;
	u32												m_BufferSize;

	LinkData<TElement>				*m_pEmptyTop;
	LinkData<TElement>				*m_pDataTop;
	u32												m_UsedSize;

};

template <typename TElement, s32 arrayLength=-1>
class FixedSizeContainer;

template <typename TElement, s32 arrayLength>
class FixedSizeContainer : public FixedSizeContainerBase< TElement >
{
public:
	FixedSizeContainer() : FixedSizeContainerBase<TElement>()
	{
		FixedSizeContainerBase< TElement >::m_pBuffer = &m_Buffer[0];
		FixedSizeContainerBase< TElement >::m_pLinkList = &m_LinkList[0];
		FixedSizeContainerBase< TElement >::m_ppIndexer = &m_pIndexer[0];
		FixedSizeContainerBase< TElement >::m_BufferSize = arrayLength;

		FixedSizeContainerBase< TElement >::initialize();
	}

	virtual ~FixedSizeContainer(){}

	//! @note アライメントサイズを渡せるように作りたい…。１２８アラインで扱えない特殊なものが来たときに考えよう。
	GF_ALIGNED(0x80) u8				m_Buffer[ sizeof(TElement) * arrayLength ];
	LinkData<TElement>				m_LinkList[arrayLength];
	LinkData<TElement>				*m_pIndexer[arrayLength];

};

template <typename TElement>
class FixedSizeContainer<TElement, -1> : public util::FixedSizeContainerBase< TElement >
{
public:
	FixedSizeContainer() : FixedSizeContainerBase<TElement>()
	{
		
	}

	void CreateBuffer( gfl2::heap::HeapBase *pHeap, u32 arrayLength )
	{
		FixedSizeContainerBase< TElement >::m_pBuffer = GFL_NEW(pHeap) u8[ sizeof(TElement) * arrayLength ];
		FixedSizeContainerBase< TElement >::m_pLinkList = GFL_NEW(pHeap) LinkData<TElement>[ arrayLength ];
		FixedSizeContainerBase< TElement >::m_ppIndexer = GFL_NEW(pHeap) LinkData<TElement>*[ arrayLength ];
		FixedSizeContainerBase< TElement >::m_BufferSize = arrayLength;
		FixedSizeContainerBase< TElement >::initialize();
	}

	virtual ~FixedSizeContainer()
	{
		if ( FixedSizeContainerBase< TElement >::m_pBuffer )
			GFL_SAFE_DELETE_ARRAY( FixedSizeContainerBase< TElement >::m_pBuffer );

		if ( FixedSizeContainerBase< TElement >::m_pLinkList )
			GFL_SAFE_DELETE_ARRAY( FixedSizeContainerBase< TElement >::m_pLinkList );

		if ( FixedSizeContainerBase< TElement >::m_ppIndexer )
			GFL_SAFE_DELETE_ARRAY( FixedSizeContainerBase< TElement >::m_ppIndexer );
	}

};

}}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
