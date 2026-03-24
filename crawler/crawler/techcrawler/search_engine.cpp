#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include "sqlite3.h"

using namespace std;

// ---------------- DATA STRUCTURES ----------------

unordered_map<string, unordered_map<int,int>> invertedIndex;
unordered_map<int,string> docTitles;
unordered_map<int,string> docCategory;
unordered_map<int,string> docURL;

int totalDocs = 0;

// ---------------- TOKENIZER ----------------

vector<string> tokenize(string text) {
    stringstream ss(text);
    string word;
    vector<string> words;

    while (ss >> word) {
        for (char &c : word)
            c = tolower(c);

        word.erase(remove_if(word.begin(), word.end(),
                [](char c){ return !isalnum(c); }), word.end());

        if (!word.empty())
            words.push_back(word);
    }
    return words;
}

// ---------------- INDEX BUILDER ----------------

void indexDocument(int id, string content) {
    auto words = tokenize(content);

    for (auto &w : words) {
        invertedIndex[w][id]++;
    }
}

// ---------------- SQLITE LOADER ----------------

void loadFromSQLite() {
    sqlite3* db;
    sqlite3_stmt* stmt;

    if (sqlite3_open("crawler_data.db", &db)) {
        cout << "Failed to open DB\n";
        return;
    }

    const char* sql = "SELECT id, url, title, content, category FROM pages;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    auto start = chrono::high_resolution_clock::now();

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        int id = sqlite3_column_int(stmt, 0);
        // string url = (const char*)sqlite3_column_text(stmt, 1);
        // string title = (const char*)sqlite3_column_text(stmt, 2);
        // string content = (const char*)sqlite3_column_text(stmt, 3);
        // string category = (const char*)sqlite3_column_text(stmt, 4);
        auto safeText = [&](int col) {
           const unsigned char* txt = sqlite3_column_text(stmt, col);
           return txt ? string(reinterpret_cast<const char*>(txt)) : "";
};

string url = safeText(1);
string title = safeText(2);
string content = safeText(3);
string category = safeText(4);


        docTitles[id] = title;
        docCategory[id] = category;
        docURL[id] = url;

        indexDocument(id, content);
        totalDocs++;
    }

    auto end = chrono::high_resolution_clock::now();

    cout << "Indexed " << totalDocs << " documents in "
         << chrono::duration<double>(end - start).count()
         << " seconds\n";

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

// ---------------- SEARCH ENGINE ----------------

void search(string query, string categoryFilter="") {

    auto qwords = tokenize(query);
    unordered_map<int,double> scores;

    auto start = chrono::high_resolution_clock::now();

    for (auto &word : qwords) {

        if (!invertedIndex.count(word)) continue;

        int df = invertedIndex[word].size();
        double idf = log((double)totalDocs / df);

        for (auto &p : invertedIndex[word]) {

            int docID = p.first;
            int freq = p.second;

            if (!categoryFilter.empty() &&
                docCategory[docID] != categoryFilter)
                continue;

            double tf = freq;
            double score = tf * idf;

            if (docTitles[docID].find(word) != string::npos)
                score += 5;

            scores[docID] += score;
        }
    }

    vector<pair<int,double>> ranked(scores.begin(), scores.end());

    sort(ranked.begin(), ranked.end(),
         [](auto &a, auto &b){ return a.second > b.second; });

    auto end = chrono::high_resolution_clock::now();

    cout << "\nSearch took "
         << chrono::duration<double>(end - start).count()
         << " seconds\n\n";

    int shown = 0;
    for (auto &p : ranked) {

        int docID = p.first;
        double score = p.second;

        cout << "Score: " << score << "\n";
        cout << docTitles[docID] << "\n";
        cout << docURL[docID] << "\n\n";

        if (++shown == 5) break;
    }

    if (shown == 0)
        cout << "No results found.\n";
}



// ---------------- MAIN ----------------

int main() {

    cout << "Loading database & building index...\n";
    loadFromSQLite();

    cout << "\nSearch engine ready!\n";

    while (true) {
        string query, category;

        cout << "\nEnter search query (or exit): ";
        getline(cin, query);

        if (query == "exit") break;

        cout << "Filter category (phone/laptop/audio/general or blank): ";
        getline(cin, category);

        search(query, category);
    }

    return 0;
}
