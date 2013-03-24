#ifndef NET_H
#define NET_H

#include <vector>

#include "definitions.h"

using namespace std;


class Net
{
public:

	/**
	 * @brief Net
	 * @param N  Ilosci neuronow w kolejnych warstwach - N.size() == L+1
	 */
	Net( const vector<Size>& N );
	/**
	 * @brief Net
	 * @param N    Ilosci neuronow w kolejnych warstwach - N.size() == L+1
	 * @param fun  Funkcje aktywacji dla kazdej warstwy - fun.size() == L
	 * @param dif  Pochodne funkcji aktywacji dla kazdej warstwy
	 */
	Net( const vector<Size>& N, const vector<Fun>& fun, const vector<Dif>& dif );
	~Net();


	/**
	 * @brief Odpowiedz sieci neuronowej
	 * @param x  wejscie pierwszej warstwy
	 * @return Poziom aktywacji wszystkich neuronow w warstwie wyjsciowej
	 */
	vector<Input>
	y( const vector<Input>& x );

	/**
	 * @brief Odpowiedz l-tej warstwy, ktore jest wejsciem dla warstwy l+1
	 * @param x  wejscie warstwy
	 * @param l  numer warstwy sieci
	 * @return Poziom aktywacji wszystkich neuronow w warstwie
	 * @note l C <1, L)
	 */
	vector<Input>
	y( const vector<Input>& x, Size l );

	/**
	 * @brief Odpowiedz j-tego neuronu w l-tej warstwie
	 * @param x  wejscie neuronu
	 * @param l  numer warstwy sieci
	 * @param j  numer neuronu w warstwie
	 * @return Poziom aktywacji neuronu
	 * @note l C <1, L),  j C <0, nN[l])
	 */
	Input
	y( const vector<Input>& x, Size l, Size j );


	/**
	 * @brief Wyjscie l-tej warstwy, ktore jest wejsciem dla warstwy l+1
	 * @param x  wejscie warstwy
	 * @param l  numer warstwy sieci
	 * @return Wyjscia wszystkich neuronow w warstwie
	 * @note l C <1, L)
	 */
	vector<Input>
	a( const vector<Input>& x, Size l );

	/**
	 * @brief Iloczyn wag i wejscia j-tego neuronu w l-tej warstwie
	 * @param x  wejscie neuronu
	 * @param l  numer warstwy sieci
	 * @param j  numer neuronu w warstwie
	 * @return Iloczyn wag neuronu i jego wejscia
	 * @note l C <1, L),  j C <0, nN[l])
	 */
	Input
	a( const vector<Input>& x, Size l, Size j );

//protected:

	/**
	 * @brief Indeks i-tej wagi w j-tym neuronie l-tej warstwy
	 * @param l  numer warstwy
	 * @param j  numer neuronu
	 * @param i  numer wagi
	 *
	 * @note l C <1, L),  j C <0, nN[l]),  i C <0, nN[l-1]>
	 */
	Size
	idx( Size l, Size j, Size i );

//private:

	static vector<Size>
	calculateCumW( const vector<Size>& nN_ );

//protected:

	vector<Size>  nN;     /*!< Ilosci neuronow w kolejnych warstwach - L razem z warstwa zerowa*/
	vector<Size>  nCumW;  /*!< Sumy ilosci wag w neuronach kumulujac kolejne warstwy */
	Weight*  mW;          /*!< Wagi calej sieci neuronowej (wagi z pierwszych warstw na poczatku */
	vector<Fun> mFun;     /*!< Funkcje aktywacji dla kazdej warstwy */
	vector<Dif> mDif;     /*!< Pochodne funkcji aktywacji */
};

#endif // NET_H
