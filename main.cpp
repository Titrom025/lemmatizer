#include <iostream>
#include <utility>
#include <vector>
#include "unordered_map"
#include "filesystem"
#include <codecvt>
#include <string>
#include <set>

#include "dictionary.h"
#include "filemap.h"

using namespace std;
using recursive_directory_iterator = std::__fs::filesystem::recursive_directory_iterator;

vector<string> getFilesFromDir(const string& dirPath) {
    vector<string> files;
    for (const auto& dirEntry : recursive_directory_iterator(dirPath))
        if (!(dirEntry.path().filename() == ".DS_Store"))
            files.push_back(dirEntry.path());
    return files;
}

void handleWord(wstring &wordStr, const string& filePath, unordered_map <wstring, vector<Word*>> *dictionary,
               int *newWordInDict, int *newWordCount, int *multipleLemmasCount) {
    if (dictionary->find(wordStr) != dictionary->end()) {
        vector<Word*> words = dictionary->at(wordStr);
        if (words.size() > 1)
            *multipleLemmasCount += 1;

        for (Word* word: words) {
            if (word->partOfSpeech == L"UNKW")
                *newWordCount += 1;

            word->totalEntryCount++;
            word->textEntry.insert(filePath);
        }
    } else {
        *newWordInDict += 1;
        *newWordCount += 1;

        Word *newWord = new Word();
        newWord->word = wordStr;
        newWord->partOfSpeech = L"UNKW";
        dictionary->emplace(newWord->word, vector<Word*>{newWord});
    }
}

int handleFile(const string& filePath, unordered_map <wstring, vector<Word*>> *dictionary,
               int *newWordInDict, int *newWordCount, int *multipleLemmasCount) {
    auto& f = std::use_facet<std::ctype<wchar_t>>(std::locale());

    int wordHandled = 0;

    size_t length;
    auto filePtr = map_file(filePath.c_str(), length);
    auto lastChar = filePtr + length;

    while (filePtr && filePtr != lastChar) {
        auto stringBegin = filePtr;
        filePtr = static_cast<char *>(memchr(filePtr, '\n', lastChar - filePtr));
        if (filePtr)
            filePtr++;
        else
            break;

        wstring line = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(stringBegin, filePtr - 1);

        int begin = -1;
        for (int pos = 0; pos < line.size(); pos++) {
            wchar_t symbol = line[pos];
            if (iswcntrl(symbol) || iswpunct(symbol) || iswspace(symbol)
                || iswdigit(symbol) || iswcntrl(symbol)) {
                if (begin != -1) {
                    wstring wordStr = line.substr(begin, pos - begin);
                    begin = -1;

                    if (!wordStr.empty()) {
                        f.toupper(&wordStr[0], &wordStr[0] + wordStr.size());
                        handleWord(wordStr, filePath, dictionary, newWordInDict, newWordCount, multipleLemmasCount);
                        wordHandled++;
                    }
                }
            } else if (begin == -1) {
                begin = pos;
            }
        }
    }

    return wordHandled;
}

auto getStatistics(unordered_map <wstring, vector<Word*>> *dictionary) {
    set<Word*> correctLemmasSet;

    for(const auto& pair : *dictionary) {
        for (Word* word: pair.second) {
            if (word->totalEntryCount > 0) {
                correctLemmasSet.insert(word);
            }
        }
    }

    vector<Word*> correctLemmas;
    correctLemmas.reserve(correctLemmasSet.size());
    copy(correctLemmasSet.begin(), correctLemmasSet.end(), back_inserter(correctLemmas));

    sort(correctLemmas.begin(), correctLemmas.end(), [](const Word* a, const Word* b) {
        if (a->totalEntryCount > b->totalEntryCount) return true;
        if (a->totalEntryCount < b->totalEntryCount) return false;

        if (a->word < b->word) return true;
        if (a->word > b->word) return false;

        return false;
    });

    vector<pair<wstring, pair<int, Word*>>> statistics;
    for (Word* word : correctLemmas) {
        wstring currentLemmaValue = word->word;
        int currentLemmaCount = word->totalEntryCount;
        statistics.emplace_back(currentLemmaValue, make_pair(currentLemmaCount, word));
    }
    return statistics;
}

int main() {
    locale::global(locale("ru_RU.UTF-8"));
    wcout.imbue(locale("ru_RU.UTF-8"));

    auto dictionary = initDictionary("dict_opcorpora_clear.txt");
    vector<string> files = getFilesFromDir("/Users/titrom/Desktop/Computational Linguistics/Articles");

    int textCount = 0;
    int wordsHandled = 0;
    int newWordInDict = 0;
    int newWordCount = 0;
    int multipleLemmasCount = 0;

    cout << "Analysing texts...\n";
    for (const string& file : files) {
        int wordsHandledInText = handleFile(file, &dictionary, &newWordInDict,
                                            &newWordCount, &multipleLemmasCount);
        wordsHandled += wordsHandledInText;
        textCount++;
    }

    auto statistics = getStatistics(&dictionary);

    int totalPossibleLemmas = 0;
    for (auto elem : statistics) {
        if (elem.second.second->partOfSpeech != L"UNKW")
            totalPossibleLemmas += elem.second.second->totalEntryCount;
    }

    for (int i = 0; i < 100; ++i) {
        auto elem = statistics.at(i);
        wcout << elem.first << " " << elem.second.second->partOfSpeech
              << " - Count: " << elem.second.first
              << ", Text count: " << elem.second.second->textEntry.size()
              << "/" << textCount << endl;
    }

    wcout << endl
          << "Token handled: " << wordsHandled << endl
          << "Possible lemmas count: " << totalPossibleLemmas << endl
          << "Tokens with multiple lemmas: " << multipleLemmasCount
          << " (" << (float) multipleLemmasCount / wordsHandled * 100 << "%)" << endl
          << "The number of words that were not in the dictionary: " << newWordCount
          << " (" << (float) newWordCount / wordsHandled * 100 << "%)" << endl
          << "Precision: " << (float) wordsHandled / totalPossibleLemmas * 100 << "%";

    return 0;
}
