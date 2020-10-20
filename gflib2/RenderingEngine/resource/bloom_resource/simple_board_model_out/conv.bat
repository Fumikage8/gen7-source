@echo ==============================================================================
@echo ==============================================================================
@echo ==============================================================================
@echo ==                                                                          ==
@echo == ATTENTION!! : You have to write { RepeatU="BORDER" RepeatV="BORDER" } !! ==
@echo ==                                                                          ==
@echo ==============================================================================
@echo ==============================================================================
@echo ==============================================================================

mkdir binary
cd binary
mkdir wingl
mkdir ctr
cd ..

R:/home/niji/tool/ModelConverter/ModelConverter.exe -i "exports/simple_board_model.gfmdl" -o "binary/wingl/simple_board_model.wingl.gfbmdl"
R:/home/niji/tool/ModelConverter/ModelConverter.exe -i "exports/simple_board_model.gfmdl" -o "binary/ctr/simple_board_model.ctr.gfbmdl"

R:/home/niji/tool/ShaderCompiler/ShaderCompiler.exe -vsh "exports/Shaders/FillScrean.gfvsh" -fsh "exports/Shaders/Comb1_Manual.gffsh" -o "binary/wingl/FillScrean.wingl.gfbvsh" -def HLSL -def 1
R:/home/niji/tool/ShaderCompiler/ShaderCompiler.exe -vsh "exports/Shaders/FillScrean.gfvsh" -fsh "exports/Shaders/Comb1_Manual.gffsh" -o "binary/wingl/Comb1_Manual.wingl.gfbfsh" -def HLSL -def 1
R:/home/niji/tool/ShaderCompiler/ShaderCompiler.exe -vsh "exports/Shaders/FillScrean.gfvsh" -fsh "exports/Shaders/Comb1_Manual.gffsh" -o "binary/ctr/FillScrean.ctr.gfbvsh" -def HLSL -def 1
R:/home/niji/tool/ShaderCompiler/ShaderCompiler.exe -vsh "exports/Shaders/FillScrean.gfvsh" -fsh "exports/Shaders/Comb1_Manual.gffsh" -o "binary/ctr/Comb1_Manual.ctr.gfbfsh" -def HLSL -def 1

R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "exports/Textures/simple_dummy_tex.tga" -o "binary/wingl/simple_dummy_tex.wingl.btex"
R:/home/niji/tool/TextureConverter/TextureConverter.exe -i "exports/Textures/simple_dummy_tex.tga" -o "binary/ctr/simple_dummy_tex.ctr.btex"

pause
