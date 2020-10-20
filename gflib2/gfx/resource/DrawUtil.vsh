// SimpleDraw.vertから移植

// 入力レジスタ
#define inPosition      v0
#define inUv            v1
#define inColor         v2

#pragma bind_symbol(inPosition, v0, v0)
#pragma bind_symbol(inUv, v1, v1)
#pragma bind_symbol(inColor, v2, v2)

// 出力レジスタ
#define outPosition     o0
#define outUv           o1
#define outColor        o2

#pragma output_map (position, o0)
#pragma output_map (texture0, o1)
#pragma output_map (color, o2)

// 定数バッファ
#define cVsProjectionMatrix   c0 // -> c3
#define cVsModelViewMatrix    c4 // -> c7
#define cVsModelMatrix        c8 // -> c11
#define cMaterialColor        c12 // -> c12

#pragma bind_symbol(cVsProjectionMatrix, c0, c3)
#pragma bind_symbol(cVsModelViewMatrix, c4, c7)
#pragma bind_symbol(cVsModelMatrix, c8, c11)
#pragma bind_symbol(cMaterialColor, c12, c12)

// 一次変数
#define TEMP_VEC0             r0
#define TEMP_VEC1             r1

calc_position:
  m4x4 TEMP_VEC0, inPosition, cVsModelMatrix
  m4x4 TEMP_VEC1, TEMP_VEC0, cVsModelViewMatrix
  m4x4 outPosition, TEMP_VEC1, cVsProjectionMatrix
  ret
  nop

main:
  call calc_position
  mov outUv, inUv
//  mov outColor, inColor
  mul outColor, inColor, cMaterialColor

  end
endmain:
