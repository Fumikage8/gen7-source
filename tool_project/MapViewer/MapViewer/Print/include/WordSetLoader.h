//============================================================================================
/**
 * @file    WordsetLoader.h
 * @brief   wordset�ł̎��O�o�b�t�@�ǂݍ��݊Ǘ�
 * @author  tamada
 * @date  2012.08.23
 *
 * 3DS�ł͓����ǂݍ��݂����ƂȂ邽�߁A���O�ɖ��̈ꗗ��MsgData��ǂݍ���ł����K�v������B
 * WordSet�ɑ΂��ēǂݍ��ݏ����Ɠǂݍ��݃f�[�^��񋟂��邽�߂̃N���X
 *
 * �|�P�������͏풓���Ă��邽�ߎw�肷��K�v�͂Ȃ�
 */
//============================================================================================
#if !defined( WORDSETLOADER_H_INCLUDED )
#define WORDSETLOADER_H_INCLUDED

#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/// �ǂݍ��݊Ǘ�ID�iWordsetLoader�����ł̂ݎg�p�j
//------------------------------------------------------------------
enum {
  PRELOAD_NO_ITEMNAME = 0,
  PRELOAD_NO_WAZANAME,
  PRELOAD_NO_TYPENAME,
  PRELOAD_NO_TOKUSEI,
  PRELOAD_NO_SEIKAKU,
  PRELOAD_NO_PLACENAME,
  PRELOAD_NO_TRNAME,
/* @todo ������
  PRELOAD_NO_TRTYPE,
  PRELOAD_NO_ASSIST_POWER,
  PRELOAD_NO_KAWAIGARI_GOODS,
  PRELOAD_NO_POFURENAME,
  PRELOAD_NO_CONTEST,   //!< �R���e�X�g�֘A�@@note �P��ȊO�̃��b�Z�[�W���܂܂�Ă��邽�߁A�T�C�Y���傫��
  PRELOAD_NO_TREND,     //!< ���s��֘A�@@note �P��ȊO�̃��b�Z�[�W���܂܂�Ă��邽�߁A�T�C�Y���傫��
*/

  PRELOAD_NO_MAX,
};

//------------------------------------------------------------------
/**
 * @brief �ǂݍ��ݎw��p�r�b�g��`
 * @note  src/print/WordSetLoader.cpp��garc_id_table�ƕ��т����낦�邱��
 */
//------------------------------------------------------------------
enum {
  PRELOAD_ITEMNAME        = 1 << PRELOAD_NO_ITEMNAME,
  PRELOAD_WAZANAME        = 1 << PRELOAD_NO_WAZANAME,
  PRELOAD_TYPENAME        = 1 << PRELOAD_NO_TYPENAME,
  PRELOAD_TOKUSEI         = 1 << PRELOAD_NO_TOKUSEI,
  PRELOAD_SEIKAKU         = 1 << PRELOAD_NO_SEIKAKU,
  PRELOAD_PLACENAME       = 1 << PRELOAD_NO_PLACENAME,
  PRELOAD_TRNAME          = 1 << PRELOAD_NO_TRNAME,
/* @todo ������
  PRELOAD_TRTYPE          = 1 << PRELOAD_NO_TRTYPE,
  PRELOAD_ASSIST_POWER    = 1 << PRELOAD_NO_ASSIST_POWER,
  PRELOAD_KAWAIGARI_GOODS = 1 << PRELOAD_NO_KAWAIGARI_GOODS,
  PRELOAD_POFURENAME      = 1 << PRELOAD_NO_POFURENAME,
  PRELOAD_CONTEST         = 1 << PRELOAD_NO_CONTEST,    //!< �R���e�X�g�֘A�@@note �P��ȊO�̃��b�Z�[�W���܂܂�Ă��邽�߁A�T�C�Y���傫��
  PRELOAD_TREND           = 1 << PRELOAD_NO_TREND,      //!< ���s��֘A�@@note �P��ȊO�̃��b�Z�[�W���܂܂�Ă��邽�߁A�T�C�Y���傫��
*/
};

//============================================================================================
/**
 * @class WordSetLoader
 * @brief WordSet�ł̎��O�ǂݍ��݊Ǘ��N���X
 */
//============================================================================================
class WordSetLoader
{

public:
  //------------------------------------------------------------------
  /**
   * @brief constructor
   * @param[in]   pHeap         �ǂݍ��݃f�[�^�ێ��Ɏg�p����q�[�v
   * @param[in]   read_id_bits  �ǂݍ��ރ��b�Z�[�W�f�[�^�̎w��(PRELOAD_�`��or�������́j
   */
  //------------------------------------------------------------------
  WordSetLoader( gfl2::heap::HeapBase *pHeap, u32 read_id_bits );

  //------------------------------------------------------------------
  /**
   * @brief  destructor
   */
  //------------------------------------------------------------------
  ~WordSetLoader();

  //------------------------------------------------------------------
  /**
   * @brief   �ǂݍ��݊J�n
   * @param[in]   pTempHeap   �ǂݍ��݂Ɏg�p����e���|�����q�[�v
   */
  //------------------------------------------------------------------
  void StartLoad( gfl2::heap::HeapBase *pTempHeap = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   �ǂݍ��ݑ҂�����
   * @return  bool  true�̂Ƃ��Z�b�g�A�b�v����
   *
   * @note    �S�f�[�^�ǂݍ��ݎ��ɏ��߂�MsgData�̐������s���Ă���
   */
  //------------------------------------------------------------------
  bool WaitLoad( void );

  //------------------------------------------------------------------
  /**
   * @brief   �ǂݍ��ݍς݃f�[�^�̉��
   */
  //------------------------------------------------------------------
  void Release( void );

  //------------------------------------------------------------------
  /**
   * @brief �w��ID��MsgData���擾����
   * @param[in]   dat_id
   * @return      MsgData�ւ̃|�C���^�A���݂��Ȃ��ꍇ��NULL��Ԃ�
   *
   * @note    WordSet�����Ŏg�p�����B
   */
  //------------------------------------------------------------------
  gfl2::str::MsgData * GetMsgData( u32 dat_id );


  //------------------------------------------------------------------
  /**
   * @brief   �풓�o�b�t�@�ǂݍ��ݏ���
   * @param[in]   pHeap   �ǂݍ��݃f�[�^�ێ��Ɏg�p����q�[�v
   * @param[in]   bits    �ǂݍ��ރ��b�Z�[�W�f�[�^�̎w��(PRELOAD_�`��or�������́j
   *
   * @note  �Q�[���S�̂�ʂ��ď풓������o�b�t�@��Ǎ�
   */
  //------------------------------------------------------------------
  static void LoadGlobal( gfl2::heap::HeapBase * pHeap, u32 bits );

  //------------------------------------------------------------------
  /**
   * @brief   �풓�o�b�t�@�ēǂݍ��ݏ���
   *
   * @note  LoadGlobal�Őݒ�A�ǂݍ��񂾃o�b�t�@������A�ēǂݍ��݂���
   * @note  ����؂�ւ��ւ̑Ή����s��
   */
  //------------------------------------------------------------------
  static void ReloadGlobal( void );

  static gfl2::str::MsgData * GetGlobalMsgData( u32 dat_id );

private:
  enum STATE {
    STATE_INIT,
    STATE_LOADING,
    STATE_LOADED,
  };

  //------------------------------------------------------------------
  /**
   * @brief MSG�o�b�t�@�Ǘ��\����
   */
  //------------------------------------------------------------------
  struct Buffer
  {
    void * mAddress;
    size_t mSize;
    u16     mGarcId;
    gfl2::str::MsgData * mMsgData;
  };

  gfl2::heap::HeapBase *      m_pHeap;
  const u32                   m_ReadBits;
  const u32                   m_ReadCount;
  STATE                       m_State;
  Buffer *                    m_BufferArray;
  bool                        m_bAppended;
  s32                         m_MessageArcId;

  /// �O���[�o��
  static Buffer               s_StaticBuffer[ PRELOAD_NO_MAX ];
  static u32                  s_ReadBits;
  static gfl2::heap::HeapBase * s_BufferHeap;
};

#endif  // WORDSETLOADER_H_INCLUDED
