// ============================================================================
/*
 * @file JoinFestaSelectWordTable.h
 * @brief 挨拶選択アプリで使用するテーブル
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDTABLE_H_INCLUDE )
#define JOINFESTASELECTWORDTABLE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)


class JoinFestaSelectWordTable
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordTable );

public:

  enum Category
  {
    CATEGORY_TRAINER,         //! すきな　トレーナー
    CATEGORY_MYSELF,          //! じぶんの　こと
    CATEGORY_HOBBY,           //! しゅみ
    CATEGORY_SERIF,           //! セリフ
    CATEGORY_SERIES,          //! ポケモンの　シリーズに　ついて
    CATEGORY_ALOLA_DONE,      //! アローラで　していること
    CATEGORY_WANT_TO_DO,      //! ポケモンの　せかいで　したいこと
    CATEGORY_COMM_REPORT,     //! つうしんで　つたえることば
    CATEGORY_JOY,             //! よろこび
    CATEGORY_ANGER,           //! いかり
    CATEGORY_SADNESS,         //! かなしみ
    CATEGORY_ENJOY,           //! たのしい
    CATEGORY_SURPRISE,        //! おどろき
    CATEGORY_HELLO,           //! あいさつ
    CATEGORY_CALL,            //! かけごえ
    CATEGORY_ANSWER,          //! へんじ
    CATEGORY_MOVEMENT,        //! うごき
    CATEGORY_FAVORITE_TYPE,   //! すきな　ポケモンの　タイプ
    CATEGORY_WISH,            //! ねがいごと
    CATEGORY_FEELING,         //! きもち
    CATEGORY_MAX
  };

  static const s32   INVALID_MESSAGE_ID  = -1;

  //! 開放に条件があるメッセージの総数
  static const u32 OPEN_MESSAGE_NUM = 107;

  typedef struct
  {
    s32     color;
    s32     flag;
  } MessageInfo;

  typedef struct
  {
    s32     messageID;
  } Message;


public:

  JoinFestaSelectWordTable();
  virtual~JoinFestaSelectWordTable();

  void Setup( void* pMessageInfoBuffer, void* pMessageTableBuffer );

  u32 GetCategoryTableSize(){ return CATEGORY_MAX; }
  Message* GetCategoryTable() { return &m_aCategoryTable[0]; }

  u32 GetMessageTableSize( Category eCategory );
  Message* GetMessageTable( Category eCategory ) { return m_paMessageTables[ eCategory ]; }

  //! メッセージIDがどのカテゴリか取得
  Category GetCategory( const u32 messageId );

  //! 開放するメッセージかどうか
  static int IsOpenMessageId( u32 messageId );

private:

  Message* getTableTopAddress( int index, int* pTable, int tableSize );

private:

  MessageInfo*      m_pMessageInfoTable;
  Message           m_aCategoryTable[ CATEGORY_MAX ];
  Message*          m_paMessageTables[ CATEGORY_MAX ];

  // 開放するメッセージIDテーブル
  static const u32 OPEN_MESSAGE_ID_TBL[ OPEN_MESSAGE_NUM ];

public:
#if PM_DEBUG
  // 開放するメッセージIDテーブル取得
  static const u32* GetOpenMessageIdTbl() { return OPEN_MESSAGE_ID_TBL; }
#endif
};


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTASELECTWORDTABLE_H_INCLUDE
