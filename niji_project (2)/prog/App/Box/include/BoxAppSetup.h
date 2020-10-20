//===================================================================
/**
 *	@file	    BoxAppSetup.h
 *	@brief    必要なパラメータをそろえる
              受け渡しは移動
              シンプルパラムはBoxPokeParaに移動
 *	@author	  k.ohno
 *	@date		  2015.9.23
 *
 */
//===================================================================
#ifndef __BOX_APP_SETUP_H__
#define __BOX_APP_SETUP_H__
#pragma once

#include "GameSys/include/GameManager.h"

#include "AppLib/include/Util/app_util_heap.h"

#include "App/Box/include/BoxAppParam.h"
#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"

#include <arc_index/boxLyt.gaix>

#include "niji_conv_header/app/box/res2d/box_pack.h"

#include "pml/include/pml_PokePara.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxEventListener;
class BoxSimpleAccesser;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

  class BoxAppSetup
  {
    GFL_FORBID_COPY_AND_ASSIGN( BoxAppSetup ); // コピーを禁止

  public:
    BoxAppSetup(void);
    ~BoxAppSetup();
    

  //  void SetupTemochi(gfl2::heap::HeapBase * heap,App::Box::APP_PARAM* pParam);
    void Setup(gfl2::heap::HeapBase * heap,App::Box::APP_PARAM* pAppBoxParam,GameSys::GameManager* game_manager);
    void Shutdown(void);

    void SetArcReadBuff(int gaixnum,void* pData);
    void* GetArcReadBuff(int gaixnum);

    void ProcResourceRead(int gaixnum, app::util::Heap* pAppHeap, gfl2::fs::AsyncFileManager* pFM);

    void ProcResourceFree(void);

    //@caution この関数は使わないで下さい。停止の不具合になります
    bool IsResourceLoaded(gfl2::fs::AsyncFileManager* pFM);
    //@attention こちらを使ってください
    bool IsResourceLoaded( int no);

    int GetMoveMode(void);
    int GetMoveButton(int type){ return mMoveButton[type]; };
    void SetMoveButton(int type,u8 no){ mMoveButton[type] = no; };

    int GetSelectMode(void);
    int GetSelectButton(int type){ return mSelectButton[type]; };
    void SetSelectButton(int type,u8 no){ mSelectButton[type] = no; };

    void Update(gfl2::fs::AsyncFileManager* pFM);

//    int GetPokeIconActive(int pos){ return mPokeIconActive[pos]; };
   // void SetPokeIconActive(int pos,u8 no){ mPokeIconActive[pos] = no; };
    
  //  PokeTool::SimpleParam* GetSimpleParam(int disp ,int pos);

    void SetBoxSimpleAccesser(BoxSimpleAccesser* pA ){ mpBoxSimpleAccesser = pA;};
    BoxSimpleAccesser* GetBoxSimpleAccesser(void ){ return mpBoxSimpleAccesser;};


    void* GetMenuWindowResource(void){ return mpMenuWindowRes; };
    void CreateMenuWindowResource(gfl2::fs::AsyncFileManager* pFM,app::util::Heap* pAppHeap);
    bool IsMenuWindowResourceLoaded(gfl2::fs::AsyncFileManager* pFM,app::util::Heap* pAppHeap);
    void DeleteMenuWindowResource(void);

    void SetOtherMessage(int no);
    int GetOtherMessage(void);

    void BinLinkLoad(app::util::Heap* pAppHeap);
    void* GetBinLinkReadBuff(int linkno);
    bool IsBinLinkLoad(void);
    void BinLinkClose(app::util::Heap* pAppHeap);


//ポケモンデータをボックス位置、カーソル位置から得て、設定
    void SetPokemon(gfl2::heap::HeapBase * heap);

    
  private:
    int GetNowTrayNo(void);
    void makeManagerData(gfl2::heap::HeapBase * heap,GameSys::GameManager* game_manager);
   // void makeBoxSimpleData(gfl2::heap::HeapBase * heap,pml::pokepara::CoreParam* cp);
  //  void makeTemotiSimpleData(gfl2::heap::HeapBase * heap);
   // void makeBattleBoxSimpleData(gfl2::heap::HeapBase * heap);
 //   void makeResortGimSimpleData(gfl2::heap::HeapBase * heap);
   // void makeResortHotspaSimpleData(gfl2::heap::HeapBase * heap);

  private:
    BoxSimpleAccesser* mpBoxSimpleAccesser;
      
    App::Box::APP_PARAM* mpParam;

    void* mpResourceBuff[GARC_boxLyt_DATA_NUM];
    u8 mbResourceLoaded[GARC_boxLyt_DATA_NUM];

    u8 mPokeIconActive[Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS]; //アイコンのactive,passive表示 36
    u8 mSelectButton[BOX_CHANGE_EDITTYPE_MAX];  //通常、アイテム、チーム選択
    u8 mMoveButton[BOX_CHANGE_SELECTMODE_MAX];    //通常、ショートカット、複数選択

    void * mpMenuWindowRes;
    gfl2::fs::Result mResulto;
    gfl2::fs::Result mResult;
    gfl2::fs::Result mResultc;
  void*                               m_dataBuf;
  gfl2::fs::BinLinkerAccessor         m_binLinker;

    int mSequence;
    
    int mOtherMessage;
  };


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif // __BOX_APP_SETUP_H__

