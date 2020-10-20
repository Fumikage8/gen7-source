//======================================================================
/**
 * @file app_tool_InfoWindow.cpp
 * @date 2015/07/02 15:49:24
 * @author kawada_koji
 * @brief インフォウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


// gfl2のインクルード
#include <str/include/gfl2_StrSys.h>
#include <str/include/gfl2_StrBuf.h>
#include <thread/include/gfl2_ThreadStatic.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/app_information.gaix>
#include <niji_conv_header/app/information/res2d/information_sml.h>
#include <niji_conv_header/app/information/res2d/information_sml_anim_list.h>
#include <niji_conv_header/app/information/res2d/information_sml_pane.h>
#include <niji_conv_header/app/information/res2d/information_lrg.h>
#include <niji_conv_header/app/information/res2d/information_lrg_anim_list.h>
#include <niji_conv_header/app/information/res2d/information_lrg_pane.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_info.h>

// nijiのインクルード
#include "AppLib/include/ui/UIInputListener.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <Sound/include/Sound.h>
#include <System/include/ArcIdGetter.h>
#include <Print/include/PrintSystem.h>

#include "app_tool_InfoWindowDefine.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowDataManager.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
//
// InfoWindowDataManager
//

const InfoWindowDataManager::DataType InfoWindowDataManager::DATA_TYPE[DATA_ID_NUM] =
{
  DATA_TYPE_VOID,
  DATA_TYPE_VOID,
  DATA_TYPE_STRBUF,
  DATA_TYPE_STRBUF,
};

InfoWindowDataManager::InfoWindowDataManager(void)
  : m_isEndFuncCalled(false),
    m_createState(CREATE_STATE_NONE),
    m_createType(CREATE_TYPE_SHEET_BOOK),
    m_appHeap(NULL),
    m_tempMsgDataBuf(NULL),
    m_msgArcId(gfl2::fs::ArcFile::ARCID_NULL)
{
  for(u32 i=0; i<DATA_ID_NUM; ++i)
  {
    m_dataBuf[i]   = NULL;
  }
}

InfoWindowDataManager::~InfoWindowDataManager()
{
  bool endFlag = this->EndFunc();
  GFL_ASSERT(endFlag);  // 開発中に気付かせるためのASSERT。ケア済み。  // EndFuncがtrueを返すようになってから破棄して下さい。

  // 終了処理が終わるのを待つ
  u32 loopCount = 0; 
  while( this->EndFunc() == false )
  {
    ++loopCount;
    if(loopCount >= InfoWindowDefine::LOOP_COUNT_MAX) break;  // 無限ループ対策
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  // 終了処理が終わる前にここに来てしまったら、メモリ確保したものが残っている。
  // データバッファの破棄
  this->destroyDataBuf();
}

void InfoWindowDataManager::StartAsyncCreate(
    app::util::Heap* appHeap,
    CreateType       createType
)
{
  if(m_isEndFuncCalled == false)  // Create, UpdateFunc, GetDataBufはEndFuncが呼び出された後は禁止。
  {
    if(m_createState == CREATE_STATE_NONE)
    {
      m_createState = CREATE_STATE_CREATING;
      m_appHeap = appHeap;
      m_createType = createType;
    
      // ARCID_APP_INFORMATION
      {
        app::util::FileAccessor::FileOpen(NIJI_ARCID_NUM(ARCID_APP_INFORMATION), appHeap->GetDeviceHeap(), true);
     
        if(createType == CREATE_TYPE_SHEET || createType == CREATE_TYPE_SHEET_BOOK)
        {
          gfl2::fs::ArcFile::ARCDATID arcDatId = GARC_app_information_information_sml_APPLYT;
          DataId                      dataId   = DATA_ID_SHEET_TYPE;
          app::util::FileAccessor::FileLoad(
              NIJI_ARCID_NUM(ARCID_APP_INFORMATION),
              arcDatId,
              &m_dataBuf[dataId],
              appHeap->GetDeviceHeap(),
              false,
              128
          );
        }

        if(createType == CREATE_TYPE_BOOK || createType == CREATE_TYPE_SHEET_BOOK)
        {
          gfl2::fs::ArcFile::ARCDATID arcDatId = GARC_app_information_information_lrg_APPLYT;
          DataId                      dataId   = DATA_ID_BOOK_TYPE;
          app::util::FileAccessor::FileLoad(
              NIJI_ARCID_NUM(ARCID_APP_INFORMATION),
              arcDatId,
              &m_dataBuf[dataId],
              appHeap->GetDeviceHeap(),
              false,
              128
          );
        }
     
        app::util::FileAccessor::FileClose(NIJI_ARCID_NUM(ARCID_APP_INFORMATION), appHeap->GetDeviceHeap());
      }

      // GARC_message_info_DAT
      {
        if(createType == CREATE_TYPE_BOOK || createType == CREATE_TYPE_SHEET_BOOK)
        {
          m_msgArcId                           = print::GetMessageArcId();
          gfl2::fs::ArcFile::ARCDATID arcDatId = GARC_message_info_DAT;

          app::util::FileAccessor::FileOpen(m_msgArcId, appHeap->GetDeviceHeap(), true);
          
          app::util::FileAccessor::FileLoad(
              m_msgArcId,
              arcDatId,
              &m_tempMsgDataBuf,
              appHeap->GetDeviceHeap()->GetLowerHandle(),  // 一時的に借りているだけなので後方確保
              false,
              4
          );

          app::util::FileAccessor::FileClose(m_msgArcId, appHeap->GetDeviceHeap());
        }
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // 既に生成中か生成済み。
    }
  }
  else
  {
    GFL_ASSERT(0);  // EndFuncが呼び出された後。  // 開発中に気付かせるためのASSERT
  }
}

void InfoWindowDataManager::SyncCreate(
    app::util::Heap* appHeap,
    CreateType       createType
)
{
  if(m_isEndFuncCalled == false)  // Create, UpdateFunc, GetDataBufはEndFuncが呼び出された後は禁止。
  {
    if(m_createState == CREATE_STATE_NONE)
    {
      m_createState = CREATE_STATE_CREATING;
      m_appHeap = appHeap;
      m_createType = createType;

      // ARCID_APP_INFORMATION
      {
        app::util::FileAccessor::FileOpenSync(NIJI_ARCID_NUM(ARCID_APP_INFORMATION), appHeap->GetDeviceHeap(), true);

        if(createType == CREATE_TYPE_SHEET || createType == CREATE_TYPE_SHEET_BOOK)
        {
          gfl2::fs::ArcFile::ARCDATID arcDatId = GARC_app_information_information_sml_APPLYT;
          DataId                      dataId   = DATA_ID_SHEET_TYPE;
          app::util::FileAccessor::FileLoadSync(
              NIJI_ARCID_NUM(ARCID_APP_INFORMATION),
              arcDatId,
              &m_dataBuf[dataId],
              appHeap->GetDeviceHeap(),
              false,
              128
          );
        }

        if(createType == CREATE_TYPE_BOOK || createType == CREATE_TYPE_SHEET_BOOK)
        {
          gfl2::fs::ArcFile::ARCDATID arcDatId = GARC_app_information_information_lrg_APPLYT;
          DataId                      dataId   = DATA_ID_BOOK_TYPE;
          app::util::FileAccessor::FileLoadSync(
              NIJI_ARCID_NUM(ARCID_APP_INFORMATION),
              arcDatId,
              &m_dataBuf[dataId],
              appHeap->GetDeviceHeap(),
              false,
              128
          );
        }

        app::util::FileAccessor::FileCloseSync(NIJI_ARCID_NUM(ARCID_APP_INFORMATION));
      }

      // GARC_message_info_DAT
      {
        if(createType == CREATE_TYPE_BOOK || createType == CREATE_TYPE_SHEET_BOOK)
        {
          m_msgArcId                           = print::GetMessageArcId();
          gfl2::fs::ArcFile::ARCDATID arcDatId = GARC_message_info_DAT;

          app::util::FileAccessor::FileOpenSync(m_msgArcId, appHeap->GetDeviceHeap(), true);
          
          app::util::FileAccessor::FileLoadSync(
              m_msgArcId,
              arcDatId,
              &m_tempMsgDataBuf,
              appHeap->GetDeviceHeap()->GetLowerHandle(),  // 一時的に借りているだけなので後方確保
              false,
              4
          );

          app::util::FileAccessor::FileCloseSync(m_msgArcId);

          this->createStrBufData();
        }
      }
     
      m_appHeap = NULL;
      m_createState = CREATE_STATE_CREATED;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // 既に生成中か生成済み。
    }
  }
  else
  {
    GFL_ASSERT(0);  // EndFuncが呼び出された後。  // 開発中に気付かせるためのASSERT
  }
}

bool InfoWindowDataManager::IsCreated(void) const
{
  return ( m_createState == CREATE_STATE_CREATED );
}

void InfoWindowDataManager::UpdateFunc(void)
{
  if(m_isEndFuncCalled == false)  // Create, UpdateFunc, GetDataBufはEndFuncが呼び出された後は禁止。
  {
    this->localUpdate();
  }
  else
  {
    GFL_ASSERT(0);  // EndFuncが呼び出された後。  // 開発中に気付かせるためのASSERT
  }
}

bool InfoWindowDataManager::EndFunc(
    app::util::Heap* appHeap
)
{
  m_isEndFuncCalled = true;

  this->localUpdate();
  
  bool canDestroy = ( m_createState == CREATE_STATE_NONE || m_createState == CREATE_STATE_CREATED );

  if(canDestroy)
  {
    // データバッファの破棄
    this->destroyDataBuf();
  }

  m_createState = CREATE_STATE_NONE;

  return canDestroy;
}

void* InfoWindowDataManager::GetDataBuf(
    DataId dataId
) const
{
  void* dataBuf = NULL;
  if(m_isEndFuncCalled == false)  // Create, UpdateFunc, GetDataBufはEndFuncが呼び出された後は禁止。
  {
    if(m_createState == CREATE_STATE_CREATED)
    {
      dataBuf = m_dataBuf[dataId];
    }
  }
  else
  {
    GFL_ASSERT(0);  // EndFuncが呼び出された後。  // 開発中に気付かせるためのASSERT
  }
  return dataBuf;
}

void InfoWindowDataManager::localUpdate(void)
{
  if(m_createState == CREATE_STATE_CREATING)
  {
    bool isCreated = true;
  
    // ARCID_APP_INFORMATION
    {
      if(m_createType == CREATE_TYPE_SHEET || m_createType == CREATE_TYPE_SHEET_BOOK)
      {
        if( !(    app::util::FileAccessor::IsFileOpen( NIJI_ARCID_NUM(ARCID_APP_INFORMATION ) )
               && app::util::FileAccessor::IsFileLoad( &m_dataBuf[DATA_ID_SHEET_TYPE] )
               && app::util::FileAccessor::IsFileClose( NIJI_ARCID_NUM(ARCID_APP_INFORMATION) ) ) )  // 発行したリクエスト全部見張る
        {
          isCreated = false;
        }
      }
      if(m_createType == CREATE_TYPE_BOOK || m_createType == CREATE_TYPE_SHEET_BOOK)
      {
        if( !(    app::util::FileAccessor::IsFileOpen( NIJI_ARCID_NUM(ARCID_APP_INFORMATION ) )
               && app::util::FileAccessor::IsFileLoad( &m_dataBuf[DATA_ID_BOOK_TYPE] )
               && app::util::FileAccessor::IsFileClose( NIJI_ARCID_NUM(ARCID_APP_INFORMATION) ) ) )  // 発行したリクエスト全部見張る
        {
          isCreated = false;
        }
      }
    }
    
    // GARC_message_info_DAT
    {
      if(m_createType == CREATE_TYPE_BOOK || m_createType == CREATE_TYPE_SHEET_BOOK)
      {
        if( !(    app::util::FileAccessor::IsFileOpen( m_msgArcId )
               && app::util::FileAccessor::IsFileLoad( &m_tempMsgDataBuf )
               && app::util::FileAccessor::IsFileClose( m_msgArcId ) ) )  // 発行したリクエスト全部見張る
        {
          isCreated = false;
        }
      }
    }

    if(isCreated)
    {
      if(m_createType == CREATE_TYPE_BOOK || m_createType == CREATE_TYPE_SHEET_BOOK)
      {
        this->createStrBufData();
      }
      m_appHeap = NULL;
      m_createState = CREATE_STATE_CREATED;
    }
  }
}

void InfoWindowDataManager::destroyDataBuf(void)
{
  for(u32 i=0; i<DATA_ID_NUM; ++i)
  {
    if(DATA_TYPE[i] == DATA_TYPE_STRBUF)
    {
      gfl2::str::StrBuf* strBuf = static_cast<gfl2::str::StrBuf*>(m_dataBuf[i]);
      GFL_SAFE_DELETE(strBuf);
      m_dataBuf[i] = NULL;
    }
    else  // (DATA_TYPE[i] == DATA_TYPE_VOID)
    {
      GflHeapSafeFreeMemory(m_dataBuf[i]);
    }
  }
}

void InfoWindowDataManager::createStrBufData(void)
{
  if(m_tempMsgDataBuf)
  {
    gfl2::str::StrBuf* strBufSlash  = GFL_NEW(m_appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_appHeap->GetDeviceHeap() );
    gfl2::str::StrBuf* strBufNumber = GFL_NEW(m_appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_appHeap->GetDeviceHeap() );
   
    gfl2::str::MsgData* tempMsgData = GFL_NEW(m_appHeap->GetDeviceHeap()->GetLowerHandle()) gfl2::str::MsgData(m_tempMsgDataBuf, m_appHeap->GetDeviceHeap()->GetLowerHandle());
    tempMsgData->GetString(msg_info_00_02, *strBufSlash);
    tempMsgData->GetString(msg_info_00_01, *strBufNumber);
    GFL_SAFE_DELETE(tempMsgData);
    
    m_dataBuf[DATA_ID_STRBUF_SLASH]  = strBufSlash;
    m_dataBuf[DATA_ID_STRBUF_NUMBER] = strBufNumber;

    GflHeapSafeFreeMemory(m_tempMsgDataBuf);
  }
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

