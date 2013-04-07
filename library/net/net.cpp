#include <algorithm>
#include <numeric>
#include <stdlib.h>
#ifdef DEBUG
 #include <stdexcept>
#endif

using namespace std;

#include "funs.h"
#include "net.h"


Weight Net::sNi= 0.1;
Weight Net::sMi= 0.3;


namespace my{
Weight rand()
{
//	return irand_bi();
	return rand_bi2();  // (-0.5, 0.5)
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
	: nN(N), mDw(NULL), mA(NULL), mFun(N.size(), ::ident), mDif(N.size(), ::d_ident),
	  fLockBias(lockBias), fLearningState(lsLearned)
{
	nCumN = Net::calculateCumN(N);
	nCumW = Net::calculateCumW(N);

	mW = new Weight[sizeW()];
	std::generate(&mW[0], &mW[sizeW()], my::rand);

//	this->unlearnedState();  // zarezerwuj pamiec dla mA

	mFun[0] = NULL;
	mDif[0] = NULL;
}

//*** Net **********************************************************************
Net::Net(const vector<Size>& N , const vector<Fun> &fun, const vector<Dif> &dif,
		 LockBias lockBias)
	: nN(N), mDw(NULL), mA(NULL), mFun(fun), mDif(dif), fLockBias(lockBias),
	  fLearningState(lsLearned)
{
#ifdef DEBUG
	if( N.size() != fun.size()+1  ||  N.size() != dif.size()+1 )
		throw range_error("Net::Net(vector<Size>,vector<Fun>,vector<Dif>): Zla ilosc funkcji aktywacji");
#endif

	nCumN = Net::calculateCumN(N);
	nCumW = Net::calculateCumW(N);
	mW = new Weight[sizeW()];
	std::generate(&mW[0], &mW[sizeW()], my::rand);

//	this->unlearnedState();  // zarezerwuj pamiec dla mA

	mFun.insert(mFun.begin(),NULL);  // mFun nie ma funkcji pod indeksem 0
	mDif.insert(mDif.begin(),NULL);
}

//*** ~Net *********************************************************************
Net::~Net()
{
	delete[] mW;
	this->learnedState();  // ewentualnie zwolni pamiec mA, mDw
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
	if( u.size() != sizeIn() )
		throw range_error("Net::example(vector<Input>,vector<Input>): "
						  "Rozmiar wzorcowego wejscia nie zgadza sie z wymiarem sieci");
	if( v.size() != sizeOut() )
		throw range_error("Net::example(vector<Input>,vector<Input>): "
						  "Rozmiar wzorcowego wyjscia nie zgadza sie z wymiarem sieci");
#endif

	const Size L= this->size()-1; // numer ostatniej warstwy sieci
	vector<Input>  B( sizeN(), Input(0) ); // wartosci b^l_j dla kazdego neuronu

	this->unlearnedState();  // wlaczamy nauke
	vector<Input> y_ = this->y(u);  // obliczamy odpowiedzi neuronow

	Size iA;  // globalny indeks neuronu

	// dla ostatniej warstwy L
	for( Size j= 0 ; j<sizeN(L) ; ++j ){
		iA = idxN(L,j);  // indeks wyjscia neuronu
		B[iA] = ( y_[j] - v[j] ) * (mDif[L])( mA[iA] );
	}

	Input sumBW;
	// dla warstw od L-1 do 1
	for( Size l= L-1 ; 0<l ; --l ){
		// dla kazdego neuronu w warstwie
		for( Size j= 0 ; j<sizeN(l) ; ++j ){
			iA = idxN(l,j);  // indeks wyjscia neuronu

			sumBW = Input(0);
			// wszystkie neurony z warstwy wyzszej
			for( Size jj= 0 ; jj<sizeN(l+1) ; ++jj )  //      \/ waga 1 polaczona z wejsciem 0
				sumBW += B[idxN(l+1,jj)] * mW[idxW(l+1,jj,j+1)];
			B[iA] = sumBW * (mDif[l])( mA[iA] );
		}
	}

	// korekcja wag
	Input dB;
	vector<Input> in= u;
	vector<Input> out;
	Size iW;   // globalny indeks wagi

	// TODO: zachowac wartosci dW

	// dla warstw od 1 do L
	for( Size l = 1 ; l<=L ; ++l ){  // kazda warstwa
		out = this->y(in,l);  // wejscie dla nastepnej warstwy

		for( Size j= 0 ; j<sizeN(l) ; ++j ){  // kazdy neuron
			dB= - Net::sNi * B[idxN(l,j)];  // b^l_j

			iW = idxW(l,j,0);
			/** @warning  zmiana flagi fLockBias w trakcie uczenia
			 *            moze spowodowac niezdefiniowane zachowanie */
			mDw[iW] =  dB * X0  +  Net::sMi * mDw[iW];  // zmiana wagi biasu
			mW[idxW(l,j,0)] += mDw[iW];

			for( Size i= 1 ; i<=sizeN(l-1) ; ++i ){  // kazda waga
				iW = idxW(l,j,i);
				mDw[iW] =  dB * in[i-1]  +  Net::sMi * mDw[iW];
				mW[idxW(l,j,i)] += mDw[iW];
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
 * @note  size(u) == sizeIn() @n
 *        size(v) == sizeOut()
 */
Input
Net::e( const vector<Input>& u, const vector<Input>& v )
{
#ifdef DEBUG
	if( u.size() != this->sizeIn() )
		throw range_error("Net::e(vector<Input>,vector<Input>): Dlugosc wektora u nie zgadza sie z rozmiarem wejscia sieci");
	if( v.size() != this->sizeOut() )
		throw range_error("Net::e(vector<Input>,vector<Input>): Dlugosc wektora v nie zgadza sie z rozmiarem wyjscia sieci");
#endif

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
	vector<Input> out(sizeN(l), Input());

	// TODO: wykonac obliczenia dla calej warstwy bez funkcji a(x,l,j)
	// TODO: ograniczyc ilosc wywolan funkcji wyliczajacych indeksy
	for( size_t j= 0 ; j<sizeN(l) ; ++j)
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
 * @note l C <1, L),  j C <0, sizeN(l))
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
	if( l==0 || size()<=l )
		throw out_of_range("Net::a(vector<Input>,Size): Zly numer warstwy");
	if( x.size() != sizeN(l-1) )
		throw range_error("Net::a(vector<Input>,Size): Dlugosc wektora nie zgadza sie z iloscia wejsc warstwy");
#endif

	vector<Input> out(nN[l], Input());

	// TODO: wykonac obliczenia dla calej warstwy bez funkcji a(x,l,j)
	// TODO: ograniczyc ilosc wywolan funkcji wyliczajacych indeksy
	for( size_t j= 0 ; j<sizeN(l) ; ++j)
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
 * @note l C <1, L),  j C <0, sizeN(l))
 */
Input
Net::a( const vector<Input> &x, Size l, Size j )
{
#ifdef DEBUG
	if( l==0 || size()<=l )
		throw out_of_range("Net::a(vector<Input>,Size,Size): Zly numer warstwy");
	if( sizeN(l)<=j )
		throw out_of_range("Net::a(vector<Input>,Size,Size): Zly numer neuronu");
	if( x.size() != sizeN(l-1) )
		throw range_error("Net::a(vector<Input>,Size,Size): Dlugosc wektora nie zgadza sie z iloscia wejsc neuronu");
#endif

	const Size index0= idxW(l,j,0);  // indeks wagi biasu w tym neuronie
	Input ret= inner_product( &mW[index0+1], &mW[index0+1+sizeN(l-1)], x.begin(),
			W0j(index0) );  // iloczyn skalarny wejscia z wagami

	if( fLearningState == lsUnlearned )  // siec w trakcie nauki
		mA[ idxN(l,j) ] = ret;

	return (ret);
}


//*** learnedState *************************************************************
void
Net::learnedState()
{
#ifdef DEBUG
	if( fLearningState == lsUnlearned  &&  mA == NULL )
		throw logic_error("Net::learnedState(): siec nie nauczona a pamiec dla 'A' zwolniona!");
	if( fLearningState == lsUnlearned  &&  mDw == NULL )
		throw logic_error("Net::learnedState(): siec nie nauczona a pamiec dla 'Dw' zwolniona!");
#endif

	if( fLearningState == lsUnlearned ){
		delete[] mA;
		delete[] mDw;
		fLearningState = lsLearned;
#ifdef DEBUG
		mA = NULL;
		mDw = NULL;
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
	if( fLearningState == lsLearned  &&  mDw != NULL )
		throw logic_error("Net::unlearnedState(): siec nauczona a pamiec dla 'Dw' zarezerwowana!");
#endif

	if( fLearningState == lsLearned ){
		mA = new Input[sizeN()];    // dla kazdego neuronu jego odpowiedz
		mDw = new Weight[sizeW()];  // dla kazdej wagi wartosc jej poprzedniej zmiany
#ifdef DEBUG
		std::fill( mDw, mDw+sizeW(), Weight(0) );
#endif
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
 * @note l C <1, L),  j C <0, sizeN(l)),  i C <0, sizeN(l-1)>
 */
Size
Net::idxW( Size l, Size j, Size i )  const
{
#ifdef DEBUG
//	if( l<=0 || nN.size()<=l || j<0 || nN[l]<=j || i<0 || nN[l-1]<=i )
	if( l==0 || size()<=l || sizeN(l)<=j || sizeN(l-1)<i )
		throw domain_error("Net::idxW(Size,Size,Size): zly parametr metody");

	if( sizeW() <= ( nCumW[l-1] + j*( nN[l-1]+1 ) + i ) )
		throw out_of_range("Net::idxW(Size,Size,Size): zla wartosc zwracana");
#endif

	//       \/ ile wag we wszystkich poprzednich warstwach
	//                     \/ ile wag w poprzedzajacych neuronach w obecnej warstwie
	return ( nCumW[l-1] + j*( nN[l-1]+1 ) + i );
}

//*** idxN *********************************************************************
/**
 * @brief Indeks j-tego neuronu w l-tej warstwie
 * @param l  numer warstwy
 * @param j  numer neuronu w warstwie
 *
 * @note l C <1, L),  j C <0, sizeN(l)),
 */
Size
Net::idxN( Size l, Size j )  const
{
#ifdef DEBUG
	if( l==0 || size()<=l || sizeN(l)<=j )
		throw domain_error("Net::idxN(Size,Size): zly parametr metody");

	if( sizeN() <= (nCumN[l-1] + j) )
		throw out_of_range("Net::idxN(Size,Size): zla wartosc zwracana");
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


#undef  X0
#undef  W0j
