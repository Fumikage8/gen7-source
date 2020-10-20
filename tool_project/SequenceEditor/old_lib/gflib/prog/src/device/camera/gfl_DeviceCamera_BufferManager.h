#ifndef __gfl_DeviceCamera_BufferManager__
#define __gfl_DeviceCamera_BufferManager__
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_BufferManager.h
 * @brief  カメラが使用するバッファの管理クラス
 * @author obata_toshihiro
 * @date   2010.11.19
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h> // for GFL_FORBID_COPY_AND_ASSIGN

namespace gfl {
  namespace device { 
    namespace camera {

      class BufferManager 
      {
        GFL_FORBID_COPY_AND_ASSIGN( BufferManager ); // コピーコンストラクタ, 代入演算子を禁止

        public:
        /**
         * @brief セットアップパラメータ
         */
        struct SetupParam {
          gfl::heap::HeapBase* pHeap;         // バッファの確保に使用するヒープ
          u8                   yuvBufferNum;  // YUV バッファの数
          u32                  yuvBufferSize; // YUV バッファのサイズ
          u32                  rgbBufferSize; // RGB バッファのサイズ
        }; 
        BufferManager( const SetupParam& param ); 
        virtual ~BufferManager(); 


        public:
        // バッファ取得
        void* GetYuvBuffer_Receiving( void ) const; // 受信中の YUV バッファ
        void* GetYuvBuffer_Latest( void ) const;    // 最新の YUV バッファ
        void* GetYuvBuffer_Reading( void ) const;   // 読み取り中の YUV バッファ
        void* GetRgbBuffer_Receiving( void ) const; // 受信中の RGB バッファ
        void* GetRgbBuffer_Latest( void ); // 最新の RGB バッファ
        const void* GetRgbBuffer_Latest( void ) const; // 最新の RGB バッファ

        // バッファサイズ取得
        inline u32 GetYuvBufferSize( void ) const { return m_yuvBufferSize; }
        inline u32 GetRgbBufferSize( void ) const { return m_rgbBufferSize; }

        // バッファの切り替え
        void SwitchReceivingYuvBuffer( void ); // 受信する YUV バッファを切り替える
        void SwitchReadingYuvBuffer( void );   // 読み取る YUV バッファを切り替える
        void SwapRgbBuffers( void );           // RGB バッファを入れ替える


        protected: 
        u8 m_yuvBufferNum;       // YUV バッファの数
        void** m_yuvBuffers;     // YUV バッファ配列
        u32 m_yuvBufferSize;     // YUV バッファのサイズ
        u8 m_yuvReceivingBufIdx; // 受信中の YUV バッファのインデックス
        u8 m_yuvLatestBufIdx;    // 最新の YUV バッファのインデックス
        u8 m_yuvReadingBufIdx;   // 読み取り中の YUV バッファのインデックス

        static const u8 RGB_BUFFER_NUM = 2;   // RGB バッファの数
        void* m_rgbBuffers[ RGB_BUFFER_NUM ]; // RGB バッファ配列
        u32 m_rgbBufferSize;                  // RGB バッファのサイズ
        u8 m_rgbLatestBufIdx;                 // 最新の RGB バッファのインデックス

        // YUV バッファ切り替え時のクリティカルセクション
        // 受信バッファと読み取りバッファの切り替えが同時に行われると,
        // 受信, 読み取りが同じバッファに対して行われる危険性があるため,
        // YUV バッファの切り替えは排他的に実行する.
        nn::os::CriticalSection m_criticalSectionForYuvBufferSwitch; 


        private: 
        void AllocYuvBuffers( const SetupParam& param );
        void AllocRgbBuffers( const SetupParam& param );
        void FreeYuvBuffers( void );
        void FreeRgbBuffers( void );
      }; 

    } // namespace camera
  } // namespace device
} // namespace gfl
#endif
