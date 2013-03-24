#ifndef DEFINITIONS_H
#define DEFINITIONS_H


typedef  double        Weight;  // wagi sieci neuronowej
typedef  double        Input;   // wejscia sieci neuronowej
typedef  Input         Output;  // wyjscie sieci neuronowej
typedef  unsigned int  Size;    // rozmiar neuronow
typedef  Input (*Fun)(Input);   // funkcja aktywacji
typedef  Input (*Dif)(Input);   // pochodna funkcji aktywacji

#endif // DEFINITIONS_H
