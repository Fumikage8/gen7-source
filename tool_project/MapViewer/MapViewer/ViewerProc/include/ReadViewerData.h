/**
* @brief  �t�@�C�����[�h�N���X
* @file   FieldTerrainFactory.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __READVIEWERDATA_H_INCLUDED__ )
#define __READVIEWERDATA_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <gfx/include/gfl2_GLMemory.h>



//-----------------------------------------------------------------------------
/*
*          �����N���X�錾(���\�[�X�A���P�[�^)
*          @class GLAllocator
*/
//-----------------------------------------------------------------------------
class GLResourceAllocator : public gfl2::gfx::IGLAllocator
{
	public:
		
	//----------------------------------------------------------------------------
	/**
	*  @brief  �R���X�g���N�^
	*/
	//-----------------------------------------------------------------------------
	GLResourceAllocator():IGLAllocator(),m_pHeap(NULL) {}

	//----------------------------------------------------------------------------
	/**
	*  @brief  �f�X�g���N�^
	*/
	//-----------------------------------------------------------------------------
	virtual			~GLResourceAllocator(){}
		
  //----------------------------------------------------------------------------
  /**
  *  @brief  �R���X�g���N�^
  */
  //-----------------------------------------------------------------------------
  void  init(gfl2::heap::HeapBase* pHeap){ m_pHeap = pHeap; }

	//----------------------------------------------------------------------------
	/**
	*  @brief  ���C���������̎擾
	*  @param  �擾�������������T�C�Y
	*  @param  �A���C�����g
	*  @return ������
	*/
	//-----------------------------------------------------------------------------
#if GFL_DEBUG
  virtual void*  _SystemMemoryMallocAlign(u32 size, u32 alignment,const char *filename , const int linenum){ return m_pHeap->AllocMemoryAlign(size,alignment,filename,linenum);}
#else
  virtual void*  SystemMemoryMallocAlign(u32 size, u32 alignment){ return GflHeapAllocMemoryAlign( m_pHeap , size , alignment ); }
#endif

	//----------------------------------------------------------------------------
	/**
	*  @brief  ���C���������̉��
	*  @param  ����������������̃|�C���^
	*/
	//-----------------------------------------------------------------------------
	virtual void	SystemMemoryFree(void* const address){ return GflHeapFreeMemory( address ); }

	//----------------------------------------------------------------------------
	/**
	*  @brief  �r�f�I�������̎擾
	*  @param  �擾�������������T�C�Y
	*  @param  �A���C�����g
	*  @return ������
	*/
	//-----------------------------------------------------------------------------
	virtual void*	LocalMemoryMalloc(u32 size, u32 alignment){ return NULL; }

	//----------------------------------------------------------------------------
	/**
	*  @brief  �r�f�I�������̉��
	*  @param  ����������������̃|�C���^
	*/
	//-----------------------------------------------------------------------------
	virtual void	LocalMemoryFree(void* const address){ }

	private:
    
    gfl2::heap::HeapBase *m_pHeap;

};

extern void* ReadData( const char* pFilePath,GLResourceAllocator *pAllocator );

	#endif // __READVIEWERDATA_H_INCLUDED__