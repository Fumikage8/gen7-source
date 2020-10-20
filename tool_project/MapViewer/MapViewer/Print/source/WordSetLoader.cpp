//============================================================================================
/**
 * @file  wordsetloader.cpp
 * @brief wordset�ł̎��O�o�b�t�@�ǂݍ��݊Ǘ�
 * @author  tamada
 * @date  2012.08.23
 */
//============================================================================================
///#include "System/include/PokemonVersion.h"
#include "../include/PrintSystem.h"
#include "../include/WordSetLoader.h"

#include "arc_index/message.gaix"


//------------------------------------------------------------------
/**
 * @brief �ǂݍ��ݑΏۂ�GARC�w��
 * @note  include/print/WordsetLoader.h��enum�ƕ��т����낦�邱��
 */
//------------------------------------------------------------------
static const u16 garc_id_table[ PRELOAD_NO_MAX ] =
{
  GARC_message_itemname_wordset_DAT,
  GARC_message_wazaname_DAT,
  GARC_message_typename_DAT,
  GARC_message_tokusei_DAT,
  GARC_message_seikaku_DAT,
  GARC_message_place_name_DAT,
  GARC_message_trname_DAT,
/* @todo ������
  GARC_message_trtype_DAT,
  GARC_message_assist_power_DAT,
  GARC_message_kawaigari_goods_DAT,
  GARC_message_pofure_DAT,
  GARC_message_contest_DAT,
  GARC_message_pmn_trend_DAT,
*/
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u32 countBits( u32 bits )
{
u32 count = 0;
while ( bits != 0 )
{
  if (bits & 1) count ++;
  bits >>= 1;
}
return count;
}

//------------------------------------------------------------------
/**
 * @brief �R���X�g���N�^
 * @param[in]   pHeap         �g�p����q�[�v�̎w��
 * @param[in]   p_readman     �g�p����FileReadManager�ւ̃|�C���^
 * @param[in]   read_id_bits  ���O�ǂݍ��݂��閼�O�̎w��
 */
//------------------------------------------------------------------
WordSetLoader::WordSetLoader( gfl2::heap::HeapBase *pHeap, u32 read_id_bits )
  : m_pHeap( pHeap )
  , m_ReadBits(read_id_bits)
  , m_ReadCount(countBits(read_id_bits))
  , m_State(STATE_INIT)
  , m_BufferArray( NULL )
  , m_bAppended(false)
  , m_MessageArcId(-1)
{
}

//------------------------------------------------------------------
/**
 * @brief �f�X�g���N�^
 */
//------------------------------------------------------------------
WordSetLoader::~WordSetLoader()
{
  Release();
}

//------------------------------------------------------------------
/**
 * @brief   �ǂݍ��݊J�n
 */
//------------------------------------------------------------------
void WordSetLoader::StartLoad( gfl2::heap::HeapBase * pTempHeap )
{
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  m_BufferArray = GFL_NEW_ARRAY(m_pHeap) Buffer[ m_ReadCount ];
  u32 count = 0;
  u32 temp_bits = m_ReadBits;

  m_MessageArcId = print::GetMessageArcId();

  for ( u32 bit_idx = 0; temp_bits != 0; ++ bit_idx, temp_bits >>= 1 )
  {
    GFL_ASSERT( bit_idx < GFL_NELEMS(garc_id_table) );
    if ( temp_bits & 1 )
    {
      m_BufferArray[count].mGarcId = garc_id_table[bit_idx];

      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.fileName = NULL;           //!< [in]  �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B
      req.arcId = m_MessageArcId;              //!< [in]  ARCID�BARCID_NULL�̂Ƃ���fileName���g����B
      req.datId = m_BufferArray[count].mGarcId;              //!< [in]  �A�[�J�C�u�t�@�C�����f�[�^ID
      req.prio = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;               //!< [in]  ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
      req.ppBuf = &m_BufferArray[count].mAddress;              //!< [out] heapForBuf����m�ۂ������������n�����|�C���^�B�Ăяo������GflHeapFreeMemory(*ppBuf)�ŉ�����ĉ������B  // ****�L�[****
      req.pBufSize = &m_BufferArray[count].mSize;           //!< [out] heapForBuf����m�ۂ����������̃T�C�Y�Bheap�̊Ǘ��̈�̃T�C�Y�͊܂܂�Ă��Ȃ��BNULL�ł��悢�B
      req.pRealReadSize = NULL;      //!< [out] ���ۂɓǂݍ��񂾃T�C�Y�Bcompressed��true�̂Ƃ��𓀌�̃T�C�Y�BNULL�ł��悢�B
      req.heapForBuf = m_pHeap;         //!< [in]  �t�@�C������ǂݍ��񂾓��e�����Ă����o�b�t�@���m�ۂ��邽�߂̃q�[�v�B
      req.align = 4;              //!< [in]  heapForBuf����m�ۂ���ۂ̃A���C�������g�B2�ׂ̂���(1, 2, 4, 8, 16, 32, 64, 128, ...)�ɂ��ĉ������B
      req.heapForReq = pTempHeap;         //!< [in]  ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�w�u�����Łv���uArcSrc���g��Ȃ��Ƃ��v�x��NULL�ł悢�BArcSrc���g���Ƃ��̓t�@�C���I�[�v���ɂ��g�p����B
      req.heapForCompressed = NULL;  //!< [in]  �𓀂���ꍇ�Ɉꎞ�I�Ɏg�p����q�[�v�B�𓀂��Ȃ��ꍇ��NULL�ł悢�B

      man->AddArcFileLoadDataReq( req );

      ++ count;
    }
  }
  GFL_ASSERT( count == m_ReadCount );

  m_State = STATE_LOADING;
}

//------------------------------------------------------------------
/**
 * @brief   �ǂݍ��ݑ҂�����
 * @return  bool  true�̂Ƃ��Z�b�g�A�b�v����
 *
 * @note    �S�f�[�^�ǂݍ��ݎ��ɏ��߂�MsgData�̐������s���Ă���
 */
//------------------------------------------------------------------
bool WordSetLoader::WaitLoad( void )
{
  if( m_State == STATE_LOADED )
  {
    return true;
  }

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  bool ret = true;
  for (u32 i = 0; i< m_ReadCount; ++i )
  {
    if (man->IsArcFileLoadDataFinished( &m_BufferArray[i].mAddress ) == false )
    {
      ret = false;
      break;
    }
  }
  if ( ret )
  {
    for (u32 i = 0; i < m_ReadCount; ++i )
    {
      m_BufferArray[i].mMsgData = GFL_NEW(m_pHeap) gfl2::str::MsgData(
          m_BufferArray[i].mAddress, m_pHeap );
    }
    m_State = STATE_LOADED;
  }
  return ret;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void WordSetLoader::Release( void )
{
  switch ( m_State )
  {
  case STATE_INIT:
    return;

  case STATE_LOADING:
    GFL_ASSERT_STOP( 0 );

  case STATE_LOADED:
    if ( m_bAppended )
    {
      m_MessageArcId = -1;
      m_bAppended = false;
    }
    for (u32 i = 0; i < m_ReadCount; ++i )
    {
      GflHeapFreeMemory( m_BufferArray[i].mAddress );
      GFL_DELETE( m_BufferArray[i].mMsgData );
    }
    if ( m_BufferArray )
    {
      GFL_DELETE_ARRAY( m_BufferArray );
      m_BufferArray = NULL;
    }
  }
  m_State = STATE_INIT;
}

//------------------------------------------------------------------
/**
 * @brief �w��ID��MsgData���擾����
 * @param[in]   dat_id
 * @return      MsgData�ւ̃|�C���^�A���݂��Ȃ��ꍇ��NULL��Ԃ�
 */
//------------------------------------------------------------------
gfl2::str::MsgData * WordSetLoader::GetMsgData( u32 dat_id )
{
  for (u32 i = 0; i < m_ReadCount; ++ i )
  {
    if ( m_BufferArray[i].mGarcId == dat_id )
    {
      return m_BufferArray[i].mMsgData;
    }
  }
  return NULL;
}

WordSetLoader::Buffer               WordSetLoader::s_StaticBuffer[ PRELOAD_NO_MAX ];
u32                  WordSetLoader::s_ReadBits;
gfl2::heap::HeapBase * WordSetLoader::s_BufferHeap;

//------------------------------------------------------------------
/**
 * @brief �i���I��WordSet�ɓǂݍ���ł����d�g��
 */
//------------------------------------------------------------------
void WordSetLoader::LoadGlobal( gfl2::heap::HeapBase * pHeap, u32 bits )
{
  s_ReadBits = bits;
  s_BufferHeap = pHeap;

  u32 temp_bits = bits;
  for ( u32 count = 0; temp_bits != 0; ++ count, temp_bits >>= 1 )
  {
    GFL_ASSERT( count < GFL_NELEMS(garc_id_table) );
    if ( temp_bits & 1 )
    {
      u16 dat_id = garc_id_table[count];
      s_StaticBuffer[count].mGarcId   = dat_id;
      s_StaticBuffer[count].mAddress  = NULL;
      s_StaticBuffer[count].mSize     = 0;
      s_StaticBuffer[count].mMsgData  = GFL_NEW( pHeap ) gfl2::str::MsgData(
          print::GetMessageArcId(), dat_id, pHeap, gfl2::str::MsgData::LOAD_FULL );
    }
    else
    {
      s_StaticBuffer[count].mGarcId   = 0;
      s_StaticBuffer[count].mAddress  = NULL;
      s_StaticBuffer[count].mSize     = 0;
      s_StaticBuffer[count].mMsgData  = NULL;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   �풓�o�b�t�@�ēǂݍ��ݏ���
 */
//------------------------------------------------------------------
void WordSetLoader::ReloadGlobal( void )
{
  if ( s_BufferHeap == NULL || s_ReadBits == 0 ) {
    GFL_ASSERT( 0 );  // @check �ǂݍ���ł��Ȃ��̂ɍēǂݍ��݃��N�G�X�g
    return;
  }
  u32 temp_bits = s_ReadBits;
  for ( u32 count = 0; temp_bits != 0; ++ count, temp_bits >>= 1 )
  {
    if ( s_StaticBuffer[count].mMsgData )
    {
      GFL_DELETE s_StaticBuffer[count].mMsgData;
      GFL_ASSERT( temp_bits & 1 );  //@check �t���O�Ƃ̐������`�F�b�N
    }
    s_StaticBuffer[count].mGarcId   = 0;
    s_StaticBuffer[count].mAddress  = NULL;
    s_StaticBuffer[count].mSize     = 0;
    s_StaticBuffer[count].mMsgData  = NULL;
  }
  //�ēǂݍ��݂���
  LoadGlobal( s_BufferHeap, s_ReadBits );
}

//------------------------------------------------------------------
/**
 * @brief �w��ID��MsgData���擾����
 * @param[in]   dat_id
 * @return      MsgData�ւ̃|�C���^�A���݂��Ȃ��ꍇ��NULL��Ԃ�
 */
//------------------------------------------------------------------
gfl2::str::MsgData * WordSetLoader::GetGlobalMsgData( u32 dat_id )
{
  for (u32 i = 0; i < PRELOAD_NO_MAX; ++ i )
  {
    if ( s_StaticBuffer[i].mGarcId == dat_id )
    {
      return s_StaticBuffer[i].mMsgData;
    }
  }
  return NULL;
}

























