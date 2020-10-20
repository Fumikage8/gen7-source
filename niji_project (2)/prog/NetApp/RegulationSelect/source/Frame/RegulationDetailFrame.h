// ============================================================================
/*
 * @file RegulationDetailFrame.h
 * @brief レギュレーション詳細画面のフレームです。
 * @date 2015.07.15
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONDETAIL_FRAME_H_INCLUDE )
#define REGULATIONDETAIL_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include "NetApp/RegulationSelect/source/RegulationSelectFrameBase.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"
#include "AppLib/include/Util/AppQRTextureCreateUtility.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class RegulationSelectViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(Frame)


class RegulationDetailFrame :
  public NetApp::RegulationSelect::RegulationSelectFrameBase,
  public app::tool::InfoWindowBookTypeListener
{

  GFL_FORBID_COPY_AND_ASSIGN( RegulationDetailFrame );

public:

  RegulationDetailFrame(
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork,
    NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects );

  virtual~RegulationDetailFrame();

  // InfoWindowBookTypeListener
  virtual void OnClose();
  virtual void OnChangePage( u32 pageNo );

private:

  // RegulationSelectFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();


  enum PageInfo
  {
    PAGE_INFO_POKE,
    PAGE_INFO_ITEM,
    PAGE_INFO_WAZA
  };

  enum PageMenuMax
  {
    PAGE_MENU_MAX = 16
  };

  static const u32 ENCODE_DATA_BUFFER_SIZE = 512;

  u32 getDetailPageNum();
  bool setRegisterNumber( PageInfo ePageInfo, gfl2::util::List<u16>& vetoList, u32 startPageIndex, u32 drawPageIndex );

  u32 calcPageNum( u32 listSize );

private:

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects*       m_pRegulationSelectViewObjects;
  app::util::AppQRTextureCreateUtility                                            m_AppQRTextureCreateUtility;
  app::tool::InfoWindowBookType*                                                  m_pInfoWindowBookTypeLowerView;
  gfl2::util::List<u16>                                                           m_VetoPokeList;
  gfl2::util::List<u16>                                                           m_VetoItemList;
  gfl2::util::List<u16>                                                           m_VetoWazaList;
  gfl2::util::List<u8>                                                            m_PageInfoList;
  u32                                                                             m_VetoPokeStartPageIndex;
  u32                                                                             m_VetoItemStartPageIndex;
  u32                                                                             m_VetoWazaStartPageIndex;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDETAIL_FRAME_H_INCLUDE
