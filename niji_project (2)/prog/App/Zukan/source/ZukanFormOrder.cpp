//==============================================================================
/**
 @file    ZukanFormOrder.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.03.26
 @brief   図鑑におけるフォルムの特別な並び順
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFormOrder.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


bool ZukanFormOrder::GetFormOrder(const MonsNo monsNo, FormOrder* formOrder)
{
  struct MonsNoFormOrder
  {
    MonsNo    monsNo;
    FormOrder formOrder;
  };

  static const MonsNoFormOrder MONS_NO_FORM_ORDER_TABLE[] =
  {                                                                                                                                                                                                         //  地方  新旧      系統                  フォルム順番  注釈
    { MONSNO_KORATTA    , { { FORMNO_KORATTA_ALOLA   , FORMNO_KORATTA_NORMAL   ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  19    //   15   アローラ  コラッタ：アローラ    1 0           １＝アローラ
    { MONSNO_RATTA      , { { FORMNO_RATTA_ALOLA     , FORMNO_RATTA_NUSHI      , FORMNO_RATTA_NORMAL     ,                                                    } , FORM_ORDER_TYPE_ALOLA     , 3 , 2 } },  //  20    //   16   アローラ  ラッタ：アローラ      1 0           １＝アローラ
    { MONSNO_RAITYUU    , { { FORMNO_RAITYUU_ALOLA   , FORMNO_RAITYUU_NORMAL   ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  26    //   26   アローラ  ライチュウ：アローラ  1 0           １＝アローラ
    { MONSNO_NYAASU     , { { FORMNO_NYAASU_ALOLA    , FORMNO_NYAASU_NORMAL    ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  52    //   43   アローラ  ニャース：アローラ    1 0           １＝アローラ
    { MONSNO_PERUSIAN   , { { FORMNO_PERUSIAN_ALOLA  , FORMNO_PERUSIAN_NORMAL  ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  53    //   44   アローラ  ペルシアン：アローラ  1 0           １＝アローラ
    { MONSNO_BETOBETAA  , { { FORMNO_BETOBETAA_ALOLA , FORMNO_BETOBETAA_NORMAL ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  88    //   48   アローラ  ベトベター：アローラ  1 0           １＝アローラ
    { MONSNO_BETOBETON  , { { FORMNO_BETOBETON_ALOLA , FORMNO_BETOBETON_NORMAL ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  89    //   49   アローラ  ベトベトン：アローラ  1 0           １＝アローラ
    { MONSNO_DHIGUDA    , { { FORMNO_DHIGUDA_ALOLA   , FORMNO_DHIGUDA_NORMAL   ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  50    //   71   アローラ  ディグダ：アローラ    1 0           １＝アローラ
    { MONSNO_DAGUTORIO  , { { FORMNO_DAGUTORIO_ALOLA , FORMNO_DAGUTORIO_NORMAL ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  51    //   72   アローラ  ダグトリオ：アローラ  1 0           １＝アローラ
    { MONSNO_MITUSUI    , { { FORMNO_MITUSUI_CHEER   , FORMNO_MITUSUI_HULA     , FORMNO_MITUSUI_FLAMENCO , FORMNO_MITUSUI_NICHIBU  ,                          } , FORM_ORDER_TYPE_ISLAND    , 4 , 0 } },  // 741    //   82   新        ミツスイ              1 2 0 3       島の順番に合わせて
    { MONSNO_GARAGARA   , { { FORMNO_GARAGARA_ALOLA  , FORMNO_GARAGARA_NORMAL  , FORMNO_GARAGARA_NUSHI ,                                                      } , FORM_ORDER_TYPE_ALOLA     , 3 , 1 } },  // 105    //  164   アローラ  ガラガラ：アローラ    1 0           １＝アローラ
    { MONSNO_ZIGARUDE   , { { FORMNO_ZIGARUDE_10PER  , FORMNO_ZIGARUDE_SW10PER , FORMNO_ZIGARUDE_50PER   , FORMNO_ZIGARUDE_SW50PER , FORMNO_ZIGARUDE_100PER , } , FORM_ORDER_TYPE_OTHER     , 5 , 0 } },  // 718    //  205   旧        ジガルデ              1 2 0 3 4     １０％→５０％→パーフェクト
    { MONSNO_ISITUBUTE  , { { FORMNO_ISITUBUTE_ALOLA , FORMNO_ISITUBUTE_NORMAL ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  74    //  229   アローラ  イシツブテ：アローラ  1 0           １＝アローラ
    { MONSNO_GOROON     , { { FORMNO_GOROON_ALOLA    , FORMNO_GOROON_NORMAL    ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  75    //  230   アローラ  ゴローン：アローラ    1 0           １＝アローラ
    { MONSNO_GOROONYA   , { { FORMNO_GOROONYA_ALOLA  , FORMNO_GOROONYA_NORMAL  ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  76    //  231   アローラ  ゴローニャ：アローラ  1 0           １＝アローラ
    { MONSNO_SANDO      , { { FORMNO_SANDO_ALOLA     , FORMNO_SANDO_NORMAL     ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  27    //  251   アローラ  サンド：アローラ      1 0           １＝アローラ
    { MONSNO_SANDOPAN   , { { FORMNO_SANDOPAN_ALOLA  , FORMNO_SANDOPAN_NORMAL  ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  28    //  252   アローラ  サンドパン：アローラ  1 0           １＝アローラ
    { MONSNO_ROKON      , { { FORMNO_ROKON_ALOLA     , FORMNO_ROKON_NORMAL     ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  37    //  253   アローラ  ロコン：アローラ      1 0           １＝アローラ
    { MONSNO_KYUUKON    , { { FORMNO_KYUUKON_ALOLA   , FORMNO_KYUUKON_NORMAL   ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  //  38    //  254   アローラ  キュウコン：アローラ  1 0           １＝アローラ
    { MONSNO_KARANAKUSI , { { FORMNO_KARANAKUSI_EAST , FORMNO_KARANAKUSI_WEST  ,                                                                              } , FORM_ORDER_TYPE_DIRECTION , 2 , 1 } },  // 422    //  260   旧        カラナクシ            1 0           東の海→西の海
    { MONSNO_TORITODON  , { { FORMNO_TORITODON_EAST  , FORMNO_TORITODON_WEST   ,                                                                              } , FORM_ORDER_TYPE_DIRECTION , 2 , 1 } },  // 423    //  261   旧        トリトドン            1 0           東の海→西の海
    { MONSNO_NASSII     , { { FORMNO_NASSII_ALOLA    , FORMNO_NASSII_NORMAL    ,                                                                              } , FORM_ORDER_TYPE_ALOLA     , 2 , 1 } },  // 103    //  270   アローラ  ナッシー：アローラ    1 0           １＝アローラ    
  };

  static const s32 TABLE_ELEM_NUM = sizeof(MONS_NO_FORM_ORDER_TABLE)/sizeof(MONS_NO_FORM_ORDER_TABLE[0]);

  bool ret = false;
  for(s32 i=0; i<TABLE_ELEM_NUM; ++i)
  {
    if(MONS_NO_FORM_ORDER_TABLE[i].monsNo == monsNo)
    {
      *formOrder = MONS_NO_FORM_ORDER_TABLE[i].formOrder;
      ret = true;
      break;
    }
  }
  return ret;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

