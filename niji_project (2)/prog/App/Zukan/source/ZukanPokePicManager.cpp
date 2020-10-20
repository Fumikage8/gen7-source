//==============================================================================
/**
 @file    ZukanPokePicManager.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の絵巻のポケモン画像のマネージャ
 @note    zukan_poke_pic_accessor_maker.rbが生成したzukan_poke_pic_accessor.binの中身にアクセスする。
          画像を読み込む。
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_Assert.h>
#include <thread/include/gfl2_ThreadStatic.h>

// nijiのインクルード
#include <System/include/ArcIdGetter.h>
#include <GameSys/include/GameManager.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/zukan_poke_pic.gaix>

// 図鑑のインクルード
#include "ZukanFormOrder.h"
#include "ZukanPokePicManager.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


ZukanPokePicManager::ZukanPokePicManager(gfl2::heap::HeapBase* parentDevHeap)
  : m_destroyReq(false),
    m_managerState(ManagerState::NONE),
    m_pokePicAccessorBin(NULL),
    m_arcFileDevHeap(NULL)
{
  // メモリ生成
  m_arcFileDevHeap = GFL_CREATE_LOCAL_HEAP(parentDevHeap, ZUKAN_POKE_PIC_ARC_FILE_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);

  for(u32 i=0; i<LOADING_PIC_NUM_MAX; ++i)
  {
    m_loadingPicInfo[i].compDevHeap = GFL_CREATE_LOCAL_HEAP(parentDevHeap, PIC_COMP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  }
  for(u32 i=0; i<PIC_NUM_MAX; ++i)
  {
    m_picData[i].devHeap = GFL_CREATE_LOCAL_HEAP(parentDevHeap, PIC_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  }
}

ZukanPokePicManager::~ZukanPokePicManager()
{
  GFL_ASSERT(this->DestroyingManager());  // 開発中に気付かせるためのASSERT

  u32 loopCount = 0;
  while(!this->DestroyingManager())
  {
    gfl2::thread::ThreadStatic::CurrentSleep(1);
    ++loopCount;
    if(loopCount > 10000) break;  // 無限ループ対策
  }

  // メモリ破棄
  for(u32 i=0; i<PIC_NUM_MAX; ++i)
  {
    this->deleteTexDestroyBuf(&m_picData[i]);
    GFL_DELETE_HEAP(m_picData[i].devHeap);
    m_picData[i].devHeap = NULL;
  }
  for(u32 i=0; i<LOADING_PIC_NUM_MAX; ++i)
  { 
    GFL_DELETE_HEAP(m_loadingPicInfo[i].compDevHeap);
    m_loadingPicInfo[i].compDevHeap = NULL;
  }

  GFL_DELETE_HEAP(m_arcFileDevHeap);
  m_arcFileDevHeap = NULL;
}

bool ZukanPokePicManager::CreatingManager(void)
{
  // ファイルオープン
  bool ret = false;
  switch(m_managerState)
  {
  case ManagerState::NONE:
    {
      app::util::FileAccessor::FileOpen( NIJI_ARCID_NUM(ARCID_ZUKAN_POKE_PIC), m_arcFileDevHeap, false );
      m_managerState = ManagerState::OPENING_ARC_FILE;
      break;
    }
  case ManagerState::OPENING_ARC_FILE:
    {
      if( app::util::FileAccessor::IsFileOpen( NIJI_ARCID_NUM(ARCID_ZUKAN_POKE_PIC) ) )
      {
        m_managerState = ManagerState::WORKING;
      }
      break;
    }
  case ManagerState::WORKING:
    {
      ret = true;
      break;
    }
  case ManagerState::CLOSING_ARC_FILE:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。破棄中に再び生成しようとしている。
      break;
    }
  }
  return ret;
}

bool ZukanPokePicManager::DestroyingManager(void)
{
  // ファイルクローズ
  bool ret = false;
  switch(m_managerState)
  {
  case ManagerState::WORKING:
    {
      m_destroyReq = true;
      bool canClose = true;
      for(u32 i=0; i<PIC_NUM_MAX; ++i)
      {
        if( m_picData[i].state == PicDataState::LOADING || m_picData[i].state == PicDataState::LOADING_THEN_UNLOAD )
        {
          canClose = false;
          break;
        }
      }
      if(canClose)
      {
        app::util::FileAccessor::FileClose( NIJI_ARCID_NUM(ARCID_ZUKAN_POKE_PIC), m_arcFileDevHeap );
        m_managerState = ManagerState::CLOSING_ARC_FILE;
      }
      break;
    }
  case ManagerState::CLOSING_ARC_FILE:
    {
      if( app::util::FileAccessor::IsFileClose( NIJI_ARCID_NUM(ARCID_ZUKAN_POKE_PIC) ) )
      {
        m_managerState = ManagerState::NONE;
      }
      break;
    }
  case ManagerState::NONE:
    {
      m_destroyReq = false;
      ret = true;
      break;
    }
  case ManagerState::OPENING_ARC_FILE:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。生成中に破棄しようとしている。
      break;
    }
  }
  return ret;
}

void ZukanPokePicManager::Update(void)
{
  for(u32 i=0; i<PIC_NUM_MAX; ++i)
  {
    this->updatePicData(&m_picData[i]);
  }
}

void ZukanPokePicManager::SetupPicPane(gfl2::lyt::LytPicture* pane, const ZukanContents::PokeData* pokeData, u32 figureDataIndex, const bool isLarge)
{
  gfl2::fs::ArcFile::ARCDATID arcDatId = this->getArcDatId(pokeData, figureDataIndex);
  if(arcDatId == gfl2::fs::ArcFile::ARCDATID_NULL) return;

  // PicDataが既にあるか確認する
  bool finishSetup = false;
  for(u32 i=0; i<PIC_NUM_MAX; ++i)
  {
    if(arcDatId == m_picData[i].arcDatId)
    {
      // PicDataが既にある！
      if(m_picData[i].picPaneNum < PIC_PANE_NUM_MAX)
      {
        u32 picPaneIndex = m_picData[i].picPaneNum;

        // D要望対応：光ドラゴンのテクスチャを横長にしたい為、テクスチャサイズにペインも合わせる
        {
          nw::lyt::Size size( DEFAULT_WIDTH, DEFAULT_HEIGHT );
          if( isLarge == true )
          {
            size.width = LARGE_WIDTH;
            size.height = LARGE_HEIGHT;
          }
          pane->SetSize( size );
        }

        m_picData[i].picPaneInfo[picPaneIndex].Set(pane);
         ++m_picData[i].picPaneNum;
        if(m_picData[i].state == PicDataState::EXIST)
        {
          // ポケモン画像が既にあるので貼る
          this->setTexToPicPane(&m_picData[i], picPaneIndex);
        }
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。同じ画像を貼るピクチャペインの個数が準備しておいた個数を越えた。
      }
      finishSetup = true;
      break;
    }
  }

  // PicDataがないときまだ使っていないところ使う
  if(finishSetup == false)
  {
    PicData* usePicData = NULL;
    for(u32 i=0; i<PIC_NUM_MAX; ++i)
    {
      if(m_picData[i].arcDatId == gfl2::fs::ArcFile::ARCDATID_NULL)
      {
        // まだ使っていないところだ！
        usePicData = &m_picData[i];
        break;
      }
    }

    // まだ使っていないところがないとき一番古いPicDataを削除する
    if(usePicData == NULL)
    {
      PicData* oldPicData = NULL;
      for(u32 i=0; i<PIC_NUM_MAX; ++i)
      {
        if(oldPicData == NULL)
        {
          if(m_picData[i].picPaneNum == 0)
          {
            // 今は使われていないところだ！
            oldPicData = &m_picData[i];
          }
        }
        else
        {
          if(m_picData[i].picPaneNum == 0)
          {
            // 今は使われていないところだ！
            if(oldPicData->freeCount < m_picData[i].freeCount)
            {
              // より古いところだ！
              oldPicData = &m_picData[i];
            }
          }
        }
      }
      if(oldPicData != NULL)
      {
        // 今は使われていないところで、一番古いところであるPicDataを使うために、他のポケモン画像を消しにいく
        if(oldPicData->state == PicDataState::EXIST)
        {
          // 他のポケモン画像が既にあるなら破棄する
          this->deleteTexDestroyBuf(oldPicData);
          oldPicData->state = PicDataState::NONE;
        }
        else if(oldPicData->state == PicDataState::LOADING)
        {
          // 他のポケモン画像を読み込み中なら、読み込み後に破棄する設定に変えておく
          oldPicData->state = PicDataState::LOADING_THEN_UNLOAD;
        }
        
        usePicData = oldPicData;
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。使っていないPicDataがない。
      }
    }

    if(usePicData)
    {
      // PicDataを設定する
      usePicData->pokeData = pokeData;
      usePicData->figureDataIndex = figureDataIndex;
      usePicData->arcDatId = arcDatId;

      // D要望対応：光ドラゴンのテクスチャを横長にしたい為、テクスチャサイズにペインも合わせる
      {
        nw::lyt::Size size( DEFAULT_WIDTH, DEFAULT_HEIGHT );
        if( isLarge == true )
        {
          size.width = LARGE_WIDTH;
          size.height = LARGE_HEIGHT;
        }
        pane->SetSize( size );
      }
      usePicData->picPaneInfo[0].Set(pane);
      usePicData->picPaneNum = 1;
      usePicData->freeCount = 0;
    }
  }
}
void ZukanPokePicManager::UnsetupPicPane(gfl2::lyt::LytPicture* pane, const ZukanContents::PokeData* pokeData, u32 figureDataIndex)
{
  gfl2::fs::ArcFile::ARCDATID arcDatId = this->getArcDatId(pokeData, figureDataIndex);
  if(arcDatId == gfl2::fs::ArcFile::ARCDATID_NULL) return;

  for(u32 i=0; i<PIC_NUM_MAX; ++i)
  {
    if(arcDatId == m_picData[i].arcDatId)
    {
      for(u32 j=0; j<m_picData[i].picPaneNum; ++j)
      {
        if(m_picData[i].picPaneInfo[j].pane == pane)
        {
          this->resetTexToPicPane(&m_picData[i], j);
          m_picData[i].picPaneInfo[j].Set(NULL);
        }
      }
      // 空いているところを詰める
      u32 num = 0;
      for(u32 j=0; j<m_picData[i].picPaneNum; ++j)
      {
        if(m_picData[i].picPaneInfo[j].pane != NULL)
        {
          m_picData[i].picPaneInfo[num].Set(m_picData[i].picPaneInfo[j].pane);
          ++num;
        }
      }
      m_picData[i].picPaneNum = num;
      // 空いたところにはNULLを入れておく
      for(u32 j=m_picData[i].picPaneNum; j<PIC_PANE_NUM_MAX; ++j)
      {
        m_picData[i].picPaneInfo[j].Set(NULL);
      }
      break;
    }
  }
}
bool ZukanPokePicManager::IsPokePicReady(const ZukanContents::PokeData* pokeData, u32 figureDataIndex) const
{
  gfl2::fs::ArcFile::ARCDATID arcDatId = this->getArcDatId(pokeData, figureDataIndex);
  if(arcDatId == gfl2::fs::ArcFile::ARCDATID_NULL) return false;

  for(u32 i=0; i<PIC_NUM_MAX; ++i)
  {
    if(arcDatId == m_picData[i].arcDatId)
    {
      if(m_picData[i].state == PicDataState::EXIST) return true;
    }
  }

  return false;
}

gfl2::fs::ArcFile::ARCDATID ZukanPokePicManager::getArcDatId(const ZukanContents::PokeData* pokeData, u32 figureDataIndex) const
{
  gfl2::fs::ArcFile::ARCDATID arcDatId = 0;  // 見付からないときは0番を出す

  const u16* pokePicAccessorU16 = static_cast<const u16*>(m_pokePicAccessorBin);

  s32 monsNoEnd = static_cast<s32>(pokePicAccessorU16[1]);

  if(pokeData->monsNo <= monsNoEnd)
  {
    u32 picNum      = static_cast<u32>(pokePicAccessorU16[2+pokeData->monsNo*2]);
    u32 arcDatIdTop = static_cast<u32>(pokePicAccessorU16[2+pokeData->monsNo*2+1]);

    u32 picIndex = this->getPicIndexOfFigureData(pokeData, figureDataIndex);
    if(picIndex < picNum)
    {
      arcDatId = arcDatIdTop+ picIndex;
    }
    else
    {
      if(picNum > 0)
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。引数で指定した容姿に該当する画像がない。
        arcDatId = arcDatIdTop;  // 先頭の画像にしておく。
      }
      else
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。画像のないポケモン。
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。アクセッサバイナリに書かれているポケモン数と一致していない。
  }

  return arcDatId;
}
u32 ZukanPokePicManager::getPicIndexOfFigureData(const ZukanContents::PokeData* pokeData, u32 figureDataIndex) const
{
  // 「レアなし」はない前提でデータをつくり、プログラムを書いています。
  
  // figureDataIndexはZukanFormOrder順、picIndexはpml::FormNo順なので、pml::FormNo順のリストを用意する
  // figureDataIndexとpicIndexはオスとメスの順番が逆なので入れ替える
  u32 picIndex = 0;
  if(figureDataIndex < pokeData->figureDataNum)
  {
    // 容姿データのインデックスをフォルム番号順に並べる
    u8 figureDataIndexArrayByFormNoOrder[ZukanContents::PokeData::FIGURE_DATA_NUM_MAX] = { 0 };

    if(pokeData->formOrderType != ZukanFormOrder::FORM_ORDER_TYPE_FORM_NO) 
    {
      // フォルム番号順ではなかったので、フォルム番号順にする
      u32 l_formNoOrder = 0;
      for(u32 l_formNo=0; l_formNo<pokeData->formNum; ++l_formNo)
      {
        for(u32 l_figureDataIndex=0; l_figureDataIndex<pokeData->figureDataNum; ++l_figureDataIndex)
        {
          if( pokeData->figureData[l_figureDataIndex].formNo == l_formNo )
          {
            figureDataIndexArrayByFormNoOrder[l_figureDataIndex] = l_formNoOrder;  // 性別、色違いは順番通りに並んでいる
            ++l_formNoOrder;
          }
        }
      }
    }
    else
    {
      // 元からフォルム番号順
      for(u32 l_figureDataIndex=0; l_figureDataIndex<pokeData->figureDataNum; ++l_figureDataIndex)
      {
        figureDataIndexArrayByFormNoOrder[l_figureDataIndex] = l_figureDataIndex;
      }
    }
    
    picIndex = figureDataIndexArrayByFormNoOrder[figureDataIndex];
    if(pokeData->figureData[figureDataIndex].figureGenderExistType == ZukanContents::PokeData::FIGURE_GENDER_EXIST_TYPE_MALE_FEMALE_DIFFERENT)
    {
      if(pokeData->figureData[figureDataIndex].figureGender == ZukanContents::PokeData::FIGURE_GENDER_MALE)
      {
        if(
               figureDataIndex+2 < pokeData->figureDataNum
            && pokeData->figureData[figureDataIndex].formNo == pokeData->figureData[figureDataIndex+2].formNo
            && pokeData->figureData[figureDataIndex+2].figureGender == ZukanContents::PokeData::FIGURE_GENDER_FEMALE  // 同じフォルム番号にメスが存在する
        )
        {
          picIndex += 2;
        }
      }
      else if(pokeData->figureData[figureDataIndex].figureGender == ZukanContents::PokeData::FIGURE_GENDER_FEMALE)
      {
        if(
               figureDataIndex >= 2
            && figureDataIndex-2 < pokeData->figureDataNum
            && pokeData->figureData[figureDataIndex].formNo == pokeData->figureData[figureDataIndex-2].formNo
            && pokeData->figureData[figureDataIndex-2].figureGender == ZukanContents::PokeData::FIGURE_GENDER_MALE  // 同じフォルム番号にオスが存在する
        )
        {
          picIndex -= 2;
        }
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。引数で指定した容姿の番号がオーバーしている。
  }
  return picIndex;
}

void ZukanPokePicManager::updatePicData(PicData* picData)
{
  if(m_managerState == ManagerState::WORKING)
  {
    switch(picData->state)
    {
    case PicDataState::NONE:
      {
        if(!m_destroyReq)
        {
          if(picData->arcDatId != gfl2::fs::ArcFile::ARCDATID_NULL)
          {
            // 読み込みリクエストがある！
            LoadingPicInfo* blankLoadingPicInfo = this->getBlankLoadingPicInfo();
            if(blankLoadingPicInfo != NULL)
            {
              // 読み込み枠に空きがあるので、読み込みを開始する！
              gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
              req.arcId             = NIJI_ARCID_NUM(ARCID_ZUKAN_POKE_PIC);
              req.datId             = picData->arcDatId;
              req.ppBuf             = &picData->buf;
              req.pRealReadSize     = &picData->realReadSize;
              req.heapForBuf        = picData->devHeap;
              req.align             = 128;
              req.heapForReq        = blankLoadingPicInfo->compDevHeap;
              req.heapForCompressed = blankLoadingPicInfo->compDevHeap;

              gfl2::fs::AsyncFileManager* asyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
              asyncFileManager->AddArcFileLoadDataReq(req);

              blankLoadingPicInfo->pLoadingPicData = picData;
              picData->state = PicDataState::LOADING;
            }
          }
        }
        break;
      }
    case PicDataState::LOADING:
      {
        if( app::util::FileAccessor::IsFileLoad(&picData->buf) )
        {
          // 読み込みが完了したので、読み込み枠を空ける！
          this->releaseLoadingPicInfo(picData);
          // テクスチャロード
          this->loadTex(picData);
          for(u32 i=0; i<picData->picPaneNum; ++i)
          {
            // ポケモン画像を貼る
            this->setTexToPicPane(picData, i);
          }
          picData->state = PicDataState::EXIST;
        }
        break;
      }
    case PicDataState::LOADING_THEN_UNLOAD:
      {
        if( app::util::FileAccessor::IsFileLoad(&picData->buf) )
        {
          // 読み込みが完了したので、読み込み枠を空ける！
          this->releaseLoadingPicInfo(picData);
          // 続けて他のポケモン画像を読み込むために破棄する
          this->deleteTexDestroyBuf(picData);
          picData->state = PicDataState::NONE;
        }
        break;
      }
    case PicDataState::EXIST:
      {
        break;
      }
    }

    // 使っていないPicDataならカウントを増やす
    if(picData->picPaneNum == 0)
    {
      ++picData->freeCount;
      if(picData->freeCount > FREE_COUNT_MAX) picData->freeCount = FREE_COUNT_MAX;
    }
  }
}

void ZukanPokePicManager::loadTex(PicData* picData)
{
  if(!picData->texLoaded)
  {
    nw::lyt::LoadTexture(&(picData->texInfo), picData->buf, picData->realReadSize);
    picData->texLoaded = true;
  }
}
void ZukanPokePicManager::deleteTexDestroyBuf(PicData* picData)
{
  if(picData->texLoaded)
  {
#if defined(GF_PLATFORM_CTR)
    nw::lyt::DeleteTexture(picData->texInfo);
#endif  // Win32版にnw::lyt::DeleteTextureはない
    picData->texLoaded = false;
  }
  GflHeapSafeFreeMemory(picData->buf);
  picData->realReadSize = 0;
}

void ZukanPokePicManager::setTexToPicPane(PicData* picData, u32 picPaneIndex)
{
  // 必ずポケモン画像の読み込みが終わってから呼んで下さい。
  PicPaneInfo* picPaneInfo = &(picData->picPaneInfo[picPaneIndex]);
  picPaneInfo->pane->GetMaterial(0)->SetTexMap(0, &(picData->texInfo));
}
void ZukanPokePicManager::resetTexToPicPane(PicData* picData, u32 picPaneIndex)
{
  // ポケモン画像が貼られていようが貼られる前だろうが呼んで構わない。
  // ポケモン画像の読み込みが終わっていなくても呼んで構わない。
  PicPaneInfo* picPaneInfo = &(picData->picPaneInfo[picPaneIndex]);
  picPaneInfo->pane->GetMaterial(0)->SetTexMap(0, &(picPaneInfo->texInfo));
}

ZukanPokePicManager::LoadingPicInfo* ZukanPokePicManager::getBlankLoadingPicInfo(void)
{
  for(u32 i=0; i<LOADING_PIC_NUM_MAX; ++i)
  {
    if( m_loadingPicInfo[i].pLoadingPicData == NULL )
    {
      return &m_loadingPicInfo[i];
    }
  }
  return NULL;
}

void ZukanPokePicManager::releaseLoadingPicInfo(const PicData* picData)
{
  for(u32 i=0; i<LOADING_PIC_NUM_MAX; ++i)
  {
    if( m_loadingPicInfo[i].pLoadingPicData == picData )
    {
      m_loadingPicInfo[i].pLoadingPicData = NULL;
      break;
    }
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

