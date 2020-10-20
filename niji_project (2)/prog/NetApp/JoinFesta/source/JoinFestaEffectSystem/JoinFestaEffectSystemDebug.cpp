//==============================================================================
/**
 * @file   JoinFestaEffectSystemDebug.cpp
 * @date   2015/10/22 Thu. 12:38:09
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameManager.h"

#include  "Field/FieldRo/include/Fieldmap.h"
#include  "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include  "Field/FieldRo/include/Effect/content/FieldEffectFesMessage.h"


#include  "NetApp/JoinFesta/include/JoinFestaResourceManager/JoinFestaResourceManager.h"
#include  "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystem.h"
#include  "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystemDebug.h"


#ifdef JF_DEBUG_MENU
// =============================================================================
/**/
//==============================================================================
namespace {
  static const wchar_t*   MYGROUP_NAME  = L"エフェクト";
}


#define   DEBUGFUNC_BEGIN(__funcName)  \
            static void  __funcName( void* pUserWork, gfl2::debug::DebugWinItem* pWinItem) \
            { \
              JoinFestaEffectSystemDebug* pDebug        = static_cast<JoinFestaEffectSystemDebug*>(pUserWork); \
              JoinFestaEffectSystem*      pEffectSystem = pDebug->m_pEffectSystem;  \
              gfl2::ui::Button*           pDebugButton  = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD); \
              \
              if(pDebugButton->IsTrigger( gfl2::ui::BUTTON_A )) \
              { \
                GFL_PRINT("<%s>\n", __FUNCTION__);

                
#define   DEBUGFUNC_END  \
              }\
            }

//#define   DEBUGFUNC_LITERAL(__funcName) L ## #__funcName
#define   REGISTER_DEBUGFUNC(__funcName)  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pMyGroup, pHeapBase, L"<"  L ## #__funcName L">",  this, JoinFestaEffectSystemDebug::DebugFuncs::__funcName)


namespace NetApp    {
namespace JoinFesta {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/22 Thu. 12:47:37
  */
//==============================================================================
class JoinFestaEffectSystemDebug::DebugFuncs
{
public:
  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  DEBUGFUNC_BEGIN(HeapDump)
    pDebug->m_pHeapBase->Dump();
  DEBUGFUNC_END

  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  DEBUGFUNC_BEGIN(ShowMessage)
    pEffectSystem->ShowMessageEffect(NULL);
  DEBUGFUNC_END

  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  DEBUGFUNC_BEGIN(HideMessage)
    pEffectSystem->HideMessageEffect();
  DEBUGFUNC_END


  //------------------------------------------------------------------
  /**
    */
  //------------------------------------------------------------------
  DEBUGFUNC_BEGIN(Effect_Rankup)
    pEffectSystem->Start_Rankup();
  DEBUGFUNC_END

  DEBUGFUNC_BEGIN(Effect_NewShop)
    pEffectSystem->Start_NewShop();
  DEBUGFUNC_END

  DEBUGFUNC_BEGIN(Effect_Warp)
    Field::Fieldmap*  pFieldMap = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap();
//    pEffectSystem->Start_Warp(gfl2::math::Vector3(4191.0f, 50.0f, 5511.0f));
    pEffectSystem->Start_Warp(pFieldMap->GetPlayerPosition());
  DEBUGFUNC_END

  DEBUGFUNC_BEGIN(Effect_Attraction)
    pEffectSystem->Start_Attraction();
  DEBUGFUNC_END

  DEBUGFUNC_BEGIN(Effect_Rankup_Stop)
    pEffectSystem->Stop_Rankup();
  DEBUGFUNC_END

  DEBUGFUNC_BEGIN(Effect_NewShop_Stop)
    pEffectSystem->Stop_NewShop();
  DEBUGFUNC_END

  //DEBUGFUNC_BEGIN(Effect_Warp_Stop)
    //pEffectSystem->Stop_Warp();
  //DEBUGFUNC_END

  DEBUGFUNC_BEGIN(Effect_Attraction_Stop)
    pEffectSystem->Stop_Attraction();
  DEBUGFUNC_END


};  /*  class JoinFestaEffectSystemDebug::DebugFuncs  */


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/22 Thu. 12:40:44
  */
//==============================================================================
JoinFestaEffectSystemDebug::JoinFestaEffectSystemDebug(gfl2::heap::HeapBase* pHeapBase, JoinFestaEffectSystem* pEffectSystem, gfl2::debug::DebugWinGroup* pParentGroup)
  : m_pHeapBase(pHeapBase)
  , m_pEffectSystem(pEffectSystem)
  , m_pMyGroup(NULL)
  , m_pFesMessage(NULL)
{
  m_pMyGroup  = gfl2::debug::DebugWin_AddGroup(pHeapBase, MYGROUP_NAME, pParentGroup);

  /*
  */
  REGISTER_DEBUGFUNC(HeapDump);
  REGISTER_DEBUGFUNC(ShowMessage);
  REGISTER_DEBUGFUNC(HideMessage);



  REGISTER_DEBUGFUNC(Effect_Rankup);
  REGISTER_DEBUGFUNC(Effect_Rankup_Stop);

  REGISTER_DEBUGFUNC(Effect_NewShop);
  REGISTER_DEBUGFUNC(Effect_NewShop_Stop);

  REGISTER_DEBUGFUNC(Effect_Warp);
  //REGISTER_DEBUGFUNC(Effect_Warp_Stop);

  REGISTER_DEBUGFUNC(Effect_Attraction);
  REGISTER_DEBUGFUNC(Effect_Attraction_Stop);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
JoinFestaEffectSystemDebug::~JoinFestaEffectSystemDebug()
{
  gfl2::debug::DebugWin_RemoveGroup(m_pMyGroup);
}


} /*  namespace JoinFesta */
} /*  namespace NetApp    */
#endif  /*  JF_DEBUG_MENU */
