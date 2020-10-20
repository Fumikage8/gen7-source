//================================================================================
/**
 * @file   gfl_NGWordChecker.h
 * @brief  NG���[�h�`�F�b�N�p�N���X
 * @author hisanao suzuki
 * @date   2012.2.15
 */
//================================================================================
#ifndef __GFL_NG_WORD_CHECKER_H__
#define __GFL_NG_WORD_CHECKER_H__

#pragma once

#include <nn.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>
#include <ro/gfl_RoManager.h>

#include <ngc/gfl_NGWordCheckListener.h>

namespace gfl {

/**
* @namespace gfl::ngc
* @brief NG���[�h�`�F�b�N�p�l�[���X�y�[�X
*/
namespace ngc {

  class NGWordCheckThread;

  //-------------------------------------------------------------------------------
  /**
  * @brief NG���[�h�`�F�b�N�p�N���X
  */
  //-------------------------------------------------------------------------------
  class NGWordChecker : public gfl::base::Singleton<NGWordChecker>
  {
    GFL_FORBID_COPY_AND_ASSIGN( NGWordChecker );  // �R�s�[���֎~

  public:
    typedef nn::ngc::CTR::ProfanityFilterPatternList ProfanityFilterPatternList;


    /**
     * @brief �S����`�F�b�N�w��p�萔
     */
    static const ProfanityFilterPatternList ALL_REGION = (ProfanityFilterPatternList)(
                                                          (1<<nn::ngc::CTR::PATTERNLIST_JAPAN_JAPANESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_ENGLISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_FRENCH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_SPANISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_ENGLISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_FRENCH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_GERMAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_ITALIAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_SPANISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_DUTCH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_KOREA_KOREAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_CHINA_SIMP_CHINESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_PORTUGUESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_RUSSIAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_PORTUGUESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_TAIWAN_TRAD_CHINESE));

  public:
    static const u32  ARROW_NUMBER_COUNT        = 5;  //!< ��ʒ��Ɋ܂�ł��ǂ�����������
    static const u32  ARROW_FREEWORD_CHAR_COUNT = 16; //!< �t���[���[�h������

  public:
    //-------------------------------------------------------------------------------
    /**
    * @brief  �R���X�g���N�^
    */
    //-------------------------------------------------------------------------------
    NGWordChecker( void );

    //-------------------------------------------------------------------------------
    /**
    * @brief  �f�X�g���N�^
    */
    //-------------------------------------------------------------------------------
    virtual ~NGWordChecker();

  public:
    //-------------------------------------------------------------------------------
    /**
    * @brief  ����������
    *
    * @param[in]  heap ���[�N�̈�Ƃ��Ďg�p�����q�[�v�G���A�B���悻64KB��������B
    *
    * @retval true  ����������
    * @retval false ���s
    */
    //-------------------------------------------------------------------------------
    bool Initialize( gfl::heap::HeapBase *heap );

    //-------------------------------------------------------------------------------
    /**
    * @brief  �I������
    *
    * @retval true  ����I��
    * @retval false �ُ�I��
    */
    //-------------------------------------------------------------------------------
    bool Finalize( void );

    //---------------------------------------------------------------------
    /**
    * @brief  �C�V���̃��M�����[�V�����ƏƂ炵���킹�A�t���[���[�h�����Ȃ����`�F�b�N����(�����֐��j
    *
    * @par
    *         - CheckProfanityWords()��NG�ƂȂ�Ȃ�
    *         - MaskProfanityWordsInText()��NG�ƂȂ�Ȃ�
    *         - 16�����ȓ�
    *         - ������5�����ȓ�
    *
    * @param[in]  nPatternCode   �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
    * @param[in]  pText          �����ΏۂƂȂ�NULL�I�[������
    *
    * @return ��������
    */
    // ---------------------------------------------------------------------
    Result CheckFreewordWithRegulation( ProfanityFilterPatternList nPatternCode, wchar_t *pText );

    //-------------------------------------------------------------------------------
    /**
    * @brief  �C�V���̃��M�����[�V�����ƏƂ炵���킹�A�t���[���[�h�����Ȃ����`�F�b�N����(�񓯊��֐�)
    *
    * @par
    *         - CheckProfanityWords()��NG�ƂȂ�Ȃ�
    *         - MaskProfanityWordsInText()��NG�ƂȂ�Ȃ�
    *         - 16�����ȓ�
    *         - ������5�����ȓ�
    *
    * @attention �I���C�x���g�ʒm��A�K��DeleteAsync()���R�[�����Ă��������B
    *
    * @param[in]  heap            �X���b�h�Ɏg�p����q�[�v
    * @param[in]  nPatternCode    �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
    * @param[in]  pText           �����ΏۂƂȂ�NULL�I�[������
    * @param[in]  listener        ����������ʒm���邽�߂̃��X�i
    * @param[in]  thread_priority �X���b�h�̎��s�D��x
    *
    * @retval true   �������J�n�ł���
    * @retval false  �����Ɏ��s����
    */
    //-------------------------------------------------------------------------------
    bool CheckFreewordWithRegulationASync( gfl::heap::HeapBase* heap,
                                           ProfanityFilterPatternList nPatternCode, wchar_t *pText,
                                           NGWordCheckListener* listener,
                                           gfl::base::Thread::Priority thread_priority );

    //-------------------------------------------------------------------------------
    /**
    * @brief  MaskProfanityWordsInText()���s���A���̂�����𔭌��������̋�����ݒ�
    *
    * @param  bOverWrite[in] �����̕������������ꂽ�ۂ̓��샂�[�h�B
    *         @arg true   �����̕������ׂĂ��A�X�^���X�N�L���ŏ㏑���B
    *         @arg false  �����𕶎����ɂ�炸1�����̃A�X�^���X�N�L���ɒu���B
    */
    //-------------------------------------------------------------------------------
    void SetMaskMode( bool bOverWrite );

    //-------------------------------------------------------------------------------
    /**
    * @brief  ������Ɋ܂܂��A����\�������̌����擾����
    *
    * @param[in]  pString ���ׂ�Ώۂ�NULL�I�[������
    *
    * @return ����\�������̌�
    */
    //-------------------------------------------------------------------------------
    static int CountNumbers( const wchar_t * pString );

  private:
    //-------------------------------------------------------------------------------
    /**
    * @brief  ���͂��`�F�b�N���A���̂�����𔭌��������̋�����ݒ�
    *
    * @param[out]  pProfanityWordCount ���͒��ɕs�������񂪉��񌻂ꂽ�����擾���邽�߂̃|�C���^
    * @param[in]   nPatternCode        �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
    * @param[out]  pText               �����ΏۂƂȂ�NULL�I�[������BNG�ӏ���*�ɒu����������
    *
    * @return ���s����
    */
    //-------------------------------------------------------------------------------
    nn::Result MaskProfanityWordsInText( int* pProfanityWordCount, ProfanityFilterPatternList nPatternCode, wchar_t *pText );

    //-------------------------------------------------------------------------------
    /**
    * @brief  �P����`�F�b�N���A���̂�����𔭌��������̋�����ݒ�
    *
    * @param[out] checkResult    NG���[�h�����݂���Ȃ�true���i�[
    * @param[in]  nPatternCode   �`�F�b�N���s�����[�W�������r�b�g�Ŏw�肷��
    * @param[in]  ppWords        �����ΏۂƂȂ�NULL�I�[������̔z��i�����R�[�h�FUTF16���g���G���f�B�A���j
    * @param[in]  nWordCount     �����Ώە�����̔z��(�ő�16�܂Łj
    *
    * @return ���s����
    */
    //-------------------------------------------------------------------------------
    nn::Result CheckProfanityWords( bool* checkResult, ProfanityFilterPatternList nPatternCode, const wchar_t ** ppWords, size_t nWordCount );

  public:
    //-------------------------------------------------------------------------------
    /**
     * @brief �񓯊��ʐM�����s�����ǂ����𒲂ׂ�
     * @retval true   ���s��
     * @retval false  ���s���Ă��Ȃ�
     */
    //-------------------------------------------------------------------------------
    bool CheckAsync( void ) const;

    //-------------------------------------------------------------------------------
    /**
     * @brief ���s���̔񓯊��ʐM���L�����Z������
     *
     * @note CheckAsync()�Ŕ񓯊������̏I�����m�F������ADeleteAsync()���Ă�ł��������B
     */
    //-------------------------------------------------------------------------------
    void CancelAsync( void );

    //-------------------------------------------------------------------------------
    /**
     * @brief �X���b�h�̏I���҂����s���܂��B
     */
    //-------------------------------------------------------------------------------
    void WaitAsyncFinished( void );

    //-------------------------------------------------------------------------------
    /**
     * @brief �񓯊������X���b�h��j������
     */
    //-------------------------------------------------------------------------------
    void DeleteAsync( void );

  private:
    static const u32  WORKMEMORY_SIZE   = nn::ngc::CTR::ProfanityFilter::WORKMEMORY_SIZE; //!< �����郏�[�N�������T�C�Y
    static const u32  NGWORD_STACK_SIZE = 8192;                                           //!< �X���b�h�Ŏg�p�����X�^�b�N�T�C�Y

  private:
    nn::ngc::CTR::ProfanityFilter m_filter;       //!< �t�B���^�[�N���X
    bool                          m_isInit;       //!< �������t���O
    void*                         m_workMemory;   //!< ���[�N������
    bool                          m_isOverWrite;  //!< NG���[�h�ϊ����[�h
    wchar_t                       m_tempArea[32]; //!< ���[�N�̈�
    NGWordCheckThread*            m_thread;       //!< �񓯊������ʒm�p�X���b�h
  };

} // ngc
} // gfl



/** ---------------------------------------------------------------------
* @brief  SINGLETON�̑O���Q�Ɛ錾
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( gfl::ngc::NGWordChecker );

#endif  // __GFL_NG_WORD_CHECKER_H__
