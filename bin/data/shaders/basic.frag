#version 120

//uniform sampler2DRect tex; //参照するテクスチャ
//uniform sampler2DRect maskTex;//マスク用のテクスチャ
//
//uniform int mode; //外部から設定したいパラメータ
//uniform float offsetX; //テクスチャ座標のオフセットを設定
//uniform float offsetY;
//
//uniform float maskX;
//uniform float maskY;


uniform vec2 resolution;
uniform float offset;

float rings(vec2 p)
{
    return sin(length(p)*16.0);
}

void main()
{
    vec2 pos = (gl_FragCoord.xy * offset -resolution) / resolution.y;
    gl_FragColor = vec4(rings(pos));
}