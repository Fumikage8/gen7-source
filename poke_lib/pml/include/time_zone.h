//=====================================================
/**
 * @brief  時間帯の定義
 * @file   time_zone.h
 * @author obata_toshihiro
 * @date   2011.01.27  WBより移植
 */
//=====================================================
#ifndef __PML_TIME_ZONE_H__ 
#define __PML_TIME_ZONE_H__ 
#pragma once 

namespace pml {


enum TimeZone {
  TIMEZONE_MORNING =	0,
  TIMEZONE_NOON =		  1,
  TIMEZONE_EVENING =	2,
  TIMEZONE_NIGHT =		3,
  TIMEZONE_MIDNIGHT =	4, 
  TIMEZONE_MAX,
};


} // namespace pml


#endif  // __PML_TIME_ZONE_H__
