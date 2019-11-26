# PEA 2 Plus – tabu search dla TSP
Rozwinięcie drugiego projektu na kurs Projektowanie efektywnych algorytmów na PWr w 2017 – implementacja algorytmu przeszukiwania lokalnego z zakazami (tabu search) dla problemu komiwojażera (TSP) dostosowana do pracy wielowątkowej oraz (dodatkowo) algorytmów z pierwszego projektu – przegląd zupełny (brute force), podział i ograniczenia (branch and bound). Program jest napisany w standardzie C++11 bez używania bibliotek specyficznych dla konkretnego systemu operacyjnego, powinno dać się go skompilować na Windowsie, chociaż sam nie próbowałem.

__Elementy programu__  
* interfejs grafu – klasa abstrakcyjna Graph
* implementacja grafu w postaci macierzy sąsiedztwa (domyślnie używana) i listy sąsiedztwa – klasy ArrayGraph i ListGraph
* niezależny od platformy system do pomiaru czasu, bazujący na std::chrono z C++11 – klasa Stopwatch
* parsery plików w formatach FULL_MATRIX i EUC_2D z TSPLIB95
* algorytmy przeglądu zupełnego (jednowątkowy)
* algorytm podziału i ograniczeń (jednowątkowy)
* algorytm tabu search (wielowątkowy, bazujący na std::thread z C++11)
* pomocnicze algorytmy na potrzeby tabu search: losowy, zachłanny oraz ich hybryda
* pętla do automatycznego wykonywania pomiarów dla określonych w stałej czasów pracy algorytmu tabu search

__Charakterystyka tabu search:__
* rozwiązanie początkowe generowane algorytmem zachłannym
* sąsiedztwo typu swap (zamiana wierzchołków miejscami)
* lista tabu przechowująca obydwa zamienione wierzchołki
* kryterium aspiracji omijające listę tabu, jeżeli zakazany ruch prowadzi do lepszego rozwiązania niż najlepsze znalezione dotychczas
* dwa rodzaje dywersyfikacji:
  * intensyfikacja przeszukiwania po znalezieniu lepszego rozwiązania od dotychczasowego (restart od nowego rozwiązania ze zmniejszoną 4x kadencją na liście tabu)
  * restart od nowego rozwiązania wygenerowanego algorytmem losowo-zachłannym (losowa część wierzchołków jest losowana, reszta wybierana zachłannie) po określonej liczbie iteracji bez poprawy – domyślnie 10 tysięcy
* kadencja na liście tabu dobierana automatycznie (liczba wierzchołków zaokrąglona w dół do najbliższej dziesiątki) lub konfigurowana ręcznie

__Parametry wywołania programu:__
* -t – liczba wątków tabu search (domyślnie 2)
* -l – użycie listowej reprezentacji grafu (zamiast macierzowej)
* -fmatrix – wczytanie pliku w formacie FULL_MATRIX przy uruchamianiu programu
* -feuc2d – wzytanie pliku w formacie EUC_2D przy ładowaniu programu

__Osiągi tabu search na moim laptopie – 2 wątki:__  
(1x Intel Core i5-2520M @ 2.5 GHz, 8 GB RAM, macOS 10.13.6)
* ftv33.atsp (optimum 1286)
  * po 30 sekundach za każdym razem optimum
* swiss42.tsp (optimum 1273)
  * po 30 sekundach zwykle optimum lub więcej o około 20 (np. 1288)  
  * po 60 sekundach za każdym razem optimum
* ftv47.atsp (optimum 1776)
  * po 5 minutach 1790, więcej o 14 od optimum – jeden test
  * po 15 minutach 1798, więcej o 22 od optimum – jeden test
* berlin52.tsp (optimum 7542)
  * po 5 minutach 7730, więcej o 188 od optimum – jeden test
  * po 10 minutach 7673, więcej o 131 od optimum – jeden test

__Osiągi tabu search na serwerze panamint.kcir.pwr.edu.pl – 64 wątki:__  
(4x AMD Opteron 6276 @ 2.3 GHz, 256 GB RAM, Debian Linux 9)
* ftv47.atsp (optimum 1776)
  * po 5 minutach czasami optimum lub blisko optimum (np. 1782)
  * po 15 minutach tak samo, ale większa szansa trafienia na optimum lub prawie dokładnie optimum (1777)
  * po 20 minutach za każdym razem optimum
* berlin52.tsp (optimum 7542)
  * po 5 minutach czasami optimum lub więcej o około 130 (np. 7679)
  * po 10 minutach prawie za każdym razem optimum
* ftv70.atsp (optimum 1950)
  * po 40 minutach 1972, więcej o 22 od optimum – jeden test  

Dane testowe pochodzą ze strony:  
http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/  
Poza ftv70 (kadencja 60), wszystkie testy zostały przeprowadzone z automatycznie ustawioną kadencją.
