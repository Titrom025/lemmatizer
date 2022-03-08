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

using namespace std;
using recursive_directory_iterator = std::__fs::filesystem::recursive_directory_iterator;

int MIN_WORD_LENGTH;
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
        wistringstream iss(str);
        wstring part;
        int partCount = 0;
        while ( getline(iss, part, L' ') ) {
            if (partCount == 0) {
                this->word = part;
            } else {
                this->writeGrammeme(part);
            }
            partCount++;
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

unordered_map <wstring, vector<Word*>> initDictionary(const string& filename) {
    cout << "Initializing dictionary...\n";
    unordered_map <wstring, vector<Word*>> dictionary;
    wifstream infile(filename);
    int wordCount = 0;
    Word *initWord = nullptr;
    for(wstring line; getline(infile, line); ) {
        if (line.empty() || isdigit(line.at(0))) {
            initWord = nullptr;
            continue;
        }

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

int handleFile(const string& filePath, unordered_map <wstring, vector<Word*>> *dictionary,
               int *newWordInDict, int *newWordCount, int *multipleLemmasCount) {
    auto& f = std::use_facet<std::ctype<wchar_t>>(std::locale());
    cout << "File: " << filePath << endl;

    int wordHandled = 0;
    wifstream infile(filePath);
    for (wstring line; getline(infile, line);) {

        wstring clearString = regex_replace( line, wregex(wstring(L"[[:punct:][:space:]]+")), wstring(L" "));
        wistringstream iss(clearString);
        for (wstring wordStr; getline(iss, wordStr, L' ');) {
            if (wordStr.empty())
                continue;

            f.toupper(&wordStr[0], &wordStr[0] + wordStr.size());

            if (dictionary->find(wordStr) != dictionary->end()) {
                vector<Word*> words = dictionary->at(wordStr);
                if (words.size() > 1) {
                    *multipleLemmasCount += 1;
//                    wcout << words.size() << " - " << wordStr << endl;
                }

                for (Word* word: words) {
                    if (word->partOfSpeech == L"UNKW")
                        *newWordCount += 1;

                    word->totalEntryCount++;
                    word->textEntry.insert(filePath);
                }
                wordHandled++;
            } else if (!(isdigit(wordStr[0]))) {
                *newWordInDict += 1;
                *newWordCount += 1;

                Word *newWord = new Word();
                newWord->word = wordStr;
                newWord->partOfSpeech = L"UNKW";

                dictionary->emplace(newWord->word, vector<Word*>{newWord});
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
                if (!(word->partOfSpeech == L"PREP" || word->partOfSpeech == L"CONJ" ||
                        word->partOfSpeech == L"PRCL" || word->partOfSpeech == L"INTJ" ||
                        word->partOfSpeech == L"NPRO" || word->partOfSpeech == L"PRED" ||
                        word->partOfSpeech == L"NUMR" || word->word.length() < MIN_WORD_LENGTH))
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
    MIN_WORD_LENGTH = 3;

    locale::global(locale("ru_RU.UTF-8"));
    wcout.imbue(locale("ru_RU.UTF-8"));

    auto dictionary = initDictionary("dict_opcorpora_clear.txt");
    vector<string> files = getFilesFromDir("/Users/titrom/Desktop/Computational Linguistics/Articles");

    int textCount = 0;
    int wordsHandled = 0;
    int newWordInDict = 0;
    int newWordCount = 0;
    int multipleLemmasCount = 0;
    for (const string& file : files) {
        int wordsHandledInText = handleFile(file, &dictionary, &newWordInDict,
                                            &newWordCount, &multipleLemmasCount);
        wordsHandled += wordsHandledInText;
        textCount++;
    }

    auto statistics = getStatistics(&dictionary);

    for (int i = 0; i < 100; ++i) {
        auto elem = statistics.at(i);
        wcout << elem.first << " " << elem.second.second->partOfSpeech
              << " - Count: " << elem.second.first
              << ", Text count: " << elem.second.second->textEntry.size()
              << "/" << textCount << endl;
    }

    wcout << endl
          << "WordsHandled: " << wordsHandled
          << ", MultipleLemmasCount: " << multipleLemmasCount
          << endl
          << "Words added to dict: " << newWordInDict
          << ", words not in initial dict: " << newWordCount
          << endl;
    return 0;
}
