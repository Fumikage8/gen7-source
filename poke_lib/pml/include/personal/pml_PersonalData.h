//=============================================================================
/**
 * @brief  ポケモンパーソナルデータのアクセッサ
 * @file   pml_PersonalData.h
 * @author obata_toshihiro
 * @date   2010.12.21
 */
//=============================================================================
#ifndef __PML_PERSONALDATA_H__
#define __PML_PERSONALDATA_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_PersonalConst.h> 


namespace pml { 
  namespace personal {

    struct BasicParams;


    //---------------------------------------------------------------------
    /**
     * @class PersonalData
     * @brief ポケモン種族データのアクセサークラス
     */
    //---------------------------------------------------------------------
    class PersonalData
    {
      GFL_FORBID_COPY_AND_ASSIGN( PersonalData ); 

      public: 
      static void Initialize( gfl2::heap::HeapBase* heap, const char* garc_path );
      static void Finalize( void );
      static u32 GetDataID( MonsNo monsno, FormNo formno );
      //  地方番号関係はprojectのpoke_tool関係に移動しました。
      //static u16 * CreateZenkokuToChihouTable( gfl2::heap::HeapBase* heap, size_t *size );
      private:
      static const char* m_garcPath;
      static BasicParams * m_alldata;

      public:
      PersonalData( gfl2::heap::HeapBase* heap );
      virtual ~PersonalData(); 
      void LoadData( MonsNo monsno, FormNo formno );
      void CopyData( PersonalData* dest ) const; 
      MonsNo GetMonsNo( void ) const;
      FormNo GetFormNo( void ) const;
      u32 GetParam( ParamID paramId ) const;         // 指定したパラメータを取得する
      SexType GetSexType( void ) const;              // 性別ベクトルのタイプを取得する
      bool CheckWazaMachine( u32 machineNo ) const;  // 技マシンで技を覚えるか？
      bool CheckWazaOshie(u32 oshieNo) const;      // 技教えで技を覚えるか？
      bool CheckBpWazaOshie(u32 wazaNo) const; // BP技教えで技を覚えるか？

      private:
      MonsNo m_monsno;
      FormNo m_formno;
      BasicParams* m_data; 
    };


  } // namespace personal
} // namespace pml

#endif //__PML_PERSONALDATA_H__
