//=============================================================================
/**
 * @brief 技データ
 * @file pml_WazaData.h
 * @author obata_toshihiro
 * @date 2011.05.06
 */
//=============================================================================
#ifndef __PML_WAZADATA_H__
#define __PML_WAZADATA_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <niji_conv_header/poke_lib/wazano_def.h>
#include <pml/include/waza/pml_WazaDataConst.h>

///@namespace pml
namespace pml {
  ///
  namespace wazadata { 

    struct WAZA_DATA;


    //---------------------------------------------------------------------
    /**
     * @class WazaData
     * @brief 技データを扱うためのクラス
     */
    //---------------------------------------------------------------------
    class WazaData
    {
      GFL_FORBID_COPY_AND_ASSIGN( WazaData ); 

      public: 
      static void Initialize( gfl2::heap::HeapBase* heap, const char* garc_path );
      static void Finalize( void );
      private:
      static WAZA_DATA* m_alldata;
      static const char* m_garcPath;

      public:
      WazaData( gfl2::heap::HeapBase* heap );
      virtual ~WazaData(); 
      void LoadData( WazaNo wazano );
      void CopyData( WazaData* dest ) const; 
      WazaNo GetWazaNo( void ) const;
      s32 GetParam( ParamID paramID ) const;
      bool GetFlag( WazaFlag flag ) const;

      private:
      WazaNo m_wazano;
      WAZA_DATA* m_data;
    }; 


  }
}

#endif //__PML_WAZADATA_H__
