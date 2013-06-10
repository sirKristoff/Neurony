#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../library/getopt/getopt.h"
#include "../library/net/definitions.h"
#include "../library/net/funs.h"
#include "../library/net/net.h"

using namespace std;

//******************************************************************************
#define  SEP_LINE \
	"--------------------------------------------------------------------------------\n"

#define ERROR(ret, fun, msg) \
	cerr << "ERROR: " << fun << ": " << msg << endl;  \
	return (ret);

#define WARNING(msg) \
	cerr << appName__ << ": " << msg << endl;

#define TEST( fun, exp_ret ) \
{ int ret; \
  if( (ret = fun) exp_ret ){  \
	cerr << "Nieprawidlowa wartosc powrotu: `"<<ret<<"'\n";  \
  return (ret);  }  \
}

#if defined(LVL1)
 #define  LEVEL 1
#elif defined(LVL2)
 #define  LEVEL 2
#elif defined(LVL3)
 #define  LEVEL 3
#elif defined(LVL4)
 #define  LEVEL 4
#else
 #define  LEVEL 0
#endif

#ifdef DEBUG
 #define LOG_IO(what) \
	cerr << what
#else
 #define LOG_IO(what)
 #define LOG_IO(level, what)
#endif

#define LOG_IO1(what) \
	LOG_IO1_( SEP_LINE << " " << what );
#define LOG_IO1_(what) \
	if( 1 <= LEVEL ) LOG_IO( what );
#define LOG_IO2(what) \
	LOG_IO2_( "   " << what );
#define LOG_IO2_(what) \
	if( 2 <= LEVEL ) LOG_IO( what );
#define LOG_IO3(what) \
	LOG_IO3_( "\t" << what );
#define LOG_IO3_(what) \
	if( 3 <= LEVEL ) LOG_IO( what );
#define LOG_IO4(what) \
	LOG_IO4_( "\t" << what );
#define LOG_IO4_(what) \
	if( 4 <= LEVEL ) LOG_IO( what );

#define printVal( variable )   LOG_IO3( #variable << " = " << variable << endl );


typedef  vector<Input>  Point;
typedef  vector<Point>  VecPoint;

//******************************************************************************

int fetchOptions( const GetOpt& args );
int load_structure( vector<Size>& vecN, vector<Fun>& vecFun, vector<Dif>& vecDif );
int load_points( const string& namfile, VecPoint& vecIn, Size inSize,
				 VecPoint& vecOut, Size outSize );



//******************************************************************************

const string appName__ = "zad_analiza";

bool bias__= false;     // flaga odblokowania biasu
string namNetwork__;    // plik ze struktura sieci
string namFunctions__;  // plik z funkcjami aktywacji dla sieci
string type__;
string namTrain__;      // plik z danymi treningowymi
string namTest__;       // plik z danymi testowymi
size_t epoch__= 1000;   // ilosc epok nauki
string namOutput__= "output";   // wzorzec dla plikow wyjsciowych

VecPoint vecTrainIn;    // zbior punktow treningowych
VecPoint vecTrainOut;
VecPoint vecTestIn;     // zbior punktow testowych
VecPoint vecTestOut;
Net* pNet;              // siec neuronowa

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
	vector<Size> vecN;
	vector<Fun> vecFun;
	vector<Dif> vecDif;

	TEST( fetchOptions( args ), != 0 );


	//--- Wczytywanie danych z plików ------------------------------------------

	TEST( load_structure( vecN, vecFun, vecDif ), != 0 );

	//--- Konstruowanie sieci --------------------------------------------------

	LOG_IO1("Konstruowanie sieci\n");
	pNet = new Net( vecN, vecFun, vecDif,
				   (bias__? Net::lbUnlock : Net::lbLock));

	LOG_IO1("Wczytywanie punktow\n");
	//--- punkty treningowe ---//
	TEST( load_points( namTrain__, vecTrainIn, pNet->sizeIn(),
					   vecTrainOut, pNet->sizeOut() ) ,  != 0 );
	//--- punkty testowe ---//
	TEST( load_points( namTest__, vecTestIn, pNet->sizeIn(),
					   vecTestOut, pNet->sizeOut() ) ,  != 0 );






	delete pNet;

	return 0;
}


//******************************************************************************

//--- Wczytywanie ustawien wejsciowych -----------------------------------------
int fetchOptions( const GetOpt& args )
{
	LOG_IO1("fetchOptions()\n");
	if( !args.fetchValue( namNetwork__, "network=" ) ){
		WARNING("Oczekiwano `--network=NET'");
		return  (1);
	}
	if( !args.fetchValue( namFunctions__, "functions=" ) ){
		WARNING("Oczekiwano `--functions=FUNS'");
		return  (2);
	}

	args.fetchValue( Net::sNi, "n=|ni=" );
	args.fetchValue( Net::sMi, "m=|mi=" );

	if( args.fetchValue( bias__, "lock-bias") )  bias__ = !bias__;
	args.fetchValue( bias__, "b|bias" );

	if( !args.fetchValue( type__, "t=|type=" ) ){
		WARNING("Oczekiwano `--type=TYPE'");
		return  (7);
	}else{
		if( type__ != "approx" && type__ != "class" && type__ != "trans" ){
			WARNING("Oczekiwano `--type=TYPE'" << endl
					<< "\tTYPE C {approx,class,trans}");
			return  (107);
		}
	}
	if( !args.fetchValue( namTrain__, "train=" ) ){
		WARNING("Oczekiwano `--train=TRAIN'");
		return  (8);
	}
	if( !args.fetchValue( namTest__, "test=" ) ){
		WARNING("Oczekiwano `--test=TEST'");
		return  (9);
	}

	args.fetchValue( epoch__, "e=|epoch=" );
	args.fetchValue( namOutput__, "o=|output=" );

	printVal(bias__);
	printVal(namNetwork__);
	printVal(namFunctions__);
	printVal(Net::sNi);
	printVal(Net::sMi);
	printVal(type__);
	printVal(namTrain__);
	printVal(namTest__);
	printVal(namOutput__);
	printVal(epoch__);

	return  (0);
}


//--- Wczytywanie strukruty sieci ----------------------------------------------
int load_structure( vector<Size>& vecN, vector<Fun>& vecFun, vector<Dif>& vecDif )
{
	LOG_IO1("load_structure()\n");
	ifstream file;
	Size number;
	Size nLayers;
	string str;

	//---  wielkosc warstw  ---//
	LOG_IO2("Parametry warstw sieci:\n");
	file.open( namNetwork__.c_str(), std::ios_base::in );
	if( ! file.good() ){
		ERROR(1, "::load_structure()",
			"Blad otwarcia pliku `" << namNetwork__ << "'");
	}

	vecN.clear();

	file >> nLayers;
	if( nLayers == 0 ){
		ERROR(2, "::load_structure()", "Nieprawidlowa ilosc warst sieci.");
	}

	vecN.reserve(nLayers);
	while( (file>>number) && nLayers ){
		if( number == 0 )  break;

		vecN.push_back(number);
		LOG_IO3( vecN.back() << endl );

		--nLayers;
	}
	if( nLayers != 0 ){
		ERROR(nLayers, "::load_structure()", "Nie wczytano pelnej info o warstwach.");
	}
	file.close();
	LOG_IO2("Wczytano.\n");


	//---  funkcje aktywacji  ---//

	LOG_IO2("Funkcje aktywacji warstw:\n");
	file.open( namFunctions__.c_str(), std::ios_base::in );
	if( ! file.good() ){
		ERROR(1, "::load_structure()",
			"Blad otwarcia pliku `" << namFunctions__ << "'");
		return  (1);
	}

	nLayers = vecN.size();
	vecFun.clear();  vecFun.reserve(nLayers);
	vecDif.clear();  vecDif.reserve(nLayers);


	--nLayers;  // warstwa kopiujaca nie ma funkcji aktywacji
	while( (file>>str) && nLayers ){
		if( str.empty() )  break;

		if( str == "ident" ){
			vecFun.push_back( ::ident );
			vecDif.push_back( ::d_ident );
		}
		else if( str == "uni_sigm" ){
			vecFun.push_back( ::uni_sigm );
			vecDif.push_back( ::d_uni_sigm );
		}
		else if( str == "bi_sigm" ){
			vecFun.push_back( ::bi_sigm );
			vecDif.push_back( ::d_bi_sigm );
		}
		else{
			WARNING("Wczytywanie funkcji aktywacji z pliku `" << namFunctions__
					<< "': Nieznany znacznik funkcji aktywacji: `"<<str<<"'.");
			vecFun.push_back( ::ident );    str = "ident";
			vecDif.push_back( ::d_ident );
		}

		LOG_IO3( str << endl );
		--nLayers;
	}
	if( nLayers != 0 ){
		ERROR(nLayers, "::load_structure()",
			  "Nie wczytano pelnej info o funkcjach aktywacji.");
	}
	file.close();
	LOG_IO2("Wczytano.\n");


	return (0);
}


//--- Wczytywanie ustawien wejsciowych -----------------------------------------

int load_points( const string& namfile, VecPoint& vecIn, Size inSize,
				 VecPoint& vecOut, Size outSize )
{
	LOG_IO1("load_points()\n");
	ifstream file;
	string line;

	file.open(namfile.c_str(), std::ios_base::in);
	if( ! file.good() ){
		ERROR(1, "::load_points()",
			  "Blad otwarcia pliku `" << namfile << "'");
	}

	vecIn.clear();
	vecOut.clear();

	while( file ){
		getline( file, line );
		if( line.empty() ) continue;

		vecIn.push_back(Point(inSize));
		vecOut.push_back(Point(outSize));

		std::stringstream ss(line);
		LOG_IO4("");
		for( size_t i= 0 ; i < inSize ; ++i ) {
			ss >> vecIn.back()[i];
			LOG_IO4_( vecIn.back()[i] << " ");
		}
		LOG_IO4_("| ");
		for( size_t i= 0 ; i < outSize ; ++i ) {
			ss >> vecOut.back()[i];
			LOG_IO4_( vecOut.back()[i] << " ");
		}
		LOG_IO4_("\n");
	}

	file.close();

	LOG_IO2("Wczytano punktow: `" << vecOut.size() << "'\n");

	return  (0);
}

