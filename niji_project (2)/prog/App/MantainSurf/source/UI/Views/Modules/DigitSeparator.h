#if !defined(DIGITSEPARATOR_H_INCLUDED)
#define DIGITSEPARATOR_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DigitSeparator.h
 * @date   2017/01/20 Fri. 19:59:45
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

#include  <Layout/include/gfl2_LytSys.h>
#include  <AppLib/include/Util/app_util_2d.h>

namespace App {
namespace MantainSurf {
namespace UI {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @brief  
  *  @date   2017/01/20 Fri. 19:10:57
  */
//==============================================================================
class DigitSeparator
{
  GFL_FORBID_COPY_AND_ASSIGN(DigitSeparator);
public:
  struct Info
  {
    static const u32  Languages = 11;                   /**<  タイトルの言語数  */

    /*  言語によるセパレータ切り替えAnimのFrame リソースをこの通りに合わせる  */
    static const u32  AnimFarme_Comma  = 0;
    static const u32  AnimFarme_Space  = 1;
    const static u32  AnimFarme_Dot    = 2;

    app::util::G2DUtil*   pG2DUtil;
    u32                   layoutID;
    u32                   animID;                       /**<  セパレータ制御anim                                    */
    u32                   languageID;
    /*  disp  */
    u32                   threshold;                    /**<  表示閾値（visibility制御用）                          */
    gfl2::lyt::LytPane*   pSeparatorPane;               /**<  桁区切りpane（visibility制御用）: NULLで自動制御なし  */
    

    /*  ctor  */
    Info(void)
      : pG2DUtil(NULL)
      , layoutID(0)
      , animID(UINT_MAX)
      , languageID(POKEMON_LANG_NULL)
      , threshold(0)
      , pSeparatorPane(NULL)
    {
    }
  };


  /*  ctor  */
  DigitSeparator(void)
    : m_Info()
  {
  }
  

  /*  dtor  */
  virtual ~DigitSeparator() {}

  void        Setup(const Info& rInfo)    {m_Info = rInfo;}
  const Info& GetInfo(void) const   {return m_Info;}

  void        UpdateByValue(const u32 value);

protected:
  void  SetupLocale(void);

  Info  m_Info;
};


}  /*  namespace UI  */
}  /*  namespace MantainSurf  */
}  /*  namespace App  */
#endif  /*  #if !defined(DIGITSEPARATOR_H_INCLUDED)  */
