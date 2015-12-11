using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}
int imdb::comp1(const void* m1,const void* m2)
{
	//m1=key:string
	//m2=base[i]:int that points to a char array
	//string key = *(string*)m1;
	keyK kk = *(keyK*)m1;
	string key = *(string*)kk.pName;
	int where = *(int*)m2;
	int whereNext = *(int*)((char*)m2+sizeof(int));
	unsigned int recL = whereNext-where;
        int recRead;
	if(recL<key.size()){
		recRead = recL;
	}else{
		recRead = key.size();
	}
	void* ptr = (char*)kk.aFile+where;
	string actorName;
	for(int j=0;j<recRead;j++){
		char rr = *((char*)ptr+j);
		if(rr=='\0'){
			break;
		}
		actorName.push_back(rr);
	}
	return -1*actorName.compare(key);
}
int imdb::compInt(const void* m1,const void* m2)
{
	int i1 = *(int*)m1;
	int i2 = *(int*)m2;
	if(i1>i2) return 1;
	if(i1<i2) return -1;
	return 0;
}
//int imdb::myBsearch(const void* m1,const void* m2, int N, int sizeN,int (*compF)(const void*,const void*))
// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const 
{ 
	// How many records are there: N
	int N = *(int*) actorFile; // first 4 bits
        //cout<<"Number of records "<< N << " n bites "<<4*N<<endl;
	// Use bsearch ....
	// key = player
	// base = array from i=1 to N
	// tricky part is to pass into bsearch the element
	// size, that is the number of bits per actor name
	// which is not const.
	keyK t_K;
	t_K.pName = &player;
	t_K.aFile = actorFile; // need to pass since bsearch function is a static member
	void* vActor = ((char*)actorFile+(1*sizeof(int)));
	void* test = bsearch(&t_K,vActor,N,sizeof(int),imdb::comp1);
	if(test==NULL){
		cout<<"NULL22222"<<endl;
		return false;
	}
	//cout<< *(int*)test<<endl;
	int start = *(int*)test;
	int end   = *(int*) ((char*)test+sizeof(int));
	int recSize = end-start;
	void* ptr = (char*)actorFile+start;
	string actorName;
	int counter;
	for(int j=0;j<recSize;j++){
		char rr = *((char*)ptr+j);
		counter=j;
		if(rr=='\0'){
			counter++; // pass the '\0'
			break;
		}
		actorName.push_back(rr);
	}
	// cout<<actorName<<endl;
	// Works but we have several null terminated so we
	// need to track them character by char like loop
	// above
	char* next = ((char*)ptr+counter);
	if(*next=='\0'){
		counter++;
	}
	short nM = *(short*)((char*)ptr+counter);
	//cout<<nM<<endl;
	counter+=sizeof(short); // move short
	next = ((char*)ptr+counter);
	if(*next=='\0'){
		counter++;
	}
	if(*next=='\0'){
		counter++;
	}
	next = ((char*)ptr+counter);
	// Number of movies....
	int Nm = *(int*) movieFile; // first 4 bits
	// Vector of movie locations
	void* vMovie = ((char*)movieFile+(1*sizeof(int)));
	// Do the same thing again ...
	film fhere;
	for(int j=0;j<nM;j++){
		int firstMovLoc = *(int*)(next+(j*sizeof(int)));
		//cout<<firstMovLoc<<endl;
		
		void* mLoc = bsearch(&firstMovLoc,vMovie,Nm,sizeof(int),imdb::compInt);
		if(mLoc==NULL){
			cout<<"NULL3333"<<endl;
		}
		int endRec = *(int*) ((char*)mLoc+sizeof(int));
		//cout<<endRec<<endl;
		int rLength = endRec-firstMovLoc;
		//cout<<rLength<<endl;
		int counter2;
		string movieName;
		void* ptr2 = (char*)movieFile+firstMovLoc;
		for(int j=0;j<rLength;j++){
			char rr = *((char*)ptr2+j);
			counter2=j;
			if(rr=='\0'){
				counter2++; // pass the '\0'
				break;
			}
			movieName.push_back(rr);
		}
	        char year = *((char*)ptr2+counter2);
		int  yr   = 1900+(year-'\0');
		//cout<<movieName<<endl;
		//cout<<yr<<endl;
		fhere.title = movieName;
		fhere.year  = yr;
		films.push_back(fhere);
	}
	return true;
}
int imdb::compMovie(const void* m1,const void* m2)
{
	keyK kk = *(keyK*)m1;
	film key = *(film*)kk.pName;
	int where = *(int*)m2;
	int whereNext = *(int*)((char*)m2+sizeof(int));
	unsigned int recL = whereNext-where;
        int recRead;
	if(recL<key.title.size()){
		recRead = recL;
	}else{
		recRead = key.title.size();
	}
	void* ptr = (char*)kk.aFile+where;
	string movieName;
	int counter;
	for(int j=0;j<(recRead+1);j++){
		char rr = *((char*)ptr+j);
		counter = j;
		if(rr=='\0'){
			counter++;
			break;
		}
		movieName.push_back(rr);
	}
	char year = *((char*)ptr+counter);
	int  yr   = 1900+(year-'\0');
	//cout<<movieName;
	//cout<<yr<<endl;
	//cout<<year<<endl;
	//cout<<key.title;
	//cout<<key.year<<endl;
	film t_movie;
        t_movie.title = movieName;
        t_movie.year  = yr;
	//cout<<(key.title)<<endl;
	//cout<<(key.year)<<endl;
	//cout<<(t_movie.title)<<endl;
	//cout<<(t_movie.year)<<endl;
        /*
	if((key.title).compare(movieName)==0){
		cout<<"OKKKKKKK"<<endl;
	}
	if(key.year==t_movie.year){
		cout<<"OKKKKKKYEAR"<<endl;
	}
	*/
	if(key==t_movie){
		return 0;
	}
        if(t_movie<key){
		return 1;
	}else{
		return -1;
	}
}
bool imdb::getCast(const film& movie, vector<string>& players) const { 

	// How many records are there: N
	int N = *(int*) movieFile; // first 4 bits
	keyK t_K;
	t_K.pName = &movie;
	t_K.aFile = movieFile; // need to pass since bsearch function is a static member
	void* vMovie = ((char*)movieFile+(1*sizeof(int)));
	void* testM = bsearch(&t_K,vMovie,N,sizeof(int),imdb::compMovie);
	if(testM==NULL){
		cout<<"NULLMMMM2"<<endl;
		return false;
	}
	int whereM = *(int*) testM;
		int start = whereM;
		//cout<<start<<endl;
		//int recSize = end-start;
		int recSize = 1000;
		void* ptr = (char*)movieFile+start;
		int counter=0;
		std::string movieName;
		for(int j=0;j<recSize;j++){
			char rr = *((char*)ptr+j);
			//cout<<rr<<" ";
			//movieName.push_back(rr);
			counter=j;
			if(rr=='\0'){
				counter++; // pass the '\0'
				break;
			}
			//counter=j;
			movieName.push_back(rr);
		}
		//cout<<endl;
	        char year = *((char*)ptr+counter);
		int  yr   = 1900+(year-'\0');
		cout<<movieName<<endl;
		cout<<yr<<endl;
		cout<<"Size of movie name="<<movieName.size()<<endl;
		counter++;
	        char* next = ((char*)ptr+counter);
	        if(*next=='\0'){
			counter++;
		}
	        next = ((char*)ptr+counter);
	        short int nM = *(short*)(next);
		cout<<"Numb of coactors="<<nM<<endl;
		counter += sizeof(short);
		next = ((char*)ptr+counter);
		int counter1 = counter;
	        counter1 +=2;
		char* next2 = ((char*)ptr+counter1);
		short int testN = *(short*) (next2);
		cout<<"This is inside="<<testN<<endl;
		if((*next)=='\0'){
			cout<<"We added 1"<<endl;
			counter++;
		}
	        next = ((char*)ptr+counter);
	        if(*next=='\0'){
			cout<<"We added 2"<<endl;
			counter++;
		}
		
	        next = ((char*)ptr+counter);
		
		if(testN==138){ // needed hack... do not know how to identify the next 2 bite!!! not always '\0'
			cout<<"Lower counter"<<endl;
			counter--;
		}
	        next = ((char*)ptr+counter);
		cout<<(*next)<<endl;
		if((*next)=='\0'){
			cout<<"BLANKKKKKKKKKKKKK"<<endl;
		}	

		for(int j=0;j<nM;j++){
			int firstActorLoc = *(int*)(next+(j*sizeof(int)));
			void* p2actorFile = (char*)actorFile+firstActorLoc;
			string actorName;
			for(int k=0;k<5000;k++){
				//cout<< k << endl;
				char rr = *((char*)p2actorFile+k);
				//cout<<"here="<< rr << endl;
				if(rr=='\0'){
					break;
				}
				actorName.push_back(rr);
			}
			//cout<<actorName<<endl;
			players.push_back(actorName);
		}
		
	return true; 
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
