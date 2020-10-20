//==============================================================================
/**
 * @file   EvolutionFrame_CheckZukan.cpp
 * @date   2015/09/10 Thu. 17:05:08
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../System/EvolutionWork.h"
#include  "../EvolutionResource.h"

//#include  "Savedata/include/ZukanRegisterChecker.h"

#include  "./EvolutionFrame_CheckZukan.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  EVOLUTION_CANCEL_BUTTON = gfl2::ui::BUTTON_B;

  enum  {
    STATE_Entry = 0,

    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_CheckZukan
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_CheckZukan::Frame_CheckZukan(EvolutionWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_CheckZukan
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_CheckZukan::~Frame_CheckZukan()
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
void  Frame_CheckZukan::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
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
bool  Frame_CheckZukan::startup()
{
  m_pWork->m_pAppParam->m_Result  = APPRESULT_Initial;

  m_pViewContainer->SetScene(EvolutionViewContainer::SCENE_CheckZukan);

  {
    PokeTool::PokeGetUtil::ReturnType::Tag  pokeGetUtilReturnType;
    FrameResult                             result  = RESULT_CHECKZUKAN_KnownPokemon;

    pokeGetUtilReturnType  = PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(
                                m_pWork->GetDeviceHeap(),
                                m_pWork->m_pAppParam->m_pTargetPokemon,
                                m_pWork->m_pAppParam->GetZukanCallerType()
                              );

    if(pokeGetUtilReturnType == PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER)
    {
      /*  eventに図鑑呼び出しを通知 */
      m_pWork->m_pAppParam->m_Result  = APPRESULT_CallZukan;
      result                          = RESULT_CHECKZUKAN_NewPokemon;
    }

    ExitFrame(result);
  }

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_CheckZukan::setListener()
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
void  Frame_CheckZukan::removeListener()
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
void  Frame_CheckZukan::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Exit:
      m_pWork->SetFrameResult( m_Result );
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_CheckZukan::cleanup()
{
  bool  bAllDone = false;


  bAllDone  = true;


  return  bAllDone;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_CheckZukan::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}



} /*  namespace Live    */
} /*  namespace NetApp  */


