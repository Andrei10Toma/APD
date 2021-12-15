# *TEMA 2 APD*
## Toma Andrei 331CB
### *Detalii de implementare generale*

Ca o privire de ansamblu, pentru paralelizarea problemei am folosit replicated workers pentru cele 2 taskuri: Map si Reduce.
Am creat un `ExecutorService` care va avea numarul de workeri primiti la intrare. Pe langa, `ExecutorService` am folosit si partea
de `Future<T>`. Intai workerii vor face taskurile de tip **Map** pe fisierele date la intrare, iar dupa aceea vor fi chemati din nou
dupa ce taskurile de tip **Map** s-au terminat pentru a realiza taskurile **Reduce**. Dupa ce s-au terminat si toate task-urile **Reduce**
se afiseaza in fisierul de iesire rezultatele obtinute sortate dupa rang.

### *Detalii de implementare **Map***

Clasa `Map` va implementa `Callable<MapResult>` si functia call va intoarce un obiect de tip `MapResult` care contine o mapa
de la lungimea stringului la numarul de aparitii al stringurilor de lungimea respectiva, numele fisierului si lista care contine
cuvintele de lungime maximala. Mai departe, pentru a ajunge la offset-ul bun de incput si de sfarsit am ales sa ma folosesc de
`RandomAccessFile`. Verificarea daca offset-ul de inceput se afla in mijlocul unui cuvant o fac verificand daca byte-ul aflat la
pozitia precedenta este orice alt caracter, dar nu un delimitator de cuvinte. In cazul in care se afla in mijlocul cuvantului,
deplasez offset-ul de inceput in fata pana cand intalnesc un delimitator de cuvinte. Analog fac si pentru offset-ul de la final,
diferenta fiind ca la verificarea daca se afla in mijlocul cuvantului verific cu byte-ul urmator, nu cu cel anterior. Dupa aceste
prelucrari, avem offset-urile corecte atat cel de inceput, cat si cel de final, asa ca acum fiecare **worker** va putea sa faca split
pentru partitia lui. Mai departe, dupa ce stringurile au fost splituite, workerul isi face treaba: populeaza mapa mentionata mai sus
din `MapResult` si populeaza lista cu cuvinte de lungime maximala. La final, se va intoarce rezultatul si thread-ul main va retine o
lista de `Future<MapResult>` pe care vom apela metoda `get()` cand vom avea nevoie de ele pentru a ne asigura ca worker-ul
si-a terminat treaba.

### *Detalii de implementare **Reduce***

Acum ca workerii si-au terminat task-urile pentru **Map**, thread-ul main se va pregati pentru a porni task-urile de **Reduce**. Din lista
de `Future<MapResult>` sunt luate rezultatele prin intermediul metodei `get()` si vom crea o mapa pentru **Reduce** de la 
numele fisierului la o lista de rezultate de tip `MapResult`. Dupa ce am obtinut toate rezultatele de la **Map** si am creat mapa
pentru **Reduce** o sa pornim taskurile de tipul respectiv.<br>
Clasa `Reduce` va implementa `Callable<ReduceResult>` si functia call va intoarce un obiect de tip `ReduceResult` care 
contine numele fisierului, lungimea maxima a cuvantului din fisier, numarul de cuvinte de lungime maxima si rangul fisierului.
Mai departe, din rezultatele de la **Map** calculam numarul de cuvinte si lungimea maximala din tot documentul adunand valorile
din mapa aflata in obiectele de tip `MapResult`. Dupa calculul lungimii maximale, parcurgem din nou mapa din `MapResult` si
aflam numarul de cuvinte de lungime maximala prin adunarea valorilor aferente cheii egale cu lungimea maximala respectiva.
Nu in ultimul rand, vom calcula rangul fisierului folosind formula data in enunt. Dupa acest pas, se poate returna obiectul de tip
`ReduceResult` care va contine numele fisierului, lungimea maxima, numarul de cuvinte de lungime maxima si rangul fisierului.
Similar cu **Map** salvam intr-o lista de `Future<ReduceResult>` pe ale caror elemente, ulterior vom apela `get()` pentru a siguri
ca worker-ul a terminat treaba si ne-a trimis rezultatul dorit. Dupa ce toti workerii au trimis rezultatele de la **Reduce** putem sa
facem sortarea dupa rang si sa afisam in fisierul de iesire.

> PS: Read me in markdown please!