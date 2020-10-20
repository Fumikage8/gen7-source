//==============================================================================
/**
 @file    ZukanQRDisplay.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.06
 @brief   図鑑で行うQRコード表示
 */
//==============================================================================


// gfl2のインクルード
#include <util/include/gfl2_std_string.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppQRTextureCreateUtility.h>
#include <NetStatic/NetAppLib/include/QR/QRUtility.h>

#include "ZukanQRDisplay.h"

#include "ZukanDebugWin.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


ZukanQRDisplay::ZukanQRDisplay(
    gfl2::heap::HeapBase*   heap,
    gfl2::lyt::LytPicture*  picPane
)
  : m_picPane(picPane),
    m_QRCreateUtil(NULL),
    m_visible(false),
    m_state(STATE_NONE),
    m_pokeDataOfQR(NULL),
    m_dispSp(),
    m_createReq(false),
    m_reqPokeDataOfQR(NULL),
    m_reqDispSp()
{
  m_QRCreateUtil = GFL_NEW(heap) app::util::AppQRTextureCreateUtility(heap, ENCODE_DATA_BUFFER_SIZE);

  gfl2::std::MemClear(&m_qrVal, sizeof(m_qrVal));  // @fix NMCat[1176] 図鑑QRのハッシュ値が直前のものしか保存されていない  // つくる度に値が変わってしまわないようにゼロ初期化
  gfl2::std::MemClear(&m_reqQRVal, sizeof(m_reqQRVal));

  // 初期化
  m_picPane->SetVisible(m_visible);
}
ZukanQRDisplay::~ZukanQRDisplay()
{
  GFL_ASSERT(this->CanDestroy());  // 開発中に気付かせるためのASSERT。破棄できない状態のときに破棄しようとした。
  GFL_SAFE_DELETE(m_QRCreateUtil);
}

void ZukanQRDisplay::Update(void)
{
  m_QRCreateUtil->Update();

  // 生成状態m_stateを更新する
  for(s32 rep=0; rep<3; ++rep)  // 3回繰り返す
  {
    switch(m_state)
    {
    case STATE_NONE:
      {
        if(m_createReq)
        {
          static const u32 QR_VERSION   = 6;  // バージョン：６
          static const u32 QR_CELL_SIZE = 3;  // セルサイズ：３ (画像サイズは１４７px×１４７px)

          m_QRCreateUtil->CreateStart( reinterpret_cast<const u8*>( &m_reqQRVal ), sizeof(m_reqQRVal), QR_VERSION, QR_CELL_SIZE );
 
          m_state = STATE_CREATING;
          m_pokeDataOfQR = m_reqPokeDataOfQR;
          m_dispSp = m_reqDispSp;
          gfl2::std::MemCopy(&m_reqQRVal, &m_qrVal, sizeof(m_reqQRVal));
          
          m_createReq = false;
          m_reqPokeDataOfQR = NULL;
          m_reqDispSp       = PokeTool::SimpleParam();
          gfl2::std::MemClear(&m_reqQRVal, sizeof(m_reqQRVal));  // @fix NMCat[1176] 図鑑QRのハッシュ値が直前のものしか保存されていない  // つくる度に値が変わってしまわないようにゼロ初期化
          m_reqQRVal.monsNo = MONSNO_NULL;
        }
        break;
      }
    case STATE_CREATING:
      {
        gfl2::lyt::LytResTextureInfo* texInfo = m_QRCreateUtil->GetLytResTextureInfo();
        if( texInfo != NULL )
        {
          m_picPane->GetMaterial(0)->SetTexMap(0, texInfo);

          // リクエストが出ていないなら表示する
          if(!m_createReq)
          {
            if(m_visible)
            {
              m_picPane->SetVisible(true);
            }
          }
         
          m_state = STATE_CREATED;
        }
        break;
      }
    case STATE_CREATED:
    default:
      {
        if(m_createReq)
        {
          m_state = STATE_NONE;
        }
        break;
      }
    }
  }
}

bool ZukanQRDisplay::IsCreated(void) const
{
  return (m_state == STATE_CREATED && m_createReq == false);
}

bool ZukanQRDisplay::CanDestroy(void) const
{
  return ( !( m_QRCreateUtil->IsBusy() ) );
}

void ZukanQRDisplay::ReqCreateQR(gfl2::heap::HeapBase* tempHeap, const ZukanContents::PokeData* pokeData, const PokeTool::SimpleParam& dispSp, bool dispGenderMaleFemaleFlag, const u32* perRand/*[NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX]*/)  // tempHeapはこの関数内で一時的に使用するヒープ
{
  if(  m_pokeDataOfQR != pokeData
    || (
            m_dispSp.monsNo  != dispSp.monsNo 
         || m_dispSp.formNo  != dispSp.formNo 
         || m_dispSp.sex     != dispSp.sex    
         || m_dispSp.isRare  != dispSp.isRare 
         || m_dispSp.isEgg   != dispSp.isEgg  
         || m_dispSp.perRand != dispSp.perRand
       )
  )
  {
    m_reqPokeDataOfQR = pokeData;
    m_reqDispSp       = dispSp;

    {
      {
        gfl2::std::MemClear(&m_reqQRVal, sizeof(m_reqQRVal));  // @fix NMCat[1176] 図鑑QRのハッシュ値が直前のものしか保存されていない  // つくる度に値が変わってしまわないようにゼロ初期化
        m_reqQRVal.monsNo   = pokeData->monsNo;
        m_reqQRVal.dispForm = dispSp.formNo;
        m_reqQRVal.dispSex  = dispSp.sex;
        m_reqQRVal.dispRare = dispSp.isRare;
        m_reqQRVal.dispSexSame = (dispGenderMaleFemaleFlag)?(1):(0);  // @fix NMCat[831] 性別による違いがないポケモンの図鑑QRについての質問

        for(u32 j=0; j<NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX; ++j)
        {
          m_reqQRVal.rand[j] = perRand[j];
        }

        for(u32 i=0; i<pokeData->figureDataNum; ++i)
        {
          const ZukanContents::PokeData::FigureData* figureData = &(pokeData->figureData[i]);
          if(figureData->seeFlag)
          {
            bool seeFigureFlag[NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX] = { 0 };
            switch(figureData->figureGender)
            {
            case ZukanContents::PokeData::FIGURE_GENDER_MALE:
              {
                seeFigureFlag[(figureData->rareFlag)?(NetApp::QR::QR_ZUKAN::SEE_FLAG_M_RARE):(NetApp::QR::QR_ZUKAN::SEE_FLAG_MALE)] = true;
                break;  // @fix cov_ctr[12317][12318][12319]: break忘れ
              }
            case ZukanContents::PokeData::FIGURE_GENDER_FEMALE:
              {
                seeFigureFlag[(figureData->rareFlag)?(NetApp::QR::QR_ZUKAN::SEE_FLAG_F_RARE):(NetApp::QR::QR_ZUKAN::SEE_FLAG_FEMALE)] = true;
                break;  // @fix cov_ctr[12317][12318][12319]: break忘れ
              }
            case ZukanContents::PokeData::FIGURE_GENDER_UNKNOWN:
              {
                seeFigureFlag[(figureData->rareFlag)?(NetApp::QR::QR_ZUKAN::SEE_FLAG_M_RARE):(NetApp::QR::QR_ZUKAN::SEE_FLAG_MALE)] = true;
                break;  // @fix cov_ctr[12317][12318][12319]: break忘れ
              }
            case ZukanContents::PokeData::FIGURE_GENDER_MALE_FEMALE:
              {
                seeFigureFlag[(figureData->rareFlag)?(NetApp::QR::QR_ZUKAN::SEE_FLAG_M_RARE):(NetApp::QR::QR_ZUKAN::SEE_FLAG_MALE)] = true;
                seeFigureFlag[(figureData->rareFlag)?(NetApp::QR::QR_ZUKAN::SEE_FLAG_F_RARE):(NetApp::QR::QR_ZUKAN::SEE_FLAG_FEMALE)] = true;
                break;  // @fix cov_ctr[12317][12318][12319]: break忘れ
              }
            }
            for(u32 j=0; j<NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX; ++j)
            {
              if(seeFigureFlag[j])
              {
                m_reqQRVal.formBit[j] |= 1<<(figureData->formNo);
              }
            }
          }
        }
      }

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
      {
        s32 debugQROutOfRangeVal = 0;
#if PM_DEBUG
        debugQROutOfRangeVal = ZukanDebugWin::GetQROutOfRangeVal();
#endif
        switch(debugQROutOfRangeVal)
        {
        case 1: m_reqQRVal.monsNo += 1000; break;
        case 2:
          {
            for(u32 j=0; j<NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX; ++j)
            {
              for(u32 k=0; k<32; ++k)
              {
                m_reqQRVal.formBit[j] |= 1<<k;
              }
            }
            break;
          }
        case 3: m_reqQRVal.dispForm += 100; break;
        case 4: m_reqQRVal.dispSex += 10; break;
        case 5: m_reqQRVal.dispRare += 10; break;
        case 6: m_reqQRVal.dispSexSame += 10; break;
        case 7: m_reqQRVal.dispSexSame = 1; break;
        case 8: m_reqQRVal.dispSexSame = 0; break;
        }
      }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
      NetApp::QR::QRUtility::SetUpZukanQRData(tempHeap, &m_reqQRVal);
    }

    m_createReq = true;

    // リクエストを受け付けたので非表示にする
    m_picPane->SetVisible(false);
  }
}
void ZukanQRDisplay::SetVisible(bool visible)
{
  m_visible = visible;
  if(m_state == STATE_CREATED)
  {
    m_picPane->SetVisible(m_visible);
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

