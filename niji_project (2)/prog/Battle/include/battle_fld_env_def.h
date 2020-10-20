//=======================================================================================
/**
 * @file battle_fld_env_def.h
 * @brief バトル内効果指定　定義
 * @date 2012.12.14
 */
//=======================================================================================
#if !defined(__BATTLE_FLD_ENV_DEF_H__)
#define __BATTLE_FLD_ENV_DEF_H__

//----------------------------------------------------------------------------
/**
 * @brief バトル内フィールド効果指定
 *
 * xy_doc/バトル関連/バトル基本仕様.xlsのシート「フィールド効果技」参照
 */
//----------------------------------------------------------------------------
enum BtlFldEnvId{
  BTLFLDENV_NORMAL = 0,       ///<室内その他
  BTLFLDENV_JIMEN,            ///<通常地面
  BTLFLDENV_CAVE,             ///<洞窟
  BTLFLDENV_GRASS,            ///<草
  BTLFLDENV_WATER,            ///<水上
  BTLFLDENV_MOOR,             ///<湿原
  BTLFLDENV_SNOW,             ///<雪原
  BTLFLDENV_ICE,              ///<氷
  
  //@バトル変更箇所
  BTLFLDENV_VOLCANO,          ///<火山
  BTLFLDENV_CEMETERY,         ///<墓地
  BTLFLDENV_CONTEST,          ///<コンテスト中
  BTLFLDENV_SKYPOLE,          ///<空の柱
  BTLFLDENV_UNIVERSE,         ///<デオキシス戦


  BTLFLDENV_NUM,              ///<総数
};

#endif  // __BATTLE_FLD_ENV_DEF_H__
