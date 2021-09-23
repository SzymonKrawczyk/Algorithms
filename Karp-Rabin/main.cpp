//
//      Wyszukiwanie KR
//
//  Autor: Szymon Krawczyk
//
//  Data utworzenia: 29.04.2020
//
//  Modyfikacje:
//
//      10.05.2020 by Szymon Krawczyk:
//          Przyspieszenie z 200000ms do 300ms na jednym z testow
//          Poprawienie kilku warningow
//
//      12.05.2020 by Szymon Krawczyk:
//			Usuniecie mierzenia czasu
//          Dodanie komentarzy
//
//


#include <iostream>
#include <string>
#include <fstream>
#include <chrono> // do mierzenia czasu

using std::cout;
using std::cin;
using std::string;

namespace KrawczykS {

	class StringFinder {

		private:

			class StringHash {

				public:

					//static const unsigned short PRIME = 65111u;		// (liczba pierwsza * podstawa) + podstawa =< pojemność * 2, a pojemnosc*2 to unsigned long wiec 
					static const unsigned short PRIME = 2u;		// (liczba pierwsza * podstawa) + podstawa =< pojemność * 2, a pojemnosc*2 to unsigned long wiec 
																	//	najwieksza liczba pierwsza dla unsigned short jest najlepsza dla najmniejszej
																	//	ilosci kolizji
					static const unsigned long BASE = 256ul;

					unsigned short _value;
					unsigned int   _size;

				protected:

					unsigned short returnHash(const string&) const;

				public:

					StringHash(const string&);
					StringHash(const StringHash&) = delete;
					~StringHash();
					StringHash& operator= (const StringHash&) = delete;

					void add(const char&, const char&, const unsigned long&);
			};

		public:

			StringFinder();
			StringFinder(const StringHash&) = delete;
			~StringFinder();
			StringFinder& operator= (const StringFinder&) = delete;

			void find(const string&, const string&) const;
	};

	StringFinder::StringFinder() {}

	StringFinder::~StringFinder() {}

	void StringFinder::find(const string& filename_, const string& query_) const {

		try {

			// Plik (czytanie calej zawartosci na raz)

				std::fstream fileIn;
				fileIn.open(filename_.c_str(), std::ios::in);
				if (!fileIn.is_open()) {
					throw ("\nBlad otwarcia pliku " + filename_ + "\n");
				}

				fileIn.seekg(0, std::ios_base::end);								// Przenies znacznik na koniec
				unsigned int filesize = (unsigned int) (fileIn.tellg());			// Pozycja = ilosc znakow w pliku
				char* fileContent = new char[filesize];								// Utworz odpowiednia tablice
				fileIn.seekg(0, std::ios_base::beg);								// Wroc znacznik na poczatek

				fileIn.read(fileContent, filesize);									// Odczytaj <ilosc wszystkich znakow> z pliku do tablicy

				fileIn.close();
			//


			// Algorytm				

				StringHash queryHash(query_);
				string temp(&fileContent[0], &fileContent[0u + query_.size()]);
				StringHash currentHash(temp);

				// Mnoznik		Gdy przenioslem te linijki z metody add poza nia, uzyskalem przyspieszenie z 250000ms do 300ms (w moim tescie)

					unsigned long tempBaseMultiplayer = 0ul;
					if (query_.size() > 1u) tempBaseMultiplayer = StringHash::BASE % StringHash::PRIME;
					for (unsigned int i = 2u; i < query_.size(); ++i) {
						tempBaseMultiplayer = (tempBaseMultiplayer * StringHash::BASE) % StringHash::PRIME;
					}
				//

				if (queryHash._value == currentHash._value) {

					if (temp == query_) {
						cout << '0' << ' ';
					}
				}

				for (unsigned int i = query_.size(); i < filesize; ++i) {

					// aby bylo kilka mniej obliczen:
					unsigned int iMinusSize = i - query_.size();

						// Teraz tylko mnoznik jest przekazywany a nie obliczany za kazdym razem
					currentHash.add(fileContent[iMinusSize], fileContent[i], tempBaseMultiplayer);

					if (queryHash._value == currentHash._value) {

						string temp(&fileContent[iMinusSize + 1u], &fileContent[i+1u]);
						if (temp == query_) {
							cout << (iMinusSize + 1u) << ' ';
						}
						
					}

				}
			//


			// Czyszczenie

				delete[] fileContent;
			//
		}
		catch (const string& err) {

			std::clog << err;
		}
	}

	unsigned short StringFinder::StringHash::returnHash(const string& in_) const {

		unsigned long val = 0ul;

		unsigned int size = in_.size();
		for (unsigned int i = 0u; i < size; ++i) {

			val = ((val * BASE) + (in_[i] + 128ul)) % PRIME;	// +128, aby zmapowac char na unsigned char (aby nie bylo ujemnych wartosci ktore psuja algorytm)
		}
		return (unsigned short)val;
	}

	StringFinder::StringHash::StringHash(const string& in_)
		:
			_value(returnHash(in_))
		,	_size(in_.size())
	{}

	StringFinder::StringHash::~StringHash() {}

	void StringFinder::StringHash::add(const char& old_, const char& new_, const unsigned long& tempBaseMultiplayer_) {

		unsigned long temp = old_ + 128ul;

		if (_size > 1u) {
			temp = (temp * tempBaseMultiplayer_) % PRIME;
		}
		_value = unsigned short((((_value + PRIME - temp) * BASE) + (new_ + 128ul)) % PRIME);
	}
}

int main() {

	auto start = std::chrono::steady_clock::now();	// do mierzenia czasu

    unsigned int cycles = 0u;
    std::cin >> cycles;
	cin.get();	// pobranie znaku nowej linii

	KrawczykS::StringFinder finder;

    for (unsigned int i = 0u; i < cycles; i++) {

        string filename = "";
        string query = "";

        getline(cin, filename);
        getline(cin, query);

		finder.find(filename, query);
		cout.seekp(-1, std::ios_base::cur);
		cout << std::endl;
    }

	auto end = std::chrono::steady_clock::now(); // do mierzenia czasu
	cout << "\n\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n"; // do mierzenia czasu
    return 0;
}