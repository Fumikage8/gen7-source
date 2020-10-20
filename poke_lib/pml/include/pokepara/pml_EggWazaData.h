//=============================================================================
/**
 * @file   pml_EggWazaData.h
 * @date   2015/10/01 10:38:56
 * @author obata_toshihiro
 * @brief  タマゴ技データ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined PML_EGGWAZADATA_H_INCLUDED
#define PML_EGGWAZADATA_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <pml/include/pml_Type.h>

#include <niji_conv_header/poke_lib/monsno_def.h>
#include <niji_conv_header/poke_lib/wazano_def.h>


GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( pokepara )


  /**
   * @class EggWazaData
   * @brief タマゴ技データを扱うクラス
   */
  class EggWazaData
  {
    GFL_FORBID_COPY_AND_ASSIGN( EggWazaData );

  public:

    // タマゴ技の最大数
    static const u32 MAX_EGG_WAZA_NUM = 31;

    /**
     * @brief コンストラクタ
     * @param heap  バッファの確保に使用するヒープ
     */
    EggWazaData( gfl2::heap::HeapBase* heap );

    /**
     * @brief デストラクタ
     */
    virtual ~EggWazaData();

    /**
     * @brief タマゴ技データを読み込む
     * @param arcFileName  アーカイブファイルの名前
     * @param monsno       読み込み対象モンスターの番号
     * @param formno       読み込み対象フォルムの番号
     */
    void Load( const char* arcFileName, MonsNo monsno, FormNo formno );

    /**
     * @brief タマゴ技の数を取得する
     */
    u32 GetEggWazaNum( void ) const;

    /**
     * @brief タマゴ技を取得する
     * @param eggWazaIndex  タマゴ技のインデックス
     */
    WazaNo GetEggWazaNo( u32 eggWazaIndex ) const;





  private:


    /**
     * @brief タマゴ技データ
     */
    struct TamagoWazaData
    {
      u16 anotherFormHeadDataID;                    // 0x00 別フォルムの先頭データID
      u16 wazaNum;                                  // 0x02 タマゴ技の数
      u16 wazano[ EggWazaData::MAX_EGG_WAZA_NUM ];  // 0x04 タマゴ技
    };

    bool m_isLoaded;
    TamagoWazaData m_buffer;


    static u32 GetDataID( MonsNo monsno, FormNo formno );
    static void LoadData( TamagoWazaData* buffer, u32 dataID );

  };


GFL_NAMESPACE_END( pokepara )
GFL_NAMESPACE_END( pml )

#endif // PML_EGGWAZADATA_H_INCLUDED