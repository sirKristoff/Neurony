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


//*****************************************************************************

class TestGroupSimple : public TestGroup, public Net
{

	static const Size L;
	static const Size tabL[];

public:

	TestGroupSimple()
		: TestGroup("Testy pol po konstrukcji obiektu"),
		  Net(vector<Size>(tabL,tabL+L))
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
		FOR_EACH(l, TestGroupSimple::L, \
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

		COMPAR( mFun.size() , TestGroupSimple::L, "Zla ilosc funkcji aktywacji");
		COMPAR( mDif.size() , TestGroupSimple::L, "Zla ilosc pochodnych funkcji aktywacji");

		COMPAR( fLearningState, lsLearned, "Zla wartosc flagi nauki sieci" );

		return  (trPass);
	}

};

const Size TestGroupSimple::L= 4;
const Size TestGroupSimple::tabL[L] = {2,4,2,1};



//*****************************************************************************

class TestGroupOperations : public TestGroup, public Net
{

	static const Size L;
	static const Size tabL[];
	static const Fun f[];
	static const Dif df[];

public:

	TestGroupOperations()
		: TestGroup("Testy operacji na sieci"),
		  Net(vector<Size>(tabL,tabL+L), vector<Fun>(f,f+L-1),
						   vector<Dif>(df,df+L-1), lbUnlock)
	{
		FOR_EACH(i,sizeW(), mW[i]=int(i%4)-2; );


		REGISTER_TC( TestGroupOperations, tc01_change_learningState );
		REGISTER_TC( TestGroupOperations, tc02_response );
		REGISTER_TC( TestGroupOperations, tc03_learning );
	}


	// zmiana stanu nauczania sieci
	TcResult
	tc01_change_learningState()
	{
		LOG( "Pruba zmianu stanu sieci na nauczona mimo tego ze ten stan jest juz osiagniety" << endl );
		learnedState();
		COMPAR( fLearningState, lsLearned, "Siec powinna byc w stanie \'lsLearned\'" );
		mA= NULL;

		LOG( "Zmiana stanu sieci na \'lsUnlearned\'" << endl );
		unlearnedState();
		COMPAR( fLearningState, lsUnlearned, "Siec powinna byc w stanie \'lsUnlearned\'" );
//		UNCOMP( mA, NULL, "Tablica odpowiedzi neuronow nie powinna byc pusta" );


		return  (trPass);
	}


	// obliczanie odpowiedzi sieci i jej bledu
	TcResult
	tc02_response()
	{
		LOG( "mW(l,j,i):  " << endl );
		for( Size l= 1 ; l<dynamic_cast<Net*>(this)->size() ; ++l ){
			for( Size j= 0 ; j<sizeN(l) ; ++j ){
				LOG( "(" << l << "," << j << ",i): ");
				for( Size i= 0 ; i<=sizeN(l-1) ; ++i )
					LOG( mW[ idxW(l,j,i) ] << " ");
				LOG(endl);
			}
			LOG(endl);
		}
		LOG(endl);


		LOG("u(i):  ");
		vector<Input> u(sizeIn());
		FOR_EACH(i, u.size(), \
				 u[i]=2*(i+1);
				LOG(u[i] << " "); );
		LOG(endl<<endl);


		unlearnedState();
		y(u);

		LOG( "mA(l,j):  " << endl );
		for( Size l= 1 ; l<dynamic_cast<Net*>(this)->size() ; ++l ){
			LOG( "(" << l << ",j): ");
			for( Size j= 0 ; j<sizeN(l) ; ++j ){
				LOG_ASSERT( mA[ idxN(l,j) ] << " ");
			}
			LOG(endl);
		}
		LOG(endl<<endl);

		ASSERT("-4 2 2 7 ", "Zla odpowiedz neuronow");


		LOG("v(i):  ");
		vector<Input> v(sizeOut(), Input(1));
		FOR_EACH(i, v.size(), \
				LOG(v[i] << " "); );
		LOG(endl<<endl);

		LOG("error:  ");
		LOG_ASSERT( e(u,v) );
		LOG(endl);
		ASSERT( 18.5, "Zle wyliczony blad sieci");


		return  (trPass);
	}


	// uczenie sieci
	TcResult
	tc03_learning()
	{
		LOG("u(i):  ");
		vector<Input> u(sizeIn());
		FOR_EACH(i, u.size(), \
				 u[i]=2*(i+1);
				LOG(u[i] << " "); );
		LOG(endl);
		LOG("v(i):  ");
		vector<Input> v(sizeOut(), Input(1));
		FOR_EACH(i, v.size(), \
				LOG(v[i] << " "); );
		LOG(endl<<endl);


		unlearnedState();
		y(u);


		LOG(SEPLINE);
		LOG( "mW(l,j,i):  " << endl );
		for( Size l= 1 ; l<dynamic_cast<Net*>(this)->size() ; ++l ){
			for( Size j= 0 ; j<sizeN(l) ; ++j ){
				LOG( "(" << l << "," << j << ",i): ");
				for( Size i= 0 ; i<=sizeN(l-1) ; ++i )
					LOG( mW[ idxW(l,j,i) ] << " ");
				LOG(endl);
			}
			LOG(endl);
		}
		LOG(endl);

		LOG( "mA(l,j):  " << endl );
		for( Size l= 1 ; l<dynamic_cast<Net*>(this)->size() ; ++l ){
			LOG( "(" << l << ",j): ");
			for( Size j= 0 ; j<sizeN(l) ; ++j ){
				LOG( mA[ idxN(l,j) ] << " ");
			}
			LOG(endl);
		}
		LOG(SEPLINE);


		example(u,v);
//		FOR_EACH(i,50,example(u,v););
		y(u);

		LOG( "mW(l,j,i):  " << endl );
		for( Size l= 1 ; l<dynamic_cast<Net*>(this)->size() ; ++l ){
			for( Size j= 0 ; j<sizeN(l) ; ++j ){
				LOG( "(" << l << "," << j << ",i): ");
				for( Size i= 0 ; i<=sizeN(l-1) ; ++i )
					LOG_ASSERT( mW[ idxW(l,j,i) ] << " ");
				LOG(endl);
			}
			LOG(endl);
		}
		LOG(endl);

		LOG( "mA(l,j):  " << endl );
		for( Size l= 1 ; l<dynamic_cast<Net*>(this)->size() ; ++l ){
			LOG( "(" << l << ",j): ");
			for( Size j= 0 ; j<sizeN(l) ; ++j ){
				LOG( mA[ idxN(l,j) ] << " ");
			}
			LOG(endl);
		}
		LOG(endl);

		ASSERT("-0.7 1.6 0.6 2.2 -2.1 -0.6 -0.2 0.4 0.4 -2.2 ",
			   "Wagi sieci zostaly blednie zmienione podczas nauki");

		return  (trPass);
	}

};

const Size TestGroupOperations::L= 3;
const Size TestGroupOperations::tabL[L] = {1,2,2};
const Fun TestGroupOperations::f[L-1] =  {  ident,   ident};
const Dif TestGroupOperations::df[L-1] = {d_ident, d_ident};


//*****************************************************************************
//*****************************************************************************
int main()
{
//	srand(time(0));

	TestGroupSimple tg1;

	tg1.run_group();


	TestGroupOperations tg2;

	tg2.run_group();


	return 0;
}
