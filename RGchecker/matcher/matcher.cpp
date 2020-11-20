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
#include "tis.h"

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

string v_filename = " ";
class Var {
    public:
        list<tuple<int,int>>* ran;
        string filename;
        string varname;
        string type;
        int size;
        int line;

    public:

        Var(string f, string var, string t, int l, uint64_t s): filename(f), varname(var), type(t), line(l), size(s){
          ran = new list<tuple<int,int>>();
        }
        void setfilename(string f){
          filename = f;
        }

        void setvarname(string var){
          varname = var;
        }

        void setType(string t){
          type = t;
        }

        void setSize(uint64_t s){
          size = (int)s;
        }
        void setLine(int l){
          line = l;
        }

        void setRange(std::tuple<int,int>  r){
          ran->push_back(r);
        }


};

std::map<std::string, std::map<std::string, Var*>*> *rans;
bool collect = true;


class VarsCallback: public MatchFinder::MatchCallback {
	private:
    	Rewriter &Rewrite;

  	public:
    	VarsCallback(Rewriter &Rewrite): Rewrite(Rewrite) {}



      std::tuple<std::string,uint64_t> getVarType(const VarDecl *var){
          std::string tt;
          const Type *type = var->getType().getTypePtr();
          if(type->isConstantArrayType())
          {
                  const ArrayType *Array = type->castAsArrayTypeUnsafe();
                  uint64_t elecnt = dyn_cast_or_null<ConstantArrayType>(Array)->getSize().getZExtValue();
                  tt = Array->getElementType().getAsString();
                  return std::make_tuple(tt, elecnt);
          }
          tt = var->getType().getAsString();
          return std::make_tuple(tt, 1);
      }
      /*
      void add_items(std::list<std::tuple<int, int>> *ls){
          if(ty == "char"){
                  if(size > 255){
                      ls.push_back(std::make_tuple<0,255>);
                  }
                  for(int i = 0; i< size; i++){
                      ls.push_back(std::make_tuple<0,255>);
                  }
          }  
          if(ty == "uint64_t" || ty == "int"){
              for(int i = 0; i< size; i++){
                      ls.push_back(std::make_tuple<0,100>);
              }
              item2[filename] = ls;
              rans[varname] = item2;
          }
      }
*/

      virtual void run(const MatchFinder::MatchResult &Result) final {
  		    if(const auto *var = Result.Nodes.getNodeAs<clang::VarDecl>("var")){
              std::string varname = var->getNameAsString();
      				llvm::outs() << "Var name: " << varname << "\n";
              std::tuple<std::string, uint64_t> x = getVarType(var);
              std::string ty = std::get<0>(x);
              uint64_t size = std::get<1>(x);
              //llvm::outs() << "Type Name: " << ty << "\n";
              //llvm::outs() << "Type Size: " << size << "\n";
              const auto& SM = *Result.SourceManager;
      				const auto& Loc = var->getLocation();
              string filename = SM.getFilename(Loc).str();
              int line = SM.getSpellingLineNumber(Loc);
      	      // llvm::outs() << filename << ":"
      	      //              << SM.getSpellingLineNumber(Loc) << ":"
      	      //              << SM.getSpellingColumnNumber(Loc) << "\n";
              Var* v = new Var(filename, varname, ty, line, size);
              if(rans->find(varname) != rans->end()){
                    std::map<std::string, Var*> *item = (*rans)[varname];
                    if(item->find(filename) != item->end()){
                      if((*(*rans)[varname])[filename]->ran->size() != 1)
                      {
                        delete (*(*rans)[varname])[filename]->ran;
                      }
                    }
                    list<tuple<int,int>> * ls2 = new list<tuple<int,int>>();
                    ls2->push_back(make_tuple(0,255));
                    v->ran = ls2;
                    (*item)[filename] = v;
              }
              else{
                  std::map<std::string, Var*> *item2 = new std::map<std::string, Var*> ();
                  if(size > 255){
                      (*v).ran->push_back(std::make_tuple(0,255));
                  }
                  else{
                      for( int i = 0; i< (int)size; i++){
                        (*v).ran->push_back(std::make_tuple(0,255));
                      }
                  }
                  
                  (*item2)[filename] = v;
                  (*rans)[varname] = item2;
              }
              
  		  }
    }
};

class RangeCallback : public MatchFinder::MatchCallback {
  private:
    Rewriter &Rewrite;

  public:
    RangeCallback(Rewriter &Rewrite): Rewrite(Rewrite) {}

    virtual void run(const MatchFinder::MatchResult &Result) final {
          string rs;
		      if (const auto *F =Result.Nodes.getNodeAs<clang::BinaryOperator>("b")) 
		      {
		        if(const auto *var = Result.Nodes.getNodeAs<clang::VarDecl>("var"))
		        {

              if(const auto *defvar = Result.Nodes.getNodeAs<clang::VarDecl>("revar"))
              { 
                  string tt = var->getNameAsString();
                  std::string varname = defvar->getNameAsString();
                  rs = fmt::format("{0}= tis_int_interval({1},{2})", varname, 0, 5);
                  Rewrite.ReplaceText(F->getSourceRange (), rs);
                  const auto& SM = *Result.SourceManager;
                  const auto& Loc = var->getLocation();
                  string filename = SM.getFilename(Loc).str();
                  if(rans->find(tt)!= rans->end()){
                      Var * v = (*(*rans)[tt])[filename];
                      if(v->size > 1){
                          if(v->ran->size() == 1){
                              rs = fmt::format("tis_make_unknown( {0}, sizeof({1})", tt, tt);
                              Rewrite.ReplaceText(F->getSourceRange(), rs);
                          }
                          else{
                              list<tuple<int,int>>::iterator it;
                              int i=0;
                              for (it = v->ran->begin(); it != v->ran->end(); it++){
                                  if(v->type == "char")
                                      rs = fmt::format("\n{0}[{1}] = tis_char_interval({2},{3});\n", tt, i, get<0>(*it), get<1>(*it));
                                  else{
                                      rs = fmt::format("\n{0}[{1}] = tis_int_interval({2},{3});\n", tt, i, get<0>(*it), get<1>(*it));
                                  } 
                                  Rewrite.InsertText(F->getBeginLoc(), rs, true, true);     
                                  i++;      
                              }
                          }

                      }
                      // if(v->ran->size() == 1 && v->size > 1){
                          
                      // }
                      // else{
                      //     if(v->size == 1) 

                      // }
                  }
		              // if(var->getNameAsString() == "buffer"){
		              //     const auto& SM = *Result.SourceManager;
		              //     Rewrite.ReplaceText(F->getSourceRange (), "buffer[0] = tis_char_interval(0,127);\n");
		              //     //Rewrite.InsertText(F->getBeginLoc(),"//");
		              //     //Rewrite.InsertTextAfter(F->getBeginLoc(), "buffer[0] = tis_char_interval(0,127);\n");
		              //     const auto& Loc = F->getExprLoc();
		              //     llvm::outs() << SM.getFilename(Loc) << ":"
		              //              << SM.getSpellingLineNumber(Loc) << ":"
		              //              << SM.getSpellingColumnNumber(Loc) << "\n";
		              // }
              }
		        }
		      }
    	}
};

class RangeASTConsumer : public ASTConsumer {

    public:
        RangeASTConsumer(Rewriter &R) : RC(R), vars(R) {
        // Add a simple matcher for finding 'if' statements.
        if(collect)
        {

        	Matcher.addMatcher(callExpr(callee(functionDecl(hasName("cgc_getline"))),
                                            hasArgument(0, ignoringParenImpCasts(declRefExpr(to(varDecl().bind("var")))))), &vars);
        }
        else{
        	Matcher.addMatcher(binaryOperator(hasLHS(declRefExpr(to(varDecl().bind("revar")))), 
                hasRHS(ignoringImpCasts(callExpr(callee(functionDecl(hasName("cgc_getline"))), 
                  hasArgument(0, ignoringParenImpCasts(declRefExpr(to(varDecl().bind("var"))))))))).bind("b"), &RC);        	
        }


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
      RangeCallback RC;
      VarsCallback vars;
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
            SourceManager &SM = TheRewriter.getSourceMgr();
            string path = SM.getFileEntryForID(SM.getMainFileID())->getName().str();
            std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
            std::string::size_type const p(path.find_last_of('.'));
            std::string file_without_extension = path.substr(0, p);
            string ex = path.substr(p+1);
            string v_f = fmt::format("{0}_v.{1}", file_without_extension, ex);
            llvm::outs() << v_f << "\n";
        	  if(!collect)
            	{
            		std::error_code error_code;
	                llvm::raw_fd_ostream outFile(v_f, error_code, llvm::sys::fs::F_None);
	                TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
	                TheRewriter.getEditBuffer(SM.getMainFileID()).write(outFile);
	                outFile.close();
            	}
            
        }

        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                       StringRef file) override {
            //ASTContext *astContext = &(CI.getASTContext()); 
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

  void printALL(){
      list<tuple<int,int>>::iterator it;
      for(auto & imap : (*rans))
      {
        for(auto & iimap: (*imap.second)){
            llvm::outs() << iimap.first << "\n";
            llvm::outs() << " filename: " << iimap.second->filename << "\n";
            llvm::outs() << " line: " << iimap.second->line << "\n";
            llvm::outs() << " varname: " << iimap.second->varname << "\n";
            llvm::outs() << " type: " << iimap.second->type << "\n";
            llvm::outs() << " size: " << iimap.second->size << "\n";
            llvm::outs() << " rans: " << "  " << "\n";
            for (it = iimap.second->ran->begin(); it != iimap.second->ran->end(); it++){
                llvm::outs() << "   : " << get<0>(*it) << ",";
                llvm::outs() << "   : " << get<1>(*it) << "\n";                  }
        }
      }

  }

 // int searchL(list<tuple<int,int>>::iterator it){
 //    int start = get<0>(*it);
 //    int end = get<1>(*it);
 //    if(start == end)
 //      return 2;
 //    int mid = (start + end)/2;
 //    end = mid;
 //    get<0>(*it) = start;
 //    get<1>(*it) = end;    
 //    if(start == end)
 //      return 0;
 //    return 1;
 // }

 // int searchR(list<tuple<int,int>>::iterator it){
 //    int start = get<0>(*it);
 //    int mid = (start + end)/2;
 //    start = mid + 1;
 //    get<0>(*it) = start;
 //    get<1>(*it) = end;
 //    if(start > end)
 //      return -1;
 //    return 1;
 // }

 int searching(list<tuple<int,int>>::iterator it, clang::tooling::ClangTool * tool){
    int start = get<0>(*it);
    int end = get<1>(*it);
    int re = 0;
    tool->run(clang::tooling::newFrontendActionFactory<RangeFrontendAction>().get());
    re = tisRun(v_filename);
    if(!re) return 0; 
    if(start >= end){
        if(start == end){
          return 1;
        }
    }
    else{
        int mid = (start + end)/2;
        get<0>(*it) = mid+1;
        get<1>(*it) = end;
        re = searching(it, tool);
        if (re) return 1;
        get<0>(*it) = start;
        get<1>(*it) = end;
        re = searching(it, tool);  
        if(re) return 1;
        return 1;      
    }
 }

 void search(clang::tooling::ClangTool *tool){
      int re=0;
      list<tuple<int,int>>::iterator it;
      for(auto & imap : (*rans))
      {
        for(auto & iimap: (*imap.second)){
            llvm::outs() << iimap.first << "\n";
            llvm::outs() << " filename: " << iimap.second->filename << "\n";
            llvm::outs() << " line: " << iimap.second->line << "\n";
            llvm::outs() << " varname: " << iimap.second->varname << "\n";
            llvm::outs() << " type: " << iimap.second->type << "\n";
            llvm::outs() << " size: " << iimap.second->size << "\n";
            llvm::outs() << " rans: " << "  " << "\n";
            for (it = iimap.second->ran->begin(); it != iimap.second->ran->end(); it++){
                llvm::outs() << "   : " << get<0>(*it) << ",";
                llvm::outs() << "   : " << get<1>(*it) << "\n";     
                re = searching(it, tool); 
                if(!re) llvm::outs()  << "****There is no bug for the current type****" << "\n";      
            }
        }
      }


 }

int main(int argc, const char **argv) {
  //llvm::sys::PrintStackTraceOnErrorSignal();
  clang::tooling::CommonOptionsParser Options(argc, argv, FindVecCategory);
  clang::tooling::ClangTool Tool(Options.getCompilations(),
                                 Options.getSourcePathList());

  // VecCallback VC;
  // MatchFinder Finder;
  // Finder.addMatcher(M2, &VC);
  rans = new std::map<std::string, std::map<std::string, Var*>*>();
  llvm::outs() << " map size: " << rans->size() << "\n";
  Tool.run(clang::tooling::newFrontendActionFactory<RangeFrontendAction>().get());
  llvm::outs() << " map size: " << rans->size() << "\n";
  printALL();
  collect = false;
  search(&Tool);
  //Tool.run(clang::tooling::newFrontendActionFactory<RangeFrontendAction>().get());

}


