/**
* @brief  ポケモンふれあい用データアクセッサー
* @file   FieldDataAccessor.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDPOKEMONFUREAI_H_INCLUDED__ )
#define __FIELDPOKEMONFUREAI_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#if 0
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#endif

// field
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTypes.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( PokemonFureai );

/**
* @brief ポケモンふれあい用データアクセッサー
*/
class DataAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( DataAccessor );

public:

  struct Header
  {
    s32 version;
    s32 data_num;
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  DataAccessor( void* pData );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~DataAccessor( void );
  
#if 0
  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );
  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );
#endif

  /**
  * @brief 設定データを取得
  *
  * @param modelId 取得するポケモンのモデルID
  *
  * @return 設定データへのポインタ
  */
  const POKEMON_SETTING_DATA* GetSettingData( s32 modelId );

  /**
  * @brief 鳴き声ラベルを取得
  */
  u32 GetVoiceID( s32 modelId, s32 type );

#if PM_DEBUG
  void Dump( void );
#endif

private:


  //
  // 制御
  //
  void*                       m_pData;
  Header*                     m_pHeaderData;
  POKEMON_SETTING_DATA*       m_pBodyData;
  //
  // 内部オブジェクト
  //

  //
  // 外部オブジェクト
  //

  //
  // デバッグ用
  //
#if PM_DEBUG
#endif // PM_DEBUG

};  // class FieldDataAccessor

GFL_NAMESPACE_END( PokemonFureai );
GFL_NAMESPACE_END( Field );

#endif // __FIELDPOKEMONFUREAI_H_INCLUDED__
