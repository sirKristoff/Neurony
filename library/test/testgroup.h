#ifndef TESTGROUP_H
#define TESTGROUP_H

#include <vector>
#include <ostream>
#include <iostream>

using std::vector;
using std::ostream;
using std::cerr;
using std::endl;


#define  GLINE  "*******************************************************************************\n"
#define  TCLINE "-------------------------------------------------------------------------------\n"
#define  REGISTER_TC( TestGroupClass, tcMethod )  { \
	log() << "Adding new test case: "#tcMethod << std::endl; \
	this->addTc(reinterpret_cast<PtrTc>(&TestGroupClass::tcMethod)); }

/**
 * @class TestGroup
 * @brief Abstrakcja grupy test case'ow
 */
class TestGroup
{
public:

	// TODO: dodac nazwy test casow
	typedef  enum{
		trFail,
		trPass
		} TcResult;

	/**
	 * @brief  Wskaznik do metody test case'a
	 */
	typedef TcResult (TestGroup::*PtrTc)(void);

protected:

	TestGroup( ostream& tcstream= cerr )
		: plog( &tcstream )
	{}

	TestGroup( const TestGroup& src )
		: plog(src.plog), mTcs(src.mTcs)
	{}

public:

	TestGroup&
	operator=( const TestGroup& rhs )
	{  plog = rhs.plog;  mTcs = rhs.mTcs;  return (*this);  }

	virtual
	~TestGroup()
	{}


	/**
	 * @brief  Przetestuj grupe
	 * @return Ilosc testow ktore przeszly
	 */
	size_t
	run_group()
	{
		size_t _nPass= 0;
		size_t _nAll= size();
		bool _tcResult;

		log() << GLINE;
		log() << "Test cases in group: " << _nAll << endl;
		for( size_t i= 0 ; i<_nAll ; ++i ){
			log() << TCLINE;
			try{
				_tcResult = (this->*(mTcs[i]))();
			} catch( ... ){  // TODO: obsluga wiadomosci zawartej w wyjatku
				_tcResult = trFail;
			}
			if( _tcResult == trPass ){
				log() << "Test case PASSED" << endl;
				++_nPass;
			}else{
				log() << "Test case FAILED!" << endl;
			}
		}

		log() << TCLINE
			  << "Test group result:" << endl
			  << "\tPassed: " << _nPass << endl
			  << "\tFailed: " << _nAll-_nPass << endl
			  << "\tAll:    " << _nAll << endl;
		return  (_nPass);
	}

	/**
	 * @brief  Rozmiar grupy
	 * @return Ilosc test case'ow w grupie
	 */
	size_t
	size()  const
	{  return  (mTcs.size());  }

	ostream&
	log()
	{  return  (*plog);  }

protected:

	/**
	 * @brief Dodaj test case do grupy
	 * @param tc  Wskaznik do metody test case'a
	 */
	void
	addTc( PtrTc tc )
	{  mTcs.push_back(tc);  }

	vector<PtrTc>::const_iterator
	begin()  const
	{  return  (mTcs.begin());  }

	vector<PtrTc>::const_iterator
	end()  const
	{  return  (mTcs.end());  }

private:

	ostream* plog;
	vector<PtrTc> mTcs;
};



#endif // TESTGROUP_H
