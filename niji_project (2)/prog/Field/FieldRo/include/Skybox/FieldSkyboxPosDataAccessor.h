//======================================================================
/**
 * @file FieldSkyboxPosPosDataAccessor.h
 * @date 2016/2/19
 * @author saita_kazuki
 * @brief 天球位置データアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_SKYBOX_DATA_ACCESSOR_H_INCLUDED__
#define __FIELD_SKYBOX_DATA_ACCESSOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


// 前方参照
namespace Field {
  class EventWork;
}
namespace GameSys {
  class GameManager;
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球位置データアクセサー
 */
class PosDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( PosDataAccessor );

public:

  /**
   * @brief コンストラクタ
   * @param pData バイナリデータ
   */
  PosDataAccessor( void* pData);

  /**
   * @brief デストラクタ
   */
  virtual ~PosDataAccessor();

  /**
   *  @brief データセット
   *  @param pData バイナリデータ
   */
  void SetData( void* pData);

  /**
   *  @brief データが存在するか
   *  @param worldID ワールドID
   *  @retval true 存在する
   *  @retval false 存在しない
   */
  bool IsExistData( u16 worldID);

  /**
   *  @brief 位置Yの取得
   *  @param worldID ワールドID
   *  @return Y座標
   *  @note データが存在しなければ0を返す
   */
  f32 GetPositionY( u16 worldID);

private:

GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  /**
   * @brief ヘッダーデータ
   */
  struct HeaderData
  {
    u32   num;              ///< データ数
  };

  /**
   * @brief コアデータ
   */
  struct CoreData
  {
    f32     posY;
    u16     worldID;
    u8      padding[2];
  };

  /**
   * @brief バイナリ全体
   */
  struct Data
  {
    HeaderData header;      ///< ヘッダー
    CoreData   data[];      ///< コアデータ配列
  };

GFL_WARNING_WIN32_POP


private:

  CoreData* GetCoreData( u16 worldID) const ;

private:

  Data*     m_pData;

};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_SKYBOX_DATA_ACCESSOR_H_INCLUDED__
