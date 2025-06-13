In cadrul ultimei teme la PCOM am avut de implementat un sistem de management
al unei biblioteci de filme. Sistemul include atat functionalitati pentru admin,
care poate administra conturi de useri, prin crearea, afisarea si stergerea lor,
cat si functionalitati la nivel de user, care poate manipula filme si colectii de
filme, tot prin creare, afisare si stergere. Exista 19 functii principale, care
indeplinesc functionalitatile mentionate.

## Detalii de implementare
* implementarea a durat aproximativ 20h, iar punctul de start a fost laboratorul 9, de 
unde am luat fisierele buffer.c/.h, requests.c/.h, helper.c/.h.
* in functiile care primesc input de la tastatura exista verificari ale corectitudinii
datelor: pentru username si parole se verifica sa nu existe spatii libere, iar in cazul
numerelor se verifica sa nu se introduca un alt tip decat cel cerut sau un sir de caractere.
* pentru parsarea JSON am folosit biblioteca parson.
* principalele probleme: 
  * citirea datelor de la tastatura si validarea tipurilor introduse(scanf intoarce eroare
  daca introducem un sir in loc de numar, iar in cazul tipurilor numerice pentru a valida introducerea
  unui int am luat un caracter extra ce va fi citit de la tastatura, in cazul in care acela 
  este '.' in loc de '\n' intorc mesaj de eroare).
  * parsarea JSON pe colectii a fost putin mai complicata decat cea normala.
  * add_collection a fost putin neintuitiva, am stat ceva timp pana am gasit logica 
  potrivita si am adaptat functiile add_movie_to_collection si delete_collection pentru
  a se mula pe lipsa de input cand o apelam din add_collection.
  * unele functii generau mai multe mesaje de eroare, mai ales in add_collection, de unde
  veneau mesaje si din delete_collection, si din add_movie_to_collection, ceea ce era
  confusing pentru checker si ducea la propagarea mesajului in testul urmator, respectiv
  fail, asadar a durat putin pana am gasit o solutie ce imi va afisa un numar ok de 
  mesaje de eroare.

## Fluxul programului
* se deschide conexiunea cu serverul.
* se citeste de la tastatura comanda si se intra pe ramura aferenta functiei ce o gestioneaza.
* se realizeaza fiecare operatie pana la intalnirea comenzii "exit", care opreste efectuarea.
de comenzi.
* se inchide conexiunea si datele sunt eliberate.

## Functii la nivel de admin - autentificare, management useri
### login_admin
* citeste de la tastatura datele de autentificare ale adminului, creeaza un JSON cu
ele si transmite o cerere de tip POST catre server. 
* in cazul in care primeste raspuns afirmativ si datele corespund, se va retine un
cookie de sesiune al adminului ce va fi folosit pentru validari ulterioare. 
* se fac afisari corespunzatoare mesajului primit de la server.
### add_user
* citeste de la tastatura numele si parola viitorului utilizator, creeaza un JSON cu acele
date si le transmite intr-o cerere de tip POST, impreuna cu cookie-ul de sesiune(pentru a 
confirma accesul privilegiat). 
* in cazul in care primeste raspuns afirmativ de la server(datele introduse sunt complete
si au tipul cerut) se fac afisari corespunzatoare.
### get_users
* face o cerere de tip GET prin care afiseaza toti userii existenti, care se afla in JSON-ul
din raspuns, parsat in mod corespunzator.
* cererea de tip GET contine cookie-ul de sesiune pentru a demonstra permisiunea de admin.
### delete_user
* citeste de la tastatura numele userului ce urmeaza sa fie sters, concateneaza acest nume
caii URL si trimite o cerere de tip DELETE catre server.
* afiseaza un mesaj corespunzator rezultatului.
### logout_admin
* trimite o cerere de tip GET catre server, la calea specificata.
* trimite un raspuns corespunzator succesului si elibereaza cookie-ul de sesiune al 
adminului.
## Functii la nivel de user - autentificare, acces biblioteca
### login
* citeste de la tastatura numele adminului si credentialele userului ce se doreste a fi logat.
* parseaza datele in format JSON si trimite o cerere de tip POST.
* se verifica existenta userului, in caz afirmativ se salveaza un cookie de sesiune aferent
ce va fi folosit pentru verificari ulterioare.
### get_access
* trimite o cerere de tip GET, la calea specificata, pentru a oferi utilizatorului acces la 
biblioteca de filme.
* cererea de tip get contine cookie-ul de sesiune al userului, pentru a demonstra faptul ca 
sesiunea este activa.
* salveaza un JWT pentru validari ulterioare(pentru a demonstra ca utilizatorul are acces
la biblioteca).
* afiseaza un mesaj corespunzator raspunsului de la server.
### logout
* trimite o cerere de tip GET catre server si realizeaza delogarea in functie de cookie-ul 
de sesiune.
* elibereaza cookie-ul de sesiune si afiseaza un mesaj corespunzator raspunsului.
## Functii la nivel de user - gestionarea bibliotecii de filme
### get_movies
* trimite o cerere de tip GET catre server, la calea specificata, pentru a afisa lista de 
filme din biblioteca.
* in cererea GET este inclus cookie-ul de sesiune al userului si JWT-ul.
* afiseaza un mesaj corespunzator raspunsului de la server.
### add_movie
* citeste de la tastatura titlul, anul, descrierea si rating-ul filmului, le parseaza 
in format JSON si trimite o cerere de tip POST la calea specificata.
* cererea contine cookie-ul de sesiune al userului si JWT-ul.
* afiseaza un mesaj corespunzator raspunsului de la server.
### get_movie
* afiseaza datele aferente unui film, in baza id-ului asignat la adaugarea lui.
* citeste de la tastatura id-ul si il concateneaza in calea URL.
* trimite o cerere de tip GET serverului si parseaza JSON-ul primit ca raspuns.
* afiseaza mesaje corespunzatoare raspunsului de la server.
### update_movie
* actualizeaza datele unui film, citind de la tastatura id-ul, titlul, anul, descrierea 
si rating-ul.
* parseaza datele in format JSON si trimite o cerere de tip PUT catre server.
* afiseaza un mesaj corespunzator raspunsului de la server.
### delete_movie
* sterge un film din biblioteca, pe baza id-ului primit de la tastatura.
* concateneaza id-ul caii URL si trimite o cerere de tip DELETE catre server.
* afiseaza mesaje corespunzatoare raspunsului de la server.
## Functii la nivel de user - gestionarea colectiilor
### get_collections
* afiseaza lista colectiilor, in format id-titlu, prin trimiterea unei cereri de tip GET
catre server.
* raspunsul de la server este parsat din format JSON si afisat, impreuna cu un mesaj corespunzator.
### delete_collection
* citeste de la tastatura id-ul colectiei si il concateneaza la calea URL.
* parseaza id-ul colectiei in format JSON, apoi trimite o cerere de tip POST catre server.
* afiseaza mesaje corespunzatoare raspunsului de la server.
### add_movie_to_collection
* citeste de la tastatura id-ul colectiei si id-ul filmului, concateneaza id-ul colectiei
la calea URL, parseaza datele citite in format JSON si trimite o cerere de tip POST catre
server.
* afiseaza un mesaj corespunzator raspunsului de la server.
### add_collection
* functie de adaugare a unei colectii, impreuna cu filmele ei.
* este impartita in 2 sectiuni: crearea efectiva a colectiei, prin trimiterea unei cereri de 
tip POST cu un JSON ce contine titlul colectiei, si adaugarea filmelor in colectie folosind 
add_movie_to_collection.
* cererea de tip POST pentru crearea colectiei va intoarce un id care va fi folosit pentru
adaugarea filmelor la colectie.
* am adaptat functiile add_movie_to_collection si delete_collection, prin adaugarea unor parametri
suplimentari care indica daca functia va citi de la tastatura sau va folosi id-urile primite
de la add_collection.
* in cazul in care exista vreo eroare venita din functia add_movie_to_collection, atunci
intreaga colectie va fi corupta si o voi sterge folosind delete_collection.
* se afiseaza mesaje corespunzatoare raspunsului de la server.
### delete_movie_from_collection
* citeste de la tastatura id-ul colectiei si al filmului, concateneaza id-ul colectiei si 
al filmului la calea URL, apoi trimite o cerere de tip DELETE catre server.
* afiseaza mesaje corespunzatoare raspunsului de la server.
## Functii ajutatoare
### contains_spaces
* verifica daca exista vreun spatiu in username sau parola.
### check_for_error_and_print
* deoarece nu pot identifica orice eroare poate aparea in raspunsul de la server, am 
definit aceasta functie care detecteaza in raspunsul de la server cuvantul "error" si
afiseaza corespunzator pentru toate functiile de mai sus care a fost problema concreta.
### get_session_cookie
* extrage cookie-ul de sesiune din mesajul de la server, facand pattern matching pentru 
inceputul de cookie.
* folosit pentru functiile de login.
### parse_and_print_users
* extrage lista de utilizatori din raspunsul serverului, facand pattern matching in 
raspuns pentru identificarea inceputului de JSON.
* folosit de functia get_users.
### get_jwt
* extrage JWT-ul din raspunsul de la server, facand pattern matching pe inceputul de JSON.
* folosit de functia get_access.
### parse_and_print_movies
* extrage lista de filme din raspunsul serverului, facand pattern matching pe inceputul 
de JSON.
* folosit de functia get_movies.
### parse_and_print_movie
* extrage detaliile unui singur film, facand pattern matching pe inceputul de JSON.
* folosit de functia get_movie.
### parse_and_print_collections_simple
* extrage detaliile colectiilor, adica perechea id-nume.
* folosit de functia get_collections.
### parse_and_print_collection_details
* extrage detaliile unei colectii.
* folosit de functia get_collection .
## Functii aditionale
### compute_delete_request
* construieste cererea de tip DELETE.
* similar cu GET, mici modificari in sintaxa.
### compute_put_request
* construieste cererea de tip PUT.
* similar cu POST, mici modificari in sintaxa.
### Observatii
* toate functiile de compute au primit un parametru suplimentar, anume JWT care se adauga
in mesaj pentru a fi validat de server in functiile unde este solicitat.

## Referinte
* https://gitlab.cs.pub.ro/pcom/pcom-laboratoare-public/-/tree/master/lab9
* https://github.com/kgabis/parson