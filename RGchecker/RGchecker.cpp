#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Signals.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/raw_ostream.h"
#include "fmt/core.h"
//#include "tis.h"
#include "parser.h"

using namespace std;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

//DeclarationMatcher M = varDecl(decl().bind("name"), hasName("size"));
// StatementMatcher M = binaryOperator(hasRHS(ignoringImpCasts(callExpr(callee(functionDecl(hasName("cgc_getline"))
//   ),hasAnyArgument(ignoringParenImpCasts(declRefExpr(to(varDecl().bind("buffer"))))))))).bind("b");
//StatementMatcher M2 = binaryOperator(hasLHS(expr().bind("name")), hasRHS(callExpr(hasAnyArgument(ignoringParenImpCasts(declRefExpr(to(functionDecl(hasName("cgc_getline")))))))));
//  hasRHS(callExpr(hasAnyArgument(ignoringParenImpCasts(declRefExpr(to(varDecl(hasName("buffer")))))))));
//StatementMatcher M2 = declRefExpr(expr().bind("name"), to(varDecl()));
//callExpr(matchesName("buffer")), isAssignmentOperator(callExpr()));
//allOf(hasInitializer(callExpr()), 
//hasLHS(ignoringParenImpCasts(declRefExpr(to(varDecl(hasType(pointsTo(AnyType)))
// ValueDecl(  (hasInitializer(callExpr())
//     decl().bind(FunctionID),
//     hasAnyParameter(hasType(recordDecl(matchesName("std::vector")))));

string fname = " ";
string dest_path, unvisited_file;
bool collect = true;
map<string, map<int, int>*>* unvisited;


class SwitchHandler: public MatchFinder::MatchCallback {
    private:
        Rewriter &Rewrite;

    public:
        SwitchHandler(Rewriter &Rewrite):Rewrite(Rewrite){

        }

        virtual void run(const MatchFinder::MatchResult &Result) final{
            const auto& SM = *Result.SourceManager;
            string dump;
            if(const SwitchStmt *swt = Result.Nodes.getNodeAs<clang::SwitchStmt>("switchStmt"))
            {
                const auto& switchloc = swt->getBeginLoc();
                int switchline = SM.getSpellingLineNumber(switchloc); 
                int i = 0;
                for (const SwitchCase *c = swt->getSwitchCaseList(); c != NULL; c = c->getNextSwitchCase()) {
                      if (isa<CaseStmt>(c)) {
                          i += 1;
                          const auto& case_loc = c->getColonLoc();
                          const CaseStmt *caseStmt = cast<CaseStmt>(c);
                          // SourceRange range = cas->getSourceRange();
                          // SourceLocation case_end = Lexer::getLocForEndOfToken(case_loc,0, SM,  Result.Context->getLangOpts());
                          // llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(range), SM, Result.Context->getLangOpts());
                          // string stmt_source = ref.str();
                          // llvm::outs() << stmt_source << "\n";
                          dump = fmt::format("\n\ttis_dump_each_file(\"{0}:{1}.case{2}\");\n", fname, switchline, i);
                          Rewrite.InsertTextAfterToken(case_loc, dump);
                      }

                }
                dump = fmt::format("\ntis_dump_each_file(\"{0}:{1}.switch\");\n", fname, switchline);
                Rewrite.InsertText(switchloc, dump);
            }
      }

};


class IfStmtHandler: public MatchFinder::MatchCallback {
	private:
    	Rewriter &Rewrite;

  	public:
    	IfStmtHandler(Rewriter &Rewrite): Rewrite(Rewrite) {
      }



      virtual void run(const MatchFinder::MatchResult &Result) final {
          if (const IfStmt *IfS = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
              const auto& SM = *Result.SourceManager;
              const Stmt *Then = IfS->getThen();
              const Stmt *Else;
              int else_line = 10000000000;
              int then_line;
              bool enter_else = false;
              bool inserted = false;
              bool has_no_else = true;
              SourceLocation else_ending;
              if (Else = IfS->getElse()) {
                  const auto& Loc = Else->getBeginLoc(); 
                  SourceLocation else_b = Lexer::GetBeginningOfToken(Loc, SM,  Result.Context->getLangOpts());
                  else_line = SM.getSpellingLineNumber(else_b); 
                  else_ending = Lexer::getLocForEndOfToken(Else->getEndLoc(), 0, SM,  Result.Context->getLangOpts());
                  has_no_else = false;
              }
              const auto& then_loc = Then->getBeginLoc();
              SourceLocation Beginning = Lexer::GetBeginningOfToken(then_loc, SM,  Result.Context->getLangOpts());
              int then_token_line = SM.getSpellingLineNumber(Beginning);

              const auto& then_end_loc = Then->getEndLoc();
              SourceLocation then_ending = Lexer::getLocForEndOfToken(then_end_loc, 0, SM,  Result.Context->getLangOpts());


              //Rewrite.InsertTextAfterToken(Beginning, "// the then part");
              clang::SourceRange loc = IfS->getSourceRange();
              PresumedLoc locStart = SM.getPresumedLoc(loc.getBegin());
              PresumedLoc locEnd = SM.getPresumedLoc(loc.getEnd());
              unsigned int st = locStart.getLine();
              unsigned int end = locEnd.getLine();
              string dump; 
              llvm::outs() << fname << "\n";
              llvm::outs() << "then: " << then_line << "\n";
              llvm::outs() << "then token " << then_token_line << "\n";
              llvm::outs() << "if: " << st << "\n";
              llvm::outs() << "else: " << else_line << "\n";
              dump = fmt::format("\ntis_dump_each_file(\"{0}:{1}.then\");\n", fname, st);
              if(not has_no_else)
              {
                  dump = fmt::format("\ntis_dump_each_file(\"{0}:{1}.else\");\n", fname, st);
                  SourceRange range = Else->getSourceRange();
                  llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(range), SM, Result.Context->getLangOpts());
                  string stmt_source = ref.str();
                  stmt_source = stmt_source.substr(0,20);
                  llvm::outs() << stmt_source << "\n";
                  // if(stmt_source.rfind("if") == std::string::npos){
                  if(stmt_source.rfind("{") == std::string::npos || stmt_source.rfind("if") != std::string::npos ){
                            Rewrite.InsertText(Else->getBeginLoc(), "{"+dump);
                            //llvm::outs() << r << "return value for else insert\n";
                            Rewrite.InsertTextAfterToken(else_ending, "\n}");
                  }
                  else{
                            Rewrite.InsertTextAfterToken(Else->getBeginLoc(), dump);
                  }
              }
              else{

                    SourceRange range = Then->getSourceRange();
                    llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(range), SM, Result.Context->getLangOpts());
                    string stmt_source = ref.str();
                    //llvm::outs() << stmt_source << "\n";
                    if (stmt_source == "") return;
                    if(stmt_source.rfind("{") == std::string::npos){
                        Rewrite.InsertText(then_loc, "{"+dump);
                        Rewrite.InsertTextAfterToken(then_ending, "\n}");
                    }
                    else{
                        Rewrite.InsertTextAfterToken(then_loc, dump);
                    }                       
              }
              dump = fmt::format("\ntis_dump_each_file(\"{0}:{1}.if\");\n", fname, st); 
              Rewrite.InsertText(IfS->getBeginLoc(), dump, true,true);
              llvm::outs() << st << " is inserted\n"; 
       }
    }
};


class RangeASTConsumer : public ASTConsumer {

    public:
        RangeASTConsumer(Rewriter &R) : ifh(R),sh(R){
        // Add a simple matcher for finding 'if' statements.
        if(collect)
        {

        	Matcher.addMatcher(ifStmt().bind("ifStmt"), &ifh);
          Matcher.addMatcher(switchStmt().bind("switchStmt"), &sh);

        }
        // else{
        // 	Matcher.addMatcher(binaryOperator(hasLHS(declRefExpr(to(varDecl().bind("revar")))), 
        //         hasRHS(ignoringImpCasts(callExpr(callee(functionDecl(hasName("cgc_getline"))), 
        //           hasArgument(0, ignoringParenImpCasts(declRefExpr(to(varDecl().bind("var"))))))))).bind("b"), &RC);        	
        // }


        // Matcher.addMatcher(ifStmt().bind("ifStmt"), &HandlerForIf);

        // // Add a complex matcher for finding 'for' loops with an initializer set
        // // to 0, < comparison in the codition and an increment. For example:
        // //
        // //  for (int i = 0; i < N; ++i)
        // Matcher.addMatcher(
        //     forStmt(hasLoopInit(declStmt(hasSingleDecl(
        //                 varDecl(hasInitializer(integerLiteral(equals(0))))
        //                     .bind("initVarName")))),
        //             hasIncrement(unaryOperator(
        //                 hasOperatorName("++"),
        //                 hasUnaryOperand(declRefExpr(to(
        //                     varDecl(hasType(isInteger())).bind("incVarName")))))),
        //             hasCondition(binaryOperator(
        //                 hasOperatorName("<"),
        //                 hasLHS(ignoringParenImpCasts(declRefExpr(to(
        //                     varDecl(hasType(isInteger())).bind("condVarName"))))),
        //                 hasRHS(expr(hasType(isInteger()))))))
        //         .bind("forLoop"),
        //     &HandlerForFor);
        }

      void HandleTranslationUnit(ASTContext &Context) override {
        // Run the matchers when we have the whole TU parsed.
        //TheRewriter.setSourceMgr(Context.getSourceManager(), Context.getLangOpts());
        Matcher.matchAST(Context);
      }

    private:
      IfStmtHandler ifh;
      SwitchHandler sh;
      MatchFinder Matcher;
};

// For each source file provided to the tool, a new FrontendAction is created.
class RangeFrontendAction : public ASTFrontendAction {
    public:
        RangeFrontendAction() {}

       //  string getVeriFile(SourceManager &SM){
       //  	string path = SM.getFileEntryForID(SM.getMainFileID())->getName().str();
       //  	std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
    			// // std::string::size_type const p(path.find_last_of('.'));
    			// // std::string file_without_extension = path.substr(0, p);
    			// // string ex = path.substr(p);
    			// string v_f = fmt::format("{0}_v.{1}", base_filename, ".c");
    			// llvm::outs() << v_f << "\n";
       //  }

       void EndSourceFileAction() override {
              std::error_code error_code;
              SourceManager &SM = TheRewriter.getSourceMgr();
              //TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
              string path = SM.getFileEntryForID(SM.getMainFileID())->getName().str();
              // std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
              // std::string::size_type const p(path.find_last_of('.'));
              // std::string file_without_extension = path.substr(0, p);
              // string ex = path.substr(p+1);
              //string v_f = fmt::format("{0}_tis.{1}", file_without_extension, ex);
              //llvm::outs() << v_f << "\n";
              //string v_f = fmt::format("{0}/{1}", dest_path, fname);
              llvm::raw_fd_ostream outFile(path, error_code, llvm::sys::fs::F_None);
              TheRewriter.getEditBuffer(SM.getMainFileID()).write(outFile);
              outFile.close();
        //     SourceManager &SM = TheRewriter.getSourceMgr();
        //     string path = SM.getFileEntryForID(SM.getMainFileID())->getName().str();
        //     std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
        //     std::string::size_type const p(path.find_last_of('.'));
        //     std::string file_without_extension = path.substr(0, p);
        //     string ex = path.substr(p+1);
        //     string v_f = fmt::format("{0}_v.{1}", file_without_extension, ex);
        //     llvm::outs() << v_f << "\n";
        // 	  if(!collect)
        //     	{
        //     		std::error_code error_code;
	       //          llvm::raw_fd_ostream outFile(v_f, error_code, llvm::sys::fs::F_None);
	       //          TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
	       //          TheRewriter.getEditBuffer(SM.getMainFileID()).write(outFile);
	       //          outFile.close();
        //     	}
            
        }

        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                       StringRef file) override {
            //ASTContext *astContext = &(CI.getASTContext()); 
            const auto& SM = CI.getSourceManager();
            string path = SM.getFileEntryForID(SM.getMainFileID())->getName().str();
            fname = path.substr(path.find_last_of("/\\") + 1);
            //llvm::outs() << "Current file is " << fname << "\n";
            TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
            return std::unique_ptr<RangeASTConsumer>(new RangeASTConsumer(TheRewriter));
        }
    private:
        Rewriter TheRewriter;
};

  static llvm::cl::OptionCategory FindVecCategory("Range options");

  static llvm::cl::extrahelp
      CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

  static llvm::cl::extrahelp MoreHelp(
      "\nclang-vec will list all functions that accept std::vector as a"
      "\npass-by-value argument.");

  static llvm::cl::opt<string> dest_opt("dest", llvm::cl::cat(FindVecCategory));
  static llvm::cl::opt<string> unvisited_opt("unvisited", llvm::cl::cat(FindVecCategory));

int main(int argc, const char **argv) {
  //llvm::sys::PrintStackTraceOnErrorSignal();
  // if(argc>2){
  //     dest_path = string(argv[4]);
  //     unvisited_file = string(argv[5]);
  //     llvm::outs() << "dest_path: " << dest_path << "\n";
  //     llvm::outs() << "unvisited_file" << unvisited_file << "\n";
  // }
  // else{
  //     return 0;
  // }
  clang::tooling::CommonOptionsParser Options(argc, argv, FindVecCategory);
  clang::tooling::ClangTool Tool(Options.getCompilations(),
                                 Options.getSourcePathList());
  llvm::outs() << dest_opt.getValue() << '\n';
  // unvisited_file = unvisited_opt.getValue();
  //dest_path = dest_opt.getValue();
  // unvisited = parse(unvisited_file);
  // llvm::outs() << unvisited->size() << "\n";
  Tool.run(clang::tooling::newFrontendActionFactory<RangeFrontendAction>().get());

  //Tool.run(clang::tooling::newFrontendActionFactory<RangeFrontendAction>().get());

}


