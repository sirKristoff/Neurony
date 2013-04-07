#include <iostream>
#include <string>
#include <fstream>

#include "../library/net/definitions.h"
#include "../library/net/funs.h"
#include "../library/net/net.h"

using namespace std;


//*****************************************************************************

#define FOR_EACH(i,n,what) \
	for(unsigned int i= 0 ; i<(n) ; ++i){ \
		what; }

//*****************************************************************************

string namIn=      "in.txt";           // wejscie sieci jednoczesnie wyjscie
string namNetPre=  "network_pre.txt";  // siec przed nauka
string namOutPre=  "out.txt";          // wyjscie z sieci przed nauka
string namNetPost= "network.txt";      // siec po nauce
string namTrain=   "train.png";        // przebieg nauki

//*****************************************************************************
/** in.txt
1 0 0 0
0 1 0 0
0 0 1 0
0 0 0 1

*/
/** network_pre.txt
2
4
2
4
0.000000 0.472801 0.935628 -0.491067 0.372298
0.000000 -0.100727 0.297812 -0.615205 -0.153709
0.000000 0.346888 0.484956
0.000000 0.654139 -0.961072
0.000000 0.872161 0.603206
0.000000 0.665244 -0.489092

*/
//*****************************************************************************

int main()
{
	ifstream fIn;
	ifstream fNetPre;
	ofstream fOutPre;
	ofstream fNetPost;

	fIn.open(namIn.c_str());
	fNetPre.open(namNetPre.c_str());
	fOutPre.open(namOutPre.c_str());
	fNetPost.open(namNetPost.c_str());




	fIn.close();
	fNetPre.close();
	fOutPre.close();
	fNetPost.close();

	return 0;
}

