#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <regex>
#include "unordered_map"
#include "filesystem"
#include <locale>
#include <codecvt>
#include <string>
#include <set>

using namespace std;
using recursive_directory_iterator = std::__fs::filesystem::recursive_directory_iterator;

class Word {
    public:
        int entryCount = 0;
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
        wstring initialForm;

    explicit Word(const wstring& str, wstring initialForm) {
        wistringstream iss(str);
        wstring part;
        int partCount = 0;
        while ( getline( iss, part, L' ' ) ) {
            if (partCount == 0) {
                this->word = part;
            } else {
                this->writeGrammeme(part);
            }
            partCount++;
        }
        this->initialForm = move(initialForm);

//        cout << this->word << ' ' << this->partOfSpeech << ' ' << this->anim << ' ' << this->gender << ' ' << this->number << ' ' <<
//                this->Case << ' ' << this->aspc << ' ' << this->trns << ' ' << this->pers << ' ' << this->tens << ' ' <<
//                this->mood << ' ' << this->invi << ' ' << this->voic << ' ' << '\n';
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

ostream &operator<<(ostream &os, Word const &word) {
    return os << word.word.c_str() << " -> " << word.initialForm.c_str();
}

unordered_map <wstring, vector<Word>> initDictionary(const string& filename) {
    cout << "Initializing dictionary...\n";
    unordered_map <wstring, vector<Word>> dictionary;
    wifstream infile(filename);
    int wordCount = 0;
    wstring initForm;
    for(wstring line; getline(infile, line); ) {
        if (line.empty() || isdigit(line.at(0))) {
            initForm.clear();
            continue;
        }

        Word word = Word(line, initForm);
        if (initForm.empty()) {
            initForm = word.word;
            word.initialForm = initForm;
        }

        if (dictionary.find(word.word) != dictionary.end()) {
            dictionary[word.word].push_back(word);
        }
        else {
            dictionary.emplace(word.word, vector<Word> {word});
        }
        if (wordCount % 500000 == 0)
            cout << "Words inserted: " << wordCount << endl;
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

void handleFile(const string& filePath, unordered_map <wstring, vector<Word>> *dictionary) {
    auto& f = std::use_facet<std::ctype<wchar_t>>(std::locale());
    cout << "File: " << filePath << endl;
    wifstream infile(filePath);
    for (wstring line; getline(infile, line);) {
        wstring clearString = regex_replace( line, wregex(wstring(L"[[:punct:][:space:]]+")), wstring(L" "));
        wistringstream iss(clearString);
        for (wstring wordStr; getline(iss, wordStr, L' ');) {
            f.toupper(&wordStr[0], &wordStr[0] + wordStr.size());

            set<wstring> initForms {};
            if (dictionary->find(wordStr) != dictionary->end()) {
                vector<Word> words = dictionary->at(wordStr);
                for (const Word& word: words) {
                    initForms.insert(word.initialForm);
                }

                for (const wstring& initForm : initForms) {
                    vector<Word> initFormWords = dictionary->at(initForm);
                    for (auto & initFormWord : initFormWords) {
                        initFormWord.entryCount++;
                    }
                    dictionary->at(initForm) = initFormWords;
                }
            }
        }
    }
}

auto getStatistics(unordered_map <wstring, vector<Word>> *dictionary) {
    vector<vector<Word>> vals;
    vals.reserve(dictionary->size());

    for(const auto& kv : *dictionary) {
        if (kv.second.at(0).entryCount > 0)
            vals.push_back(kv.second);
    }

    sort(vals.begin(), vals.end(), [](const vector<Word>& a, const vector<Word>& b) {
        if (a.at(0).entryCount > b.at(0).entryCount) return true;
        if (a.at(0).entryCount < b.at(0).entryCount) return false;

        if (a.at(0).word < b.at(0).word) return true;
        if (a.at(0).word > b.at(0).word) return false;

        return false;
    });

    vector<pair<wstring , pair<int, vector<Word>>>> statistics;
    for (const vector<Word>& wordVector : vals) {
        if (wordVector.at(0).entryCount == 0)
            break;

        wstring currentLemmaValue = wordVector.at(0).word;
        int currentLemmaCount = wordVector.at(0).entryCount;
        statistics.emplace_back(currentLemmaValue, make_pair(currentLemmaCount, wordVector));
    }
    return statistics;
}

int main() {
    locale::global(locale("ru_RU.UTF-8"));
    wcout.imbue(locale("ru_RU.UTF-8"));

    auto dictionary = initDictionary("dict.opcorpora_clear.txt");
    vector<string> files = getFilesFromDir("/Users/titrom/Desktop/Computational Linguistics/Articles");

    for (const string& file : files) {
        handleFile(file, &dictionary);
    }

    for (const auto& elem : getStatistics(&dictionary)) {
        wcout << elem.first << " - " << elem.second.first << endl;
    }
    return 0;
}
