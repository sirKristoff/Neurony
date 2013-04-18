/**
 * @file     getopt.h
 * @author   Krzysztof Lasota
 * @date     2013-04-17
 * @version  2.0 alfa
 */

#ifndef GETOPT_H
#define GETOPT_H

#include <ctype.h>
#include <sstream>
#include <string>
#include <vector>


template< typename T >
std::istream&  extractValue( std::istream& stream, T& variable )
{  return  stream >> variable;  }
template<>
std::istream&  extractValue<std::string>( std::istream& stream, std::string& variable )
{  return  std::getline(stream, variable);  }



/**
 * @brief Klasa do wyluskiwania flag z argumentow wejsciowych programu
 *
 * Przyjmuje podczas konstruowania argumenty funkcji @a main.@n
 * Dostep do argumentow umozliwia operator tablicowy.@n
 * Mozliwe jest wyszukanie argumentu zawierajacego flage (find())
 * oraz uzyskanie wartosci skojarzonej z flaga (valueOf()).
 */
class GetOpt
{
protected:

	typedef std::vector<std::string> Args;

	typedef enum{
		ftUndefined = 0,
		ftNone      = 1,
		ftBasic     = 1<<1,
		ftExtended  = 1<<2,
		ftCombined  = 1<<3,
		ftValued    = 1<<4,

		ftBasicCombined  = ftBasic | ftCombined,
		ftExtendedValued = ftExtended | ftValued
		}  FlagType;

public:

	GetOpt( int argc, char *argv[] )
		: args_( argv, argv+argc )
	{}


	const std::string&
	operator[]( size_t idx )  const
	{  return  (args_[idx]);  }

	size_t
	size()  const
	{  return  (args_.size());  }


	/**
	 * @brief  Odnajdz flage w argumentach wejsciowych
	 * @param  flag  Flaga do odnalezienia
	 * @return Numer argumentu w ktorym jest flaga
	 */
	size_t
	find( const std::string& flag )  const
	{
		// szukamy w kazdym argumencie
		for( Args::const_iterator itArg= args_.begin()+1 ;
			 itArg != args_.end() ; ++itArg ){

			switch (typeOfFlag(flag)) {
				// szukamy flagi '-x'
				case ftBasic:

					if(  typeOfArg(*itArg) & ftBasic  &&  // argument tego samego typu
						 itArg->find(flag) != std::string::npos  )
						return (itArg-args_.begin());
					break;

				// szukamy flagi '--xxx-x
				case ftExtended:

					if(  typeOfArg(*itArg) & ftExtended  &&  // argument tego samego typu
						 itArg->find( "--"+flag ) == 0  &&
						 (  flag.size()+2 == itArg->size()  ||  // argument zawiera tylko flage
							(*itArg)[flag.size()+2] == '='  )  )  // argument jest flaga z wartoscia
						return (itArg-args_.begin());
					break;

				default:
					return (0);  // EXC: zly format flagi
			}

		}
		// nie ma argumentu z flaga
		return (0);
	}


	std::string
	valueOf( const std::string& flag )  const
	{
		Args::const_iterator itArg;
		FlagType type;

		if(  (itArg = args_.begin() + this->find(flag)) == args_.end()  )
			return  ("");  // EXC: nie znaleziono takiej flagi w argumentach

		type = typeOfArg(*itArg);
		if(  type & ftBasic  &&  !(type & ftCombined)  ){
			++itArg;
			if(  itArg != args_.end()  )
				return  (*itArg);
			else
				return  ("");  // EXC: szukana flaga nie ma wartosci
		}
		else if(  type & ftExtendedValued  ){
			return  itArg->substr( itArg->find('=')+1 );
		}

		return  ("");  // EXC: szukana flaga nie ma wartosci
	}


	/**
	 * @brief   Przypisz wartosc powiazana z ktorakolwiek z flag
	 * @param   variable  Zmienna do zapisania wartosci
	 * @param   pattern   Wzorce flag do odnalezienia
	 * @returns true   odnaleziono ktorykolwiek ze wzorcow
	 * @returns false  nie odnaleziono zadnej flagi pasujacej do wzorca
	 */
	template< typename T >
	bool
	fetchValue( T& variable, std::string pattern )  const
	{
		std::vector<std::string> ptts;
		std::stringstream sstr;

		size_t pos;                  // pozycja separatora wzorcow
		while( !pattern.empty() ){   // az wszystkie wzorce zostana odzseparowane
			pos= pattern.find_last_of("|");      // gdzie jest ostatni separator
			if( pos != std::string::npos ){      // sa conajmniej dwa wzorce
				ptts.insert( ptts.begin(), pattern.substr(pos+1) );
			}
			else{                                      // ostatni wzorzec
				ptts.insert( ptts.begin(), pattern );
				pos = 0;
			}
			pattern.erase(pos);
		}


		FlagType ft= ftNone;
		// szukaj pierwszego wzorca dla kturego sie cos znajdzie
		for( size_t ip= 0 ; ip<ptts.size() ; ++ip ){
			if( ptts[ip].find('=') != std::string::npos ){
				ft = ftValued;  // szukamy wartosci flagi a nie tylko jej wystepowania
				ptts[ip].erase( ptts[ip].end()-1 );
			}

			if( this->find(ptts[ip]) != 0 ){      // odnaleziono flage
				if( ft & ftValued )
					sstr << this->valueOf(ptts[ip]);
				else
					sstr << true;

				extractValue(sstr, variable);        // przypisz wartosc flagi do 'variable'

				// w przypadkach niejawnych konwersji (double->int) bedzie fail
//				if( !sstr.fail() )
					return  (true);
//				else
//					break;
			}
		}

		return  (false);
	}

protected:

	/**
	 * @brief   Okreslenie jakiego typu flaga moze byc argument
	 * @param   arg  Argument programu
	 * @return  Typ flagi jakim moze byc argument
	 */
	static
	FlagType
	typeOfArg( const std::string& arg )
	{
		FlagType type= ftUndefined;

		// flagi zaczynaja sie '-'
		if(  arg[0] == '-'  ){
			// flaga typu: '--xxx-x'
			if(  arg[1] == '-'  &&  isalnum(arg[2])  ){
				type = FlagType(type | ftExtended);
				// flaga z wartoscia
				if( arg.find('=') != std::string::npos )
					type = FlagType(type | ftValued);
			}
			// flaga typu: '-x'
			else if(  isalnum(arg[1])  ){
				type = FlagType(type | ftBasic);
				// argument moze zawierac wiele flag
				if(  2 < arg.size()  )
					type = FlagType(type | ftCombined );
			}
		}
		else
			// argument nie jest flaga
			type = ftNone;

		return  (type);
	}

	/**
	 * @brief  Okreslenie typu flagi
	 * @param  flag  Flaga
	 * @return Typ flagi
	 */
	static
	FlagType
	typeOfFlag( const std::string& flag )
	{
		if(  flag.size() == 1  &&  isalnum(flag[0])  )
			return  (ftBasic);
		else if(  1 < flag.size()  && isalnum(flag[0])  )
			return  (ftExtended);

		return  (ftNone);
	}

protected:

	std::vector<std::string> args_;

};

#endif // GETOPT_H
