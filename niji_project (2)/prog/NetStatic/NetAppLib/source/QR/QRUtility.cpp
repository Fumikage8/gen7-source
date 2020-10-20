//======================================================================
/**
 * @file QRUtility.cpp
 * @brief QR系ユーティリティ
 */
//======================================================================

#include "../../include/QR/QRUtility.h"
#include <System/include/PokemonVersion.h>
#include <pml/include/pmlib.h>
#include <crypto/include/gfl2_ISHA256Calculator.h>
#include "System/include/ThreadPriority.h"
#include <NetStatic\NetAppLib\include\Util\NetAppCommonSaveUtility.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/ZukanSave.h"
#include "Savedata/include/MyStatus.h"

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>
#include "Savedata/include/EventWork.h"
#ifdef  GF_PLATFORM_CTR
#include <zorro.h>
#endif

#include <PokeTool/include/PokeToolExtendData.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QR)

static const u16 QR_POKE_TABLE[QRUtility::QR_POKE_TABLE_NUM][QRUtility::QR_POKE_TABLE_ELEM] = 
{
 { 29, 1 , 0 , 7560 },
 { 30, 1 , 0 , 7560 },
 { 31, 1 , 0 , 7560 },
 { 32, 0 , 0 , 7560 },
 { 33, 0 , 0 , 7560 },
 { 34, 0 , 0 , 7560 },
 { 43, 0 , 0 , 3780 },
 { 43, 1 , 0 , 3780 },
 { 44, 0 , 0 , 3780 },
 { 44, 1 , 0 , 3780 },
 { 45, 0 , 0 , 3780 },
 { 45, 1 , 0 , 3780 },
 { 48, 0 , 0 , 3780 },
 { 48, 1 , 0 , 3780 },
 { 49, 0 , 0 , 3780 },
 { 49, 1 , 0 , 3780 },
 { 69, 0 , 0 , 3780 },
 { 69, 1 , 0 , 3780 },
 { 70, 0 , 0 , 3780 },
 { 70, 1 , 0 , 3780 },
 { 71, 0 , 0 , 3780 },
 { 71, 1 , 0 , 3780 },
 { 77, 0 , 0 , 3780 },
 { 77, 1 , 0 , 3780 },
 { 78, 0 , 0 , 3780 },
 { 78, 1 , 0 , 3780 },
 { 83, 0 , 0 , 3780 },
 { 83, 1 , 0 , 3780 },
 { 84, 0 , 0 , 3780 },
 { 84, 1 , 0 , 3780 },
 { 85, 1 , 0 , 3780 },
 { 85, 0 , 0 , 3780 },
 { 98, 0 , 0 , 3780 },
 { 98, 1 , 0 , 3780 },
 { 99, 0 , 0 , 3780 },
 { 99, 1 , 0 , 3780 },
 { 100, 2 , 0 , 7560 },
 { 101, 2 , 0 , 7560 },
 { 106, 0 , 0 , 7560 },
 { 107, 0 , 0 , 7560 },
 { 109, 0 , 0 , 3780 },
 { 109, 1 , 0 , 3780 },
 { 110, 0 , 0 , 3780 },
 { 110, 1 , 0 , 3780 },
 { 111, 0 , 0 , 3780 },
 { 111, 1 , 0 , 3780 },
 { 112, 0 , 0 , 3780 },
 { 112, 1 , 0 , 3780 },
 { 114, 0 , 0 , 3780 },
 { 114, 1 , 0 , 3780 },
 { 116, 0 , 0 , 3780 },
 { 116, 1 , 0 , 3780 },
 { 117, 0 , 0 , 3780 },
 { 117, 1 , 0 , 3780 },
 { 152, 0 , 0 , 3780 },
 { 152, 1 , 0 , 3780 },
 { 153, 0 , 0 , 3780 },
 { 153, 1 , 0 , 3780 },
 { 154, 0 , 0 , 3780 },
 { 154, 1 , 0 , 3780 },
 { 155, 0 , 0 , 3780 },
 { 155, 1 , 0 , 3780 },
 { 156, 0 , 0 , 3780 },
 { 156, 1 , 0 , 3780 },
 { 157, 0 , 0 , 3780 },
 { 157, 1 , 0 , 3780 },
 { 158, 0 , 0 , 3780 },
 { 158, 1 , 0 , 3780 },
 { 159, 0 , 0 , 3780 },
 { 159, 1 , 0 , 3780 },
 { 160, 0 , 0 , 3780 },
 { 160, 1 , 0 , 3780 },
 { 161, 0 , 0 , 3780 },
 { 161, 1 , 0 , 3780 },
 { 162, 0 , 0 , 3780 },
 { 162, 1 , 0 , 3780 },
 { 175, 0 , 0 , 3780 },
 { 175, 1 , 0 , 3780 },
 { 176, 0 , 0 , 3780 },
 { 176, 1 , 0 , 3780 },
 { 182, 0 , 0 , 3780 },
 { 182, 1 , 0 , 3780 },
 { 183, 0 , 0 , 3780 },
 { 183, 1 , 0 , 3780 },
 { 184, 0 , 0 , 3780 },
 { 184, 1 , 0 , 3780 },
 { 187, 0 , 0 , 3780 },
 { 187, 1 , 0 , 3780 },
 { 188, 0 , 0 , 3780 },
 { 188, 1 , 0 , 3780 },
 { 189, 0 , 0 , 3780 },
 { 189, 1 , 0 , 3780 },
 { 191, 0 , 0 , 3780 },
 { 191, 1 , 0 , 3780 },
 { 192, 0 , 0 , 3780 },
 { 192, 1 , 0 , 3780 },
 { 201, 2 , 0 , 270 },
 { 201, 2 , 1 , 270 },
 { 201, 2 , 2 , 270 },
 { 201, 2 , 3 , 270 },
 { 201, 2 , 4 , 270 },
 { 201, 2 , 5 , 270 },
 { 201, 2 , 6 , 270 },
 { 201, 2 , 7 , 270 },
 { 201, 2 , 8 , 270 },
 { 201, 2 , 9 , 270 },
 { 201, 2 , 10 , 270 },
 { 201, 2 , 11 , 270 },
 { 201, 2 , 12 , 270 },
 { 201, 2 , 13 , 270 },
 { 201, 2 , 14 , 270 },
 { 201, 2 , 15 , 270 },
 { 201, 2 , 16 , 270 },
 { 201, 2 , 17 , 270 },
 { 201, 2 , 18 , 270 },
 { 201, 2 , 19 , 270 },
 { 201, 2 , 20 , 270 },
 { 201, 2 , 21 , 270 },
 { 201, 2 , 22 , 270 },
 { 201, 2 , 23 , 270 },
 { 201, 2 , 24 , 270 },
 { 201, 2 , 25 , 270 },
 { 201, 2 , 26 , 270 },
 { 201, 2 , 27 , 270 },
 { 202, 0 , 0 , 3780 },
 { 202, 1 , 0 , 3780 },
 { 203, 0 , 0 , 3780 },
 { 203, 1 , 0 , 3780 },
 { 207, 0 , 0 , 3780 },
 { 207, 1 , 0 , 3780 },
 { 211, 0 , 0 , 3780 },
 { 211, 1 , 0 , 3780 },
 { 213, 0 , 0 , 3780 },
 { 213, 1 , 0 , 3780 },
 { 216, 0 , 0 , 3780 },
 { 216, 1 , 0 , 3780 },
 { 217, 0 , 0 , 3780 },
 { 217, 1 , 0 , 3780 },
 { 220, 0 , 0 , 3780 },
 { 220, 1 , 0 , 3780 },
 { 221, 0 , 0 , 3780 },
 { 221, 1 , 0 , 3780 },
 { 230, 0 , 0 , 3780 },
 { 230, 1 , 0 , 3780 },
 { 231, 0 , 0 , 3780 },
 { 231, 1 , 0 , 3780 },
 { 232, 0 , 0 , 3780 },
 { 232, 1 , 0 , 3780 },
 { 234, 0 , 0 , 3780 },
 { 234, 1 , 0 , 3780 },
 { 236, 0 , 0 , 7560 },
 { 237, 0 , 0 , 7560 },
 { 261, 0 , 0 , 3780 },
 { 261, 1 , 0 , 3780 },
 { 262, 0 , 0 , 3780 },
 { 262, 1 , 0 , 3780 },
 { 263, 0 , 0 , 3780 },
 { 263, 1 , 0 , 3780 },
 { 264, 0 , 0 , 3780 },
 { 264, 1 , 0 , 3780 },
 { 265, 0 , 0 , 3780 },
 { 265, 1 , 0 , 3780 },
 { 266, 0 , 0 , 3780 },
 { 266, 1 , 0 , 3780 },
 { 267, 0 , 0 , 3780 },
 { 267, 1 , 0 , 3780 },
 { 268, 0 , 0 , 3780 },
 { 268, 1 , 0 , 3780 },
 { 269, 0 , 0 , 3780 },
 { 269, 1 , 0 , 3780 },
 { 285, 0 , 0 , 3780 },
 { 285, 1 , 0 , 3780 },
 { 286, 0 , 0 , 3780 },
 { 286, 1 , 0 , 3780 },
 { 287, 0 , 0 , 3780 },
 { 287, 1 , 0 , 3780 },
 { 288, 0 , 0 , 3780 },
 { 288, 1 , 0 , 3780 },
 { 289, 0 , 0 , 3780 },
 { 289, 1 , 0 , 3780 },
 { 290, 0 , 0 , 3780 },
 { 290, 1 , 0 , 3780 },
 { 291, 0 , 0 , 3780 },
 { 291, 1 , 0 , 3780 },
 { 292, 2 , 0 , 7560 },
 { 293, 0 , 0 , 3780 },
 { 293, 1 , 0 , 3780 },
 { 294, 0 , 0 , 3780 },
 { 294, 1 , 0 , 3780 },
 { 295, 0 , 0 , 3780 },
 { 295, 1 , 0 , 3780 },
 { 298, 0 , 0 , 3780 },
 { 298, 1 , 0 , 3780 },
 { 300, 0 , 0 , 3780 },
 { 300, 1 , 0 , 3780 },
 { 301, 1 , 0 , 3780 },
 { 301, 0 , 0 , 3780 },
 { 311, 0 , 0 , 3780 },
 { 311, 1 , 0 , 3780 },
 { 312, 0 , 0 , 3780 },
 { 312, 1 , 0 , 3780 },
 { 313, 0 , 0 , 7560 },
 { 314, 1 , 0 , 7560 },
 { 315, 0 , 0 , 3780 },
 { 315, 1 , 0 , 3780 },
 { 316, 0 , 0 , 3780 },
 { 316, 1 , 0 , 3780 },
 { 317, 0 , 0 , 3780 },
 { 317, 1 , 0 , 3780 },
 { 322, 0 , 0 , 3780 },
 { 322, 1 , 0 , 3780 },
 { 323, 0 , 0 , 3780 },
 { 323, 1 , 0 , 3780 },
 { 331, 0 , 0 , 3780 },
 { 331, 1 , 0 , 3780 },
 { 332, 0 , 0 , 3780 },
 { 332, 1 , 0 , 3780 },
 { 335, 0 , 0 , 3780 },
 { 335, 1 , 0 , 3780 },
 { 336, 0 , 0 , 3780 },
 { 336, 1 , 0 , 3780 },
 { 337, 2 , 0 , 7560 },
 { 338, 2 , 0 , 7560 },
 { 355, 0 , 0 , 3780 },
 { 355, 1 , 0 , 3780 },
 { 356, 0 , 0 , 3780 },
 { 356, 1 , 0 , 3780 },
 { 358, 0 , 0 , 3780 },
 { 358, 1 , 0 , 3780 },
 { 360, 0 , 0 , 3780 },
 { 360, 1 , 0 , 3780 },
 { 363, 0 , 0 , 3780 },
 { 363, 1 , 0 , 3780 },
 { 364, 0 , 0 , 3780 },
 { 364, 1 , 0 , 3780 },
 { 365, 0 , 0 , 3780 },
 { 365, 1 , 0 , 3780 },
 { 396, 0 , 0 , 3780 },
 { 396, 1 , 0 , 3780 },
 { 397, 0 , 0 , 3780 },
 { 397, 1 , 0 , 3780 },
 { 398, 0 , 0 , 3780 },
 { 398, 1 , 0 , 3780 },
 { 399, 0 , 0 , 3780 },
 { 399, 1 , 0 , 3780 },
 { 400, 0 , 0 , 3780 },
 { 400, 1 , 0 , 3780 },
 { 401, 0 , 0 , 3780 },
 { 401, 1 , 0 , 3780 },
 { 402, 0 , 0 , 3780 },
 { 402, 1 , 0 , 3780 },
 { 403, 0 , 0 , 3780 },
 { 403, 1 , 0 , 3780 },
 { 404, 0 , 0 , 3780 },
 { 404, 1 , 0 , 3780 },
 { 405, 0 , 0 , 3780 },
 { 405, 1 , 0 , 3780 },
 { 406, 0 , 0 , 3780 },
 { 406, 1 , 0 , 3780 },
 { 407, 0 , 0 , 3780 },
 { 407, 1 , 0 , 3780 },
 { 412, 0 , 0 , 1260 },
 { 412, 0 , 1 , 1260 },
 { 412, 0 , 2 , 1260 },
 { 412, 1 , 0 , 1260 },
 { 412, 1 , 1 , 1260 },
 { 412, 1 , 2 , 1260 },
 { 413, 1 , 0 , 2520 },
 { 413, 1 , 1 , 2520 },
 { 413, 1 , 2 , 2520 },
 { 414, 0 , 0 , 7560 },
 { 415, 0 , 0 , 3780 },
 { 415, 1 , 0 , 3780 },
 { 416, 1 , 0 , 7560 },
 { 417, 0 , 0 , 3780 },
 { 417, 1 , 0 , 3780 },
 { 420, 0 , 0 , 3780 },
 { 420, 1 , 0 , 3780 },
 { 421, 0 , 0 , 3780 },
 { 421, 1 , 0 , 3780 },
 { 431, 0 , 0 , 3780 },
 { 431, 1 , 0 , 3780 },
 { 432, 0 , 0 , 3780 },
 { 432, 1 , 0 , 3780 },
 { 433, 0 , 0 , 3780 },
 { 433, 1 , 0 , 3780 },
 { 434, 0 , 0 , 3780 },
 { 434, 1 , 0 , 3780 },
 { 435, 0 , 0 , 3780 },
 { 435, 1 , 0 , 3780 },
 { 436, 2 , 0 , 7560 },
 { 437, 2 , 0 , 7560 },
 { 441, 0 , 0 , 3780 },
 { 441, 1 , 0 , 3780 },
 { 442, 0 , 0 , 3780 },
 { 442, 1 , 0 , 3780 },
 { 453, 0 , 0 , 3780 },
 { 453, 1 , 0 , 3780 },
 { 454, 0 , 0 , 3780 },
 { 454, 1 , 0 , 3780 },
 { 455, 0 , 0 , 3780 },
 { 455, 1 , 0 , 3780 },
 { 464, 0 , 0 , 3780 },
 { 464, 1 , 0 , 3780 },
 { 465, 0 , 0 , 3780 },
 { 465, 1 , 0 , 3780 },
 { 468, 0 , 0 , 3780 },
 { 468, 1 , 0 , 3780 },
 { 472, 0 , 0 , 3780 },
 { 472, 1 , 0 , 3780 },
 { 473, 0 , 0 , 3780 },
 { 473, 1 , 0 , 3780 },
 { 477, 0 , 0 , 3780 },
 { 477, 1 , 0 , 3780 },
 { 495, 0 , 0 , 3780 },
 { 495, 1 , 0 , 3780 },
 { 496, 0 , 0 , 3780 },
 { 496, 1 , 0 , 3780 },
 { 497, 0 , 0 , 3780 },
 { 497, 1 , 0 , 3780 },
 { 498, 0 , 0 , 3780 },
 { 498, 1 , 0 , 3780 },
 { 499, 0 , 0 , 3780 },
 { 499, 1 , 0 , 3780 },
 { 500, 0 , 0 , 3780 },
 { 500, 1 , 0 , 3780 },
 { 501, 0 , 0 , 3780 },
 { 501, 1 , 0 , 3780 },
 { 502, 0 , 0 , 3780 },
 { 502, 1 , 0 , 3780 },
 { 503, 0 , 0 , 3780 },
 { 503, 1 , 0 , 3780 },
 { 504, 0 , 0 , 3780 },
 { 504, 1 , 0 , 3780 },
 { 505, 0 , 0 , 3780 },
 { 505, 1 , 0 , 3780 },
 { 509, 0 , 0 , 3780 },
 { 509, 1 , 0 , 3780 },
 { 510, 0 , 0 , 3780 },
 { 510, 1 , 0 , 3780 },
 { 511, 0 , 0 , 3780 },
 { 511, 1 , 0 , 3780 },
 { 512, 0 , 0 , 3780 },
 { 512, 1 , 0 , 3780 },
 { 513, 0 , 0 , 3780 },
 { 513, 1 , 0 , 3780 },
 { 514, 0 , 0 , 3780 },
 { 514, 1 , 0 , 3780 },
 { 515, 0 , 0 , 3780 },
 { 515, 1 , 0 , 3780 },
 { 516, 0 , 0 , 3780 },
 { 516, 1 , 0 , 3780 },
 { 517, 0 , 0 , 3780 },
 { 517, 1 , 0 , 3780 },
 { 518, 0 , 0 , 3780 },
 { 518, 1 , 0 , 3780 },
 { 519, 0 , 0 , 3780 },
 { 519, 1 , 0 , 3780 },
 { 520, 0 , 0 , 3780 },
 { 520, 1 , 0 , 3780 },
 { 521, 0 , 0 , 3780 },
 { 521, 1 , 0 , 3780 },
 { 522, 0 , 0 , 3780 },
 { 522, 1 , 0 , 3780 },
 { 523, 0 , 0 , 3780 },
 { 523, 1 , 0 , 3780 },
 { 527, 0 , 0 , 3780 },
 { 527, 1 , 0 , 3780 },
 { 528, 0 , 0 , 3780 },
 { 528, 1 , 0 , 3780 },
 { 529, 0 , 0 , 3780 },
 { 529, 1 , 0 , 3780 },
 { 530, 0 , 0 , 3780 },
 { 530, 1 , 0 , 3780 },
 { 532, 0 , 0 , 3780 },
 { 532, 1 , 0 , 3780 },
 { 533, 0 , 0 , 3780 },
 { 533, 1 , 0 , 3780 },
 { 534, 0 , 0 , 3780 },
 { 534, 1 , 0 , 3780 },
 { 535, 0 , 0 , 3780 },
 { 535, 1 , 0 , 3780 },
 { 536, 0 , 0 , 3780 },
 { 536, 1 , 0 , 3780 },
 { 537, 0 , 0 , 3780 },
 { 537, 1 , 0 , 3780 },
 { 538, 0 , 0 , 7560 },
 { 539, 0 , 0 , 7560 },
 { 540, 0 , 0 , 3780 },
 { 540, 1 , 0 , 3780 },
 { 541, 0 , 0 , 3780 },
 { 541, 1 , 0 , 3780 },
 { 542, 0 , 0 , 3780 },
 { 542, 1 , 0 , 3780 },
 { 543, 0 , 0 , 3780 },
 { 543, 1 , 0 , 3780 },
 { 544, 0 , 0 , 3780 },
 { 544, 1 , 0 , 3780 },
 { 545, 0 , 0 , 3780 },
 { 545, 1 , 0 , 3780 },
 { 554, 0 , 0 , 3780 },
 { 554, 1 , 0 , 3780 },
 { 555, 0 , 0 , 3780 },
 { 555, 1 , 0 , 3780 },
 { 556, 0 , 0 , 3780 },
 { 556, 1 , 0 , 3780 },
 { 562, 0 , 0 , 3780 },
 { 562, 1 , 0 , 3780 },
 { 563, 0 , 0 , 3780 },
 { 563, 1 , 0 , 3780 },
 { 574, 0 , 0 , 3780 },
 { 574, 1 , 0 , 3780 },
 { 575, 0 , 0 , 3780 },
 { 575, 1 , 0 , 3780 },
 { 576, 0 , 0 , 3780 },
 { 576, 1 , 0 , 3780 },
 { 577, 0 , 0 , 3780 },
 { 577, 1 , 0 , 3780 },
 { 578, 0 , 0 , 3780 },
 { 578, 1 , 0 , 3780 },
 { 579, 0 , 0 , 3780 },
 { 579, 1 , 0 , 3780 },
 { 585, 0 , 0 , 945 },
 { 585, 0 , 1 , 945 },
 { 585, 0 , 2 , 945 },
 { 585, 0 , 3 , 945 },
 { 585, 1 , 0 , 945 },
 { 585, 1 , 1 , 945 },
 { 585, 1 , 2 , 945 },
 { 585, 1 , 3 , 945 },
 { 586, 0 , 0 , 945 },
 { 586, 0 , 1 , 945 },
 { 586, 0 , 2 , 945 },
 { 586, 0 , 3 , 945 },
 { 586, 1 , 0 , 945 },
 { 586, 1 , 1 , 945 },
 { 586, 1 , 2 , 945 },
 { 586, 1 , 3 , 945 },
 { 588, 0 , 0 , 3780 },
 { 588, 1 , 0 , 3780 },
 { 589, 0 , 0 , 3780 },
 { 589, 1 , 0 , 3780 },
 { 590, 0 , 0 , 3780 },
 { 590, 1 , 0 , 3780 },
 { 591, 0 , 0 , 3780 },
 { 591, 1 , 0 , 3780 },
 { 595, 0 , 0 , 3780 },
 { 595, 1 , 0 , 3780 },
 { 596, 0 , 0 , 3780 },
 { 596, 1 , 0 , 3780 },
 { 597, 0 , 0 , 3780 },
 { 597, 1 , 0 , 3780 },
 { 598, 0 , 0 , 3780 },
 { 598, 1 , 0 , 3780 },
 { 599, 2 , 0 , 7560 },
 { 600, 2 , 0 , 7560 },
 { 601, 2 , 0 , 7560 },
 { 602, 0 , 0 , 3780 },
 { 602, 1 , 0 , 3780 },
 { 603, 0 , 0 , 3780 },
 { 603, 1 , 0 , 3780 },
 { 604, 0 , 0 , 3780 },
 { 604, 1 , 0 , 3780 },
 { 607, 0 , 0 , 3780 },
 { 607, 1 , 0 , 3780 },
 { 608, 0 , 0 , 3780 },
 { 608, 1 , 0 , 3780 },
 { 609, 0 , 0 , 3780 },
 { 609, 1 , 0 , 3780 },
 { 610, 0 , 0 , 3780 },
 { 610, 1 , 0 , 3780 },
 { 611, 0 , 0 , 3780 },
 { 611, 1 , 0 , 3780 },
 { 612, 0 , 0 , 3780 },
 { 612, 1 , 0 , 3780 },
 { 613, 0 , 0 , 3780 },
 { 613, 1 , 0 , 3780 },
 { 614, 0 , 0 , 3780 },
 { 614, 1 , 0 , 3780 },
 { 615, 2 , 0 , 7560 },
 { 616, 0 , 0 , 3780 },
 { 616, 1 , 0 , 3780 },
 { 617, 0 , 0 , 3780 },
 { 617, 1 , 0 , 3780 },
 { 626, 0 , 0 , 3780 },
 { 626, 1 , 0 , 3780 },
 { 631, 0 , 0 , 3780 },
 { 631, 1 , 0 , 3780 },
 { 632, 0 , 0 , 3780 },
 { 632, 1 , 0 , 3780 },
 { 633, 0 , 0 , 3780 },
 { 633, 1 , 0 , 3780 },
 { 634, 0 , 0 , 3780 },
 { 634, 1 , 0 , 3780 },
 { 635, 0 , 0 , 3780 },
 { 635, 1 , 0 , 3780 },
 { 659, 0 , 0 , 3780 },
 { 659, 1 , 0 , 3780 },
 { 660, 0 , 0 , 3780 },
 { 660, 1 , 0 , 3780 },
 { 672, 0 , 0 , 3780 },
 { 672, 1 , 0 , 3780 },
 { 673, 0 , 0 , 3780 },
 { 673, 1 , 0 , 3780 },
 { 677, 0 , 0 , 3780 },
 { 677, 1 , 0 , 3780 },
 { 678, 0 , 0 , 3780 },
 { 678, 1 , 1 , 3780 },
 { 710, 0 , 0 , 945 },
 { 710, 0 , 1 , 945 },
 { 710, 0 , 2 , 945 },
 { 710, 0 , 3 , 945 },
 { 710, 1 , 0 , 945 },
 { 710, 1 , 1 , 945 },
 { 710, 1 , 2 , 945 },
 { 710, 1 , 3 , 945 },
 { 711, 0 , 0 , 945 },
 { 711, 0 , 1 , 945 },
 { 711, 0 , 2 , 945 },
 { 711, 0 , 3 , 945 },
 { 711, 1 , 0 , 945 },
 { 711, 1 , 1 , 945 },
 { 711, 1 , 2 , 945 },
 { 711, 1 , 3 , 945 },
 { 684, 0 , 0 , 3780 },
 { 684, 1 , 0 , 3780 },
 { 685, 0 , 0 , 3780 },
 { 685, 1 , 0 , 3780 },
 { 679, 0 , 0 , 3780 },
 { 679, 1 , 0 , 3780 },
 { 680, 0 , 0 , 3780 },
 { 680, 1 , 0 , 3780 },
 { 681, 0 , 0 , 3780 },
 { 681, 1 , 0 , 3780 },
 { 712, 0 , 0 , 3780 },
 { 712, 1 , 0 , 3780 },
 { 713, 0 , 0 , 3780 },
 { 713, 1 , 0 , 3780 },
 { 682, 0 , 0 , 3780 },
 { 682, 1 , 0 , 3780 },
 { 683, 0 , 0 , 3780 },
 { 683, 1 , 0 , 3780 },
};


//------------------------------------------------------------------
/**
  *  @fix      MMCat[192]   momijiの図鑑QRをnijiで読み込むと状況に合わないテキストが表示される
  *  @brief    momiji図鑑でのみ図鑑QRをScanできるポケモンのリスト
  * 
  */
//------------------------------------------------------------------
static const MonsNo mons_CanReadMomijiZukanOnly[] =
{
  MONSNO_AABO,
  MONSNO_AABOKKU,
  MONSNO_PAUWAU,
  MONSNO_ZYUGON,
  MONSNO_BERORINGA,
  MONSNO_BARIYAADO,
  MONSNO_RUUZYURA,
  MONSNO_OMUNAITO,
  MONSNO_OMUSUTAA,
  MONSNO_KABUTO,
  MONSNO_KABUTOPUSU,
  MONSNO_HOOHOO,
  MONSNO_YORUNOZUKU,
  MONSNO_NEITHI,
  MONSNO_NEITHIO,
  MONSNO_MERIIPU,
  MONSNO_MOKOKO,
  MONSNO_DENRYUU,
  MONSNO_EIPAMU,
  MONSNO_KUNUGIDAMA,
  MONSNO_FORETOSU,
  MONSNO_NOKOTTI,
  MONSNO_HERAKUROSU,
  MONSNO_TEPPOUO,
  MONSNO_OKUTAN,
  MONSNO_MANTAIN,
  MONSNO_DERUBIRU,
  MONSNO_HERUGAA,
  MONSNO_MUTYUURU,
  MONSNO_YOOGIRASU,
  MONSNO_SANAGIRASU,
  MONSNO_BANGIRASU,
  MONSNO_KUTIITO,
  MONSNO_RAKURAI,
  MONSNO_RAIBORUTO,
  MONSNO_HEIGANI,
  MONSNO_SIZARIGAA,
  MONSNO_YAZIRON,
  MONSNO_NENDOORU,
  MONSNO_RIRIIRA,
  MONSNO_YUREIDORU,
  MONSNO_ANOPUSU,
  MONSNO_AAMARUDO,
  MONSNO_KAKUREON,
  MONSNO_KAGEBOUZU,
  MONSNO_ZYUPETTA,
  MONSNO_TOROPIUSU,
  MONSNO_PAARURU,
  MONSNO_HANTEERU,
  MONSNO_SAKURABISU,
  MONSNO_ETEBOOSU,
  MONSNO_MIMIRORU,
  MONSNO_MIMIROPPU,
  MONSNO_MANENE,
  MONSNO_TAMANTA,
  MONSNO_BEROBERUTO,
  MONSNO_BASURAO,
  MONSNO_ZURUGGU,
  MONSNO_ZURUZUKIN,
  MONSNO_ZOROA,
  MONSNO_ZOROAAKU,
  MONSNO_TIRAAMHI,
  MONSNO_TIRATIINO,
  MONSNO_PURURIRU,
  MONSNO_BURUNGERU,
  MONSNO_RIGUREE,
  MONSNO_OOBEMU,
  MONSNO_KOZYOHUU,
  MONSNO_KOZYONDO,
  MONSNO_KURIMUGAN,
  MONSNO_GOBITTO,
  MONSNO_GORUUGU,
  MONSNO_KOMATANA,
  MONSNO_KIRIKIZAN,
  MONSNO_MERARUBA,
  MONSNO_URUGAMOSU,
  MONSNO_SISIKO,
  MONSNO_KAENZISI,
  MONSNO_HURABEBE,
  MONSNO_HURAETTE,
  MONSNO_HURAAJESU,
  MONSNO_TORIMIAN,
  MONSNO_MAAIIKA,
  MONSNO_KARAMANERO,
  MONSNO_KUZUMOO,
  MONSNO_DORAMIDORO,
  MONSNO_UDEPPOU,
  MONSNO_BUROSUTAA,
  MONSNO_TIGORASU,
  MONSNO_GATIGORASU,
  MONSNO_AMARUSU,
  MONSNO_AMARURUGA,
  MONSNO_RUTYABURU,
  MONSNO_DEDENNE,
  MONSNO_ONBATTO,
  MONSNO_ONBAAN,
  MONSNO_BIISUTOd1,
  MONSNO_BIISUTOd2,
  MONSNO_BIISUTOs,
  MONSNO_BIISUTOh,
  MONSNO_MABOROSI18,
};
static const u32  mons_CanReadMomijiZukanOnly_Elems = GFL_NELEMS(mons_CanReadMomijiZukanOnly);


//------------------------------------------------------------------
/**
  *  @brief    monsNOがmomiji図鑑専用処理対象であるかを確認する
  *            対象とされたポケモンの情報はnijiの図鑑には存在しない
  */
//------------------------------------------------------------------
static bool IsMomijiZukanOnly(const MonsNo monsNo)
{
  for(u32 index = 0; index < mons_CanReadMomijiZukanOnly_Elems; ++index)
  {
    if(monsNo == mons_CanReadMomijiZukanOnly[index])  return true;
  }
  return false;
}




//------------------------------------------------------------------
/**
  * @brief 図鑑用QRのセットアップ
  */
//------------------------------------------------------------------
void QRUtility::SetUpZukanQRData( gfl2::heap::HeapBase* pHeap , QR_ZUKAN* pZukanQR )
{
  if( pZukanQR == NULL || pHeap == NULL )
  {
    GFL_ASSERT(0);//@fix
    return;
  }


  {
    /** MMcat[200]   ルガルガン(フォルム2)の図鑑QRが表示される事について: ルガルガンQR補正  */
    /** MMcat[200]（再）  FORMNO_OOKAMI2_TWILIGHT のビットを倒す                            */
    if(pZukanQR->monsNo == MONSNO_OOKAMI2)
    {
      const u32 formBitMask = ~(0x01u << FORMNO_OOKAMI2_TWILIGHT);    /**<  黄昏の姿以外を全て許可  */
      u32       bitOR       = 0;
      
      /*  黄昏の姿のときのみdispFormを0に変更  */
      if(pZukanQR->dispForm == FORMNO_OOKAMI2_TWILIGHT)
      {
        pZukanQR->dispForm = 0;                              /*  form 0  */
        pZukanQR->formBit[QR_ZUKAN::SEE_FLAG_MALE] |= 0x01u; /*  form 0  */
      }

      for(u32 index = 0; index < QR_ZUKAN::SEE_FLAG_MAX; ++index) {pZukanQR->formBit[index] &= formBitMask;  bitOR |= pZukanQR->formBit[index];}   /*  only form0  */
      if(!bitOR)  pZukanQR->formBit[QR_ZUKAN::SEE_FLAG_MALE] = 0x01u;   /*  救済:ノーマル♂  */
    }

    /** MMcat[320]   フラエッテ（えいえんのはな）の図鑑QRがそのまま読み取れる */
    /** MMcat[320]（再）  FORMNO_HURAETTE_HAKAIのビットを倒す                 */
    if(pZukanQR->monsNo == MONSNO_HURAETTE)
    {
      const u32 formBitMask = ~(0x01u << FORMNO_HURAETTE_HAKAI);  /**<  FORMNO_HURAETTE_HAKAI以外の全てを許可  */
      u32       bitOR = 0;
      
      /*  永遠の花であればForm0へ  */
      if(pZukanQR->dispForm == FORMNO_HURAETTE_HAKAI)
      {
        pZukanQR->dispForm = 0;                                   /*  form 0              */
        pZukanQR->formBit[QR_ZUKAN::SEE_FLAG_FEMALE] |= 0x01u;    /*  ノーマル♀:form 0   */
      }

      for(u32 index = 0; index < QR_ZUKAN::SEE_FLAG_MAX; ++index) {pZukanQR->formBit[index] &= formBitMask;  bitOR |= pZukanQR->formBit[index];}

      if(!bitOR)  pZukanQR->formBit[QR_ZUKAN::SEE_FLAG_FEMALE] = 0x01u;   /*  救済:ノーマル♀  */
    }
    
    /**
      MMcat[521]   momijiのサトピカ図鑑QRをnijiで読み込んだ際の不具合
        momijiで追加されたForm7のbitを立てたQRではnijiで不正扱いされてしまうため、ここでForm7以降のBitを倒す
        ※読み取り側はForm0のみ図鑑登録する処理となっており、Nijiとの互換性のためここでは敢えてFormBitのみの対処とする
    */
    if(pZukanQR->monsNo == MONSNO_PIKATYUU)
    {
      const u32 formMaskForNijiComatibility  = 0x7Fu;   /**<  form6までのbitmask: 0000 0000 0111 1111b  */
      
      /*  nijiで不正扱いされてしまうため、君に決めたキャップのときのみここでdispFormを0に変更  */
      if(pZukanQR->dispForm == FORMNO_PIKATYUU_SATOPIKA20)
      {
        pZukanQR->dispForm = 0;
        pZukanQR->formBit[QR_ZUKAN::SEE_FLAG_MALE] |= 0x01u;    /*  ノーマルオス:form 0   */
      }

      for(u32 index = 0; index < QR_ZUKAN::SEE_FLAG_MAX; ++index) {pZukanQR->formBit[index] &= formMaskForNijiComatibility;}
    }
  }


#if defined(GF_PLATFORM_CTR)
  // リージョン
  pZukanQR->header.region = 0xFFFF;
#endif // defined(GF_PLATFORM_CTR)
  // ROMバージョン
  pZukanQR->header.romVersion = 0xFFFFFFFF;

  /**
    @fix      MMCat[192]   momijiの図鑑QRをnijiで読み込むと状況に合わないテキストが表示される
              nijiの図鑑には載らないmonsのQRはnijiでは読めないようにする
  */
  if(IsMomijiZukanOnly(static_cast<MonsNo>(pZukanQR->monsNo)))
  {
    pZukanQR->header.romVersion &= ~GetRomBit(VERSION_SUN);
    pZukanQR->header.romVersion &= ~GetRomBit(VERSION_MOON);
  }


  //署名
  QR_ZUKAN* pTemp = GFL_NEW( pHeap ) QR_ZUKAN;
  ::std::memcpy( pTemp , pZukanQR , sizeof(QR_ZUKAN) );

  SignBinary( pZukanQR , pTemp , BINARY_SIZE_TYPE_1 , pHeap );

  GFL_SAFE_DELETE( pTemp );

  //平文
  pZukanQR->footer.type[0] = 'P';
  pZukanQR->footer.type[1] = 'O';
  pZukanQR->footer.type[2] = 'K';
  pZukanQR->footer.type[3] = 'E';
  pZukanQR->footer.type[4] = 3;//QR_CRYPTO_TYPE_3
}

//------------------------------------------------------------------
/**
  * @brief QRから読み取ったバイナリ解析（QRアプリ用）
  */
//------------------------------------------------------------------
QR_TYPE_APP QRUtility::AnalyzeQRBinaryForApp( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , pml::pokepara::CoreParam* pOutput, bool& rIsSameFormBothGender )
{
  //サイズ＆フッターチェック
  QR_CRYPTO_TYPE type = CheckQRType( pBinary , size );

  GFL_PRINT("AnalyzeQRBinaryForApp : type[%d]\n",type);

  rIsSameFormBothGender = false;

  if( type == QR_CRYPTO_TYPE_NONE )
  {//一般QR
    CreateNormalQRPoke( pHeap , pBinary , size , pOutput );
    return QR_TYPE_APP_NORMAL;
  }

  //サイズ決定
  u32 bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
  switch( type )
  {
  case QR_CRYPTO_TYPE_0://大会、バトルチーム
  case QR_CRYPTO_TYPE_1:
  case QR_CRYPTO_TYPE_2:
    bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
  break;
  case QR_CRYPTO_TYPE_3://図鑑
  case QR_CRYPTO_TYPE_4://外部連動
  case QR_CRYPTO_TYPE_5:
  case QR_CRYPTO_TYPE_6:
  case QR_CRYPTO_TYPE_7:
  case QR_CRYPTO_TYPE_8:
  case QR_CRYPTO_TYPE_9:
  case QR_CRYPTO_TYPE_10:
  case QR_CRYPTO_TYPE_11:
  case QR_CRYPTO_TYPE_12:
    bufferSize = BINARY_SIZE_TYPE_1+CRYPTO_OVERHEAD;
  break;
  }

  u8* pTempRecoveredData = GFL_NEW_ARRAY( pHeap->GetLowerHandle() ) u8[bufferSize];    /*  recovered message  */
  QR_TYPE_APP result = QR_TYPE_APP_INVALID;
  do
  {
    //署名チェック
    if( VerifyBinary( pBinary , bufferSize , pTempRecoveredData , pHeap , type ) == false )
    {//一般QR
      CreateNormalQRPoke( pHeap , pBinary , size , pOutput );
      result = QR_TYPE_APP_NORMAL;
      break;
    }

    QR_HEADER* pHeader = reinterpret_cast<QR_HEADER*>(pTempRecoveredData);

  #if defined(GF_PLATFORM_CTR)
    GFL_PRINT("QR_HEADER : ROM Version Bits[%d],ROM Region Bits[%d]\n",pHeader->romVersion,pHeader->region);
    /*
      System::GetVersion() からはPM_VERSION(or CasetteVersion) が返る
      PM_VERSION は
        niji   : VERSION_SUN,  VERSION_MOON   (30, 31) ※System/include/PokemonVersion.h
        momiji : VERSION_SUN2, VERSION_MOON2  (32, 33)
      で定義される
    */
//    if( ( pHeader->romVersion & ( 1 << ( System::GetVersion() - VERSION_SUN2 ) ) ) == 0 )//SUNが0
    if( !(pHeader->romVersion & GetRomBit(System::GetVersion())) )
    {//ROMコードエラー
      result = QR_TYPE_APP_ROM_NG;
      break;
    }

    if( ( pHeader->region & ( 1 << nn::cfg::GetRegion() ) ) == 0 )
    {//リージョンコードエラー
      result = QR_TYPE_APP_REGION_NG;
      break;
    }
  #endif

    if( type == QR_CRYPTO_TYPE_3 )
    {//図鑑ポケモン不正チェック
      QR_ZUKAN* pZukanQR = reinterpret_cast<QR_ZUKAN*>(pTempRecoveredData);
      /*
        ネタバレ防止
      */
      const u16 ZUKAN_CHECK_POKE[ZUKAN_CHECK_SIZE] =
      {
        MONSNO_THIKITORI,       //#785：カプ・コケコ
        MONSNO_THIKITYOU,       //#786：カプ・テテフ
        MONSNO_THIKIUSI,        //#787：カプ・ブルル
        MONSNO_THIKISAKANA,     //#788：カプ・レヒレ
        MONSNO_BEBII,           //#789：ベビー
        MONSNO_MAYU,            //#790：マユ
        MONSNO_NIKKOU,          //#791：ソルガレオ
        MONSNO_GEKKOU,          //#792：ルナアーラ
        MONSNO_BIISUTOr,        //#793：ウツロイド
        MONSNO_BIISUTOy,        //#794：マッシブーン
        MONSNO_BIISUTOb,        //#795：フェローチェ
        MONSNO_BIISUTOg,        //#796：デンジュモク
        MONSNO_BIISUTOp,        //#797：テッカグヤ
        MONSNO_BIISUTOi,        //#798：カミツルギ
        MONSNO_BIISUTOo,        //#799：アクジキング
        MONSNO_PURIZUMU,        //#800：ネクロズマ
        MONSNO_MABOROSI16,      //#801：マギアナ
        MONSNO_MABOROSI17,      //#802：マーシャドー
        /*  momiji追加  */
        MONSNO_BIISUTOd1,       //#803：ベベノム
        MONSNO_BIISUTOd2,       //#804：アーゴヨン
        MONSNO_BIISUTOs,        //#805：ツンデツンデ
        MONSNO_BIISUTOh,        //#806：ズガドーン
        MONSNO_MABOROSI18       //#807：ゼラオラ
      };

      for( u32 checkIndex = 0 ; checkIndex < ZUKAN_CHECK_SIZE ; ++checkIndex )
      {
        bool isExists = false;
        if( pZukanQR->monsNo == ZUKAN_CHECK_POKE[checkIndex] )
        {
          type = QR_CRYPTO_TYPE_NONE;//一般QRにする
          break;
        }
      }

      if( CheckZukanQR( pZukanQR ) == false )
      {
        type = QR_CRYPTO_TYPE_NONE;//一般QRにする
      }
    }
    else if( type > QR_CRYPTO_TYPE_3 )
    {
      QR_ZOO* pZoo = reinterpret_cast<QR_ZOO*>( pTempRecoveredData );

      //範囲チェック
      if( CheckZoo( pZoo ) == false )
      {
        type = QR_CRYPTO_TYPE_NONE;//一般QRにする
      }
    }

    //QRタイプ別処理
    switch( type )
    {
    case QR_CRYPTO_TYPE_3://図鑑
      {
        QR_ZUKAN* pZukanQR = reinterpret_cast<QR_ZUKAN*>(pTempRecoveredData);

        //サトピカ用補正
        if( pZukanQR->monsNo == MONSNO_PIKATYUU )
        {
          pZukanQR->dispForm = 0;//フォルムは0に戻す
          pZukanQR->formBit[0] = pZukanQR->formBit[0] & 1;//フォルム0以外は開放しない
          pZukanQR->formBit[1] = pZukanQR->formBit[1] & 1;//フォルム0以外は開放しない
          pZukanQR->formBit[2] = pZukanQR->formBit[2] & 1;//フォルム0以外は開放しない
          pZukanQR->formBit[3] = pZukanQR->formBit[3] & 1;//フォルム0以外は開放しない

          if( pZukanQR->formBit[0] == 0 &&
              pZukanQR->formBit[1] == 0 &&
              pZukanQR->formBit[2] == 0 &&
              pZukanQR->formBit[3] == 0 
            )
          {//何も見てない状態になってしまったら、ノーマルオスを開放する
            pZukanQR->formBit[0] = 1;
          }
        }

        /*  NMCat[831]:性別による違いがないポケモンの図鑑QRについての質問 対応  */
        rIsSameFormBothGender = (pZukanQR->dispSexSame != 0);
        result                = ZukanQRFunc( pZukanQR , pOutput , pHeap );
      }
    break;

    /*
      マギアナ
    */
    case QR_CRYPTO_TYPE_5:
      {
        const bool  isQualified = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_GAME_CLEAR );

        if(isQualified)
        {
          QR_ZOO* const pZooData  = reinterpret_cast<QR_ZOO*>( pTempRecoveredData );

          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRReaderSave()->ReadMagianaQR();

          if( pZooData->monsNo != MONSNO_MABOROSI16 )   /**< @attention  monsNoのみ正しく他パラメータが間違っている場合に正しく動作しないので、無条件に全パラメータを設定した方が良いのでは  */
          {//強制的にマギアナにする
            pZooData->monsNo  = MONSNO_MABOROSI16;
            pZooData->form    = 0;
            pZooData->sex     = pml::SEX_UNKNOWN;
            pZooData->rare    = false;
          }
          
          /*  pokeparaを生成  */
          if(CreatePoke_FromZooData(pOutput, pHeap, pZooData) == QR_TYPE_APP_ALOLA_NEW_OFFICIAL)
          {
            result = QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA;
          }
          else  /*  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL  */
          {
            const bool isClearedMagianaEvent  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_MAGIANA_CLEAR );

            result = isClearedMagianaEvent ? QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT : QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA;
          }
        }
        else
        {
          result = QR_TYPE_APP_MAGIANA_NG;
        }
      }
    break;

    /*
      ピカチュウM20
    */
    case QR_CRYPTO_TYPE_9:
      {
        const bool  isQualified = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_SATOPIKA_OPEN );

        if(isQualified)
        {
          QR_ZOO* const pZooData  = reinterpret_cast<QR_ZOO*>( pTempRecoveredData );

          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRReaderSave()->ReadM20PikaQR();  /*  既読  */

          {
            /*  ピカチュウM20固定  */
            pZooData->monsNo  = MONSNO_PIKATYUU;
            pZooData->form    = FORMNO_PIKATYUU_SATOPIKA20;
            pZooData->sex     = pml::SEX_MALE;
            pZooData->rare    = false;
          }
          
          /*  pokeparaを生成（表示用）  */
          if(CreatePoke_FromZooData(pOutput, pHeap, pZooData) == QR_TYPE_APP_ALOLA_NEW_OFFICIAL)
          {
            result = QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU;   /**<  M20ピカチュウOK: 図鑑未登録/M20ピカチュウイベント終了前  */
          }
          else  /*  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL  */
          {
            /**
              @fix MMcat[195]   QRスキャンでサトピカ入手イベント後のテキストが表示されない
                フラグが仮実装時のマギアナ用のもののままだった不具合の修正
            */
            const bool isClearedEvent  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_SATOPIKA_CLEAR );

            /*  ? M20ピカチュウOK: M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了後 : 図鑑登録済/M20ピカチュウイベント終了前  */
            result = isClearedEvent ? QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT : QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU;
          }
        }
        else
        {
          /*  到達前は読めない  */
          result = QR_TYPE_APP_M20PIKATYU_NG;
        }
      }
    break;

    case QR_CRYPTO_TYPE_4://外部連動
    case QR_CRYPTO_TYPE_6:
    case QR_CRYPTO_TYPE_7:
    case QR_CRYPTO_TYPE_8:
    case QR_CRYPTO_TYPE_10:
    case QR_CRYPTO_TYPE_11:
    case QR_CRYPTO_TYPE_12:
      {
        QR_ZOO* pZoo = reinterpret_cast<QR_ZOO*>( pTempRecoveredData );

        QR_ZUKAN zukanQR;
        ::std::memset( &zukanQR , 0 , sizeof(zukanQR) );
        zukanQR.monsNo    = pZoo->monsNo;
        zukanQR.dispForm  = pZoo->form;
        zukanQR.dispSex   = pZoo->sex;
        zukanQR.dispRare  = pZoo->rare;

        result = ZukanQRFunc( &zukanQR , pOutput , pHeap );//表示用モデルを生成

        if( PokeTool::ExtendData::IsExistLocalZukanStatic( pZoo->monsNo , PokeTool::ExtendData::LOCAL_AREA_A) == false )
        {//アローラのポケモンでない
          result = QR_TYPE_APP_OFFICIAL;
        }
        else
        {//アローラのポケモン
          //見た回数カウント
          Savedata::ZukanData* pZukan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
          PokeTool::SimpleParam simpleParam;
          simpleParam.monsNo = static_cast<MonsNo>( pZoo->monsNo );
          simpleParam.formNo = pZoo->form;
          simpleParam.isRare = pZoo->rare;
          simpleParam.sex = static_cast<pml::Sex>( pZoo->sex );
          simpleParam.isEgg = false;

          pZukan->SetPokeSee( simpleParam );

          if( result == QR_TYPE_APP_ALOLA_UPDATE )
          {//更新
            result = QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL;
          }
          else
          {//新規
            result = QR_TYPE_APP_ALOLA_NEW_OFFICIAL;
          }
        }
      }
    break;
    default:
      {//一般QRとして処理
        CreateNormalQRPoke( pHeap , pBinary , size , pOutput );
        result = QR_TYPE_APP_NORMAL;
      }
    break;
    }

  }while(0);

  GFL_SAFE_DELETE_ARRAY( pTempRecoveredData );

  return result;
}






//------------------------------------------------------------------
/**
  *  @brief    QR_ZOOからPokeparaを生成する
  *  @retval   QR_TYPE_APP_ALOLA_NEW_OFFICIAL     : 公式新規
  *  @retval   QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL  : 公式既視
  *  @retval   QR_TYPE_APP_OFFICIAL               : 公式図鑑外
  */
//------------------------------------------------------------------
QR_TYPE_APP QRUtility::CreatePoke_FromZooData(pml::pokepara::CoreParam* pOutput, gfl2::heap::HeapBase* const pHeapBase, const QR_ZOO* const pZooData)
{
  QR_TYPE_APP qrType_ZukanQRFuncResult  = QR_TYPE_APP_INVALID;

  /*  表示用モデルを生成、既知ならQR_TYPE_APP_ALOLA_UPDATE、初見ならQR_TYPE_APP_ALOLA_NEWが返る  */
  {
    QR_ZUKAN    zukanQR = {0};

    zukanQR.monsNo    = pZooData->monsNo;
    zukanQR.dispForm  = pZooData->form;
    zukanQR.dispSex   = pZooData->sex;
    zukanQR.dispRare  = pZooData->rare;

    qrType_ZukanQRFuncResult = ZukanQRFunc( &zukanQR , pOutput , pHeapBase );   /*  formBitが0なのでseeflagは変化しない  */
  }

  /*  アローラ生息ポケモンであれば既視フラグを立てる  */
  if( PokeTool::ExtendData::IsExistLocalZukanStatic( pZooData->monsNo , PokeTool::ExtendData::LOCAL_AREA_A /* アローラ全域 */))
  {
    Savedata::ZukanData*    pZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
    PokeTool::SimpleParam   simpleParam;

    simpleParam.monsNo  = static_cast<MonsNo>( pZooData->monsNo );
    simpleParam.formNo  = pZooData->form;
    simpleParam.isRare  = pZooData->rare;
    simpleParam.sex     = static_cast<pml::Sex>( pZooData->sex );
    simpleParam.isEgg   = false;

    pZukanData->SetPokeSee( simpleParam );    /*  当該formのみ既視となる  */
  }

  /*  result  */
  switch(qrType_ZukanQRFuncResult)
  {
    case  QR_TYPE_APP_ALOLA_NEW:    return  QR_TYPE_APP_ALOLA_NEW_OFFICIAL;
    case  QR_TYPE_APP_ALOLA_UPDATE: return  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL;
    default:                        return  QR_TYPE_APP_OFFICIAL;
  }
}


//------------------------------------------------------------------
/**
  * @brief QRから読み取ったバイナリ解析（バトルチーム選択モード用）
  */
//------------------------------------------------------------------
QR_TYPE_BATTLE_TEAM QRUtility::AnalyzeQRBinaryForBattleTeam( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , pml::PokeParty* pOutput )
{
  //サイズ＆フッターチェック
  QR_CRYPTO_TYPE type = CheckQRType( pBinary , size );

  GFL_PRINT("AnalyzeQRBinaryForBattleTeam : type[%d]\n",type);

  if( type == QR_CRYPTO_TYPE_NONE )
  {//一般QR
    return QR_TYPE_BATTLE_TEAM_ZUKAN_NG;
  }

  //サイズ決定
  u32 bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
  switch( type )
  {
  case QR_CRYPTO_TYPE_0://大会、バトルチーム
  case QR_CRYPTO_TYPE_1:
  case QR_CRYPTO_TYPE_2:
    bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
    break;
  case QR_CRYPTO_TYPE_3://図鑑
  case QR_CRYPTO_TYPE_4://外部連動
  case QR_CRYPTO_TYPE_5:
  case QR_CRYPTO_TYPE_6:
  case QR_CRYPTO_TYPE_7:
  case QR_CRYPTO_TYPE_8:
  case QR_CRYPTO_TYPE_9:
  case QR_CRYPTO_TYPE_10:
  case QR_CRYPTO_TYPE_11:
  case QR_CRYPTO_TYPE_12:
    bufferSize = BINARY_SIZE_TYPE_1+CRYPTO_OVERHEAD;
    break;
  }

  u8* pTemp = GFL_NEW_ARRAY( pHeap->GetLowerHandle() ) u8[bufferSize];
  QR_TYPE_BATTLE_TEAM result = QR_TYPE_BATTLE_TEAM_INVALID;
  do
  {
    //署名チェック
    if( VerifyBinary( pBinary , bufferSize , pTemp , pHeap , type ) == false )
    {//一般QR
      result = QR_TYPE_BATTLE_TEAM_ZUKAN_NG;
      break;
    }

    QR_HEADER* pHeader = reinterpret_cast<QR_HEADER*>(pTemp);

#if defined(GF_PLATFORM_CTR)
    /*
      System::GetVersion() からはPM_VERSION(or CasetteVersion) が返る
      PM_VERSION は
        niji   : VERSION_SUN,  VERSION_MOON   (30, 31) ※System/include/PokemonVersion.h
        momiji : VERSION_SUN2, VERSION_MOON2  (32, 33)
      で定義される
    */
    if( !(pHeader->romVersion & GetRomBit(System::GetVersion())) )
    {//ROMコードエラー
      result = QR_TYPE_BATTLE_TEAM_ROM_NG;
      break;
    }

    if( ( pHeader->region & ( 1 << nn::cfg::GetRegion() ) ) == 0 )
    {//リージョンコードエラー
      result = QR_TYPE_BATTLE_TEAM_REGION_NG;
      break;
    }
#endif

    //QRタイプ別処理
    switch( type )
    {
    case QR_CRYPTO_TYPE_2:
      {//目的のチームQR
        QR_BATTLE_TEAM* pQRTeam = GFL_NEW( pHeap->GetLowerHandle() ) QR_BATTLE_TEAM;
        DecryptAesForBattleTeamQR( &pHeader[1] , AES_CRYPT_SIZE , &pQRTeam->body );

        if( pQRTeam->body.nexUniqueID != GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetNexUniqueID() )
        {//ゲームシンクID不一致
          GFL_PRINT("GameSyncID error [%llu][%llu]\n", pQRTeam->body.nexUniqueID , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetNexUniqueID() );
          result = QR_TYPE_BATTLE_TEAM_GAMESYNC_ID_NG;
        }
        else
        {//ゲームシンクID一致
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRParty()->Clear();
          for( u32 index = 0 ; index < pml::PokeParty::MAX_MEMBERS ; index++ )
          {
            //初期設定
            pml::pokepara::InitialSpec spec;
            QR_POKE* pQRPoke = &pQRTeam->body.pokeData[index];
            spec.monsno = static_cast<MonsNo>(pQRPoke->monsno);
            spec.formno = pQRPoke->form_no;
            spec.sex = pQRPoke->sex;
            spec.id = 0;
            spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;
            spec.personal_rnd = pQRPoke->personal_rnd;
            spec.talentPower[pml::pokepara::POWER_HP] = pQRPoke->talent_hp;
            spec.talentPower[pml::pokepara::POWER_ATK] = pQRPoke->talent_atk;
            spec.talentPower[pml::pokepara::POWER_DEF] = pQRPoke->talent_def;
            spec.talentPower[pml::pokepara::POWER_SPATK] = pQRPoke->talent_spatk;
            spec.talentPower[pml::pokepara::POWER_SPDEF] = pQRPoke->talent_spdef;
            spec.talentPower[pml::pokepara::POWER_AGI] = pQRPoke->talent_agi;
            spec.seikaku = pQRPoke->seikaku;
            spec.level = pQRPoke->level;
            spec.sex = pQRPoke->sex;
            spec.familiarity = pQRPoke->familiarity;

            if( pQRPoke->tokusei_1_flag )
            {
              spec.tokusei_index = 0;
            }
            else if ( pQRPoke->tokusei_2_flag )
            {
              spec.tokusei_index = 1;
            }
            else
            {
              spec.tokusei_index = 2;
            }
            

            pml::pokepara::PokemonParam* pCreateParam = GFL_NEW( pHeap ) pml::pokepara::PokemonParam( pHeap , spec );

            //ポケパラ設定
            pCreateParam->SetWaza( 0 , static_cast<WazaNo>( pQRPoke->waza[0] ) );
            pCreateParam->SetWaza( 1 , static_cast<WazaNo>( pQRPoke->waza[1] ) );
            pCreateParam->SetWaza( 2 , static_cast<WazaNo>( pQRPoke->waza[2] ) );
            pCreateParam->SetWaza( 3 , static_cast<WazaNo>( pQRPoke->waza[3] ) );

            pCreateParam->SetWazaPPUpCount( 0 , ((u8*)&pQRPoke->pointup_used_count)[0] );
            pCreateParam->SetWazaPPUpCount( 1 , ((u8*)&pQRPoke->pointup_used_count)[1] );
            pCreateParam->SetWazaPPUpCount( 2 , ((u8*)&pQRPoke->pointup_used_count)[2] );
            pCreateParam->SetWazaPPUpCount( 3 , ((u8*)&pQRPoke->pointup_used_count)[3] );

            pCreateParam->SetItem( pQRPoke->itemno );

            pCreateParam->SetEventPokeFlag( pQRPoke->event_get_flag );

            pCreateParam->ChangeEffortPower( pml::pokepara::POWER_HP , pQRPoke->effort_hp );
            pCreateParam->ChangeEffortPower( pml::pokepara::POWER_ATK , pQRPoke->effort_atk );
            pCreateParam->ChangeEffortPower( pml::pokepara::POWER_DEF , pQRPoke->effort_def );
            pCreateParam->ChangeEffortPower( pml::pokepara::POWER_SPATK , pQRPoke->effort_spatk );
            pCreateParam->ChangeEffortPower( pml::pokepara::POWER_SPDEF , pQRPoke->effort_spdef );
            pCreateParam->ChangeEffortPower( pml::pokepara::POWER_AGI , pQRPoke->effort_agi );

            pCreateParam->SetGetBall( pQRPoke->get_ball );            

            pCreateParam->SetCassetteVersion( pQRPoke->rom_version );
            pCreateParam->SetLangId( pQRPoke->pokemon_language_code );
            
            pCreateParam->SetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL , pQRPoke->level );
            
            //すごい特訓
            for( u32 id = 0 ; id < pml::pokepara::POWER_NUM ; ++id )
            {
              if( ( ( 1 << id ) & pQRPoke->training_flag ) != 0 )
              {
                pml::pokepara::PowerID trainingID = static_cast<pml::pokepara::PowerID>( id );
                pCreateParam->SetTrainingDone( trainingID );
              }
            }

            //固定部分
            {
              pCreateParam->RemoveAllBoxMark();
              pCreateParam->SetTamagoWazaNo( 0 , (WazaNo)0 );
              pCreateParam->SetTamagoWazaNo( 1 , (WazaNo)0 );
              pCreateParam->SetTamagoWazaNo( 2 , (WazaNo)0 );
              pCreateParam->SetTamagoWazaNo( 3 , (WazaNo)0 );

              const gfl2::str::STRCODE name[System::STRLEN_PLAYER_NAME+System::EOM_LEN] = L"PGL";

              pCreateParam->SetParentName( name );
              pCreateParam->SetParentSex( (pml::Sex)GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetSex() );
              pCreateParam->SetFriendship( 0 );
              pCreateParam->SetCountryCode( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetCountry() );
              pCreateParam->SetCountryRegionCode( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetArea() );
              pCreateParam->SetHWRegionCode( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetRegionCode() );

              //捕獲or孵化の思い出
#if defined(GF_PLATFORM_CTR)
              nn::fnd::DateTime now = nn::fnd::DateTime::GetNow();
              pCreateParam->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR , now.GetYear()%100 );  // タマゴが孵化した or 捕獲した年
              pCreateParam->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH , now.GetMonth() );   // タマゴが孵化した or 捕獲した月
              pCreateParam->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY , now.GetDay() );  // タマゴが孵化した or 捕獲した日
              pCreateParam->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE , 40052 );  // タマゴが孵化した or 捕獲した場所
#endif

              pCreateParam->SetFamiliarity( 70 );

              pCreateParam->RecalculateCalcData();
              pCreateParam->RecoverAll();
            }

            //ゲームデータの領域に
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRParty()->AddMember( *pCreateParam );

            GFL_SAFE_DELETE( pCreateParam );
          }

          //ゲームデータの領域に
          ::std::memcpy( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetQRPartyName() , pQRTeam->body.teamName , sizeof( pQRTeam->body.teamName ) );

          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetQRBattleTeamID( pQRTeam->body.battleTeamID );

          //あったら入れる
          if( pOutput )
          {
            pOutput->CopyFrom( *pOutput );
          }

          result = QR_TYPE_BATTLE_TEAM_OK;
        }

        GFL_SAFE_DELETE( pQRTeam );
      }
    break;
    case QR_CRYPTO_TYPE_0:
      {
        result = QR_TYPE_BATTLE_TEAM_FRIEND_NG;
      }
    break;
    case QR_CRYPTO_TYPE_1:
      {
        result = QR_TYPE_BATTLE_TEAM_LIVE_NG;
      }
    break;
    case QR_CRYPTO_TYPE_3://図鑑
    case QR_CRYPTO_TYPE_4://外部連動
    case QR_CRYPTO_TYPE_5:
    case QR_CRYPTO_TYPE_6:
    case QR_CRYPTO_TYPE_7:
    case QR_CRYPTO_TYPE_8:
    case QR_CRYPTO_TYPE_9:
    case QR_CRYPTO_TYPE_10:
    case QR_CRYPTO_TYPE_11:
    case QR_CRYPTO_TYPE_12:
    default:
      {
        result = QR_TYPE_BATTLE_TEAM_ZUKAN_NG;
      }
      break;
    }

  }while(0);

  GFL_SAFE_DELETE_ARRAY( pTemp );

  return result;
}

//------------------------------------------------------------------
/**
  * @brief QRから読み取ったバイナリ解析（なかま大会モード用）
  */
//------------------------------------------------------------------
QR_TYPE_FRIEND_CUP QRUtility::AnalyzeQRBinaryForFriendCup( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , Regulation* pOutput )
{
  //サイズ＆フッターチェック
  QR_CRYPTO_TYPE type = CheckQRType( pBinary , size );

  GFL_PRINT("AnalyzeQRBinaryForFriendCup : type[%d]\n",type);

  if( type == QR_CRYPTO_TYPE_NONE )
  {//一般QR
    return QR_TYPE_FRIEND_CUP_ZUKAN_NG;
  }

  //サイズ決定
  u32 bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
  switch( type )
  {
  case QR_CRYPTO_TYPE_0://大会、バトルチーム
  case QR_CRYPTO_TYPE_1:
  case QR_CRYPTO_TYPE_2:
    bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
    break;
  case QR_CRYPTO_TYPE_3://図鑑
  case QR_CRYPTO_TYPE_4://外部連動
  case QR_CRYPTO_TYPE_5:
  case QR_CRYPTO_TYPE_6:
  case QR_CRYPTO_TYPE_7:
  case QR_CRYPTO_TYPE_8:
  case QR_CRYPTO_TYPE_9:
  case QR_CRYPTO_TYPE_10:
  case QR_CRYPTO_TYPE_11:
  case QR_CRYPTO_TYPE_12:
    bufferSize = BINARY_SIZE_TYPE_1+CRYPTO_OVERHEAD;
    break;
  }

  u8* pTemp = GFL_NEW_ARRAY( pHeap->GetLowerHandle() ) u8[bufferSize];
  QR_TYPE_FRIEND_CUP result = QR_TYPE_FRIEND_CUP_INVALID;
  do
  {
    //署名チェック
    if( VerifyBinary( pBinary , bufferSize , pTemp , pHeap , type ) == false )
    {//一般QR
      result = QR_TYPE_FRIEND_CUP_ZUKAN_NG;
      break;
    }

    QR_HEADER* pHeader = reinterpret_cast<QR_HEADER*>(pTemp);

#if defined(GF_PLATFORM_CTR)
    /*
      System::GetVersion() からはPM_VERSION(or CasetteVersion) が返る
      PM_VERSION は
        niji   : VERSION_SUN,  VERSION_MOON   (30, 31) ※System/include/PokemonVersion.h
        momiji : VERSION_SUN2, VERSION_MOON2  (32, 33)
      で定義される
    */
    if( !(pHeader->romVersion & GetRomBit(System::GetVersion())) )
    {//ROMコードエラー
      result = QR_TYPE_FRIEND_CUP_ROM_NG;
      break;
    }

    if( ( pHeader->region & ( 1 << nn::cfg::GetRegion() ) ) == 0 )
    {//リージョンコードエラー
      result = QR_TYPE_FRIEND_CUP_REGION_NG;
      break;
    }
#endif

    //QRタイプ別処理
    switch( type )
    {
    case QR_CRYPTO_TYPE_0:
      {//目的の仲間大会QR
        QR_FRIEND_REGULATION* pQRRegulation = reinterpret_cast<QR_FRIEND_REGULATION*>( pTemp );
        if( pOutput->SetSimpleData( pQRRegulation->regulation , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetNexUniqueID() ) == false )
        {//ゲームシンクID不一致で失敗
          result = QR_TYPE_FRIEND_CUP_GAMESYNC_ID_NG;
        }
        else
        {//成功
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetQRParam1( pQRRegulation->param1 );
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetQRParam2( pQRRegulation->param2 );

#if PM_DEBUG
          pOutput->DebugPrint();
#endif

          result = QR_TYPE_FRIEND_CUP_OK;
        }
      }
      break;
    case QR_CRYPTO_TYPE_1:
      {
        result = QR_TYPE_FRIEND_CUP_LIVE_NG;
      }
      break;
    case QR_CRYPTO_TYPE_2:
      {
        result = QR_TYPE_FRIEND_CUP_BATTLE_TEAM_NG;
      }
      break;
    case QR_CRYPTO_TYPE_3://図鑑
    case QR_CRYPTO_TYPE_4://外部連動
    case QR_CRYPTO_TYPE_5:
    case QR_CRYPTO_TYPE_6:
    case QR_CRYPTO_TYPE_7:
    case QR_CRYPTO_TYPE_8:
    case QR_CRYPTO_TYPE_9:
    case QR_CRYPTO_TYPE_10:
    case QR_CRYPTO_TYPE_11:
    case QR_CRYPTO_TYPE_12:
    default:
      {
        result = QR_TYPE_FRIEND_CUP_ZUKAN_NG;
      }
      break;
    }

  }while(0);

  GFL_SAFE_DELETE_ARRAY( pTemp );

  return result;
}

//------------------------------------------------------------------
/**
  * @brief QRから読み取ったバイナリ解析（ライブ大会モード用）
  */
//------------------------------------------------------------------
QR_TYPE_LIVE_CUP QRUtility::AnalyzeQRBinaryForLiveCup( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , Regulation* pOutput )
{
  //サイズ＆フッターチェック
  QR_CRYPTO_TYPE type = CheckQRType( pBinary , size );

  GFL_PRINT("AnalyzeQRBinaryForLiveCup : type[%d]\n",type);

  if( type == QR_CRYPTO_TYPE_NONE )
  {//一般QR
    return QR_TYPE_LIVE_CUP_ZUKAN_NG;
  }

  //サイズ決定
  u32 bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
  switch( type )
  {
  case QR_CRYPTO_TYPE_0://大会、バトルチーム
  case QR_CRYPTO_TYPE_1:
  case QR_CRYPTO_TYPE_2:
    bufferSize = BINARY_SIZE_TYPE_2+CRYPTO_OVERHEAD;
    break;
  case QR_CRYPTO_TYPE_3://図鑑
  case QR_CRYPTO_TYPE_4://外部連動
  case QR_CRYPTO_TYPE_5:
  case QR_CRYPTO_TYPE_6:
  case QR_CRYPTO_TYPE_7:
  case QR_CRYPTO_TYPE_8:
  case QR_CRYPTO_TYPE_9:
  case QR_CRYPTO_TYPE_10:
  case QR_CRYPTO_TYPE_11:
  case QR_CRYPTO_TYPE_12:
    bufferSize = BINARY_SIZE_TYPE_1+CRYPTO_OVERHEAD;
    break;
  }

  u8* pTemp = GFL_NEW_ARRAY( pHeap->GetLowerHandle() ) u8[bufferSize];
  QR_TYPE_LIVE_CUP result = QR_TYPE_LIVE_CUP_INVALID;
  do
  {
    //署名チェック
    if( VerifyBinary( pBinary , bufferSize , pTemp , pHeap , type ) == false )
    {//一般QR
      result = QR_TYPE_LIVE_CUP_ZUKAN_NG;
      break;
    }

    QR_HEADER* pHeader = reinterpret_cast<QR_HEADER*>(pTemp);

#if defined(GF_PLATFORM_CTR)
    /*
      System::GetVersion() からはPM_VERSION(or CasetteVersion) が返る
      PM_VERSION は
        niji   : VERSION_SUN,  VERSION_MOON   (30, 31) ※System/include/PokemonVersion.h
        momiji : VERSION_SUN2, VERSION_MOON2  (32, 33)
      で定義される
    */
    if( !(pHeader->romVersion & GetRomBit(System::GetVersion())) )
    {//ROMコードエラー
      result = QR_TYPE_LIVE_CUP_ROM_NG;
      break;
    }

    if( ( pHeader->region & ( 1 << nn::cfg::GetRegion() ) ) == 0 )
    {//リージョンコードエラー
      result = QR_TYPE_LIVE_CUP_REGION_NG;
      break;
    }
#endif

    //QRタイプ別処理
    switch( type )
    {
    case QR_CRYPTO_TYPE_1:
      {//目的のライブ大会QR
        QR_FRIEND_REGULATION* pQRRegulation = reinterpret_cast<QR_FRIEND_REGULATION*>( pTemp );
        pOutput->SetSimpleData( pQRRegulation->regulation );

        result = QR_TYPE_LIVE_CUP_OK;
      }
      break;
    case QR_CRYPTO_TYPE_0:
      {
        result = QR_TYPE_LIVE_CUP_FRIEND_CUP_NG;
      }
      break;
    case QR_CRYPTO_TYPE_2:
      {
        result = QR_TYPE_LIVE_CUP_BATTLE_TEAM_NG;
      }
      break;
    case QR_CRYPTO_TYPE_3://図鑑
    case QR_CRYPTO_TYPE_4://外部連動
    case QR_CRYPTO_TYPE_5:
    case QR_CRYPTO_TYPE_6:
    case QR_CRYPTO_TYPE_7:
    case QR_CRYPTO_TYPE_8:
    case QR_CRYPTO_TYPE_9:
    case QR_CRYPTO_TYPE_10:
    case QR_CRYPTO_TYPE_11:
    case QR_CRYPTO_TYPE_12:
    default:
      {
        result = QR_TYPE_LIVE_CUP_ZUKAN_NG;
      }
      break;
    }

  }while(0);

  GFL_SAFE_DELETE_ARRAY( pTemp );

  return result;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  QRUtility::GetRomBit(const u32 romVersion)
{
  switch(romVersion)
  {
    case  VERSION_SUN:    return (0x01u <<  0);
    case  VERSION_MOON:   return (0x01u <<  1);
    case  VERSION_SUN2:   return (0x01u <<  2);
    case  VERSION_MOON2:  return (0x01u <<  3);
  }

  GFL_ASSERT_MSG(false, "invalid romVersion %u\n", romVersion);
  return 0;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool QRUtility::CheckZukanQR( QR_ZUKAN* pZukan )
{
  if ( pZukan->monsNo <= 0 || pZukan->monsNo > MONSNO_END )
  {//monsNo
    return false;
  }

  pml::personal::LoadPersonalData( static_cast<MonsNo>( pZukan->monsNo ), static_cast<FormNo>(0));
  int formmax = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_form_max);//フォルムの最大値は無効値
  //フォルムのビットをマージする
  u32 formBit = pZukan->formBit[QR_ZUKAN::SEE_FLAG_MALE] | 
                pZukan->formBit[QR_ZUKAN::SEE_FLAG_FEMALE] | 
                pZukan->formBit[QR_ZUKAN::SEE_FLAG_M_RARE] |
                pZukan->formBit[QR_ZUKAN::SEE_FLAG_F_RARE];

  u32 maxFormNo = 0;//最大フォルム見つかるまでのカウント
  for ever
  {
    if( formBit == 0 )
    {
      break;
    }

    formBit = formBit >> 1;
    maxFormNo++;
  }

  if( formmax <= pZukan->dispForm || formmax < maxFormNo )
  {//formNo
    return false;
  }

  u8 sex = pZukan->dispSex;
  pml::personal::SexVector sex_vec;
  sex_vec = static_cast<pml::personal::SexVector>(pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ));
  if ( sex_vec == pml::personal::SEX_VECTOR_ONLY_MALE )
  {
    if ( sex != pml::SEX_MALE )
    {
      // 性別があり得ない値
      return false;
    }
    else if( pZukan->formBit[QR_ZUKAN::SEE_FLAG_FEMALE] + pZukan->formBit[QR_ZUKAN::SEE_FLAG_F_RARE] != 0 )
    {
      // 性別があり得ない値
      return false;
    }
    else if( pZukan->dispSexSame == 1 )
    {
      // 性別があり得ない値
      return false;
    }
  }
  else if ( sex_vec == pml::personal::SEX_VECTOR_ONLY_FEMALE )
  {
    if ( sex != pml::SEX_FEMALE )
    {
      // 性別があり得ない値
      return false;
    }
    else if( pZukan->formBit[QR_ZUKAN::SEE_FLAG_MALE] + pZukan->formBit[QR_ZUKAN::SEE_FLAG_M_RARE] != 0 )
    {
      // 性別があり得ない値
      return false;
    }
    else if( pZukan->dispSexSame == 1 )
    {
      // 性別があり得ない値
      return false;
    }
  }
  else if ( sex_vec == pml::personal::SEX_VECTOR_UNKNOWN )
  {
    if ( sex != pml::SEX_UNKNOWN )
    {
      // 性別があり得ない値
      return false;
    }
    else if( pZukan->formBit[QR_ZUKAN::SEE_FLAG_FEMALE] + pZukan->formBit[QR_ZUKAN::SEE_FLAG_F_RARE] != 0 )
    {
      // 性別があり得ない値
      return false;
    }
    else if( pZukan->dispSexSame == 1 )
    {
      // 性別があり得ない値
      return false;
    }
  }
  else if ( sex != pml::SEX_MALE && sex != pml::SEX_FEMALE )
  {
    // 性別があり得ない値
    return false;
  }

  if( pZukan->dispRare != 0 && pZukan->dispRare != 1 )
  {//レア情報がありえない
    return false;
  }

  if( pZukan->dispSexSame != 0 && pZukan->dispSexSame != 1 )
  {//見た目情報がありえない
    return false;
  }

  return true;
}

bool QRUtility::CheckZoo( QR_ZOO* pZoo )
{
  if ( pZoo->monsNo <= 0 || pZoo->monsNo > MONSNO_END )
  {//monsNo
    pZoo->monsNo = MONSNO_HUSIGIDANE;
  }

  pml::personal::LoadPersonalData( static_cast<MonsNo>( pZoo->monsNo ), static_cast<FormNo>(0));
  int formmax = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max );//フォルムの最大値は無効値
  if( formmax <= pZoo->form )
  {//formNo
    pZoo->form = 0;
  }

  //性別補正
  pml::personal::SexVector sex_vec;
  sex_vec = static_cast<pml::personal::SexVector>(pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ));
  if ( sex_vec == pml::personal::SEX_VECTOR_ONLY_MALE )
  {
    pZoo->sex = pml::SEX_MALE;
  }
  else if ( sex_vec == pml::personal::SEX_VECTOR_ONLY_FEMALE )
  {
    pZoo->sex = pml::SEX_FEMALE;
  }
  else if ( sex_vec == pml::personal::SEX_VECTOR_UNKNOWN )
  {
    pZoo->sex = pml::SEX_UNKNOWN;
  }
  else if ( pZoo->sex != pml::SEX_MALE && pZoo->sex != pml::SEX_FEMALE )
  {
    pZoo->sex = pml::SEX_MALE;
  }

  //レア
  if( pZoo->rare != 0 && pZoo->rare != 1 )
  {
    pZoo->rare = 0;
  }

  return true;
}

//type03
static const u8 s_publicKeyData[14][126] =
{
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB3, 0xD6, 0x8C, 0x9B, 0x10, 0x90, 0xF6, 0xB1, 0xB8, 0x8E, 0xCF, 0xA9, 0xE2, 0xF6, 0x0E, 0x9C, 0x62, 0xC3, 0x03, 0x3B, 0x5B, 0x64, 0x28, 0x2F, 0x26, 0x2C, 0xD3, 0x93, 0xB4, 0x33, 0xD9, 0x7B, 0xD3, 0xDB, 0x7E, 0xBA, 0x47, 0x0B, 0x1A, 0x77, 0xA3, 0xDB, 0x3C, 0x18, 0xA1, 0xE7, 0x61, 0x69, 0x72, 0x22, 0x9B, 0xDA, 0xD5, 0x4F, 0xB0, 0x2A, 0x19, 0x54, 0x6C, 0x65, 0xFA, 0x47, 0x73, 0xAA, 0xBE, 0x9B, 0x8C, 0x92, 0x67, 0x07, 0xE7, 0xB7, 0xDD, 0xE4, 0xC8, 0x67, 0xC0, 0x1C, 0x08, 0x02, 0x98, 0x5E, 0x43, 0x86, 0x56, 0x16, 0x8A, 0x44, 0x30, 0xF3, 0xF3, 0xB9, 0x66, 0x2D, 0x7D, 0x01, 0x02, 0x03, 0x01, 0x00, 0x01, } , //00
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xC1, 0x0F, 0x40, 0x97, 0xFD, 0x3C, 0x78, 0x1A, 0x8F, 0xDE, 0x10, 0x1E, 0xF3, 0xB2, 0xF0, 0x91, 0xF8, 0x2B, 0xEE, 0x47, 0x42, 0x32, 0x4B, 0x92, 0x06, 0xC5, 0x81, 0x76, 0x6E, 0xAF, 0x2F, 0xBB, 0x42, 0xC7, 0xD6, 0x0D, 0x74, 0x9B, 0x99, 0x9C, 0x52, 0x9B, 0x0E, 0x22, 0xAD, 0x05, 0xE0, 0xC8, 0x80, 0x23, 0x12, 0x19, 0xAD, 0x47, 0x31, 0x14, 0xEC, 0x45, 0x43, 0x80, 0xA9, 0x28, 0x98, 0xD7, 0xA8, 0xB5, 0x4D, 0x94, 0x32, 0x58, 0x48, 0x97, 0xD6, 0xAF, 0xE4, 0x86, 0x02, 0x35, 0x12, 0x61, 0x90, 0xA3, 0x28, 0xDD, 0x65, 0x25, 0xD9, 0x7B, 0x90, 0x58, 0xD9, 0x86, 0x40, 0xB0, 0xFA, 0x05, 0x02, 0x03, 0x01, 0x00, 0x01, } , //01
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xC3, 0xC8, 0xD8, 0x9F, 0x55, 0xD6, 0xA2, 0x36, 0xA1, 0x15, 0xC7, 0x75, 0x94, 0xD4, 0xB3, 0x18, 0xF0, 0xA0, 0xA0, 0xE3, 0x25, 0x2C, 0xC0, 0xD6, 0x34, 0x5E, 0xB9, 0xE3, 0x3A, 0x43, 0xA5, 0xA5, 0x6D, 0xC9, 0xD1, 0x0B, 0x7B, 0x59, 0xC1, 0x35, 0x39, 0x61, 0x59, 0xEC, 0x4D, 0x01, 0xDE, 0xBC, 0x5F, 0xB3, 0xA4, 0xCA, 0xE4, 0x78, 0x53, 0xE2, 0x05, 0xFE, 0x08, 0x98, 0x2D, 0xFC, 0xC0, 0xC3, 0x9F, 0x05, 0x57, 0x44, 0x9F, 0x97, 0xD4, 0x1F, 0xED, 0x13, 0xB8, 0x86, 0xAE, 0xBE, 0xEA, 0x91, 0x8F, 0x47, 0x67, 0xE8, 0xFB, 0xE0, 0x49, 0x4F, 0xFF, 0x6F, 0x6E, 0xE3, 0x50, 0x8E, 0x3A, 0x3F, 0x02, 0x03, 0x01, 0x00, 0x01, } , //02
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB6, 0x1E, 0x19, 0x20, 0x91, 0xF9, 0x0A, 0x8F, 0x76, 0xA6, 0xEA, 0xAA, 0x9A, 0x3C, 0xE5, 0x8C, 0x86, 0x3F, 0x39, 0xAE, 0x25, 0x3F, 0x03, 0x78, 0x16, 0xF5, 0x97, 0x58, 0x54, 0xE0, 0x7A, 0x9A, 0x45, 0x66, 0x01, 0xE7, 0xC9, 0x4C, 0x29, 0x75, 0x9F, 0xE1, 0x55, 0xC0, 0x64, 0xED, 0xDF, 0xA1, 0x11, 0x44, 0x3F, 0x81, 0xEF, 0x1A, 0x42, 0x8C, 0xF6, 0xCD, 0x32, 0xF9, 0xDA, 0xC9, 0xD4, 0x8E, 0x94, 0xCF, 0xB3, 0xF6, 0x90, 0x12, 0x0E, 0x8E, 0x6B, 0x91, 0x11, 0xAD, 0xDA, 0xF1, 0x1E, 0x7C, 0x96, 0x20, 0x8C, 0x37, 0xC0, 0x14, 0x3F, 0xF2, 0xBF, 0x3D, 0x7E, 0x83, 0x11, 0x41, 0xA9, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01, } , //03
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xA0, 0xF2, 0xAC, 0x80, 0xB4, 0x08, 0xE2, 0xE4, 0xD5, 0x89, 0x16, 0xA1, 0xC7, 0x06, 0xBE, 0xE7, 0xA2, 0x47, 0x58, 0xA6, 0x2C, 0xE9, 0xB5, 0x0A, 0xF1, 0xB3, 0x14, 0x09, 0xDF, 0xCB, 0x38, 0x2E, 0x88, 0x5A, 0xA8, 0xBB, 0x8C, 0x0E, 0x4A, 0xD1, 0xBC, 0xF6, 0xFF, 0x64, 0xFB, 0x30, 0x37, 0x75, 0x7D, 0x2B, 0xEA, 0x10, 0xE4, 0xFE, 0x90, 0x07, 0xC8, 0x50, 0xFF, 0xDC, 0xF7, 0x0D, 0x2A, 0xFA, 0xA4, 0xC5, 0x3F, 0xAF, 0xE3, 0x8A, 0x99, 0x17, 0xD4, 0x67, 0x86, 0x2F, 0x50, 0xFE, 0x37, 0x59, 0x27, 0xEC, 0xFE, 0xF4, 0x33, 0xE6, 0x1B, 0xF8, 0x17, 0xA6, 0x45, 0xFA, 0x56, 0x65, 0xD9, 0xCF, 0x02, 0x03, 0x01, 0x00, 0x01, } , //04 本番
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xD0, 0x46, 0xF2, 0x87, 0x28, 0x68, 0xA5, 0x08, 0x92, 0x05, 0xB2, 0x26, 0xDE, 0x13, 0xD8, 0x6D, 0xA5, 0x52, 0x64, 0x6A, 0xC1, 0x52, 0xC8, 0x46, 0x15, 0xBE, 0x8E, 0x0A, 0x58, 0x97, 0xC3, 0xEA, 0x45, 0x87, 0x10, 0x28, 0xF4, 0x51, 0x86, 0x0E, 0xA2, 0x26, 0xD5, 0x3B, 0x68, 0xDD, 0xD5, 0xA7, 0x7D, 0x1A, 0xD8, 0x2F, 0xAF, 0x85, 0x7E, 0xA5, 0x2C, 0xF7, 0x93, 0x31, 0x12, 0xEE, 0xC3, 0x67, 0xA0, 0x6C, 0x07, 0x61, 0xE5, 0x80, 0xD3, 0xD7, 0x0B, 0x6B, 0x9C, 0x83, 0x7B, 0xAA, 0x3F, 0x16, 0xD1, 0xFF, 0x7A, 0xA2, 0x0D, 0x87, 0xA2, 0xA5, 0xE2, 0xBC, 0xC6, 0xE3, 0x83, 0xBF, 0x12, 0xD5, 0x02, 0x03, 0x01, 0x00, 0x01, } , //05
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xD3, 0x79, 0x91, 0x90, 0x01, 0xD7, 0xFF, 0x40, 0xAC, 0x59, 0xDF, 0x47, 0x5C, 0xF6, 0xC6, 0x36, 0x8B, 0x19, 0x58, 0xDD, 0x4E, 0x87, 0x0D, 0xFD, 0x1C, 0xE1, 0x12, 0x18, 0xD5, 0xEA, 0x9D, 0x88, 0xDD, 0x7A, 0xD5, 0x30, 0xE2, 0x80, 0x6B, 0x0B, 0x09, 0x2C, 0x02, 0xE2, 0x5D, 0xB0, 0x92, 0x51, 0x89, 0x08, 0xED, 0xA5, 0x74, 0xA0, 0x96, 0x8D, 0x49, 0xB0, 0x50, 0x39, 0x54, 0xB2, 0x42, 0x84, 0xFA, 0x75, 0x44, 0x5A, 0x07, 0x4C, 0xE6, 0xE1, 0xAB, 0xCE, 0xC8, 0xFD, 0x01, 0xDA, 0xA0, 0xD2, 0x1A, 0x0D, 0xD9, 0x7B, 0x41, 0x7B, 0xC3, 0xE5, 0x4B, 0xEB, 0x72, 0x53, 0xFC, 0x06, 0xD3, 0xF3, 0x02, 0x03, 0x01, 0x00, 0x01, } , //06
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB7, 0x51, 0xCB, 0x7D, 0x28, 0x26, 0x25, 0xF2, 0x96, 0x1A, 0x71, 0x38, 0x65, 0x0A, 0xBE, 0x1A, 0x6A, 0xA8, 0x0D, 0x69, 0x54, 0x8B, 0xA3, 0xAE, 0x9D, 0xFF, 0x06, 0x5B, 0x28, 0x05, 0xEB, 0x36, 0x75, 0xD9, 0x60, 0xC6, 0x20, 0x96, 0xC2, 0x83, 0x5B, 0x1D, 0xF1, 0xC2, 0x90, 0xFC, 0x19, 0x41, 0x19, 0x44, 0xAF, 0xDF, 0x34, 0x58, 0xE3, 0xB1, 0xBC, 0x81, 0xA9, 0x8C, 0x3F, 0x3E, 0x95, 0xD0, 0xEE, 0x0C, 0x20, 0xA0, 0x25, 0x9E, 0x61, 0x43, 0x99, 0x40, 0x43, 0x54, 0xD9, 0x0F, 0x0C, 0x69, 0x11, 0x1A, 0x4E, 0x52, 0x5F, 0x42, 0x5F, 0xBB, 0x31, 0xA3, 0x8B, 0x8C, 0x55, 0x8F, 0x23, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01, } , //07
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB3, 0x28, 0xFE, 0x4C, 0xC4, 0x16, 0x27, 0x88, 0x2B, 0x04, 0xFB, 0xA0, 0xA3, 0x96, 0xA1, 0x52, 0x85, 0xA8, 0x56, 0x4B, 0x61, 0x12, 0xC1, 0x20, 0x30, 0x48, 0x76, 0x6D, 0x82, 0x7E, 0x8E, 0x4E, 0x56, 0x55, 0xD4, 0x4B, 0x26, 0x6B, 0x28, 0x36, 0x57, 0x5A, 0xE6, 0x8C, 0x83, 0x01, 0x63, 0x2A, 0x3E, 0x58, 0xB1, 0xF4, 0x36, 0x21, 0x31, 0xE9, 0x7B, 0x0A, 0xA0, 0xAF, 0xC3, 0x8F, 0x2F, 0x76, 0x90, 0xCB, 0xD4, 0xF3, 0xF4, 0x65, 0x20, 0x72, 0xBF, 0xD8, 0xE9, 0x42, 0x1D, 0x2B, 0xEE, 0xF1, 0x77, 0x87, 0x3C, 0xD7, 0xD0, 0x8B, 0x6C, 0x0D, 0x10, 0x22, 0x10, 0x9C, 0xA3, 0xED, 0x5B, 0x63, 0x02, 0x03, 0x01, 0x00, 0x01, } , //08
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xC4, 0xB3, 0x2F, 0xD1, 0x16, 0x1C, 0xC3, 0x0D, 0x04, 0xBD, 0x56, 0x9F, 0x40, 0x9E, 0x87, 0x8A, 0xA2, 0x81, 0x5C, 0x91, 0xDD, 0x00, 0x9A, 0x5A, 0xE8, 0xBF, 0xDA, 0xEA, 0x7D, 0x11, 0x6B, 0xF2, 0x49, 0x66, 0xBF, 0x10, 0xFC, 0xC0, 0x01, 0x4B, 0x25, 0x8D, 0xFE, 0xF6, 0x61, 0x4E, 0x55, 0xFB, 0x6D, 0xAB, 0x23, 0x57, 0xCD, 0x6D, 0xF5, 0xB6, 0x3A, 0x5F, 0x05, 0x9F, 0x72, 0x44, 0x69, 0xC0, 0x17, 0x8D, 0x83, 0xF8, 0x8F, 0x45, 0x04, 0x89, 0x82, 0xEA, 0xE7, 0xA7, 0xCC, 0x24, 0x9F, 0x84, 0x66, 0x7F, 0xC3, 0x93, 0x68, 0x4D, 0xA5, 0xEF, 0xE1, 0x85, 0x6E, 0xB1, 0x00, 0x27, 0xD1, 0xD7, 0x02, 0x03, 0x01, 0x00, 0x01, } , //09
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xC5, 0xB7, 0x54, 0x01, 0xE8, 0x33, 0x52, 0xA6, 0x4E, 0xEC, 0x89, 0x16, 0xC4, 0x20, 0x6F, 0x17, 0xEC, 0x33, 0x8A, 0x24, 0xA6, 0xF7, 0xFD, 0x51, 0x52, 0x60, 0x69, 0x6D, 0x72, 0x28, 0x49, 0x6A, 0xBC, 0x14, 0x23, 0xE1, 0xFF, 0x30, 0x51, 0x41, 0x49, 0xFC, 0x19, 0x97, 0x20, 0xE9, 0x5E, 0x68, 0x25, 0x39, 0x89, 0x2E, 0x51, 0x0B, 0x23, 0x9A, 0x8C, 0x7A, 0x41, 0x3D, 0xE4, 0xEE, 0xE7, 0x45, 0x94, 0xF0, 0x73, 0x81, 0x5E, 0x9B, 0x43, 0x47, 0x11, 0xF6, 0x80, 0x7E, 0x8B, 0x9E, 0x7C, 0x10, 0xC2, 0x81, 0xF8, 0x9C, 0xF3, 0xB1, 0xC1, 0x4E, 0x3F, 0x0A, 0xDF, 0x83, 0xA2, 0x80, 0x5F, 0x09, 0x02, 0x03, 0x01, 0x00, 0x01, } , //10
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xAC, 0x36, 0xB8, 0x8D, 0x00, 0xC3, 0x99, 0xC6, 0x60, 0xB4, 0x84, 0x62, 0x87, 0xFF, 0xC7, 0xF9, 0xDF, 0x5C, 0x07, 0x48, 0x7E, 0xAA, 0xE3, 0xCD, 0x4E, 0xFD, 0x00, 0x29, 0xD3, 0xB8, 0x6E, 0xD3, 0x65, 0x8A, 0xD7, 0xDE, 0xE4, 0xC7, 0xF5, 0xDA, 0x25, 0xF9, 0xF6, 0x00, 0x88, 0x85, 0xF3, 0x43, 0x12, 0x22, 0x74, 0x99, 0x4C, 0xAB, 0x64, 0x77, 0x76, 0xF0, 0xAD, 0xCF, 0xBA, 0x1E, 0x0E, 0xCE, 0xC8, 0xBF, 0x57, 0xCA, 0xAB, 0x84, 0x88, 0xBD, 0xD5, 0x9A, 0x55, 0x19, 0x5A, 0x01, 0x67, 0xC7, 0xD2, 0xC4, 0xA9, 0xCF, 0x67, 0x9D, 0x0E, 0xFF, 0x4A, 0x62, 0xB5, 0xC8, 0x56, 0x8E, 0x09, 0x77, 0x02, 0x03, 0x01, 0x00, 0x01, } , //11
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xCA, 0xC0, 0x51, 0x4D, 0x4B, 0x6A, 0x3F, 0x70, 0x77, 0x1C, 0x46, 0x1B, 0x01, 0xBD, 0xE3, 0xB6, 0xD4, 0x7A, 0x0A, 0xDA, 0x07, 0x80, 0x74, 0xDD, 0xA5, 0x07, 0x03, 0xD8, 0xCC, 0x28, 0x08, 0x93, 0x79, 0xDA, 0x64, 0xFB, 0x3A, 0x34, 0xAD, 0x34, 0x35, 0xD2, 0x4F, 0x73, 0x31, 0x38, 0x3B, 0xDA, 0xDC, 0x48, 0x77, 0x66, 0x2E, 0xFB, 0x55, 0x5D, 0xA2, 0x07, 0x76, 0x19, 0xB7, 0x0A, 0xB0, 0x34, 0x2E, 0xBE, 0x6E, 0xE8, 0x88, 0xEB, 0xF3, 0xCF, 0x4B, 0x7E, 0x8B, 0xCC, 0xA9, 0x5C, 0x61, 0xE9, 0x93, 0xBD, 0xD6, 0x10, 0x4C, 0x10, 0xD1, 0x11, 0x15, 0xDC, 0x84, 0x17, 0x8A, 0x58, 0x94, 0x35, 0x02, 0x03, 0x01, 0x00, 0x01, } , //12
  { 0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB9, 0x06, 0x46, 0x67, 0x40, 0xF5, 0xA9, 0x42, 0x8D, 0xA8, 0x4B, 0x41, 0x8C, 0x7F, 0xA6, 0x14, 0x6F, 0x7E, 0x24, 0xC7, 0x83, 0x37, 0x3D, 0x67, 0x1F, 0x92, 0x14, 0xB4, 0x09, 0x48, 0xA4, 0xA3, 0x17, 0xC1, 0xA4, 0x46, 0x01, 0x11, 0xB4, 0x5D, 0x2D, 0xAD, 0xD0, 0x93, 0x81, 0x54, 0x01, 0x57, 0x3E, 0x52, 0xF0, 0x17, 0x88, 0x90, 0xD3, 0x5C, 0xBD, 0x95, 0x71, 0x2E, 0xFA, 0xAE, 0x0D, 0x20, 0xAD, 0x47, 0x18, 0x76, 0x48, 0x77, 0x5C, 0xD9, 0x56, 0x94, 0x31, 0xB1, 0xFC, 0x3C, 0x78, 0x41, 0x13, 0xE3, 0xA4, 0x84, 0x36, 0xD3, 0x0B, 0x2C, 0xD1, 0x62, 0x21, 0x8D, 0x67, 0x81, 0xF5, 0xED, 0x02, 0x03, 0x01, 0x00, 0x01, } , //04 dev
};

//------------------------------------------------------------------
/**
  * @brief 署名付加
  */
//------------------------------------------------------------------
void QRUtility::SignBinary( void* signature , const void* message , u32 messageLen , gfl2::heap::HeapBase* pHeap )
{
#if defined(GF_PLATFORM_CTR)

  //type03
  const u8 privateKeyData[] =
  {
      0x30, 0x82, 0x01, 0xC9, 0x02, 0x01, 0x00, 0x02, 0x61, 0x00, 0xB6, 0x1E, 0x19, 0x20, 0x91, 0xF9, 0x0A, 0x8F, 0x76, 0xA6, 0xEA, 0xAA, 0x9A, 0x3C, 0xE5, 0x8C, 0x86, 0x3F, 0x39, 0xAE, 0x25, 0x3F, 0x03, 0x78, 0x16, 0xF5, 0x97, 0x58, 0x54, 0xE0, 0x7A, 0x9A, 0x45, 0x66, 0x01, 0xE7, 0xC9, 0x4C, 0x29, 0x75, 0x9F, 0xE1, 0x55, 0xC0, 0x64, 0xED, 0xDF, 0xA1, 0x11, 0x44, 0x3F, 0x81, 0xEF, 0x1A, 0x42, 0x8C, 0xF6, 0xCD, 0x32, 0xF9, 0xDA, 0xC9, 0xD4, 0x8E, 0x94, 0xCF, 0xB3, 0xF6, 0x90, 0x12, 0x0E, 0x8E, 0x6B, 0x91, 0x11, 0xAD, 0xDA, 0xF1, 0x1E, 0x7C, 0x96, 0x20, 0x8C, 0x37, 0xC0, 0x14, 0x3F, 0xF2, 0xBF, 0x3D, 0x7E, 0x83, 0x11, 0x41, 0xA9, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x60, 0x77, 0x54, 0x55, 0x66, 0x8F, 0xFF, 0x3C, 0xBA, 0x30, 0x26, 0xC2, 0xD0, 0xB2, 0x6B, 0x80, 0x85, 0x89, 0x59, 0x58, 0x34, 0x11, 0x57, 0xAE, 0xB0, 0x3B, 0x6B, 0x04, 0x95, 0xEE, 0x57, 0x80, 0x3E, 0x21, 0x86, 0xEB, 0x6C, 0xB2, 0xEB, 0x62, 0xA7, 0x1D, 0xF1, 0x8A, 0x3C, 0x9C, 0x65, 0x79, 0x07, 0x76, 0x70, 0x96, 0x1B, 0x3A, 0x61, 0x02, 0xDA, 0xBE, 0x5A, 0x19, 0x4A, 0xB5, 0x8C, 0x32, 0x50, 0xAE, 0xD5, 0x97, 0xFC, 0x78, 0x97, 0x8A, 0x32, 0x6D, 0xB1, 0xD7, 0xB2, 0x8D, 0xCC, 0xCB, 0x2A, 0x3E, 0x01, 0x4E, 0xDB, 0xD3, 0x97, 0xAD, 0x33, 0xB8, 0xF2, 0x8C, 0xD5, 0x25, 0x05, 0x42, 0x51, 0x02, 0x31, 0x00, 0xEE, 0x48, 0x58, 0xBC, 0x36, 0x7E, 0xFD, 0xE4, 0x2A, 0x6C, 0x0E, 0x15, 0x7E, 0xAB, 0x32, 0x7B, 0x48, 0x84, 0x5E, 0x55, 0xB7, 0xBA, 0x8E, 0x0A, 0x1D, 0x17, 0xCC, 0x53, 0xC4, 0x66, 0x84, 0x59, 0xD7, 0x90, 0xE9, 0xFB, 0xD9, 0x0D, 0x50, 0x18, 0x47, 0x88, 0xB2, 0xF3, 0xE6, 0x26, 0xC8, 0xC5, 0x02, 0x31, 0x00, 0xC3, 0xA8, 0xA9, 0xCF, 0x94, 0x1E, 0xDB, 0x0B, 0xDF, 0x2D, 0x57, 0x2D, 0xBD, 0xC3, 0x9D, 0x00, 0xBA, 0x1A, 0x9F, 0xB7, 0xC1, 0xFC, 0x6A, 0xC4, 0xF4, 0x39, 0xF9, 0x9A, 0x33, 0x8A, 0xCC, 0x8D, 0x5A, 0x78, 0x3C, 0x4C, 0x08, 0x1A, 0xC8, 0xB9, 0xB9, 0xDA, 0x8E, 0x89, 0xF2, 0x6B, 0x9C, 0xD7, 0x02, 0x30, 0x5F, 0x8D, 0xC1, 0x8F, 0xD0, 0x66, 0xC2, 0x97, 0xC1, 0xEF, 0x4D, 0x02, 0x8F, 0xA6, 0x45, 0x7B, 0x2C, 0xCC, 0x70, 0x2B, 0xFD, 0x9E, 0xA1, 0x21, 0x2B, 0x23, 0x84, 0xFB, 0xBE, 0x91, 0xC1, 0xD7, 0x69, 0x5B, 0xA2, 0x89, 0x2D, 0x34, 0x6A, 0x7F, 0x22, 0x08, 0x8E, 0x82, 0xC2, 0x33, 0xA1, 0x65, 0x02, 0x30, 0x3E, 0x7E, 0xB0, 0x06, 0xAA, 0x94, 0x14, 0x79, 0x3D, 0x46, 0xD9, 0x32, 0xB6, 0x0B, 0x36, 0x29, 0xD6, 0x47, 0xCC, 0xBE, 0x40, 0x0A, 0xE4, 0xFB, 0x95, 0xDA, 0xB2, 0xBC, 0x2B, 0x3C, 0x69, 0x4B, 0xB9, 0x1A, 0xF0, 0x7F, 0x84, 0x93, 0x31, 0x8C, 0x09, 0x2E, 0x23, 0x1B, 0x32, 0xC0, 0x66, 0xAF, 0x02, 0x30, 0x68, 0xC2, 0xD3, 0x45, 0x0C, 0x8E, 0x66, 0x1C, 0xAA, 0x82, 0xF1, 0x8C, 0xF1, 0xEF, 0x9B, 0xEC, 0xB7, 0xAF, 0xD1, 0x11, 0xDF, 0xCC, 0xEA, 0x6E, 0xA6, 0x09, 0xFC, 0x23, 0xFA, 0x7B, 0x09, 0x9C, 0x23, 0xA4, 0x05, 0x5D, 0xF6, 0x3E, 0x1F, 0x59, 0x28, 0xE9, 0x45, 0xC1, 0x17, 0xBD, 0xFD, 0xEE
  };

  u32 signatureLen = messageLen + CRYPTO_OVERHEAD;
  u8* pTempBuffer = GFL_NEW_ARRAY( pHeap ) u8[SIGN_VERIFY_BUFFER_LEN];
  bool result = mw::zorro::OpssrCmcSha1_Sign( reinterpret_cast<u8*>(signature) , signatureLen , reinterpret_cast<const u8*>(message) , messageLen , CRYPTO_OVERHEAD , s_publicKeyData[3], sizeof(s_publicKeyData[3]),
                                          privateKeyData, sizeof(privateKeyData), MODULE_BITS_LEN ,
                                          pTempBuffer , SIGN_VERIFY_BUFFER_LEN );
  GFL_SAFE_DELETE_ARRAY( pTempBuffer );
  GFL_PRINT("QRUtility::SignBinary sign [%d]\n",result);
  GFL_ASSERT( result );

#endif
}

//------------------------------------------------------------------
/**
  * @brief 署名検証
  */
//------------------------------------------------------------------
bool QRUtility::VerifyBinary( const void* signature , u32 signatureLen , void* recoveredMessage , gfl2::heap::HeapBase* pHeap , QRUtility::QR_CRYPTO_TYPE type )
{
#if defined(GF_PLATFORM_CTR)

  u32 recoveredMessageLen = signatureLen - CRYPTO_OVERHEAD;
  u8* pTempBuffer = GFL_NEW_ARRAY( pHeap ) u8[SIGN_VERIFY_BUFFER_LEN];
  bool result = mw::zorro::OpssrCmcSha1_Verify( reinterpret_cast<u8*>(recoveredMessage), recoveredMessageLen,
                                            reinterpret_cast<const u8*>(signature), signatureLen, CRYPTO_OVERHEAD,
                                            s_publicKeyData[type], sizeof(s_publicKeyData[type]), s_publicKeyData[type],
                                            sizeof(s_publicKeyData[type]), MODULE_BITS_LEN ,
                                            pTempBuffer , SIGN_VERIFY_BUFFER_LEN );

  GFL_SAFE_DELETE_ARRAY( pTempBuffer );
  GFL_PRINT("QRUtility::VerifyBinary verify [%d]\n",result);
  if( result )
  {
    GFL_PRINT("QRUtility::VerifyBinary verify OK\n");
  }
  else
  {
    GFL_PRINT("QRUtility::VerifyBinary verify NG\n");
  }

  return result;
#else
  return true;
#endif
}

//------------------------------------------------------------------

//------------------------------------------------------------------

//------------------------------------------------------------------
/**
  * @brief 図鑑QR用処理
  */
//------------------------------------------------------------------
QR_TYPE_APP QRUtility::ZukanQRFunc( QR_ZUKAN* pZukanQR , pml::pokepara::CoreParam* pOutput , gfl2::heap::HeapBase* pHeap )
{
  Savedata::ZukanData* pZukan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  MonsNo monsNo = static_cast<MonsNo>( pZukanQR->monsNo );
  pml::Sex sex = static_cast<pml::Sex>( pZukanQR->dispSex );
  FormNo formNo = pZukanQR->dispForm;
  u8 isRare = pZukanQR->dispRare;
  u32 rand = 0;
  if( sex == pml::SEX_FEMALE && isRare == false )
  {
    rand = pZukanQR->rand[QR_ZUKAN::SEE_FLAG_FEMALE];
  }
  else if( sex == pml::SEX_FEMALE && isRare == true )
  {
    rand = pZukanQR->rand[QR_ZUKAN::SEE_FLAG_F_RARE];
  }
  else if( isRare == false )
  {
    rand = pZukanQR->rand[QR_ZUKAN::SEE_FLAG_MALE];
  }
  else
  {
    rand = pZukanQR->rand[QR_ZUKAN::SEE_FLAG_M_RARE];
  }

  GFL_PRINT( "ZukanQRFunc:: Create Pokemon Setup MonsNo[%d] FormNo[%d] Sex[%d] Rare[%d]\n" ,monsNo,formNo,sex,isRare);

  pml::pokepara::InitialSpec spec;
  spec.monsno = monsNo;
  spec.formno = formNo;
  spec.sex = sex;
  if( isRare == 0 )
  {
    spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;//レアにしない
  }
  else
  {
    spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_TRUE;//レアにする
  }
  spec.personal_rnd = rand;

  pml::pokepara::CoreParam* pCreateParam = GFL_NEW( pHeap ) pml::pokepara::CoreParam( pHeap , spec );

  pOutput->CopyFrom( *pCreateParam );

  GFL_PRINT( "ZukanQRFunc:: Create Pokemon Param MonsNo[%d] FormNo[%d] Sex[%d] Rare[%d]\n" ,pOutput->GetMonsNo(),pOutput->GetFormNo(),pOutput->GetSex(),pOutput->IsRare());

  GFL_SAFE_DELETE( pCreateParam );

  //図鑑情報マージ
  if( pZukan->GetPokeSeeFlag( static_cast<MonsNo>( pZukanQR->monsNo ) ) )
  {
    NetAppLib::Util::NetAppCommonSaveUtility::SetZukanSavePokeSee( pZukan , *pZukanQR );
    return QR_TYPE_APP_ALOLA_UPDATE;
  }
  else
  {
    NetAppLib::Util::NetAppCommonSaveUtility::SetZukanSavePokeSee( pZukan , *pZukanQR );
    return QR_TYPE_APP_ALOLA_NEW;
  }
}

//------------------------------------------------------------------
/**
  * @brief QRタイプ判別
  */
//------------------------------------------------------------------
QRUtility::QR_CRYPTO_TYPE QRUtility::CheckQRType( const void* pBinary , u32 size )
{
  switch( size )
  {//まずサイズで判断する
  case sizeof(QR_TYPE_MIN):
  case sizeof(QR_ZUKAN):
  case sizeof(QR_TYPE_MAX):
  break;
  default:
    return QR_CRYPTO_TYPE_NONE;
  }

  return CheckFooter( pBinary , size );
}

//------------------------------------------------------------------
/**
  * @brief フッターチェック
  */
//------------------------------------------------------------------
QRUtility::QR_CRYPTO_TYPE QRUtility::CheckFooter( const void* pBinary , u32 size )
{
  char tempBuff[ FOOTER_SIZE + 1 ];
  ::std::memset( tempBuff , 0 , sizeof(tempBuff) );

  const u8* pFooter = reinterpret_cast<const u8*>(pBinary);
  ::std::memcpy( tempBuff , &pFooter[size-FOOTER_SIZE] , FOOTER_SIZE );

  GFL_PRINT("QRUtility::CheckFooter : footer[%s]\n" , tempBuff );
  GFL_PRINT("QRUtility::CheckFooter : type[%d]\n" , tempBuff[4] );

  if( ::std::memcmp( tempBuff , "POKE" , 4 ) == 0 )
  {//シグネチャ一致
    u32 type = tempBuff[4];//タイプが入っている

    if( type < QR_CRYPTO_TYPE_NONE )
    {
      GFL_PRINT("QRUtility::CheckFooter : check OK\n");
      return static_cast<QR_CRYPTO_TYPE>( type );
    }
  }

  GFL_PRINT("QRUtility::CheckFooter : check NG\n");

  return QR_CRYPTO_TYPE_NONE;
}

//------------------------------------------------------------------
/**
  * @brief 一般QRポケモン生成
  */
//------------------------------------------------------------------
void QRUtility::CreateNormalQRPoke( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , pml::pokepara::CoreParam* pOutput )
{
  gfl2::crypto::ISHA256Calculator* pSHA256Calculator = gfl2::crypto::ISHA256Calculator::CreateSHA256Calculator( pHeap, 256, System::ThreadPriority::ASYNC_SHA256_CRYPTO_THREAD_PRIORITY );

  u64 value = 0;

  gfl2::crypto::SHA256Hash* pSHA256Hash = pSHA256Calculator->CalcHash( pBinary, size );
  memcpy(&value, pSHA256Hash->GetData(), sizeof(value) );

  GFL_SAFE_DELETE( pSHA256Calculator );

  u64 hitValue = value % QR_POKE_TABLE_TOTAL;
  u32 tableIndex = 0;
  u64 totalValue = 0;

  GFL_PRINT( "CreateNormalQRPoke::hit value[%llu]\n" ,hitValue);

  for ever
  {
    totalValue += QR_POKE_TABLE[tableIndex][QR_POKE_TABLE_HIT_VALUE];

    if( hitValue < totalValue )
    {
      break;
    }

    if( tableIndex + 1 >= QR_POKE_TABLE_NUM )
    {
      GFL_ASSERT( 0 );//@fix
      break;
    }

    tableIndex++;
  }

  MonsNo monsNo = static_cast<MonsNo>( QR_POKE_TABLE[tableIndex][QR_POKE_TABLE_MONSNO] );
  pml::Sex sex = static_cast<pml::Sex>( QR_POKE_TABLE[tableIndex][QR_POKE_TABLE_SEX] );
  FormNo formNo = QR_POKE_TABLE[tableIndex][QR_POKE_TABLE_FORMNO];

  GFL_PRINT( "CreateNormalQRPoke::Create Pokemon MonsNo[%d] FormNo[%d] Sex[%d]\n" ,monsNo,formNo,sex);

  pml::pokepara::InitialSpec spec;
  spec.monsno = monsNo;
  spec.formno = formNo;
  spec.sex = sex;
  spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;//レアにしない

  pml::pokepara::CoreParam* pCreateParam = GFL_NEW( pHeap ) pml::pokepara::CoreParam( pHeap , spec );

  pOutput->CopyFrom( *pCreateParam );

  GFL_SAFE_DELETE( pCreateParam );
}

//------------------------------------------------------------------
/**
  * @brief バトルチームQR用複合化
  */
//------------------------------------------------------------------
void QRUtility::DecryptAesForBattleTeamQR( const void* cryptMessage , u32 cryptMessageSize , void* recoveredMessage )
{
#if defined(GF_PLATFORM_CTR)
  const u8 AES_KEY[] = { 0x0F , 0x8E , 0x2F , 0x40 , 0x5E , 0xAE , 0x51 , 0x50 , 0x4E , 0xDB , 0xA7 , 0xB4 , 0xE2 , 0x97 , 0x00 , 0x5B };

  nn::crypto::DecryptAes128Ctr( recoveredMessage , cryptMessage , cryptMessageSize , AES_KEY );
#endif
}


GFL_NAMESPACE_END(QR)
GFL_NAMESPACE_END(NetApp)


