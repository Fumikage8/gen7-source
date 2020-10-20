//==============================================================================
/**
 * @file   FieldEffectFesMessage.cpp
 * @date   2015/10/20 Tue. 18:32:44
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <niji_conv_header/field/effect/fes_message.h>
#include  <niji_conv_header/field/effect/fes_message_anim_list.h>
#include  <niji_conv_header/field/effect/fes_message_pane.h>


#include  "AppLib/include/Util/AppRenderingManager.h"
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "../../../include/Window/FieldWindowManager.h"
#include  "../../../include/Effect/FieldEffectLayout.h"
#include  "../../../include/Effect/content/FieldEffectFesMessage.h"


namespace {
  static const u32  LOCAL_SYSHEAP_SIZE  = 0x1000;
//  static const u32  LOCAL_DECHEAP_SIZE  = 0x8000;
  static const u32  LOCAL_DECHEAP_SIZE  = 0xc000;

  static const u32  MYWORDSET_WORDMAX   = 2;
  static const u32  MYWORDSET_MAXLENGTH = 32;
  static const u32  MYSTRBUF_MAXLENGTH  = 128;


  static const u32  ANIMID_IN           = LA_FES_MESSAGE_JFF_WIN_UPP_000_TAG_ALLIN;
  static const u32  ANIMID_OUT          = LA_FES_MESSAGE_JFF_WIN_UPP_000_TAG_ALLOUT;
  static const u32  ANIMID_COLOR_NORMAL = LA_FES_MESSAGE_JFF_WIN_UPP_000_COLOR_00;
  static const u32  ANIMID_COLOR_RARE   = LA_FES_MESSAGE_JFF_WIN_UPP_000_COLOR_01;
  static const u32  ANIMID_COLOR_EVENT  = LA_FES_MESSAGE_JFF_WIN_UPP_000_COLOR_02;
}




// =============================================================================
/**/
//==============================================================================
namespace
{
  bool  STRCODEisNullOrEmpty(const gfl2::str::STRCODE* pStrCode)
  {
    return ((!pStrCode) || (pStrCode[0]==L'\0'));
  }
}








namespace Field   {
namespace Effect  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EffectFesMessage::EffectFesMessage(void)
  : IEffectBase()
  , m_SetupData()
  , m_LocalHeap()
  , m_pEffectLayout(NULL)
  , m_pWordSet(NULL)
  , m_pStrBuf(NULL)
  , m_pTmpBuf(NULL)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pTextBoxes); ++index)
  {
    m_pTextBoxes[index] = NULL;
  }

  for(u32 index = 0; index < GFL_NELEMS(m_pPanes); ++index)
  {
    m_pPanes[index] = NULL;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EffectFesMessage::~EffectFesMessage()
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
void  EffectFesMessage::Setup(const SetupData& rSetupData)
{
  gfl2::heap::HeapBase*   pLocalHeapBase;

  m_SetupData = rSetupData;


  m_LocalHeap.LocalHeapCreate(m_SetupData.pHeapBase, m_SetupData.pHeapBase, LOCAL_SYSHEAP_SIZE, LOCAL_DECHEAP_SIZE );
  pLocalHeapBase  = m_LocalHeap.GetDeviceHeap();

  m_pWordSet      = GFL_NEW(pLocalHeapBase) print::WordSet(MYWORDSET_WORDMAX, MYWORDSET_MAXLENGTH, pLocalHeapBase);
  m_pStrBuf       = GFL_NEW(pLocalHeapBase) gfl2::str::StrBuf(MYSTRBUF_MAXLENGTH, pLocalHeapBase); 
  m_pTmpBuf       = GFL_NEW(pLocalHeapBase) gfl2::str::StrBuf(MYSTRBUF_MAXLENGTH, pLocalHeapBase); 
  m_pEffectLayout = GFL_NEW(pLocalHeapBase) EffectLayout(&m_LocalHeap, m_SetupData.pFieldWindowManager->GetRenderingManager());


  // レイアウト初期化
  {
    const app::util::G2DUtil::LytResData lytResData[] = 
    {
      { m_SetupData.pResource, 0, app::util::G2DUtil::ATTACH_NONE },
    };
    app::util::G2DUtil::LytwkData lytwkData[] =
    {
      {
        0,
        LYTRES_FES_MESSAGE_JFF_WIN_UPP_000_BFLYT,
        LA_FES_MESSAGE_JFF_WIN_UPP_000___NUM,
        m_SetupData.pResource,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };

    {
      EffectLayout::SetupParam  effectLayoutSetupParam;

      effectLayoutSetupParam.layoutNum  = 1;
      effectLayoutSetupParam.lytResTbl  = lytResData;
      effectLayoutSetupParam.resTblNum  = GFL_NELEMS( lytResData );
      effectLayoutSetupParam.lytDataTbl = lytwkData;
      effectLayoutSetupParam.dataTblNum = GFL_NELEMS( lytwkData );

      m_pEffectLayout->Initialize( effectLayoutSetupParam, NULL, m_pWordSet);
    }

    /*  pane  */
    {
      app::util::G2DUtil*  pG2DUtil  = m_pEffectLayout->GetG2DUtil();
      
      app::util::GetPaneHelper  helper(pG2DUtil->GetLayoutWork(0), &pG2DUtil->GetLayoutResourceID(0));
      
      m_pTextBoxes[TEXTBOX_Name]      = helper.Clear().GetTextBoxPane(PANENAME_JFF_WIN_UPP_000_PANE_TEXTBOX_NAME);
      m_pTextBoxes[TEXTBOX_Message]   = helper.Clear().GetTextBoxPane(PANENAME_JFF_WIN_UPP_000_PANE_TEXTBOX_MESSAGE);
      
      m_pPanes[PANE_NamePlateControl] = helper.Clear().GetPane(PANENAME_JFF_WIN_UPP_000_PANE_NULL_NAMEPLATE);
    }
  }
  // 登録
  m_SetupData.pFieldWindowManager->GetUIView()->AddSubView( m_pEffectLayout );
  m_pEffectLayout->SetVisible(true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesMessage::Show(const gfl2::str::STRCODE* pName, const gfl2::str::STRCODE* pMessage, const bool isExpand, const bool isImmediate, const ColorMode colMode)
{
  app::util::G2DUtil* pG2DUtil  = m_pEffectLayout->GetG2DUtil();

  /*  anim  */
  pG2DUtil->StopAnime(0, ANIMID_IN);
  pG2DUtil->StopAnime(0, ANIMID_OUT);

  pG2DUtil->StartAnime(0, ANIMID_IN);
  if(isImmediate)
  {
    pG2DUtil->SetAnimeFrame(0, ANIMID_IN, pG2DUtil->GetAnimeMaxFrame(0, ANIMID_IN));
  }

  /*  colmode  */
  {
    u32   animID  = ANIMID_COLOR_NORMAL;

    switch(colMode)
    {
      case  COLMODE_Rare:    animID = ANIMID_COLOR_RARE;  break;
      case  COLMODE_Event:   animID = ANIMID_COLOR_EVENT; break;

      case  COLMODE_Normal:
      default:
        /* nop   */
        break;
    }
    pG2DUtil->StartAnime(0, animID);
  }


  /*  name  */
  if(STRCODEisNullOrEmpty(pName))
  {
    m_pPanes[PANE_NamePlateControl]->SetVisible(false);
  }
  else
  {
    m_pPanes[PANE_NamePlateControl]->SetVisible(true);
    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Name], pName);
  }
  

  /*  message  */
  if(STRCODEisNullOrEmpty(pMessage))
  {
    /*  NOP  */
  }
  else
  {
    m_pStrBuf->SetStr(pMessage);

    if( isExpand )
    {
      m_pTmpBuf->SetStr( *m_pStrBuf );
      m_pWordSet->Expand( m_pStrBuf, m_pTmpBuf );
    }

    pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_Message], m_pStrBuf);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesMessage::Hide(const bool isImmediate)
{
  app::util::G2DUtil*  pG2DUtil  = m_pEffectLayout->GetG2DUtil();

  /*  anim  */
  pG2DUtil->StopAnime(0, ANIMID_IN);
  pG2DUtil->StopAnime(0, ANIMID_OUT);

  pG2DUtil->StartAnime(0, ANIMID_OUT);
  if(isImmediate)
  {
    pG2DUtil->SetAnimeFrame(0, ANIMID_OUT, pG2DUtil->GetAnimeMaxFrame(0, ANIMID_OUT));
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void EffectFesMessage::SetRegisterMonsNameNo( u32 bufID, MonsNo monsno )
{
  m_pWordSet->RegisterPokeMonsNameNo( bufID, monsno );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  EffectFesMessage::Terminate( void )
{
  // @fix
  if( m_pEffectLayout )
  {
    m_pEffectLayout->RemoveFromSuperView();
  }

  GFL_SAFE_DELETE( m_pEffectLayout );
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pTmpBuf );
  GFL_SAFE_DELETE( m_pStrBuf );

  m_LocalHeap.Delete();
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
void  EffectFesMessage::Delete( void )
{
  //m_pEffectLayout->RemoveFromSuperView();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*
void  EffectFesMessage::updateNormal( void )
{
  if(!m_pEffectLayout)  return;

  app::util::G2DUtil* pG2DUtil  = m_pEffectLayout->GetG2DUtil();
}
*/



} /*  namespace Effect  */
} /*  namespace Field   */
