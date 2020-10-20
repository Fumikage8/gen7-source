//===================================================================
/**
 * @file    ZoneDataLoaderTypes.h
 * @brief   ゾーンデータ読み込みクラスヘッダからZoneID等のタイプ定義のみ抜き出した
 * @author  Miyuki Iwasawa
 * @date    2015.09.16
 */
//===================================================================

#if !defined( ZONE_DATA_LOADER_TYPES_HINCLUDED )
#define ZONE_DATA_LOADER_TYPES_HINCLUDED
#pragma once


#include <niji_conv_header/field/zone/zone_id.h>


namespace Field
{

typedef u16 ZoneID;                           ///< ゾーンID型を定義

const ZoneID ZONEID_ERROR = ZONE_ID_INVALID;  ///< ゾーンIDの不正値

}

#endif // ZONE_DATA_LOADER_TYPES_HINCLUDED

