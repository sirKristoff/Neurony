#ifndef FUNS_H
#define FUNS_H

#include "definitions.h"


// *** Funkcje wartosci losowych ***********************************************

/**  @return  {min, ..., max}*/
int
irand( int min, int max );

/**  @return  {0,1} */
int
irand_uni();

/**  @return  {-1,0,1} */
int
irand_bi();


/**
 * @brief   Losowy float
 * @param   min   Wartosc minimalna
 * @param   max  Wartosc maksymalna
 * @return  R C (min, max)
 */
Weight
rand( Weight min, Weight max );

/**
 * @brief   Losowy ulamek bez znaku
 * @return  R C (0, 1)
 */
Weight
rand_uni();

/**
 * @brief   Losowy ulamek ze znakiem
 * @return  R C (-1, 1)
 */
Weight
rand_bi();

/**
 * @brief   Losowy ulamek ze znakiem bezwzglednie mniejszy od 1/2
 * @return  R C (-0.5, 0.5)
 */
Weight
rand_bi2();


//*** Funkcje aktywacji ********************************************************

/**
 * @brief   Funkcja identycznosciowa
 * @param   x  R C (-inf, inf)
 * @return  R C (-inf, inf)
 *
 * @codeline f(x) = x
 */
Input ident( Input x );
Input d_ident(Input);

/**
 * @brief   Sigmoida unipolarna
 * @param   x  R C (-inf, inf)
 * @return  R C (0, 1)
 *
 * @codeline f(x) = 1/(1+e^-Bx)
 */
Input uni_sigm( Input x );
Input d_uni_sigm( Input x );


/**
 * @brief   Sigmoida bipolarna
 * @param   x  R C (-inf, inf)
 * @return  R C (-1, 1)
 *
 * @codeline f(x) = (1-e^-B*x)/(1+e^-B*x)
 */
Input bi_sigm( Input x );
Input d_bi_sigm( Input x );


#endif // FUNS_H
