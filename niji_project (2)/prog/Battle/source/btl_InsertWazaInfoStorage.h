//======================================================================
/**
 * @file btl_InsertWazaInfoStorage.h
 * @date 2016/05/19 16:44:24
 * @author taya
 * @brief 割り込みで追加されたワザアクションを実行する前の情報表示用パラメータ管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTL_INSERT_WAZA_INFO_STORAGE_H__
#define __BTL_INSERT_WAZA_INFO_STORAGE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "btl_HeManager.h"

GFL_NAMESPACE_BEGIN(btl)


  /**
   * @class InsertWazaInfoStorage
   * @brief クラスの説明
   */
  class InsertWazaInfoStorage
  {
      GFL_FORBID_COPY_AND_ASSIGN(InsertWazaInfoStorage);

  public:

    enum {
      //! パラメータ格納失敗時の無効ID
      ID_NULL = 0,
    };

    // ctor / dtor
    InsertWazaInfoStorage();
    ~InsertWazaInfoStorage();

    /**
     * @brief  パラメータを１件、格納する
     * @param[in]   actionParam    情報表示コマンドの生成時に参照するパラメータ
     * @return   格納ID（格納失敗時には無効値 ID_NULL ）
     */
    uint AddParam( const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION* actionParam );

    /**
     * @brief  格納したパラメータへのポインタを取得する
     * @param[in]   ID    AddParam の戻り値
     * @return   格納されたパラメータが見つかれば、そのポインタ／なければNULL
     */
    const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION*  GetParam( uint ID ) const;

    /**
     * @brief  格納したパラメータを削除
     * @param[in]   ID    AddParam の戻り値
     * @detail  STORAGE_SIZE分だけ格納するとそれ以上は格納できなくなる。そうなる前に適宜この関数を呼び出し情報を破棄する必要がある。
     */
    void RemoveParam( uint ID );

    /**
     * @brief   パラメータ格納領域の情報をクリアし、再度、１から格納することが可能な状態にする。
     * @detail  STORAGE_SIZE分だけ格納するとそれ以上は格納できなくなる。
     *          毎ターン呼び出すことで問答無用に全内容がRemoveParamされる。
     */
    void Clear( void );


  private:
    enum {
      //! 保存できるパラメータの数
      STORAGE_SIZE  =  16,  // これだけあれば充分。

      ID_MIN = ID_NULL + 1,
      ID_MAX = ID_MIN + STORAGE_SIZE - 1,
    };

    //! 保存パラメータ構造体
    typedef struct { 
      BTL_HANDEX_PARAM_INSERT_WAZA_ACTION   actionParam;
      bool                                  isEnable;
    }Storage;

    //! 保存先配列
    Storage    m_storage[ STORAGE_SIZE ];


    static bool isValidID( uint ID )
    {
      return (ID >= ID_MIN && ID <= ID_MAX);
    }
    static uint idToIdx( uint ID )
    {
      return ID - ID_MIN;
    }

  };

GFL_NAMESPACE_END(btl)

#endif // __BTL_INSERT_WAZA_INFO_STORAGE_H__