//=======================================================================================
/**
 * @file BgSetupUtil.cpp
 * @brief バトル背景、簡易指定処理
 * @author ikuta_junya
 *         ariizumi_nobuhiko(niji移植15.09.09)
 * @date 2012.08.08
 */
//=======================================================================================
#include "Battle/Background/include/BgSetupUtil.h"

#include "niji_conv_header/battle/background/bgsys_far_type.cdat"
#include "niji_conv_header/battle/background/bgsys_near_type.cdat"
#include "niji_conv_header/battle/background/bgsys_env.cdat"

#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldRo/include/Fieldmap.h"

GFL_NAMESPACE_BEGIN(btl)
  
BgComponentData* GetBgComponentData( btl::bg::bgsys_far_typeID farTypeId, btl::bg::bgsys_near_typeID nearTypeId, BgComponentData* pOut )
{
  // 引数チェック
  
  //@todo 今回この例外は必要か？
  //NearTypeIdがNULLの時FarTypeIdと同じ値を入れる。
  /*
  NearTypeId nearTypeId = tempNearTypeId;
  if( nearTypeId == NEAR_TYPE_NONE )
  {
    nearTypeId = static_cast<NearTypeId>(farTypeId);
  }
  */

  //背景モデルの種類
  pOut->groudnType = static_cast<btl::bg::bgsys_normal_groundID>(btl::bg::bgsys_near_type[nearTypeId][0]);
  pOut->nearType = static_cast<btl::bg::bgsys_near_groundID>(btl::bg::bgsys_far_type[farTypeId][1]);
  pOut->farType = static_cast<btl::bg::bgsys_far_groundID>(btl::bg::bgsys_far_type[farTypeId][2]);

  //天球の有無
  pOut->useSkyBox = btl::bg::bgsys_far_type[farTypeId][0];

  //環境の値
  btl::bg::bgsys_envID envId = static_cast<btl::bg::bgsys_envID>(btl::bg::bgsys_near_type[nearTypeId][1]);
  //ロジックに影響のある項目
  pOut->enableDarkBall      = btl::bg::bgsys_far_type[farTypeId][3];          //ダークボール有効
  pOut->enableDiveBall      = btl::bg::bgsys_env_env[envId][5];          //ダイブボール有効
  pOut->sizennotikaraWazaNo = btl::bg::bgsys_env_env[envId][1];     //「しぜんのちから」で出る技
  pOut->himitinotikaraEffect= btl::bg::bgsys_env_env[envId][4];    //「ひみつのちから」で出るエフェクト(他の技)
  pOut->himitinotikaraType  = btl::bg::bgsys_env_env[envId][3];      //「ひみつのちから」の効果
  pOut->minomuttiFormNo     = btl::bg::bgsys_env_env[envId][0];         //ミノムッチが変化するフォルム
  pOut->hogoshokuType       = btl::bg::bgsys_env_env[envId][2];           //保護色で変わるタイプ

  return pOut;
}

//----------------------------------------------------------------------------
/**
 *	@brief  バトル背景　アトリビュートIDとゾーンIDから表示設定を取得する 
 *
 *  @param  attrId           アトリビュートID
 *  @param  zone_data_loader ゾーンデータローダー
 *  @param  zoneID           ゾーンID
 *	@param	pOut             設定をセットするポインタ
 *	@return pOutで渡されたポインタ
 */
//-----------------------------------------------------------------------------
BgComponentData* GetBgComponentData( Field::Attr attrId, const Field::ZoneDataLoader* zone_data_loader, Field::ZoneID zoneId, BgComponentData* pOut )
{
  Field::ZoneDataAccessor accessor( zone_data_loader->GetZoneData(zoneId));
  btl::bg::bgsys_far_typeID farType     = static_cast<btl::bg::bgsys_far_typeID>( accessor.GetBattleFarBGID());
  btl::bg::bgsys_near_typeID nearType   = static_cast<btl::bg::bgsys_near_typeID>( Field::Attribute::GetBattleNearBGID(attrId, accessor.GetAttributeBgNearPattern()));

  return GetBgComponentData( farType, nearType, pOut );
}



//----------------------------------------------------------------------------
/**
 *	@brief  バトル背景　現在のプレイヤー位置から表示設定を取得する 
 *
 * @param fieldmap  フィールドマップ
 * @param zone_data_loader  ゾーンデータローダー
 * @param zoneId ゾーンID
 * @param pOut  設定をセットするポインタ
 */
//----------------------------------------------------------------------------
BgComponentData* GetBgComponentData( Field::Fieldmap* fieldmap, const Field::ZoneDataLoader* zone_data_loader, Field::ZoneID zoneId, BgComponentData* pOut )
{
  Field::MoveModel::FieldMoveModelPlayer* player = fieldmap->GetPlayerCharacter();
  Field::Attr now_attr = player->GetGroundAttributeLastSuccesful();

  Field::ZoneDataAccessor accessor( zone_data_loader->GetZoneData(zoneId));
  btl::bg::bgsys_far_typeID farType     = static_cast<btl::bg::bgsys_far_typeID>( accessor.GetBattleFarBGID());
  btl::bg::bgsys_near_typeID nearType   = static_cast<btl::bg::bgsys_near_typeID>( Field::Attribute::GetBattleNearBGID(now_attr, accessor.GetAttributeBgNearPattern()));

  return GetBgComponentData( farType, nearType, pOut );
}

GFL_NAMESPACE_END(btl)

