//
//      Wyszukiwanie MST i sciezek
//
//  Autor: Szymon Krawczyk
//
//  Data utworzenia: 10.04.2020
//
//  Modyfikacje:
//
//      16.04.2020 by Szymon Krawczyk:
//          Zmiana poczatkowego sposobu przechowywania krawedzi:
//          z listy krawedzi na macierz sasiedztwa w celu poprawy wydajnosci czasowej kosztem pamieci
//          (MST jest nadal w postaci listy krawedzi, poniewaz maksymalny rozmiar to 99 krawedzi, wiec malo)
//
//          Poprawienie kilku warningow
//



#include <iostream>
#include <vector>
#include <cstdlib>  // random
//#include <ctime>    // random
#include <cmath>    // ceil()

using std::cin;
using std::cout;
using std::vector;

namespace KrawczykS {

    class Graph {
    private:

        // Prywatne klasy i struktury wewnetrzne 

            struct Node {

                unsigned short _name = 0u;
                bool _processed = false;
            };

            class Path {
            public:         // Pola publiczne dla ulatwienia programowania;
                            // Same klasy sa i tak widoczne tylko w Graph

                Node* _start;
                Node* _end;
                long _cost;

                bool _visited;

                Path();
                Path(const Path&) = delete;
                ~Path();
                Path& operator=(const Path&) = delete;

                Node* returnUnprocessedNode();

            };

            class PriorityQueue {
            public:

                vector<Path*>_container;

                PriorityQueue();
                PriorityQueue(const PriorityQueue&) = delete;
                ~PriorityQueue();
                PriorityQueue& operator=(const PriorityQueue&) = delete;

                void add(Path*);
                Path* returnFirstUnprocessedPath();

            };
        //

        unsigned short _nodeVal;
        Node* _nodeContainer;
        Path** _pathContainer;

        vector<Path*>__processedPaths;  // Pojemnik na MST

    protected:

        void changePaths(bool = false);

        void findPath(Node*, Node*, vector<Path*>*, bool*);

    public:

        // Prosty interface dla uzytkownika

            Graph(unsigned short);
            Graph(const Graph&) = delete;
            ~Graph();

            Graph& operator=(const Graph&) = delete;

            void addPath(unsigned short, unsigned short, long);
            void createMST();
            unsigned int findPath(unsigned short, unsigned short, unsigned long);
        //
    };

    Graph::Path::Path()
        :
            _start  (nullptr)
        ,   _end    (nullptr)
        ,   _cost   (0l)
        ,   _visited(false)
    {}

    Graph::Path::~Path() {}

    Graph::Node* Graph::Path::returnUnprocessedNode() {

        if (!_start->_processed) {

            return _start;
        }
        if (!_end->_processed) {

            return _end;
        }
        return nullptr;
    }

    Graph::PriorityQueue::PriorityQueue()
        :
            _container()
    {}

    Graph::PriorityQueue::~PriorityQueue() {}

    void Graph::PriorityQueue::add(Path* new_) {

        for (unsigned int i = 0u; i < _container.size(); ++i) {      // Unikanie duplikowania sciezek

            if (_container[i] == new_) {

                return;
            }
        }

        for (unsigned int i = 0u; i < _container.size(); ++i) {
            if (new_->_cost <= _container[i]->_cost) {

                //new na pozycje i
                _container.insert(_container.begin() + i, new_);    // Umieszczenie na odpowiedniej pozycji
                return;
            }
        }

        _container.push_back(new_);
    }

    Graph::Path* Graph::PriorityQueue::returnFirstUnprocessedPath() {

        for (unsigned int i = 0u; i < _container.size(); ++i) {

            if (!_container[i]->_start->_processed || !_container[i]->_end->_processed) {

                return _container[i];
            }
        }
        return nullptr;
    }

    Graph::Graph(unsigned short nV)
        :
            _nodeVal(nV)
        ,   _nodeContainer(nullptr)
        ,   _pathContainer(nullptr)
        ,   __processedPaths()
    {
        _nodeContainer = new Node[_nodeVal];
        _pathContainer = new Path*[_nodeVal];

        for (unsigned short i = 0u; i < _nodeVal; i++) {

            _nodeContainer[i]._name = i + 1u;
        }

        for (unsigned short i = 0u; i < _nodeVal; i++) {

            _pathContainer[i] = new Path[_nodeVal];

            for (unsigned short j = 0u; j < _nodeVal; j++) {

                _pathContainer[i][j]._cost = 0l;
            }
        }
    }

    Graph::~Graph() {

        delete[] _nodeContainer;

        for (unsigned short i = 0u; i < _nodeVal; i++) {

            delete[] _pathContainer[i];
        }
        delete[] _pathContainer;
    }

    void Graph::changePaths(bool mode) {

        if (!mode) {

            for (unsigned short i = 0u; i < _nodeVal; i++) {
                for (unsigned short j = 0u; j < _nodeVal; j++) {

                    _pathContainer[i][j]._cost *= -1l;
                }
            }
        }
        else {
            for (unsigned int i = 0u; i < __processedPaths.size(); i++) {

                __processedPaths[i]->_cost *= -1l;          // Tylko MST
            }
        }
    }

    void Graph::addPath(unsigned short s_, unsigned short e_, long v_) {

        _pathContainer[s_ - 1u][e_ - 1u]._start = &_nodeContainer[s_ - 1u];
        _pathContainer[s_ - 1u][e_ - 1u]._end   = &_nodeContainer[e_ - 1u];
        _pathContainer[s_ - 1u][e_ - 1u]._cost  = v_;
    }

    void Graph::createMST() {

        changePaths();  // *= -1, chwilowa zamiana wartosci na potrzeby algorytmu

        PriorityQueue queue;



        //srand(time(NULL));    // W wyniku tego dzialania jest warning konwersji typow, wiec niestety zakomentowane :(

        unsigned int randomStart = (rand() % _nodeVal); // Losowy wierzcholek

        Node* currentNode = &_nodeContainer[randomStart];

        while (true) {  // Warunek wyjscia ponizej: jezeli jest krawedz laczaca z nieprzetworzonym wierzcholkiem

            // Przetwarzanie wierzcholka:
            //  Wczytaj wszystkie krawedzie go laczace
            //  Zmien pole _processed na true

            for (unsigned int i = 0u; i < _nodeVal; i++) {

                if (_pathContainer[i][currentNode->_name - 1u]._cost != 0l) {   // Bierz pod uwage tylko krawedzie istniejace

                    queue.add(&_pathContainer[i][currentNode->_name - 1u]);

                } else if (_pathContainer[currentNode->_name - 1u][i]._cost != 0l) {

                    queue.add(&_pathContainer[currentNode->_name - 1u][i]);
                }
            }
            currentNode->_processed = true;


            // Jesli jest krawedz do wierzcholka nieprzetworzonego, 
            //  dodaj ja do MDR i przetworz ten wierzcholek (w kolejnym cyklu petli)
            // Jesli nie, koniec!

            Path* tempP = queue.returnFirstUnprocessedPath();
            if (tempP != nullptr) { 
                __processedPaths.push_back(tempP);
                currentNode = tempP->returnUnprocessedNode();
            }
            else {
                break;
            }
        }

        changePaths(true); // *= -1 czyli powrot do rzeczywistych wartosci
                           //   Poprzez parametr true zmieniamy tylko krawedzie nalezace do MST

    }

    unsigned int Graph::findPath(unsigned short s_, unsigned short e_, unsigned long t_) {

        Node* start = &_nodeContainer[s_ - 1u];
        Node* end   = &_nodeContainer[e_ - 1u];

        unsigned int tempSize = __processedPaths.size();
        for (unsigned int i = 0u; i < tempSize; i++) {       // Zajmujemy sie tylko sciezkami nalezacymi do MST
            __processedPaths[i]->_visited = false;
        }

        vector<Path*> path;
        
        bool cond = false;  // Pomocniczy bool
        findPath(start, end, &path, &cond);


        // Szukanie 'waskiego gardla'
        long min = path[0]->_cost;
        for (unsigned int i = 0u; i < path.size(); i++) {
            if (path[i]->_cost < min) {
                min = path[i]->_cost;
            }
        }

        min -= 1l;          // min-1 poniewaz kierowca
        cout << (long)ceil((double)t_/min);

        return 0;
    }

    // Pomocnicza:
    void Graph::findPath(Node* current_, Node* finish_, vector<Path*>* path, bool* condition) {

        // Dziala to tak:
        //  Algorytm wykonuje operacje do momentu znalezienia konca
        //  Dla obecnego wierzcholka wykonuje:
        //      Dla kazdej nieodwiedzonej krawedzi z niego:
        //          odwiedzona = true
        //          Zapisuje wielkosc obecnej poprawnej do tej pory drogi (bez obecnej krawedzi)
        //          wywoluje algorytm ponownie dla drugiego wierzcholka 
        //              (jezeli nie jest on koncem poszukiwan; wtedy koniec)
        //          Jezeli algorytm nie znalazl sciezki do celu, 
        //              usuwa dodane przez siebie krawedzie do drogi tak, aby miala stan sprzed przetworzenia

        if (!(*condition)) {
            unsigned int size = __processedPaths.size(); // w celu przyspieszenia

            for (unsigned int i = 0u; i < size; i++) {

                if (!__processedPaths[i]->_visited && (__processedPaths[i]->_start == current_ || __processedPaths[i]->_end == current_)) {

                    __processedPaths[i]->_visited = true;

                    unsigned int pathSize = path->size();   // Kazda sciezka musi przechowywac ilosc sciezek
                                                            //  ktore byly w drodze zanim ja przetwarzalismy 
                                                            //   (zeby usunac do tej ilosci jezeli dalsza droga to slepa uliczka)


                    if (__processedPaths[i]->_start == finish_ || __processedPaths[i]->_end == finish_) {   // koniec algorytmu

                        path->push_back(__processedPaths[i]);
                        *condition = true;                  // zmiana na true spowoduje niewykonanie 
                                                            //  innych mozliwie pozostalych polecen

                        return;                             // koniec
                    }

                    if (!(*condition)) {                        // Jezeli jeszcze nie ma konca

                        path->push_back(__processedPaths[i]);   // Dodaj kolejna sciezke
                    }

                    // I wywolaj ta metode dla kolejnego wierzcholka 
                    //  (Ponizsze if else jest dlatego, ze nie wiemy czy 
                    //   teraz przetworzony byl pierwszy czy drugi wierzcholek sciezki) 

                    if (__processedPaths[i]->_end == current_) {

                        findPath(__processedPaths[i]->_start, finish_, path, condition);
                    }
                    else {

                        findPath(__processedPaths[i]->_end, finish_, path, condition);
                    }



                    // Jesli jestesmy tutaj, to obecna droga to slepa uliczka - trzeba usunac nadmiar 
                    //  korzystajac ze wczesniej ustalonej pathSize

                    if (!(*condition)) {
                        path->erase(path->begin() + pathSize, path->end());
                    }
                }
            }
        }
    }
}

int main() {

    unsigned short m = 0u;
    unsigned int d = 0u;

    unsigned short c1 = 0u, c2 = 0u;
    long p = 0l;

    unsigned short s = 0u, e = 0u;
    unsigned long t = 0ul;

    cin >> m;
    if (m == 0u) return 0;
    cin >> d;
    if (d == 0u) return 0;

    KrawczykS::Graph graph(m);

    for (unsigned int i = 0u; i < d; i++) {

        cin >> c1 >> c2 >> p;

        // Wczytujemy drogi
        graph.addPath(c1, c2, p);
    }

    // Formujemy mst
    graph.createMST();

    cin >> s >> e;
    while (s && e) {
        cin >> t;

        // Konkretne przypadki wycieczek
        graph.findPath(s, e, t);
        cout << '\n';

        cin >> s >> e;
    }

    return 0;
}