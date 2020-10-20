/**
* @brief  フィールド地形ブロックのLOD切り替えの通知を受ける基底クラス( 初期配置以降、動かないオブジェクト用 )
* @file   FieldTerrainBlockLodChangeStaticListener.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINLODCHANGESTATICLISTENER_H_INCLUDED__ )
#define __FIELDTERRAINLODCHANGESTATICLISTENER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>

// field
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodType.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );
GFL_NAMESPACE_BEGIN( Block );

/**
* @brief フィールド地形ブロックのLOD切り替えの通知を受ける基底クラス
* @note  初期配置以降、動かないオブジェクト用
*/
class LodChangeStaticListener
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( LodChangeStaticListener );

public:

  /*
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  LodChangeStaticListener( void ) :
    m_IsRegist( false )
    ,m_X( 0 )
    ,m_Z( 0 )
  {
  }

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~LodChangeStaticListener( void ){}

  /**
  * @brief 地形ブロックに登録された瞬間に実行される関数
  *
  * @param  lodType 登録時の地形のLOD状態
  *
  * @return 無し
  */
  virtual void RegistAfter( LodType::Value lodType ) = 0;
  
  /**
  * @brief 地形ブロックのLODが切り替わった瞬間に実行される関数
  *
  * @param  lodType 切り替え後のLOD状態
  *
  * @return 無し
  */
  virtual void ChangeLod( LodType::Value lodType ) = 0;

  /**
  * @brief 所属する地形ブロックを決定する座標を取得する
  *
  * @param  pOut　座標格納先  
  *
  * @return 成否
  */
  virtual b32 GetControlPosition( gfl2::math::Vector3* pOut ) const = 0; 


  //
  // 管理用関数、地形マネージャーのみ実行可
  //

  /**
  * @brief　レジスト情報を記憶
  *
  * @param  x  レジスト先
  * @param  z  レジスト先
  *
  * @return 無し
  */
  void registBlock( u32 x, u32 z )
  {
    m_IsRegist = true;
    m_X = x;
    m_Z = z;
  }

  /**
  * @brief  レジスト情報を破棄
  *
  * @return 無し
  */
  void unRegistBlock( void )
  {
    m_IsRegist = false;
    m_X = 0;
    m_Z = 0;
  }

  /**
  * @brief　レジスト済みか取得
  *
  * @return true レジスト済み
  */
  b32 isRegist( void ) const
  {
    return m_IsRegist;
  }

  /**
  * @brief　レジスト先を取得
  *
  * @return 無し
  */
  void getRegistBlock( u32* pX, u32* pZ ) const
  {
    *pX = m_X;
    *pZ = m_Z;
  }

private:

  b32 m_IsRegist;
  u32 m_X;
  u32 m_Z;

};  // class LodChangeStaticListener


GFL_NAMESPACE_END( Block );
GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINLODCHANGESTATICLISTENER_H_INCLUDED__