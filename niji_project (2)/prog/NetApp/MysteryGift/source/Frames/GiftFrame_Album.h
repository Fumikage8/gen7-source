#if !defined(GIFTFRAME_ALBUM_H_INCLUDED)
#define GIFTFRAME_ALBUM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_Album.h
 * @date   2015/08/03 Mon. 20:01:37
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "NetStatic/NetAppLib/include/Util/NetAppSaveUtility.h"
#include  "../GiftFrameDef.h"

#include  "../Views/GiftViewContainer.h"

namespace NetApp  {
namespace Gift {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_Album
  */
//==============================================================================
class Frame_Album
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_Album::IEventHandler
  , public LowerView_Album::ICardAlbumDataProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Album );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_Album
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Album(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_Album
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Album();



private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  ExitFrame(const FrameResult = RESULT_CARDALBUM_Exit);

  /*  LowerView_Album::IEventHandler  */
  virtual void  LowerView_Album_OnEvent(LowerView_Album* pSender, const LowerView_Album::IEventHandler::EventCode event);

  /*  LowerView_Album::ICardAlbumDataProvider  */
  virtual u32   AlbumDataProvider_DataNum(void) const;
  virtual bool  AlbumDataProvider_GetCardInfo(CardInfo& rInfo, const u32 listIndex);
  virtual bool  AlbumDataProvider_DeleteQuery(const u32 listIndex);


  void  OnExit(void);
  void  RefreshViews(const bool bRefreshUpper, const bool bRefreshLower, const bool isSkipLowerViewResetAnime = false);


  /*
    members
  */
  GiftWork*                           m_pWork;
  GiftViewContainer*                  m_pViewContainer;
  u32                                 m_State;
  FrameResult                         m_Result;
  bool                                m_bFrontFace;         /**<  表面  */
  bool                                m_IsDirty;
  pml::pokepara::PokemonParam*        m_pPokepara;          /**<  icon用Pokepara  */


  UpperView_Card*                     m_pUpperView_Card;
  LowerView_Album*                    m_pLowerView_Album;

  NetAppLib::Util::NetAppSaveUtility  m_SaveUtility;

};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_ALBUM_H_INCLUDED)  */
