#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "../library/net/definitions.h"
#include "../library/net/funs.h"
#include "../library/net/net.h"

using namespace std;


//*****************************************************************************

#define FOR_EACH(i,n,what) \
	for(unsigned int i= 0 ; i<(n) ; ++i){ \
		what; }


template<class InputIterator, class SizeN, class OutputIterator>
OutputIterator copy_n (InputIterator first, SizeN n, OutputIterator result)
{
	while (n>0) {
		*result = *first;
		++result; ++first;
		--n;
	}
	return result;
}

//*****************************************************************************

string namIn=      "in.txt";           // wejscie sieci jednoczesnie wyjscie
string namNetPre=  "network_pre.txt";  // siec przed nauka
string namOutPre=  "out.txt";          // wyjscie z sieci przed nauka
string namNetPost= "network.txt";      // siec po nauce
string namTrain=   "train.png";        // przebieg nauki
string namData=    "data.txt";         // plik z danymi do wykresu

//*****************************************************************************
/** in.txt
1 0 0 0
0 1 0 0
0 0 1 0
0 0 0 1

*/
/** network_pre.txt
2
4
2
4
0.000000 0.472801 0.935628 -0.491067 0.372298
0.000000 -0.100727 0.297812 -0.615205 -0.153709
0.000000 0.346888 0.484956
0.000000 0.654139 -0.961072
0.000000 0.872161 0.603206
0.000000 0.665244 -0.489092

*/
//*****************************************************************************
ostream_iterator<Size> itCerrI( cerr, " " );
ostream_iterator<Weight> itCerrF( cerr, " " );

int main()
{
	ifstream fIn;
	ifstream fNetPre;
	ofstream fOutPre;
	ofstream fNetPost;
	ofstream fData;

	//--- wczytywanie sieci ----------------------------------------------------

	fNetPre.open(namNetPre.c_str());

	const Size nL=  1 + *istream_iterator<Size>(fNetPre);  // ilosc warstw ukrytych
	vector<Size> vN;                               // wektor z rozmiarami warstw

	copy_n( istream_iterator<Size>(fNetPre), nL,                // rozmiary nL warstw
			insert_iterator< vector<Size> >(vN, vN.begin()) );  // skopiuj do wektora

	Net net( vN, vector<Fun>(nL-1, uni_sigm), vector<Dif>(nL-1, d_uni_sigm) );

	// wczytaj wagi sieci
	copy_n( istream_iterator<Weight>(fNetPre), net.sizeW(), net.mW );

	fNetPre.close();

	//--- wczytywanie przykladow -----------------------------------------------

	fIn.open(namIn.c_str());
	vector< vector<Input> > train(0);

	{
		istream_iterator<Input> itIn(fIn);
		while( fIn ){
			train.push_back( vector<Input>(0) );
			for( Size i= 0 ; i<net.sizeIn() ; ++i ){
				train.back().insert(train.back().end(), *itIn);
				++itIn;
			}
		}
	}

	const Size nTrain= train.size();  // ilosc przykladow

	fIn.close();

	//--- wyjscie sieci przed nauka --------------------------------------------

	fOutPre.open(namOutPre.c_str());

	for( size_t i= 0 ; i<nTrain ; ++i ){
		copy_n( net.y(train[i]).begin(), net.sizeOut(),
				ostream_iterator<Input>(fOutPre, " ") );
		fOutPre << endl;
	}

	fOutPre.close();

	//--- nauka sieci ----------------------------------------------------------

	vector<Input> errs(0);     // bledy siecie w trakcie nauki
	const Size nEpoch= 4000;  // ilosc epok nauki
	Net::sNi= 0.1;

	for( Size e= 0 ; e<nEpoch ; ++e ){
		for( size_t i= 0 ; i<nTrain ; ++i ){
			net.example( train[i], train[i] );
		}

		errs.push_back(Input(0));
		for( size_t i= 0 ; i<nTrain ; ++i ){
			errs.back() += net.e( train[i], train[i] );
		}
		errs.back() /= nTrain;
	}

	cerr << "Odpowiedz sieci dla przykladow:" << endl;
	for( size_t i= 0 ; i<nTrain ; ++i ){
		copy_n( net.y(train[i]).begin(), net.sizeOut(), itCerrF );
		cerr << endl;
	}

	cerr << endl << "Blad sieci dla przykladow:" << endl
		 << errs.back() << endl;


	//--- zapis struktury sieci po nauce ---------------------------------------

	fNetPost.open(namNetPost.c_str());

	fNetPost << net.size()-1 << endl;
	for( Size l= 0 ; l<net.size() ; ++l )
		fNetPost << net.nN[l] << endl;

	for( Size l= 1 ; l<net.size() ; ++l ){
		for( Size j= 0 ; j<net.sizeN(l) ; ++j ){
			copy_n( &net.mW[ net.idxW(l,j,0) ], net.sizeN(l-1)+1,
					ostream_iterator<Weight>(fNetPost, " ") );
			fNetPost << endl;
		}
	}

	fNetPost.close();


	//--- zapis struktury sieci po nauce ---------------------------------------

	fData.open(namData.c_str());

	for( vector<Input>::const_iterator itData= errs.begin() ;
		 itData != errs.end() ; ++itData )
		fData << *itData << endl;

	fData.close();

	ostringstream strNi;
	strNi << Net::sNi;

	string cmd= string("") +
				"gnuplot -persist -e \""
				"set term png size 600, 400; "
				"set output \'" + namTrain.c_str() + "\'; "
				"plot '"+ namData +"' "
				"title \'ni= "+ strNi.str() +"\' "
				"with linespoints ls 3 pt -1"
				"\"";

	system(cmd.c_str());

	return 0;
}

