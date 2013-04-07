#ifndef NET_H
#define NET_H

#include <vector>

#include "definitions.h"

using namespace std;


/**
 * @brief Wielowarstwowy perceptron
 *
 * Siec neuronowa posiadajaca @a L (size()) warstw razem z zerowa warstwa kopiujaca.@n
 * Rozmiar wejscia i wyjscia sieci wynosi: sizeIn() oraz sizeOut().@n
 * W kazdej warstwie @a l znajduje sie N^l (sizeN(l)) neuronow, razem w sieci
 * jest sizeN() neuronow.@n
 * Kazda warstwa moze posiadac swoja funkcje aktywacji dla neuronow tej warstwy.@n
 *
 */
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
	 * @brief Stan uczenia sieci
	 * @note  Mozliwe wartosci dla pola fLearningState
	 */
	typedef enum{
		lsLearned = 0,
		lsUnlearned = 1
		} LearningState;


	/**
	 * @brief Net
	 * @param N  Ilosci neuronow w kolejnych warstwach - N.size() == L
	 * @param lockBias  Znacznik zablokowania biasu
	 */
	Net( const vector<Size>& N, LockBias lockBias= lbUnlock );
	/**
	 * @brief Net
	 * @param N    Ilosci neuronow w kolejnych warstwach - N.size() == L
	 * @param fun  Funkcje aktywacji dla kazdej warstwy - fun.size() == L-1
	 * @param dif  Pochodne funkcji aktywacji dla kazdej warstwy
	 * @param lockBias  Znacznik zablokowania biasu
	 */
	Net( const vector<Size>& N, const vector<Fun>& fun, const vector<Dif>& dif,
		 LockBias lockBias= lbUnlock );

	virtual
	~Net();


	/**
	 * @brief Uczenie na jednym przykladzie
	 * @param u  wzorcowe wejscie
	 * @param v  wzorcowe wyjscie
	 * @note  size(u) == this->sizeIn() @n
	 *        size(v) == this->sizeOut()
	 */
	void
	example( const vector<Input>& u, const vector<Input>& v );


	/**
	 * @brief Blad sieci wzgledem wzorca
	 * @param u  wzorcowe wejscie
	 * @param v  wzorcowe wyjscie
	 * @return  Srednie odchylenie wyjscia sieci od wzorcowego wyjscia
	 * @note  size(u) == sizeIn() @n
	 *        size(v) == sizeOut()
	 */
	Input
	e( const vector<Input>& u, const vector<Input>& v );


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
	 * @note l C <1, L),  j C <0, sizeN(l))
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
	 * @note l C <1, L),  j C <0, sizeN(l))
	 */
	Input
	a( const vector<Input>& x, Size l, Size j );

	void
	lockBias()
	{  fLockBias = lbLock;  }

	void
	unlockBias()
	{  fLockBias = lbUnlock;  }

	void
	learnedState();

	void
	unlearnedState();


	/**
	 * @brief   Ilosc warstw sieci
	 * @return  L
	 */
	Size
	size()  const
	{  return  (nN.size());  }

	/**
	 * @brief Ilosc neuronow w warstwie
	 * @param l  Numer warstwy
	 */
	Size
	sizeN( Size l )  const
	{  return  (nN[l]);  }

	/**  @brief  Ilosc neuronow w sieci  */
	Size
	sizeN()  const
	{  return  (nCumN.back());  }

	/**  @brief  Ilosc wag neuronow w sieci  */
	Size
	sizeW()  const
	{  return  (nCumW.back());  }

	/**  @brief  Rozmiar wejscia sieci  */
	Size
	sizeIn()  const
	{  return  (nN.front());  }

	/**  @brief  Rozmiar wyjscia sieci  */
	Size
	sizeOut()  const
	{  return  (nN.back());  }

//protected:

	/**
	 * @brief Indeks i-tej wagi w j-tym neuronie l-tej warstwy
	 * @param l  numer warstwy
	 * @param j  numer neuronu
	 * @param i  numer wagi
	 *
	 * @note l C <1, L),  j C <0, sizeN(l)),  i C <0, sizeN(l-1)>
	 */
	Size
	idxW( Size l, Size j, Size i )  const;

	/**
	 * @brief Indeks j-tego neuronu w l-tej warstwie
	 * @param l  numer warstwy
	 * @param j  numer neuronu w warstwie
	 *
	 * @note l C <1, L),  j C <0, sizeN(l)),
	 */
	Size
	idxN( Size l, Size j )  const;

//private:

	static vector<Size>
	calculateCumW( const vector<Size>& N );

	static vector<Size>
	calculateCumN( const vector<Size>& N );

//protected:

	vector<Size>  nN;     /*!< Ilosci neuronow w kolejnych warstwach - L razem z warstwa zerowa*/
	vector<Size>  nCumN;  /*!< Skumulowane ilosci neuronow we wszystkich poprzedzajacych warstwach */
	vector<Size>  nCumW;  /*!< Sumy ilosci wag w neuronach kumulujac kolejne warstwy */
	Weight*       mW;     /*!< Wagi calej sieci neuronowej (wagi z pierwszych warstw na poczatku */
	Weight*       mDw;    /*!< Wartosci poprzedniej zmiany wag */
	Input*        mA;     /*!< Odpowiedzi wszystkich neuronow sieci */
	vector<Fun>   mFun;   /*!< Funkcje aktywacji dla kazdej warstwy */
	vector<Dif>   mDif;   /*!< Pochodne funkcji aktywacji */

	LockBias fLockBias;   /*!< flaga zablokowana biasu: zablokowany ma wartosc zero i jego waga sie nie zmienia */
	LearningState fLearningState;  /*!< flaga stanu uczenia sie sieci: nauczona siec nie zmienia swoich wag */

	static Weight sNi;     /*!< Wspolczynnik szybkosci zmian wag - sNi C (0,1) */
	static Weight sMi;     /*!< Wspolczynnik momentu - sMi C (0,1) */

private:

	Net( const Net& src );
	Net& operator=( const Net& lhs );
};

#endif // NET_H
