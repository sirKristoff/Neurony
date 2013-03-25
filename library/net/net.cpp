#include <algorithm>
#include <numeric>
#include <stdlib.h>
#ifdef DEBUG
 #include <stdexcept>
#endif

using namespace std;

#include "funs.h"
#include "net.h"


namespace my{
Weight rand(){
	return ::rand()%2;
//	return Weight(::rand())/RAND_MAX -0.5;
}
}

//*** Net **********************************************************************
/**
 * @brief Net
 * @param N  Ilosci neuronow w kolejnych warstwach
 */
Net::Net(const vector<Size>& N, LockBias lockBias)
	: nN(N), mFun(N.size(), ::ident), mDif(N.size(), ::d_ident),
	  fLockBias(lockBias)
{
	nCumW = Net::calculateCumW(N);
	mW = new Weight[ nCumW.back() ];

	generate(&mW[0], &mW[nCumW.back()], my::rand);

	mFun[0] = NULL;
	mDif[0] = NULL;
}

//*** Net **********************************************************************
Net::Net(const vector<Size>& N , const vector<Fun> &fun, const vector<Dif> &dif,
		 LockBias lockBias)
	: nN(N), mFun(fun), mDif(dif), fLockBias(lockBias)
{
#ifdef DEBUG
	if( N.size() != fun.size()+1  ||  N.size() != dif.size()+1 )
		throw range_error("Net::Net(vector<Size>,vector<Fun>,vector<Dif>): Zla ilosc funkcji aktywacji");
#endif

	nCumW = Net::calculateCumW(N);
	mW = new Weight[ nCumW.back() ];

	generate(&mW[0], &mW[nCumW.back()], my::rand);

	mFun.insert(mFun.begin(),NULL);
	mDif.insert(mDif.begin(),NULL);
}

//*** ~Net *********************************************************************
Net::~Net()
{
	delete[] mW;
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

//	Input out= mW[idx(l,j,0)];
//	for( size_t i= 1 ; i<=nN[l-1] ; ++i ){
//		out += mW[idx(l,j,i)] * x[i-1];
//	}
//	return (out);
	const Size index0= idx(l,j,0);
	return  inner_product( &mW[index0+1], &mW[index0+1+nN[l-1]], x.begin(),
			(fLockBias==lbLock ? Weight(0) : mW[index0]) );
}


//*** idx **********************************************************************
/**
 * @brief Indeks i-tej wagi w j-tym neuronie l-tej warstwy
 * @param l  numer warstwy
 * @param j  numer neuronu
 * @param i  numer wagi
 *
 * @note l C <1, L),  j C <0, nN[l]),  i C <0, nN[l-1]>
 */
Size
Net::idx( Size l, Size j, Size i )
{
#ifdef DEBUG
//	if( l<=0 || nN.size()<=l || j<0 || nN[l]<=j || i<0 || nN[l-1]<=i )
	if( l==0 || nN.size()<=l || nN[l]<=j || nN[l-1]<i )
		throw domain_error("Net::idx(Size,Size,Size): zly parametr metody");

	if( nCumW.back() <= ( nCumW[l-1] + j*( nN[l-1]+1 ) + i ) )
		throw out_of_range("Net::idx(Size,Size,Size): zla wartosc zwracana");
#endif

	//       \/ ile wag we wszystkich poprzednich warstwach
	//                     \/ ile wag w poprzedzajacych neuronach w obecnej warstwie
	return ( nCumW[l-1] + j*( nN[l-1]+1 ) + i );
}


//*** calculateCumW ************************************************************
vector<Size>
Net::calculateCumW( const vector<Size>& nN_ )
{
#ifdef DEBUG
	if( nN_.empty() )
		throw invalid_argument("Net::calculateCumW(vector<Size>): Pusty wektor");
#endif

	vector<Size> sumW(nN_.size()); // suma ilosci wag neuronow w konkretnej warstwie
	vector<Size> cumW(nN_.size()); // skumulowana suma ilosci wag

	for( size_t i= 1 ; i<sumW.size() ; ++i ){
		sumW[i] = ( nN_[i-1]+1 ) * nN_[i];
	}

	std::partial_sum( sumW.begin(), sumW.end(), cumW.begin() );

	return (cumW);
}
