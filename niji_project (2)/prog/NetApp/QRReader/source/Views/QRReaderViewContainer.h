
#if !defined(QRREADERVIEWCONTAINER_H_INCLUDED)
#define QRREADERVIEWCONTAINER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderViewContainer.h
 * @date   2016/01/14 Thu. 17:47:57
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>

#include  "./UpperView_Main.h"
#include  "./UpperView_Pokemon.h"
#include  "./LowerView_Main.h"


namespace NetApp    {
namespace QRReader {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/17 Fri. 16:13:11
  */
//==============================================================================
class QRReaderViewContainer
{
  GFL_FORBID_COPY_AND_ASSIGN(QRReaderViewContainer);

public:
  QRReaderViewContainer(QRReaderWork* pWork);
  virtual ~QRReaderViewContainer();


  enum  SceneID
  {
    SCENE_Void,

    SCENE_Tutorial,
    SCENE_QRRead,
    SCENE_FieldScan,

    SCENE_
  };
  void  SetScene(const SceneID sceneID);

  void  Update(void);
  void  Draw(const gfl2::gfx::CtrDisplayNo dispNo);

  bool  IsDrawingAny(void) const;
  void  SetAllInputEnable(const bool isEnable);

  /*
    view
  */
  UpperView_Main*     m_pUpperView_Main;
  UpperView_Pokemon*  m_pUpperView_Pokemon;
  LowerView_Main*     m_pLowerView_Main;

private:
  void  ClearViewArray(void);

  QRReaderWork*     m_pWork;
  app::ui::UIView*  m_pViewArray[4];    /**<  カレント  */
};


} /*  namespace QRReader */
} /*  namespace NetApp    */
#endif  /*  #if !defined(QRREADERVIEWCONTAINER_H_INCLUDED)  */
