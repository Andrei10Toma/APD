# *TEMA 3 APD*
## Toma Andrei 331CB
### *Detalii generale de implementare*

Tema este facuta in `C` in fisierul `tema3.c` si m-am folosit de **MPI** pentru a face calculele de
aflare al topologiei pentru fiecare nod si de a dubla vectorul de lungime data. Fiecare coordonator
(0, 1, 2) isi vor deschide fisierele din care trebuie sa citeasca cluster-ul de workeri aferent lui.
Dupa ce fiecare coordonator si-a citit clusterul de workeri programul va fi impartit in 2 parti:
aflarea tuturor nodurilor de topologie care va fi reprezentata ca un `int **topology` si dublarea
elementelor din vector de catre workeri. Variabila `topology` va fi un tablou bidimensional cu 3 randuri
si numar variabil de coloane in functie de dimensiunea cluster-ului. Pentru a reprezenta dimensiunea fiecarui
cluster al fiecarui dintre cei 3 coordonatori am folosit un array `int *topology_sizes`.

### *Detalii de implementare **Stabilirea Topologiei***

Daca procesul este coordonator atunci atunci acesta va citi cluster-ul si il va salva in varaiabila
`topology`. Mai departe, coordonatorul isi va anunta workerii din cluster ca el este coordonatorul si mai
departe coordonatorul va anunta ceilalti coordonatori in legatura cu clusterul lor: cand 0 a citit
cluster-ul din fisier, trimite celor din cluster informatia ca el este coordonator si celorlalti coordonatori
0 si 2 cluster-ul citit trimitand mai intai dimensiunea clusterului. Mai departe ceilalti coordonatori vor primi
informatia trimisa (dimensiunea cluster-ului si cluster-ul) si vor actualiza corespunzator `topology` si `topology_sizes`.
In momentul in care intreaga topologie a fost primita de catre un coordonator aceasta este printata si va fi trimisa
mai departe catre workerii coordonatorului respectiv. In acest pas workerii vor primi informatia despre cine este 
coordonatorul lor si vor mai primi si topologia de la coordonatorii lor in mod similar cu modul in care o
primesc coordonatori (mai intai lungimea si dupa topologia). Aceasta parte a luat final acum. Vom pune o bariera
pentru a astepta ca toate nodurile sa stie topologia si vom trece mai departe pentru partea cu dublarea elementelor
dintr-un array.

### *Detalii de implementare **Realizarea Calculeor***
Pentru acasta parte procesul master va fi procesul 0. Mai intai genereaza vectorul si il va trimite altauri de 
dimensiunea sa catre ceilalti coordonatori. Mai departe fiecare coordonator va trimite catre workerii sai
vectorul generat si dimensiunea sa. Acum workerii vor incepe treaba in functie de rankul pe care il au isi
vor imparti treaba in mod egal. Am folosit aceleasi formule ca la threaduri:
* `int start = (rank - 3) * (double)n / number_of_workers;`
* `int end = MIN((rank - 2) * (double)n / number_of_workers, n);`

Acestia vor trimite cele 2 segmente in care au modificat vectorul si vectorul modificat pentru ca procesul master
sa stie ce pozitii sa actualizeze. Workerii procesului mater vor trimite direct procesului 0 in timp ce ceilalti
workeri vor trimite mai intai coordonatorului lor si acestia vor trimite informatia mai departe catre procesul
master modificand tagul pentru a se diferentia mesajele primite. Dupa ce fiecare bucatica actualizata de workeri
a vectorului a ajuns la master si acesta a facut modificarile in vectorul sau il afiseaza cu valorile actualizate.


> PS: Read me in markdown please!