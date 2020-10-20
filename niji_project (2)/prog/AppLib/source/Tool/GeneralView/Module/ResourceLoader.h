#if !defined(RESOURCELOADER_H_INCLUDED)
#define RESOURCELOADER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   ResourceLoader.h
 * @date   2015/09/30 Wed. 11:45:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
/*  gfl  */
#include  <heap/include/gfl2_Heap.h>
#include  <fs/include/gfl2_Fs.h>


namespace app   {
  namespace util  {class Heap;}
namespace tool  {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @brief  ひとつのArcを読み込むローダ
  *  @note   arcのopen/closeを外部で行う場合はリクエスト時にisSkipOpenClose=trueとする
  *
     @code
        class TestClass {
          ResourceLoader m_Loader;
          
          void Initialize();
          void Update();
          void Finalize();
        };


        void TestClass::Initialize()
        {
          //  ロードリクエスト
          m_Loader.RequestLoad(pUtilHeap, arcID, arcDatID, false);
        
          if(isEnd)
          {
            void*  pData = m_Loader.GetData();
            //  do something
          }
        
          return isEnd;
        }


        void Update()
        {
          switch(state)
          {
            case STATE_Loading:
              if(m_Loader.ObserveLoad())
              {
                void* pData = m_Loader.GetData();
                // do something
                
                NextState();
              }
              break;
          }
        }


        bool Finalize()
        {
          m_Loader.ObserveLoad();   // 内部状態の更新
          // 解放
          // リクエスト前か、ロード完了状態であれば成功する
          return m_Loader.ReleaseData();
        }
     @endcode
  *
  *  @date   2015/09/30 Wed. 11:45:58
  */
//==============================================================================
/*  app::tool  */
class ResourceLoader
{
  GFL_FORBID_COPY_AND_ASSIGN(ResourceLoader);
public:
  ResourceLoader(void);
  ResourceLoader(app::util::Heap* pHeap);
  virtual ~ResourceLoader();
  
  bool    RequestLoad(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID, const bool isCompressedData = false, bool isSkipOpenClose = false);
  bool    RequestLoad(app::util::Heap* pHeap, const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID, const bool isCompressedData = false, bool isSkipOpenClose = false);
  bool    ObserveLoad(void);
  bool    IsLoadComplete(void) const;
  void*   GetData(void)                {return m_pData;}
  size_t  GetDataBytes(void) const     {return m_ReadBytes;}
  bool    ReleaseData(void);

  gfl2::fs::ArcFile::ARCID      GetArcID(void) const    {return m_ArcID;}
  gfl2::fs::ArcFile::ARCDATID   GetArcDatID(void) const {return m_ArcDatID;}

protected:
  app::util::Heap*              m_pHeap;
  void*                         m_pData;
  size_t                        m_ReadBytes;
  gfl2::fs::ArcFile::ARCID      m_ArcID;
  gfl2::fs::ArcFile::ARCDATID   m_ArcDatID;
  u32                           m_State;
  bool                          m_IsCompressedData;
  bool                          m_IsSkipOpenClose;
};





} /*  namespace tool  */
} /*  namespace app   */
#endif  /*  #if !defined(RESOURCELOADER_H_INCLUDED)  */
