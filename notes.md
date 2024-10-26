### Algortym zachłanny

---

Szuka miasta, które jest najbliżej od wybranego w poprzednim kroku.

### Notatki ogólne

---

**Parametry algorytmu** :

- *Tour* : rozmiar turnieju
- *Pm* : prawdopodobieństwo mutacji
- *Px* : prawdopodobieństwo krzyżowania
- *gen* : liczba pokoleń
- *pop_size* : rozmiar populacji

---

**Do zrobienia**

- [ ] Inicjalizacja populacji [1/2] (~~Losowa~~, Zachłanna)
- [X] Operatory krzyżowania [2/2] (~~Ordered Crossover~~, **[TO BE FIXED]** Partially Mixed Crossover)
- [X] Mutacja [2/2] (~~Swap~~, ~~Inwersja~~)
- [X] Selekcja [2/2] (~~Turniej~~, Ruletka)
- [ ] Klasa "wytwarzająca" algorytm na podstawie parametrów wejściowych
- [ ] Utworzenie struktury pliku z ustawieniami [?]
- [ ] Rozbicie PopulationManager na mniejsze klasy (np. opearatory selekcji -> dziedziczą z jednego interfejsu itd.)
- [ ] Wczytywanie parametrów programu na podstawie pliku JSON
- [ ] Automatyczne wielokrotne uruchamianie programu (najlepiej z różnymi parametrami) 
