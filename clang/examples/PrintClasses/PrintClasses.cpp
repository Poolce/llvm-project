#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

class PrintFunctionsConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;

public:
  PrintFunctionsConsumer(CompilerInstance &Instance,
                         std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {}

  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    return true;
  }

  void HandleTranslationUnit(ASTContext& context) override {

    if (!Instance.getLangOpts().DelayedTemplateParsing)
      return;
      
    struct Visitor : public RecursiveASTVisitor<Visitor> {
      const std::set<std::string> &ParsedTemplates;
      Visitor(const std::set<std::string> &ParsedTemplates)
          : ParsedTemplates(ParsedTemplates) {}
          
      bool VisitCXXRecordDecl(CXXRecordDecl *CxxRDecl){
        if(CxxRDecl->isClass()||CxxRDecl->isStruct()){
          llvm::errs() << CxxRDecl->getNameAsString()<<"\n";
          for(auto field = CxxRDecl->decls_begin(); field!=CxxRDecl->decls_end();++field){
            if(FieldDecl* declField = dyn_cast<FieldDecl>(*field)){
              llvm::errs() << "|_ " <<declField->getNameAsString()<<"\n";
            }
          }
        }
        return true;
      }
    } v(ParsedTemplates);
    v.TraverseDecl(context.getTranslationUnitDecl());
  }
};

class PrintClassesNamesAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;

protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<PrintFunctionsConsumer>(CI, ParsedTemplates);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

} // namespace

static FrontendPluginRegistry::Add<PrintClassesNamesAction>
    X("print-c", "print classes names");

