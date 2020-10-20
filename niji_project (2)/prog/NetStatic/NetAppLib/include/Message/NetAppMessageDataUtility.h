#if !defined(NETAPPMESSAGEDATAUTILITY_H_INCLUDED)
#define NETAPPMESSAGEDATAUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   NetAppMessageDataUtility.h
  *  @brief  メッセージデータ、StrBuf、WordSetを扱う
  *  @note   通常はUIViewの機能を使ったほうが良い。複数のメッセージデータを扱う必要があるなど
  *          UIViewの機能で対応できない場合の例外的な使用を想定している
  *  @date   2015.06.03 Wed.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <util/include/gfl2_Map.h>
#include  <str/include/gfl2_Str.h>

#include  "Print/include/wordset.h"


/*
  NetAppLib::Message
*/
namespace NetAppLib {
namespace Message {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  MessageUtility
  *  @brief  メッセージリソースの読み込み、StrBufの構築などを行うUtility
  *          複数のメッセージリソースを使うなど、UIView(G2DUtil)のサポートを得られない場合に例外的に使う
  *  @note   UIView::Create2DなどでG2DUtilへ読み込み済みのMessageDataを渡すこともできるので、
  *          メッセージリソースをResourceManagerで読み込んでおき、そのバッファで本クラスを初期化することも可能
  *  @date   2015.06.03 Wed.
  */
//==============================================================================
class MessageUtility
{
  GFL_FORBID_COPY_AND_ASSIGN( MessageUtility );

public:
  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  MessageUtility(gfl2::heap::HeapBase* pHeapBase, u32 dataNum);
  MessageUtility(gfl2::heap::HeapBase* pHeapBase, u32 dataNum, u32 wordMax, u32 bufLen);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual ~MessageUtility();


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Load(const gfl2::fs::ArcFile::ARCDATID garcID);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  LoadFromMemory(const gfl2::fs::ArcFile::ARCDATID garcID, const void* pMessageData);


  //------------------------------------------------------------------
  /**
    *  @func      
    *  @brief    
    */
  //------------------------------------------------------------------
  gfl2::str::MsgData*   GetData(const gfl2::fs::ArcFile::ARCDATID garcID);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  print::WordSet*       GetWordSet(void)
                        {
                          return &m_WordSet;
                        }

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  gfl2::str::StrBuf&    GetStrBuf(void)
                        {
                          return m_StrBuf;
                        }


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  gfl2::str::StrBuf&    GetString(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  gfl2::str::StrBuf&    GetExpandedString(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsValidMessageID(const gfl2::fs::ArcFile::ARCDATID garcID, const u32 messageID)
  {
    return (messageID <= GetData(garcID)->GetStrCount());
  }



  /*
    register系
  */
  void  RegisterPokeLevel(const u32 bufID, const u32 level)
        {
          const u32                     digits    = 3;
          const print::NumberDispType   dispType  = print::NUM_DISP_LEFT;
          const print::NumberCodeType   codeType  = print::NUM_CODE_HANKAKU;
          
          m_WordSet.RegisterNumber(bufID, level, digits, dispType, codeType);
        }



protected:
  static const u32                                                    STR_MAX = 1024; // @fix GFNMCat[2375] 使用言語が「ドイツ語」のとき、「ゲームシンク」にはいるとアサートが発生してはまる
                                                                                      // 512 では足りない箇所が出てきたので 1024 にしました

  gfl2::heap::HeapBase*                                               m_pHeapBase;
  u32                                                                 m_DataNum;
  gfl2::util::Map<gfl2::fs::ArcFile::ARCDATID, gfl2::str::MsgData*>   m_MsgDataMap;   /**<  GARC_xxxから MsgData* を引くためのMap  */
  /*    */
  gfl2::str::StrBuf                                                   m_StrBuf;
  print::WordSet                                                      m_WordSet;
};


} /*  namespace Message   */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPMESSAGEDATAUTILITY_H_INCLUDED)  */
