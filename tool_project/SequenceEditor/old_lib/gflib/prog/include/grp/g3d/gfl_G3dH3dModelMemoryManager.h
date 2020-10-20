//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dModelMemoryManager.h
 *	@brief  H3D�@���f������������Ǘ��@�}�l�[�W��
 *	@author	tomoya takahashi
 *	@date		2012.08.30
 *
 *	@note H3D���f���̃L���b�V�����������P�t���[���҂���
 *	�@�@�@�����_�����O�I����ɍ폜����}�l�[�W��
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_G3dH3DMODELMEMORYMANAGER_H__
#define __GFL_G3dH3DMODELMEMORYMANAGER_H__
#pragma once

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_Debug.h>
#include <gfl_fs.h>


#if GFL_GRP_G3D_H3D_USE


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
class H3dModelMemoryManager
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dModelMemoryManager);

public:
  
  class IMemortObject{
  public:
    virtual ~IMemortObject(){}
  protected:
    IMemortObject(){}
    
  };

  // �R���X�g���N�^�E�f�X�g���N�^
  H3dModelMemoryManager(void);
  ~H3dModelMemoryManager(void);
  
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �}�l�[�W��������
   *
   *	@param  p_heap        �q�[�v
   *	@param  h3dmodel_max  ���f���ő吔
   */
  //-----------------------------------------------------------------------------
  void Initialize( gfl::heap::HeapBase* p_heap, u32 h3dmodel_max );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �}�l�[�W���j���@�i�o�^���ꂽ���������ꊇ�ŉ�����܂��j
   */
  //-----------------------------------------------------------------------------
  void Finalize( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ��������̍X�V    Calculate��SubmitView�̃^�C�~���O�ŌĂ΂��z��ł���B
   *	                          �P�t���[���ɂP��B
   */
  //-----------------------------------------------------------------------------
  void Update( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �j�����郁������o�^    �P�񂾂��o�^����悤�ɒ���
   */
  //-----------------------------------------------------------------------------
  //void SetDeleteMemory( void * p_state_heap, void * p_state_device );
  void SetDeleteMemory( IMemortObject* pObject );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �f�[�^�̗L�����`�F�b�N
   */
  //-----------------------------------------------------------------------------
  bool IsHeapIdMemoryExists( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    heapID������ꂽ�����������݂��Ă��邩�`�F�b�N
   *
   *	@retval true  ���݂��Ă���
   *	@retval false ���݂��Ă��Ȃ�
   */
  //-----------------------------------------------------------------------------
  bool IsHeapIdMemoryExists( gfl::heap::HEAPID heapID ) const;


private:

  struct Data
  {
    //void * p_state_heap;
    //void * p_state_device;
    IMemortObject*    pMemortObject;
    
    s8 count;
    b8 data_in;
    u8  pad[2];
    
    Data() : pMemortObject(NULL){}
  };
  

private:
  void FreeMemoryData( Data* p_data );

private:

  Data* m_pBuffer;
  s32   m_BufferMax;

};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl


#endif  // GFL_GRP_G3D_H3D_USE


#endif	// __GFL_G3dH3DMODELMEMORYMANAGER_H__

