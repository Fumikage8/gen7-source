//===========================================================================
/**
 * @file  gfl_MsgData.cpp
 * @brief GAMEFREAK Library  String Print System ( MsgData )
 * @author  GAMEFREAK inc.
 * @date  2010.11.08
 */
//===========================================================================


#include <gfl_Common.h>
#include <gfl_Std.h>
#include <gfl_Fs.h>
#include <gfl_Heap.h>

#include <str/gfl_MsgData.h>

namespace gfl {
namespace str {

//----------------------------------------------------------------------
// �f�o�b�O�o�͗L��
//----------------------------------------------------------------------
enum {
  MSGDAT_PRINT_FLAG = (gfl::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl::debug::PRINT_FLAG_LEVEL2)
};

//----------------------------------------------------------------------
// statics
//----------------------------------------------------------------------
u32 MsgData::s_CurrentLangID = 0;


//----------------------------------------------------------------------
/**
 * �R���X�g���N�^���f�X�g���N�^
 */
//----------------------------------------------------------------------

//--------------------------------------------------------------
// �A�[�J�C�u�p�X���f�[�^ID����\�z
//--------------------------------------------------------------
MsgData::MsgData( const wchar_t* arcFilePath, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType, u32 arc_init_flag )
 : m_pHeap(pHeap), m_pArc(NULL), m_loadType(loadType), m_dataID(0), m_targetLangID(0), m_pDataHeader(NULL),  m_pLangHeader(NULL)
{
  m_pArc = GFL_NEW(pHeap) gfl::fs::ArcFile( pHeap, arcFilePath, arc_init_flag );
  construct_common( datID, pHeap, loadType );
}
//--------------------------------------------------------------
// �A�[�J�C�uID���f�[�^ID����\�z
//--------------------------------------------------------------
MsgData::MsgData( gfl::fs::ArcFile::ARCID arcID, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType, u32 arc_init_flag )
 : m_pHeap(pHeap), m_pArc(NULL), m_loadType(loadType), m_dataID(0), m_targetLangID(0), m_pDataHeader(NULL),  m_pLangHeader(NULL)
{
  m_pArc = GFL_NEW(pHeap) gfl::fs::ArcFile( pHeap, arcID, arc_init_flag );
  construct_common( datID, pHeap, loadType );
}
/**
 *  ���R���X�g���N�^���ʉ������^m_pArc���쐬���ꂽ��ɌĂяo������
 */
void MsgData::construct_common( u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType )
{
  GFL_ASSERT_STOP(m_pArc!=NULL);

  m_strParamBody.len = 0;
  m_strParamBody.offset = 0;
  m_strParamBody.userParam = 0;

  m_pDataHeader    = loadDataHeader( m_pArc, datID, pHeap );
  m_dataID         = datID;
  m_targetLangID   = getCurrentLangID( m_pDataHeader );

  // �ʏ�͌���u���b�N�������Ȃ�
  if( m_loadType == LOAD_PART )
  {
    m_pLangHeader = NULL;
  }
  // �������[�h�ł͌���u���b�N��ǂݍ���ł���
  else
  {
    m_pLangHeader = reinterpret_cast<MSG_LANG_BLOCK*>GflHeapAllocMemory( pHeap, m_pDataHeader->maxLangBlockSize );
    loadLangHeader( &m_pLangHeader, m_pDataHeader, m_pArc, m_dataID, m_targetLangID );
  }
}
//--------------------------------------------------------------
// ��������ɓǂݍ��ݍς݂̃f�[�^����\�z
//--------------------------------------------------------------
MsgData::MsgData( const void* dataAdress, gfl::heap::HeapBase* pHeap )
 : m_pHeap(pHeap), m_pArc(NULL), m_loadType(LOAD_FULL)
{
  m_pDataHeader    = reinterpret_cast<const MSG_HEADER_BLOCK*>( dataAdress );
  m_dataID         = 0;
  m_targetLangID   = getCurrentLangID( m_pDataHeader );
  loadLangHeader( &m_pLangHeader, m_pDataHeader, NULL, m_dataID, m_targetLangID );
}

//--------------------------------------------------------------
// �f�X�g���N�^
//--------------------------------------------------------------
MsgData::~MsgData()
{
  // pArc != NULL �Ȃ玩�O�Ń��[�h�����f�[�^�Ȃ̂ŉ��
  if( m_pArc != NULL )
  {
    if( m_pLangHeader != NULL ){
      GflHeapFreeMemory( m_pLangHeader );
    }

    if( m_pDataHeader != NULL ){
      GflHeapFreeMemory( const_cast<MSG_HEADER_BLOCK*>(m_pDataHeader) );
    }
  }

  if( m_pArc != NULL ){
    GFL_DELETE( m_pArc );
  }
}


MsgData::MSG_HEADER_BLOCK*
MsgData::loadDataHeader( gfl::fs::ArcFile* pArc, u32 dataID, gfl::heap::HeapBase* pHeap )
{
  u8   tmp_buffer[ sizeof(MSG_HEADER_BLOCK) ];

  MSG_HEADER_BLOCK *header, *tmp_header;

  pArc->SeekDataPtr( dataID, 0 );
  pArc->LoadDataPtr( sizeof(tmp_buffer), tmp_buffer );

  tmp_header = reinterpret_cast<MsgData::MSG_HEADER_BLOCK*>(tmp_buffer);
  {
    u32 headerSize = sizeof(MSG_HEADER_BLOCK) + (sizeof(u32)*tmp_header->numLangs);
    header = reinterpret_cast<MSG_HEADER_BLOCK*>( GflHeapAllocMemory( pHeap, headerSize ) );
  }

  if( header )
  {
    gfl::std::MemCopy( tmp_header, header, sizeof(MSG_HEADER_BLOCK) );
    pArc->LoadDataPtr( sizeof(u32)*header->numLangs, header->ofsLangBlocks );
  }

  return header;
}

//--------------------------------------------------------------------------
/**
 * @brief ����u���b�N�w�b�_��ǂݍ���
 *
 * @param[out]  ppLangHeader    ����u���b�N�f�[�^�ǂݍ��ݐ�o�b�t�@�ւ̃|�C���^
 * @param[in]   pDataHeader     �ǂݍ��ݍς݃f�[�^�w�b�_�ւ̃|�C���^
 * @param[in]   pArc            ���b�Z�[�W�f�[�^�A�[�J�C�u�n���h���i�O���f�[�^�\�z�̏ꍇ��NULL�j
 * @param[in]   dataID          �Ώۃf�[�^ID
 * @param[in]   langID          �Ώی���ID
 */
//--------------------------------------------------------------------------
void
MsgData::loadLangHeader( MSG_LANG_BLOCK** ppLangHeader, const MSG_HEADER_BLOCK* pDataHeader,
    gfl::fs::ArcFile* pArc, u32 dataID, u32 langID )
{
  u32  langBlockOfs = getTargetLangBlockOfs( pDataHeader, langID );

  // pArc!=NULL �Ȃ玩�O�Ń��[�h�����f�[�^
  if( pArc != NULL )
  {
    GFL_ASSERT( *ppLangHeader );

    MSG_LANG_BLOCK  langBlockHeader;

    pArc->SeekDataPtr( dataID, langBlockOfs );
    pArc->LoadDataPtr( sizeof(MSG_LANG_BLOCK), reinterpret_cast<void*>(*ppLangHeader) );

    pArc->SeekDataPtr( dataID, langBlockOfs );
    pArc->LoadDataPtr( (*ppLangHeader)->size, reinterpret_cast<void*>(*ppLangHeader) );
  }
  // pArc==NULL �Ȃ�O���f�[�^�\�z�Ƃ݂Ȃ�
  else
  {
    *(ppLangHeader) = reinterpret_cast<MSG_LANG_BLOCK*>(
                        reinterpret_cast<uptr>(pDataHeader) + langBlockOfs
                      );
  }
}
//------------------------------------------------------------------------------
/**
 * @brief   �ǂݍ��ނׂ�����ID���擾����
 *
 * @param[in]     dataHeader    ���b�Z�[�W�f�[�^�w�b�_
 *
 * @retval  u32   ����ID
 */
//------------------------------------------------------------------------------
u32
MsgData::getCurrentLangID( const MSG_HEADER_BLOCK* dataHeader ) const
{
  if( s_CurrentLangID >= dataHeader->numLangs )
  {
    return 0;
  }
  return s_CurrentLangID;
}
//------------------------------------------------------------------------------
/**
 * @brief  ���[�h�Ώی���u���b�N�́A�f�[�^�擪����̃I�t�Z�b�g���擾����
 *
 * @param[in]   dataHeader  ���b�Z�[�W�f�[�^�w�b�_
 * @param[in]   langID      ���[�h�Ώی���ID
 *
 * @retval  u32   ����u���b�N�̃f�[�^�擪����̃I�t�Z�b�g�ibyte�j
 */
//------------------------------------------------------------------------------
u32
MsgData::getTargetLangBlockOfs( const MSG_HEADER_BLOCK* dataHeader, u32 langID ) const
{
  return dataHeader->ofsLangBlocks[ langID ];
}

//----------------------------------------------------------------------
/**
 * @brief   �w�蕶������o�b�t�@�ɓǂݍ���
 *
 * @param[in]    strID   ������ID
 * @param[out]   dst     �ǂݍ��ݐ敶����o�b�t�@
 *
 * @return  u16      ���[�U�[�ݒ�p�����[�^
 */
//----------------------------------------------------------------------
u16
MsgData::GetString( u32 strID, StrBuf& dst )
{
  //GF_ASSERT_MSG(strID < handle->dataHeader->numStrings, "strID=%d, numStr=%d", strID, handle->dataHeader->numStrings);
  if( strID >= m_pDataHeader->numStrings )
  {
    GFL_ASSERT_MSG(0, "strID=%d, numStr=%d dataID=%d", strID, m_pDataHeader->numStrings, m_dataID);
    dst.Clear();
    return 0;
  }

  checkTargetLangChanged();

//  GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] ReadStart, StrID=%d, LangID=%d, bufSize=%d\n", strID, m_targetLangID, dst.GetSize() );

  {
    const MSG_STRPARAM_BLOCK* param = getStrParamPointer( strID );

//    GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] StrParam. ofs=%08x, len=%d\n", param->offset, param->len );

    // �������[�h
    if( m_loadType == LOAD_PART )
    {
      wchar_t*  buf;
      u32  size;

      size = param->len * sizeof(wchar_t);
      buf = reinterpret_cast<wchar_t*>GflHeapAllocMemoryAlign( m_pHeap, size, -(m_pHeap->GetDefaultAlign()) );
      if( buf )
      {
        u32  langBlockOfs = getTargetLangBlockOfs( m_pDataHeader, m_targetLangID ) + param->offset;
//        GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] AllocTmpBuf, size=%d\n", size );
        m_pArc->SeekDataPtr( m_dataID, langBlockOfs );
        m_pArc->LoadDataPtr( size, buf );

        putToStrBuf( buf, param->len, strID, dst );

        GflHeapFreeMemory( buf );
      }
    }
    // �������[�h
    else
    {
      wchar_t* str = reinterpret_cast<wchar_t*> (
                       reinterpret_cast<uptr>(m_pLangHeader) + param->offset
                     );
      putToStrBuf( str, param->len, strID, dst );
    }

    return param->userParam;
  }
}


//----------------------------------------------------------------------
/**
 * @brief   �ꎞ�I�Ɍ����؂�ւ��Ďw�蕶������o�b�t�@�ɓǂݍ���
 *
 * @param[in]    strID   ������ID
 * @param[in]    langID  ����
 * @param[out]   dst     �ǂݍ��ݐ敶����o�b�t�@
 *
 * @return  u16      ���[�U�[�ݒ�p�����[�^
 */
//----------------------------------------------------------------------
u16
MsgData::GetStringByLanguage( u32 strID, u32 langID, StrBuf& dst )
{
  u32 default_langID = GetLangID();

  //�^�[�Q�b�g��ID���ꎞ�I�ɕύX���Ă���邱�ƂŁA
  //GetString������checkTargetLangChanged�ɂăw�b�_���ēǂݍ��݂���
  SetLangID( langID );

  u16 userParam = GetString( strID, dst );

  SetLangID( default_langID );

  return userParam;
}

//----------------------------------------------------------------------
/**
 * @brief   ���[�U�[�ݒ�p�����[�^���擾
 *
 * @param[in]    strID   ������ID
 *
 * @return  u16      ���[�U�[�ݒ�p�����[�^
 */
//----------------------------------------------------------------------
u16
MsgData::GetUserParam( u32 strID )
{
  if( strID >= m_pDataHeader->numStrings )
  {
    GFL_ASSERT_MSG(0, "strID=%d, numStr=%d", strID, m_pDataHeader->numStrings);
    return 0;
  }

  const MSG_STRPARAM_BLOCK* param = getStrParamPointer( strID );
  return param->userParam;
}



//------------------------------------------------------------------------------
/**
 * ������f�[�^���AStrBuf�N���X�ɃZ�b�g����i�Í��ς݃f�[�^�Ή��j
 *
 * @param[in]   src      �Z�b�g���镶����
 * @param[in]   strLen   �����񒷁i�I�[�R�[�h���܂ށj
 * @param[in]   strID    src �̕�����ID�i�����L�[�Ƃ��Ďg�p�j
 * @param[out]  dst      �Z�b�g�敶����N���X
 */
//------------------------------------------------------------------------------
void
MsgData::putToStrBuf( const wchar_t* src, u32 strLen, u32 strID, StrBuf& dst ) const
{
  u32 bufLen = dst.GetSize();
  if ( bufLen < strLen )
  {
    GFL_ASSERT_MSG( 0, "buffer size over ! strID=%d strLen=%d bufLen=%d \n", strID, strLen, bufLen ); //@check
  }

  dst.Clear();

  if( m_pDataHeader->reserved == DATA_CODED )
  {
    u16 mask = static_cast<u16>(strID);
    mask = (0x2983 * (mask+3)) & 0xffff;

    for(u32 i=0; i<strLen; ++i)
    {
      wchar_t  code = (*src);
      (code) ^= mask;

      if( !dst.AddCode( code ) )
      { // ������ǉ��ł��Ȃ������ꍇ�A���r���[�ȃ^�O�\����������邽�ߌ����N���A
        dst.Clear();
        return;
      }
      ++src;
      mask = (((mask&0xe000)>>13) | ((mask&0x1fff)<<3));
    }
  }
  else
  {
    for(u32 i=0; i<strLen; ++i)
    {
      wchar_t  code = (*src);
      if( !dst.AddCode( code ) )
      { //������ǉ��ł��Ȃ������ꍇ�A���r���[�ȃ^�O�\����������邽�ߌ����N���A
        dst.Clear();
        return;
      }
      ++src;
    }
  }

  dst.AddCodeFinish();
}
//--------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�f�[�^�Ɋ܂܂�镶���񐔂��擾
 *
 * @retval  u32     ������
 */
//--------------------------------------------------------------------------------------
u32
MsgData::GetStrCount( void ) const
{
  return m_pDataHeader->numStrings;
}



//--------------------------------------------------------------------------
/**
 * �ǂݍ��ݑΏۂ̌���ID���ύX����Ă��邩�ǂ����`�F�b�N
 * �ύX����Ă���΁A�K�v�ɉ����ē����f�[�^���X�V����B
 */
//--------------------------------------------------------------------------
void
MsgData::checkTargetLangChanged( void )
{
  u8 currentLangID = getCurrentLangID( m_pDataHeader );

  if( m_targetLangID != currentLangID )
  {
//    GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] targetLangChanged %d->%d\n", m_targetLangID, currentLangID );

    m_targetLangID = currentLangID;

    if( m_loadType != LOAD_PART )
    {
      loadLangHeader( &m_pLangHeader, m_pDataHeader, m_pArc, m_dataID, m_targetLangID );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * �w�蕶����̃p�����[�^�����|�C���^��Ԃ�
 *
 * @param   strID     ������ID
 *
 * @retval  const MSG_STRPARAM_BLOCK*   �p�����[�^�����ւ̃|�C���^
 */
//--------------------------------------------------------------------------
const MsgData::MSG_STRPARAM_BLOCK*
MsgData::getStrParamPointer( u32 strID )
{
  // �������[�h
  if( m_loadType == LOAD_PART )
  {
    // �t�@�C������n���h���������ϐ��̈�ɓǂݍ��݁A���̃A�h���X��Ԃ��Ă���
    u32 offset = getTargetLangBlockOfs( m_pDataHeader, m_targetLangID )
                + sizeof(MSG_LANG_BLOCK) + sizeof(MSG_STRPARAM_BLOCK) * strID;

    m_pArc->SeekDataPtr( m_dataID, offset );
    m_pArc->LoadDataPtr( sizeof(MSG_STRPARAM_BLOCK), &m_strParamBody );
    return &m_strParamBody;
  }
  // �������[�h
  else
  {
    // ���ɓǂݍ��ݍς݂̃p�����[�^�A�h���X��Ԃ��Ă���
    return &(m_pLangHeader->params[ strID ]);
  }
}


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
 * ���[�h�ΏۂƂȂ錾��ID��ݒ肷��B
 * �ȍ~�AMsgData �N���X����Ď擾�ł��镶���񂪐؂�ւ��B
 *
 * @param[in]   langID    ����ID
 *
 */
//--------------------------------------------------------------------------------------
void
MsgData::SetLangID( u32 langID )
{
  s_CurrentLangID = langID;
}

//--------------------------------------------------------------------------------------
/**
 * �ݒ肳�ꂽ���[�h�Ώۂ̌���ID���擾�B
 *
 * @retval  u8    �ݒ肳��Ă��錾��ID
 */
//--------------------------------------------------------------------------------------
u32
MsgData::GetLangID( void )
{
  return s_CurrentLangID;
}

} /* end of namespace str */
} /* end of namespace gfl */



