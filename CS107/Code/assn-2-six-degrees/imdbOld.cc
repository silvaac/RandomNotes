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
	int recL = whereNext-where;
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
        cout<<"Number of records "<< N << " n bites "<<4*N<<endl;
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
	}
	cout<< *(int*)test<<endl;
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
	cout<<actorName<<endl;
	// Works but we have several null terminated so we
	// need to track them character by char like loop
	// above
	//string actorName2((char*)actorFile+start,recSize);
	//cout<<actorName2<<endl;
	char* next = ((char*)ptr+counter);
	if(*next=='\0'){
		counter++;
	}
	short nM = *(short*)((char*)ptr+counter);
	cout<<nM<<endl;
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
			movieName.push_back(rr);
			counter2=j;
			if(rr=='\0'){
				counter2++; // pass the '\0'
				break;
			}
		}
	        char year = *((char*)ptr2+counter2);
		int  yr   = 1900+(year-'\0');
		cout<<movieName<<endl;
		cout<<yr<<endl;
	}
		
	return false;
	// old code:
	for(int i=1;i<(N-1);i++){
		int start = *(int*) ((char*)actorFile+(i*sizeof(int)));
		int end   = *(int*) ((char*)actorFile+((i+1)*sizeof(int)));
		int recSize = end-start;
		void* ptr = (char*)actorFile+start;
		//char movieName[recSize];
		//memcpy(movieName,ptr,recSize);
		//cout<<recSize<<endl;
		//cout<<start<<endl;
		int counter;
		std::string actorName;
		for(int j=0;j<recSize;j++){
			char rr = *((char*)ptr+j);
		//	cout<<rr<<" ";
			counter=j;
			if(rr=='\0'){
				counter++; // pass the '\0'
				break;
			}
			actorName.push_back(rr);
		}
	//	cout<<endl;
	        char* next = ((char*)ptr+counter);
	        if(*next=='\0'){
			counter++;
		}
	        //short nM = *(short*)((char*)ptr+counter);
		cout<<start<<endl;
		//cout<<actorName<<endl;
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
       /*         
		// Do the same thing again ...
		for(int j=0;j<nM;j++){
			int firstMovLoc = *(int*)(next+(j*sizeof(int)));
			cout<<firstMovLoc<<endl;
		}
		*/
		int cpr = actorName.compare(player);
		if(cpr==0){
			cout<<actorName<<endl;
			break;
		}
		
	}
		
	return false; 

}
bool imdb::getCast(const film& movie, vector<string>& players) const { 
	
	// How many records are there: N
	int N = *(int*) movieFile; // first 4 bits
        cout<<"Number of records "<< N << " n bites "<<4*N<<endl;
	//for(int i=1;i<(N-1);i++){
	for(int i=10000;i<10004;i++){
		int start = *(int*) ((char*)movieFile+(i*sizeof(int)));
		int end   = *(int*) ((char*)movieFile+((i+1)*sizeof(int)));
		int recSize = end-start;
		void* ptr = (char*)movieFile+start;
		//char movieName[recSize];
		//memcpy(movieName,ptr,recSize);
		//cout<<recSize<<endl;
		//cout<<start<<endl;
		int counter;
		std::string movieName;
		for(int j=0;j<recSize;j++){
			char rr = *((char*)ptr+j);
			cout<<rr<<" ";
			movieName.push_back(rr);
			counter=j;
			if(rr=='\0'){
				counter++; // pass the '\0'
				break;
			}
		}
		cout<<endl;
	        char year = *((char*)ptr+counter);
		int  yr   = 1900+(year-'\0');
		cout<<movieName<<endl;
		cout<<yr<<endl;
		counter++;
	        char* next = ((char*)ptr+counter);
	        if(*next=='\0'){
			counter++;
		}
	        next = ((char*)ptr+counter);
	        short int nM = *(short*)(next);
		cout<<nM<<endl;
		counter = counter+sizeof(short);
	        next = ((char*)ptr+counter);
	        if(*next=='\0'){
			counter++;
		}
	        next = ((char*)ptr+counter);
	        if(*next=='\0'){
			counter++;
		}
	        next = ((char*)ptr+counter);
                
		// Do the same thing again ...
		for(int j=0;j<nM;j++){
			int firstActorLoc = *(int*)(next+(j*sizeof(int)));
			cout<<firstActorLoc<<endl;
		}
		
	}
		
	return false; 
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
