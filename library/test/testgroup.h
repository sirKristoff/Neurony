#ifndef TESTGROUP_H
#define TESTGROUP_H

#include <exception>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;


#define  GLINE  "*******************************************************************************\n"
#define  TCLINE "-------------------------------------------------------------------------------\n"
#define  REGISTER_TC( TestGroupClass, tcMethod )  { \
	log() << "Adding new test case: "#tcMethod << std::endl; \
	this->addTc(string(#tcMethod), reinterpret_cast<PtrTc>(&TestGroupClass::tcMethod)); }


/**
 * @class TestGroup
 * @brief Abstrakcja grupy test case'ow
 */
class TestGroup
{
public:

	typedef  enum{
		trFail,
		trPass
		} TcResult;

	/**
	 * @brief  Wskaznik do metody test case'a
	 */
	typedef TcResult (TestGroup::*PtrTc)(void);

protected:

	TestGroup( const string& tgName, ostream& logStream= cout, ostream& errStream= cerr )
		: plog(&logStream), perr(&errStream), mTestGroupName(tgName)
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

		log() << GLINE
			  << "\tTest Group: \"" << mTestGroupName << "\"" << endl;
		for( size_t i= 0 ; i<_nAll ; ++i ){
			log() << TCLINE
				  << "\tTest Case: \"" << mTcNames[i] << "\"" << endl;
			try{
				_tcResult = (this->*(mTcs[i]))();
			} catch( const exception& e ){
				reason( e.what() );
				_tcResult = trFail;
			} catch( ... ){
				reason("Unexpected exception caught!");
				_tcResult = trFail;
			}
			if( _tcResult == trPass ){
				log() << "Test case PASSED" << endl;
				++_nPass;
			}else{
				err() << "Test case FAILED!" << endl
					  << "Reason is: \"" << reason() << endl;
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
	log()  const
	{  return  (*plog);  }

	ostream&
	err()  const
	{  return  (*perr);  }

	void
	reason(const string& reasonOfFailure )
	{  mReason = reasonOfFailure;  }

	const string&
	reason()  const
	{  return  (mReason);  }

protected:

	/**
	 * @brief Dodaj test case do grupy
	 * @param tc  Wskaznik do metody test case'a
	 */
	void
	addTc( const string& tcName, PtrTc tc )
	{  mTcNames.push_back(tcName);  mTcs.push_back(tc);  }

	vector<PtrTc>::const_iterator
	begin()  const
	{  return  (mTcs.begin());  }

	vector<PtrTc>::const_iterator
	end()  const
	{  return  (mTcs.end());  }

private:

	ostream* plog;   // strumien logow
	ostream* perr;   // strumien bledow
	string mReason;  // powod z jakiego test case nie przeszedl
	string mTestGroupName;   // nazwa grupy testowej
	vector<PtrTc> mTcs;      // lista test case'ow
	vector<string> mTcNames; // lista nazw test case'ow
};



#endif // TESTGROUP_H
