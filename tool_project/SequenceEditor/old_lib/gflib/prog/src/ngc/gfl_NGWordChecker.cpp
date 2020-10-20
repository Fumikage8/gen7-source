//================================================================================
/**
 * @file   gfl_NGWordChecker.cpp
 * @brief  NG���[�h�`�F�b�N�p�N���X
 * @author hisanao suzuki
 * @date   2012.2.15
 */
//================================================================================
#include <base/gfl_Singleton.h>
#include <ngc/gfl_NGWordChecker.h>
#include "gfl_NGWordCheckThread.h"

//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::ngc::NGWordChecker);
template class gfl::base::SingletonAccessor<gfl::ngc::NGWordChecker>;
#endif


namespace gfl {
namespace ngc {


/** ---------------------------------------------------------------------
* @brief  �R���X�g���N�^
* @return ����
// ---------------------------------------------------------------------*/
NGWordChecker::NGWordChecker( void )
{
  m_isOverWrite = false;
  m_isInit      = false;
  m_workMemory  = NULL;
  m_thread      = NULL;
}


/** ---------------------------------------------------------------------
* @brief  �f�X�g���N�^
* @param  ����
* @return ����
// ---------------------------------------------------------------------*/
NGWordChecker::~NGWordChecker()
{
  Finalize();
}


/** ---------------------------------------------------------------------
* @brief  ����������
* @param  heap ���[�N�̈�Ƃ��Ďg�p�����q�[�v�G���A�B
*         ���悻64KB��������B
* @return ����I������true�A�ȏ�I����false��ԋp
// ---------------------------------------------------------------------*/
bool NGWordChecker::Initialize( gfl::heap::HeapBase *heap )
{
  nn::Result result = nn::ResultSuccess();

  if( !m_isInit )
  {
    if( m_workMemory == NULL )
    {
      m_workMemory = GflHeapAllocMemoryAlign( heap, WORKMEMORY_SIZE, 4 );
      if( m_workMemory == NULL ){
        return false;
      }
    }

    result = m_filter.Initialize( (uptr)m_workMemory );
    if( result == nn::ResultSuccess() )
    {
      m_isInit = true;
      SetMaskMode( m_isOverWrite );
    }
  }
  return result.IsSuccess();
}


/** ---------------------------------------------------------------------
* @brief  �I������
* @param  ����
* @return ����I������true�A�ȏ�I����false��ԋp
// ---------------------------------------------------------------------*/
bool NGWordChecker::Finalize( void )
{
  nn::Result result = nn::ResultSuccess();

  this->CancelAsync();
  this->DeleteAsync();

  if( m_isInit )
  {
    result = m_filter.Finalize();
    if( result == nn::ResultSuccess() )
    {
      m_isInit = false;
    }
    if( m_workMemory )
    {
      GflHeapFreeMemory( m_workMemory );
      m_workMemory = NULL;
    }
  }
  return result.IsSuccess();
}


/** ---------------------------------------------------------------------
* @brief  ������Ɋ܂܂��A����\�������̌����擾����
* @param  pString ���ׂ�Ώۂ�NULL�I�[������
* @return ����\�������̌�
// ---------------------------------------------------------------------*/
int NGWordChecker::CountNumbers( const wchar_t * pString )
{
  return nn::ngc::CTR::CountNumbers( pString );
}


/** ---------------------------------------------------------------------
* @brief  ���͂��`�F�b�N���A���̂�����𔭌��������̋�����ݒ�
* @param  bOverWrite [true]  �����̕������ׂĂ��A�X�^���X�N�L���ŏ㏑��
*                    [false] �����𕶎����ɂ�炸1�����̃A�X�^���X�N�L���ɒu��
* @return ����
// ---------------------------------------------------------------------*/
void NGWordChecker::SetMaskMode( bool bOverWrite )
{
  m_isOverWrite = bOverWrite;
  m_filter.SetMaskMode( bOverWrite );
}


/** ---------------------------------------------------------------------
* @brief  �P����`�F�b�N���A���̂�����𔭌��������̋�����ݒ�
* @param  checkResult    NG���[�h�����݂���Ȃ�true���i�[
* @param  nPatternCode   �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
* @param  ppWords        �����ΏۂƂȂ�NULL�I�[������̔z��i�����R�[�h�FUTF16���g���G���f�B�A���j
* @param  nWordCount     �����Ώە�����̔z��(�ő�16�܂Łj
* @return ���s����
// ---------------------------------------------------------------------*/
nn::Result NGWordChecker::CheckProfanityWords( bool* checkResult, ProfanityFilterPatternList nPatternCode, const wchar_t ** ppWords, size_t nWordCount )
{
  bit32      ngBits;
  nn::Result result;

  if( nPatternCode == ALL_REGION ){
    result = m_filter.CheckProfanityWords( &ngBits, ppWords, nWordCount );
  }
  else{
    result = m_filter.CheckProfanityWords( &ngBits, nPatternCode, ppWords, nWordCount );
  }

  if( result.IsSuccess() )
  {
    if( checkResult != NULL ){
      *checkResult = (ngBits!=0);
    }
  }

  return result;
}


/** ---------------------------------------------------------------------
* @brief  �C�V���̃��M�����[�V�����ƏƂ炵���킹�A�t���[���[�h�����Ȃ����`�F�b�N����(�񓯊��֐�)
*         �ECheckProfanityWords()��NG�ƂȂ�Ȃ�
*         �EMaskProfanityWordsInText()��NG�ƂȂ�Ȃ�
*         �E16�����ȓ�
*         �E������5�����ȓ�
*
* @attention �I���C�x���g�ʒm��A�K��DeleteAsync()���R�[�����Ă��������B
*
* @param  nPatternCode    �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
* @param  pText           �����ΏۂƂȂ�NULL�I�[������
* @param  listener        ����������ʒm���邽�߂̃��X�i
* @param  thread_priority �X���b�h�̎��s�D��x
*
* @return �������J�n�ł����Ȃ�true�A���s�����Ȃ�false��ԋp
// ---------------------------------------------------------------------*/
bool NGWordChecker::CheckFreewordWithRegulationASync( gfl::heap::HeapBase* heap,
                                                      ProfanityFilterPatternList nPatternCode, wchar_t *pText,
                                                      NGWordCheckListener* listener, gfl::base::Thread::Priority thread_priority )
{
  this->CancelAsync();
  this->DeleteAsync();

  m_thread = GFL_NEW_LOW( heap ) NGWordCheckThread( heap, NGWORD_STACK_SIZE, listener );

  if( m_thread == NULL ){
    return false;
  }
  m_thread->Setup( nPatternCode, pText, this );
  m_thread->Start( thread_priority );
  
  return true;
}


/** ---------------------------------------------------------------------
* @brief  ���͂��`�F�b�N���A���̂�����𔭌��������̋�����ݒ�
* @param  pProfanityWordCount ���͒��ɕs�������񂪉��񌻂ꂽ�����擾���邽�߂̃|�C���^
* @param  nPatternCode        �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
* @param  pText               �����ΏۂƂȂ�NULL�I�[������BNG�ӏ���*�ɒu����������
* @return ���s����
// ---------------------------------------------------------------------*/
nn::Result NGWordChecker::MaskProfanityWordsInText( int* pProfanityWordCount, ProfanityFilterPatternList nPatternCode, wchar_t *pText )
{
  if( nPatternCode == ALL_REGION ){
    return m_filter.MaskProfanityWordsInText( pProfanityWordCount, pText );
  }
  else{
    return m_filter.MaskProfanityWordsInText( pProfanityWordCount, nPatternCode, pText );
  }
}


/** ---------------------------------------------------------------------
* @brief  �C�V���̃��M�����[�V�����ƏƂ炵���킹�A�t���[���[�h�����Ȃ����`�F�b�N����
*         �ECheckProfanityWords()��NG�ƂȂ�Ȃ�
*         �EMaskProfanityWordsInText()��NG�ƂȂ�Ȃ�
*         �E�P�U�����ȓ�
*         �E������5�����ȓ�
*
* @param  nPatternCode    �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
* @param  pText           �����ΏۂƂȂ�NULL�I�[������
* @return ��������
// ---------------------------------------------------------------------*/
gfl::ngc::Result NGWordChecker::CheckFreewordWithRegulation( ProfanityFilterPatternList nPatternCode, wchar_t *pText )
{
  if( pText == NULL ){
    return E_SUCCESS;
  }
  // 16�����I�[�o�[
  if( ::std::wcslen( pText ) > ARROW_FREEWORD_CHAR_COUNT ){
    return E_ERROR_FREEWORD_CHAR_COUNT;
  }
  // �����������I�[�o�[
  if( CountNumbers(pText) > ARROW_NUMBER_COUNT ){
    return E_ERROR_NUMBER_COUNT;
  }
  ::std::wcscpy( m_tempArea, pText );

  // CheckProfanityWords()
  bool isError = false;
  const wchar_t* ppWords[1] = { m_tempArea };
  CheckProfanityWords( &isError, nPatternCode, ppWords, 1 );
  if( isError ){
    return E_ERROR_WORD;
  }

  // MaskProfanityWordsInText()
  int profanityWordCount = 0;
  MaskProfanityWordsInText( &profanityWordCount, nPatternCode, m_tempArea );
  if( profanityWordCount > 0 ){
    return E_ERROR_WORD;
  }

  return E_SUCCESS;
}


//-------------------------------------------------------------------------------
/**
 * @brief �񓯊��ʐM�����s�����ǂ����𒲂ׂ�
 * @retval true   ���s��
 * @retval false  ���s���Ă��Ȃ�
 */
//-------------------------------------------------------------------------------
bool NGWordChecker::CheckAsync( void ) const
{
  return ( m_thread && m_thread->IsAlive() );
}


//-------------------------------------------------------------------------------
/**
 * @brief ���s���̔񓯊��ʐM���L�����Z������
 *
 * @note
 * CheckAsync()�Ŕ񓯊������̏I�����m�F������,
 * EndAsync()���Ă�ł��������B
 */
//-------------------------------------------------------------------------------
void NGWordChecker::CancelAsync( void )
{
  if( this->CheckAsync() ) {
    m_thread->Kill();
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief �X���b�h�̏I���҂����s���܂��B
 */
//-------------------------------------------------------------------------------
void NGWordChecker::WaitAsyncFinished( void )
{
  while( this->CheckAsync() ) {
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief �񓯊������X���b�h��j������
 */
//-------------------------------------------------------------------------------
void NGWordChecker::DeleteAsync( void )
{
  this->WaitAsyncFinished();

  if( m_thread )
  {
    GFL_DELETE m_thread;
    m_thread = NULL;
  }
}


} // ngc
} // gfl


