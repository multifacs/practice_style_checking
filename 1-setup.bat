if exist .\llvm-10.0.0.src.tar.xz (
    echo Skipping file, already downloaded
) else (
    powershell -Command "Invoke-WebRequest https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/llvm-10.0.0.src.tar.xz -OutFile llvm-10.0.0.src.tar.xz"
)

if exist .\clang-10.0.0.src.tar.xz (
    echo Skipping file, already downloaded
) else (
    powershell -Command "Invoke-WebRequest https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/clang-10.0.0.src.tar.xz -OutFile clang-10.0.0.src.tar.xz"
)

if exist .\llvm (
    echo Skipping, already moved
) else (
    if exist .\llvm-10.0.0.src (
        echo Skipping file, already extracted
    ) else (
        7z x "llvm-10.0.0.src.tar.xz" -so | 7z x -aoa -si -ttar
    )

    if exist .\clang-10.0.0.src (
        echo Skipping file, already downloaded
    ) else (
        7z x "clang-10.0.0.src.tar.xz" -so | 7z x -aoa -si -ttar
    )

    move .\llvm-10.0.0.src\ .\llvm
    move .\clang-10.0.0.src\ .\clang
    move .\clang\ .\llvm\tools\
)
