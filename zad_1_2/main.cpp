#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include "../library/net/definitions.h"
#include "../library/net/funs.h"
#include "../library/net/net.h"

using namespace std;


string fnNet= "network.txt";
string fnIn= "in.txt";
string fnOut= "out.txt";

#define FOR_EACH(i,n,what) \
	for(unsigned int i= 0 ; i<(n) ; ++i){ \
		what; }

#define LINE  "-------------------------------------------------------------\n"

#ifdef DEBUG
 #define LOG(what) \
	what
#else
 #define LOG(what)
#endif

#define LOG_IO(what) \
	LOG( cerr << what )

#define RAND_SIZE(min,max) ((::rand()%(max-min))+(min))
#define RAND_UNI (Input(::rand())/RAND_MAX)

//*****************************************************************************
int main()
{
	ofstream fNet;
	ofstream fIn;
	ofstream fOut;

	fNet.open(fnNet.c_str());
	fIn.open(fnIn.c_str());
	fOut.open(fnOut.c_str());


	srand(time(0));

	const size_t L= 4;
	const size_t S= 5;
	vector<Size> l(L);
	vector<Fun> f(L-1, uni_sigm); // funkcje aktywacji
	vector<Dif> d(L-1, d_uni_sigm);

	// losowanie ilosci neuronow
	for( size_t i= 0 ; i<l.size() ; ++i ){
		l[i] = RAND_SIZE(5, 15);
	}

	Net net(l,f,d); // tworzenie sieci

	vector<vector<Input> > tabX(S,vector<Input>(net.nN[0])); // S wejsc

	// losowanie wartosci wejsciowych
	for( size_t j= 0 ; j<tabX.size() ; ++j )
		for( size_t i= 0 ; i<tabX[j].size() ; ++i )
			tabX[j][i] = RAND_UNI;


	FOR_EACH(i,l.size(),
			 LOG_IO( net.nN[i] << " " ););
	LOG_IO( endl );

	for( size_t j= 0 ; j<tabX.size() ; ++j ){
		for( size_t i= 0 ; i<tabX[j].size() ; ++i )
			LOG_IO( tabX[j][i] << " ");
		LOG_IO( endl );
	}



	// generowanie wyjsc
	vector< vector<Input> > tabOut;
	for( size_t i= 0 ; i<tabX.size() ; ++i )
		tabOut.push_back( net.y(tabX[i]) );


	LOG_IO(LINE);
	for( size_t o= 0 ; o<tabX.size() ; ++o ){
		for( size_t i= 0 ; i<tabOut[o].size() ; ++i )
			LOG_IO( tabOut[o][i] << " " );
		LOG_IO( endl );
	}


	// in
	for( size_t o= 0 ; o<tabX.size() ; ++o ){
		for( size_t j= 0 ; j<tabX.size() ; ++j )
			fIn << tabX[o][j] << " ";
		fIn << endl;
	}

	// out
	for( size_t o= 0 ; o<tabOut.size() ; ++o ){
		for( size_t j= 0 ; j<tabOut.size() ; ++j )
			fOut << tabOut[o][j] << " ";
		fOut << endl;
	}


	// net
	fNet << L-1 << endl;
	for( size_t i= 0; i<net.nN.size() ; ++i )
		fNet <<net.nN[i] << endl;

//	for( size_t l= 0 ; l<net.nCumW.back() ; ++l )
//		fNet <<

	for( Size l= 1 ; l<net.nN.size() ; ++l ){
		for( Size j= 0 ; j<net.nN[l] ; ++j ){
			for( Size i= 0 ; i<=net.nN[l-1] ; ++i )
				fNet << net.mW[ net.idx(l,j,i) ] << " ";
			fNet << endl;
		}
	}


	fNet.close();
	fIn.close();
	fOut.close();

	return 0;
}

