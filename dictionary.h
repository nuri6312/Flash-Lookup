#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <unordered_map>

class Dictionary {
private:
    std::unordered_map<std::string, std::vector<std::string>> words;
    std::vector<std::string> wordList;
    size_t totalEntries;
    
    std::string makeLower(const std::string& text);
    std::vector<std::string> parseCSV(const std::string& line);
    std::string trimWhitespace(const std::string& text);
    
public:
    struct Result {
        bool found;
        std::string definition;
        std::vector<std::string> definitions; // For accessing individual definitions if needed
    };
    
    void addWord(const std::string& word, const std::string& definition = "");
    Result findWord(const std::string& word);
    std::vector<std::string> getSuggestions(const std::string& prefix, int limit = 10);
    int calculateDistance(const std::string& word1, const std::string& word2);
    std::vector<std::string> getCorrections(const std::string& word, int maxDist = 2, int limit = 5);
    bool loadFromFile(const std::string& filename);
    size_t size() const;
    size_t totalSize() const;
};

#endif // DICTIONARY_H