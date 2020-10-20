//=============================================================================
/**
 * @file    btl_ZenryokuWazaStatus.h
 * @brief   全力技の使用状況を管理するクラス
 * @author  obata_toshihiro
 * @date    2015.05.27
 */
//=============================================================================
#if !defined( BTL_ZENRYOKUWAZASTATUS_H_INCLUDED )
#define BTL_ZENRYOKUWAZASTATUS_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


namespace btl {

  class MainModule;
  class BTL_POKEPARAM;


  /**
   * @class ZenryokuWazaStatus
   * @brief 1クライアントについての、全力技の使用状況を管理するクラス
   */
  class ZenryokuWazaStatus
  {
  public:

    /**
     * @brief セットアップパラメータ
     */
    struct SetupParam
    {
      bool haveZenryokuRing;  // クライアントが「ぜんりょくリング」を持っているか？
    };
  
    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param setupParam  セットアップパラメータ
     */
    //-------------------------------------------------------------------------
    ZenryokuWazaStatus( const SetupParam& setupParam );
  
    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    virtual ~ZenryokuWazaStatus();

    //-------------------------------------------------------------------------
    /**
     * @brief 初期状態に戻す
     */
    //-------------------------------------------------------------------------
    void Clear( void );
  
    //-------------------------------------------------------------------------
    /**
     * @brief 全力技を使用したか？
     * @retval true   すでに全力技を使用した
     * @retval false  まだ全力技を使用していない
     */
    //-------------------------------------------------------------------------
    bool IsZenryokuWazaUsed( void ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 全力技を使用済みであることを登録する
     */
    //-------------------------------------------------------------------------
    void SetZenryokuWazaUsed( void );

    //-------------------------------------------------------------------------
    /**
     * @brief 全力技として使用できる技が存在するか？
     * @param pokeParam  技を使用するポケモン
     * @retval true   全力技を使用できる
     * @retval false  全力技を使用できない
     */
    //-------------------------------------------------------------------------
    bool CanUseZenryokuWaza( const BTL_POKEPARAM& pokeParam ) const;
  
    //-------------------------------------------------------------------------
    /**
     * @brief 全力技を使用できるか？
     * @param pokeParam  技を使用するポケモン
     * @param wazaIndex  使用する技のインデックス
     * @retval true   全力技を使用できる
     * @retval false  全力技を使用できない
     */
    //-------------------------------------------------------------------------
    bool CanUseZenryokuWaza( const BTL_POKEPARAM& pokeParam, u8 wazaIndex ) const;

    //-------------------------------------------------------------------------
    /**
     * @brief 全力技に変換後の技No.を取得する
     * @param pokeParam  技を使用するポケモン
     * @param wazaIndex  使用する技のインデックス
     * @return 全力技に変換後の技No.
     * @retval WAZANO_NULL  全力技に変換できない場合
     */
    //-------------------------------------------------------------------------
    WazaNo GetZenryokuWazaNo( const BTL_POKEPARAM& pokeParam, u8 wazaIndex ) const;

    //-------------------------------------------------------------------------
    /**
    * @brief 二度目の全力技を使用したか？
    * @retval true   すでに全力技を使用した（又は二度目の技使用不可）
    * @retval false  まだ全力技を使用していない
    */
    //-------------------------------------------------------------------------
    bool IsSecondZenryokuWazaUsed(void) const;

    //-------------------------------------------------------------------------
    /**
    * @brief 二度目の全力技を使用済みであることを登録する
    */
    //-------------------------------------------------------------------------
    void SetSecondZenryokuWazaUsed(void);

    //-------------------------------------------------------------------------
    /**
    * @brief 二度目の全力技を使用可能であることを登録する
    */
    //-------------------------------------------------------------------------
    void SetCanSecondZenryokuWaza(void);
  
  private:
  
    bool m_haveZenryokuRing;     // ぜんりょくリングを持っているか？
    bool m_isZenryokuWazaUsed;   // 全力技を使用したか？
    bool m_isSecondZenryokuWazaUsed;// 二度目の全力技を使用したか？
    bool m_isCanSecondZenryokuWaza; // 二度目の全力技を使用可能か？
  
  };


} // namespace btl

#endif // BTL_ZENRYOKUWAZASTATUS_H_INCLUDED