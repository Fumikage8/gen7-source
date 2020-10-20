#ifndef __GFL_HEAPBASE_H__
#define __GFL_HEAPBASE_H__
//======================================================================
/**
 * @file  gfl_HeapBase.h
 * @brief GFL�q�[�v���N���X
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

//�������̓��ɂ��w�b�_�[
//12���ً}�Ή��I
static const int MEMORY_HEADER_SIZE = 32;
//static const int MEMORY_HEADER_SIZE = 16;
static const int MEMORY_HEADER_USER_SIZE = MEMORY_HEADER_SIZE-6;
typedef struct
{
  //�q�[�vID
  HEAPID  heapId;
  //�������擪�ʒu�܂ł̃I�t�Z�b�g
  u16     offset;
  //���[�U�[�f�[�^
  u8      userData[MEMORY_HEADER_USER_SIZE];
}MEMORY_HEADER;

//�������̓��ɂ��w�b�_�[�̃f�t�H���g���[�U�[�w�b�_�[
static const int MEMORY_USER_HEADER_SIZE = MEMORY_HEADER_USER_SIZE;
#if GFL_HIO_ENABLE
static const int MEMORY_USER_HEADER_FILE_BUFLEN = MEMORY_USER_HEADER_SIZE - 7;
#else
static const int MEMORY_USER_HEADER_FILE_BUFLEN = MEMORY_USER_HEADER_SIZE-3;
#endif
typedef struct
{
#if GFL_HIO_ENABLE
  //�A���C�����g
  s16 align;
  //���C���C���f�b�N�X(HIO�o�R��PC�c�[���Ń��C���폜�Ɏg���C���f�b�N�X�j
  u16 line_index;
#endif
  //�\�[�X�̍s
  u16 line;
  //���[�U�[�w��ID
  u8  id;
  //�\�[�X��
  char fileName[MEMORY_USER_HEADER_FILE_BUFLEN];
}MEMORY_USER_HEADER;

//�������쐬���ɐ��������f�[�^
typedef struct {
  MEMORY_USER_HEADER  sourceFile;   // �쐬�����\�[�X�t�@�C��
  bool  parentFlag;                 // �e�����邩
}MEMORY_USER_CREATE_DATA;

//----------------------------------------------------------------------------
/**
 *  @brief  ���������m��
 *          �������̊m�ۂ͂��̃}�N�����g���čs���Ă��������B
 *          AllocMemoryBlock��AllocMemoryBlockAlign�𒼐ڎg�p���Ȃ��ł��������B
 *
 *  @param  heap             HeapBase�N���X(����) �������� HEAPID(�Ăяo���֐����I�[�o�[���[�h���Ă��܂�)
 *  @param  const int size   �m�ۃT�C�Y
 *  @param  const int align  �A���C�����g(�}�C�i�X�Ō������m��)
 *
 *  @return void* �m�ۃ�����(0�N���A����Ă��܂�)
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
 *  @brief  ���������J��
 *          �֐�������̂��ߗp�ӂ��Ă��܂�
 *
 *  @param  void  �J��������
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
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(GflHeapBase);

  friend class Manager;
  friend class GflHeapBaseLower;
public:
  GflHeapBase();
  virtual ~GflHeapBase();

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���������J��
   *          GflHeapFreeMemoryBlock���g�p���Ă�������
   *          �m�ۂ��s����HeapBase���g���ĊJ�����Ă�������
   *
   *  @param  void  �J��������
   */
  //-----------------------------------------------------------------------------
  void FreeMemory( void* buf );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �����Őe�q�[�v�𔻒f�����������J��
   *          GflHeapFreeMemoryBlock���g�p���Ă�������
   *
   *  @param  void  �J��������
   */
  //-----------------------------------------------------------------------------
  static void FreeMemoryAuto( void* buf );

  //----------------------------------------------------------------------------
  /**
   *  @brief  �m�ۂ����������̃��T�C�Y
   *
   *  @param  void* buf ���T�C�Y���郁����
   *  @param  int       �ύX��̃T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual void ResizeMemory( void* buf , const int newSize){GFL_UNUSED(buf);GFL_UNUSED(newSize);};

  //----------------------------------------------------------------------------
  /**
   *  @brief  �������󋵂̕\��
   *          �Ǝ��ŗp�ӂ���MemoryHeader�����Ƀ_���v���܂�
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void Dump(void){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  �������󋵂̕\��
   *          �Ǝ��ŗp�ӂ���MemoryHeader�����Ƀ_���v���܂��B�@�w�肵��ID�̏��̂݃_���v�����
   *
   *  @param  id   SetDebugUserID �ł��Ă��Ă���ID
   */
  //-----------------------------------------------------------------------------
  virtual void DumpId(u8 /*id*/){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  �������󋵂̕\��
   *          ���C�u�����Ȃǂŗp�ӂ��ꂽ�֐����g���_���v���܂�
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void SystemDump(void){};

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�j��`�F�b�N
   *
   *  @param  none
   *  @return  bool true���Ă��Ȃ� false���Ă���
   */
  //-----------------------------------------------------------------------------
  virtual bool Check(void){return true;};

  //----------------------------------------------------------------------------
  /**
   *  @brief  �f�t�H���g�A���C�����g�l�̎擾
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual int GetDefaultAlign(void) = 0;

  //----------------------------------------------------------------------------
  /**
   *  @brief  HEAPID�̎擾
   *
   *  @return  HEAPID
   */
  //-----------------------------------------------------------------------------
  virtual HEAPID GetHeapId(void) const{ return m_heapId; };

  //----------------------------------------------------------------------------
  /**
   *  @brief  �A���C�����g��n���w�b�_�[���݂Ŏ��ۂ̃������m�ۗʂ��v�Z
   *          32�o�C�g�ȏ�̃A���C�����g��ݒ肵���ꍇ�A���̃T�C�Y���w�b�_�[��
   *          �m�ۂ���܂��B
   *
   *  @return  int ���m�ۃT�C�Y
   */
  //-----------------------------------------------------------------------------
  int CalcAllocateAdjustSize( const int align ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̃T�C�Y���擾���܂�
   *
   *  @return  int �T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalSize(void){return 0;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̍��v�󂫃T�C�Y���擾���܂�
   *
   *  @return  int �T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalFreeSize(void){return 0;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�Ŋm�ۂł���ő�̃T�C�Y���擾���܂�
   *
   *  @return  int �T�C�Y
   */
  //-----------------------------------------------------------------------------
  virtual int GetTotalAllocatableSize(void){return 0;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̐擪�A�h���X��Ԃ��܂�(�Ǘ��̈���܂܂Ȃ�
   *
   *  @return  void*
   */
  //-----------------------------------------------------------------------------
  virtual void* GetStartAddress(void){return NULL;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v����Ɍ���m�ۂ���n���h�����擾���܂�
   */
  //-----------------------------------------------------------------------------
  virtual HeapBase* GetLowerHandle();

  //----------------------------------------------------------------------------
  /**
   *  @brief  �m�ۂ��ꂽ�̈�̃T�C�Y���擾(HeapManager�ɂ�����܂�)
   *          �p�����Ă��Ȃ���Δ�Ή�
   *
   *  @param  const void mem ���ׂ郁����
   *  @return int �������T�C�Y(-1�͎��s)
   */
  //-----------------------------------------------------------------------------
  virtual int GetMemorySize( const void *mem ){return -1;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  �n���ꂽ���������������ǂ������`�F�b�N���܂��B
   *
   *  @param  const void mem ���ׂ郁����
   *  @return bool �`�F�b�N���s��false
   */
  //-----------------------------------------------------------------------------
  virtual bool CheckMemoryBlock( const void *mem );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ���ׂẴ�������������܂��B
   *
   *  @return bool ���s������false
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

  // ������������̃������N���A�ݒ�
  enum DebugFreeMemClearType
  {
    DEBUG_FREE_MEMC_NONE = 0,     ///< �N���A���Ȃ�
    DEBUG_FREE_MEMC_ZERO,     ///< �[���ŃN���A
    DEBUG_FREE_MEMC_FF,       ///< 0xFF�ŃN���A

    DEBUG_FREE_MEMC_TYPE_MAX,
  };
  static inline void SetDebugFreeMemClear( DebugFreeMemClearType type ){ mDebugFreeMemClear = type; }
  static inline DebugFreeMemClearType GetDebugFreeMemClear( void ){ return (DebugFreeMemClearType)mDebugFreeMemClear; }
#endif

//Heap�̊m�ۂ͏�̃}�N�����g���čs���Ă��������B
//AllocMemoryBlock��AllocMemoryBlockAlign�𒼐ڎg�p���Ȃ��ł��������B
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

  //���ۂ̃������擪�擾
  //FreeMemoryAuto����ĂԂ̂�static
  static void* GetMemoryTop( const void *buf );
  //�w�b�_�[�ʒu�擾
  //FreeMemoryAuto����ĂԂ̂�static
  static MEMORY_HEADER* GetMemoryHeader( const void *buf );
  //�w�b�_�[�T�C�Y�擾(GetMemoryTop�̑O�̃A�h���X��n��
  int GetMemoryHeaderSize( const void *buf );

  //AllocCount�̑���(HeapBaseLower���瑀�삪����̂ł��������g���I)
  virtual int GetAllocCount(void) const{ return m_allocCount; };
  virtual void IncAllocCount(void);
  virtual void DecAllocCount(void);

#if GFL_HIO_ENABLE
  bool  mHioDump;
  u32   mHioLineIndex;
#endif

private:

  //HeapBaseLower���瑀�삪����̂Œ��ڎg��Ȃ����ƁI
  HEAPID m_heapId;
  int    m_allocCount;

  //�������m�ۂ��邽�߂̃n���h��
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

  //�ȉ��������z�֐�
  //�p����Ŏ������Ă��������B
  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̍쐬
   *          friend��gfl::heap::Manager����̂݌Ăяo��
   *
   *  @param  void *buf �m�ۗp�o�b�t�@
   *  @param  const int size �o�b�t�@�T�C�Y
   *  @param  void* param ���[�U�[�p�����[�^�[
   */
  //-----------------------------------------------------------------------------
  virtual void CreateHeap( void *buf , const int size , void *param ) = 0;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �q�[�v�̍폜
   *          friend��gfl::heap::Manager����̂݌Ăяo��
   *
   *  @param  none
   */
  //-----------------------------------------------------------------------------
  virtual void DeleteHeap(void) = 0;

  //���ۂ̃������m�ہE�J������
  virtual void* AllocAlign( const int size , const int align ) = 0;
  virtual void Free( void* buf ) = 0;
  //�����܂ŏ������z�֐�


  //friend��gfl::heap::Manager����̂݌Ăяo��.HeapBase�̏�����
  void InitHeapBase( const HEAPID heapId );
  //friend��gfl::heap::Manager����̂݌Ăяo��.HeapBase�̊J��
  //isCheck�̓\�t�g���Z�b�g�̎��Ȃǂ�false�ɂȂ�B�c���Ă��Ă��������
  void TermHeapBase( bool isCheck = true );

  //friend��gfl::heap::Manager����̂݌Ăяo��LowerHandle�̐ݒ�
  virtual void SetLowerHandle( GflHeapBaseLower *handle );

#if GFL_HEAP_DEBUG
  //�������m�ۃR�A����
  void* AllocMemoryCore(  const int size , const int align , const bool isClearMemory , const char *filename , const u16 linenum );
#if GFL_HIO_ENABLE
  //�w�b�_�[�Z�b�g
  void SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset , const char *filename , const u16 linenum, const int align );
  //���[�U�[�w�b�_�[�Z�b�g(�f�t�H���g)
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum, const int align );
#else
  //�w�b�_�[�Z�b�g
  void SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset , const char *filename , const u16 linenum );
  //���[�U�[�w�b�_�[�Z�b�g(�f�t�H���g)
  virtual void SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum );
#endif
#else
  //�������m�ۃR�A����
  void* AllocMemoryCore(  const int size , const int align , const bool isClearMemory );
  //�w�b�_�[�Z�b�g
  void SetMemoryHeader( MEMORY_HEADER *buf , const u32 offset );
  //���[�U�[�w�b�_�[�Z�b�g(�f�t�H���g)
  virtual void SetMemoryHeader_UserData( void *userHeader );
#endif

#if GFL_HIO_ENABLE
void  HioLineDel( HEAPID heapId, void* userHeader );
#endif

};

} //namespace heap
} //namespace gfl

#endif // __GFL_HEAPBASE_H__
