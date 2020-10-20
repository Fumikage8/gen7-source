#if !defined(FONTRESOURCEMANAGER_H_INCLUDED)
#define FONTRESOURCEMANAGER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   FontResourceManager.h
 * @date   2015/07/22 Wed. 14:39:29
 * @author muto_yu
 * @brief  フォント読み込みのラッパー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

namespace app {
namespace util  {
  class Heap;
}
}



// =============================================================================
/**/
//==============================================================================

namespace NetAppLib {
namespace System    {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/22 Wed. 14:39:56
  */
//==============================================================================
class FontResourceManager
{
  GFL_FORBID_COPY_AND_ASSIGN( FontResourceManager );
public:
  
  /**
    フォント種別
  */
  /*  ※実装側関数テーブルと並びを一致させること  */
  enum  FontType
  {
    FONT_NUMBER,
    FONT_BATTLE,
    
    FONT_MAX
  };
  
  
  
  FontResourceManager(app::util::Heap* pHeap);
  virtual ~FontResourceManager();


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    フォントの読み込みをリクエスト
    *  @note     フォントは読み込み完了後、自動的にシステムにセットされる
    */
  //------------------------------------------------------------------
  void  LoadRequest(const FontType fontType);


  //------------------------------------------------------------------
  /**
    *  @brief    状態更新
    *  @return   IsStableの値
    */
  //------------------------------------------------------------------
  bool  Update(void);


  //------------------------------------------------------------------
  /**
    *  @brief    安定状態である（読み込みがリクエストされていないか、読み込みがリクエストされた後読み込みが完了し、システムにセットされている）か
    */
  //------------------------------------------------------------------
  bool  IsStable(void) const;


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    全フォントの解除及び解体
    *  @note     dtorで自動的に呼ばれる
    */
  //------------------------------------------------------------------
  void  Release(void);


protected:
  app::util::Heap*  m_pHeap;

  struct  FontInfo
  {
    bool  isRequestedLoad;    /**<  読み込みがリクエストされている      */
    bool  isCalledSetFont;    /**<  読み込みが完了し、SetFont済みである */
    
    /** ctor  */
    FontInfo(void)
    {
      Clear();
    }
    
    /** 安定状態である（読み込みがリクエストされていないか、読み込みがリクエストされた後読み込みが完了し、システムにセットされている）か  */
    bool  IsStable(void) const  {return ((!isRequestedLoad) || (isRequestedLoad && isCalledSetFont));}
    /** クリア  */
    void  Clear(void)
    {
      isRequestedLoad = false;
      isCalledSetFont = false;
    }
  }m_FontInfo[FONT_MAX];
};




} /*  namespace System    */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(FONTRESOURCEMANAGER_H_INCLUDED)  */
