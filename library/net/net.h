#ifndef NET_H
#define NET_H

#include <vector>

#include "definitions.h"

using namespace std;


class Net
{
public:

	/**
	 * @brief Opcja zablokowania biasu
	 * @note  Mozliwe wartosci dla pola fLockBias
	 */
	typedef enum {
		lbLock = 0,
		lbUnlock = 1
		} LockBias;

	/**
	 * @brief Net
	 * @param N  Ilosci neuronow w kolejnych warstwach - N.size() == L+1
	 * @param lockBias  Znacznik zablokowania biasu
	 */
	Net( const vector<Size>& N, LockBias lockBias= lbUnlock );
	/**
	 * @brief Net
	 * @param N    Ilosci neuronow w kolejnych warstwach - N.size() == L+1
	 * @param fun  Funkcje aktywacji dla kazdej warstwy - fun.size() == L
	 * @param dif  Pochodne funkcji aktywacji dla kazdej warstwy
	 * @param lockBias  Znacznik zablokowania biasu
	 */
	Net( const vector<Size>& N, const vector<Fun>& fun, const vector<Dif>& dif,
		 LockBias lockBias= lbUnlock );
	~Net();


	/**
	 * @brief Wyjscie sieci neuronowej
	 * @param x  wejscie pierwszej warstwy
	 * @return Poziom aktywacji wszystkich neuronow w warstwie wyjsciowej
	 */
	vector<Input>
	y( const vector<Input>& x );

	/**
	 * @brief Wyjscie l-tej warstwy, ktore jest wejsciem dla warstwy l+1
	 * @param x  wejscie warstwy
	 * @param l  numer warstwy sieci
	 * @return Poziom aktywacji wszystkich neuronow w warstwie
	 * @note l C <1, L)
	 */
	vector<Input>
	y( const vector<Input>& x, Size l );

	/**
	 * @brief Wyjscie j-tego neuronu w l-tej warstwie
	 * @param x  wejscie neuronu
	 * @param l  numer warstwy sieci
	 * @param j  numer neuronu w warstwie
	 * @return Poziom aktywacji neuronu
	 * @note l C <1, L),  j C <0, nN[l])
	 */
	Input
	y( const vector<Input>& x, Size l, Size j );


	/**
	 * @brief Odpowiedz l-tej warstwy, ktore jest wejsciem dla funkcji aktywacji warstwy l+1
	 * @param x  wejscie warstwy
	 * @param l  numer warstwy sieci
	 * @return Odpowiedzi wszystkich neuronow w warstwie
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

	LockBias fLockBias;       /*!< flaga zablokowana biasu: zablokowany ma wartosc zero i jego waga sie nie zmienia */

};

#endif // NET_H
