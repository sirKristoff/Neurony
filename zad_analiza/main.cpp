#include <iostream>

#include "../library/getopt/getopt.h"
#include "../library/net/definitions.h"
#include "../library/net/funs.h"
#include "../library/net/net.h"

using namespace std;


//******************************************************************************
/**
 * zad_analiza  OPTIONS
 *
 * OPTIONS:
 *  --network=NET           plik ze struktura sieci
 *  --functions=FUNS        plik z funkcjami aktywacji
 *  -n VALUE,  --ni=VALUE   krok nauki bedzie wynosic VALUE  (domyslnie '0.1')
 *  -m VALUE,  --mi=VALUE   wartosc momentu bedzie wynosic VALUE  (domyslnie '0.1')
 *  -e NUM,  --epoch=NUM    wykonaj NUM epok w fazie uczenia  (domyslnie '1000')
 *  -b,  --bias             siec z biasem  (domyslnie ustawiony)
 *  --lock-bias             siec bez biasu  (opcjonalnie)
 *  -o OUT,  --output=OUT   etykieta dla plikow wyjsciowych  (domyslnie 'output')
 *
 *
 * Format NET:
 *  <ILOSC_WARSTW_UKRYTYCH>   // np 'n' warstw
 *  <ILOSC_WEJSC_SIECI>
 *  <ILOSC_NEURONOW_PIERWSZEJ_WARSTWY_UKRYTEJ>
 *  .
 *  .
 *  <ILOSC_NEURONOW_OSTATNIEJ_WARSTWY>   // n-ta warstwa
 *  EOF
 *
 *
 * Format FUNS:
 *  <FUN_1>
 *  .
 *  .
 *  <FUN_N>
 *  EOF
 *  // <FUN_X> nalezy do zbioru: {ident, uni_sigm, bi_sigm}
 *  // ident    - Funkcja identycznosciowa
 *  // uni_sigm - Sigmoida unipolarna
 *  // bi_sigm  - Sigmoida bipolarna
 *
 *
 * Usage:
 * $ zad_analiza  --networt='net.txt'  --functions='funs.txt'  \
 *                --ni='0.1'  -m 0.125  --epoch=2000  --bias  --output="out_file"
 */
int main( int argc, char *argv[] )
{
	const GetOpt args( argc, argv );



	return 0;
}

