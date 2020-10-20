#ifndef __GFL_MSGDATA_H__
#define __GFL_MSGDATA_H__
//===========================================================================
/**
 * @file  gfl_MsgData.h
 * @brief GAMEFREAK Library  String Print System ( ���b�Z�[�W�f�[�^�o�C�i����舵���N���X )
 * @author  GAMEFREAK inc.
 * @date    2010.10.27
 */
//===========================================================================
#pragma once

#include <gfl_Common.h>
#include <gfl_Heap.h>
#include <gfl_Fs.h>
#include <str/gfl_StrBuf.h>

namespace gfl {
namespace str {



class MsgData {
  // �R�s�[�R���X�g���N�^��������Z�q�̋֎~
  GFL_FORBID_COPY_AND_ASSIGN( MsgData );

public:
  //------------------------------------------------------
  /**
   *  �f�[�^���[�h�`���w��
   */
  //------------------------------------------------------
  typedef enum {
    LOAD_PART,    ///< �v�����ꂽ���������𒀎��������ɓǂݍ���
    LOAD_FULL     ///< �ŏ��Ɉꊇ���đS�ă�������ɓǂݍ���
  }LoadType;

  //----------------------------------------------------------------------
  /**
   * �R���X�g���N�^���f�X�g���N�^
   */
  //----------------------------------------------------------------------
  MsgData( const wchar_t* arcFilePath, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType = LOAD_PART, u32 arc_init_flag = gfl::fs::ArcFile::OPEN_FASTMODE );
  MsgData( gfl::fs::ArcFile::ARCID arcID, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType = LOAD_PART, u32 arc_init_flag = gfl::fs::ArcFile::OPEN_FASTMODE );
  MsgData( const void* dataAdress, gfl::heap::HeapBase* pHeap );
  virtual ~MsgData();

  //----------------------------------------------------------------------
  /**
   * @brief   �w�蕶������o�b�t�@�ɓǂݍ���
   *
   * @param[in]    strID   ������ID
   * @param[out]   dst     �ǂݍ��ݐ敶����o�b�t�@
   *
   * @retval  u16    ���[�U�[�ݒ�p�����[�^
   */
  //----------------------------------------------------------------------
  u16 GetString( u32 strID, StrBuf& dst );

  //----------------------------------------------------------------------
  /**
   * @brief   �ꎞ�I�Ɍ����؂�ւ��Ďw�蕶������o�b�t�@�ɓǂݍ���
   *
   * @param[in]    strID   ������ID
   * @param[in]    langID  ����
   * @param[out]   dst     �ǂݍ��ݐ敶����o�b�t�@
   *
   * @retval  u16    ���[�U�[�ݒ�p�����[�^
   */
  //----------------------------------------------------------------------
  u16 GetStringByLanguage( u32 strID, u32 langID, StrBuf& dst );

  //----------------------------------------------------------------------
  /**
   * @brief   �f�[�^�Ɋ܂܂�镶���񐔂��擾
   *
   * @retval  u32    ������
   */
  //----------------------------------------------------------------------
  u32 GetStrCount( void ) const;

  //----------------------------------------------------------------------
  /**
   * @brief   ���[�U�[�ݒ�p�����[�^���擾
   *
   * @param[in]    strID   ������ID
   *
   * @return  u16      ���[�U�[�ݒ�p�����[�^
   */
  //----------------------------------------------------------------------
  u16 GetUserParam( u32 strID );


private:

  /**
   *  �Í���
   */
  enum {
    DATA_CODED = 0,
    DATA_NO_CODED = 1,
  };

  /**
   *  �R���X�g���N�^����������
   */
  void construct_common( u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType );

  /*------------------------------------------------------------*/
  /** ���b�Z�[�W�f�[�^�w�b�_                                    */
  /*------------------------------------------------------------*/
  typedef struct {
    u16   numLangs;         // �i�[�����ސ��i���ȁ������Ȃ�Q�j
    u16   numStrings;       // �P����ӂ�̕�����
    u32   maxLangBlockSize; // �ł��傫������u���b�N�̃o�C�g�T�C�Y
    u32   reserved;         // �\��̈�i�Í����Ƃ��Ɏg�������j
    u32   ofsLangBlocks[0]; // ���ꂲ�Ƃ̃u���b�N�I�t�Z�b�g�l�i�t�@�C���擪����C�o�C�g�P�ʁj
  }MSG_HEADER_BLOCK;

  /*------------------------------------------------------------*/
  /** ������p�����[�^                                          */
  /*------------------------------------------------------------*/
  typedef struct {
    u32   offset;     ///< ����u���b�N�擪����̃o�C�g�I�t�Z�b�g
    u16   len;        ///< �������iEOM���܂ށj
    u16   userParam;  ///< ���[�U�[�w��p�����[�^
  }MSG_STRPARAM_BLOCK;

  /*------------------------------------------------------------*/
  /** ����u���b�N�w�b�_                                        */
  /*------------------------------------------------------------*/
  typedef struct {
    u32                     size;       // ���u���b�N�̃o�C�g�T�C�Y
    MSG_STRPARAM_BLOCK      params[0];  // ������p�����[�^�z��
  }MSG_LANG_BLOCK;

  static  u32           s_CurrentLangID;


  gfl::heap::HeapBase*        m_pHeap;
  gfl::fs::ArcFile*           m_pArc;
  LoadType                    m_loadType;
  u32                         m_dataID;
  u32                         m_targetLangID;
  const MSG_HEADER_BLOCK*     m_pDataHeader;
  MSG_LANG_BLOCK*             m_pLangHeader;
  MSG_STRPARAM_BLOCK          m_strParamBody;


  /**
   *
   */
  MSG_HEADER_BLOCK*
    loadDataHeader( gfl::fs::ArcFile* pArc, u32 dataID, gfl::heap::HeapBase* pHeap );



  //--------------------------------------------------------------------------
  /**
   *
   */
  //--------------------------------------------------------------------------
  void
    loadLangHeader( MSG_LANG_BLOCK** ppLangHeader, const MSG_HEADER_BLOCK* pDataHeader,
        gfl::fs::ArcFile* pArc, u32 dataID, u32 langID );

  //--------------------------------------------------------------------------
  /**
   *  �ǂݍ��ނׂ�����ID���擾����
   */
  //--------------------------------------------------------------------------
  u32 getCurrentLangID( const MSG_HEADER_BLOCK* dataHeader ) const;


  //--------------------------------------------------------------------------
  /**
   *  �w�肳�ꂽ����ID�̃f�[�^���I�t�Z�b�g�ibyte�j���擾����
   */
  //--------------------------------------------------------------------------
  u32 getTargetLangBlockOfs( const MSG_HEADER_BLOCK* dataHeader, u32 langID ) const;

  //--------------------------------------------------------------------------
  /**
   * �ǂݍ��ݑΏۂ̌���ID���ύX����Ă��邩�ǂ����`�F�b�N
   * �ύX����Ă���΁A�K�v�ɉ����ē����f�[�^���X�V����B
   */
  //--------------------------------------------------------------------------
  void checkTargetLangChanged( void );

  //--------------------------------------------------------------------------
  /**
   * �w�蕶����̃p�����[�^�����|�C���^��Ԃ�
   */
  //--------------------------------------------------------------------------
  const MsgData::MSG_STRPARAM_BLOCK* getStrParamPointer( u32 strID );


  //--------------------------------------------------------------------------
  /**
   * ������f�[�^���AStrBuf�N���X�ɃZ�b�g����i�Í��ς݃f�[�^�Ή��j
   */
  //--------------------------------------------------------------------------
  void putToStrBuf( const wchar_t* src, u32 strLen, u32 strID, StrBuf& dst ) const;

public:
  /*============================================================================================*/
  /*                                                                                            */
  /*  ���[�h���錾��̐؂�ւ�����                                                              */
  /*                                                                                            */
  /*  ����̕�����ID���g���āA���ۂɓǂݏo����镶�����؂�ւ��邽�߂̎d�g�݁B                */
  /*  ���{��ł́w�Ђ炪�ȁ|�����x�؂�ւ��Ƃ��A                                                */
  /*  ���B�ł́w�p�E���E�ƁE�ɁE���x�T������؂�ւ��Ƃ��Ɏg���Ă��炤���Ƃ�z��B              */
  /*                                                                                            */
  /*============================================================================================*/

  //--------------------------------------------------------------------------------------
  /**
   * @brief  ���[�h�ΏۂƂȂ錾��ID��ݒ肷��B�ȍ~�AMsgData �N���X����Ď擾�ł��镶���񂪐؂�ւ��B
   */
  //--------------------------------------------------------------------------------------
  static void SetLangID( u32 langID );

  //--------------------------------------------------------------------------------------
  /**
   * @brief  ���[�h�ΏۂƂȂ��Ă��錾��ID���擾����B
   */
  //--------------------------------------------------------------------------------------
  static u32 GetLangID( void );

};

} /* end of namespace str */
} /* end of namespace gfl */



#endif // __GFL_MSGDATA_H__
