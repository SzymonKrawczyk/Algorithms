/*

    Tablica z Hashowaniem   (Wersja z tablica wskaznikow na dane oraz z przesypywaniem ostatniego pasujacego elementu na miejsce wolne)

    Autor: Szymon Krawczyk

    Data utworzenia: 08.03.2020


    Modyfikacje:

        14.03.2020 - Przejscie z tablicy zwyklej na tablice wskaznikow 
                        (wieksza wydajnosc pamieciowa dla tablic ktore nie sa od razu zapelnione, wieksza elastycznosc kodu)

        17.03.2020 - Poprawienie komentarzy

*/


#include <iostream>
#include <string>
#include <stdlib.h>     /* exit, EXIT_FAILURE */ // Raczej niepotrzebne, gdyz dane maja byc poprawne

using std::cout;
using std::cin;
using std::string;

// Parser help

    int countSpaces(const string&);

    unsigned int stringToUInt(const string&);
    unsigned int power10u(int);

    long stringToLong(const string&);
    long power10l(int);

//


// Main class

    class myHashTable
    {
    private:

        struct data {
            long key;
            char val[8];
        };

        data** _dataContainer;
        unsigned int _size;

    protected:

        void resetDataCell(unsigned int);
        unsigned int returnHash(long);
        unsigned int findAvailableIndex(unsigned int);
        void fillEmptyIndex(unsigned int);
        void moveData(unsigned int, unsigned int);

    public:

        myHashTable(unsigned int = 0);
        virtual ~myHashTable();

        void size(unsigned int);
        void print();
        void add(long, const string&);
        void delete_(long);

    };


    // Class helpers

        void copy8chars(char*, const string&);
        void write8chars(const char*);
        void reset8chars(char*);

        unsigned int distance(unsigned int, unsigned int, unsigned int);    // Oblicza odleglosc (w dol) od miejsca do innego (z zawijaniem tablicy)
    //


    myHashTable::myHashTable(unsigned int size_)
        :
            _size(size_)
        ,   _dataContainer(nullptr)
    {
        _dataContainer = new data*[size_];

        for (unsigned int i = 0; i < _size; ++i) {

            _dataContainer[i] = nullptr;
        }
    }

    myHashTable::~myHashTable() {

        for (unsigned int i = 0; i < _size; ++i) {

            delete _dataContainer[i];
        }
        delete[] _dataContainer;
    }

    void myHashTable::resetDataCell(unsigned int index_) {

        delete _dataContainer[index_];
        _dataContainer[index_] = nullptr;
    }

    unsigned int myHashTable::returnHash(long key_) {

        return key_%_size;
    }

    unsigned int myHashTable::findAvailableIndex(unsigned int start_) {

        unsigned int i = start_;
        do {
            i %= _size;                         // Gdy poza tablica, pojscie na poczatek
            if (_dataContainer[i] == nullptr) { // Jesli wolne miejsce
                return i;
            }
            ++i;
        } while (i != start_);                  // Do momentu powrotu na start
        throw("\nBrak miejsca w tablicy\n");    // Zadanie gwarantuje poprawnosc danych, wiec nie powinno sie tutaj dojsc
    }

    void myHashTable::fillEmptyIndex(unsigned int index_) {

        if (_dataContainer[(index_+1u)%_size] != nullptr) {     // Postaraj sie zapelnic puste miejsce, jesli nastepne nie jest puste 
                                                                //  (czyli jesli moga byc pasujace na to miejsce dane dalej w tablicy)
            unsigned int i = index_;
            if (i == 0u) {                  // Na poczatku?
                i = _size - 1u;             // Idz na koniec
            }
            else {
                --i;                        // Cofnij o jeden indeks
            }
            do {
                if (_dataContainer[i] != nullptr) {
                    if (distance(index_, returnHash((*_dataContainer[i]).key), _size) < distance(i, returnHash((*_dataContainer[i]).key), _size)) {

                                                            // Jesli puste miejsce oferuje lepsze polozenie (mniejsza odleglosc w kierunku dolnym)
                                                            //  wzgledem miejsca gdzie dany element powinien byc niz jego obecne polozenie,
                        _dataContainer[index_] = new data;
                        moveData(i, index_);                //  przenies go na nowe miejsce
                        fillEmptyIndex(i);                  //  oraz wykonaj cala operacje dla nowego pustego miejsca
                                                            //  powstalego po przeniesieniu
                        break;
                    }
                }

                if (i == 0u) {
                    i = _size - 1u;
                }
                else {
                    --i;
                }
            } while (i != index_);      // Szukaj emelentu do momentu powrotu na start
        }
    }

    void myHashTable::moveData(unsigned int what_, unsigned int where_) {

        for (int i = 0; i < 8; i++) {
            
            (*_dataContainer[where_]).val[i] = (*_dataContainer[what_]).val[i];
        }
        (*_dataContainer[where_]).key = (*_dataContainer[what_]).key;
        resetDataCell(what_);
    }

    void myHashTable::size(unsigned int size_) {

        for (unsigned int i = 0; i < _size; ++i) {

            delete _dataContainer[i];
        }
        delete[] _dataContainer;

        _size = size_;
        _dataContainer = new data*[size_];
        for (unsigned int i = 0; i < _size; ++i) {

            _dataContainer[i] = nullptr;
        }
    }

    void myHashTable::print() {

        for (unsigned int i = 0; i < _size; i++) {

            if (_dataContainer[i] != nullptr) {     // Jesli element istnieje

                cout << i << ' ' << (*_dataContainer[i]).key << ' ';
                write8chars((*_dataContainer[i]).val);
                cout << '\n';
            }
        }
        cout << '\n';
    }

    void myHashTable::add(long key_, const string& chars_) {

        unsigned int index = findAvailableIndex(returnHash(key_));
        _dataContainer[index] = new data;
        (*_dataContainer[index]).key = key_;
        reset8chars((*_dataContainer[index]).val);
        copy8chars((*_dataContainer[index]).val, chars_);
                                                            // Dlaczego reset a potem przepisywanie? 
                                                            //  Chodzi o to, ze jak string jest mniejszy niz 8 wtedy bedzie w tablicy to,
                                                            //  co akurat bylo w pamieci (nie ma zakonczenia) 
                                                            //   (a gdy wyswietlam 8 charow ktore akurat wszystkie 
                                                            //   powinny byc wypelnione to wyswietliloby jeszcze rzeczy poza tablica,
                                                            //   gdyz nie ma miejsca na znak zakonczenia
                                                            //   (wtedy tablice mialyby 9 miejsc, a to 1/8 wiecej pamieci))
    }

    void myHashTable::delete_(long key_) {

        unsigned int start = returnHash(key_);
        unsigned int i = start;
        do {
            if ((*_dataContainer[i]).key == key_) {
                resetDataCell(i);
                fillEmptyIndex(i);
                break;
            }
            ++i;
            i %= _size;
        } while (i != start && _dataContainer[i] != nullptr);       // Szukaj do memntu powrotu na start
                                                                    //  lub znalezenia 'pustego' elementu 
                                                                    //  (oznacza to brak pasujacych elementow)
    }
//

int main() {

    try {
        unsigned int numberOfCycles = 0;

        cin >> numberOfCycles;
        for (size_t i = 0; i < numberOfCycles; i++) {

            myHashTable hashTable;
            string input;

            getline(cin, input);
            while (input != "stop") {

                int distinctInputs = countSpaces(input) + 1;
                string nameOfCommand;
                string firstParam, secondParam, thirdParam;
                string::size_type spacePos1, spacePos2;         // Pozycje spacji

                switch (distinctInputs) {
                case 1:

                    if (input == "print") {
                        hashTable.print();
                    }
                    break;

                case 2:

                    spacePos1 = input.find(' ');                // Znajdz spacje

                    // Podziel wejscie na parametry
                    firstParam = input.substr(0, spacePos1);
                    secondParam = input.substr(spacePos1+1);
                
                    nameOfCommand = firstParam;
                    if (nameOfCommand == "size") {
                        hashTable.size(stringToUInt(secondParam));
                    }
                    else if (nameOfCommand == "delete") {
                        hashTable.delete_(stringToLong(secondParam));
                    }

                    break;

                case 3:

                    spacePos1 = input.find(' ');                // Znajdz pierwsza spacje
                    spacePos2 = input.find(' ', spacePos1+1);   // Znajdz druga spacje

                    // Podziel wejscie na parametry
                    firstParam  = input.substr(0, spacePos1);
                    secondParam = input.substr(spacePos1 + 1, spacePos2 - (spacePos1+1));
                    thirdParam  = input.substr(spacePos2 + 1);

                    nameOfCommand = firstParam;
                    if (nameOfCommand == "add") {

                        hashTable.add(stringToLong(secondParam), thirdParam);
                    }
                    break;

                }
                getline(cin, input);
            }
        }
    }
    catch (const char* err) {   // Raczej niepotrzebne, dane maja byc poprawne
        std::clog << err;
        exit(EXIT_FAILURE);
    }
    return 0;
}



// Class helpers implementation

    void copy8chars(char* charVal_, const string& input_) {

        for (int i = 0; i < 8 && i < input_.size(); i++) {
            charVal_[i] = input_[i];
        }
    }

    void write8chars(const char* charVal_) {

        const char* temp = charVal_;

        for (int i = 0; i < 8; i++) {
            if (temp[i] == '\0') {
                break;
            }
                cout << temp[i];
        }
    }

    void reset8chars(char* charVal_) {

        for (int i = 0; i < 8; i++) {
            charVal_[i] = '\0';
        }
    }

    unsigned int distance(unsigned int availableIndex, unsigned int targetedIndex, unsigned int size) {

        unsigned int dist = 0u;

        if (availableIndex < targetedIndex) {
            dist = availableIndex + size - targetedIndex;
        }
        else {
            dist = availableIndex - targetedIndex;
        }

        return dist;
    }
//


// Parser help implementation

    int countSpaces(const string& input_) {
        int temp = 0;
        for (int i = 0; i < input_.size(); i++) {
            if (input_[i] == ' ') {
                ++temp;
            }
        }
        return temp;
    }

    unsigned int stringToUInt(const string& input_) {

        unsigned int temp = 0;
        for (int i = input_.size()-1, count = 0; i >= 0; i--, count++) {
            temp += (input_[i] - '0') * power10u(count);
        }
        return temp;
    }

    unsigned int power10u(int y) {

        unsigned int temp = 1u;
        for (int i = 0; i < y; i++) {
            temp *= 10u;
        }
        return temp;
    }

    long stringToLong(const string& input_) {

        long temp = 0l;
        for (int i = input_.size() - 1, count = 0; i >= 0; i--, count++) {
            temp += (input_[i] - '0') * power10l(count);
        }
        return temp;
    }

    long power10l(int y) {

        long temp = 1l;
        for (int i = 0; i < y; i++) {
            temp *= 10l;
        }
        return temp;
    }
//