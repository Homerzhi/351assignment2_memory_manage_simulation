#include <fstream> 
#include <iostream>
#include <sstream> 
#include <iomanip>  //setw 
#include <string>
#include <queue> 
#include <cctype>
 
 
using namespace std; 

int switchOn=0;
int currentTime=0, endTime=0, totalRunTime=0, numProcess, pageSize=200, totalPage=2000/pageSize;
 
void read(string filename, string outputFilename);

struct Process{
  int processID,arrivalTime, boostTime,pageNumber, waitingTime;
}Processes;
void outputProcess(queue<Process> m);
void assignPage(int[], int); 
void outputState(int*, int); 
void freePages(int*, int, int);
int howManyPageAvailable(int *pages, int n);
queue<Process> memu, input, finishQueue; 


int main( int argc, char *argv[]){ 
  string filename=argv[1];   
  string outputFilename=argv[2]; 
  read(filename, outputFilename );
 //outputProcess(memu); cin.get();
  int *pages;
  pages=new int[totalPage];
  for(int i=0; i< totalPage; i++){
    pages[i]=0;  //0 means the page is free. 
  }
  finishQueue=memu; 
  do{
       //if(!finishQueue.empty()) cout<< "===========finish queue size:"<<finishQueue.size()<<endl;
       queue<Process> finish;
      while(!finishQueue.empty() ){ // check if the process finished.
          queue<Process> wait;
          while(!input.empty()){ //
            input.front().waitingTime=currentTime-input.front().arrivalTime;
            input.front().arrivalTime=currentTime;
            wait.push(input.front() );
            input.pop();
          }         
          
          if(!wait.empty() ) { //if some process in input queue due to the limited page.
              input=wait;

              while(!wait.empty()){
                  queue<Process> tem;
                  while(!finishQueue.empty()){
                      if(finishQueue.front().processID==wait.front().processID){
                      
                          finishQueue.front().waitingTime=currentTime-wait.front().arrivalTime;
                          
                         //cout<<"arrival time*********************"<<wait.front().arrivalTime<<endl;
                         finishQueue.front().arrivalTime+=currentTime;  
                         //cout<<"waiting time*********************"<<wait.front().arrivalTime<<endl;                 
                       } 
                       tem.push(finishQueue.front() );
                          
                       finishQueue.pop();                   
                  }
                   finishQueue=tem;
                   wait.pop();
              }                       
         }
         if((finishQueue.front().arrivalTime+finishQueue.front().boostTime+finishQueue.front().waitingTime)> currentTime){ //if process haven't finished save it.
            finish.push( finishQueue.front() );   
         }  
         
        if((finishQueue.front().arrivalTime+finishQueue.front().boostTime)==currentTime){ //process finished.
          endTime+= finishQueue.front().waitingTime;  
          totalRunTime+= finishQueue.front().waitingTime; 
          cout<<"arrival time*********************"<<finishQueue.front().arrivalTime<<endl;
          cout<<"t = "<<currentTime<<" : ";
          cout<<"Process "<< finishQueue.front().processID <<" completes\n";
          freePages(pages, totalPage, finishQueue.front().processID );
          outputState(pages, totalPage);          
        }
        finishQueue.pop();       
     }
     finishQueue=finish;
      
      if(!input.empty() ){assignPage( pages, totalPage);}
     
    while(!memu.empty()){//processID,arrivalTime, boostTime,pageNumber;

       if(memu.front().arrivalTime==currentTime){ //process arrived.
          int temp=memu.front().boostTime + currentTime;
           if(endTime < temp) endTime=temp; 
          input.push(memu.front() );
          cout<<"t = "<<currentTime<<":"; 
   
          queue<Process> printID=input;
          cout<<" Process "<<memu.front().processID <<" arrives\n";
          cout<<"\tInput Queue:[ ";
          while(!printID.empty() ){
              cout<<printID.front().processID<<" ";
              printID.pop();
          }
          cout<<"]\n";

          memu.pop();
          //cout<<"end time:"<<endTime<<endl;
        }//end if 
        else break;     
      }//end while
 
      assignPage( pages, totalPage);
     
    currentTime++;
    
  }while(currentTime!=endTime  );

 if(finishQueue.empty()) cout<<"finishQueue is empty\n";
 else cout<<"finishQueue size:"<<memu.size()<<endl;

 if(memu.empty()) cout<<"Queue is empty\n";
 else cout<<"Queue size:"<<memu.size()<<endl; 
  cout << "\n"<<"total running time:"<<totalRunTime<<". and avg time:"<< totalRunTime/numProcess<<"\n"; 
 //system("pause"); 
  return 0; 
} 


void assignPage(int *pages, int n  ){ //n is pages size
  

  while(!input.empty() ){
  
    queue<Process> printID=input;
    int p=(input.front().pageNumber+(pageSize-1) )/pageSize;
    int availablePage= howManyPageAvailable(pages, totalPage);
    if(availablePage < p) return;
    
    cout<<"\tMM moves Process "<<printID.front().processID<<" to memory\n";
    printID.pop();
    cout<<"\tInput Queue:[";
    while(!printID.empty()){
    cout<<printID.front().processID<<" ";
    printID.pop();
    }cout<<"]\n";
  

    
    
    
    int assignedPage=0; 
    for(int i=0; i< n; i++){
        if(pages[i]==0 && assignedPage!=p){
           pages[i]=input.front().processID; 
           assignedPage++;        
         }
     }
    if( assignedPage!=p){cout<<"assign page error, no enough space OR else\n";cin.get();}
    cout<<"\tMemory Map:";
    
    outputState(pages, n);
    input.pop();
  }

}
int howManyPageAvailable(int *pages, int n){
    int pageAvailable=0;
    for(int i=0; i< n; i++){
        if(pages[i]==0){
           pageAvailable++;        
         }
     }
     return pageAvailable;
}
void freePages(int *pages, int n, int ID){
    for(int i=0; i< n; i++){
        if(pages[i]==ID){
           pages[i]=0;        
         }
     }
}

  void outputState(int *pages, int n){
    for(int i=0; i< n; i++){
      if(pages[i]!=0) 
        if(i==0) cout<<i*pageSize<<"-"<<i*pageSize+pageSize-1<<": process "<<pages[i]<<", Page "<<i+1<<endl;
        else cout<<"\t\t"<<i*pageSize<<"-"<<i*pageSize+pageSize-1<<": process "<<pages[i]<<", Page "<<i+1<<endl;
      if(pages[i]==0){
        int start=i, end=i;
        while(pages[i]==0 && i<n){
          end=i; i++;
        }
        cout<<"\t\t"<<start*pageSize<<"-"<<end*pageSize+pageSize-1<<": Free frame(s)"<<endl;
      }
  }
} 

void outputProcess(queue<Process> m){
 while( !m.empty() ){
  cout<<m.front().processID<<" arrival time:"<<m.front().arrivalTime<<"\tboost time:"<<m.front().boostTime
  <<"\t page number:"<<m.front().pageNumber<<" waiting time:"<< m.front().waitingTime<<endl;
  m.pop();
 }
}
 
//read the in.text file and initialize the queue 
void read(string filename, string outputFilename ){ 
  ifstream infile; 
  infile.open(filename.c_str()); 
  if (infile.is_open()){ cout << "input file is open\n"; }   else cout << "input file open error\n"; 
 
  ofstream outfile; 
  outfile.open(outputFilename.c_str(), ios_base::out | ios_base::trunc); //std::ios::in | fstream::out | ios_base::in 
  if (outfile.is_open()){ cout << "output file is open\n"; }  else cout << "output file open error\n"; 
  //save head to file  //outfile <<left<<setw(16)<< "token\t"   << "lexeme" << endl << endl; 
 
  string line; int num=0,a,b; 
  infile>>numProcess; 
  //get each integer from file. 
  while (getline(infile,line )  ){  
       num++; 

    getline(infile,line ) ; //process ID
    istringstream is(line);
      is>>Processes.processID; 
      if(switchOn)cout<<num<<" process ID: "<<"-> "<<Processes.processID<<endl;
    getline(infile,line ) ;
    istringstream iis(line); iis>>Processes.arrivalTime>>Processes.boostTime;
    totalRunTime+=Processes.boostTime;
      if(switchOn)cout<<num<<"Arrivial Time: "<<Processes.arrivalTime<< " boost time-> "<<Processes.boostTime<<endl;
      
    getline(infile,line ) ;
    istringstream iiis(line); iiis>>a;int sum=0;
    for (int i=0; i<a; i++){iiis>>b; sum+=b;}
    Processes.pageNumber=sum;
      if(switchOn)cout<<num<<"# Mem space: "<<a <<" space:-> "<<Processes.pageNumber<<endl;
   Processes.waitingTime=0;
    memu.push(Processes);
 }    
  infile.close(); 
  outfile.close(); 
 
 
}