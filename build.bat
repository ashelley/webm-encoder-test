set CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib 	Gdi32.lib
pushd build

del *.pdb > NUL 2> NUL

cl %CommonCompilerFlags% ..\src\win32_encoder.cpp ..\src\EbmlSupport.cpp ..\src\EbmlWriter.cpp ..\src\VideoEncoder.cpp  /link %CommonLinkerFlags% ..\lib\vpx.lib
popd