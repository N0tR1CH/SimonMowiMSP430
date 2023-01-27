/* Dyrektywy include */
#include<msp430x14x.h>	        // Główna biblioteka mikrokontrolera msp430
#include "lcd.h"		// Biblioteka obsługująca ekran ciekłokrystaliczny
#include "portyLcd.h"	        // Biblioteka obsługąjąca porty wyświetlacza
#include <stdio.h>		// Biblioteka wejścia-wyjścia języka C
#include <stdlib.h>		// Biblioteka standardowa języka C

/* Dyrektywy define */
#define PRZYCISK0 (P4IN & BIT4) // Definicja przycisku nr. 1
#define PRZYCISK1 (P4IN & BIT5) // Definicja przycisku nr. 2
#define PRZYCISK2 (P4IN & BIT6) // Definicja przycisku nr. 3
#define PRZYCISK3 (P4IN & BIT7) // Definicja przycisku nr. 4

/* Tablice globalne */
int wyniki[4] = {0, 0, 0, 0};   // Tablica zawierajaca najwyzsze wyniki
int sekwencjaProgramu[50];      // Tablica zawierajaca sekwencje programu
int sekwencjaUzytkownika[50];   // Tablica zawierajaca sekwencje uzytkownika

/* Tablica zawierająca znaki unikalne. (Kwadraty) */
unsigned char uniq_chars[3][8] = {
        {31, 31, 31, 31, 31, 31, 31, 31},
        {31, 31, 31, 31, 31, 31, 31, 31},
        {31, 31, 31, 31, 31, 31, 31, 31}
};

/* Prototypy funkcji */
void inicjalizacjaPrzerwan(void);       // Funkcja inicjalizujaca przerwania potrzebne do generatora liczb pseudolosowych
void pokazWynik(int);                   // Funkcja pokazujaca wynik danego gracza (przyjmuje parametr w postaci numeru gracza)
void opoznienie(void);                  // Funkcja wprowadzajaca opoznienie potrzebne do unikniecia kolizji r�norakich funkcjonalno�ci np. przyciskow
void menuStartowe(void);                // Funkcja uruchamiającca menu startowe programu
void rozgrywka(void);                   // Funkcja rozpoczynajaca rozgrywke
void sekwencja(int);                    // Funkcja generujaca sekwencje i przepytujaca gracza
int generujLosowaLiczbe(void);          // Funkcja generujaca pseudolosowa liczbe od 1 do 4
void narysujKwadrat(int);               // Funkcja rysujaca kwadrat na wyswietlaczu
void inicjalizacjaPrzyciskow(void);     // Inicjalizacja przyciskow
void pokazWyniki(void);	                // Funkcja obslugujaca menu wyboru wyniku
void wyslanieKwadratowDoPamieci(void);  // Funkcja wysylajaca kwadraty do pamieci

/* Zmienne globalne */
int licznik = 0;        // Licznik czasu jaki upłynął od startu urządzenia

/* Funkcja główna */
void main(void) {
        inicjalizacjaPrzerwan();        // inicjalizacja przerwan
        inicjalizacjaPrzyciskow();      // inicjalizacja przycisko
        wyslanieKwadratowDoPamieci();

        WDTCTL = WDTPW + WDTHOLD;       // zatrzymanie WDT

        InitPortsLcd();                 // inicjalizacja port�w
        InitLCD();                      // inicjalizacja LCD
        clearDisplay();                 // czyszczenie LCD
        SEND_CMD(CG_RAM_ADDR);          // mozliwosc wprowadzenia znakow specjalnych

        // Start programu
        menuStartowe();
}

void inicjalizacjaPrzerwan(void) {
        /* Basic Clock Module ustawiamy na ACLK(zegar 8 MHz ) i dzielimy częstotliwość przez 2 (4 MHz) */
        BCSCTL1 |= XTS; // ACLK = LFXT1 = HF XTAL 8MHz

        do {
                IFG1 &= ~OFIFG;                                 // Czyszczenie flgi OSCFault
                for (unsigned int i = 0xFF; i > 0; i--);        // odczekanie
        }
        while ((IFG1 & OFIFG) == OFIFG);                        // dopóki OSCFault jest ciągle ustawiona

        BCSCTL1 |= DIVA_1;                                      // ACLK=8 MHz/2=4 MHz
        BCSCTL2 |= SELM0 | SELM1;                               // MCLK= LFTX1 =ACLK

        /* Timer_A  ustawiamy na 500 kHz
        a przerwanie generujemy co 100 ms */
        TACTL = TASSEL_1 + MC_1 + ID_3;                         // Wybieram ACLK, ACLK/8=500kHz,tryb Up
        CCTL0 = CCIE;                                           // włączenie przerwań od CCR0
        CCR0 = 50000;                                           // podzielnik 50000: przerwanie co 100 ms

        _EINT();                                                // włączenie przerwań
}

/* Inicjalizacja przycisków */
void inicjalizacjaPrzyciskow() {
        P4DIR &= ~BIT4;         // Stan wysoki bitu4 rejestru P4DIR
        P4DIR &= ~BIT5;         // Stan wysoki bitu5 rejestru P4DIR
        P4DIR &= ~BIT6;         // Stan wysoki bitu6 rejestru P4DIR
        P4DIR &= ~BIT7;         // Stan wysoki bitu7 rejestru P4DIR
}

void menuStartowe(void) {
        clearDisplay();

        /* Pierwsza linia
           - BTN1 -> GRA */
        SEND_CMD(DD_RAM_ADDR);
        SEND_CHAR('B');
        SEND_CHAR('T');
        SEND_CHAR('N');
        SEND_CHAR('1');
        SEND_CHAR('-');
        SEND_CHAR('>');
        SEND_CHAR('G');
        SEND_CHAR('R');
        SEND_CHAR('A');

        /* Druga linijka
           - BTN2 -> WYNIKI */
        SEND_CMD(DD_RAM_ADDR2);
        SEND_CHAR('B');
        SEND_CHAR('T');
        SEND_CHAR('N');
        SEND_CHAR('2');
        SEND_CHAR('-');
        SEND_CHAR('>');
        SEND_CHAR('W');
        SEND_CHAR('Y');
        SEND_CHAR('N');
        SEND_CHAR('I');
        SEND_CHAR('K');
        SEND_CHAR('I');

        while (1) {
                if (!PRZYCISK0) {
                        /* Rozpoczecie rozgrywki */
                        opoznienie();   // Wprowadzenie opoznienia zapobiegajacego kolizjom
                        rozgrywka();    // Przejscie do rozgrywki
                } else if (!PRZYCISK1) {
                        /* Pokazanie najlepszych wynikow graczy */
                        opoznienie();   // Wprowadzenie opoznienia zapobiegajacego kolizjom
                        pokazWyniki();  // Przejscie do menu z najlepszymi wynikami graczy
                }
        }
}

int wybierzUzytkownika() {
        /* Pierwsza linia
           - PLAYER1 PLAYER 2 */
        SEND_CMD(DD_RAM_ADDR);
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('1');
        SEND_CHAR(' ');
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('2');
        /* Druga linia
           - PLAYER3 PLAYER 4 */
        SEND_CMD(DD_RAM_ADDR2);
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('3');
        SEND_CHAR(' ');
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('4');

        while (1) {
                /* Przycisk zwraca liczbe calkowita
                odpowiadajaca odpowiedniumu graczu */
                if (!PRZYCISK0) {
                        opoznienie();
                        return 0;
                } else if (!PRZYCISK1) {
                        opoznienie();
                        return 1;
                } else if (!PRZYCISK2) {
                        opoznienie();
                        return 2;
                } else if (!PRZYCISK3) {
                        opoznienie();
                        return 3;
                }
        }
}

void rozgrywka(void) {
        /* Na sam start rozgrywki wybierany jest gracz
        poprzez funkcje wybierzUzytkownika uzytkownik
        przenoszony jest do menu z wyborem a po jego
        dokonaniu do zmiennej typu Integer przesyłana
        jest liczba calkowita odpowiednia dla danego
        uzytkownika. Co pozwoli na zapamietanie wyniku do
        jego pozycji w tabeli najlepszego wyniku. */
        int uzytkownik = wybierzUzytkownika();

        clearDisplay();         // Czyszczenie ekranu
        /* Pierwsza linia
           - BTN1->START */
        SEND_CMD(DD_RAM_ADDR);
        SEND_CHAR('B');
        SEND_CHAR('T');
        SEND_CHAR('N');
        SEND_CHAR('1');
        SEND_CHAR('-');
        SEND_CHAR('>');
        SEND_CHAR('S');
        SEND_CHAR('T');
        SEND_CHAR('A');
        SEND_CHAR('R');
        SEND_CHAR('T');
        /* Pierwsza linia
           - BTN4->POWROT */
        SEND_CMD(DD_RAM_ADDR2);
        SEND_CHAR('B');
        SEND_CHAR('T');
        SEND_CHAR('N');
        SEND_CHAR('4');
        SEND_CHAR('-');
        SEND_CHAR('>');
        SEND_CHAR('P');
        SEND_CHAR('O');
        SEND_CHAR('W');
        SEND_CHAR('R');
        SEND_CHAR('O');
        SEND_CHAR('T');

        while (1) {
                if (!PRZYCISK3) {
                        /* Przycisk 4
                        przenosi do menu
                        startowego gry */
                        opoznienie();
                        menuStartowe();
                } else if (!PRZYCISK0) {
                        /* Przycisk 0 przenosi do
                        faktycznej rozgrywki poprzez
                        funkcje sekwencja pobierajca
                        danego uzytkownika okreslanego
                        wartoscia zmiennej uzytkownik */
                        opoznienie();
                        sekwencja(uzytkownik);
                }
        }
}

/* Faktyczna rozgrywka:
   - Generuje sekwencje kwadratów
   - Prosi gracza o jej powtórzenie
   - Udane powtórzenie dodaje punkt
     do zmiennej wynik
   - Porazka pokazuje wynik i zapisuje
     go w najwiekszych wynikach, lecz tylko
     w przypadku gdy ten wynik jest większy
     od dotychczasowego zapisanego w tablicy */
void sekwencja(int gracz) {
        int wynik = 0;          // Przechowuje wynik gracza podczas rozgrywki
        int iteracje = 1;       // Liczba iteracji jakich dokonuje gra

        /* Nieskonczona petla rozgrywki */
        while (1) {
                sekwencjaProgramu[wynik] = generujLosowaLiczbe();       // Zapamietanie sekwencji

                /* Pętla for generuje sekwencje zapisując ją do tablicy
                z sekwencja programu i ukazuje wizualnie kwadraty tak aby
                uzytkowik mial okazje ja zapamietac */
                for (int i = 0; i < iteracje; i++) {
                        opoznienie();
                        opoznienie();
                        clearDisplay();
                        narysujKwadrat(sekwencjaProgramu[i]);
                        opoznienie();
                        opoznienie();
                        clearDisplay();
                        opoznienie();
                        opoznienie();
                }

                clearDisplay();

                /* Pierwsza linia
                   - PODAJ */
                SEND_CMD(DD_RAM_ADDR);
                SEND_CHAR('P');
                SEND_CHAR('O');
                SEND_CHAR('D');
                SEND_CHAR('A');
                SEND_CHAR('J');

                /* Druga linia
                   - SEKWENCJE */
                SEND_CMD(DD_RAM_ADDR2);
                SEND_CHAR('S');
                SEND_CHAR('E');
                SEND_CHAR('K');
                SEND_CHAR('W');
                SEND_CHAR('E');
                SEND_CHAR('N');
                SEND_CHAR('C');
                SEND_CHAR('J');
                SEND_CHAR('E');

                opoznienie();

                for (int j = 0; j < iteracje; j++) {
                        while (1) {
                                /* Tu zapisujemy sekwencje uzytkownika na ture*/
                                if (!PRZYCISK0) {
                                        opoznienie();
                                        sekwencjaUzytkownika[j] = 1;
                                        break;
                                } else if (!PRZYCISK1) {
                                        opoznienie();
                                        sekwencjaUzytkownika[j] = 2;
                                        break;
                                } else if (!PRZYCISK2) {
                                        opoznienie();
                                        sekwencjaUzytkownika[j] = 3;
                                        break;
                                } else if (!PRZYCISK3) {
                                        opoznienie();
                                        sekwencjaUzytkownika[j] = 4;
                                        break;
                                }
                        }

                        /* Kontrola - czy sekwencja uzytkownika zgadza sie z sekwencja programu? */
                        if (sekwencjaUzytkownika[j] != sekwencjaProgramu[j]) {
                                clearDisplay();

                                SEND_CHAR('W');
                                SEND_CHAR('Y');
                                SEND_CHAR('N');
                                SEND_CHAR('I');
                                SEND_CHAR('K');
                                SEND_CHAR(':');
                                SEND_CHAR(wynik / 10 + 48);
                                SEND_CHAR(wynik % 10 + 48);

                                /* Wiekszy wynik jest zapisywany w tabeli */
                                if (wynik > wyniki[gracz]) {
                                        wyniki[gracz] = wynik;
                                }

                                for (int i = 0; i < 5; i++) {
                                        opoznienie();
                                }

                                menuStartowe();
                        }
                }

                /* Udana proba inkrementuje zmienna z wynikiem i iteracje */
                iteracje++;
                wynik++;
        }
}

void pokazWynik(int gracz) {
        /* Czyszczenie ekranu */
        clearDisplay();
        /* Pierwsza linia
           -> PLAYER(odpowiedni numer)-> (wynik) */
        SEND_CMD(DD_RAM_ADDR);
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR((gracz + 48) + 1);
        SEND_CHAR('-');
        SEND_CHAR('>');
        /* Jako ze gra ma do 50 poziomow
        pokazujemy jednie pierwsza cyfre wyniku i druga */
        SEND_CHAR((wyniki[gracz] / 10) + 48);   // Pierwsza cyfra wyniku
        SEND_CHAR((wyniki[gracz] % 10) + 48);   // Druga cyfra wyniku
        /* Druga linia
           - BTN4->POWROT */
        SEND_CMD(DD_RAM_ADDR2);
        SEND_CHAR('B');
        SEND_CHAR('T');
        SEND_CHAR('N');
        SEND_CHAR('4');
        SEND_CHAR('-');
        SEND_CHAR('>');
        SEND_CHAR('P');
        SEND_CHAR('O');
        SEND_CHAR('W');
        SEND_CHAR('R');
        SEND_CHAR('O');
        SEND_CHAR('T');
        /* Wprowadzenie opoznienia zapobiegajacego kolizjom */
        opoznienie();
        while (1) {
                if (!PRZYCISK3) {
                        menuStartowe();
                }
        }
}

void pokazWyniki(void) {
        /* Czyszczenie ekranu */
        clearDisplay();
        /* Pierwsza linia
           - PLAYER 1
           - PLAYER 2 */
        SEND_CMD(DD_RAM_ADDR);
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('1');
        SEND_CHAR(' ');
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('2');
        /* Druga linia
           - PLAYER 3
           - PLAYER 4 */
        SEND_CMD(DD_RAM_ADDR2);
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('3');
        SEND_CHAR(' ');
        SEND_CHAR('P');
        SEND_CHAR('L');
        SEND_CHAR('A');
        SEND_CHAR('Y');
        SEND_CHAR('E');
        SEND_CHAR('R');
        SEND_CHAR('4');
        while (1) {
                if (!PRZYCISK0) {
                        pokazWynik(0);
                } else if (!PRZYCISK1) {
                        pokazWynik(1);
                } else if (!PRZYCISK2) {
                        pokazWynik(2);
                } else if (!PRZYCISK3) {
                        pokazWynik(3);
                }
        }
}

void opoznienie(void) {
        for (int i = 0; i < 10; i++)
                Delayx100us(250);
}

void wyslanieKwadratowDoPamieci(void) {
        for (unsigned int i = 0; i < 3; i++)
                for (unsigned int j = 0; j < 8; j++)
                        SEND_CHAR(uniq_chars[i][j]);
}

/* Generowanie pseudolosowej liczby od 1 do 4 */
int generujLosowaLiczbe(void) {
        /* Opoznienie aby nie powtorzyc wygenerowanej liczby */
        for (int i = 0; i < 3; i++) {
                opoznienie();
        }

        /* Zwrocenie liczby pseudolosowej. */
        return ((licznik % 4) + 1);
}

void narysujKwadrat(int numer) {
        /* Czyszczenie ekranu */
        clearDisplay();

        /* Polozenie sa przez nas ustalane tak aby jak najdokladniej
        odpowiadaly przyciskom pod ekranem. */
        switch (numer) {
        case 1:
                /* Wypisywanie pierwszego kwadratu od lewej */
                SEND_CMD(DD_RAM_ADDR);
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                SEND_CMD(DD_RAM_ADDR2);
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                break;
        case 2:
                /* Wypisywanie drugiego kwadratu od lewej */
                SEND_CMD(DD_RAM_ADDR);
                for (unsigned int i = 0; i < 4; i++)
                        SEND_CHAR(' ');
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                SEND_CMD(DD_RAM_ADDR2);
                for (unsigned int i = 0; i < 4; i++)
                        SEND_CHAR(' ');
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                break;
        case 3:
                /* Wypisywanie trzeciego kwadratu od lewej */
                SEND_CMD(DD_RAM_ADDR);
                for (unsigned int i = 0; i < 8; i++)
                        SEND_CHAR(' ');
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                SEND_CMD(DD_RAM_ADDR2);
                for (unsigned int i = 0; i < 8; i++)
                        SEND_CHAR(' ');
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                break;
        case 4:
                /* Wypisanie czwartego kwadratu od lewej */
                SEND_CMD(DD_RAM_ADDR);
                for (unsigned int i = 0; i < 12; i++)
                        SEND_CHAR(' ');
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                SEND_CMD(DD_RAM_ADDR2);
                for (unsigned int i = 0; i < 12; i++)
                        SEND_CHAR(' ');
                for (unsigned int i = 0; i < 2; i++)
                        SEND_CHAR(10);
                break;
        }
}

/* Procedura obsługi przerwania od TimerA */
#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A(void) {
        ++licznik;              // Licznik czasu inkrementowany co jedną sekundę od startu obsługi przerwań
        _BIC_SR_IRQ(LPM3_bits); // wyjście z trybu LPM3
}
