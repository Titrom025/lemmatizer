#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "unordered_map"

using namespace std;

class Word {
    public:
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

    explicit Word(const string& str) {
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

vector<Word> initDictionary(const string& filename) {
    cout << "Initializing dictionary...\n";
    vector <Word> wordVector;
    unordered_map <string, vector<Word>> dictionary;
    ifstream infile(filename);
    int wordCount = 0;
    for(string line; getline( infile, line ); ) {
        char *endptr;
        std::strtol(line.c_str(), &endptr, 10);
        if (*endptr == '\0') {
            continue;
        }
        Word word = Word(line);
        if (dictionary.find(word.word) != dictionary.end()) {
            dictionary[word.word].push_back(word);
        }
        else {
            dictionary.emplace(word.word, vector<Word> {word});
        }
        if (wordCount % 500000 == 0)
            cout << "Words inserted: " << wordCount << "\n";
        wordCount++;
    }
    cout << "Dictionary initialized.\n";
    return wordVector;
}
int main() {
    vector<Word> wordVector = initDictionary("dict.opcorpora.txt");

    return 0;
}
