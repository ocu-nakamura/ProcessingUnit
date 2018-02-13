#pragma once
#include <stdio.h>
#include <iostream>
#include <regex>
#include <queue>
#include <sstream>
#include <sstream>
#include <iomanip>
#include <tools.hpp>

class PrefixParser {
private:
std::vector<std::string> prefixArray;
public:
//parent,childPrefix は参照渡し
bool startParse(const std::string inputPrefix,std::string &parentPrefix,std::vector<std::string> &childPrefix);
void showPrefixArray();
std::string urlEncode(std::string input);
void removePrefixArrayEndSlash();
};

bool PrefixParser::startParse(std::string inputPrefix,std::string &parentPrefix,std::vector<std::string> &childPrefix){
        std::string bufPrefix = urlEncode(inputPrefix);

        std::regex rePrefixComfirmation("(\\/([a-z]|[A-Z]|[0-9])+)+\\/\\(((\\/([a-z]|[A-Z]|[0-9])+),)*(\\/([a-z]|[A-Z]|[0-9])+)\\)");
        if(!std::regex_match(bufPrefix.c_str(),rePrefixComfirmation)) {
                INFO lout("Syntax Error",bufPrefix);
                return false;
        }
        std::string bufStr;
        int beginArg = 0;
        int endArg = 0;
        int countRBracket = 0;
        for(int i = 0; i< (int)bufPrefix.size(); i++) {
                if(bufPrefix[i] == '(') {
                        //まだ開始括弧が見つかっていないなら
                        if(beginArg ==0) {
                                beginArg = i;
                        }
                }
                else if((int)bufPrefix[i]==')') {
                        endArg =i;
                }
        }
        //処理名を抽出
        for(int i = 0; i< (int)bufPrefix.size(); i++) {
                if(i == beginArg ||i==endArg) {
                        prefixArray.push_back(bufStr);
                        bufStr.clear();
                }
                else{
                        bufStr += bufPrefix[i];
                }
        }
        //引数を抽出
        //引数部分をコピー
        bufPrefix.clear();
        bufPrefix = prefixArray[1];
        //引数部分を削除
        prefixArray.erase(prefixArray.begin()+1);
        bufStr.clear();
        for(int i =0; i<(int)bufPrefix.size(); i++) {
                if(bufPrefix[i] =='(') {
                        countRBracket++;
                }
                else if(bufPrefix[i]==')') {
                        countRBracket--;
                }

                if(bufPrefix[i]==','&&countRBracket ==0) {
                        prefixArray.push_back(bufStr);
                        bufStr.clear();
                }
                else{
                        bufStr+=bufPrefix[i];
                }
        }
        prefixArray.push_back(bufStr);
        removePrefixArrayEndSlash();

        //parent,childPrefix へ代入
        for (int i = 0; i < (int)prefixArray.size(); i++) {
                if(i ==0) {
                        parentPrefix = prefixArray[i];
                }
                else{
                        childPrefix.push_back(prefixArray[i]);
                }
        }
        return true;
}
void PrefixParser::showPrefixArray(){
        std::cout << "LOG:prefixArray" << '\n';
        for (auto x:prefixArray) {
                std::cout << x << '\n';
        }
}
std::string PrefixParser::urlEncode(std::string input){
        std::string buf;
        buf = input;
        std::regex reComma("%2C");
        buf = regex_replace(buf,reComma,",");
        std::regex reEqual("%3D");
        buf = regex_replace(buf,reEqual,"=");
        std::regex reLeftCurb("%28");
        buf = regex_replace(buf,reLeftCurb,"(");
        std::regex reRightCurb("%29");
        buf = regex_replace(buf,reRightCurb,")");
        return buf;
}
void PrefixParser::removePrefixArrayEndSlash(){
        //--------------------------//
        //デフォルトで後ろのプレフィクスは
        //無視されてるみたいなのでそのうち
        //実装
        //--------------------------//
}

//  /test/process1(/test/data1/,/test/process2/test/data3)
