copy .\clang-style-checker\ .\llvm\tools\clang\tools\
cd .\llvm\tools\clang\tools\

echo add_clang_subdirectory(clang-style-checker)>>CMakeLists.txt
cd .\..\..\..\..