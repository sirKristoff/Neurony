#include <math.h>
#include <stdlib.h>

#include "funs.h"


// *** Funkcje wartosci losowych ***********************************************

/**  @return  {min, ..., max}*/
int
irand( int min, int max )
{
	return  ::rand()%(max+1-min) + min;
}

/**  @return  {0,1} */
int
irand_uni()
{
	static unsigned short ofset;
	ofset = ::rand() % 16;

	return  (::rand() & (0x01<<ofset)) >> ofset;
}

/**  @return  {-1,0,1} */
int
irand_bi()
{
	return  ::rand()%3 -1;
}


/**
 * @brief   Losowy float
 * @param   min   Wartosc minimalna
 * @param   max  Wartosc maksymalna
 * @return  R C (min, max)
 */
Weight
rand( Weight min, Weight max )
{
	return  Weight(1+::rand()%(RAND_MAX-1))/RAND_MAX  * (max-min) + min;
}

/**
 * @brief   Losowy ulamek bez znaku
 * @return  R C (0, 1)
 */
Weight
rand_uni()
{
	return  rand( 0.0, 1.0 );
}

/**
 * @brief   Losowy ulamek ze znakiem
 * @return  R C (-1, 1)
 */
Weight
rand_bi()
{
	return  rand( -1.0, 1.0 );
}

/**
 * @brief   Losowy ulamek ze znakiem bezwzglednie mniejszy od 1/2
 * @return  R C (-0.5, 0.5)
 */
Weight
rand_bi2()
{
	return  rand( -0.5, 0.5 );
}



//*** Funkcje aktywacji ********************************************************

//  parametr funkcji sigmoidalnych B C (0, 1>
const Input kBeta= 1.0;

Input ident( Input x )
{
	return (x);
}

Input d_ident( Input )
{
	return Input(1.0);
}


Input uni_sigm( Input x )
{
	return 1.0/(Input(1.0)+exp(-kBeta*x));
}

Input d_uni_sigm( Input x )
{
	const Input sigm= uni_sigm(x);
	return kBeta * sigm * (1.0 - sigm);
}

Input bi_sigm( Input x )
{
	return 2.0/(Input(1.0)+exp(-kBeta*x))-1.0;
//	return (1-exp(-kBeta*x))/(1+exp(-kBeta*x));
}

Input d_bi_sigm( Input x )
{
	const Input sigm= bi_sigm(x);
	return 0.5*kBeta * (1.0 - sigm*sigm);
}
