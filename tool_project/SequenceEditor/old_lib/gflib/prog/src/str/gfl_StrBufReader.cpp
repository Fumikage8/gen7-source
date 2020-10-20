//===========================================================================
/**
 * @file    gfl_StrBufReader.cpp
 * @brief   GAMEFREAK Library  String Print System ( StrBuf Reader )
 * @author  GAMEFREAK inc.
 * @date    2010.10.27
 */
//===========================================================================

#include <gfl_Common.h>
#include <gfl_Heap.h>

#include <str/gfl_StrBufReader.h>


namespace gfl {
namespace str {

enum {
  READER_PRINT_FLAG = (gfl::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl::debug::PRINT_FLAG_LEVEL1)
};


//----------------------------------------------------------------------
/**
 * @brief   �R���X�g���N�^
 *
 * @param[in]   srcBuf  �ǂݎ�錳������
 * @param[in]   pHeap   �������m�ۗp�q�[�v�n���h��
 */
//----------------------------------------------------------------------

StrBufReader::StrBufReader( gfl::heap::HeapBase* pHeap )
 : m_readPtr(0), m_replaceCode(EOM_CODE), m_tagProceeded(false), m_bAvailable(false)
{
  m_latestTagCode = TAG_USER_NULL;
  m_srcLength = 0;
  m_bufLength = TEMP_BUFFER_SIZE_MIN;
  m_buffer = GFL_NEW(pHeap) StrBuf( m_bufLength, pHeap );
  if( m_buffer )
  {
    m_replaceCode = *(m_buffer->GetPtr());
    m_bAvailable = true;
  }
}

//----------------------------------------------------------------------
/**
 * @brief   �f�X�g���N�^
 */
//----------------------------------------------------------------------
StrBufReader::~StrBufReader()
{
  if( m_buffer )
  {
    GFL_DELETE( m_buffer );
  }
}

//----------------------------------------------------------------------
/**
 * @brief   ���̃��\�b�h���Ăяo�����ƂɂP���������e�������� StrBuf �ւ̎Q�Ƃ��Ԃ�
 *          �^�O�J�n�R�[�h�`�^�O�p�����[�^�����͏��O�����
 *          ���̃��\�b�h�Ăяo���ɂ��^�O�J�n�R�[�h�����O���ꂽ���ǂ������肵�����ꍇ�AGetLatestTagCode ���g��
 *
 * @retval  const StrBuf&
 */
//----------------------------------------------------------------------
const StrBuf&
StrBufReader::ReadNext( void )
{
  GFL_ASSERT(m_bAvailable);

  if( m_readPtr < m_srcLength )
  {
    if( m_replaceCode != TAG_START_CODE )
    {
      u16 prevCode = m_replaceCode;
      m_buffer->replaceCode( m_readPtr++, m_replaceCode );
      m_replaceCode = m_buffer->replaceCode( m_readPtr, EOM_CODE );
      m_latestTagCode = TAG_USER_NULL;
      m_tagProceeded = false;
    }
    else
    {
      // �^�O�J�n�R�[�h�����߂�
      m_buffer->replaceCode( m_readPtr, m_replaceCode );

      const STRCODE* sp = m_buffer->GetPtr() + m_readPtr;
      const STRCODE* next_sp;

      // �^�O�p�����[�^��ۑ�
      m_latestTagCode = GetTagCode( sp );
      m_latestTagParamCount = GetTagParamCount( sp );

      next_sp = SkipTagCode( sp );
      while( sp != next_sp ){
        m_buffer->replaceCode( m_readPtr, *sp );
        ++m_readPtr;
        ++sp;
      }

      m_replaceCode = m_buffer->replaceCode( m_readPtr, EOM_CODE );
      m_tagProceeded = true;

    }
  }

  return *(m_buffer);
}

//----------------------------------------------------------------------
/**
 * @brief     ReadNext�ŏ������ꂽ�^�O�R�[�h���擾
 *
 * @param[out]   dst      �������ꂽ�^�O�R�[�h���󂯎��
 *
 * @retval       bool     ���O��ReadNext�Ń^�O�R�[�h���������Ă����ꍇ�Adst�ɗL���Ȓl��ݒ肵true��Ԃ��B ����ȊO false�B
 */
//----------------------------------------------------------------------
bool
StrBufReader::GetLatestTagCode( STRCODE* dst ) const
{
  if( m_tagProceeded )
  {
    *dst = m_latestTagCode;
    return true;
  }
  else
  {
    *dst = 0;
    return false;
  }
}

//----------------------------------------------------------------------
/**
 * @brief     �Ō�܂œǂݏo����������
 *
 * @retval    bool    �Ō�܂œǂ�ł����� true
 */
//----------------------------------------------------------------------
bool
StrBufReader::IsEnd( void ) const
{
  GFL_ASSERT(m_bAvailable);

  return (m_readPtr >= m_srcLength);
}




//----------------------------------------------------------------------
/**
 * ���ɓǂݏo�������R�[�h���擾
 *
 * @retval  STRCODE
 */
//----------------------------------------------------------------------
STRCODE
StrBufReader::GetNextCode( void ) const
{
  return m_replaceCode;
}

//----------------------------------------------------------------------
/**
 * @brief       �ǂݏo���������؂�ւ���
 *
 * @param[in]   srcBuf    �ǂݏo������������
 * @param[in]   pHeap     �����o�b�t�@�� srcBuf ���i�[������Ȃ��ꍇ�A
 *                        �V���Ƀo�b�t�@���\�z���������߂̃q�[�v
 */
//----------------------------------------------------------------------
void
StrBufReader::SetString( const StrBuf& srcBuf, gfl::heap::HeapBase* pHeap )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    // �V�����n���ꂽ�����񂪌��݂̃o�b�t�@�Ŏ��܂肫��Ȃ��ꍇ�Ɋg������
    m_srcLength = srcBuf.GetLength();
    if( m_srcLength > m_bufLength )
    {
      m_bufLength = m_srcLength;
      GFL_DELETE( m_buffer );
      m_buffer = GFL_NEW(pHeap) StrBuf( m_bufLength, pHeap );
    }
    if( m_buffer != NULL )
    {
      m_readPtr = 0;
      m_buffer->SetStr( srcBuf );
      m_replaceCode = *(m_buffer->GetPtr());
    }
    else
    {
      m_bAvailable = false;
    }
  }
}

void StrBufReader::SetString( const STRCODE* str, u32 strLen, gfl::heap::HeapBase* pHeap )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    // �V�����n���ꂽ�����񂪌��݂̃o�b�t�@�Ŏ��܂肫��Ȃ��ꍇ�Ɋg������
    m_srcLength = strLen;
    if( m_srcLength > m_bufLength )
    {
      m_bufLength = m_srcLength;
      GFL_DELETE( m_buffer );
      m_buffer = GFL_NEW(pHeap) StrBuf( m_bufLength, pHeap );
    }
    if( m_buffer != NULL )
    {
      m_buffer->Clear();
      while( strLen-- )
      {
        m_buffer->AddCode( *str++ );
      }
      m_readPtr = 0;
      m_replaceCode = *(m_buffer->GetPtr());
    }
    else
    {
      m_bAvailable = false;
    }
  }
}

//----------------------------------------------------------------------
/**
 * @brief     �ǂݏo���|�C���^�����Z�b�g�i���� ReadNext() ���ĂԂƁA�ŏ��̂P���������Ԃ�j
 */
//----------------------------------------------------------------------
void
StrBufReader::Reset( void )
{
  GFL_ASSERT(m_bAvailable);

  if( m_bAvailable )
  {
    m_readPtr = 0;
    m_replaceCode = *(m_buffer->GetPtr());
  }
}


} /* end of namespace str */
} /* end of namespace gfl */



