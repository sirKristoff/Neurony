#include <algorithm>
#include <numeric>
#include <stdlib.h>
#ifdef DEBUG
 #include <stdexcept>
#endif

using namespace std;

#include "funs.h"
#include "net.h"


Input Net::sNi= 0.1;

namespace my{
Weight rand()
{
	return ::rand()%2;
//	return Weight(::rand())/RAND_MAX -0.5;  // <-0.5, 0.5)
}
}

// wartosc biasu
#define  X0      (fLockBias==lbLock ? Input(0) : Input(1))
// wartosc wagi dla biasu
#define  W0j(j)  (fLockBias==lbLock ? Weight(0) : mW[j])


//*** Net **********************************************************************
/**
 * @brief Net
 * @param N  Ilosci neuronow w kolejnych warstwach
 */
Net::Net(const vector<Size>& N, LockBias lockBias)
	: nN(N), mA(NULL), mFun(N.size(), ::ident), mDif(N.size(), ::d_ident),
	  fLockBias(lockBias), fLearningState(lsLearned)
{
	nCumN = Net::calculateCumN(N);
	nCumW = Net::calculateCumW(N);

	mW = new Weight[ nCumW.back() ];
	std::generate(&mW[0], &mW[nCumW.back()], my::rand);

	this->unlearnedState();  // zarezerwuj pamiec dla mA

	mFun[0] = NULL;
	mDif[0] = NULL;
}

//*** Net **********************************************************************
Net::Net(const vector<Size>& N , const vector<Fun> &fun, const vector<Dif> &dif,
		 LockBias lockBias)
	: nN(N), mA(NULL), mFun(fun), mDif(dif), fLockBias(lockBias),
	  fLearningState(lsLearned)
{
#ifdef DEBUG
	if( N.size() != fun.size()+1  ||  N.size() != dif.size()+1 )
		throw range_error("Net::Net(vector<Size>,vector<Fun>,vector<Dif>): Zla ilosc funkcji aktywacji");
#endif

	nCumN = Net::calculateCumN(N);
	nCumW = Net::calculateCumW(N);
	mW = new Weight[ nCumW.back() ];
	std::generate(&mW[0], &mW[nCumW.back()], my::rand);

	this->unlearnedState();  // zarezerwuj pamiec dla mA

	mFun.insert(mFun.begin(),NULL);  // mFun nie ma funkcji pod indeksem 0
	mDif.insert(mDif.begin(),NULL);
}

//*** ~Net *********************************************************************
Net::~Net()
{
	delete[] mW;
	this->learnedState();  // ewentualnie zwolni pamiec mA
}



//*** example ******************************************************************
/**
 * @brief Uczenie na jednym przykladzie
 * @param u  wzorcowe wejscie
 * @param v  wzorcowe wyjscie
 */
void
Net::example( const vector<Input>& u, const vector<Input>& v )
{
#ifdef DEBUG
	if( u.size() != nN[0] )
		throw range_error("Net::example(vector<Input>,vector<Input>): "
						  "Rozmiar wzorcowego wejscia nie zgadza sie z wymiarem sieci");
	if( v.size() != nN.back() )
		throw range_error("Net::example(vector<Input>,vector<Input>): "
						  "Rozmiar wzorcowego wyjscia nie zgadza sie z wymiarem sieci");
#endif

	const Size L= nN.size()-1; // ilosc warstw sieci
	vector<Input>  B( nCumN[L], Input(0) ); // wartosci b^l_j dla kazdego neuronu

	this->unlearnedState();  // wlaczamy nauke
	vector<Input> y_ = this->y(u);  // obliczamy odpowiedzi neuronow

	Size iA;  // globalny indeks neuronu

	// dla ostatniej warstwy L
	for( Size j= 0 ; j<nN[L] ; ++j ){
		iA = idxA(L,j);  // indeks wyjscia neuronu
		B[iA] = ( y_[j] - v[j] ) * (mDif[L])( mA[iA] );
	}

	Input sumBW;
	// dla warstw od L-1 do 1
	for( Size l= L-1 ; 0<l ; --l ){
		// dla kazdego neuronu w warstwie
		for( Size j= 0 ; j<nN[l] ; ++j ){
			iA = idxA(l,j);  // indeks wyjscia neuronu

			sumBW = Input(0);
			// wszystkie neurony z warstwy wyzszej
			for( Size jj= 0 ; jj<nN[l+1] ; ++jj )  //      \/ waga 1 polaczona z wejsciem 0
				sumBW += B[idxA(l+1,jj)] * mW[idxW(l+1,jj,j+1)];
			B[iA] = sumBW * (mDif[l])( mA[iA] );
		}
	}

	// korekcja wag
	Input dB;
	Input dW= 0;
	vector<Input> in= u;
	vector<Input> out;

	// TODO: zachowac wartosci dW

	// dla warstw od 1 do L
	for( Size l = 1 ; l<=L ; ++l ){  // kazda warstwa
		out = this->y(in,l);  // wejscie dla nastepnej warstwy

		for( Size j= 0 ; j<nN[l] ; ++j ){  // kazdy neuron
			dB= - Net::sNi * B[idxA(l,j)];  // b^l_j

			dW = dB * X0;  // waga biasu
			mW[idxW(l,j,0)] += dW;

			for( Size i= 1 ; i<=nN[l-1] ; ++i ){  // kazda waga
				dW =  dB * in[i-1];
				mW[idxW(l,j,i)] += dW;
			}
		}

		in=out; // wyjscie obecnej warstwy staje sie wejsciem dla kolejnej
	}
}


//*** e ************************************************************************
/**
 * @brief Blad sieci wzgledem wzorca
 * @param u  wzorcowe wejscie
 * @param v  wzorcowe wyjscie
 * @return  Srednie odchylenie wyjscia sieci od wzorcowego wyjscia
 */
Input
Net::e( const vector<Input>& u, const vector<Input>& v )
{
	vector<Input> out= this->y( u );
	Input ret= Input(0);

	for( size_t i= 0 ; i<out.size() ; ++i ){
#ifdef DEBUG
		ret += ( out.at(i) - v.at(i) ) * ( out.at(i) - v.at(i) );
#else
		ret += ( out[i] - v[i] ) * ( out[i] - v[i] );
#endif
	}

	return (ret / out.size());
}

//*** y ************************************************************************
/**
 * @brief Wyjscie sieci neuronowej
 * @param x  wejscie pierwszej warstwy
 * @return Poziom aktywacji wszystkich neuronow w warstwie wyjsciowej
 */
vector<Input>
Net::y( const vector<Input>& x )
{
	vector<Input> out = y(x,1);

	for( Size l= 2 ; l<nN.size() ; ++l ){
		out = y(out, l);
	}

	return (out);
}

//*** y ************************************************************************
/**
 * @brief Wyjscie l-tej warstwy, ktore jest wejsciem dla warstwy l+1
 * @param x  wejscie warstwy
 * @param l  numer warstwy sieci
 * @return Poziom aktywacji wszystkich neuronow w warstwie
 * @note l C <1, L)
 */
vector<Input>
Net::y( const vector<Input>& x, Size l )
{
	vector<Input> out(nN[l], Input());

	// TODO: wykonac obliczenia dla calej warstwy bez funkcji a(x,l,j)
	// TODO: ograniczyc ilosc wywolan funkcji wyliczajacych indeksy
	for( size_t j= 0 ; j<nN[l] ; ++j)
		out[j] = (mFun[l])( a(x,l,j) );

	return (out);
}

//*** y ************************************************************************
/**
 * @brief Wyjscie j-tego neuronu w l-tej warstwie
 * @param x  wejscie neuronu
 * @param l  numer warstwy sieci
 * @param j  numer neuronu w warstwie
 * @return Poziom aktywacji neuronu
 * @note l C <1, L),  j C <0, nN[l])
 */
Input
Net::y( const vector<Input>& x, Size l, Size j )
{
	return  (mFun[l])( a(x,l,j) );
}

//*** a ************************************************************************
/**
 * @brief Odpowiedz l-tej warstwy, ktore jest wejsciem dla funkcji aktywacji warstwy l+1
 * @param x  wejscie warstwy
 * @param l  numer warstwy sieci
 * @return Odpowiedzi wszystkich neuronow w warstwie
 * @note l C <1, L)
 */
vector<Input>
Net::a( const vector<Input>& x, Size l )
{
#ifdef DEBUG
	if( l==0 || nN.size()<=l )
		throw out_of_range("Net::a(vector<Input>,Size): Zly numer warstwy");
	if( x.size() != nN[l-1] )
		throw range_error("Net::a(vector<Input>,Size): Dlugosc wektora nie zgadza sie z iloscia wejsc warstwy");
#endif

	vector<Input> out(nN[l], Input());

	// TODO: wykonac obliczenia dla calej warstwy bez funkcji a(x,l,j)
	// TODO: ograniczyc ilosc wywolan funkcji wyliczajacych indeksy
	for( size_t j= 0 ; j<nN[l] ; ++j)
		out[j] = a(x,l,j);

	return (out);
}


//*** a ************************************************************************
/**
 * @brief Iloczyn wag i wejscia j-tego neuronu w l-tej warstwie
 * @param x  wejscie neuronu
 * @param l  numer warstwy sieci
 * @param j  numer neuronu w warstwie
 * @return Iloczyn wag neuronu i jego wejscia
 * @note l C <1, L),  j C <0, nN[l])
 */
Input
Net::a( const vector<Input> &x, Size l, Size j )
{
#ifdef DEBUG
	if( l==0 || nN.size()<=l )
		throw out_of_range("Net::a(vector<Input>,Size,Size): Zly numer warstwy");
	if( nN[l]<=j )
		throw out_of_range("Net::a(vector<Input>,Size,Size): Zly numer neuronu");
	if( x.size() != nN[l-1] )
		throw range_error("Net::a(vector<Input>,Size,Size): Dlugosc wektora nie zgadza sie z iloscia wejsc neuronu");
#endif

	const Size index0= idxW(l,j,0);
	Input ret= inner_product( &mW[index0+1], &mW[index0+1+nN[l-1]], x.begin(),
			W0j(index0) );  // iloczyn skalarny wejscia z wagami

	if( fLearningState == lsUnlearned )  // siec w trakcie nauki
		mA[ idxA(l,j) ] = ret;

	return (ret);
}


//*** learnedState *************************************************************
void
Net::learnedState()
{
#ifdef DEBUG
	if( fLearningState == lsUnlearned  &&  mA == NULL )
		throw logic_error("Net::learnedState(): siec nie nauczona a pamiec dla 'A' zwolniona!");
#endif

	if( fLearningState == lsUnlearned ){
		delete[] mA;
		fLearningState = lsLearned;
#ifdef DEBUG
		mA = NULL;
#endif
	}
}


//*** unlearnedState ***********************************************************
void
Net::unlearnedState()
{
#ifdef DEBUG
	if( nCumW.empty() )
		throw logic_error("Net::unlearnedState(): nie obliczono skumulowanej ilosci neuronow");
	if( fLearningState == lsLearned  &&  mA != NULL )
		throw logic_error("Net::unlearnedState(): siec nauczona a pamiec dla 'A' zarezerwowana!");
#endif

	if( fLearningState == lsLearned ){
		mA = new Input[ nCumN.back() ]; // skumulowana ilosc neuronow
		fLearningState = lsUnlearned;
	}
}

//*** idxW *********************************************************************
/**
 * @brief Indeks i-tej wagi w j-tym neuronie l-tej warstwy
 * @param l  numer warstwy
 * @param j  numer neuronu
 * @param i  numer wagi
 *
 * @note l C <1, L),  j C <0, nN[l]),  i C <0, nN[l-1]>
 */
Size
Net::idxW( Size l, Size j, Size i )  const
{
#ifdef DEBUG
//	if( l<=0 || nN.size()<=l || j<0 || nN[l]<=j || i<0 || nN[l-1]<=i )
	if( l==0 || nN.size()<=l || nN[l]<=j || nN[l-1]<i )
		throw domain_error("Net::idxW(Size,Size,Size): zly parametr metody");

	if( nCumW.back() <= ( nCumW[l-1] + j*( nN[l-1]+1 ) + i ) )
		throw out_of_range("Net::idxW(Size,Size,Size): zla wartosc zwracana");
#endif

	//       \/ ile wag we wszystkich poprzednich warstwach
	//                     \/ ile wag w poprzedzajacych neuronach w obecnej warstwie
	return ( nCumW[l-1] + j*( nN[l-1]+1 ) + i );
}

//*** idxA *********************************************************************
/**
 * @brief Indeks j-tego neuronu w l-tej warstwie
 * @param l  numer warstwy
 * @param j  numer neuronu w warstwie
 *
 * @note l C <1, L),  j C <0, nN[l]),
 */
Size
Net::idxA( Size l, Size j )  const
{
#ifdef DEBUG
	if( l==0 || nN.size()<=l || nN[l]<=j )
		throw domain_error("Net::idxA(Size,Size): zly parametr metody");

	if( nCumN.back() <= (nCumN[l-1] + j) )
		throw out_of_range("Net::idxA(Size,Size): zla wartosc zwracana");
#endif

	return  (nCumN[l-1] + j);
}

//*** calculateCumW ************************************************************
vector<Size>
Net::calculateCumW(const vector<Size>& N )
{
#ifdef DEBUG
	if( N.empty() )
		throw invalid_argument("Net::calculateCumW(vector<Size>): Pusty wektor");
#endif

	vector<Size> sumW(N.size()); // suma ilosci wag neuronow w konkretnej warstwie
	vector<Size> cumW(N.size()); // skumulowana suma ilosci wag

	for( size_t i= 1 ; i<sumW.size() ; ++i ){
		sumW[i] = ( N[i-1]+1 ) * N[i];
	}

	std::partial_sum( sumW.begin(), sumW.end(), cumW.begin() );  // skumulowana suma z sumW

	return (cumW);
}

//*** calculateCumN ************************************************************
vector<Size>
Net::calculateCumN( const vector<Size>& N )
{
#ifdef DEBUG
	if( N.empty() )
		throw invalid_argument("Net::calculateCumN(vector<Size>): Pusty wektor");
#endif

	vector<Size> cumN(N.size(), Size(0));

	std::partial_sum( N.begin()+1, N.end(), cumN.begin()+1 );  // skumulowana suma z N

	return (cumN);
}
