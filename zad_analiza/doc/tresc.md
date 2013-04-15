#Analiza sieci neuronowych o różnych struktrach

Zadanie to składa się z trzech części, z których każda prezentuje jeden przykładowy sposób wykorzystania wielowarstwowego perceptronu (MLP):

##1.  Aproksymacja

Stworzyć sieć neuronową (MLP) z jednym wejściem i jednym wyjściem. Sieć powinna mieć jedną warstwę ukrytą z neuronami o sigmoidalnej funkcji aktywacji oraz warstwę wyjściową z neuronem z liniową funkcją aktywacji. Korzystając z poniższych danych treningowych (wszystkie eksperymenty należy przeprowadzić dla obu zbiorów):

* `approximation_train_1.txt`
* `approximation_train_2.txt`

należy nauczyć sieci dla liczby neuronów w wartwie ukrytej od 1 do 20. Należy przetestować sieci z biasem. Nauczona sieć powinna aproksymować funkcję (przybliżać jej wartości) dla danych, które nie były w zbiorze treningowym. W celu sprawdzenia jakości aproksymacji należy każdorazowo skorzystać z poniższego zbioru testowego:

* `approximation_test.txt`
    
Zarówno pliki zawierające zbiory treningowe, jak i plik zawierający dane testowe mają ten sam format - w każdej linii zawarte są wejście i odpowiadającej jej wyjście (oddzielone spacją). Jako ocenę jakości aproksymacji należy rozważyć błąd średniokwadratowy na zbiorze testowym.
    
W sprawozdaniu należy zwrócić uwagę na następujące rzeczy:

* Jak zmienia się błąd średniokwadratowy po każdej epoce nauki na zbiorze treningowym i zbiorze testowym?
* Jaka liczba neuronów w warstwie ukrytej jest potrzebna, aby sieć dokonywała poprawnej aproksymacji?
* Kiedy można uznać, że sieć jest nauczona?
* Jak wpływają parametry nauki (współczynnik nauki i momentum) na szybkość nauki?
* Jak wyglądają wykresy funkcji aproksymowanej przez sieć w porównaniu z rozkładem punktów treningowych?

##2.  Klasyfikacja

Stworzyć sieć neuronową (MPL) o 1, 2, 3 oraz 4 wejściach i 3 wyjściach. Sieć powinna posiadać od 1 do 20 neuronów w jednej warstwie ukrytej. Wszystkie neurony powinny posiadać sigmoidalną funkcję aktywacji. Należy nauczyć sieci z wykorzystaniem poniższych danych:

* classification_train.txt

gdzie w każdej linicje opisany jest jeden obiekt (pierwsze 4 liczby oznaczają cechy tego obiektu) ostatnia liczba oznacza rodzaj obiektu (oddzielone spacją). Za wyjście sieci należy uznać odpowiednio zakodowany rodzaj obiektu: 1 - `(1,0,0)`, 2 - `(0,1,0)`, 3 - `(0,0,1)`. Na wejście sieci należy podawać wybrane cechy obiektu w zależoności od liczby wejść (należy rozważyć 4 sieci z 1 wejściem, 6 sieci z 2 wejściami, 4 sieci z 3 wejściami i 1 sieć z 4 wejściami). Należy przetestować sieci z biasem. Nauczona sieć powinna klasyfikować obiekty (określać prawidłowo ich rodzaj), których nie widziała podczas nauki. W celu sprawdzenia jakości nauczonej sieci należy skorzystać z następujących danych testowych:

* `classification_test.txt`

Format tego pliku jest identyczny z formatem pliku z danymi treningowymi. Jako ocenę jakości należy rozważyć procent poprawnie sklasyfikowanych obiektów (odpowiednio odkodowany rodzaj obiektu) ze zbioru testowego.

W sprawozdaniu należy zwrócić uwagę na następujące rzeczy:

* Jak zmienia się błąd średniokwadratowy oraz procent poprawnie sklasyfikowanych obiektów po każdej epoce nauki na zbiorze treningowym i zbiorze testowym?
* Jaka liczba neuronów w warstwie ukrytej jest potrzebna, aby sieć dokonywała poprawnej klasyfikiacji?
* Kiedy można uznać, że sieć jest nauczona?
* Jak wpływają parametry nauki (współczynnik nauki i momentum) na szybkość nauki?
* Jak wpływa wybór liczby i rodzaju cech obiektów na możlwości nauki sieci?

##3. Transformacja

Stworzyć sieć neuronową (MLP) o 4 wejściach i 4 wyjściach oraz jedną warstwą ukrytą. Należy nauczyć sieć z wykorzystaniem poniższych danych wejściowych:

* `transformation.txt`

zakładając, że na wyjściu sieci te same dane powinny zostać odtworzone. Wszystkie eksperymenty należy powtórzyć dla 1, 2 oraz 3 neuronów w warstwie ukrytej. Wszystkie neurony powinny posiadać sigmoidalną funkcję aktywacji. Należy przetestować sieci z biasem i bez biasu. Format pliku z danymi wejściowymi zawiera kolejne dane wejściowe zawarte w kolejnych liniach (oddzielone spacją).

W sprawozdaniu należy zwrócić uwagę na następujące rzeczy:

* Jak zmienia się błąd średniokwadratowy  po każdej epoce nauki na zbiorze treningowym?
* Jak wpływają parametry nauki (współczynnik nauki i momentum) na szybkość nauki?
* Jak wpływa obecność lub brak obecności biasu na proces nauki?
* Jak można interpretować wyjścia z warstwy ukrytej w tego rodzaju sieci?

Wyniki i wnioski z eksperymentów należy zaprezentować w jak najbardziej przejrzysty, a zarazem zwarty sposób. Sensowny sposób prezentacji wyników uwiarygadniający zamieszczone wnioski będzie jednym z głównych elementów podlegających ocenie.
