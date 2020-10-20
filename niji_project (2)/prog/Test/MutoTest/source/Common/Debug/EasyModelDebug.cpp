
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyModelDebug.cpp
 * @date   2016/09/15 Thu. 15:07:07
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./EasyModelDebug.h"

namespace
{
  void  dummy(void* _pThis, gfl2::debug::DebugWinItem* pItem){}
}



namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------

EasyModel::EasyModelDebug::EasyModelDebug(void)
  : m_pOwner(NULL)
  , m_pGroup_Root(NULL)
{
}

EasyModel::EasyModelDebug::~EasyModelDebug()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyModel::EasyModelDebug::AddTo(gfl2::debug::DebugWinGroup* pGroup, gfl2::heap::HeapBase* pHeapBase)
{
  m_pGroup_Root = pGroup;

#define VALDEF(_ItemID, _CaptionID, _min, _max)   do{ \
                                                    m_ItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueInt(m_pGroup_Root, pHeapBase, pMsgData, _CaptionID, &m_ItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ItemInfos[(_ItemID)].value      = (_min); \
                                                  }while(false)
#define VALDEFDIRECT(_ItemID, _Caption, _min, _max)   do{ \
                                                    m_ItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueInt(m_pGroup_Root, pHeapBase, _Caption, &m_ItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ItemInfos[(_ItemID)].value      = (_min); \
                                                  }while(false)

#define PROCDEF(_CaptionID, _ptrFunc)             do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, _CaptionID, this, _ptrFunc);  \
                                                  }while(false)

#define PROCDEFDIRECT(_Caption, _ptrFunc)       do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, _Caption, this, _ptrFunc);  \
                                                  }while(false)

  {
    const u32   maxChar = 255;
    wchar_t     str[maxChar+1];


    PROCDEFDIRECT(L"-----",    dummy);
    PROCDEFDIRECT(L"-stats-",  dummy);
    
    swprintf(str, maxChar, L"verts: %4u",   m_pOwner->GetBaseModel()->GetVertexCount());
    PROCDEFDIRECT(str,  dummy);
    swprintf(str, maxChar, L"polys: %4u",   m_pOwner->GetBaseModel()->GetPolygonCount());
    PROCDEFDIRECT(str,  dummy);
    swprintf(str, maxChar, L"meshs: %4u",   m_pOwner->GetBaseModel()->GetMeshCount());
    PROCDEFDIRECT(str,  dummy);
    swprintf(str, maxChar, L"mats: %4u",    m_pOwner->GetBaseModel()->GetMaterialCount());
    PROCDEFDIRECT(str,  dummy);
    swprintf(str, maxChar, L"texs: %4u",    m_pOwner->GetBaseModel()->GetTextureCount());
    PROCDEFDIRECT(str,  dummy);
    swprintf(str, maxChar, L"joints: %4u",  m_pOwner->GetBaseModel()->GetJointCount());
    PROCDEFDIRECT(str,  dummy);
  }

#undef  PROCDEFDIRECT
#undef  VALDEFDIRECT
#undef  PROCDEF
#undef  VALDEF


}



} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
