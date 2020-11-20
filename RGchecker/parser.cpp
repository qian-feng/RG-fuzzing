#include "parser.h"                                                                                                                                                        

map<string, map<int, int> *>* parse(string filename){
    map<string, map<int, int>*> *unvisited;
    std::ifstream readFile(filename);
    std::string line;
    unvisited = new map<string, map<int, int>*>();
    cout << "entering" << endl;
    while(getline(readFile,line)){
            std::stringstream ss(line);                                                                                                                                                
            std::string addr;                                                                                                                                                     
            getline(ss, addr, ',');                                                                                                                                               
            std::string fname;                                                                                                                                                        
            getline(ss, fname, ':');     
            std::string base_filename = fname.substr(fname.find_last_of("/\\") + 1);                                                                                                                                             
            int num;                                                                                                                                                                   
            ss >> num;
            // cout << addr << endl;
            // cout << base_filename << endl;
            // cout << num << endl;
            if(unvisited->find(base_filename) == unvisited->end()){
                map<int, int> *imap = new map<int, int>();
                (*imap)[num] = 1;
                (*unvisited)[base_filename] = imap;
            }
            else{
                (*(*unvisited)[base_filename])[num] = 1;
            }
    }
    return unvisited;
}

// int main(){
//     map<string, map<int, int>*> *unvisited;
//     string filename = "/data/clang-llvm/llvm-project/clang-tools-extra/RGchecker/mytest";
//     cout << "test" << endl;
//     unvisited = update(filename);
//     cout << unvisited->size() << endl;


// }
