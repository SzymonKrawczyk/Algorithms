/*

    HeapSort

    Autor: Szymon Krawczyk

    Data utworzenia: 22.03.2020

    Modyfikacja: 23.03.2020  |  Zmiana sposobu poczatkowego formowania kopca
                                (zamiast kopcowac cala tablice, kopcuje po dodaniu kazdego elementu;
                                 poprawnosc struktury jest ta sama, jednak jest teraz bardziej intuicyjnie)

    Modyfikacja: 31.03.2020  |  Dodanie komentarzy

*/

#include <iostream>

using std::cin;
using std::cout;

namespace KrawczykS {

    // Klasa kopca

        class Heap {

            private:

                long* _tab;             // Tablica do sertowania
                unsigned int _sorted;   // Ilosc elementow pokopcowanych
                unsigned int _max;      // Maksymalna ilosc elementow

            protected:

                void sort();
                void heap_Create();
                void heap_correct(unsigned int);
                void showHeap();
                void swap(long*, long*);

            public:

                Heap(long*, unsigned int);  // Podajemy tylko tablice i jej rozmiar
                Heap(const Heap&) = delete;
                Heap& operator=(const Heap&) = delete;
                virtual ~Heap();

        };

        Heap::Heap(long* in_, unsigned int size_)
            :
                _tab(in_)
            ,   _sorted(0u)
            ,   _max(size_)
        {
            sort();
        }

        Heap::~Heap() {}    // nie ma new, pusty destruktor

        void Heap::sort() {

            heap_Create();
            if (_max > 1u) showHeap();          // Nie ma pokazywania samego korzenia

            while (_sorted > 1u) {

                swap(&_tab[0], &_tab[_sorted - 1u]);
                --_sorted;

                heap_correct(0);                // 'Popraw tylko pozycje ktora nie jest w dobrej pozycji'
                                                //    Ta funkcja wywoluje sie dla kolejnych zamienionych elementow, 
                                                //    wiec przejdziemy przez wszystkie zmiany i bedzie poprawny kopiec

                if (_sorted > 1u) showHeap();   // Nie ma pokazywania samego korzenia
            }

        }

        void Heap::heap_Create() {

            for (unsigned int i = 0u; i < _max; i++) {

                ++_sorted;                      // Dodajemy element do kopcowania
                heap_correct((i - 1u) / 2u);    // Kopcujemy dla rodzica elementu
                                                //
                                                //  Notka: jezeli chcemy dodac cos do kopca, wystarczy wywolac funkcje 
                                                //         kopcujaca dla rodzica nowego elementu
            }
        }

        void Heap::heap_correct(unsigned int pos) {

            if (pos < _sorted) {

                unsigned int parent = (pos - 1u) / 2u;
                unsigned int child1 = 2u * pos + 1u;
                unsigned int child2 = 2u * pos + 2u;

                if (child1 < _sorted && _tab[pos] < _tab[child1]) {

                    swap(&_tab[pos], &_tab[child1]);
                    heap_correct(pos);          // Na wypadek, gdyby drugie dziecko bylo wieksze od 1
                    heap_correct(child1);
                    if (pos != 0ul) heap_correct(parent);
                    return;
                }
                if (child2 < _sorted && _tab[pos] < _tab[child2]) {

                    swap(&_tab[pos], &_tab[child2]);
                    heap_correct(child2);
                    if (pos != 0ul) heap_correct(parent);
                    return;
                }
                return;
            }
        }

        void Heap::showHeap() {

            for (unsigned int i = 0u; i < _sorted; i++) {

                cout << _tab[i] << ' ';
            }
            cout.seekp(-1, std::ios_base::cur);
            cout << std::endl;
        }

        void Heap::swap(long* a, long* b) {

            long temp = *a;
            *a = *b;
            *b = temp;
        }
    //
}

int main() {

    unsigned int cycles = 0u;
    cin >> cycles;

    for (unsigned int i = 0u; i < cycles; ++i) {

        unsigned int size = 0u;
        cin >> size;
        long* container = new long[size];

        for (unsigned int i = 0u; i < size; ++i) {

            cin >> container[i];
        }

        KrawczykS::Heap heap(container, size);      // Chcialem, aby uzywanie sortowania bylo 'naturalne' - proste dla uzytkownika
                                                    //  (nie ma funkcji jak np "add" czy "sort" - dzieki temu jest to prostsze do uzycia
                                                    //   w potencjalnych programach - wszystko zawiera sie w jednej linii)

        for (unsigned int i = 0u; i < size; ++i) {

            cout << container[i] << ' ';
        }
        cout.seekp(-1, std::ios_base::cur);
        cout << '\n' << '\n';

        delete[] container;
    }
    return 0;
}