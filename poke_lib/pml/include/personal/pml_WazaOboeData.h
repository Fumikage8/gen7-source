//==================================================================================
/**
 * @brief  ポケモン技覚えデータのアクセッサ
 * @file   pml_WazaOboeData.h
 * @author obata_toshihiro
 * @date   2010.12.20
 */
//==================================================================================
#ifndef __PML_WAZAOBOEDATA_H__
#define __PML_WAZAOBOEDATA_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <pml/include/pml_Type.h> 

namespace pml { 
  namespace personal {

    struct WazaOboeTable;
    class PersonalData;
    

    //---------------------------------------------------------------------
    /**
     * @class WazaOboeData
     * @brief 技覚えデータアクセサークラス
     */
    //---------------------------------------------------------------------
    class WazaOboeData
    {
      GFL_FORBID_COPY_AND_ASSIGN( WazaOboeData ); 

      public:
      static void Initialize( gfl2::heap::HeapBase* heap, const char* garc_path );
      static void Finalize( void );
      private:
      static PersonalData* s_PersonalData;
      static const char* s_garcPath;



      public:

      /**
       * @brief 覚え技の種類
       */
      enum OboeWazaKind
      {
        OBOEWAZA_KIND_EVOLVE,  // 進化習得技
        OBOEWAZA_KIND_BASE,    // 基本習得技
        OBOEWAZA_KIND_LEVEL,   // レベル習得技
        OBOEWAZA_KIND_NUM,
      };


      public:
      WazaOboeData( gfl2::heap::HeapBase* heap );
      ~WazaOboeData( void );
      void LoadData( MonsNo monsno, FormNo formno );
      bool StartAsyncRead( MonsNo monsno, FormNo formno, gfl2::heap::HeapBase * p_temp_heap );
      bool WaitAsyncReadFinish( void );
      void CopyData( WazaOboeData* dest ) const;
      MonsNo GetMonsNo( void ) const;
      FormNo GetFormNo( void ) const;
      u8 GetValidCodeNum( void ) const;                     // データの要素数
      u16 GetLevel( u16 data_index ) const;                 // 技を習得するレベル
      WazaNo GetWazaNo( u16 data_index ) const;             // 技No.
      OboeWazaKind GetOboeWazaKind( u16 data_index ) const; // 覚え技の種類

      private:
      MonsNo m_monsno;
      FormNo m_formno;
      WazaOboeTable* m_data;
      u8 m_validCodeNum;

      u32 GetDataID( MonsNo monsno, FormNo formno ) const;
      u8 CountValidWazaOboeCode( void ) const;
    };


  } // namespace personal
} // namespace pml

#endif //__PML_WAZAOBOEDATA_H__
