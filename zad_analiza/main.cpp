#include <iostream>
#include <string>

#include "../library/getopt/getopt.h"
#include "../library/net/definitions.h"
#include "../library/net/funs.h"
#include "../library/net/net.h"

using namespace std;

//******************************************************************************

#ifdef DEBUG
 #define LOG_IO(what) \
	cerr << what
#else
 #define LOG_IO(what)
#endif

const string appName__ = "zad_analiza";

//******************************************************************************
/**
 * zad_analiza  OPTIONS
 *
 * OPTIONS:
 *  --network=NET           plik ze struktura sieci
 *  --functions=FUNS        plik z funkcjami aktywacji
 *  -n VALUE,  --ni=VALUE   krok nauki bedzie wynosic VALUE  (domyslnie '0.1')
 *  -m VALUE,  --mi=VALUE   wartosc momentu bedzie wynosic VALUE  (domyslnie '0.1')
 *  -b,  --bias             siec z biasem  (opcjonalnie)
 *  --lock-bias             siec bez biasu  (domyslnie zablokowany)
 *  -t TYPE, --type=TYPE    typ programu TYPE C {approx,class,trans}
 *  --train=TRAIN           wczytaj dane treningowe z pliku TRAIN
 *  --test=TEST             wczytaj dane testowe z pliku TEST
 *  -e NUM,  --epoch=NUM    wykonaj NUM epok w fazie uczenia  (domyslnie '1000')
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

	bool bias__= false;
	string namNetwork;
	string namFunctions;
	string type__;
	string namTrain;
	string namTest;
	size_t epoch__= 1000;
	string namOutput__= "output";

	//--- Wczytywanie ustawien wejsciowych -------------------------------------

	if( !args.fetchValue( namNetwork, "network=" ) ){
		cerr << appName__ << ": Oczekiwano `--network=NET'" << endl;
		return  (1);
	}
	if( !args.fetchValue( namFunctions, "functions=" ) ){
		cerr << appName__ << ": Oczekiwano `--functions=FUNS'" << endl;
		return  (2);
	}

	args.fetchValue( Net::sNi, "n=|ni=" );
	args.fetchValue( Net::sMi, "m=|mi=" );

	if( args.fetchValue( bias__, "lock-bias") )  bias__ = !bias__;
	args.fetchValue( bias__, "b|bias" );

	if( !args.fetchValue( type__, "t=|type=" ) ){
		cerr << appName__ << ": Oczekiwano `--type=TYPE'" << endl;
		return  (7);
	}else{
		if( type__ != "approx" && type__ != "class" && type__ != "trans" ){
			cerr << appName__ << ": Oczekiwano `--type=TYPE'" << endl
				 << "\tTYPE C {approx,class,trans}" << endl;
			return  (107);
		}
	}
	if( !args.fetchValue( namTrain, "train=" ) ){
		cerr << appName__ << ": Oczekiwano `--train=TRAIN'" << endl;
		return  (8);
	}
	if( !args.fetchValue( namTest, "test=" ) ){
		cerr << appName__ << ": Oczekiwano `--test=TEST'" << endl;
		return  (9);
	}

	args.fetchValue( epoch__, "e=|epoch=" );
	args.fetchValue( namOutput__, "o=|output=" );

#define printVal( variable )   LOG_IO( #variable << " = " << variable << endl );

	printVal(bias__);
	printVal(namNetwork);
	printVal(namFunctions);
	printVal(Net::sNi);
	printVal(Net::sMi);
	printVal(type__);
	printVal(namTrain);
	printVal(namTest);
	printVal(namOutput__);
	printVal(epoch__);
	//---  ---------------------------------------------------------------------


	return 0;
}

