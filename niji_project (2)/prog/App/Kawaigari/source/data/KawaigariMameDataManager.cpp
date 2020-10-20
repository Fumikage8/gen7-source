//======================================================================
/**
 * @file	KawaigariMameDataManager.C++
 * @brief	Kawaigari Mame Handling Code
 * @author	PETE
 * @data	2015.2.18
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "../KawaigariMain.h"

#include "GameSys/include/GameData.h"

#include "niji_conv_header/app/kawaigari/res2d/kw_layout.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_pane.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariMameDataManager::KawaigariMameDataManager(void)
{
  scrollPos = 0;  
}

KawaigariMameDataManager::~KawaigariMameDataManager(void)
{  
  for(int z=0;z<NUM_MAME_SLOT;z++)
  {
    mameCount[z] = 0;
    mameType[z] = -1;
  }
}

void KawaigariMameDataManager :: RemoveBean(int type)
{
  Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  save->SetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)type ,-1);
  
  for(int z=0;z<NUM_MAME_SLOT;z++)
  {
    mameCount[z] = 0;
    mameType[z] = -1;
  }

  int pos = 0;
  // Fill Up Slots for now:
  for(int z=0;z<NUM_MAME_SLOT;z++)
  {
    if(save->GetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)z)>0)
    {    
      mameCount[pos] = save->GetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)z);    
      mameType[pos++] = z;
    }
  }
 
}

void KawaigariMameDataManager :: AddBean(int type)
{
  Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  save->SetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)type ,1);

  for(int z=0;z<NUM_MAME_SLOT;z++)
  {
    mameCount[z] = 0;
    mameType[z] = -1;
  }

  int pos = 0;
  // Fill Up Slots for now:
  for(int z=0;z<NUM_MAME_SLOT;z++)
  {
    if(save->GetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)z)>0)
    {    
      mameCount[pos] = save->GetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)z);    
      mameType[pos++] = z;
    }
  }
  
}

void KawaigariMameDataManager :: Init(void)
{
  Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  
  int pos = 0;
  // Fill Up Slots for now:
  for(int z=0;z<NUM_MAME_SLOT;z++)
  {
    if(save->GetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)z)>0)
    {    
      mameCount[pos] = save->GetPokeBeansNum( (Savedata::ResortSave::PokeBeansEnum)z);    
      mameType[pos++] = z;
    }    
  }

  scrollPos = 0;
}

bool KawaigariMameDataManager :: CanScrollRight(void) 
{
  if(scrollPos>=NUM_MAME_SLOT-6) return(false);
  return(mameCount[scrollPos+6]>0); 
}


int KawaigariMameDataManager :: GetMameType(int pos)
{
  pos += scrollPos;

  if(pos<0 || pos>=15) return(-1);
  if(mameCount[pos]<=0) return(-1);
  return(mameType[pos]);
}

void KawaigariMameDataManager :: RestoreSelectedMameIcon(int index)
{
  app::util::G2DUtil* pG2D = coreSys->GetUIManager()->GetG2DUtil();
  gfl2::lyt::LytWk* lytWk = pG2D->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = pG2D->GetLayoutResourceID( 0 );  

  {  
    const int paneList[8] =  { 
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_01,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_02,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_03,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_04,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_05,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_06,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_07 };

    int num = mameCount[index+scrollPos];

    gfl2::lyt::LytParts *parts = lytWk->GetPartsPane( paneList[index] );        
    gfl2::lyt::LytTextBox *texPane = lytWk->GetTextBoxPane( parts, PANENAME_KAWAIGARI_BTN_LOW_001_PANE_MAMENUMB_00, &resID );

    char16 wStr[4];

#if defined(GF_PLATFORM_WIN32)
    _itow_s(num,wStr,10);
#else
    swprintf(wStr,4,L"%i",num);
#endif
    texPane->SetString( wStr );
  }
}

void KawaigariMameDataManager :: SetupSelectedMameIcon(int type,int index)
{
  app::util::G2DUtil* pG2D = coreSys->GetUIManager()->GetG2DUtil();
  gfl2::lyt::LytWk* lytWk = pG2D->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = pG2D->GetLayoutResourceID( 0 );  

  const int pane = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08;

  {  
    gfl2::lyt::LytParts *parts = lytWk->GetPartsPane( pane );
    gfl2::lyt::LytPicture *picPane = lytWk->GetPicturePane( parts, PANENAME_KAWAIGARI_BTN_LOW_001_PANE_MAMEICON_01, &resID );
    coreSys->GetUIManager()->GetTexSys()->ReplaceTexture( type, picPane, 0, 0 );
  }

  
  
  {  
    const int paneList[8] =  { 
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_01,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_02,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_03,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_04,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_05,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_06,
      PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_07 };
 
    int num = mameCount[index+scrollPos]-1;

    gfl2::lyt::LytParts *parts = lytWk->GetPartsPane( paneList[index] );        
    gfl2::lyt::LytTextBox *texPane = lytWk->GetTextBoxPane( parts, PANENAME_KAWAIGARI_BTN_LOW_001_PANE_MAMENUMB_00, &resID );
    
    char16 wStr[4];

  #if defined(GF_PLATFORM_WIN32)
    _itow_s(num,wStr,10);
  #else
    swprintf(wStr,4,L"%i",num);
  #endif
    texPane->SetString( wStr );
  }

}

void KawaigariMameDataManager :: SetupMameIcons(void)
{
  app::util::G2DUtil* pG2D = coreSys->GetUIManager()->GetG2DUtil();
  gfl2::lyt::LytWk* lytWk = pG2D->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = pG2D->GetLayoutResourceID( 0 );  

  const int paneList[8] =  { PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_00,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_01,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_02,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_03,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_04,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_05,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_06,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_07 };

  pG2D->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_00,false);

  for(int z=0;z<8;z++)
  {  
    int index = scrollPos+z - 1;
    if(index<0 || index>=15) 
    {
      pG2D->SetPaneVisible(0,paneList[z] ,false);
      continue;
    }

    int type = mameType[index];
    int num = mameCount[index]; //pMiscSaveData->GetKawaigariPastDays();

    if(type==-1 || num<=0)
    {
      pG2D->SetPaneVisible(0,paneList[z] ,false);
      continue;
    }

    if(z) pG2D->SetPaneVisible(0,paneList[z] ,true);    
    else  pG2D->SetPaneVisible(0,paneList[z] ,false);    

    gfl2::lyt::LytParts *parts = lytWk->GetPartsPane( paneList[z] );
    gfl2::lyt::LytPicture *picPane = lytWk->GetPicturePane( parts, PANENAME_KAWAIGARI_BTN_LOW_001_PANE_MAMEICON_01, &resID );
    coreSys->GetUIManager()->GetTexSys()->ReplaceTexture( type, picPane, 0, 0 );

    gfl2::lyt::LytTextBox *texPane = lytWk->GetTextBoxPane( parts, PANENAME_KAWAIGARI_BTN_LOW_001_PANE_MAMENUMB_00, &resID );
   
    char16 wStr[4];
 

#if defined(GF_PLATFORM_WIN32)
    _itow_s(num,wStr,10);
#else
    swprintf(wStr,4,L"%i",num);
#endif
    texPane->SetString( wStr );
  }

}

void KawaigariMameDataManager :: Finalize(void)
{

}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)