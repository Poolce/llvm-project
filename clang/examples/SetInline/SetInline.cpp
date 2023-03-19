#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Attr.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"

using namespace clang;

namespace {

class PrintFunctionsConsumer : public ASTConsumer {
public:
  PrintFunctionsConsumer(){}
  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    for (auto D : DG) {
      if (FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
        auto body = FD->getBody();
        bool IsInline = true;
        for(auto i = body->child_begin(); i!= body->child_end();i++){
          if(IfStmt* ifIter = dyn_cast<IfStmt>(*i)){
            IsInline = false;
          }
        }
        if(IsInline)FD->addAttr(AlwaysInlineAttr::CreateImplicit(FD->getASTContext()));
      }
    }
    return true;
  }
  void HandleTranslationUnit(ASTContext &context) override {}
};

class PrintClassesNamesAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<PrintFunctionsConsumer>();
  }
  PluginASTAction::ActionType getActionType() override {
    return AddAfterMainAction;
  }
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

} // namespace

static FrontendPluginRegistry::Add<PrintClassesNamesAction>
    X("set-ai", "set always inline");
int main(){

}