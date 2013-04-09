#include <iostream>
#include <vector>
#include <stdlib.h>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <fstream>

#include "../library/net/funs.h"

using namespace std;

typedef vector<double> Vec;

#define FOR_EACH(i,n,what) \
	for(unsigned int i= 0 ; i<(n) ; ++i){ \
		what; }

void printVec( const Vec& w )
{
	copy( w.begin(), w.end(), ostream_iterator<double>(cout, " ") );
}

void printPunkty( const vector<Vec>& punkty )
{
	cout << "Wszystkie punkty:" << endl;
	FOR_EACH( i, punkty.size(),
			  printVec( punkty[i] );
			cout << endl; );
}


double odl( const Vec& x, const Vec& l );
void modyfikuj( vector<Vec>& punkty );
vector<Vec>::iterator najblizszy( vector<Vec>& punkty, Vec los );
void save_data( const vector<Vec>& punkty );
void plot();

const size_t nP= 100;
const size_t dim= 2;
const size_t nMod= 10000;
ostringstream sMod;

int main()
{
	sMod << nMod-1;
	vector<Vec> punkty;

	for( size_t i= 0 ; i<nP ; ++i ){
		punkty.push_back( Vec(dim,0) );
		for( size_t d= 0 ; d<dim ; ++d )
			punkty.back()[d] = rand_bi();
	}


//	printPunkty(punkty);
//	cout << endl << endl;

	ofstream fData("data.txt");
	fData.close();

	for( size_t m= 0 ; m<nMod ; ++m ){
		modyfikuj(punkty);
//		printPunkty(punkty);
//		cout << endl << endl;

		save_data(punkty);

	}


	plot();

	return 0;
}

// *****************************************************************************

double odl( const Vec& x, const Vec& l )
{
	double o= 0.0;

	for( size_t i= 0 ; i<x.size() ; ++i )
		o += (x[i]-l[i])*(x[i]-l[i]);
	return sqrt(o);
}

void modyfikuj( vector<Vec>& punkty )
{
	const size_t DIM = punkty.front().size();
	Vec los( DIM, rand(-3.0,3.0) );

//	cout << "Losowy wektor:" << endl;
//	printVec( los );
//	cout << endl;

	vector<Vec>::iterator bliski;

	bliski = najblizszy( punkty, los );

	for( size_t i= 0 ; i<DIM ; ++i ){
		(*bliski)[i] += (los[i] - (*bliski)[i]) /3.0;
	}

	return;
}



vector<Vec>::iterator najblizszy( vector<Vec>& punkty, Vec los )
{
	double odleglosc= odl( punkty.front(), los );
	double tmpO;

	vector<Vec>::iterator it= punkty.begin();

	for( size_t i= 1 ; i<punkty.size() ; ++i ){
		tmpO = odl( punkty[i], los );
		if(  tmpO < odleglosc  ){
			odleglosc = tmpO;
			it = punkty.begin() + i;
		}
	}

	return it;
}



void save_data( const vector<Vec>& punkty )
{

	ofstream fData;
	fData.open("data.txt", ios_base::app);

	for( size_t j= 0 ; j<punkty.size() ; ++j ){
		for( size_t i= 0 ; i<punkty[j].size() ; ++i ){
			fData << punkty[j][i] << " ";
		}
		fData << endl;
	}
	fData << endl << endl;


	fData.close();
}

void plot()
{
	string cmd= string("") +
				"gnuplot -persist -e \""
				"set term gif animate delay 50; "
				"set output 'file.gif'; "
				"do for [i = 0:"+ sMod.str() +":10] { "
				"plot [-3:3] [-3:3] "
				"'data.txt'  index i  ps 1 pt 6 notitle, "
				"'linie.txt' with lines notitle lc -1"
				"}"
				"\"";

	system(cmd.c_str());

	system("viewnior file.gif");
}
