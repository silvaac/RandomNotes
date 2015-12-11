#include <iostream>
#include <iomanip> // for setw formatter
#include <map>
#include <set>
#include <string>
#include "imdb.h"
using namespace std;

/**
 * Function: stall
 * ---------------
 * Interrupts code execution until the user hits
 * the return key.  Self-explanatory implementation.
 */

static void stall()
{
  string dummy;
  cout << "[Press enter to continue]";
  getline(cin, dummy);
}

/**
 * Function: printFill
 * -------------------
 * Silly formatter... self-explanatory.
 */

static void printFill()
{
  for (int i = 0; i < 5; i++) cout << endl;
  cout << "    " << ".... skipping one or more records... " << endl;
  cout << endl;
}

/**
 * Function: listMovies
 * --------------------
 * Assumes coming in that the specified actor/actress has appeared in
 * all of the movies populating the specified credits vector.  This routine
 * prints out the first 10 and the last 10 movies, unless there are 20 or fewer
 * movies on the specified actor's/actress's resume (in which case
 * it just prints all of them.)
 *
 * @param player the actor/actress of interest.
 * @param credits the specified actor's/actress's list of movie
 *                credits.
 */

static void listMovies(const string& player, const vector<film>& credits)
{
  const unsigned int kNumFilmsToPrint = 10;
  cout << player << " has starred in " << (int) (credits.size()) << " films." << endl;
  cout << "These films are:" << endl;
  unsigned int numMovies = 0;
  vector<film>::const_iterator curr;
  for (curr = credits.begin(); curr != credits.end() && numMovies < kNumFilmsToPrint; ++curr) {
    const film& movie = *curr;
    cout << setw(5) << ++numMovies << ".) " << movie.title << " (" << movie.year << ")" << endl;
  }
  if (curr != credits.end()) {
    if (credits.size() > 2 * kNumFilmsToPrint) printFill();
    while (numMovies < (credits.size() - kNumFilmsToPrint)) { numMovies++; ++curr; }
    for (;curr != credits.end(); ++curr) {
      const film& movie = *curr;
      cout << setw(5) << ++numMovies << ".) " << movie.title << " (" << movie.year << ")" << endl;      
    }
  }
  
  stall();
}

/**
 * Function: listCostars
 * ---------------------
 * Builds up the list of costars and then prints all these
 * costars in a format similar to that used by listMovies.
 * The STL set is used to collect actor/actress names without
 * storing duplicates.
 *
 * @param player the actor/actress of interest.
 * @param credits the list of movies that the specified actor/actress has appeared in.
 *                (No integrity checks are done, so it's the client's responsibility to make sure
 *                 the specified actor/actress really has appeared in these movies.)
 * @param db the imdb housing the specified plater, list of movies, etc.  This is passed in
 *           so that each member of each cast of each movie can be added to the specified player's
 *           set of costars.
 */

static void listCostars(const string &player, const vector<film>& credits, const imdb& db)
{
  const unsigned int kNumCostarsToPrint = 10;
  map<string, set<film> > costars;
  for (int i = 0; i < (int) credits.size(); i++) {
    const film& movie = credits[i];
    vector<string> cast;
    db.getCast(movie, cast);
    for (int j = 0; j < (int) cast.size(); j++) {
      const string& costar = cast[j];
      if (costar != player) costars[costar].insert(movie);
    }
  }
  
  cout << player << " has worked with " << (int) costars.size() << " other people." << endl;
  cout << "Those other people are:" << endl;
  
  unsigned int numCostars = 0;
  map<string, set<film> >::const_iterator curr;
  for (curr = costars.begin(); curr != costars.end() && numCostars < kNumCostarsToPrint; ++curr) {
    const string& costar = curr->first;
    cout << setw(5) << ++numCostars << ".) " << costar;
    if (curr->second.size() > 1) cout << " (in " << (int) curr->second.size() << " different films)";
    cout << endl;
  }

  if (curr != costars.end()) {
    if (costars.size() > 2 * kNumCostarsToPrint) printFill();
    while (numCostars < costars.size() - kNumCostarsToPrint) { numCostars++; ++curr; }
    for (; curr != costars.end(); ++curr) {
      const string& costar = curr->first;
      cout << setw(5) << ++numCostars << ".) " << costar;
      if (curr->second.size() > 1) cout << " (in " << (int) curr->second.size() << " different films)";
      cout << endl;
    }
  }

  stall();
}


/**
 * Function: listAllMoviesAndCostars
 * ---------------------------------
 * Pings the specified imbfile to see if the specified
 * actor/actress appears in the database (and if so, has
 * appeared in a non-zero number of films.)  If the specified
 * actor/actress is missing (or if there are no films to speak
 * of), then a polite message is printed and we return immediately.
 * Otherwise, we assume that the local vector<film> has been populated
 * with real data, and we pass the buck onto the listMovies and the
 * listCostars routines.  See the documentation for each of those functions
 * on what they do and how they work.
 *
 * @param player the name of the actor/actress of interest.  No error
 *               checking is done on the string itself.
 * @param db the imdb being queried.  The assumption is that
 *           the imdb is legitimate and has already passed its own
 *           good test.
 */

static void listAllMoviesAndCostars(const string& player,
				    const imdb& db)
{
  vector<film> credits;
  if (!db.getCredits(player, credits) || credits.size() == 0) {
    cout << "We're sorry, but " << player 
	 << " doesn't appear to be in our database." << endl;
    cout << "Perhaps someone else?" << endl;
    return;
  }
  
  listMovies(player, credits);
  listCostars(player, credits, db);
}

/**
 * Function: queryForActors
 * ------------------------
 * Loops indefinitely, and with each iteration prompts
 * the user for the name of an actor or actress.  An
 * empty string response will terminate the program, but
 * any other responses will prompt an attempt to list all
 * of the movie credits and the costars of the specified
 * actor/actresses.  It's possible that the actor/actresses
 * doesn't exist, but the listAllmoviesAndCostrars handles
 * that situation.
 * 
 * @param db a const reference to the imdb that should
 *           queried.
 */

static void queryForActors(const imdb& db)
{
  while (true) {
    cout << "Please enter the name of an actor or actress (or [enter] to quit): ";
    string response;
    getline(cin, response);
    if (cin.fail() || response == "") return;
    listAllMoviesAndCostars(response, db);
  }
}

// Writting some function that exercise simple C pointer
// from class... just put it here and see if it works.
// Using pass by reference
void swapT(int& v1,int& v2)
{
	int temp = v1;
	v1 = v2;
	v2 = temp;
}
void swapT1(int* v1,int* v2)
{
	int temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}
void swapT2(void* v1,void* v2,int size)
{
	char buffer[size];
	memcpy(buffer,v1,size);
	memcpy(v1,v2,size);
	memcpy(v2,buffer,size);
}
void* lsearch(void* key,void* base,int elemSize,int n)
{
	void *a;
	for(int i=0;i<n;i++){
		 a = (char*)base+(i*elemSize);
		 if(memcmp(a,key,elemSize)==0){return a;}
	}
	return NULL;
}
//static int sizeOfElement = sizeof(int);
static int sizeOfElement;
int compFuncInt2(const void* m1,const void* m2)
{
	return memcmp(m1,m2,sizeOfElement);
}
int compFuncInt(const void* m1,const void* m2)
{
	int diff = (*(int*)m1)-(*(int*)m2);
	return diff;	
}
/**
 * Function: main
 * --------------
 * Defines the entry point for the unit testing
 * program that exercises the imdb class.  Notice
 * that the imdb constructor is called, 
 */

int main(int argc, char **argv)
{
  imdb db(determinePathToData());
  if (!db.good()) { cerr << "Data directory not found!  Aborting..." << endl; return 1; }
  queryForActors(db);

  sizeOfElement = sizeof(int);
  // test of swap
  int a = 1;
  int b = 3;
  cout<<"a "<<a<<"b "<<b<<endl;
  //swapT(a,b); // works
  //swapT1(&a,&b); //works
  swapT2(&a,&b,sizeof(int)); //works
  cout<<"a "<<a<<"b "<<b<<endl;
  //cout<<"a "<<&a<<endl;
  double x= 2.13;
  double y= 5.56;
  cout<<"x "<<x<<" y "<<y<<endl;
  swapT2(&x,&y,sizeof(double));
  cout<<"x "<<x<<" y "<<y<<endl;
//  cout<<sizeof(double)<<endl;
//  cout<<sizeof(int)<<endl;
//  Test of lsearch from class
  int v[] = {1,2,3,4,5};
  int w   = 4;  
  void* vd = lsearch(&w,v,sizeof(int),5);//works
  cout<<*(int*)vd<<endl;
  void* found = bsearch(&w,v,5,sizeof(int),compFuncInt); //works
  if(found==NULL) cout<<"NULL"<<endl;
  cout<<*(int*)found<<endl;
  void* found2 = bsearch(&w,v,5,sizeof(int),compFuncInt2); //works
  if(found2==NULL) cout<<"NULL2"<<endl;
  cout<<"found2="<<*(int*)found2<<endl;
  char cc[] = "this is a test";
  char c[] = "is";
  vd = lsearch(c,cc,sizeof(char),5);
  cout<<*(char*)vd<<endl;

  // it should not work... but trying to see what happens
  //cout<<*((char*)vd+3)<<endl; // does not work... NULL?

  return 0;
}
