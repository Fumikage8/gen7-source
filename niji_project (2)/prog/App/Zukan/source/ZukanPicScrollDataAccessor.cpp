//==============================================================================
/**
 @file    ZukanPicScrollDataAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の絵巻データのアクセッサー
 @note    zukan_pic_scroll_data_converter.rbが生成したzukan_pic_scroll_data.binの中身にアクセスする
 */
//==============================================================================

// gfl2のインクルード
#include <debug/include/gfl2_Assert.h>

// 図鑑のインクルード
#include "ZukanPicScrollDataAccessor.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


const u32 ZukanPicScrollDataAccessor::PLACE_POS_NUM[PLACE_TYPE_NUM] =
{
  3,  // PLACE_TYPE_GRP000_EVO3     
  2,  // PLACE_TYPE_GRP001_EVO2     
  1,  // PLACE_TYPE_GRP002_EVO1     
  2,  // PLACE_TYPE_GRP003_PAIR2    
  3,  // PLACE_TYPE_GRP004_COMP_EBO3
  4,  // PLACE_TYPE_GRP005_COMP_EBO4
  9,  // PLACE_TYPE_GRP006_COMP_EBO9
  4,  // PLACE_TYPE_GRP007_EVO4     
  2,  // PLACE_TYPE_GRP008_LGD2     
  2,  // PLACE_TYPE_GRP009_LGD_PAIR2
  1,  // PLACE_TYPE_GRP010_LGD1     
  1,  // PLACE_TYPE_GRP011_EVO1
};

void ZukanPicScrollDataAccessor::SetupData(const void* buf)
{
  m_buf = buf;

  // アクセスしやすくしておく
  m_alolaNo_end = *(reinterpret_cast<const u16*>(&((static_cast<const u8*>(m_buf))[2])));
  m_startByte   = static_cast<const u32*>(m_buf);
}

void ZukanPicScrollDataAccessor::GetPokeData(u32 alolaNo, PokeData* pokeData) const
{
  const PokeDataHeader* pokeDataHeader = this->getPokeDataHeader(alolaNo);
  pokeData->placeGroupTopAlolaNo = pokeDataHeader->placeGroupTopAlolaNo;
  pokeData->placeType            = static_cast<PlaceType>(pokeDataHeader->placeType);
  pokeData->placePos             = pokeDataHeader->placePos;
  pokeData->gradeVal             = static_cast<GradeVal>(pokeDataHeader->gradeVal);
  if(pokeData->gradeVal == GRADE_VAL_UB)
  {
    pokeData->gradeVal = GRADE_VAL_QUASI_LEGEND;
    pokeData->ubFlag   = true;
    pokeData->tikiFlag = false;
    pokeData->maboroshiFlag = false;
  }
  else if(pokeData->gradeVal == GRADE_VAL_TIKI)  // @fix NMCat[1107] タイプ：ヌル、シルヴァディの図鑑表示についての質問  →準伝説とティキを分けたので、ティキがUBのような扱いになった。
  {
    pokeData->gradeVal = GRADE_VAL_QUASI_LEGEND;
    pokeData->ubFlag   = false;
    pokeData->tikiFlag = true;
    pokeData->maboroshiFlag = false;
  }
  else if(pokeData->gradeVal == GRADE_VAL_MABOROSHI)  // @fix GFNMCat[3983] まぼろしポケモン用の評価コメントが表示されない  →まぼろしを伝説と分ける。
  {
    pokeData->gradeVal = GRADE_VAL_LEGEND;
    pokeData->ubFlag   = false;
    pokeData->tikiFlag = false;
    pokeData->maboroshiFlag = true;
  }
  else
  {
    pokeData->ubFlag   = false;
    pokeData->tikiFlag = false;
    pokeData->maboroshiFlag = false;
  }
}

void ZukanPicScrollDataAccessor::GetPokeTransData(u32 alolaNo, pml::FormNo formNo, pml::Sex sex, TransData* transData) const
{
  u32 index = this->getPokeFormDataIndex(alolaNo, formNo, sex);
  const TransDataS32* transDataS32 = this->getPokeTransDataS32(alolaNo);

  transData->pokeX = transDataS32[index].pokeX / 100.0f;
  transData->pokeY = transDataS32[index].pokeY / 100.0f;
  transData->ballX = transDataS32[index].ballX / 100.0f;
  transData->ballY = transDataS32[index].ballY / 100.0f;
  transData->nameX = transDataS32[index].nameX / 100.0f;
  transData->nameY = transDataS32[index].nameY / 100.0f;
}

u32 ZukanPicScrollDataAccessor::GetPlaceGroupNum(void) const
{
  u32 groupNum = 0;
  u32 placeGroupTopAlolaNo = 0;
  for(u32 alolaNo=1; alolaNo<=m_alolaNo_end; ++alolaNo)
  {
    PokeData pokeData;
    this->GetPokeData(alolaNo, &pokeData);
    if(placeGroupTopAlolaNo != pokeData.placeGroupTopAlolaNo)
    {
      GFL_ASSERT(placeGroupTopAlolaNo < pokeData.placeGroupTopAlolaNo);  // 開発中に気付かせるためのASSERT。だんだん大きくなっていかなければならない。
      ++groupNum;
      placeGroupTopAlolaNo = pokeData.placeGroupTopAlolaNo;
    }
  }
  return groupNum;
}
u32 ZukanPicScrollDataAccessor::GetPlaceGroupNo(u32 alolaNo) const
{
  u32 placeGroupNo = PLACE_GROUP_NO_NONE;

  if( 1<=alolaNo && alolaNo<=m_alolaNo_end )
  {
    u32 groupNum = 0;
    u32 placeGroupTopAlolaNo = 0;
    for(u32 alolaNoTmp=1; alolaNoTmp<=alolaNo; ++alolaNoTmp)
    {
      PokeData pokeData;
      this->GetPokeData(alolaNoTmp, &pokeData);
      if(placeGroupTopAlolaNo != pokeData.placeGroupTopAlolaNo)
      {
        GFL_ASSERT(placeGroupTopAlolaNo < pokeData.placeGroupTopAlolaNo);  // 開発中に気付かせるためのASSERT。だんだん大きくなっていかなければならない。
        ++groupNum;
        placeGroupTopAlolaNo = pokeData.placeGroupTopAlolaNo;
      }
    }
    if(groupNum > 0) placeGroupNo = groupNum -1;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
 
  return placeGroupNo;
}

const ZukanPicScrollDataAccessor::PokeDataHeader* ZukanPicScrollDataAccessor::getPokeDataHeader(u32 alolaNo) const
{
  if( m_buf != NULL )
  {
    if( !( 1<=alolaNo && alolaNo<=m_alolaNo_end ) )
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      alolaNo = 1;
    }
    const void* addr = &( (static_cast<const u8*>(m_buf))[ m_startByte[alolaNo] ] );
    return static_cast<const PokeDataHeader*>(addr);
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
  return NULL;
}

const u8* ZukanPicScrollDataAccessor::getPokeFormData(u32 alolaNo) const
{
  const PokeDataHeader* pokeDataHeader = this->getPokeDataHeader(alolaNo);
  return static_cast<const u8*>(&(pokeDataHeader->formDataTop));
}

u32 ZukanPicScrollDataAccessor::getPokeFormDataIndex(u32 a_alolaNo, pml::FormNo a_formNo, pml::Sex a_sex) const
{
  u32 retIndex = 0;  // 戻り値  // どれも一致しなかったら0番を返す

  s32 matchIndex = -1;  // あったら0以上
  s32 formNoMatchIndex = -1;  // あったら0以上
  s32 sexMatchIndex    = -1;  // あったら0以上

  const PokeDataHeader* pokeDataHeader = this->getPokeDataHeader(a_alolaNo);
  const u8* formData = this->getPokeFormData(a_alolaNo);

  // 一番近い容姿情報のインデックスを決める
  for(u8 index=0; index<pokeDataHeader->formNum; ++index)
  {
    u32 formNo = ( formData[index] & FORM_DATA_BIT_FIELD_MASK_FORM_NO ) >> FORM_DATA_BIT_FIELD_SHIFT_FORM_NO;
    u32 gender = ( formData[index] & FORM_DATA_BIT_FIELD_MASK_GENDER  ) >> FORM_DATA_BIT_FIELD_SHIFT_GENDER ;

    // フォルム番号一致
    bool formNoMatch = false;
    if(formNo == FORM_NO_NOT_SET)
    {
      if(formNoMatchIndex == -1) formNoMatchIndex = index;  // 先に出て来たもの勝ち
      formNoMatch = true;
    }
    else if(formNo == a_formNo)
    {
      if(formNoMatchIndex == -1) formNoMatchIndex = index;  // 先に出て来たもの勝ち
      formNoMatch = true;
    }
    
    // 性別一致
    bool sexMatch = false;
    if(gender == GENDER_PROG_VAL_NOT_SET)
    {
      if(sexMatchIndex == -1) sexMatchIndex = index;  // 先に出て来たもの勝ち
      sexMatch = true;
    }
    else if(static_cast<s32>(gender) == a_sex)
    {
      if(sexMatchIndex == -1) sexMatchIndex = index;  // 先に出て来たもの勝ち
      sexMatch = true;
    }

    // 両方一致か？
    if(formNoMatch && sexMatch)
    {
      matchIndex = index;
      break;
    }
  }

  // 一致したものがないとき
  if(matchIndex == -1)
  {
    if(formNoMatchIndex >= 0)  // フォルム番号一致を優先
    {
      matchIndex = formNoMatchIndex; 
    }
    else if(sexMatchIndex >= 0)
    {
      matchIndex = sexMatchIndex; 
    }
  }
  
  // 戻り値
  if(matchIndex >= 0)
  {
    retIndex = matchIndex;
  }
  
  return retIndex;
}

const ZukanPicScrollDataAccessor::TransDataS32* ZukanPicScrollDataAccessor::getPokeTransDataS32(u32 alolaNo) const
{
  const PokeDataHeader* pokeDataHeader = this->getPokeDataHeader(alolaNo);
  
  u32 paddingByte = ( ( 4 - (pokeDataHeader->formNum % 4) ) +2 ) % 4;
  // | formNum | paddingByte | 4-(formNum-2) | formNum%4 | 4-(formNum%4) |
  // |    0    |      2      |       6       |     0     |      4        |
  // |    1    |      1      |       5       |     1     |      3        |
  // |    2    |      0      |       4       |     2     |      2        |
  // |    3    |      3      |       3       |     3     |      1        |
  // |    4    |      2      |       2       |     0     |      4        |
  // |    5    |      1      |       1       |     1     |      3        |
  // |    6    |      0      |       0       |     2     |      2        |
  // |    7    |      3      |      -1(+4=3) |     3     |      1        |
  // |    8    |      2      |      -2(+4=2) |     0     |      4        |

  const u8* formData = this->getPokeFormData(alolaNo);
  return reinterpret_cast<const TransDataS32*>(&(formData[pokeDataHeader->formNum + paddingByte]));
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

