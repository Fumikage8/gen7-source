mkdir binary
cd binary
mkdir wingl
mkdir ctr
cd ..


R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_small_tex_rgba0.tga -o=bloom_small_tex_rgba0_nw.tga -f=RGBA4
R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_small_tex_rgba1.tga -o=bloom_small_tex_rgba1_nw.tga -f=RGBA4
R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_test_tex.tga -o=bloom_test_tex_nw.tga -f=RGBA4

R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_test_tex_w128xh128_disp.tga -o=bloom_test_tex_w128xh128_disp_nw.tga -f=RGBA4
R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_test_tex_w256xh256_disp.tga -o=bloom_test_tex_w256xh256_disp_nw.tga -f=RGBA4
R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_test_tex_w256xh512_disp.tga -o=bloom_test_tex_w256xh512_disp_nw.tga -f=RGBA4
R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_test_tex_w512xh256_disp.tga -o=bloom_test_tex_w512xh256_disp_nw.tga -f=RGBA4
R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_test_tex_w512xh512_disp.tga -o=bloom_test_tex_w512xh512_disp_nw.tga -f=RGBA4

R:/home/niji/niji_extlib/ctr/NW_CTR/textool/TextureConverter/NW4C_TextureConverter.exe bloom_alpha_mask_test_tex.tga -o=bloom_alpha_mask_test_tex_nw.tga -f=RGBA4

R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_small_tex_rgba0_nw.tga" -o "binary/wingl/bloom_small_tex_rgba0.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_small_tex_rgba0_nw.tga" -o "binary/ctr/bloom_small_tex_rgba0.ctr.btex"

R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_small_tex_rgba1_nw.tga" -o "binary/wingl/bloom_small_tex_rgba1.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_small_tex_rgba1_nw.tga" -o "binary/ctr/bloom_small_tex_rgba1.ctr.btex"

R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_nw.tga" -o "binary/wingl/bloom_test_tex.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_nw.tga" -o "binary/ctr/bloom_test_tex.ctr.btex"

R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w128xh128_disp_nw.tga" -o "binary/wingl/bloom_test_tex_w128xh128_disp.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w128xh128_disp_nw.tga" -o "binary/ctr/bloom_test_tex_w128xh128_disp.ctr.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w256xh256_disp_nw.tga" -o "binary/wingl/bloom_test_tex_w256xh256_disp.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w256xh256_disp_nw.tga" -o "binary/ctr/bloom_test_tex_w256xh256_disp.ctr.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w256xh512_disp_nw.tga" -o "binary/wingl/bloom_test_tex_w256xh512_disp.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w256xh512_disp_nw.tga" -o "binary/ctr/bloom_test_tex_w256xh512_disp.ctr.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w512xh256_disp_nw.tga" -o "binary/wingl/bloom_test_tex_w512xh256_disp.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w512xh256_disp_nw.tga" -o "binary/ctr/bloom_test_tex_w512xh256_disp.ctr.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w512xh512_disp_nw.tga" -o "binary/wingl/bloom_test_tex_w512xh512_disp.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_test_tex_w512xh512_disp_nw.tga" -o "binary/ctr/bloom_test_tex_w512xh512_disp.ctr.btex"

R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_alpha_mask_test_tex_nw.tga" -o "binary/wingl/bloom_alpha_mask_test_tex.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "bloom_alpha_mask_test_tex_nw.tga" -o "binary/ctr/bloom_alpha_mask_test_tex.ctr.btex"

pause
