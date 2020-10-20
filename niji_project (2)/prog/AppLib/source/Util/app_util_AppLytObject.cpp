//==============================================================================
/**
 @file    app_util_AppLytObject.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.01.16
 @brief   applytバイナリからつくるオブジェクト郡
 */
//==============================================================================


// gfl2のインクルード
#include <str/include/gfl2_MsgData.h>
#include <str/include/gfl2_Font.h>


// nijiのデータのヘッダーファイルのインクルード
#include <niji_reference_files/font/LayoutDefaultFontDefine.h>
#include <arc_def_index/arc_def.h>


// nijiのインクルード
#include <Print/include/SystemFont.h>
#include <Print/include/WordSet.h>
#include <Print/include/WordSetLoader.h>

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <AppLib/include/Util/app_util_AppLytObject.h>




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


AppLytObject::AppLytObject(void)
  : m_appLytBuf(NULL),
    m_msgBufInfo(NULL),
    m_msgBufInfoNumMax(0),
    m_msgBufInfoNum(0),
    m_fontCreateInfo(NULL)
{}
AppLytObject::~AppLytObject()
{
  this->destroy();
}

void AppLytObject::SyncCreate(const void* appLytBuf, gfl2::heap::HeapBase* heap)
{
  m_appLytBuf = appLytBuf;

  app::util::AppLytAccessor appLytAccessor;
  appLytAccessor.SetAppLytBuf(m_appLytBuf);
  const void* lytMstxtListBuf = appLytAccessor.GetLytMstxtList();
  const void* lytFontListBuf  = appLytAccessor.GetLytFontList();

  // メッセージバッファ情報
  {
    // LytMstxtListAccessorにバイナリを設定する
    app::util::LytMstxtListAccessor lytMstxtListAccessor;
    lytMstxtListAccessor.SetMstxtListBuf(lytMstxtListBuf);

    // まずはMsgBufInfoの個数を確定させる
    u32 lytDatMessageArcNum = lytMstxtListAccessor.GetLytDatMessageArcNum();
    for(u32 i=0; i<lytDatMessageArcNum; ++i)
    {
      u32 lytDatMessageArcDatNum = lytMstxtListAccessor.GetLytDatMessageArcDatNumByIndex(i);
      m_msgBufInfoNumMax += lytDatMessageArcDatNum;
    }
    m_msgBufInfo = GFL_NEW_ARRAY(heap) MsgBufInfo[m_msgBufInfoNumMax];

    // 次にMsgBufInfoのメンバを設定していく
    for(u32 i=0; i<lytDatMessageArcNum; ++i)
    {
      u32 lytDatMessageArcId = lytMstxtListAccessor.GetLytDatMessageArcId(i);
      u32 lytDatMessageArcDatNum = lytMstxtListAccessor.GetLytDatMessageArcDatNumByIndex(i);
      for(u32 j=0; j<lytDatMessageArcDatNum; ++j)
      {
        u16 lytDatMessageArcDatId = lytMstxtListAccessor.GetLytDatMessageArcDatIdTableByIndex(i)[j];

        // arcId決定
        gfl2::fs::ArcFile::ARCID arcIdLang = gfl2::fs::ArcFile::ARCID_NULL;
        switch(lytDatMessageArcId)
        {
        case ARCID_MESSAGE_JPN:
          {
            arcIdLang = print::GetMessageArcId();
            break;
          }
        case ARCID_DEBUG_MESSAGE_JPN:
          {
#if PM_DEBUG
            arcIdLang = print::GetMessageArcId_Debug();
#endif  // PM_DEBUG
          }
        case ARCID_SCRIPT_MESSAGE_JPN:
          {
            arcIdLang = print::GetMessageArcId_Script();
            break;
          }
        default:
          {
            GFL_ASSERT(0);  // applytに入っている番号がおかしい
            break;
          }
        }

        if(arcIdLang != gfl2::fs::ArcFile::ARCID_NULL)
        {
          // メッセージ全部を同期読み込み
          void* msgBuf = NULL;
          app::util::FileAccessor::FileOpenSync( arcIdLang, heap, true );
          app::util::FileAccessor::FileLoadSync(
              arcIdLang,
              lytDatMessageArcDatId,
              &msgBuf,    
              heap,
              false,
              4
          );
          // ファイルクローズは最後にまとめて行うので、ここでは行わない。

          // メッセージデータ生成
          gfl2::str::MsgData* msgData = GFL_NEW(heap) gfl2::str::MsgData(msgBuf, heap);
          
          m_msgBufInfo[m_msgBufInfoNum].arcId     = lytDatMessageArcId;
          m_msgBufInfo[m_msgBufInfoNum].arcIdLang = arcIdLang;
          m_msgBufInfo[m_msgBufInfoNum].datId     = lytDatMessageArcDatId;
          m_msgBufInfo[m_msgBufInfoNum].msgBuf    = msgBuf;
          m_msgBufInfo[m_msgBufInfoNum].msgData   = msgData;
          ++m_msgBufInfoNum;
        }
      }
    }

    for(u32 i=0; i<m_msgBufInfoNum; ++i)
    {
      // ファイルクローズ
      app::util::FileAccessor::FileCloseSync( m_msgBufInfo[i].arcIdLang );
    }
    
    lytMstxtListAccessor.UnsetMstxtListBuf();
  }

  // フォント生成情報
  {
    // LytFontListAccessorにバイナリを設定する
    app::util::LytFontListAccessor lytFontListAccessor;
    lytFontListAccessor.SetFontListBuf(lytFontListBuf);

    // まずはFontCreateInfoを作成する
    m_fontCreateInfo = GFL_NEW_ARRAY(heap) FontCreateInfo[DEFAULT_FONT_IDX_MAX];
    for(u32 fontId=0; fontId<DEFAULT_FONT_IDX_MAX; ++fontId)
    {
      switch(fontId)
      {
      case DEFAULT_FONT_IDX_KUJIRA_DEFAULT:
        {
          // 設定済みのはずなので、何もしなくていいはず。
          break;
        }
      case DEFAULT_FONT_LAYOUT_NUM_FONT:
        {
          m_fontCreateInfo[fontId].CreateFunc = print::SystemFont_CreateNumFont;
          m_fontCreateInfo[fontId].DeleteFunc = print::SystemFont_DeleteNumFont;
          m_fontCreateInfo[fontId].SetFunc    = print::SystemFont_SetLytSystemNumFont;
          m_fontCreateInfo[fontId].ResetFunc  = print::SystemFont_ResetNumFontInfo;
          m_fontCreateInfo[fontId].GetFunc    = print::SystemFont_GetNumFont;
          m_fontCreateInfo[fontId].bitFlag    |= FontCreateInfo::BIT_FLAG_FUNC_EXIST;
          break;
        }
      case DEFAULT_FONT_LAYOUT_BATTLE_FONT:
        {
          m_fontCreateInfo[fontId].CreateFunc = print::SystemFont_CreateBattleFont;
          m_fontCreateInfo[fontId].DeleteFunc = print::SystemFont_DeleteBattleFont;
          m_fontCreateInfo[fontId].SetFunc    = print::SystemFont_SetLytSystemBattleFont;
          m_fontCreateInfo[fontId].ResetFunc  = print::SystemFont_ResetBattleFontInfo;
          m_fontCreateInfo[fontId].GetFunc    = print::SystemFont_GetBattleFont;
          m_fontCreateInfo[fontId].bitFlag    |= FontCreateInfo::BIT_FLAG_FUNC_EXIST;
          break;
        }
      case DEFAULT_FONT_LAYOUT_TWHK_FONT:
        {
          // TESTで括られていたので今は何もしない。
          break;
        }
      }
    }

    // 次にフォントを生成していく
    u32 lytDatFontNum = lytFontListAccessor.GetLytDatFontNum();
    for(u32 i=0; i<lytDatFontNum; ++i)
    {
      u16 fontId = lytFontListAccessor.GetLytDatFontIdTable()[i];
      GFL_ASSERT(fontId < DEFAULT_FONT_IDX_MAX);  // applytに入っている番号がおかしい
      
      if(m_fontCreateInfo[fontId].bitFlag & FontCreateInfo::BIT_FLAG_FUNC_EXIST)
      {
        if(m_fontCreateInfo[fontId].GetFunc() == NULL)
        {
          m_fontCreateInfo[fontId].CreateFunc(heap, heap);
          m_fontCreateInfo[fontId].SetFunc();
          m_fontCreateInfo[fontId].bitFlag |= FontCreateInfo::BIT_FLAG_CREATE;
        }
      }
    }
   
    lytFontListAccessor.UnsetFontListBuf();
  }

  appLytAccessor.UnsetAppLytBuf();
}

void AppLytObject::destroy(void)
{
  ////////////////////////////////////////////////////////////////////////////////
  // 何回呼んでも大丈夫な書き方をしてある。
  ////////////////////////////////////////////////////////////////////////////////

  // フォント生成情報
  {
    if(m_fontCreateInfo)
    {
      for(u32 fontId=0; fontId<DEFAULT_FONT_IDX_MAX; ++fontId)
      {
        if(m_fontCreateInfo[fontId].bitFlag & FontCreateInfo::BIT_FLAG_FUNC_EXIST)
        {
          if(m_fontCreateInfo[fontId].bitFlag & FontCreateInfo::BIT_FLAG_CREATE)
          {
            m_fontCreateInfo[fontId].ResetFunc();
            m_fontCreateInfo[fontId].DeleteFunc();
          }
        }
      }
      GFL_SAFE_DELETE_ARRAY(m_fontCreateInfo);
    }
  }

  // メッセージバッファ情報
  {
    for(u32 i=0; i<m_msgBufInfoNum; ++i)
    {
      GFL_SAFE_DELETE(m_msgBufInfo[i].msgData);
      GflHeapSafeFreeMemory(m_msgBufInfo[i].msgBuf);
    }
    GFL_SAFE_DELETE_ARRAY(m_msgBufInfo);
  
    m_msgBufInfoNumMax = 0;
    m_msgBufInfoNum    = 0;
  }
}

//void AppLytObject::SetWordSetToG2DUtil(app::util::G2DUtil* g2DUtil, print::WordSet* wordSet, WordSetLoader* ploader)
//{
//  g2DUtil->SetMessageData(NULL, wordSet);
//  g2DUtil->SetWordSetLoader(ploader);
//}

void AppLytObject::SetMessageToTextBoxPane(app::util::G2DUtil* g2DUtil, print::WordSet* wordSet, u32 lytID, u32 lytresId) const
{
  gfl2::str::StrBuf* strBuf0 = g2DUtil->GetTempStrBuf(0);
  gfl2::str::StrBuf* strBuf1 = g2DUtil->GetTempStrBuf(1);

  app::util::AppLytAccessor appLytAccessor;
  appLytAccessor.SetAppLytBuf(m_appLytBuf);
  const void* lytMstxtListBuf = appLytAccessor.GetLytMstxtList();

  // メッセージバッファ情報
  {
    // LytMstxtListAccessorにバイナリを設定する
    app::util::LytMstxtListAccessor lytMstxtListAccessor;
    lytMstxtListAccessor.SetMstxtListBuf(lytMstxtListBuf);

    // lytresIdからメッセージデータを取得する
    u32 textBoxPaneNum = lytMstxtListAccessor.GetTextBoxPaneNum(lytresId);
    const app::util::LytMstxtListAccessor::TextBoxPaneInfo* textBoxPaneInfo = lytMstxtListAccessor.GetTextBoxPaneInfoTable(lytresId);
    for(u32 i=0; i<textBoxPaneNum; ++i)
    {
      gfl2::str::MsgData* msgData = NULL;
      for(u32 j=0; j<m_msgBufInfoNum; ++j)
      {
        if(
             m_msgBufInfo[j].arcId == textBoxPaneInfo[i].messageArcId
          && m_msgBufInfo[j].datId == textBoxPaneInfo[i].messageArcDatId
        )
        {
          msgData = m_msgBufInfo[j].msgData;
          break;
        }
      }

      if(msgData)
      {
        msgData->GetString(textBoxPaneInfo[i].messageId, *strBuf0);  // strBuf0に文字列を読み込む
        wordSet->Expand(strBuf1, strBuf0);  // strBuf0の文字列をstrBuf1に展開する

        // g2DUtil内のlytIDに文字列を設定する
        g2DUtil->SetTextBoxPaneString(lytID, textBoxPaneInfo[i].paneId, strBuf1);
      }
    }

    lytMstxtListAccessor.UnsetMstxtListBuf();
  } 
}

gfl2::str::MsgData* AppLytObject::GetMsgData(gfl2::fs::ArcFile::ARCID arcIdLang, gfl2::fs::ArcFile::ARCDATID datId) const
{
  ////////////////////////////////////////////////////////////////////////////////
  // いつ呼んでも大丈夫な書き方をしてある。
  ////////////////////////////////////////////////////////////////////////////////

  gfl2::str::MsgData* msgData = NULL;
  for(u32 j=0; j<m_msgBufInfoNum; ++j)
  {
    if(
         m_msgBufInfo[j].arcIdLang == arcIdLang
      && m_msgBufInfo[j].datId == datId
    )
    {
      msgData = m_msgBufInfo[j].msgData;
      break;
    }
  }
  return msgData;
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
