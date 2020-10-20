//======================================================================
/**
 * @file    PokeCapAccesser.cpp
 * @brief   ポケモンキャプチャー画像をアクセスするクラス
 * @attention   16/05/12 現在ポケモンセンターでしか使っていないので、DLLへ持っていくのはアリ
 * @author  k.ohno
 * @data    15/10/25
 */
//======================================================================

#include <arc_def_index/arc_def.h>
#include <arc_index/pokecap128.gaix>
#include <arc_index/caplist.gaix>

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Tool/PokeCapAccesser.h"
#include "pml/include/pml_PokePara.h"
#include <AppLib/include/Util/app_util_FileAccessor.h>

//#include "pokecaplist.cdat"
//ARCID_CAPLIST

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Tool)

PokeCapAccesser::PokeCapAccesser(app::util::Heap* pAppHeap,gfl2::fs::AsyncFileManager* pAsyncFileManager)
{
  mpAppHeap = pAppHeap;
  mpAsyncFileManager = pAsyncFileManager;
  mpReadBuff = NULL;
  
}

PokeCapAccesser::~PokeCapAccesser(void)
{
  GflHeapSafeFreeMemory(mpReadBuff);
}


int PokeCapAccesser::GetArcFileIndex(pml::pokepara::CoreParam* pPoke)
{
  int monsno = pPoke->GetMonsNo();
  int form = pPoke->GetFormNo();
  bool rare = pPoke->IsRare();
  pml::Sex gender = pPoke->GetSex();

#if 0
  for(int i=0;i< GARC_pokecap128_DATA_NUM ;i++){
    if(App::Pokemon::pokecaplistcdat_env[i][0] == monsno){
      if(App::Pokemon::pokecaplistcdat_env[i][1] == form){
        if(App::Pokemon::pokecaplistcdat_env[i][2] == (u16)rare){
          switch(App::Pokemon::pokecaplistcdat_env[i][3]){
          case 0:   // 性別はおなじ、もしくは不明 = 比較の必要が無いもの
            return i;
          case 1:   // オス
            if(gender == pml::SEX_MALE){
              return i;
            }
            break;
          case 2:   // メス
            if(gender == pml::SEX_FEMALE){
              return i;
            }
          }
        }
      }
    }
  }
#else

  if(mpReadBuff==NULL){
    OpenPokemonCaptureList();
    PokemonCaptureListRead();
    ClosePokemonCaptureList();
  }

  //GFNMCat[5695]  //報告のポケモンを確認すれば完了となるのでこの修正にした
  //@attention ラランテス,エンニュートについてはデータの追加を見送ったので同じ修正を施している
  switch(monsno){
  case MONSNO_KOHUKIMUSI:
  case MONSNO_KOHUURAI:
  case MONSNO_GAAMEIRU:
  case MONSNO_HANAKAMA2://ラランテス
  case MONSNO_DOKUTOKAGE2://エンニュート
  case MONSNO_MIZUGUMO2://オニシズクモ  @fix GFMMCat[323]
  case MONSNO_NIZITYUU://トゲデマル  @fix GFMMCat[323]
    form=0;
    break;
  }
  
  u16* listdat = static_cast<u16*>(mpReadBuff);
  
  for(int i=0;i< GARC_pokecap128_DATA_NUM ;i++){
    if(listdat[i*4+0] == (u16)monsno){
      if(listdat[i*4+1] == (u16)form){
        if(listdat[i*4+2] == (u16)rare){
          switch(listdat[i*4+3]){
          case 0:   // 性別はおなじ、もしくは不明 = 比較の必要が無いもの
            return i;
          case 1:   // オス
            if(gender == pml::SEX_MALE){
              return i;
            }
            break;
          case 2:   // メス
            if(gender == pml::SEX_FEMALE){
              return i;
            }
          }
        }
      }
    }
  }
#endif

  return 1;
}


void PokeCapAccesser::PokemonCaptureRead(int arcFileIndex, void** pReadBuff)
{
  app::util::FileAccessor::RequestData frs;

  {
    frs.loadr.datId      = arcFileIndex;  //poke                      // [in]  ファイル名。終端は'\0'になっていること。
    frs.loadr.arcId         = ARCID_POKECAP_128;                               // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    frs.loadr.prio          = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;    // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    frs.loadr.ppBuf         = pReadBuff;                                  // [out] 読み込んだデータの格納先( 4 byte alignment )
    frs.loadr.heapForBuf    = mpAppHeap->GetDeviceHeap();                      // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
    frs.loadr.heapForReq    = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
    frs.loadr.heapForCompressed = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
		frs.loadr.align = 128;
    
    mpAsyncFileManager->SyncArcFileLoadData( frs.loadr );
  }

}


void PokeCapAccesser::OpenPokemonCapture(void)
{
  app::util::FileAccessor::RequestData frs;

  {
    frs.openr.arcId       = ARCID_POKECAP_128;
    frs.openr.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.openr.heapForFile = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    frs.openr.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    mpAsyncFileManager->SyncArcFileOpen( frs.openr );
  }
}




void PokeCapAccesser::ClosePokemonCapture(void)
{
  app::util::FileAccessor::RequestData frs;
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    frs.closer.arcId       = ARCID_POKECAP_128;
    frs.closer.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.closer.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    mpAsyncFileManager->SyncArcFileClose( frs.closer );
  }
}






void PokeCapAccesser::PokemonCaptureListRead(void)
{
  app::util::FileAccessor::RequestData frs;

  {
    frs.loadr.datId      = 0;  //データはひとつしかない                      // [in]  ファイル名。終端は'\0'になっていること。
    frs.loadr.arcId         = ARCID_CAPLIST;                               // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
    frs.loadr.prio          = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;    // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
    frs.loadr.ppBuf         = &mpReadBuff;                                  // [out] 読み込んだデータの格納先( 4 byte alignment )
    frs.loadr.heapForBuf    = mpAppHeap->GetDeviceHeap();                      // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
    frs.loadr.heapForReq    = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
 //   frs.loadr.heapForCompressed = mpAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
		frs.loadr.align = 128;
    
    mpAsyncFileManager->SyncArcFileLoadData( frs.loadr );
  }

}


void PokeCapAccesser::OpenPokemonCaptureList(void)
{
  app::util::FileAccessor::RequestData frs;

  {
    frs.openr.arcId       = ARCID_CAPLIST;
    frs.openr.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.openr.heapForFile = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    frs.openr.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    mpAsyncFileManager->SyncArcFileOpen( frs.openr );
  }
}




void PokeCapAccesser::ClosePokemonCaptureList(void)
{
  app::util::FileAccessor::RequestData frs;
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    frs.closer.arcId       = ARCID_CAPLIST;
    frs.closer.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
    frs.closer.heapForReq  = mpAppHeap->GetFileReadHeap()->GetLowerHandle();
    mpAsyncFileManager->SyncArcFileClose( frs.closer );
  }
}






GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(AppLib)



