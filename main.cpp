#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <regex>
#include "unordered_map"
#include "filesystem"
#include <codecvt>
#include <string>
#include <set>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
using recursive_directory_iterator = std::__fs::filesystem::recursive_directory_iterator;

class Word {
    public:
        int totalEntryCount = 0;
        set <string> textEntry;
        wstring word;
        wstring partOfSpeech;
        wstring anim;
        wstring gender;
        wstring number;
        wstring Case;
        wstring aspc;
        wstring trns;
        wstring pers;
        wstring tens;
        wstring mood;
        wstring invi;
        wstring voic;

    Word() {}

    explicit Word(const wstring& str) {
        int begin = 0;
        for (int pos = 0; pos < str.size(); pos++) {
            if (str[pos] == L' ') {
                if (begin == 0)
                    this->word = str.substr(begin, pos - begin);
                else
                    this->writeGrammeme(str.substr(begin, pos - begin));

                begin = pos + 1;
            }

        }
    }

    void writeGrammeme(const wstring& str) {
        if (str == L"NOUN" || str == L"ADJF" || str == L"ADJS" || str == L"COMP" ||
            str == L"VERB" || str == L"INFN" || str == L"PRTF" || str == L"PRTS" ||
            str == L"GRND" || str == L"NUMR" || str == L"ADVB" || str == L"NPRO" ||
            str == L"PRED" || str == L"PREP" || str == L"CONJ" || str == L"PRCL" ||
            str == L"INTJ") {
            this->partOfSpeech = str;
        } else
        if (str == L"anim" || str == L"inan") {
            this->anim = str;
        } else
        if (str == L"masc" || str == L"femn" || str == L"neut" || str == L"ms-f" ||
            str == L"GNdr") {
            this->gender = str;
        } else
        if (str == L"sing" || str == L"plur" || str == L"Sgtm" || str == L"Pltm" || str == L"Fixd") {
            this->number = str;
        } else
        if (str == L"nomn" || str == L"gent" || str == L"datv" || str == L"accs" ||
            str == L"ablt" || str == L"loct" || str == L"voct" || str == L"gen1" ||
            str == L"gen2" || str == L"acc2" || str == L"loc1" || str == L"loc2" ||
            str == L"Abbr" || str == L"Name" || str == L"Surn" || str == L"Patr" ||
            str == L"Geox" || str == L"Orgn" || str == L"Trad" || str == L"Subx" ||
            str == L"Supr" || str == L"Qual" || str == L"Apro" || str == L"Anum" ||
            str == L"Poss" || str == L"V-ey" || str == L"V-oy" || str == L"Cmp2" ||
            str == L"V-ej") {
            this->Case = str;
        } else
        if (str == L"perf" || str == L"impf") {
            this->aspc = str;
        } else
        if (str == L"tran" || str == L"intr" || str == L"Impe" || str == L"Impx" ||
            str == L"Mult" || str == L"Refl") {
            this->trns = str;
        } else
        if (str == L"1per" || str == L"2per" || str == L"3per") {
            this->pers = str;
        } else
        if (str == L"pres" || str == L"past" || str == L"futr") {
            this->tens = str;
        } else
        if (str == L"indc" || str == L"impr") {
            this->mood = str;
        } else
        if (str == L"incl" || str == L"excl") {
            this->invi = str;
        } else
        if (str == L"actv" || str == L"pssv" || str == L"Infr" || str == L"Slng" ||
            str == L"Arch" || str == L"Litr" || str == L"Erro" || str == L"Dist" ||
            str == L"Ques" || str == L"Dmns" || str == L"Prnt" || str == L"V-be" ||
            str == L"V-en" || str == L"V-ie" || str == L"V-bi" || str == L"Fimp" ||
            str == L"Prdx" || str == L"Coun" || str == L"Coll" || str == L"V-sh" ||
            str == L"Af-p" || str == L"Inmx" || str == L"Vpre" || str == L"Anph" ||
            str == L"Init" || str == L"Adjx" || str == L"Ms-f" || str == L"Hypo") {
            this->voic = str;
        }
    }

};

wostream &operator<<(wostream &os, Word const &word) {
    return os << word.word.c_str() << " : " << word.partOfSpeech;
}

wstring getWord(const wstring &line) {
    int endPosition = 0;
    while (line.at(endPosition) != L' ')
        endPosition++;

    return line.substr(0, endPosition);
}

void handle_error(const char* msg) {
    perror(msg);
    exit(255);
}

char* map_file(const char* fname, size_t& length)
{
    int fd = open(fname, O_RDONLY);
    if (fd == -1)
        handle_error("open");

    struct stat sb;
    if (fstat(fd, &sb) == -1)
        handle_error("fstat");

    length = sb.st_size;

    char* addr = static_cast<char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));
    if (addr == MAP_FAILED)
        handle_error("mmap");

    return addr;
}

unordered_map <wstring, vector<Word*>> initDictionary(const string& filename) {
    cout << "Initializing dictionary...\n";
    unordered_map <wstring, vector<Word*>> dictionary;
    wifstream infile(filename);
    int wordCount = 0;
    Word *initWord = nullptr;

    size_t length;
    auto filePtr = map_file(filename.c_str(), length);
    auto lastChar = filePtr + length;

    while (filePtr && filePtr != lastChar) {
        auto stringBegin = filePtr;
        filePtr = static_cast<char *>(memchr(filePtr, '\n', lastChar - filePtr));
        if (filePtr)
            filePtr++;
        else
            break;

        if (filePtr - stringBegin < 10) {
            initWord = nullptr;
            continue;
        }

        wstring line = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(stringBegin, filePtr - 1);

        if (initWord == nullptr) {
            initWord = new Word(line);
        }

        wstring word = getWord(line);

        if (dictionary.find(word) != dictionary.end()) {
            bool wordExist = false;
            for (Word *existWord : dictionary.at(word)) {
                if (existWord == initWord) {
                    wordExist = true;
                    break;
                }
            }
            if (!wordExist)
                dictionary[word].push_back(initWord);
        } else {
            dictionary.emplace(word, vector<Word*>{initWord});
        }
        if (wordCount % 500000 == 0) {
            cout << "Words inserted: " << wordCount << endl;
        }
        wordCount++;
    }

    cout << "Dictionary initialized.\n";
    return dictionary;
}


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
