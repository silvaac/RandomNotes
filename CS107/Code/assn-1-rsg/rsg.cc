/**
 * File: rsg.cc
 * ------------
 * Provides the implementation of the full RSG application, which
 * relies on the services of the built-in string, ifstream, vector,
 * and map classes as well as the custom Production and Definition
 * classes provided with the assignment.
 */
 
#include <map>
#include <fstream>
#include "definition.h"
#include "production.h"
using namespace std;

/**
 * Takes a reference to a legitimate infile (one that's been set up
 * to layer over a file) and populates the grammar map with the
 * collection of definitions that are spelled out in the referenced
 * file.  The function is written under the assumption that the
 * referenced data file is really a grammar file that's properly
 * formatted.  You may assume that all grammars are in fact properly
 * formatted.
 *
 * @param infile a valid reference to a flat text file storing the grammar.
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 */

static void readGrammar(ifstream& infile, map<string, Definition>& grammar)
{
  while (true) {
    string uselessText;
    getline(infile, uselessText, '{');
    if (infile.eof()) return;  // true? we encountered EOF before we saw a '{': no more productions!
    infile.putback('{');
    Definition def(infile);
    grammar[def.getNonterminal()] = def;
  }
}

/**
 * Writting basic function that find <> and replaces if it is there
 * This is a test: version that print as it goes.... 
 * Suggested improve: store in vector and then print in the end
 *
 */

void testParse(map<string,Definition>::const_iterator it,map<string,Definition>& grammar)
{

  char tk1[] = "<";
  char tk2[] = ">";
  Production prod = (it->second).getRandomProduction();
  for(Production::iterator curr = prod.begin();curr!=prod.end();++curr){
       string str = *curr;
       char firstLetter = *(str.begin()); // char not string !!
       char lastLetter  = *(str.rbegin()); // char not string !!
       // Is it a word to replace? 
       if(firstLetter==*tk1 && lastLetter==*tk2){
	 // cout<<str<<" ";
	  map<string,Definition>::const_iterator itr = grammar.find(str);
	  if(itr==grammar.end()){
		cerr << str << " was not found. Check input file"<<endl;
		return;
	  }
	  // rep of same code
	  testParse(itr,grammar);
	  //cout<<str<<" ";
	}else{
	  cout<<str<<" ";
	}
  }
 // cout<<endl;
//  return tester;
}


/**
 * Performs the rudimentary error checking needed to confirm that
 * the client provided a grammar file.  It then continues to
 * open the file, read the grammar into a map<string, Definition>,
 * and then print out the total number of Definitions that were read
 * in.  You're to update and decompose the main function to print
 * three randomly generated sentences, as illustrated by the sample
 * application.
 *
 * @param argc the number of tokens making up the command that invoked
 *             the RSG executable.  There must be at least two arguments,
 *             and only the first two are used.
 * @param argv the sequence of tokens making up the command, where each
 *             token is represented as a '\0'-terminated C string.
 */

int main(int argc, char *argv[])
{
  if (argc == 1) {
    cerr << "You need to specify the name of a grammar file." << endl;
    cerr << "Usage: rsg <path to grammar text file>" << endl;
    return 1; // non-zero return value means something bad happened 
  }
  
  ifstream grammarFile(argv[1]);
  if (grammarFile.fail()) {
    cerr << "Failed to open the file named \"" << argv[1] << "\".  Check to ensure the file exists. " << endl;
    return 2; // each bad thing has its own bad return value
  }
  
  // things are looking good...
  map<string, Definition> grammar;
  readGrammar(grammarFile, grammar);
  cout << "The grammar file called \"" << argv[1] << "\" contains "
       << grammar.size() << " definitions." << endl;
 
  // a simple look into the map... getting used to stl again
/*  
  map<string,Definition>::const_iterator it = grammar.begin();
  Production prod;
  for(;it!=grammar.end();++it){
	cout<< it->first <<endl;
        prod = (it->second).getRandomProduction();
	for(Production::iterator curr = prod.begin();curr!=prod.end();++curr){
		cout<<(*curr)<<endl;
	}	
  }
*/

/*
  cout<<"THE FIND CODE --------------------"<<endl;
  it = grammar.find("<start>");
  if(it==grammar.end()){
	cerr << "<start> was not found. Check input file"<<endl;
	return 3;
  }
  prod = (it->second).getRandomProduction();
  for(Production::iterator curr = prod.begin();curr!=prod.end();++curr){
       string str = *curr;
       char firstLetter = *(str.begin()); // char not string !!
       char lastLetter  = *(str.rbegin()); // char not string !!
       char tk1[] = "<";
       char tk2[] = ">";
       // Is it a word to replace? 
       if(firstLetter==*tk1 && lastLetter==*tk2){
	 // cout<<str<<" ";
	  map<string,Definition>::const_iterator itr = grammar.find(str);
	  if(itr==grammar.end()){
		cerr << str << " was not found. Check input file"<<endl;
		return 4;
	  }
	  Production prod2 = (itr->second).getRandomProduction();
	  for(Production::iterator curr2=prod2.begin();curr2!=prod2.end();++curr2){
	  	cout<<*curr2<<" ";
	  }
	}else{
	  cout<<str<<" ";
	}
  }
  cout<<endl;
*/

  cout<<"THE FUNCTION VERSION --------------------"<<endl;
// USING RECURSION!!!!	
  testParse(grammar.find("<start>"),grammar);
  cout << endl;
  return 0;
}
