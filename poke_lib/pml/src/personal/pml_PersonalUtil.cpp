//=============================================================================
/**
 * @file   pml_PersonalUtil.cpp
 * @date   2015/12/28 15:10:38
 * @author obata_toshihiro
 * @brief  簡単な説明
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <pml/include/personal/pml_PersonalUtil.h>
#include <pml/include/personal/pml_PersonalSystem.h>


GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( personal )


//-----------------------------------------------------------------------------
/**
 * @brief 指定した地方種のフォルムNo.を取得する
 * @param[out] formArray  取得したフォルムNo.の格納先( 要素数が MAX_FORM_NUM 以上である必要があります )
 * @param[out] formNum    取得したフォルムNo.の数
 * @param      monsno     取得対象のモンスターNo.
 * @param      regionId   取得対象の地方種ID
 */
//-----------------------------------------------------------------------------
void Util::GetRegionFormNo( FormNo* formArray, u32* formNum, MonsNo monsno, RegionID regionId )
{
  *formNum = 0;

  personal::LoadPersonalData( monsno, 0 );
  const u32 formCount = personal::GetPersonalParam( personal::PARAM_ID_form_max );

  for( u32 formNo=0; formNo<formCount; ++formNo )
  {
    personal::LoadPersonalData( monsno, formNo );
    const RegionID region = static_cast<RegionID>( personal::GetPersonalParam( personal::PARAM_ID_region ) );

    if( region == regionId )
    {
      formArray[ *formNum ] = formNo;
      (*formNum)++;
    }
  }
}



GFL_NAMESPACE_END( personal )
GFL_NAMESPACE_END( pml )