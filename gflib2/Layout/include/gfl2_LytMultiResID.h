﻿//=============================================================================
/**
 * @file    gfl2_LytMultiResID.h
 * @brief   レイアウトリソースID
 * @author  Hiroyuki Nakamura
 * @date    2014.12.19
 *
 *  MultiArcResourceAccessorの対応のため、
 *    typedef u32 LytResID;
 *  から変更。
 *
 *  <grp/g2d/gfl_LytSys.h>
 *
 *  typedef struct {
 *    u16 lyt_resource_max; // 管理リソース最大数
 *    u16 lyt_wk_max;       // 管理LytWk最大数
 *  }LytSysSetUp;
 *
 *  parentID:
 *    LytSysSertUP.lyt_resource_max個作成されたリソースの番号
 *
 *  multiID:
 *    parentIDのリソースが持つMultiArcResourceAccessorに登録したARCの番号
 *  
 */
//=============================================================================
#if !defined( GFL2_LYTMULTIRESID_H_INCLUDED )
#define GFL2_LYTMULTIRESID_H_INCLUDED

#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//=============================================================================
/**
 * @class   LytMultiResID
 * @brief   レイアウトリソースID
 * @date    2014.12.19
 */
//=============================================================================
class LytMultiResID
{
public:
  static const u16 NULL_ID = 0xffff;   //!< 未登録ID


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  LytMultiResID(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~LytMultiResID();

  //-----------------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //-----------------------------------------------------------------------------
  void Init(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   IDが設定されているか
   *
   * @retval  "true = 設定されている"
   * @retval  "false = それ以外"
   */
  //-----------------------------------------------------------------------------
  bool IsEnable(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   親IDを取得
   *
   * @return  親ID
   */
  //-----------------------------------------------------------------------------
  u16 GetParentID(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   親IDを設定
   *
   * @param   id  セットするID
   */
  //-----------------------------------------------------------------------------
  void SetParentID( u16 id );

  //-----------------------------------------------------------------------------
  /**
   * @brief   登録IDを取得
   *
   * @return  登録ID
   */
  //-----------------------------------------------------------------------------
  u16 GetMultiID(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   登録IDを設定
   *
   * @param   id  セットするID
   */
  //-----------------------------------------------------------------------------
  void SetMultiID( u16 id );


private:
  u16 parentID;   //!< 親ID
  u16 multiID;    //!< 登録ID
};


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

#endif  // GFL2_LYTMULTIRESID_H_INCLUDED
