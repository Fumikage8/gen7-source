//=======================================================================================
/**
 * @file BgSetupUtil.h
 * @brief バトル背景、簡易指定処理
 * @author ikuta_junya
 *         ariizumi_nobuhiko(niji移植15.09.09)
 * @date 2012.08.08
 */
//=======================================================================================
#ifndef __SYSTEM_BACKGROUND_BGSYSUTILSETUP_H__
#define __SYSTEM_BACKGROUND_BGSYSUTILSETUP_H__

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

// enum name
#include <niji_conv_header/battle/background/bgsys_near_type_def.h>
#include <niji_conv_header/battle/background/bgsys_far_type_def.h>
#include <niji_conv_header/battle/background/bgsys_env_def.h>
#include <niji_conv_header/battle/background/bgsys_normal_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_near_ground_def.h>
#include <niji_conv_header/battle/background/bgsys_far_ground_def.h>

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/FieldAttribute.h"

namespace Field
{
  class Fieldmap;
}

GFL_NAMESPACE_BEGIN(btl)

enum
{
  HIMITUNOTIKARA_TYPE_ATTACK,   //こうげき
  HIMITUNOTIKARA_TYPE_DEFENCE,  //ぼうぎょ
  HIMITUNOTIKARA_TYPE_SP_ATK,   //とくこう
  HIMITUNOTIKARA_TYPE_SP_DEF,   //とくぼう
  HIMITUNOTIKARA_TYPE_AGIRITY,  //すばやさ
  HIMITUNOTIKARA_TYPE_HIT_RATIO,//めいちゅう
  HIMITUNOTIKARA_TYPE_DODGE,    //かいひ
  HIMITUNOTIKARA_TYPE_DOKU,     //どく
  HIMITUNOTIKARA_TYPE_MAHI,     //まひ
  HIMITUNOTIKARA_TYPE_NEMURI,   //ねむり
  HIMITUNOTIKARA_TYPE_KOORI,    //こおり
  HIMITUNOTIKARA_TYPE_YAKEDO,   //やけど
  HIMITUNOTIKARA_TYPE_KONRAN,   //こんらん
  HIMITUNOTIKARA_TYPE_HIRUMI,   //ひるみ
};

/// 簡易指定で決定されるデータをまとめた構造体
struct BgComponentData
{
  //背景モデルの種類
  btl::bg::bgsys_normal_groundID groudnType;
  btl::bg::bgsys_near_groundID nearType;
  btl::bg::bgsys_far_groundID farType;

  //天球の有無
  bool useSkyBox;

  //ロジックに影響のある項目
  bool enableDarkBall;          //ダークボール有効
  bool enableDiveBall;          //ダイブボール有効
  u16  sizennotikaraWazaNo;     //「しぜんのちから」で出る技(WazaID)
  u16  himitinotikaraEffect;    //「ひみつのちから」で出る演出(WazaID)
  u8   himitinotikaraType;      //「ひみつのちから」の効果(HIMITUNOTIKARA_TYPE_○○)
  u8   minomuttiFormNo;         //ミノムッチが変化するフォルム(くさ・すな・ごみ)
  u8   hogoshokuType;           //保護色で変わるタイプ(PokeType_tag)
};

//----------------------------------------------------------------------------
/**
 *	@brief  バトル背景　外背景IDと内背景IDから表示設定を取得する 
 *
 *  @param  farTypeId     外背景ID
 *  @param  nearTypeId    内背景ID
 *	@param	pOut          設定をセットするポインタ
 *	@return pOutで渡されたポインタ
 */
//-----------------------------------------------------------------------------
BgComponentData* GetBgComponentData( btl::bg::bgsys_far_typeID farTypeId, btl::bg::bgsys_near_typeID nearTypeId, BgComponentData* pOut );

//----------------------------------------------------------------------------
/**
 *	@brief  バトル背景　アトリビュートIDとゾーンIDから表示設定を取得する 
 *
 *  @param  attrId           アトリビュートID
 *  @param zone_data_loader  ゾーンデータローダー
 *  @param  zoneID           ゾーンID
 *	@param	pOut             設定をセットするポインタ
 *	@return pOutで渡されたポインタ
 */
//-----------------------------------------------------------------------------
BgComponentData* GetBgComponentData( Field::Attr attrId, const Field::ZoneDataLoader* zone_data_loader, Field::ZoneID zoneId, BgComponentData* pOut );

//----------------------------------------------------------------------------
/**
 *	@brief  バトル背景　現在のゾーンデータ、プレイヤー位置から表示設定を取得する 
 *
 * @param fieldmap  フィールドマップ
 * @param zone_data_loader  ゾーンデータローダー
 * @param zoneId ゾーンID
 * @param pOut  設定をセットするポインタ
 */
//----------------------------------------------------------------------------
BgComponentData* GetBgComponentData( Field::Fieldmap* fieldmap, const Field::ZoneDataLoader* zone_data_loader, Field::ZoneID zoneId, BgComponentData* pOut );


GFL_NAMESPACE_END(btl)

#endif  // __SYSTEM_BACKGROUND_BGSYSUTILSETUP_H__
