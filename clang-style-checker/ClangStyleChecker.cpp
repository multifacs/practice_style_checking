#include <iostream>
#include <sstream>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Error.h>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class CastCallBack : public MatchFinder::MatchCallback {
public:
  CastCallBack(Rewriter& rewriter) : _rewriter(rewriter) {};
  void run(const MatchFinder::MatchResult& Result) override {
    const auto* cast_expr = Result.Nodes.getNodeAs<CStyleCastExpr>("cast");
    auto replace_range = CharSourceRange::getCharRange(cast_expr->getLParenLoc(),
      cast_expr->getSubExprAsWritten()->getBeginLoc());
    auto& source_manager = *Result.SourceManager;
    auto type_name = Lexer::getSourceText(CharSourceRange::getTokenRange(
      cast_expr->getLParenLoc().getLocWithOffset(1),
      cast_expr->getRParenLoc().getLocWithOffset(-1)),
      source_manager, LangOptions());
    const auto* expr = cast_expr->getSubExprAsWritten()->IgnoreImpCasts();
    auto new_txt_begin = ("static_cast<" + type_name + ">(").str();
    auto new_expr = Lexer::getLocForEndOfToken(expr->getEndLoc(), 0,
      source_manager, LangOptions());
    _rewriter.InsertText(new_expr, ")");

    _rewriter.ReplaceText(replace_range, new_txt_begin);
  }
private:
  Rewriter& _rewriter;
};

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &rewriter) : callback_(rewriter) {
    matcher_.addMatcher(
        cStyleCastExpr(unless(isExpansionInSystemHeader())).bind("cast"),
        &callback_);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    matcher_.matchAST(Context);
  }

private:
  CastCallBack callback_;
  MatchFinder matcher_;
};

class CStyleCheckerFrontendAction : public ASTFrontendAction {
public:
  CStyleCheckerFrontendAction() = default;

  void EndSourceFileAction() override {
    rewriter_.getEditBuffer(rewriter_.getSourceMgr().getMainFileID())
        .write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, StringRef /* file */) override {
    rewriter_.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<MyASTConsumer>(rewriter_);
  }

private:
  Rewriter rewriter_;
};

static llvm::cl::OptionCategory CastMatcherCategory("cast-matcher options");

int main(int argc, const char **argv) {
  CommonOptionsParser Parser(argc, argv, CastMatcherCategory);

  ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());
  return Tool.run(
      newFrontendActionFactory<CStyleCheckerFrontendAction>().get());
}