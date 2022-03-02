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
        string word;
        string partOfSpeech;
        string anim;
        string gender;
        string number;
        string Case;
        string aspc;
        string trns;
        string pers;
        string tens;
        string mood;
        string invi;
        string voic;
        string initialForm;

    explicit Word(const string& str, string initialForm) {
        istringstream iss(str);
        string part;
        int partCount = 0;
        while ( getline( iss, part, ' ' ) ) {
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


    void writeGrammeme(const string& str) {
        if (str == "NOUN" || str == "ADJF" || str == "ADJS" || str == "COMP" ||
            str == "VERB" || str == "INFN" || str == "PRTF" || str == "PRTS" ||
            str == "GRND" || str == "NUMR" || str == "ADVB" || str == "NPRO" ||
            str == "PRED" || str == "PREP" || str == "CONJ" || str == "PRCL" ||
            str == "INTJ") {
            this->partOfSpeech = str;
        } else
        if (str == "anim" || str == "inan") {
            this->anim = str;
        } else
        if (str == "masc" || str == "femn" || str == "neut" || str == "ms-f" ||
            str == "GNdr") {
            this->gender = str;
        } else
        if (str == "sing" || str == "plur" || str == "Sgtm" || str == "Pltm" || str == "Fixd") {
            this->number = str;
        } else
        if (str == "nomn" || str == "gent" || str == "datv" || str == "accs" ||
            str == "ablt" || str == "loct" || str == "voct" || str == "gen1" ||
            str == "gen2" || str == "acc2" || str == "loc1" || str == "loc2" ||
            str == "Abbr" || str == "Name" || str == "Surn" || str == "Patr" ||
            str == "Geox" || str == "Orgn" || str == "Trad" || str == "Subx" ||
            str == "Supr" || str == "Qual" || str == "Apro" || str == "Anum" ||
            str == "Poss" || str == "V-ey" || str == "V-oy" || str == "Cmp2" ||
            str == "V-ej") {
            this->Case = str;
        } else
        if (str == "perf" || str == "impf") {
            this->aspc = str;
        } else
        if (str == "tran" || str == "intr" || str == "Impe" || str == "Impx" ||
            str == "Mult" || str == "Refl") {
            this->trns = str;
        } else
        if (str == "1per" || str == "2per" || str == "3per") {
            this->pers = str;
        } else
        if (str == "pres" || str == "past" || str == "futr") {
            this->tens = str;
        } else
        if (str == "indc" || str == "impr") {
            this->mood = str;
        } else
        if (str == "incl" || str == "excl") {
            this->invi = str;
        } else
        if (str == "actv" || str == "pssv" || str == "Infr" || str == "Slng" ||
            str == "Arch" || str == "Litr" || str == "Erro" || str == "Dist" ||
            str == "Ques" || str == "Dmns" || str == "Prnt" || str == "V-be" ||
            str == "V-en" || str == "V-ie" || str == "V-bi" || str == "Fimp" ||
            str == "Prdx" || str == "Coun" || str == "Coll" || str == "V-sh" ||
            str == "Af-p" || str == "Inmx" || str == "Vpre" || str == "Anph" ||
            str == "Init" || str == "Adjx" || str == "Ms-f" || str == "Hypo") {
            this->voic = str;
        }
    }

};

ostream &operator<<(ostream &os, Word const &word) {
    return os << word.word << " -> " << word.initialForm;
//                this->Case << ' ' << this->aspc << ' ' << this->trns << ' ' << this->pers << ' ' << this->tens << ' ' <<
//                this->mood << ' ' << this->invi << ' ' << this->voic << ' ' << '\n';
}

unordered_map <string, vector<Word>> initDictionary(const string& filename) {
    cout << "Initializing dictionary...\n";
    unordered_map <string, vector<Word>> dictionary;
    ifstream infile(filename);
    int wordCount = 0;
    string initForm;
    for(string line; getline(infile, line); ) {
        char *endptr;
        std::strtol(line.c_str(), &endptr, 10);
        if (*endptr == '\0') {
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

void handleFile(const string& filePath, unordered_map <string, vector<Word>> *dictionary) {
    auto& f = std::use_facet<std::ctype<wchar_t>>(std::locale());
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    cout << "File: " << filePath << endl;
    ifstream infile(filePath);
    for (string line; getline(infile, line);) {
        string clearString = std::regex_replace( line, regex( "[[:punct:][:space:]]+"), " ");
        istringstream iss(clearString);
        for (string wordStr; getline(iss, wordStr, ' ');) {
            std::wstring wideStr = converter.from_bytes(wordStr);
            f.toupper(&wideStr[0], &wideStr[0] + wideStr.size());
            wordStr = converter.to_bytes(wideStr);

            set<string> initForms {};
            if (dictionary->find(wordStr) != dictionary->end()) {
                vector<Word> words = dictionary->at(wordStr);
                for (const Word& word: words) {
                    initForms.insert(word.initialForm);
                }

                for (const string& initForm : initForms) {
                    vector<Word> initFormWords = dictionary->at(initForm);
                    for (auto & initFormWord : initFormWords) {
                        initFormWord.entryCount++;
                    }
                    dictionary->at(initForm) = initFormWords;
//                    cout << "Word: " << wordStr <<  " - Lemma: " << initForm << endl;
                }
            }
//            else {
//                cout << "Word not founded: " << wordStr << endl;
//            }
        }
    }
}

auto getStatistics(unordered_map <string, vector<Word>> *dictionary) {
    vector<vector<Word>> vals;
    vals.reserve(dictionary->size());

    for(const auto& kv : *dictionary) {
        vals.push_back(kv.second);
//        for (const Word& word : kv.second) {
//            vals.push_back(word);
//        }
    }

    sort(vals.begin(), vals.end(), [](const vector<Word>& a, const vector<Word>& b) {
//        return (a.at(0).entryCount > b.at(0).entryCount);
        if (a.at(0).entryCount > b.at(0).entryCount) return true;
        if (a.at(0).entryCount < b.at(0).entryCount) return false;

        if (a.at(0).word < b.at(0).word) return true;
        if (a.at(0).word > b.at(0).word) return false;

        return false;
    });

    vector<pair<string , pair<int, vector<Word>>>> statistics;
    for (const vector<Word>& wordVector : vals) {
        if (wordVector.at(0).entryCount == 0)
            break;

        string currentLemmaValue = wordVector.at(0).word;
        int currentLemmaCount = wordVector.at(0).entryCount;
        statistics.emplace_back(currentLemmaValue, make_pair(currentLemmaCount, wordVector));
    }
    return statistics;
}

int main() {
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());

    auto dictionary = initDictionary("dict.opcorpora_clear.txt");
    vector<string> files = getFilesFromDir("/Users/titrom/Desktop/Computational Linguistics/Articles");

    for (const string& file : files) {
        cout << file << endl;
        handleFile(file, &dictionary);
//        break;
    }

    for (const auto& elem : getStatistics(&dictionary)) {
        cout << elem.first << " - " << elem.second.first << endl;
    }
    return 0;
}
