//==============================================================================
/**
 @file    ZukanDataManager.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のデータマネージャ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/gfl2_Fs.h>
#include <thread/include/gfl2_ThreadStatic.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_Heap.h>
#include <System/include/ArcIdGetter.h>
#include <Print/include/PrintSystem.h>
#include <System/include/PokemonVersion.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/zukan_gra.gaix>
#include <arc_index/ZukanGrpFont.gaix>

#include <arc_index/zukan_map_s.gaix>
#include <arc_index/zukan_map_m.gaix>

#include <arc_index/fieldwindow.gaix>
#include <arc_index/MenuWindow.gaix>
#include <arc_index/PokeIcon.gaix>
#include <arc_index/message.gaix>

#include <arc_index/Font.gaix>

#include <niji_conv_header/app/zukan/zukan_res2d_pack.h>

#include <niji_conv_header/app/zukan_map/zukan_map_s.h>
#include <niji_conv_header/app/zukan_map/zukan_map_m.h>

// 図鑑のインクルード
#include "ZukanDataManager.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


ZukanDataManager::ZukanDataManager(gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager* romAsyncFileManager, Mode mode)
  : m_heap(heap),
    m_romAsyncFileManager(romAsyncFileManager),
    m_mode(mode),
    m_gameLang(ZukanType::ZUKAN_LANG_JAPAN),
    m_state(STATE_STOP),
    m_stopReq(false),
    m_endReq(false),
    m_remainIsCloseAtEnd(false),
    m_currDataIdOrderIndex(0),  // 最初のデータはスキップしない必須データ。  // DATA_ID_ZUKAN_RES2D  // DATA_ID_START
    m_zukanRes2dPack(NULL),
    m_zukanDistributionPack(NULL)
{
  if(m_mode==MODE_ZUKAN_REGISTER)
  {
    m_dataIdOrderList[ 0] = DATA_ID_START;  // 最初のデータはスキップしない必須データ。  // DATA_ID_ZUKAN_RES2D
    m_dataIdOrderList[ 1] = DATA_ID_ZUKAN_MSTXT;
    // ↑最初の画面を出すにはここまで必要 
    m_dataIdOrderList[ 2] = DATA_ID_ZUKAN_GRPFONT_RES2D;
    m_dataIdOrderList[ 3] = DATA_ID_TALK_WINDOW_RES2D;
    m_dataIdOrderList[ 4] = DATA_ID_MENU_WINDOW_RES2D;
    m_dataIdOrderList[ 5] = DATA_ID_POKE_ICON_RES2D;
    m_dataIdOrderList[ 6] = DATA_ID_POKE_SPECIES_MSTXT;
    m_dataIdOrderList[ 7] = DATA_ID_POKE_FORM_MSTXT;
    m_dataIdOrderList[ 8] = DATA_ID_POKE_HEIGHT_MSTXT;
    m_dataIdOrderList[ 9] = DATA_ID_POKE_WEIGHT_MSTXT;
    m_dataIdOrderList[10] = DATA_ID_POKE_COMMENT_MSTXT;
    m_dataIdOrderList[11] = DATA_ID_ZUKAN_HYOUKA_MSTXT;
    // ↑他の画面も出すにはここまで必要
    m_dataIdOrderList[12] = DATA_ID_ZUKAN_DISTRIBUTION;
    m_dataIdOrderList[13] = DATA_ID_FONT_DUMMY_OPEN;
    m_dataIdOrderList[14] = DATA_ID_FONT_JAPAN;
    m_dataIdOrderList[15] = DATA_ID_FONT_CHINA;
    m_dataIdOrderList[16] = DATA_ID_FONT_TAIWAN;
    m_dataIdOrderList[17] = DATA_ID_FONT_DUMMY_CLOSE;
    m_dataIdOrderList[18] = DATA_ID_INITIAL_MSTXT;
    m_dataIdOrderList[19] = DATA_ID_END;
  }
  else  // (m_mode==MODE_ZUKAN)
  {
    m_dataIdOrderList[ 0] = DATA_ID_START;  // 最初のデータはスキップしない必須データ。  // DATA_ID_ZUKAN_RES2D
    m_dataIdOrderList[ 1] = DATA_ID_ZUKAN_GRPFONT_RES2D;
    m_dataIdOrderList[ 2] = DATA_ID_ZUKAN_MSTXT;
    // ↑最初の画面を出すにはここまで必要 
    m_dataIdOrderList[ 3] = DATA_ID_ZUKAN_DISTRIBUTION;
    m_dataIdOrderList[ 4] = DATA_ID_TALK_WINDOW_RES2D;
    m_dataIdOrderList[ 5] = DATA_ID_MENU_WINDOW_RES2D;
    m_dataIdOrderList[ 6] = DATA_ID_FONT_DUMMY_OPEN;
    m_dataIdOrderList[ 7] = DATA_ID_FONT_JAPAN;
    m_dataIdOrderList[ 8] = DATA_ID_FONT_CHINA;
    m_dataIdOrderList[ 9] = DATA_ID_FONT_TAIWAN;
    m_dataIdOrderList[10] = DATA_ID_FONT_DUMMY_CLOSE;
    m_dataIdOrderList[11] = DATA_ID_POKE_ICON_RES2D;      
    m_dataIdOrderList[12] = DATA_ID_POKE_SPECIES_MSTXT;
    m_dataIdOrderList[13] = DATA_ID_POKE_FORM_MSTXT;
    m_dataIdOrderList[14] = DATA_ID_POKE_HEIGHT_MSTXT;
    m_dataIdOrderList[15] = DATA_ID_POKE_WEIGHT_MSTXT;
    m_dataIdOrderList[16] = DATA_ID_POKE_COMMENT_MSTXT;
    m_dataIdOrderList[17] = DATA_ID_INITIAL_MSTXT;
    m_dataIdOrderList[18] = DATA_ID_ZUKAN_HYOUKA_MSTXT;
    // ↑他の画面も出すにはここまで必要
    m_dataIdOrderList[19] = DATA_ID_END;               

/*
    m_dataIdOrderList[ 0] = DATA_ID_START;  // 最初のデータはスキップしない必須データ。  // DATA_ID_ZUKAN_RES2D
    m_dataIdOrderList[ 1] = DATA_ID_ZUKAN_GRPFONT_RES2D;
    m_dataIdOrderList[ 2] = DATA_ID_ZUKAN_DISTRIBUTION;
    m_dataIdOrderList[ 3] = DATA_ID_TALK_WINDOW_RES2D;
    m_dataIdOrderList[ 4] = DATA_ID_MENU_WINDOW_RES2D;
    m_dataIdOrderList[ 5] = DATA_ID_FONT_DUMMY_OPEN;
    m_dataIdOrderList[ 6] = DATA_ID_FONT_JAPAN;
    m_dataIdOrderList[ 7] = DATA_ID_FONT_CHINA;
    m_dataIdOrderList[ 8] = DATA_ID_FONT_TAIWAN;
    m_dataIdOrderList[ 9] = DATA_ID_FONT_DUMMY_CLOSE;
    m_dataIdOrderList[10] = DATA_ID_POKE_ICON_RES2D;
    m_dataIdOrderList[11] = DATA_ID_ZUKAN_MSTXT;      
    m_dataIdOrderList[12] = DATA_ID_POKE_SPECIES_MSTXT;
    m_dataIdOrderList[13] = DATA_ID_POKE_FORM_MSTXT;
    m_dataIdOrderList[14] = DATA_ID_POKE_HEIGHT_MSTXT;
    m_dataIdOrderList[15] = DATA_ID_POKE_WEIGHT_MSTXT;
    m_dataIdOrderList[16] = DATA_ID_POKE_COMMENT_MSTXT;
    m_dataIdOrderList[17] = DATA_ID_INITIAL_MSTXT;
    m_dataIdOrderList[18] = DATA_ID_ZUKAN_HYOUKA_MSTXT;
    m_dataIdOrderList[19] = DATA_ID_END;                   
*/ 
  }                   

  m_gameLang = ZukanType::ConvertPokemonLang2ZukanLang(System::GetLang());

  this->initDataInfo();
  m_zukanRes2dPack        = GFL_NEW(m_heap) gfl2::fs::BinLinkerAccessor;
  m_zukanDistributionPack = GFL_NEW(m_heap) gfl2::fs::BinLinkerAccessor;
}
ZukanDataManager::~ZukanDataManager()
{
  // 図鑑、図鑑登録は通信エラーに対応しなくてよいので、不意に終了することはない。
  if( !(this->CanDestroy()) )
  {
    // 破棄してはいけない状態のとき
    GFL_ASSERT(0);  // 破棄してはいけないときに破棄しようとしている。
  }

  // ケア
  {
    this->End();
    while( !(this->CanDestroy()) )
    {
      this->Update();
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
  }

  // ここまで来たら、破棄してもいい状態になったので破棄する
  for(s32 i=0; i<DATA_ID_NUM; ++i)
  {
    GflHeapSafeFreeMemory(m_dataInfo[i].dataBuf);
  }

  GFL_SAFE_DELETE(m_zukanDistributionPack);
  GFL_SAFE_DELETE(m_zukanRes2dPack);
}

bool ZukanDataManager::CanDestroy(void) const
{
  return (m_state == STATE_END);
}

void ZukanDataManager::Update(void)
{
  if(m_state == STATE_WORK)
  {
    // 残りの処理が終了処理で行うクローズだけになっているので、終了処理に移行しているかチェックする
    if(m_remainIsCloseAtEnd && (!m_endReq))
    {
      return;
    }

    s32 totalLoopCount = 100;  // 無限ループ対策
    s32 loopCount = 1;
    while(
           totalLoopCount > 0
        && loopCount > 0
    )
    {
      // 今
      DataId  currDataId  = m_dataIdOrderList[m_currDataIdOrderIndex];
      DataId  prevDataId  = currDataId;  // currDataIdがDATA_ID_ENDのときはm_stateがSTATE_ENDになっている。つまり、ここに来たということはDATA_ID_ENDではないということ。
      DataSeq prevDataSeq = m_dataInfo[currDataId].dataSeq;
 
      switch(m_dataInfo[currDataId].dataSeq)
      {
      case DATA_SEQ_START_OPEN:
        this->updateData_StartOpen(&m_dataInfo[currDataId]);
        break;
      case DATA_SEQ_WAIT_OPEN:
        this->updateData_WaitOpen(&m_dataInfo[currDataId]);
        break;
      case DATA_SEQ_START_LOAD:
        this->updateData_StartLoad(&m_dataInfo[currDataId]);
        break;
      case DATA_SEQ_WAIT_LOAD:
        this->updateData_WaitLoad(&m_dataInfo[currDataId]);
        this->doAtLoadFinished(&m_dataInfo[currDataId]);
        break;
      case DATA_SEQ_START_CLOSE:
        this->updateData_StartClose(&m_dataInfo[currDataId]);
        this->doAtCloseNotStarted(&m_dataInfo[currDataId]);
        break;
      case DATA_SEQ_WAIT_CLOSE:
        this->updateData_WaitClose(&m_dataInfo[currDataId]);
        this->doAtCloseFinished(&m_dataInfo[currDataId]);
        break;
      case DATA_SEQ_END:
      default:
        this->updateData_End(&m_dataInfo[currDataId]);
        break;
      }

      // 次
      currDataId  = m_dataIdOrderList[m_currDataIdOrderIndex];
      if( currDataId == DATA_ID_END )
      {
        m_state = STATE_END;
        break;
      }
      else
      {
        if(
               currDataId == prevDataId
            && m_dataInfo[currDataId].dataSeq == prevDataSeq
        )
        {
          --loopCount;
        }
        else
        {
          --totalLoopCount;
        }
    
        // 残りの処理が終了処理で行うクローズだけになっているので、終了処理に移行しているかチェックする
        if(m_remainIsCloseAtEnd && (!m_endReq))
        {
          break;
        }
      }
    }
  }
}

void ZukanDataManager::Start(void)
{
  if(m_state != STATE_END && m_endReq == false)
  {
    m_stopReq = false;
    if(m_state == STATE_STOP)
    {
      m_state = STATE_WORK;
    }
  }
}
void ZukanDataManager::Stop(void)
{
  if(m_state != STATE_END && m_endReq == false)
  {
    if(m_state == STATE_WORK)
    {
      m_stopReq = true;  // m_stopReqは使っていないのでこの関数は無意味。今のところ停止には対応していない。停止に対応する場合は、停止中のEndにも対応しなければならない。
    }
  }
}
void ZukanDataManager::End(void)
{
  if(m_state != STATE_END && m_endReq == false)
  {
    m_endReq = true;
  }
}

void* ZukanDataManager::GetDataBuf(DataId dataId) const
{
  if(
         m_dataInfo[dataId].dataSeq == DATA_SEQ_START_CLOSE
      || m_dataInfo[dataId].dataSeq == DATA_SEQ_WAIT_CLOSE 
      || m_dataInfo[dataId].dataSeq == DATA_SEQ_END        
  )
  {
    return m_dataInfo[dataId].dataBuf;
  }
  else
  {
    return NULL;
  }
}

void* ZukanDataManager::GetZukanRes2dPackElemBuf(ZukanRes2dPackElemId packElemId) const
{
  static const u32 ZUKAN_RES2D_PACK_ELEM_BINLINKER_ID[ZUKAN_RES2D_PACK_ELEM_ID_NUM] =
  {
    BL_IDX_ZUKAN_RES2D_PACK_LAUNCH_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_ALOLA_TOP_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_ISLAND_TOP_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_INFO_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_REGISTER_COMMENT_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_MAP_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_SEARCH_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_REGISTER_FADE_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_BG_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_ROTOM_FADE_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_MENUWINDOW2_APPLYT,
    BL_IDX_ZUKAN_RES2D_PACK_ZUKAN_PIC_SCROLL_DATA_BIN,
    BL_IDX_ZUKAN_RES2D_PACK_ZUKAN_POKE_PIC_ACCESSOR_BIN,
  };

  void* elemBuf = NULL;
  void* dataBuf = this->GetDataBuf(DATA_ID_ZUKAN_RES2D);
  if(dataBuf)
  {
    elemBuf = m_zukanRes2dPack->GetData(ZUKAN_RES2D_PACK_ELEM_BINLINKER_ID[packElemId]);
  }
  return elemBuf;
}

void* ZukanDataManager::GetZukanDistributionPackElemBuf(ZukanDistributionPackElemId packElemId) const
{
  static const u32 ZUKAN_DISTRIBUTION_PACK_ELEM_BINLINKER_ID[ZUKAN_DISTRIBUTION_PACK_ELEM_ID_NUM] =
  {
    0,  // BL_IDX_ZUKAN_MAP_S_ZUKAN_DISTRIBUTION_POKE_DATA_S_BIN      // BL_IDX_ZUKAN_MAP_M_ZUKAN_DISTRIBUTION_POKE_DATA_M_BIN
    1,  // BL_IDX_ZUKAN_MAP_S_ZUKAN_DISTRIBUTION_POSITION_DATA_S_BIN  // BL_IDX_ZUKAN_MAP_M_ZUKAN_DISTRIBUTION_POSITION_DATA_M_BIN
    2,  // BL_IDX_ZUKAN_MAP_S_ZUKAN_DISTRIBUTION_ZONE_DATA_BIN        // BL_IDX_ZUKAN_MAP_M_ZUKAN_DISTRIBUTION_ZONE_DATA_BIN
  };

  void* elemBuf = NULL;
  void* dataBuf = this->GetDataBuf(DATA_ID_ZUKAN_DISTRIBUTION);
  if(dataBuf)
  {
    elemBuf = m_zukanDistributionPack->GetData(ZUKAN_DISTRIBUTION_PACK_ELEM_BINLINKER_ID[packElemId]);
  }
  return elemBuf;
}

const gfl2::fs::ArcFile*  ZukanDataManager::GetArcFile(DataId dataId) const
{
  const gfl2::fs::ArcFile* arcFile = NULL;
  void* dataBuf = this->GetDataBuf(dataId);
  if(dataBuf)
  {
    arcFile = m_dataInfo[dataId].arcFile;
  }
  return arcFile;
}

void ZukanDataManager::updateData_StartOpen(DataInfo* dataInfo)
{
  if( dataInfo->dataType != DATA_TYPE_FONT_LOAD && dataInfo->dataType != DATA_TYPE_FONT_CLOSE )
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId       = dataInfo->arcId;
    req.heapForFile = (dataInfo->closeSoon)?(m_heap->GetLowerHandle()):(m_heap);  // すぐ閉じるときは後方確保で一時的に使う : すぐ閉じないときは前方確保で最後まで保持する
    req.heapForReq  = m_heap->GetLowerHandle();  // @fix GFCHECK[8400] 一時ヒープの使い方が間違っていたのを修正。
    m_romAsyncFileManager->AddArcFileOpenReq(req);
  }
  dataInfo->dataSeq = DATA_SEQ_WAIT_OPEN;
}
void ZukanDataManager::updateData_WaitOpen(DataInfo* dataInfo)
{
  if( m_romAsyncFileManager->IsArcFileOpenFinished(dataInfo->arcId) )
  {
    if(dataInfo->dataType == DATA_TYPE_GARC_BUF)
    {
      dataInfo->arcFile = m_romAsyncFileManager->GetArcFile(dataInfo->arcId);
    }
    dataInfo->dataSeq = DATA_SEQ_START_LOAD;
  }
}
void ZukanDataManager::updateData_StartLoad(DataInfo* dataInfo)
{
  if( dataInfo->dataType != DATA_TYPE_FONT_OPEN && dataInfo->dataType != DATA_TYPE_FONT_CLOSE )
  {
    if(dataInfo->dataType == DATA_TYPE_GARC_BUF)
    {
      size_t startOffsetFromArchiveTop = 0;
      dataInfo->arcFile->GetOffsetFromArchiveTop(&startOffsetFromArchiveTop, dataInfo->datId);
 
      size_t endOffsetFromArchiveTop = 0;
      dataInfo->arcFile->GetOffsetFromArchiveTop(&endOffsetFromArchiveTop, dataInfo->endDatId);
      size_t endDataSize = 0;
      dataInfo->arcFile->GetDataSize(&endDataSize, dataInfo->endDatId, m_heap->GetLowerHandle());  // 一気読みしているので、ArcSrcを使いたくない。fs側で防ぐフラグがないのでこのままにしておく。
 
      size_t readSize = endOffsetFromArchiveTop + endDataSize - startOffsetFromArchiveTop;
      dataInfo->dataBuf = GflHeapAllocMemoryAlign(m_heap, readSize, dataInfo->align);
 
      gfl2::fs::AsyncFileManager::ArcFileLoadDataPieceBufReq  req;
      req.arcId             = dataInfo->arcId;
      req.datId             = dataInfo->datId;
      req.readSize          = readSize;
      req.pBuf              = dataInfo->dataBuf;
      req.bufSize           = readSize;
      req.heapForReq        = m_heap->GetLowerHandle();
      m_romAsyncFileManager->AddArcFileLoadDataPieceBufReq(req);
    }
    else
    {
      if(dataInfo->datId != gfl2::fs::ArcFile::ARCDATID_NULL)
      {
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
        req.arcId             = dataInfo->arcId;
        req.datId             = dataInfo->datId;
        req.ppBuf             = &(dataInfo->dataBuf);
        req.heapForBuf        = m_heap;
        req.align             = dataInfo->align;
        req.heapForReq        = m_heap->GetLowerHandle();
        req.heapForCompressed = (dataInfo->compressed)?(m_heap->GetLowerHandle()):(NULL);
        m_romAsyncFileManager->AddArcFileLoadDataReq(req);
      }
    }
  }
  dataInfo->dataSeq = DATA_SEQ_WAIT_LOAD;
}
void ZukanDataManager::updateData_WaitLoad(DataInfo* dataInfo)
{
  if(dataInfo->dataType == DATA_TYPE_GARC_BUF)
  {
    if( m_romAsyncFileManager->IsArcFileLoadDataPieceBufFinished(dataInfo->dataBuf) )
    {
      dataInfo->dataSeq = DATA_SEQ_START_CLOSE;
    }
  }
  else
  {
    if( m_romAsyncFileManager->IsArcFileLoadDataFinished(&(dataInfo->dataBuf)) )
    {
      dataInfo->dataSeq = DATA_SEQ_START_CLOSE;
    }
  }
}
void ZukanDataManager::updateData_StartClose(DataInfo* dataInfo)
{
  if( dataInfo->dataType != DATA_TYPE_FONT_OPEN && dataInfo->dataType != DATA_TYPE_FONT_LOAD )
  {
    if(dataInfo->closeSoon || m_endReq)
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId      = dataInfo->arcId;
      req.heapForReq = m_heap->GetLowerHandle();
      m_romAsyncFileManager->AddArcFileCloseReq(req);
      dataInfo->dataSeq = DATA_SEQ_WAIT_CLOSE;
    }
  }
  else
  {
    dataInfo->dataSeq = DATA_SEQ_WAIT_CLOSE;
  }
}
void ZukanDataManager::updateData_WaitClose(DataInfo* dataInfo)
{
  if(dataInfo->closeSoon || m_endReq)
  {
    if( m_romAsyncFileManager->IsArcFileCloseFinished(dataInfo->arcId) )
    {
      dataInfo->dataSeq = DATA_SEQ_END;
    }
  }
}
void ZukanDataManager::updateData_End(DataInfo* dataInfo)
{
  this->doAtEnd(dataInfo);
}

void ZukanDataManager::initDataInfo(void)
{
  u8 cassetteVersion = System::GetVersion();
  
  for(s32 i=0; i<DATA_ID_NUM; ++i)
  {
    m_dataInfo[i] = DataInfo();
  }

  DataId    dataId   = DATA_ID_END;
  DataInfo* dataInfo = NULL;

  dataId = DATA_ID_ZUKAN_RES2D;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_APPLYT_PACK;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_ZUKAN_GRA);
    dataInfo->datId      = GARC_zukan_gra_zukan_res2d_pack_pack_COMP;
    dataInfo->align      = 128;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = true;
  }

  dataId = DATA_ID_ZUKAN_GRPFONT_RES2D;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_APPLYT;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_ZUKAN_GRPFONT);
    dataInfo->datId      = GARC_ZukanGrpFont_ZukanGrpFont_applyt_COMP;
    dataInfo->align      = 128;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = true;
  }

  dataId = DATA_ID_ZUKAN_DISTRIBUTION;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_APPLYT_PACK;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_ZUKAN_MAP);
    dataInfo->datId      = 0;  // GARC_zukan_map_s_zukan_map_s_PACK  // GARC_zukan_map_m_zukan_map_m_PACK
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  dataId = DATA_ID_TALK_WINDOW_RES2D;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_APPLYT;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_FIELDWINDOW);
    dataInfo->datId      = GARC_fieldwindow_fieldwindow_applyt_COMP;
    dataInfo->align      = 128;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = true;
  }

  dataId = DATA_ID_MENU_WINDOW_RES2D;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_APPLYT;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_MENU_WINDOW);
    dataInfo->datId      = GARC_MenuWindow_MenuWindow_applyt_COMP;
    dataInfo->align      = 128;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = true;
  }

  dataId = DATA_ID_FONT_DUMMY_OPEN;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_FONT_OPEN;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_FONT);
    dataInfo->closeSoon  = true;
    dataInfo->skip       = (m_mode==MODE_ZUKAN_REGISTER);
  }
  dataId = DATA_ID_FONT_JAPAN;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_FONT_LOAD;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_FONT);
    if(!this->isFontAlreadyRead(ZukanType::ZUKAN_LANG_JAPAN))
    {
      dataInfo->datId      = GARC_Font_cbf_std_kujira_BFFNT;
    }
    dataInfo->align      = nw::font::GlyphDataAlignment;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
    dataInfo->skip       = (m_mode==MODE_ZUKAN_REGISTER);
  }
  dataId = DATA_ID_FONT_CHINA;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_FONT_LOAD;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_FONT);
    if(!this->isFontAlreadyRead(ZukanType::ZUKAN_LANG_CHINA))
    {
      dataInfo->datId      = GARC_Font_cbf_ch_BFFNT;
    }
    dataInfo->align      = nw::font::GlyphDataAlignment;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
    dataInfo->skip       = (m_mode==MODE_ZUKAN_REGISTER);
  }
  dataId = DATA_ID_FONT_TAIWAN;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_FONT_LOAD;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_FONT);
    if(!this->isFontAlreadyRead(ZukanType::ZUKAN_LANG_TAIWAN))
    {
      dataInfo->datId      = GARC_Font_cbf_tw_BFFNT;
    }
    dataInfo->align      = nw::font::GlyphDataAlignment;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
    dataInfo->skip       = (m_mode==MODE_ZUKAN_REGISTER);
  }
  dataId = DATA_ID_FONT_DUMMY_CLOSE;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_FONT_CLOSE;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_FONT);
    dataInfo->closeSoon  = true;
    dataInfo->skip       = (m_mode==MODE_ZUKAN_REGISTER);
  }

  dataId = DATA_ID_POKE_ICON_RES2D;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_GARC_BUF;
    dataInfo->arcId      = NIJI_ARCID_NUM(ARCID_POKEICON);
    dataInfo->datId      = 0;
    dataInfo->endDatId   = GARC_PokeIcon_DATA_NUM -1;
    dataInfo->align      = 128;
    dataInfo->closeSoon  = false;
    dataInfo->compressed = false;
  }
  
  dataId = DATA_ID_ZUKAN_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_zukan_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  /*
  dataId = DATA_ID_POKE_NAME_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_monsname_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }
  */

  dataId = DATA_ID_POKE_SPECIES_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_zkn_type_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  dataId = DATA_ID_POKE_FORM_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_zkn_form_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  dataId = DATA_ID_POKE_HEIGHT_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_zkn_height_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  dataId = DATA_ID_POKE_WEIGHT_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_zkn_weight_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  dataId = DATA_ID_POKE_COMMENT_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    if(cassetteVersion == VERSION_SUN2)
    {
      dataInfo->datId    = GARC_message_zukan_comment_A_DAT;
    }
    else  // if(cassetteVersion == VERSION_MOON)
    {
      dataInfo->datId    = GARC_message_zukan_comment_B_DAT;
    }
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  dataId = DATA_ID_INITIAL_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_initial_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }

  dataId = DATA_ID_ZUKAN_HYOUKA_MSTXT;
  {
    dataInfo = &m_dataInfo[dataId];
    dataInfo->dataId     = dataId;
    dataInfo->dataIdOrderIndex = this->getDataIdOrderIndex(dataId);
    dataInfo->dataType   = DATA_TYPE_MSTXT;
    dataInfo->arcId      = print::GetMessageArcId();  // ゲーム開始時に選んだ言語
    dataInfo->datId      = GARC_message_zukan_hyouka_DAT;
    dataInfo->align      = 4;
    dataInfo->closeSoon  = true;
    dataInfo->compressed = false;
  }
}

u32  ZukanDataManager::getDataIdOrderIndex(DataId dataId) const
{
  u32 dataIdOrderIndex = 0;
  for(u32 i=0; i<DATA_ID_NUM; ++i)
  {
    if(m_dataIdOrderList[i] == dataId)
    {
      dataIdOrderIndex = i;
      break;
    }
  }
  return dataIdOrderIndex;
}

void ZukanDataManager::doAtLoadFinished(DataInfo* dataInfo)
{
  // ロードが完了したら
  if(dataInfo->dataSeq == DATA_SEQ_START_CLOSE)
  {
    // ロード完了時の処理
    if(dataInfo->dataId == DATA_ID_ZUKAN_RES2D)
    {
      m_zukanRes2dPack->Initialize(dataInfo->dataBuf);
    }
    else if(dataInfo->dataId == DATA_ID_ZUKAN_DISTRIBUTION)
    {
      m_zukanDistributionPack->Initialize(dataInfo->dataBuf);
    }
    
    // 次へ
    s32 loopCount = 100;  // 無限ループ対策
    u32    nextDataIdOrderIndex = dataInfo->dataIdOrderIndex;
    DataId nextDataId           = m_dataIdOrderList[nextDataIdOrderIndex];
    while(loopCount > 0)
    {
      nextDataIdOrderIndex = nextDataIdOrderIndex +1;
      GFL_ASSERT(nextDataIdOrderIndex < DATA_ID_NUM+1);  // 開発中に気付かせるASSERT
      nextDataId           = m_dataIdOrderList[nextDataIdOrderIndex];
      if(nextDataId == DATA_ID_END)
      {
        // 最初に戻ってファイルを閉じる
        m_currDataIdOrderIndex = 0;  // 最初のデータはスキップしない必須データ。  // DATA_ID_ZUKAN_RES2D  // DATA_ID_START
        break;
      }
      else
      {
        // 次のファイルを開く
        if(!m_dataInfo[nextDataId].skip)
        {
          m_currDataIdOrderIndex = nextDataIdOrderIndex;
          break;
        }
      }
      --loopCount;
    }
  }
}
void ZukanDataManager::doAtCloseNotStarted(DataInfo* dataInfo)
{
  // クローズが始まらなかったら
  if(dataInfo->dataSeq == DATA_SEQ_START_CLOSE)
  {
    this->doAtEnd(dataInfo);
  }
}
void ZukanDataManager::doAtCloseFinished(DataInfo* dataInfo)
{
  // クローズが完了したら
  if(dataInfo->dataSeq == DATA_SEQ_END)
  {
    this->doAtEnd(dataInfo);
  }
}
void ZukanDataManager::doAtEnd(DataInfo* dataInfo)
{
  // 次へ
  s32 loopCount = 100;  // 無限ループ対策
  u32    nextDataIdOrderIndex = dataInfo->dataIdOrderIndex;
  DataId nextDataId           = m_dataIdOrderList[nextDataIdOrderIndex];
  while(loopCount > 0)
  {
    nextDataIdOrderIndex = nextDataIdOrderIndex +1;
    GFL_ASSERT(nextDataIdOrderIndex < DATA_ID_NUM+1);  // 開発中に気付かせるASSERT
    nextDataId           = m_dataIdOrderList[nextDataIdOrderIndex];
    if(nextDataId == DATA_ID_END)
    {
      m_remainIsCloseAtEnd = true;  // 残りの処理が終了処理で行うクローズだけになった
      
      // 全てのファイルを閉じていたら終了
      bool dataSeqIsNotEnd = false;
      for(s32 i=0; i<DATA_ID_NUM; ++i)
      {
        if(m_dataInfo[i].skip == false && m_dataInfo[i].dataSeq != DATA_SEQ_END)
        {
          dataSeqIsNotEnd = true;
          break;
        }
      }
      if(dataSeqIsNotEnd)
      {
        // 最初に戻ってファイルを閉じる
        m_currDataIdOrderIndex = 0;  // 最初のデータはスキップしない必須データ。  // DATA_ID_ZUKAN_RES2D  // DATA_ID_START
      }
      else
      {
        // 終了
        m_currDataIdOrderIndex = nextDataIdOrderIndex;
      }
      break;
    }
    else
    {
      // 次のファイルを閉じる
      if(!m_dataInfo[nextDataId].skip)
      {
        m_currDataIdOrderIndex = nextDataIdOrderIndex;
        break;
      }
    }
    --loopCount;
  }
}

bool ZukanDataManager::isFontAlreadyRead(ZukanType::ZukanLang zukanLang) const
{
  bool ret = false;
  switch(zukanLang)
  {
  case ZukanType::ZUKAN_LANG_CHINA:
    {
      ret = (m_gameLang == ZukanType::ZUKAN_LANG_CHINA);
      break;
    }
  case ZukanType::ZUKAN_LANG_TAIWAN:
    {
      ret = (m_gameLang == ZukanType::ZUKAN_LANG_TAIWAN);
      break;
    }
  default:
    {
      ret = (
           m_gameLang == ZukanType::ZUKAN_LANG_JAPAN  
        || m_gameLang == ZukanType::ZUKAN_LANG_ENGLISH
        || m_gameLang == ZukanType::ZUKAN_LANG_FRANCE 
        || m_gameLang == ZukanType::ZUKAN_LANG_ITALY  
        || m_gameLang == ZukanType::ZUKAN_LANG_GERMANY
        || m_gameLang == ZukanType::ZUKAN_LANG_SPAIN  
        || m_gameLang == ZukanType::ZUKAN_LANG_KOREA  
      );
      break;
    }
  }
  return ret;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

