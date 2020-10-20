//===================================================================
/**
 * @file    ZoneDataAccessor.cpp
 * @brief   ゾーンデータのアクセサー
 * @author  saita_kazuki
 * @date    2015.04.01
 */
//===================================================================

#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "./ZoneDataFormat.h"

// debug
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


#include "niji_conv_header/field/FieldGimmickTypes.h"


GFL_NAMESPACE_BEGIN( Field )

/**
 * @brief   コンストラクタ
 * @param   pZoneData ZONE_DATA構造体バイナリのポインタ
 */
ZoneDataAccessor::ZoneDataAccessor() :
  m_pZoneData( NULL)
{
}

ZoneDataAccessor::ZoneDataAccessor( void* pZoneData) :
  m_pZoneData( pZoneData)
{
}

/**
 * @brief   デストラクタ
 */
ZoneDataAccessor::~ZoneDataAccessor()
{
}

/**
 * @brief   アクセスするデータを設定
 * @param   pZoneData ZONE_DATA構造体バイナリのポインタ
 * @note    コンストラクタで設定した場合は呼ばなくてもOK
 */
void ZoneDataAccessor::SetData( void* pZoneData)
{
  m_pZoneData = pZoneData;
}

/**
 * @brief   データを取得できる状態か
 * @retval  true 有効
 * @retval  false 無効
 */
bool ZoneDataAccessor::IsEnable() const
{
  return ( m_pZoneData != NULL );
}

/**
 * @brief   BGM指定IDの取得
 */
u32 ZoneDataAccessor::GetBGMIDDay() const
{
  return this->GetZoneData()->bgm_id_day;
}
u32 ZoneDataAccessor::GetBGMIDNight() const
{
  return this->GetZoneData()->bgm_id_night;
}

/**
 * @brief   メッセージのアーカイブIDの取得
 */
u16 ZoneDataAccessor::GetMessageArcID() const
{
  return this->GetZoneData()->msg_id;
}

/**
 * @brief   タウンマップ上での表示位置グループの取得
 */
u16 ZoneDataAccessor::GetZoneGroup() const
{
  return this->GetZoneData()->zone_group;
}

/**
 * @brief   ギミックIDの取得
 */
u16 ZoneDataAccessor::GetGimmickID() const
{
  return this->GetZoneData()->gimmick_id;
}

/**
 * @brief   戦闘背景遠景IDの取得
 */
u16 ZoneDataAccessor::GetBattleFarBGID() const
{
  return this->GetZoneData()->battle_bg_far_type;
}

/**
 * @brief   マップ切り替え種別の取得
 */
u8 ZoneDataAccessor::GetMapChangeType() const
{
  return this->GetZoneData()->map_change_type;
}

/**
 * @brief   地名メッセージIDの取得
 */
u16 ZoneDataAccessor::GetPlaceLabelID() const
{
  return this->GetZoneData()->placename_id;
}

/**
 * brief  天候用environmentデータインデックスを取得
 * @return データインデックス
*/
u8 ZoneDataAccessor::GetWeatherEnvIndex() const
{
  return this->GetZoneData()->weather_env_index;
}

/**
 * @brief   マップ開始位置の取得
 */
void ZoneDataAccessor::GetStartPosition( gfl2::math::Vector3* pOutPos) const
{
  pOutPos->SetX( this->GetZoneData()->sx);
  pOutPos->SetY( this->GetZoneData()->sy);
  pOutPos->SetZ( this->GetZoneData()->sz);
}

/**
 * @brief   走るポケライドができるか
 */
bool ZoneDataAccessor::IsPokeRideRunEnable() const
{
  return this->GetZoneData()->pokeride_run_flag;
}

/**
 * @brief   そらをとぶポケライドができるか
 */
bool ZoneDataAccessor::IsPokeRideFlyEnable() const
{
  return this->GetZoneData()->pokeride_fly_flag;
}

/**
 * @brief   自転車でBGMが変わるか
 */
bool ZoneDataAccessor::IsBGMChangeEnable() const
{
  return this->GetZoneData()->bgm_change_flag;
}

/**
 * @brief   あなぬけができるか
 */
bool ZoneDataAccessor::IsEscapeEnable() const
{
  return this->GetZoneData()->escape_flag;
}

/**
 * @brief   天球が有効なゾーンか
 */
bool ZoneDataAccessor::IsSkyboxEnable() const
{
  return this->GetZoneData()->skybox_flag;
}


/**
 * @brief   リバーブが有効か
 */
bool ZoneDataAccessor::IsReverbEnable() const
{
  return this->GetZoneData()->reverb_flag;
}

/**
 * @brief   通信機能が使えるか
 */
bool ZoneDataAccessor::IsCommunicationEnable() const
{
  return this->GetZoneData()->communication_flag;
}
/**
 * @brief   脱出先を保存するか
 */
bool ZoneDataAccessor::IsEscapeSaveEnable() const
{
  return this->GetZoneData()->escape_save_flag;
}

/**
 * @brief   下画面が外マップ表示モードか
 * @retval true 外マップ
 * @retval false ダンジョン/室内
 */
bool ZoneDataAccessor::IsSubscreenOutsideMapMode() const
{
  return this->GetZoneData()->subscreen_map_mode_flag;
}

/**
 * @brief   下画面をスリープにするか
 * @retval true する
 * @retval false しない
 */
bool ZoneDataAccessor::IsSubscreenSleep() const
{
  return this->GetZoneData()->subscreen_sleep;
}

/**
 * @brief   下画面座標設定がタウンマップ用か
 * @retval true タウンマップ用
 * @retval false それ以外
 */
bool ZoneDataAccessor::IsTownmapMode() const
{
  return this->GetZoneData()->townmap_mode;
}

/**
 * @brief   動作モデルの表示数制御が有効か
 * @retval true 有効
 * @retval false 無効
 */
bool ZoneDataAccessor::IsMoveModelDrawNumControl() const
{
  return this->GetZoneData()->movemodel_draw_num_control;
}

/**
 * @brief   往来の描画優先度を下げる制御が有効か
 * @retval true 有効
 * @retval false 無効
 */
bool ZoneDataAccessor::IsDispLowPriorityForTraffic() const
{
  return this->GetZoneData()->disp_low_priority_for_traffic;
}

/**
 * @brief   エリアの環境データパックのどれを使うかの番号を取得
 */
u8 ZoneDataAccessor::GetEnvironmentIndex() const
{
  return this->GetZoneData()->environment_index;
}

/**
* @brief   このゾーンがデフォルトで使用するカメラデータのデータ番号を取得(デフォルトデータローカル)
*/
u8 ZoneDataAccessor::GetCameraDefaultIndex() const
{
  return this->GetZoneData()->camera_default_index;
}

/**
* @brief   このゾーンがデフォルトで使用するカメラサポートデータのデータ番号を取得(デフォルトデータローカル)
*/
u8 ZoneDataAccessor::GetCameraSupportDefaultIndex() const
{
  return this->GetZoneData()->camera_support_default_index;
}

/**
* @brief   島番号を取得
* @return 1オリジンの島番号。島1なら1。1～５まで
*/
u8 ZoneDataAccessor::GetIslandIndex() const
{
  return this->GetZoneData()->island_index;
}

/**
* @brief  アトリビュートのバトル近景IDパターンを取得
* @return バトル近景IDパターン
* @note 取得したパターンはアトリビュートからバトル近景IDを取得する際に使用する
*/
u16 ZoneDataAccessor::GetAttributeBgNearPattern() const
{
  return this->GetZoneData()->attr_bg_near_pattern;
}

/**
 * @brief   保持しているポインタをZONE_DATA構造体のポインタにして返す
 */
ZONE_DATA* ZoneDataAccessor::GetZoneData() const 
{
  GFL_ASSERT( m_pZoneData);
  return static_cast<ZONE_DATA*>( m_pZoneData);
}

/**
 * @brief  天候テーブル取得
 * @return 天候テーブル戦闘アドレス
 */
u32 *ZoneDataAccessor::GetWeatherTable() const
{
  return this->GetZoneData()->weather;
}

/**
* @brief  下画面基準位置X座標の取得
* @return 下画面基準位置X座標
*/
s32 ZoneDataAccessor::GetSubscreenBasisPositionX() const
{
  return this->GetZoneData()->subscreen_basis_pos_x;
}

/**
* @brief  下画面基準位置Z座標の取得
* @return 下画面基準位置Z座標
*/
s32 ZoneDataAccessor::GetSubscreenBasisPositionZ() const
{
  return this->GetZoneData()->subscreen_basis_pos_z;
}

/**
* @brief  下画面マップ種類の取得
* @return subscreen_map_type.hの値
*/
u8 ZoneDataAccessor::GetSubscreenMapType() const
{
  return this->GetZoneData()->subscreen_map_type;
}

/**
* @brief  下画面基準位置座標がどのゾーンIDかを取得
* @return ゾーンID
*/
u16 ZoneDataAccessor::GetTownmapBasisPosZone() const
{
  return this->GetZoneData()->townmap_basis_pos_zone;
}

/**
 * @brief   ポケモンセンターゾーンか？
 */
bool ZoneDataAccessor::IsPokemonCenter() const
{
  return (this->GetZoneData()->gimmick_id==GIMMICK_TYPE_POKECEN);
}

  /**
  * @brief   異世界人がゴーグルをつけるゾーンか？
  */
bool ZoneDataAccessor::IsChrGogglesOn() const
{
  return this->GetZoneData()->chr_goggles_on;
}

/**
* @brief   下画面ロトム変化の種別
*/
u8 ZoneDataAccessor::GetRottomEffectType() const
{
  return this->GetZoneData()->rottom_effect_type;
}

#if 0
// niji未使用
/**
 * @brief   ダッシュできるか
 */
bool ZoneDataAccessor::IsDashEnable() const
{
  return this->GetZoneData()->dash_flag;
}

/**
 * @brief   フラッシュが使えるか
 */
bool ZoneDataAccessor::IsFlashEnable() const
{
  return this->GetZoneData()->flash_flag;
}


/**
 * @brief   自機の吐息が有効か
 */
bool ZoneDataAccessor::IsBreathEnable() const
{
  return this->GetZoneData()->breath_flag;
}

/**
 * @brief   ダウジングポケライドが使えるか
 */
bool ZoneDataAccessor::IsPokeRideDowsingEnable() const
{
  return this->GetZoneData()->pokeride_dowsing_flag;
}


/**
 * @brief   ブルームが有効か
 */
bool ZoneDataAccessor::IsBloomEnable() const
{
  return this->GetZoneData()->bloom_flag;
}

/**
 * @brief   立体視が有効か
 */
bool ZoneDataAccessor::IsStereoDrawEnable() const
{
  return this->GetZoneData()->stereo_draw_flag;
}

/**
 * @brief   イベントデータのアーカイブIDの取得
 */
u16 ZoneDataAccessor::GetEventDataID() const
{
  return this->GetZoneData()->event_data_id;
}

/**
 * @brief   ゾーンの風景IDの取得。
 * @note    ポケモンの記憶関連
 */
u8 ZoneDataAccessor::GetZoneSceneryID() const
{
  return this->GetZoneData()->zone_scenery;
}

/**
 * @brief   地名メッセージウインドウ種別の取得
 */
u8 ZoneDataAccessor::GetPlaceNameType() const
{
  return this->GetZoneData()->placename_type;
}

/**
 * @brief   マップ種別指定IDの取得
 */
u8 ZoneDataAccessor::GetMapResourceID() const
{
  return this->GetZoneData()->maprsc_id;
}

#endif

GFL_NAMESPACE_END( Field )
