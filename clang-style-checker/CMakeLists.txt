set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Option
  Support
  )

add_clang_tool(clang-style-checker ClangStyleChecker.cpp)

target_link_libraries(clang-style-checker
  PRIVATE
  clangBasic
  clangFrontend
  clangRewrite
  clangTooling
  clangToolingCore
  clangToolingRefactoring
  clangAST
  clangDriver
  clangRewriteFrontend
  clangStaticAnalyzerFrontend
  )

install(TARGETS clang-style-checker
  RUNTIME DESTINATION bin)