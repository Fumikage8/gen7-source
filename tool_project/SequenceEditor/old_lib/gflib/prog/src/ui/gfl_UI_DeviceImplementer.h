//=============================================================================
/**
 * @file   gfl_UI_DeviceImplementer.h
 * @brief  デバイス入力を検知するクラスのインターフェース
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_DEVICEIMPLEMENTER__
#define __GFL_UI_DEVICEIMPLEMENTER__

namespace gfl {
  namespace ui {


    class DeviceImplementer 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( DeviceImplementer );

      public:
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      //-----------------------------------------------------------------------
      DeviceImplementer( void ){}

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~DeviceImplementer(){}

      //-----------------------------------------------------------------------
      /**
       * @brief メイン処理
       */
      //-----------------------------------------------------------------------
      virtual void Main( void ) = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief 検出データを取得する
       *
       * @param[out] p_buffer  取得したデータの格納先 
       *
       * @note 
       * 派生クラスは自身が扱うデバイスの検出データを返す.
       * 自身が扱うデバイスの検出データは, そのインターフェースクラスが定義する.
       */
      //-----------------------------------------------------------------------
      virtual void GetDetectiveData( void* p_buffer ) const = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief 検出データのサイズを取得する
       *
       * @note
       * 派生クラスは自身が扱うデバイスの検出データのサイズを返す.
       * 自身が扱うデバイスの検出データは, そのインターフェースクラスが定義する.
       */
      //-----------------------------------------------------------------------
      virtual u32 GetDetectiveDataSize( void ) const = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief 実効データのサイズを取得する
       *
       * @note
       * 派生クラスは自身が扱うデバイスの実効データのサイズを返す.
       * 自身が扱うデバイスの実効データは, そのインターフェースクラスが定義する.
       */
      //-----------------------------------------------------------------------
      virtual u32 GetEffectiveDataSize( void ) const = 0; 
    };


  } //namespace ui
} //namespace gfl

#endif //__GFL_UI_DEVICEIMPLEMENTER__
