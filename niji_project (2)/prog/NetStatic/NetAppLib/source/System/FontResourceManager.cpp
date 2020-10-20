//==============================================================================
/**
 * @file   FontResourceManager.cpp
 * @date   2015/07/22 Wed. 14:55:22
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "Print/include/SystemFont.h"
#include  "AppLib/include/Util/app_util_heap.h"
#include  "NetStatic/NetAppLib/include/System/FontResourceManager.h"

// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace
{
  /**
    フォント種別に対応した各種関数テーブル
  */
  static const struct
  {
    void  (*pFunction_LoadRequest)(gfl2::heap::HeapBase *, gfl2::heap::HeapBase*);
    bool  (*pFunction_IsLoad)(void);
    void  (*pFunction_SetLoadFont)(gfl2::heap::HeapBase *);
    void  (*pFunction_SetLytFont)(void);
    void  (*pFunction_Reset)(void);
    void  (*pFunction_Delete)(void);
  }fontFunctionTable[NetAppLib::System::FontResourceManager::FONT_MAX] =
  {
    /*  ※FontTypeを直接添え字に使用するので、宣言側と並びを一致させること  */
    /*  FONT_NUMBER */
    {
      print::SystemFont_LoadNumFont,
      print::SystemFont_IsLoadNumFont,
      print::SystemFont_SetLoadNumFont,
      print::SystemFont_SetLytSystemNumFont,
      print::SystemFont_ResetNumFontInfo,
      print::SystemFont_DeleteNumFont
    },
    /*  FONT_BATTLE */
    {
      print::SystemFont_LoadBattleFont,
      print::SystemFont_IsLoadBattleFont,
      print::SystemFont_SetLoadBattleFont,
      print::SystemFont_SetLytSystemBattleFont,
      print::SystemFont_ResetBattleFontInfo,
      print::SystemFont_DeleteBattleFont
    },
  };
}


namespace NetAppLib {
namespace System    {
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
FontResourceManager::FontResourceManager(app::util::Heap* pHeap)
  : m_pHeap(pHeap)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    dtor
  */
//------------------------------------------------------------------
FontResourceManager::~FontResourceManager()
{
  Release();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  FontResourceManager::LoadRequest(const FontType fontType)
{
  GFL_ASSERT(fontType < FONT_MAX);

  if(m_FontInfo[fontType].isRequestedLoad)
  {
    GFL_PRINT("warning %s(%d):%s: fonttype(%d) is already requested\n", __FILE__, __LINE__, __FUNCTION__, static_cast<int>(fontType));
    return;
  }

  fontFunctionTable[fontType].pFunction_LoadRequest(m_pHeap->GetSystemHeap(), m_pHeap->GetDeviceHeap());
  m_FontInfo[fontType].isRequestedLoad  = true;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
bool  FontResourceManager::Update(void)
{

  for(u32 fontInfoIndex  = 0; fontInfoIndex < GFL_NELEMS(m_FontInfo); ++fontInfoIndex)
  {
    GFL_ASSERT(fontInfoIndex < FONT_MAX);
    if(!m_FontInfo[fontInfoIndex].IsStable())
    {
      if(fontFunctionTable[fontInfoIndex].pFunction_IsLoad())
      {
        fontFunctionTable[fontInfoIndex].pFunction_SetLoadFont(m_pHeap->GetDeviceHeap());
        fontFunctionTable[fontInfoIndex].pFunction_SetLytFont();
        m_FontInfo[fontInfoIndex].isCalledSetFont = true;
      }
    }
  }


  return  IsStable();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  FontResourceManager::IsStable(void) const
{
  bool  isStable  = true;

  for(u32 fontInfoIndex  = 0; fontInfoIndex < GFL_NELEMS(m_FontInfo); ++fontInfoIndex)
  {
    GFL_ASSERT(fontInfoIndex < FONT_MAX);

    if(!m_FontInfo[fontInfoIndex].IsStable())
    {
      isStable  = false;
      break;
    }
  }

  return isStable;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  FontResourceManager::Release(void)
{
  for(u32 fontInfoIndex  = 0; fontInfoIndex < GFL_NELEMS(m_FontInfo); ++fontInfoIndex)
  {
    GFL_ASSERT(fontInfoIndex < FONT_MAX);

    fontFunctionTable[fontInfoIndex].pFunction_Reset();
    fontFunctionTable[fontInfoIndex].pFunction_Delete();
    m_FontInfo[fontInfoIndex].Clear();
  }
}




} /*  namespace System    */
} /*  namespace NetAppLib */
