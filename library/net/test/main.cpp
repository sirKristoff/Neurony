#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "./../definitions.h"
#include "./../funs.h"
#include "./../net.h"

using namespace std;


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

//*****************************************************************************

const Size nIn= 2;
const Size L= 3;
const Size l[L+1] = {nIn,4,2,1};
const Fun f[L] = {ident,uni_sigm,bi_sigm};
const Dif df[L] = {d_ident,d_uni_sigm,d_bi_sigm};

Net net(vector<Size>(l,l+L+1), vector<Fun>(f,f+L), vector<Dif>(df,df+L),
		Net::lbLock );

//*****************************************************************************
/** rozmiary warstw */
void printN()
{
	LOG_IO( LINE << "nN(l):    ");
	FOR_EACH(l,net.nN.size(),\
			 LOG_IO( net.nN[l] << " " ));
	LOG_IO(endl);
}

/** skumulowane ilosci wag warstw */
void printCumW()
{
	LOG_IO( LINE << "nCumW(l): ");
	FOR_EACH(l, net.nCumW.size(),\
			 LOG_IO( net.nCumW[l] << " " ));
	LOG_IO(endl);
}

/** indeksy wag */
void testIdx()
{
	LOG_IO( LINE << "Net idx(l,j,i):\n");
	for( Size l= 1 ; l<net.nN.size() ; ++l ){
		for( Size j= 0 ; j<net.nN[l] ; ++j ){
			LOG_IO( "(" << l << "," << j << ",i): ");
			for( Size i= 0 ; i<=net.nN[l-1] ; ++i )
				LOG_IO( net.idx(l,j,i) << " ");
			LOG_IO(endl);
		}
		LOG_IO( endl );
	}
}

/** wartosci wag */
void printW()
{
	LOG_IO( LINE << "mW(l,j,i):\n");
	for( Size l= 1 ; l<net.nN.size() ; ++l ){
		for( Size j= 0 ; j<net.nN[l] ; ++j ){
			LOG_IO( "(" << l << "," << j << ",i): ");
			for( Size i= 0 ; i<=net.nN[l-1] ; ++i )
				LOG_IO( net.mW[ net.idx(l,j,i) ] << " ");
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

//*****************************************************************************
int main()
{
//	srand(time(0));

	printN();    // 2 4 2 1
	printCumW(); // 0 12 22 25
	printW();

	testIdx();

	a();

	printOdpowiedz();

	testFun();

	return 0;
}
