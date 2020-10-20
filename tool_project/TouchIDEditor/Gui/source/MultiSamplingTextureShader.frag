uniform sampler2D Texture[3];//テクスチャユニット

// -------------------------------------------------------------
// 頂点プログラムからフラグメントプログラムに渡すデータ
// -------------------------------------------------------------
varying vec4 outUv;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------
uniform vec4 cPsMultiSamplingOffSet;

// ------------------------------------------------------------
// フラグメントシェーダプログラム
// ------------------------------------------------------------
void main (void)
{
	vec4				multiSamplingColor[9];
	vec4				outColor = vec4(0, 0, 0, 0);
	vec2				uv;
	float				alpha;

	//0,0
	uv.x = outUv.x - cPsMultiSamplingOffSet.x;
	uv.y = outUv.y - cPsMultiSamplingOffSet.y;
	multiSamplingColor[0] = texture2D( Texture[0], uv );

	//2,2
	uv.x = outUv.x + cPsMultiSamplingOffSet.x;
	uv.y = outUv.y + cPsMultiSamplingOffSet.y;
	multiSamplingColor[8] = texture2D( Texture[0], uv );

	//2,0
	uv.x = outUv.x + cPsMultiSamplingOffSet.x;
	uv.y = outUv.y - cPsMultiSamplingOffSet.y;
	multiSamplingColor[2] = texture2D( Texture[0], uv );

	//0,2
	uv.x = outUv.x - cPsMultiSamplingOffSet.x;
	uv.y = outUv.y + cPsMultiSamplingOffSet.y;
	multiSamplingColor[6] = texture2D( Texture[0], uv );

	//1,0
	uv.x = outUv.x;
	uv.y = outUv.y - cPsMultiSamplingOffSet.y;
	multiSamplingColor[1] = texture2D( Texture[0], uv );

	//1,2
	uv.x = outUv.x;
	uv.y = outUv.y + cPsMultiSamplingOffSet.y;
	multiSamplingColor[7] = texture2D( Texture[0], uv );

	//0,1
	uv.x = outUv.x - cPsMultiSamplingOffSet.x;
	uv.y = outUv.y;
	multiSamplingColor[3] = texture2D( Texture[0], uv );

	//2,1
	uv.x = outUv.x + cPsMultiSamplingOffSet.x;
	uv.y = outUv.y;
	multiSamplingColor[5] = texture2D( Texture[0], uv );

	//1,1
	uv.x = outUv.x;
	uv.y = outUv.y;
	multiSamplingColor[4] = texture2D( Texture[0], uv );

	for( int i = 0; i < 9; i ++ )
	{
		alpha = multiSamplingColor[i].w;
		outColor = (multiSamplingColor[i] * alpha) + (outColor * (1 - alpha));
	}

	gl_FragColor = outColor;
}
