#ifndef __GFL_HEAPMANAGER_H__
#define __GFL_HEAPMANAGER_H__
//======================================================================
/**
 * @file	gfl_HeapManager.h
 * @brief	�q�[�v�Ǘ��V�X�e��
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#pragma once

#include <gfl_Macros.h>

#if GFL_DEBUG
#define GFL_HEAP_DEBUG 0
#else
#define GFL_HEAP_DEBUG 0
#endif

namespace gfl
{
namespace heap
{


//GflHeapBase���p�����āA�v���b�g�t�H�[�����Ƃ̏�����ǉ����邽�߂̑Ή��B
//�A�v���ł�HeapBase�N���X�Ƃ��ċ��ʂŎg�����Ƃ��\

//�v���b�g�t�H�[���ʏ���
class CtrHeapBase;
typedef CtrHeapBase HeapBase;
//�v���b�g�t�H�[���ʏ��� �����܂�


//HEAPID�^�̒�`
typedef int HEAPID;

//�s����ID
static const HEAPID GFL_HEAP_INVALID_ID = -1;

//�q�[�v�̎��(�v���b�g�t�H�[���ʂɂ��邩�H)
typedef enum
{
  HEAP_TYPE_EXP,  //�g���q�[�v
  HEAP_TYPE_UNIT, //���j�b�g�q�[�v
  
  HEAP_TYPE_MAX
}HeapType;

//HEAPID�p��`
enum
{
  HEAPID_SYSTEM,
  
  HEAPID_MAX,
  //�A�v�����Ƃ�HEAPID�J�n�ʒu
  HEAPID_USER_START = HEAPID_MAX
};

typedef struct _HEAP_MNG_DATA HEAP_MNG_DATA;

#if GFL_HEAP_DEBUG
#define	GFL_CREATE_HEAP(parentHeap,childId,size,heapType,isBack)	( gfl::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack,__FILE__,__LINE__) )
#define	GFL_CREATE_HEAP_BUFFER(buffer,childId,size,heapType)	( gfl::heap::Manager::CreateHeapBuffer(buffer,childId,size,heapType,__FILE__,__LINE__) )
#define	GFL_CREATE_HEAP_BUFFER_EX(buffer,childId,size,heapType,param)	( gfl::heap::Manager::CreateHeapBufferEx(buffer,childId,size,heapType,param,__FILE__,__LINE__) )
#else
#define	GFL_CREATE_HEAP(parentHeap,childId,size,heapType,isBack)	( gfl::heap::Manager::CreateHeap(parentHeap,childId,size,heapType,isBack) )
#define	GFL_CREATE_HEAP_BUFFER(buffer,childId,size,heapType)	( gfl::heap::Manager::CreateHeapBuffer(buffer,childId,size,heapType) )
#define	GFL_CREATE_HEAP_BUFFER_EX(buffer,childId,size,heapType,param)	( gfl::heap::Manager::CreateHeapBufferEx(buffer,childId,size,heapType,param) )
#endif

//#pragma mark [> gfl::heap::Manager
class Manager
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�Ǘ�������
   *
   *  @param  void *buf �����o�b�t�@
   *                    �Ǘ��̈���������HEAPID_SYSTEM�Ƃ��Ċm�ۂ���܂�
   *  @param  const int size �����o�b�t�@�̃T�C�Y
   *  @param  const int HeappNum ��`���Ă���HEAPID�̍ő吔�ł�
   */
  //-----------------------------------------------------------------------------
  static void Initialize( void *buf , const int size , const int HeapNum );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�Ǘ��J��
   *
   *  @param  bool isCheck ����Y��`�F�b�N���s��
   *                       �\�t�g���Z�b�g�ȂǓ���ȏ󋵈ȊO�Ńt���O�𗎂Ƃ��Ȃ����ƁI
   */
  //-----------------------------------------------------------------------------
  static void Finalize( bool isCheck = true );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�Ǘ����Z�b�g(�\�t�g���Z�b�g�p)
   *          ���ׂẴq�[�v�̒��g�����ׂĉ�����A�q�[�v���̂�������܂��B
   *          ��O�Ώۂ������Ŏw�肵�܂��B
   *
   *  @param  const HEAPID clearIdArr ���g�͏������A�q�[�v���͎̂c���Ă���HEAPID�̔z��
   *  @param  const int clearIdNum    ���̌�
   *  @param  const HEAPID keepIdArr  ���������Ȃ�HEAPID�̔z��
   *  @param  const int keepIdNum     ���̌�
   */
  //-----------------------------------------------------------------------------
  static void Reset( const HEAPID *clearIdArr , const int clearIdNum ,  const HEAPID *keepIdArr , const int keepIdNum );
  

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�쐬���̎�����T�C�Y�v�Z �m�ۂ���T�C�Y+�Ǘ��̈�
   *          Create�̎��͊Ǘ��̈�͎����Œǉ�����܂�
   *
   *  @param  const int size �m�ۂ������������̃T�C�Y
   *  @param  const HeapType heapType �m�ۂ������q�[�v�̎��
   */
  //-----------------------------------------------------------------------------
  static int CalcCreateHeapSize( const int size , const HeapType heapType );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�쐬 �eHeapBase�w���
   *          �w��T�C�Y�{�Ǘ��̈敪���e�q�[�v����m�ۂ���܂�
   *
   *  @param  HeapBase *parentHeap �e�ƂȂ�HeapBase�̃|�C���^
   *  @param  const HEAPID childId �V�������q�[�v��HEAPID
   *  @param  const int size �m�ۂ��郁�����̃T�C�Y
   *  @param  const HeapType heapType �m�ۂ���q�[�v�̎��
	 *  @param  const bool isBack ����m��
   */
  //-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
  static HeapBase* CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack = false, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeap( HeapBase *parentHeap , const HEAPID childId , const int size , const HeapType heapType , const bool isBack = false );
#endif

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�쐬 �o�b�t�@����쐬��
   *          �w��T�C�Y����Ǘ��̈敪��������A�c��T�C�Y�����ۂ̃q�[�v�T�C�Y�ɂȂ�܂�
   *
   *  @param  void *buffer �q�[�v���쐬����o�b�t�@
   *  @param  const HEAPID childId �V�������q�[�v��HEAPID
   *  @param  const int size �m�ۂ��郁�����̃T�C�Y
   *  @param  const HeapType heapType �m�ۂ���q�[�v�̎��
   */
  //-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
  static HeapBase* CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeapBuffer( void *buffer , const HEAPID childId , const int size , const HeapType heapType );
#endif

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�쐬 �o�b�t�@����쐬��
   *          �w��T�C�Y����Ǘ��̈敪��������A�c��T�C�Y�����ۂ̃q�[�v�T�C�Y�ɂȂ�܂�
   *          ����ȃq�[�v�����ۂɃp�����[�^��n����
   *
   *  @param  void *buffer �q�[�v���쐬����o�b�t�@
   *  @param  const HEAPID childId �V�������q�[�v��HEAPID
   *  @param  const int size �m�ۂ��郁�����̃T�C�Y
   *  @param  const HeapType heapType �m�ۂ���q�[�v�̎��
   *  @param  void *param �q�[�v�p�p�����[�^
   */
  //-----------------------------------------------------------------------------
#if GFL_HEAP_DEBUG
  static HeapBase* CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param, const char *filename=__FILE__, const int linenum=__LINE__ );
#else
  static HeapBase* CreateHeapBufferEx( void *buffer , const HEAPID childId , const int size , const HeapType heapType , void *param );
#endif

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�폜
   *
   *  @param  HeapBase *heap �폜����HeapBase
   */
  //-----------------------------------------------------------------------------
  static void DeleteHeap( HeapBase *heap );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�폜(�`�F�b�N�����Ȃ�)
   *          �q�q�[�v�Ɖ���c�������ׂĉ�����܂��B
   *          ������CreateHeapBuffer�ō��ꂽHeap�͉���ł��܂���B
   *
   *  @param  HeapBase *heap �폜����HeapBase
   */
  //-----------------------------------------------------------------------------
  static void DeleteHeapNoCheck( HeapBase *heap );

  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPID����HeapBase�N���X���擾
   *
   *  @param  const HEAPID heapId �擾�������q�[�v��HEAPID
   *  @return HeapBase* �Ή�����HeapBase�N���X 
   */
  //-----------------------------------------------------------------------------
  static HeapBase* GetHeapByHeapId( const HEAPID heapId );

  //======================================================================
  //  utility�֐�
  //======================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPID�ɑΉ�����HeapBase�����邩�`�F�b�N
   *
   *  @param  const HEAPID heapId ���ׂ����q�[�v��HEAPID
   *  @return bool ture:���݂��� false:���݂��Ȃ�
   */
  //-----------------------------------------------------------------------------
  static bool IsExistHeapId( const HEAPID heapId );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �m�ۂ��ꂽ�̈�̃T�C�Y���擾(HeapBase�ɂ�����܂�)
   *
   *  @param  const void mem ���ׂ郁����
   *  @return int �������T�C�Y(-1�͎��s)
   */
  //-----------------------------------------------------------------------------
  static int GetMemorySize( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �n���ꂽ���������������ǂ������`�F�b�N���܂��B
   *
   *  @param  const void mem ���ׂ郁����
   *  @return bool �`�F�b�N���s��false
   */
  //-----------------------------------------------------------------------------
  static bool CheckMemoryBlock( const void *mem );

  //======================================================================
  //  �f�o�b�O�@�\
  //======================================================================
  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̎擾�󋵂̏o��
   */
  //-----------------------------------------------------------------------------
  static void Dump(void);
  static void DumpTree( HeapBase *heap );

private:
  //�q�[�v�쐬�R�A����
  static HeapBase* CreateHeapCore( void *buffer , const HEAPID childId , const int size , const HeapType heapType , const bool isBuffer , void *param );
  //�q�[�v�폜�R�A(�`�F�b�N�t���O�̗L���̂�
  static void DeleteHeapCore( HeapBase *heap , bool isCheck = true );

  //�Ǘ��f�[�^
  static HEAP_MNG_DATA *m_heapMngData;
  //�ő��
  static int m_heapNum;

};

} //namespace heap
} //namespace gfl

#endif // __GFL_HEAPMANAGER_H__
