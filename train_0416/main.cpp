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
vector< vector<Vec>::iterator > bliskie_sasiedztwo( vector<Vec>::iterator bliski, const vector<Vec>& punkty, const double g );
void save_data( const vector<Vec>& punkty );
void plot();

const size_t nP= 50;
const size_t dim= 2;
const size_t nMod= 1000;
ostringstream sMod;

const double alfa= 1.0/3.0;
const double g= 0.15;

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

Vec losowy()
{
	Vec v( 2 );
	size_t figura = irand_uni();  // 0 lub 1
	if( figura == 0 ){ // prosta
		v[0]= -4.0;
		v[1] = rand(-10,10);
	}else{
//		Vec v(2);
		double a= rand(0, 2*M_PI);
		double r= rand(0,1);

		v[0]= 5 + r * cos(a);
		v[1] = 0 + r * sin(a);
	}
	return v;
//	return Vec(2, rand(-3.0,3.0));
}


void modyfikuj( vector<Vec>& punkty )
{
	const size_t DIM = punkty.front().size();
	Vec los= losowy();


	vector<Vec>::iterator bliski;
	vector< vector<Vec>::iterator > sasiedzi;

	bliski = najblizszy( punkty, los );
	sasiedzi = bliskie_sasiedztwo( bliski, punkty, g );

	for( size_t i= 0 ; i<DIM ; ++i ){
		(*bliski)[i] += (los[i] - (*bliski)[i]) * alfa;
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

vector< vector<Vec>::iterator >
bliskie_sasiedztwo( vector<Vec>::iterator bliski, const vector<Vec>& punkty, const double gamma )
{
	vector< vector<Vec>::iterator > s;

	for( size_t i= 0 ; i<punkty.size() ; ++i ){
//		if( odl )
	}

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
				"set term gif animate delay 15; "
				"set output 'file.gif'; "
				"do for [i = 0:"+ sMod.str() +":10] { "
				"plot [-10.5:6.5] [-10.5:10.5] "
				"'data.txt'  index i  ps 1 pt 6 notitle, "
				"'linie.txt' with lines notitle lc -1"
				"}"
				"\"";

	system(cmd.c_str());

	system("viewnior file.gif");
}
