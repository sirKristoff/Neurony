#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "./../definitions.h"
#include "./../funs.h"
#include "./../net.h"
#include "./../../test/testgroup.h"

using namespace std;


#define FOR_EACH(i,n,what) \
	for(unsigned int i= 0 ; i<(n) ; ++i){ \
		what; }

#define LINE  "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n"

#ifdef DEBUG
 #define LOGD(what) \
	what
#else
 #define LOGD(what)
#endif

#define LOG_IO(what) \
	LOGD( cerr << what )

//*****************************************************************************

class TestGroupSimple : public TestGroup, public Net
{

	static const Size nIn;
	static const Size L;
	static const Size tabL[];
	static const Fun f[];
	static const Dif df[];

public:

	TestGroupSimple()
		: TestGroup("Testy pol po konstrukcji obiektu"),
		  Net(vector<Size>(tabL,tabL+L+1), vector<Fun>(f,f+L),
						   vector<Dif>(df,df+L), Net::lbLock)
	{
		REGISTER_TC( TestGroupSimple, tc01_nN );
		REGISTER_TC( TestGroupSimple, tc02_Cum );
		REGISTER_TC( TestGroupSimple, tc03_Idx );
		REGISTER_TC( TestGroupSimple, tc04_constructor_structures );
	}


	// ilosci neuronow w warstwach
	TcResult
	tc01_nN()
	{
		LOG( "nN(l):    ");
		FOR_EACH(l, nN.size(), \
				LOG_ASSERT( nN[l] << " " ); );

		LOG(endl);

		ostringstream ilosci_neuronow;
		FOR_EACH(l, TestGroupSimple::L+1, \
				 ilosci_neuronow << TestGroupSimple::tabL[l]<<" " );
		ASSERT( ilosci_neuronow.str(),\
				"Ilosc neuronow w kazdej warstwie sie nie zgadza" );  // "2 4 2 1 "

		return  (trPass);
	}


	// Skumulowane ilosci wag, neuronow
	TcResult
	tc02_Cum()
	{
		LOG( "nCumW(l): " );
		FOR_EACH(l, nCumW.size(),\
				 LOG_ASSERT( nCumW[l] << " " ) );
		LOG(endl);
		ASSERT( "0 12 22 25 ", \
				"Ilosc wag w warstwach sie nie zgadza" );


		LOG(SEPLINE);


		LOG( "nCumN(l): " );
		FOR_EACH(l, nCumN.size(),\
				 LOG_ASSERT( nCumN[l] << " " ) );
		LOG(endl);
		ASSERT( "0 4 6 7 ", \
				"Ilosc neuronow w warstwach sie nie zgadza" );  // sumowane ilosci neuronow

		return  (trPass);
	}


	// Indeksy wag, neuronow
	TcResult
	tc03_Idx()
	{
		Size ilosc_wag= 0;
		LOG( "idxW(l,j,i):" << endl );

		for( Size l= 1 ; l<nN.size() ; ++l ){
			for( Size j= 0 ; j<nN[l] ; ++j ){
				LOG( "(" << l << "," << j << ",i): " );
				for( Size i= 0 ; i<=nN[l-1] ; ++i ){
					LOG_ASSERT( idxW(l,j,i) << " ");
					++ilosc_wag;
				}
				LOG(endl);
			}
			LOG(endl);
		}


		ostringstream indeksy;
		FOR_EACH(i, ilosc_wag, indeksy<<i<<" ");
		ASSERT( indeksy.str(), \
				"Zle wartoscie metody idxW(l,j,i)" );


		// sprawdz ilosc_wag
		COMPAR( ilosc_wag, nCumW.back(), \
				"Niezgodnosc calkowitej ilosci wag w sieci" );

		LOG(SEPLINE);

		Size ilosc_neuronow= 0;
		LOG( "idxA(l,j):\n" );

		for( Size l= 1 ; l<nN.size() ; ++l ){
			LOG( "(" << l << ",j): " );
			for( Size j= 0 ; j<nN[l] ; ++j ){
				LOG_ASSERT( idxN(l,j) << " " );
				++ilosc_neuronow;
			}
			LOG(endl);
		}
		LOG(endl);

		indeksy.str("");
		FOR_EACH(i, ilosc_neuronow, indeksy<<i<<" ");
		ASSERT( indeksy.str(), \
				"Zle wartosci metody idxA(l,j)");

		// sprawdz ilosc_neuronow
		COMPAR( ilosc_neuronow, nCumN.back(), \
				"Niezgodnosc calkowitej ilosci neuronow w sieci");

		return  (trPass);
	}


	// Sprawdzanie struktur wypelnianych w trakcie dzialania konstruktora
	TcResult
	tc04_constructor_structures()
	{
		COMPAR( mFun[0] , NULL, "Nieprawidlowa funkcja aktywacji" );
		COMPAR( mDif[0] , NULL, "Nieprawidlowa pochodna funkcji aktywacji" );

		COMPAR( mFun.size() , TestGroupSimple::L+1, "Zla ilosc funkcji aktywacji");
		COMPAR( mDif.size() , TestGroupSimple::L+1, "Zla ilosc pochodnych funkcji aktywacji");

		COMPAR( fLearningState, lsUnlearned, "Zla wartosc flagi nauki sieci" );

		return  (trPass);
	}

};

const Size TestGroupSimple::nIn= 2;
const Size TestGroupSimple::L= 3;
const Size TestGroupSimple::tabL[L+1] = {nIn,4,2,1};
const Fun TestGroupSimple::f[L] = {ident,ident,ident};
const Dif TestGroupSimple::df[L] = {d_ident,d_ident,d_ident};



//*****************************************************************************

const Size nIn= 2;                // ilosc wejsc sieci
const Size L= 3;                  // ilosc warstw sieci
const Size l[L+1] = {nIn,4,2,1};  // ilosc neuronow w warstwach
const Fun f[L] = {ident,uni_sigm,bi_sigm};        // funkcje aktywacji neuronow w warstwach
const Dif df[L] = {d_ident,d_uni_sigm,d_bi_sigm};


Net net(vector<Size>(l,l+L+1), vector<Fun>(f,f+L), vector<Dif>(df,df+L),
		Net::lbLock );

//*****************************************************************************




/** wartosci wag */
void printW()
{
	LOG_IO( LINE << "mW(l,j,i):\n");
	for( Size l= 1 ; l<net.nN.size() ; ++l ){
		for( Size j= 0 ; j<net.nN[l] ; ++j ){
			LOG_IO( "(" << l << "," << j << ",i): ");
			for( Size i= 0 ; i<=net.nN[l-1] ; ++i )
				LOG_IO( net.mW[ net.idxW(l,j,i) ] << " ");
			LOG_IO(endl);
		}
		LOG_IO(endl);
	}
}

/** odpowiedz sieci */
void a()
{

	Input tmpX[nIn]= {2.0,4.0};  // wejscie
	vector<Input> x(tmpX,tmpX+nIn);

	LOG_IO( LINE << "Odpowiedzi neuronow pierwszej warstwy:\n" );
	LOG_IO("x:   ");
	LOG_IO( (net.fLockBias==Net::lbLock ? 0 : 1) << " ");
	FOR_EACH(i,x.size(),
			 LOG_IO( x[i] << " "););
	LOG_IO(endl);

	vector<Input> out= net.a(x,1);
	LOG_IO( "a:  ");
	FOR_EACH(j, out.size(),\
			 LOG_IO( out[j] << " " ););
	LOG_IO(endl);
}

/** funkcje aktywacji */
void testFun()
{
	LOG_IO( LINE << "funkcje aktywacji\n" );
#define TEST_FUN(funk,poch,wart) \
	LOG_IO( "x= " << wart << "  f(x)= " << (funk)(wart) << "  df(x)= " << (poch)(wart) << endl);

	FOR_EACH(i,3,\
			 TEST_FUN(f[i],df[i],0.0););
}

void printOdpowiedz()
{
	Input tmpX[nIn]= {2.0,4.0};  // wejscie
	vector<Input> x(tmpX,tmpX+nIn);

	LOG_IO( LINE << "Odpowiedz sieci\n" );
	LOG_IO( "y(x) = " << net.y(x).front() << endl);
}


void learnState()
{
	LOG_IO( LINE << "LearningState\n" );
	LOG_IO( "fLearningState= "
			<< (net.fLearningState==Net::lsLearned ? "lsLearned" : "lsUnlearned")
			<< endl );

//	net.fLearningState= Net::lsLearned;
	LOGD( net.learnedState() );
	LOG_IO( "  turn learnedState()" << endl );
	LOG_IO( "fLearningState= "
			<< (net.fLearningState==Net::lsLearned ? "lsLearned" : "lsUnlearned")
			<< endl );

	LOGD( net.unlearnedState() );
	LOG_IO( "  turn unlearnedState()" << endl );
	LOG_IO( "fLearningState= "
			<< (net.fLearningState==Net::lsLearned ? "lsLearned" : "lsUnlearned")
			<< endl );
}

void printA()
{
	Input tmpX[nIn]= {2.0,4.0};  // wejscie
	vector<Input> x(tmpX,tmpX+nIn);
	net.y(x);

	LOG_IO( LINE << "A:\n" );

	LOG_IO("x:   ");
	LOG_IO( (net.fLockBias==Net::lbLock ? 0 : 1) << " ");
	FOR_EACH(i,x.size(),
			 LOG_IO( x[i] << " "););
	LOG_IO(endl);

	LOG_IO( "nCumN(l):  ");
	FOR_EACH(l,net.nCumN.size(),\
			 LOG_IO( net.nCumN[l] << " " ));
	LOG_IO(endl);

	LOG_IO( "mA(l,j):\n");
	for( Size l= 1 ; l<net.nN.size() ; ++l ){
		LOG_IO( "(" << l << ",j): ");
		for( Size j= 0 ; j<net.nN[l] ; ++j ){
			LOG_IO( net.mA[ net.idxN(l,j) ] << " ");
		}
		LOG_IO(endl);
	}

}

void error()
{
	Input tmpX[nIn]= {2.0,4.0};  // wejscie
	vector<Input> u(tmpX,tmpX+nIn);
	vector<Input> v(1,Input(1));
	vector<Input> y= net.y(u);

	LOG_IO( LINE << "Blad sieci" << endl );

	LOG_IO("u:   ");
	FOR_EACH(i,u.size(),
			 LOG_IO( u[i] << " "););
	LOG_IO(endl);

	LOG_IO("v:   ");
	FOR_EACH(i,v.size(),
			 LOG_IO( v[i] << " "););
	LOG_IO(endl);

	LOG_IO("y:   ");
	FOR_EACH(i,y.size(),
			 LOG_IO( y[i] << " "););
	LOG_IO(endl);

	LOG_IO("err: " << net.e(u,v) << endl);
}

void learning()
{
	Input tmpX[nIn]= {2.0,4.0};  // wzorcowe wejscie
	Input tmpV[] = {0.0};        // wzorcowe wyjscie
	vector<Input> u(tmpX,tmpX+nIn);
	vector<Input> v(tmpV, tmpV+1);
	vector<Input> y_;

	LOG_IO( LINE << "Uczenie sieci" << endl );

	LOG_IO("u:   ");
	FOR_EACH(i,u.size(),
			 LOG_IO( u[i] << " "););
	LOG_IO(endl);
	LOG_IO("v:   ");
	FOR_EACH(i,v.size(),
			 LOG_IO( v[i] << " "););
	LOG_IO(endl);

	y_ = net.y(u);
	LOG_IO("y(t=0): ");
	FOR_EACH(i,y_.size(),
			 LOG_IO( y_[i] << " "););
	LOG_IO(endl);

	FOR_EACH(i,10000,
			 net.example(u,v););

	y_ = net.y(u);
	LOG_IO("y(t=10000): ");
	FOR_EACH(i,y_.size(),
			 LOG_IO( y_[i] << " "););
	LOG_IO(endl);

}


//*****************************************************************************
int main()
{
//	srand(time(0));

	TestGroupSimple tg1;

	tg1.run_group();


//	printW();
//	printA();

//	error();  // 0.289337

//	a();

//	printOdpowiedz();

//	testFun();

//	learnState();

//	printW();
//	learning();
//	printW();

	return 0;
}
