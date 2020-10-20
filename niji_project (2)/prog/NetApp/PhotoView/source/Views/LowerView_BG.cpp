//==============================================================================
/**
 * @file   LowerView_BG.cpp
 * @date   2017/02/13 Mon. 11:27:53
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../ApplicationSystem/PhotoViewWork.h"
#include  "../PhotoViewResource.h"
#include  "./LowerView_BG.h"

namespace NetApp {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_LowerView_BG
namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_BG::LowerView_BG(PhotoViewWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork)
  , m_pWork(pWork)
{
  Create2DOneLayout(
    m_pWork,
    RESOURCE_Layout,
    LYTRES_JOIN_FESTA_PLAYERLIST_JFPL_BGR_LOW_000_BFLYT,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_BGR_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    &(m_pWork->GetMessageUtility()),
    GARC_message_jf_playerlist_DAT
  );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_BG::~LowerView_BG()
{
  Delete2D();
  DeleteGraphicsSystem();
}



/*  implement UIView I/F  */

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
//  virtual void  LowerView_BG::Update(void);


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_BG::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, 0);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
//  virtual bool  LowerView_BG::IsDrawing(void) const;
/*  endimplement UIView I/F  */



}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
