// ============================================================================
/*
 * @file JoinFestaSelectWordTable.cpp
 * @brief 挨拶選択アプリで使用するテーブル
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaSelectWordTable.h"
#include <debug/include/gfl2_Assert.h>
#include <niji_conv_header/message/msg_setphrase.h>
#include <niji_conv_header/message/msg_jf_phrase.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)


/************************************************************************/
/* 開放するメッセージIDテーブル                                         */
/************************************************************************/
const u32 JoinFestaSelectWordTable::OPEN_MESSAGE_ID_TBL[ JoinFestaSelectWordTable::OPEN_MESSAGE_NUM ] = {
  msg_jf_phrase_m_082, msg_jf_phrase_m_083, msg_jf_phrase_m_084, msg_jf_phrase_m_085, msg_jf_phrase_m_086,
  msg_jf_phrase_m_087, msg_jf_phrase_m_088, msg_jf_phrase_m_089, msg_jf_phrase_m_090, msg_jf_phrase_m_091,
  msg_jf_phrase_m_092, msg_jf_phrase_m_093, msg_jf_phrase_m_094, msg_jf_phrase_m_095, msg_jf_phrase_m_096,
  msg_jf_phrase_m_097, msg_jf_phrase_m_098, msg_jf_phrase_m_099, msg_jf_phrase_m_100, msg_jf_phrase_m_101,
  msg_jf_phrase_m_102, msg_jf_phrase_m_103, msg_jf_phrase_m_104, msg_jf_phrase_m_105, msg_jf_phrase_m_106,
  msg_jf_phrase_m_107, msg_jf_phrase_m_108, msg_jf_phrase_m_109, msg_jf_phrase_m_110, msg_jf_phrase_m_111,
  msg_jf_phrase_m_112, msg_jf_phrase_m_113, msg_jf_phrase_m_114, msg_jf_phrase_m_115, msg_jf_phrase_m_116,
  msg_jf_phrase_m_117, msg_jf_phrase_m_118, msg_jf_phrase_m_119, msg_jf_phrase_m_120, msg_jf_phrase_m_121,
  msg_jf_phrase_m_122, msg_jf_phrase_m_123, msg_jf_phrase_m_124, msg_jf_phrase_m_125, msg_jf_phrase_m_126,
  msg_jf_phrase_m_127, msg_jf_phrase_m_128, msg_jf_phrase_m_129, msg_jf_phrase_m_130, msg_jf_phrase_m_131,
  msg_jf_phrase_m_132, msg_jf_phrase_m_133, msg_jf_phrase_m_134, msg_jf_phrase_m_135, msg_jf_phrase_m_136,
  msg_jf_phrase_m_137, msg_jf_phrase_m_138, msg_jf_phrase_m_139, msg_jf_phrase_m_140, msg_jf_phrase_m_141,
  msg_jf_phrase_m_142, msg_jf_phrase_m_143, msg_jf_phrase_m_144, msg_jf_phrase_m_145, msg_jf_phrase_m_146,
  msg_jf_phrase_m_147, msg_jf_phrase_m_148, msg_jf_phrase_m_149, msg_jf_phrase_m_150, msg_jf_phrase_m_151,
  msg_jf_phrase_m_152, msg_jf_phrase_m_153, msg_jf_phrase_m_154, msg_jf_phrase_m_155, msg_jf_phrase_m_156,
  msg_jf_phrase_m_157, msg_jf_phrase_m_158, msg_jf_phrase_m_159, msg_jf_phrase_m_160, msg_jf_phrase_m_161,
  msg_jf_phrase_m_162, msg_jf_phrase_m_163, msg_jf_phrase_m_164, msg_jf_phrase_m_165, msg_jf_phrase_m_166,
  msg_jf_phrase_m_167, msg_jf_phrase_m_168, msg_jf_phrase_m_169, msg_jf_phrase_m_170, msg_jf_phrase_m_171,
  msg_jf_phrase_m_172, msg_jf_phrase_m_173, msg_jf_phrase_m_174, msg_jf_phrase_m_175, msg_jf_phrase_m_176,
  msg_jf_phrase_m_177, msg_jf_phrase_m_178, msg_jf_phrase_m_179, msg_jf_phrase_m_180, msg_jf_phrase_m_181,
  msg_jf_phrase_m_182, msg_jf_phrase_m_183, msg_jf_phrase_m_184, msg_jf_phrase_m_185, msg_jf_phrase_m_186,
  msg_jf_phrase_m_187, msg_jf_phrase_m_250
};


JoinFestaSelectWordTable::JoinFestaSelectWordTable() :
  m_pMessageInfoTable( NULL ),
  m_aCategoryTable(),
  m_paMessageTables()
{
}


JoinFestaSelectWordTable::~JoinFestaSelectWordTable()
{
}


void JoinFestaSelectWordTable::Setup( void* pMessageInfoBuffer, void* pMessageTableBuffer )
{
  int* pTable = reinterpret_cast<int*>( pMessageTableBuffer );

  int tableSize = pTable[0];

  // こんな感じのフォーマット
  // [全体サイズ][messageID][messageID][messageID][-1][messageID][messageID][messageID][messageID][-1][messageID][messageID][messageID][-1]
  // この場合、全体サイズは 14 となる

  m_pMessageInfoTable   = reinterpret_cast<MessageInfo*>( pMessageInfoBuffer );

  for( int i = 0; i < CATEGORY_MAX; ++i )
  {
    m_paMessageTables[i] = getTableTopAddress( i, &pTable[1], tableSize - 1 );
  }

  m_aCategoryTable[ CATEGORY_TRAINER        ].messageID  = msg_setphrase_categ_01;
  m_aCategoryTable[ CATEGORY_MYSELF         ].messageID  = msg_setphrase_categ_02;
  m_aCategoryTable[ CATEGORY_HOBBY          ].messageID  = msg_setphrase_categ_03;
  m_aCategoryTable[ CATEGORY_SERIF          ].messageID  = msg_setphrase_categ_04;
  m_aCategoryTable[ CATEGORY_SERIES         ].messageID  = msg_setphrase_categ_05;
  m_aCategoryTable[ CATEGORY_ALOLA_DONE     ].messageID  = msg_setphrase_categ_06;
  m_aCategoryTable[ CATEGORY_WANT_TO_DO     ].messageID  = msg_setphrase_categ_07;
  m_aCategoryTable[ CATEGORY_COMM_REPORT    ].messageID  = msg_setphrase_categ_08;
  m_aCategoryTable[ CATEGORY_JOY            ].messageID  = msg_setphrase_categ_09;
  m_aCategoryTable[ CATEGORY_ANGER          ].messageID  = msg_setphrase_categ_10;
  m_aCategoryTable[ CATEGORY_SADNESS        ].messageID  = msg_setphrase_categ_11;
  m_aCategoryTable[ CATEGORY_ENJOY          ].messageID  = msg_setphrase_categ_12;
  m_aCategoryTable[ CATEGORY_SURPRISE       ].messageID  = msg_setphrase_categ_13;
  m_aCategoryTable[ CATEGORY_HELLO          ].messageID  = msg_setphrase_categ_14;
  m_aCategoryTable[ CATEGORY_CALL           ].messageID  = msg_setphrase_categ_15;
  m_aCategoryTable[ CATEGORY_ANSWER         ].messageID  = msg_setphrase_categ_16;
  m_aCategoryTable[ CATEGORY_MOVEMENT       ].messageID  = msg_setphrase_categ_17;
  m_aCategoryTable[ CATEGORY_FAVORITE_TYPE  ].messageID  = msg_setphrase_categ_18;
  m_aCategoryTable[ CATEGORY_WISH           ].messageID  = msg_setphrase_categ_19;
  m_aCategoryTable[ CATEGORY_FEELING        ].messageID  = msg_setphrase_categ_20;
}


JoinFestaSelectWordTable::Message* JoinFestaSelectWordTable::getTableTopAddress( int index, int* pTable, int tableSize )
{
  int topAddressIndex = 0;

  if( index > 0 )
  {
    int temp = 0;

    for( int i = 0; i < tableSize; ++i )
    {
      if( pTable[i] == INVALID_MESSAGE_ID )
      {
        ++temp;
      }

      if( index == temp )
      {
        topAddressIndex = i + 1;

        GFL_ASSERT( ( topAddressIndex < tableSize ) ); // @fix : ここで止まるならデータが壊れている

        break;
      }
    }
  }

  Message* pMessageTable = reinterpret_cast<Message*>( &pTable[ topAddressIndex ] );

  return pMessageTable;
}


u32 JoinFestaSelectWordTable::GetMessageTableSize( Category eCategory )
{
  u32 size = 0;

  u32 tableMaxSize = msg_jf_phrase_max / 2;

  for( u32 i = 0; i < tableMaxSize; ++i )
  {
    if( m_paMessageTables[ eCategory ][i].messageID == INVALID_MESSAGE_ID )
    {
      break;
    }
    ++size;
  }

  return size;
}

/**
 *  @brief メッセージIDがどのカテゴリか取得
 *  @param  messageId チェックするメッセージのId
 *  @retrun カテゴリ定義
 */
JoinFestaSelectWordTable::Category JoinFestaSelectWordTable::GetCategory( const u32 messageId )
{
  for( int category = 0; category < CATEGORY_MAX; ++category )
  {
    const u32 tableSize = GetMessageTableSize( static_cast<Category>(category) );

    for( int tblIndex=0; tblIndex<tableSize; tblIndex++ )
    {
      if( m_paMessageTables[ category ][ tblIndex ].messageID == messageId )
      {
        return static_cast<Category>(category);
      }
    }
  }
  return CATEGORY_MAX;
}

/**
 *  @brief 開放するメッセージかどうか
 *  @param  messageId チェックするメッセージのId
 *  @return 開放するメッセージの場合、インデックスを返す。開放するメッセージじゃない場合は-1
 */
int JoinFestaSelectWordTable::IsOpenMessageId( u32 messageId )
{
  GFL_ASSERT( messageId < msg_jf_phrase_max );

  // 女性用メッセージだった場合は、男性用メッセージに合わせる
  if( messageId >= (msg_jf_phrase_max/2) )
  {
    messageId -= (msg_jf_phrase_max/2);
  }


  for( u32 i=0; i<OPEN_MESSAGE_NUM; i++ )
  {
    if( OPEN_MESSAGE_ID_TBL[i] == messageId )
    {
      return i;
    }
  }

  return -1;
}


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)
