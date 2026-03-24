#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include "sqlite3.h"

using namespace std;

/* ================= DATA ================= */

unordered_map<string, unordered_map<int,int>> invertedIndex;
unordered_map<int,string> docTitles;
unordered_map<int,string> docCategory;
unordered_map<int,string> docURL;
unordered_map<int,int> docLength;

int totalDocs = 0;
int maxDocID = 0;

/* ================= STOPWORDS ================= */

unordered_set<string> stopwords = {
 "the","is","and","or","to","of","in","on","for","with","a","an","by",
 "this","that","it","as","are","was","were","be","has","have","had","at"
};

/* ================= TRUSTED DOMAINS ================= */

unordered_set<string> trustedDomains = {
 "techradar.com","anandtech.com","xda-developers.com","androidauthority.com"
};

/* ================= UTIL ================= */

bool fileExists(const string& f){
    ifstream in(f);
    return in.good() && in.peek()!=ifstream::traits_type::eof();
}

/* ================= TOKENIZER ================= */

vector<string> tokenize(string text){
    for(char &c:text) c = isalnum(c)?tolower(c):' ';

    stringstream ss(text);
    vector<string> words;
    string w;

    while(ss>>w){
        if(w.size()<2) continue;
        if(stopwords.count(w)) continue;
        words.push_back(w);
    }
    return words;
}

/* ================= INDEX ================= */

void indexDocument(int id,const string& content){
    for(auto &w:tokenize(content))
        invertedIndex[w][id]++;
}

/* ================= SAVE INDEX ================= */

void saveIndex(){
    ofstream out("index.dat");

    out<<"TOTAL "<<totalDocs<<"\n";
    out<<"MAX "<<maxDocID<<"\n";

    for(auto &e:invertedIndex){
        out<<e.first;
        for(auto &p:e.second)
            out<<" "<<p.first<<":"<<p.second;
        out<<"\n";
    }
}

/* ================= LOAD INDEX ================= */

bool loadIndex(){

    ifstream in("index.dat");
    if(!in) return false;

    string line;

    if(!getline(in,line) || line.rfind("TOTAL ",0)!=0) return false;
    totalDocs = stoi(line.substr(6));

    if(!getline(in,line) || line.rfind("MAX ",0)!=0) return false;
    maxDocID = stoi(line.substr(4));

    invertedIndex.clear();

    while(getline(in,line)){
        if(line.empty()) continue;

        stringstream ss(line);
        string word; ss>>word;

        string pair;
        while(ss>>pair){
            int p=pair.find(":");
            invertedIndex[word][stoi(pair.substr(0,p))]=stoi(pair.substr(p+1));
        }
    }
    return true;
}

/* ================= LOAD METADATA ================= */

void loadMetadata(){

    sqlite3 *db; sqlite3_stmt *stmt;
    sqlite3_open("crawler_data.db",&db);

    const char* sql="SELECT id,url,title,category,length FROM pages;";
    sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);

    auto txt=[&](int c){
        auto t=sqlite3_column_text(stmt,c);
        return t?string((char*)t):"";
    };

    while(sqlite3_step(stmt)==SQLITE_ROW){
        int id=sqlite3_column_int(stmt,0);
        docURL[id]=txt(1);
        docTitles[id]=txt(2);
        docCategory[id]=txt(3);
        docLength[id]=sqlite3_column_int(stmt,4);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

/* ================= FULL BUILD ================= */

void buildFullIndex(){

    sqlite3 *db; sqlite3_stmt *stmt;
    sqlite3_open("crawler_data.db",&db);

    const char* sql="SELECT id,content FROM pages;";
    sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr);

    auto txt=[&](int c){
        auto t=sqlite3_column_text(stmt,c);
        return t?string((char*)t):"";
    };

    while(sqlite3_step(stmt)==SQLITE_ROW){

        int id=sqlite3_column_int(stmt,0);
        indexDocument(id,txt(1));

        maxDocID=max(maxDocID,id);
        totalDocs++;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

/* ================= SEARCH ================= */

void searchJSON(const string& query,const string& filter){

    auto qwords=tokenize(query);
    unordered_map<int,double> scores;

    for(auto &w:qwords){

        if(!invertedIndex.count(w)) continue;

        double idf=log((double)totalDocs/invertedIndex[w].size());

        for(auto &p:invertedIndex[w]){

            int id=p.first;
            int freq=p.second;

            if(!filter.empty() && docCategory[id]!=filter) continue;

            double score=(freq*idf)/log(docLength[id]+1);

            if(docTitles[id].find(w)!=string::npos) score+=5;

            for(auto &d:trustedDomains)
                if(docURL[id].find(d)!=string::npos){score+=2;break;}

            if(docLength[id]>6000) score*=0.7;

            scores[id]+=score;
        }
    }

    vector<pair<int,double>> ranked(scores.begin(),scores.end());
    sort(ranked.begin(),ranked.end(),
        [](auto&a,auto&b){return a.second>b.second;});

    cout<<"[";

    bool first=true;
    for(int i=0;i<ranked.size() && i<10;i++){

        if(!first) cout<<",";
        first=false;

        int id=ranked[i].first;

        cout<<"{\"score\":"<<ranked[i].second
            <<",\"title\":\""<<docTitles[id]
            <<"\",\"url\":\""<<docURL[id]<<"\"}";
    }
    cout<<"]";
}

/* ================= MAIN ================= */

int main(int argc,char* argv[]){

    if(!fileExists("index.dat")){
        cout<<"Building index...\n";
        buildFullIndex();
        saveIndex();
    }
    else{
        if(!loadIndex()){
            cout<<"Index corrupted — rebuilding\n";
            buildFullIndex();
            saveIndex();
        }
    }

    loadMetadata();

    if(argc<2){
        cout<<"[]";
        return 0;
    }

    string query=argv[1];
    string category=(argc>=3)?argv[2]:"";

    searchJSON(query,category);
    return 0;
}