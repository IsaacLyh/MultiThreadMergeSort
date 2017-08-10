// Merge Sort starter code
// Scott B. Baden, CSE Dept, UCSD
// 1/18/2016
// 

#include <cstdlib>
#include <iostream>     // std::cout

#include <algorithm>    // std::sort

#include <mutex>
#include <thread>
#include <time.h>
#include "mergeSort.h"

using namespace std;
//
// Globals
//
extern control_block cb;

double getTime();

bool checkThread();

void copyVec(vector<int> *in, vector<int> *out, int be, 
    int end, int offset);

void P_Merge(vector<int> *keysIn, vector<int> *keysOut, int stA, 
                  int endA, int stB, int endB, int offset);
int threadNum = 1;
mutex mut;

/* Helper function to copy certain part of the vector to another one at 
 * certain location
 */
void copyVec(vector<int> *in, vector<int> *out, int be, int end, int offset)
{
   for(vector<int>::iterator it = (*in).begin() + be; 
       it != (*in).begin() + end + 1; ++it)
   {
      (*out)[it - (*in).begin() - be + offset] = *it;
   }
}

/* Helper function to check whether we still have spare thread to use
 * Use a mutex lock guard to protect the counter and prevent from 
 * dead lock
 */
bool checkThread()
{
   lock_guard<std::mutex> guard(mut);
   if (threadNum < cb.NT)
   {
      threadNum++;
      return true;
   }
   return false;
}

// Local Sort - each thread sorts its assigned keys
void LSort(std::vector<int> *keys, int l0, int l1, int level){

    std::vector<int>::iterator I0 = keys->begin()+l0;
    std::vector<int>::iterator I1 = keys->begin()+l1+1;
#ifdef DEBUG
    if ((l1-l0+1) <= 16){
        std::cout << "Keys to be sorted (LSORT): \n";
        for (std::vector<int>::iterator it=I0; it!=I1; ++it)
            std::cout << ' ' << *it;
        std::cout << std::endl;
        std::cout << std::endl;
    }
#endif

    std::stable_sort(I0,I1);

#ifdef DEBUG
    if ((l1-l0+1) <= 16){
        std::cout << "Keys were sorted (LSORT): \n";
        for (std::vector<int>::iterator it=I0; it!=I1; ++it)
            std::cout << ' ' << *it;
        std::cout << std::endl;
        std::cout << std::endl;
    }
#endif
    return;
}


//
// Divide and conquer algorithm for Merge Sort
// Don't change the signature to this function
//

void DC_MergeSort(std::vector<int> *keysIn, int l0, int l1, std::vector<int> *keysOut, int level, bool& direction_flag)
{

    int n = (l1-l0) + 1;
    if (n < 2){                 // If list has 0 or 1 elements,
        direction_flag = true;
        return;                 // then it is already sorted
    }

    // Otherwise, split list at the mid-point, and sort each part recursively
    // Upon return, we merge the two lists
    // We stop the recursion at a specified threshold value minN
    // and at that point we use a "base" sort
    //
    //
    //        TODO for the assignment

    // 1. Spawn threads to handle the 2 MergeSort calls in parallel,
    //    up to the limit (NT) on the number of threads specified
    //    in the -t command line argument
    //
    // 2. Do the merges in parallel
    //

    if (n <= cb.minN){
        LSort(keysIn,l0,l1,level);
        direction_flag = true;
    }
    else{
        int mid = l0+n/2;
        bool dir0, dir1;
        int levNext0 =  level+1;
        int levNext1 =  level+1;
        assert(levNext0 == levNext1);   // Sanity Check
                                        // Error if they aren't equal!
        
        if(checkThread()){ //Check whether we still have spare thread
            /* Spawn a new thread to continue split the right part
             * of the input integer vector
             */
            thread newThread = thread(DC_MergeSort, ref(keysIn), mid, l1, 
                                    ref(keysOut), levNext1, ref(dir1));
            /* Current thread keeps working on the right side of the 
             * input integer vector
             */
            DC_MergeSort(keysIn, l0, mid-1, keysOut, levNext0, dir0);
            newThread.join();
            threadNum--;
        }
        else{
            DC_MergeSort(keysIn, l0, mid-1, keysOut, levNext0,dir0);
            DC_MergeSort(keysIn, mid, l1, keysOut, levNext1,dir1);
        }
        assert(dir0 == dir1);           // Sanity check
                                        // Error if they aren't equal!

// Do the merge with a serial algorithm
// As we merge, we place the merged output in a different list from
// that containing the input to the merge, and we alternate
// as we merge back up to the root of the divide and conquer call tree
//
// Since we sort in place for an input size of 2, or
// when the recursion terminates (and we use qsort to sort in place),
// we set the flag to true in these cases
//
// Can you avoid the need for a 2nd buffer?
//
        int stB = l0 + (l1-l0)/2 + 1;
        int stA = l0;
        int endA = l0 + (l1-l0)/2;
        int endB = l1;
        if (dir0) // Decide the direction between buffers
        {
          if(cb.par_merge == true){ // If using parallel merge
            P_Merge(keysIn,keysOut,stA,endA,stB,endB, stA);
          }
          else{ // using serial merge
            Merge(keysIn,keysOut,l0,l1);
          }
        }
        else
        {
          if(cb.par_merge == true){
            P_Merge(keysOut,keysIn,stA,endA,stB,endB, stA);
          }
          else{
            Merge(keysOut,keysIn,l0,l1);
          }
        }
        direction_flag = !dir0;
        return;
    }
}
