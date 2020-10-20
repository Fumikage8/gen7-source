//===========================================================================
/**
 * @file    gfl_StrBuf.cpp
 * @brief   GAMEFREAK Library  String Print System ( ������o�b�t�@�N���X )
 * @author  GAMEFREAK inc.
 * @date    2010.10.27
 */
//===========================================================================
#include <gfl_Common.h>
#include <gfl_Heap.h>

#include <str/gfl_StrBuf.h>


namespace gfl {
namespace str {

//----------------------------------------------------------------------
// �f�o�b�O�o�̓t���O
//----------------------------------------------------------------------
enum {
  STRBUF_PRINT_FLAG = (gfl::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl::debug::PRINT_FLAG_LEVEL3)
};

//----------------------------------------------------------------------
/**
 * �R���X�g���N�^
 */
//----------------------------------------------------------------------

// ���e�����������n���č\�z
StrBuf::StrBuf( const STRCODE* string, gfl::heap::HeapBase* pHeap )
 : m_bAvailable(false)
{
  GFL_ASSERT(string);

  m_strLength       = StrLen( string );
  m_numBufferElems  = m_strLength + 1;
  m_buffer          = static_cast<STRCODE*> GflHeapAllocMemory( pHeap, sizeof(STRCODE) * m_numBufferElems );
  if( m_buffer )
  {
    StrNCopy( m_buffer, string, m_numBufferElems );
    m_bAvailable = true;
  }
}

// StrBuf ��n���č\�z�iCopy�j
StrBuf::StrBuf( const StrBuf& src, gfl::heap::HeapBase* pHeap )
 : m_bAvailable(false)
{
  m_strLength       = src.m_strLength;
  m_numBufferElems  = src.m_numBufferElems;
  m_buffer          = static_cast<STRCODE*> GflHeapAllocMemory( pHeap, sizeof(STRCODE) * m_numBufferElems );
  if( m_buffer )
  {
    StrNCopy( m_buffer, src.m_buffer, m_numBufferElems );
    m_bAvailable = true;
  }
}

// �i�[�ł��镶�����i�I�[�����܂ށj���w�肵�ď�����
StrBuf::StrBuf( u32 numCharMax, gfl::heap::HeapBase* pHeap )
 : m_bAvailable(false)
{
  GFL_ASSERT(numCharMax);

  m_strLength       = 0;
  m_numBufferElems  = numCharMax;
  m_buffer          = static_cast<STRCODE*> GflHeapAllocMemory( pHeap, sizeof(STRCODE) * m_numBufferElems );
  if( m_buffer )
  {
    m_buffer[0] = EOM_CODE;
    m_bAvailable = true;
  }
}

//----------------------------------------------------------------------
/**
 * �f�X�g���N�^
 */
//----------------------------------------------------------------------
StrBuf::~StrBuf()
{
  if( m_buffer )
  {
    GflHeapFreeMemory( m_buffer );
  }
}

//==========================================================================================
//==========================================================================================

//----------------------------------------------------------------------
/**
 * @brief   ������|�C���^���擾
 *
 * @retval  const STRCODE*  �i�[����Ă��镶����ւ̃|�C���^
 */
//----------------------------------------------------------------------
const STRCODE*
StrBuf::GetPtr( void ) const
{
  GFL_ASSERT(m_bAvailable);

  return m_buffer;
}

//----------------------------------------------------------------------
/**
 * @brief   �i�[����Ă��镶�����i�I�[�������܂܂Ȃ��j���擾
 *
 * @retval  u32   ������̒���
 */
//----------------------------------------------------------------------
u32
StrBuf::GetLength( void ) const
{
  GFL_ASSERT(m_bAvailable);

  return m_strLength;
}

//----------------------------------------------------------------------
/**
 * @brief   �i�[�\�ȕ������i�I�[�������܂ށj���擾
 *
 * @retval  u32   �i�[�\�ȕ�����
 */
//----------------------------------------------------------------------
u32
StrBuf::GetSize( void ) const
{
  GFL_ASSERT(m_bAvailable);

  return m_numBufferElems;
}

//----------------------------------------------------------------------
/**
 * @brief   ��������O���o�b�t�@�ɏo��
 *
 * @param[out]  array        �o�͐�z��
 * @param[in]   numAryElems  array �̗v�f��
 *
 * @retval      u32    �R�s�[�����������i�I�[�������܂܂Ȃ��j
 */
//----------------------------------------------------------------------
u32
StrBuf::PutStr( STRCODE* array, u32 numAryElems ) const
{
  GFL_ASSERT(m_bAvailable);
  GFL_ASSERT(numAryElems);

  if( m_bAvailable && numAryElems )
  {
    return StrNCopy( array, m_buffer, numAryElems);
  }

  return 0;
}

//----------------------------------------------------------------------
/**
 * @brief   ��������Z�b�g����i�N���X�g�p�j
 *
 * @param[in]   src      �Z�b�g���镶����i�N���X�j
 *
 * @retval      u32      �Z�b�g�����������i�I�[�������܂܂Ȃ��j
 */
//----------------------------------------------------------------------
u32
StrBuf::SetStr( const StrBuf& src )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_strLength =  StrNCopy( m_buffer, src.GetPtr(), m_numBufferElems);
    GFL_PRINT_EX( STRBUF_PRINT_FLAG, "[STRBUF] Set from Class, 1stCode=%04x, len=%d\n",
            *(src.GetPtr()), m_strLength);
    return m_strLength;
  }
  return 0;
}
//----------------------------------------------------------------------
/**
 * @brief   ��������Z�b�g����i null�����I�[ �z��g�p�j
 *
 * @param[in]   src      �Z�b�g���镶����i�z��j
 *
 * @retval      u32      �Z�b�g�����������i�I�[�������܂܂Ȃ��j
 */
//----------------------------------------------------------------------
u32
StrBuf::SetStr( const STRCODE* src )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_strLength = StrNCopy( m_buffer, src, m_numBufferElems );
    return m_strLength;
  }
  return 0;
}

//----------------------------------------------------------------------
/**
 * @brief       �I�[�ɕ�����ǉ�
 *
 * @param[in]   buf   �ǉ����镶����
 *
 * @retval      u32   �A����̕������i�I�[�������܂܂Ȃ��j
 */
//----------------------------------------------------------------------
u32
StrBuf::AddStr( const StrBuf& buf )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    u32  restLen = m_numBufferElems - m_strLength;
    m_strLength += StrNCopy( &m_buffer[m_strLength], buf.GetPtr(), restLen );
    return m_strLength;
  }
  return 0;
}
//----------------------------------------------------------------------
/**
 * @brief   �I�[�ɂP�����ǉ�
 *
 * @param[in]   code     �ǉ�������
 */
//----------------------------------------------------------------------
bool
StrBuf::AddCode( STRCODE code )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    if( (m_strLength+1) < m_numBufferElems )
    {
      m_buffer[ m_strLength++ ] = code;
      //1�����ǉ������璼��ɏI�[����������
      m_buffer[ m_strLength ] = EOM_CODE;
      return true;
    }
    else if( (m_strLength+1) == m_numBufferElems && code == EOM_CODE )
    {
      //�o�b�t�@�����ɏ������߂�̂͏I�[�R�[�h����
      m_buffer[ m_strLength++ ] = EOM_CODE;
      return true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "bufSize=%d, strLen=%d", m_numBufferElems, m_strLength);
    }
  }
  return false;
}

//----------------------------------------------------------------------
/**
 *  @brief  AddCode�ŕ������}�����ꂽ��A�����̕����񒷂��v�����Ȃ���
 */
//----------------------------------------------------------------------
void
StrBuf::AddCodeFinish( void )
{
  if( m_bAvailable )
  {
    m_strLength = StrLen( m_buffer );
  }
}


//----------------------------------------------------------------------
/**
 * @brief   �󕶎�����Z�b�g����
 */
//----------------------------------------------------------------------
void StrBuf::Clear( void )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_buffer[0] = EOM_CODE;
    m_strLength = 0;
  }
}

//----------------------------------------------------------------------
/**
 * @brief   ������̔�r
 *
 * @param[in]   buf      ��r������
 *
 * @retval      bool     ��r������Ɠ��e����v���Ă���� true
 */
//----------------------------------------------------------------------
bool
StrBuf::Compare( const StrBuf& buf ) const
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    return StrComp( m_buffer, buf.m_buffer );
  }
  return false;
}

//================================================================================
// private methods
//================================================================================

//----------------------------------------------------------------------
/**
 * @brief   �w��ʒu�̕����R�[�h���擾
 *
 * @param[in]   index     �擾���镶������ʒu�i0 origin�j
 *
 * @retval      STRCODE   �u���O�̕����R�[�h
 */
//----------------------------------------------------------------------
STRCODE
StrBuf::getCode( u32 index ) const
{
  GFL_ASSERT(m_bAvailable);

  if( index < m_numBufferElems )
  {
    return m_buffer[ index ];
  }
  else
  {
    GFL_ASSERT( 0 );
    return EOM_CODE;
  }
}

//----------------------------------------------------------------------
/**
 * @brief   �w��ʒu�̕����R�[�h��u��
 *
 * @param[in]   index     �u�����镶������ʒu�i0 origin�j
 * @param[in]   code      �u���㕶���R�[�h
 *
 * @retval      STRCODE   �u���O�̕����R�[�h
 */
//----------------------------------------------------------------------
STRCODE
StrBuf::replaceCode( u32 index, STRCODE code )
{
  GFL_ASSERT(m_bAvailable);

  STRCODE oldCode = EOM_CODE;

  if( index < m_numBufferElems )
  {
    oldCode = m_buffer[ index ];
    m_buffer[ index ] = code;
    m_strLength = StrLen( m_buffer );
  }
  else
  {
    GFL_ASSERT( 0 );
  }

  return oldCode;
}
//----------------------------------------------------------------------
/**
 * @brief   �w��ʒu���當������㏑���R�s�[����
 *
 * @param[in]   index     �㏑���J�n���镶������ʒu�i0 origin�j
 * @param[in]   src       �㏑�����镶����
 */
  //----------------------------------------------------------------------
void
StrBuf::replaceStr( u32 index, const STRCODE* src )
{
  GFL_ASSERT(m_bAvailable);

  if( index < m_numBufferElems )
  {
    unsigned int buf_len = m_numBufferElems - index;
    StrNCopy( &m_buffer[index], src, buf_len );
    m_strLength = StrLen( m_buffer );
  }
}



} /* end of namespace str */
} /* end of namespace gfl */



