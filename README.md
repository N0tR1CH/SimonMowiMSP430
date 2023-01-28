| Wydział Informatyki Politechniki Białostockiej <br/>Przedmiot: Sieci Komputerowe | Data: 12-12-2022                         |
| ------------------------------------------------------------ | ---------------------------------------- |
| Temat: Pakiet Apache2. <br/>Damian Richter                   | Prowadzący: dr. inż. Andrzej Chmielewski |

# Pakiet: apache2

## Serwer HTTP Apache

Celem projektu serwera HTTP Apache jest zbudowanie bezpiecznego, efektywnego i rozszerzalnego serwera HTTP zgodnego ze standardami otwartego oprogramowania. W konsekwencji Apache już od dawna jest numerem jeden wśród serwerów internetowych. Jego instalacja prowadzi do kompletnej instalacji Apache, wraz z plikami konfiguracyjnymi, skryptami inicjującymi i obsługującymi tenże serwer.

## Instalacja pakietu

Pakiet Apache2 można zainstalować za pomocą menedżera pakietów APT dystrybucji GNU/Linux jaką jest Debian, a także innych "distro" od niego pochodzących. Można to wykonać za pomocą tego polecenia:

```bash
sudo apt-get update && sudo apt-get install apache2 apache2-utils apache2-bin apache2-data -y
```

wszystkie kroki wykonuję w Windows Subsystem for Linux z dystrybucją Ubuntu lecz jak wcześniej wspomniałem powinny być takie same dla każdej dystrybucji wywodzącej się z forku Debiana.

![image-20230115190736765](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115190736765.png)

W celu sprawdzenia czy pakiety zostały zainstalowane poprawnie użyjmy następującego polecenia:

```bash
dpkg --get-selections | grep apache
```

![image-20230115190902946](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115190902946.png)

## Start serwera

Aby serwer działał musi zostać uruchomiony. Każda zmiana w plikach konfiguracyjnych będzie wymagała od nas zrestartowania serwera.

**Start serwera:**

```bash
sudo service apache2 start
```

**Restart serwera:**

```bash
sudo service apache2 restart
```

**Połączenie się z serwerem:**

W celu połączenia się z serwerem w wyszukiwarce URL przeglądarki internetowej należy wprowadzić adres *localhost*.

![image-20230115193342696](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115193342696.png)

Ustawmy teraz ścieżkę do korzenia drzewa dokumentów:

*Przez większość sprawozdania będą nas interesować głównie dwa foldery jakimi są /etc/apache2/ i /var/www/ chcąc ułatwić sobie przeskakiwanie między nimi korzystam z multipleksera terminala TMUX.*

Za pomocą dowolnego edytora tekstowego otwórzmy plik konfiguracyjny: /etc/apache2/sites-enabled/000-default.conf i dodajmy następującą linie ustalającą korzeń drzewa dokumentów

```php
<VirtualHost *:80>
    .
    .
    .
	DocumentRoot /var/www
    .
    .
    .
</VirtualHost>
```

![image-20230115194344328](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115194344328.png)

DocumentRoot w apache2 oznacza które miejsce będzie posiadało domyślnie wszystkie zasoby serwera. To właśnie te miejsce nazywa się korzeniem dokumentów. Dla następującej zawartości:

![image-20230115195128322](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115195128322.png)

Po wpisaniu w wyszukiwarce adresu 'localhost' otrzymamy następujący rezultat:

![image-20230115195153589](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115195153589.png)

## Aliasy do zasobu

Stwórzmy strukturę katalogów /var/www/student/oceny/kolokwium za pomocą komendy:

```bash
mkdir -p /var/www/student/oceny/kolokwium
```

Przy tworzeniu zasobów serwera apache2 miejmy na uwadzę prawa użytkowników do nich!

W folderze kolokwium tworzymy plik o nazwie index z rozszerzeniem html:

```bash
touch /var/www/student/oceny/kolokwium/index.html
```

Za pomocą dowolnego polecenia wprowadźmy do pliku jakiś tekst np:

![image-20230115195510124](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115195510124.png)

Dodajmy teraz alias /oceny do folderu kolokwium:

```php
# Alias do zasobów
Alias /oceny "/var/www/student/oceny/kolowkium"
```

![image-20230115195630938](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115195630938.png)

W następstwie zarówno 'localhost/oceny' jak i 'localhost/student/oceny/kolowkium' przenoszą nas do tego samego zasobu:

![image-20230115195801783](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115195801783.png)

## Przekierowanie na zewnętrzny serwer

Dodanie do pliku konfiguracyjnego komendy:

```php
Redirect /pb https://pb.edu.pl
```

Spowoduje, że po wpisaniu adresu 'localhost/pb' zostaniemy przeniesieni na zewnętrzny serwer wybranej strony.

![image-20230115200027613](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115200027613.png)

![image-20230115200051237](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115200051237.png)

![image-20230115200107681](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115200107681.png)

## Obsługa błędów

Serwer apache2 posiada domyślne strony informujące nas o danym błędzie po którym możemy stwierdzić czy błąd jest na poziomie klienta/serwera. Odpowiednio modyfikując plik konfiguracyjny możemy wprowadzić własne komunikaty. Chcąc uzyskać proste 'Strona nie istnieje' dodajemy do pliku następującą linijke:

```php
ErrorDocument 404 "Strona nie istnieje"
```

Z kolei chcąc odwoływać się do pliku w przypadku wykrycia błędu należy również użyć komendy ErrorDocuement ale w tym przypadku należy podać adres do zasobu w przeciwieństwie do tekstowego komunikatu. Stwórzmy plik error z rozszerzeniem html zawierający informację o braku dostępie do strony.

![image-20230115200637390](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115200637390.png)

Komenda:

```php
ErrorDocument 403 /error.html
```

![image-20230115200752987](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115200752987.png)

Przetestujmy naszą obsługę wyjątków:

![image-20230115200929612](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115200929612.png)

Zablokuje teraz dostęp dla serwera dla siebie aby sprawdzić obsługę błędu 403, mogę tego dokonać za pomocą komendy deny from all w tagu opisującym korzeń drzewa co oznacza że dostęp do korzenia będzie zablokowany dla każdego użytkownika i dla każdego apache powinien wyświetlić mój spersonalizowany komunikat.

![image-20230115201619284](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115201619284.png)

****

## Katalog na hasło

W pliku konfiguracyjnym należy dodać następujące linie przy tagach dotyczących nowego katalogu private w którym przechowywane hasła:

![image-20230115212051682](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115212051682.png)

Utwórzmy katalog i plik w którym będą przechowywane hasła.

```bash
mkdir /var/www/private
```

Stwórzmy testowego użytkownika:

```bash
htpasswd -c /var/www/private/password.txt testowy
```

Przy dodawaniu następnych użytkowników opuszczamy dopisywanie flagi -c. Plik z hashami został umieszczony w folderze private bo w zamyśle ma być on folderem niedostępnym dla użytkowników. Można również utworzyć plik password.txt jako plik .password czyli w UNIXie plik ukryty i następnie zakazać użytkownikom dostępu do plików ukrytych.

![image-20230115212336666](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115212336666.png)

## Ograniczenie dostępu na podstawie adresu IP

Ograniczenie dostępu do zasobów omawiałem już przy testowaniu wyjątku błędu 403 czyli komunikatu informującego o braku dostępu do zasobów. Aby zezwolić na dostęp adresom IP z pewnego zakresu można skorzystać z następującej komendy:

```php
<Directory "/var/www/protected">
    order deny,allow
    Allow from 127.0.0.1 10.64.104.XXX
    Deny from all
</Directory>
```

Stwórzmy również katalog protected na który będą nałożone te wymagania.

![image-20230115212636535](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115212636535.png)

order oznacza porządek w którym będą wykonywane komendy deny (zablokowanie) allow (zezwolenie). To znaczy, że wpierw dostęp zostanie zablokowany do wszystkich adresów a następnie zezwolony jedynie dla adresu hosta i adresów mających początek 10.64.104.XXX. Aby zmiany zostały wprowadzone pamiętajmy o resecie pakietu.

## Konfiguracja poprzez plik .htaccess

Zakomentujmy sekcje odwołującą się do folderu private i przenieśmy zawartość tej sekcji do nowego pliku w katalogu private jakim będzie plik ukryty ".htaccess". Dodajmy do tego pliku następujące linie:

```bash
AuthType Basic
AuthName "Wprowadz haslo"
AuthUserFile "var/www/private/password.txt"
Require user testowy
```

Po wykonaniu tych akcji po próbie odwołaniu się do zasobów powinien powitać nas prymitywny ekran logowania protokołu http, w przeglądarce Google Chrome wygląda następująco:

![image-20230115213937983](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115213937983.png)

## Przestrzenie użytkowników na serwerze WWW

Odblokujmy moduł userdir

```bash
a2enmod userdir
```

W katalogu domowym użytkownika utwórzmy folder public_html, zmieńmy jego prawa na pełne dla użytkownika, odczytywanie i uruchamianie dla grupy, innych. Następnie stwórzmy plik html zawierający jakiś tekst.

```bash
cd ~student
mkdir public_html
chmod 755 public_html
echo "Strona domowa studenta" > ~student/public_html/index.html
chmod o+x /home/student
```

![image-20230115214349662](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115214349662.png)

![image-20230115214359948](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115214359948.png)

W moim przypadku odwołując się do adresu localhost/~damian wypisywana jest zawartość pliku html znajdującego się w moim osobistym zasobie, który mogę modyfikować jedynie ja jako jego właściciel. Spróbujmy teraz za pomocą pliku htaccess skonfigurować dostęp na hasło do katalogu private w moim katalogu.

```bash
mkdir private
touch .htaccess
```

w .htaccess dopiszmy następujące linie:

```bash
AuthType Basic
AuthName "Wprowadz haslo"
AuthUserFile /home/damian/public_html/private/.htpasswd
Require user student
```

i tak jak poprzednio tworzymy jakiegoś autoryzowanego użytkownika. Podczas próby połączenia się do folderu powinien nas ponownie powitać prymitywne okno logowania protokołu HTTP.

![image-20230115221219900](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115221219900.png)

Wpiszmy dane użytkownika, który ma dostęp do katalogu:

![image-20230115221254240](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230115221254240.png)

Katalog jest pusty, bądź ma pliki ukryte ale jak widać na załączonym zrzucie ekranu mamy do niego dostęp.

## Tworzenie wirtualnych hostów opartych o adres IP i nazwe

### IP-Based

W głównym pliku konfiguracyjnym dodajmy sekcje konfiguracyjną dla naszego pierwszego i drugiego serwera wirtualnego:

```php
<VirtualHost A.B.C.D:80>
  ServerName nazwaserwera1.pl
  DocumentRoot /var/www/srv1
  ErrorLog /var/log/apache2/srv1_error.log
  CustomLog /var/log/apache2/srv1_access.log
  <Directory /var/www/srv1>
    Options Indexes FollowSymLinks
    AllowOverride All
    Order allow,deny
    Allow from all
  </Directory>
</VirtualHost>
```

Jako, że wykonuję te sprawozdanie w zaciszu domowym sprawdzę adres hosta za pomocą komendy:

```bash
lsb_release -d ; hostname -I
```

Otrzymałem następujący wynik:

```bash
Description:    Ubuntu 20.04.5 LTS
192.168.138.82
```

Oznacza to, że na ten moment posiadam jedynie jeden adres hosta prowadzący do localhosta.

192.168.138.82

![image-20230128170550313](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128170550313.png)

localhost:

![image-20230128170603660](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128170603660.png)

W związku z tym łatwo możemy wywnioskować, że to te same strony. Stwórzmy teraz nowy serwer wirtualny ip-based. W folderze:

```bash
/etc/apache2/sites-available
```

Tworzę nowy katalog o nazwie "srv1" za pomocą następującej komendy:

```bash
mkdir /etc/apache2/sites-available/srv1
```

W środku tego katalogu utwórzmy stronę główną naszego serwera "index.html" zawierającą prosty komunikat: "Witaj swiecie!".

![image-20230128171029371](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128171029371.png)

Przejdźmy teraz do katalogu apache2 z dostępnymi stronami i utwórzmy tam katalog naszej nowej strony i utwórzmy w nim plik konfiguracyjny. Po utworzeniu edytujmy go naszym ulubionym edytorem tekstowym, w moim przypadku będzie to VIM.

```bash
z /etc/apache2/sites-available/srv1
touch srv1.conf
sudo vim srv1.conf
```

Do pliku konfiguracyjnego wpisuje następującą treść:

```bash
<VirtualHost 192.168.138.83:80>
        ServerName srv1
        DocumentRoot /var/www/srv1
        ErrorDocument 404 /error.html
        ErrorDocument 403 /error.html

        <Directory /var/www/srv1>
                Options Indexes FollowSymLinks MultiViews
                AllowOverride All
                Order allow,deny
                allow from all
        </Directory>

        ErrorLog /var/log/apache2/srv1_error.log
        CustomLog permanent /var/log/apache2/srv1_access.log
</VirtualHost>
```

Na tym etapie nie mogę jeszcze się podłączyć do zdefiniowanego dla hostu adresu, gdyż mój interfejs sieciowy nie interpretuje jeszcze go.

Aby móc stworzyć kolejnego wirtualnego hosta potrzebuje aby mój interfejs miał zdefiniowany jeszcze jeden adres IPv4. Mogę do tego użyć polecenia ifconfig.

```bash
sudo ifconfig eth0:1 192.168.138.83
```

Zresetujmy serwer apache2 za pomocą polecenia:

```bash
sudo service apache2 restart
```

Spróbujmy teraz połączyć się z naszą stroną.

![image-20230128173845934](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128173845934.png)

Świetnie. Powtarzając te kroki jesteśmy w stanie stworzyć kolejne wirtualne hosty. Spróbujmy stworzyć 2.

```bash
sudo ifconfig eth0:2
```

Dzięki poleceniu *ip addr* mogę zobaczyć dostępne hosty:

![image-20230128174017812](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128174017812.png)

Tym razem zacznę od pliku konfiguracyjnego. Skoro pierwszy to był srv1 to nazwę drugi srv2.

![image-20230128174129868](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128174129868.png)

Skopiuję zawartość pliku konfiguracyjnego srv1 do srv2 zmieniając pole głównego nagłówka na odpowiedni adres: 162.168.138.84:80 a nazwy srv1 na srv2.

Utwórzmy katalog dla serwera srv2 i umieśćmy w nim plik index.html z jakąś zawartością. Ja umieszczę "Ponownie witaj swiecie! :)". Zresetujmy pakiet apache i spróbujmy połączyć się z nim.

![image-20230128174741913](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128174741913.png)

Po wykonaniu tych czynności użyjmy komend aktywujących serwer i ładujących cały pakiet apache2.

```bash
sudo a2ensite srv2
sudo service apache2 reload
```

Powinniśmy mieć dwa serwery ip-based:

- 192.168.138.83
- 192.168.138.84

Sprawdźmy to!

![image-20230128180541972](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128180541972.png)

Świetnie! Wszystko działa jak należy. Przejdźmy do serwerów name-based.

### Name-Based

Wirtualne hosty oparte o adres IP używają adresu IP połączenia w celu zdeterminowania poprawnego wirtualnego hosta do usług. W związku z tym potrzebujemy oddzielnego adresu IP dla każdego z hostów. Używając techniki name-based serwer polega na kliencie w celu zgłoszenia nazwy hosta jako części nagłówka HTTP. Używając tej techniki różne hosty mogą dzielić się tym samym adresem IP.

```
Dyrektywa globalna

NameVirtualHost 192.168.10.20:80 (lub inny adres)


Dodać sekcję identyczną jak w przypadku ip-based oraz w plku /etc/hosts dodać odwzorowanie z adresu IP na nazwę (nazwę określoną parametrem ServerName), np.

192.168.10.20 www.abc.pl

192.168.10.20 www.cba.pl
```

A więc do stworzenia dwóch serwerów name-based potrzebujemy tylko jednego adresu IP! Zajmijmy się tym. Na sam początek dodam odwzorowania adresów do pliku /etc/hosts

```bash
# na potrzeby apache
192.168.138.85 www.abc.pl
192.168.138.85 www.cba.pl
```

Stwórzmy również katalogi abc i cba w lokalizaji /var/www zawierające informacje o tym na jakiej stronie teraz przebywamy. Ostatnią rzeczą jaka nam została jest skonfigurowanie pliku konfiguracyjnego. Nazwijmy go srv3.conf i wypełnijmy w następujący sposób:

```bash
NameVirtualHost 192.168.138.85:80

<VirtualHost 192.168.138.85:80>
        ServerName www.abc.pl
        DocumentRoot /var/www/abc
        ErrorDocument 404 /error.html
        ErrorDocument 403 /error.html

        <Directory /var/www/srv1>
                Options Indexes FollowSymLinks MultiViews
                AllowOverride All
                Order allow,deny
                allow from all
        </Directory>

        ErrorLog /var/log/apache2/abc_error.log
        CustomLog permanent /var/log/apache2/abc_access.log
</VirtualHost>

<VirtualHost 192.168.138.85:80>
        ServerName www.cba.pl
        DocumentRoot /var/www/cba
        ErrorDocument 404 /error.html
        ErrorDocument 403 /error.html

        <Directory /var/www/cba>
                Options Indexes FollowSymLinks MultiViews
                AllowOverride All
                Order allow,deny
                allow from all
        </Directory>

        ErrorLog /var/log/apache2/cba_error.log
        CustomLog permanent /var/log/apache2/cba_access.log
</VirtualHost>
```

Aktywujmy serwer i ponownie załadujmy pakiet apache.

### WSL

Warto nadmienić, że korzystam z windows subsystem for linux, który ma własny adres ip przez co po skonfigurowaniu pliku /etc/hosts nie byłem przekierowywany na odpowiednie katalogi. Rozwiązaniem tego problemu było użycie tekstowej przeglądarki lynx, która z kolei te odzwierciedlenia znała (nie chciałem grzebać w pliku hostów systemu Windows, gdyż system Windows nie lubi gdy użytkownik w niego ingeruje :)).

## Dowód na działające 2 serwery ip-based i 2 działające serwery name-based

Szare pola to Multipleksera to właśnie przeglądarka lynx. Do stron za pomocą przeglądarki lynx można dostać się w następujący sposób:

```bash
lynx www.abc.pl
lynx www.cba.pl
```

![image-20230128184049112](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128184049112.png)

![image-20230128184206728](C:\Users\damia\AppData\Roaming\Typora\typora-user-images\image-20230128184206728.png)

# Wnioski

Apache jest internetowym serwerem napisanym w języku C uruchamianym jako serwis na komputerze hosta. Za jego pomocą możemy dostarczać zasoby klientom naszej sieci co sprawdza się świetnie w sieciach biurowych gdzie dzielone są różne dane. Jego elastyczność pozwala na dowolną manipulację prawami plików i katalogów. Oficjalna strona ze skompilowanymi jądrami Ubuntu również stoi na tym serwerze co pokazuje że jest to solidne rozwiązanie do udostępniania plików tysiącom użytkowników.
