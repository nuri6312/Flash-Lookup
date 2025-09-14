#include "dictionary.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_set>

using namespace std;

string Dictionary::makeLower(const string& text) {
    string result = text;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void Dictionary::addWord(const string& word, const string& definition) {
    string lower = makeLower(word);
    
    if (words.find(lower) == words.end()) {
        wordList.push_back(lower);
        words[lower] = vector<string>();
    }
    
    words[lower].push_back(definition);
}

Dictionary::Result Dictionary::findWord(const string& word) {
    string lower = makeLower(word);
    auto it = words.find(lower);
    
    if (it != words.end()) {
        vector<string> validDefs;
        for (const string& def : it->second) {
            string trimmed = trimWhitespace(def);
            if (!trimmed.empty()) {
                validDefs.push_back(trimmed);
            }
        }
        
        if (!validDefs.empty()) {
            int count = min(4, (int)validDefs.size());
            vector<string> selectedDefs(validDefs.begin(), validDefs.begin() + count);
            
            string combined = "";
            for (int i = 0; i < count; i++) {
                if (i > 0) combined += "\n\n";
                combined += to_string(i + 1) + ". " + selectedDefs[i];
            }
            
            return {true, combined, selectedDefs};
        }
    }
    
    return {false, "", {}};
}

vector<string> Dictionary::getSuggestions(const string& prefix, int limit) {
    string lower = makeLower(prefix);
    vector<string> matches;
    
    for (const string& word : wordList) {
        if (word.length() >= lower.length() && 
            word.substr(0, lower.length()) == lower) {
            matches.push_back(word);
            if (matches.size() >= limit) break;
        }
    }
    
    return matches;
}

int Dictionary::calculateDistance(const string& word1, const string& word2) {
    int len1 = word1.length(), len2 = word2.length();
    vector<vector<int>> table(len1 + 1, vector<int>(len2 + 1));
    
    for (int i = 0; i <= len1; i++) table[i][0] = i;
    for (int j = 0; j <= len2; j++) table[0][j] = j;
    
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (word1[i-1] == word2[j-1]) {
                table[i][j] = table[i-1][j-1];
            } else {
                table[i][j] = 1 + min({table[i-1][j], table[i][j-1], table[i-1][j-1]});
            }
        }
    }
    
    return table[len1][len2];
}

vector<string> Dictionary::getCorrections(const string& word, int maxDist, int limit) {
    string lower = makeLower(word);
    vector<pair<string, int>> candidates;
    
    for (const string& dictWord : wordList) {
        int dist = calculateDistance(lower, dictWord);
        if (dist > 0 && dist <= maxDist) {
            candidates.push_back({dictWord, dist});
        }
    }
    
    sort(candidates.begin(), candidates.end(), 
         [](const auto& a, const auto& b) {
             if (a.second != b.second) return a.second < b.second;
             return a.first < b.first;
         });
    
    vector<string> corrections;
    unordered_set<string> used;
    
    for (const auto& candidate : candidates) {
        if (used.find(candidate.first) == used.end()) {
            used.insert(candidate.first);
            corrections.push_back(candidate.first);
            if (corrections.size() >= limit) break;
        }
    }
    
    return corrections;
}

vector<string> Dictionary::parseCSV(const string& line) {
    vector<string> fields;
    string current;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        
        if (c == '"') {
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                current += '"';
                i++;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}

string Dictionary::trimWhitespace(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

bool Dictionary::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }
    
    totalEntries = 0;
    string line;
    int wordCount = 0;
    int lineCount = 0;
    
    while (getline(file, line)) {
        lineCount++;
        
        if (trimWhitespace(line).empty()) continue;
        
        auto fields = parseCSV(line);
        
        if (fields.size() >= 1) {
            string word = trimWhitespace(fields[0]);
            string definition = fields.size() > 1 ? trimWhitespace(fields[1]) : "";
            
            if (definition.length() >= 2 && definition[0] == '"' && definition.back() == '"') {
                definition = definition.substr(1, definition.length() - 2);
            }
            
            if (!word.empty()) {
                addWord(word, definition);
                wordCount++;
                totalEntries++;
                
                if (wordCount % 50000 == 0) {
                    cout << "Loaded " << wordCount << " entries..." << endl;
                }
            }
        }
    }
    
    cout << "Dictionary loaded! " << wordCount << " words from " << lineCount << " lines." << endl;
    return true;
}

size_t Dictionary::size() const {
    return words.size();
}

size_t Dictionary::totalSize() const {
    return totalEntries;
}