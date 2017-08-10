// Merge two lists
// haha

#include <cstdlib>
#include <iostream>     // std::cout

#include <algorithm>    // std::sort

#include <thread>
#include <time.h>
#include "mergeSort.h"

using namespace std;
//
// Globals
//
  extern control_block cb;
  extern int threadNum;
//
// Merge two lists on the interval (l0:l1), that is,
// (l0:mid-1) and (mid-1:n1), where mid = l0 + n/2
// This is a serial implementation Serial
// 

//void copyVec(int be, int end, int offset, vector<int> *in, 
  //          vector<int> *out);
//void ParallelMerge(int stA, int stB, int endA, int endB, int offset, 
  //                  bool flag, vector<int> *in, vector<int> *out);


bool checkThread();
void P_Merge(vector<int> *keysIn, vector<int> *keysOut, int stA, int endA, 
           int stB, int endB, int offset);
int BS(vector<int> *keysIn, int stB, int endB, int target);
void copyVec(vector<int> *in, vector<int> *out, int be, int end, int offset);

void Merge(std::vector<int> *keysIn, std::vector<int> *keysOut,
           int l0, int l1){
    int n = (l1-l0) + 1;
    int mid = l0 + n/2;
    int min1 = l0, max1 = mid-1;
    int min2 = mid, max2 = l1;

#ifdef DEBUG
// Output the keys if N is <= 16
    if (n <= 16){
        for (int i=min1;i<=max1;i++) 
            std::cout<<(*keysIn)[i]<<" ";
        std::cout<<std::endl;
        for (int i=min2;i<=max2;i++) 
            std::cout<<(*keysIn)[i]<<" ";
        std::cout<<std::endl;
    }
#endif
    int l=min1;
    int r=min2;
    int i;
    for (i=0; i < max2-min1+1 ; i++) {  
      if ((*keysIn)[l]<(*keysIn)[r]) {
          (*keysOut)[i+min1]=(*keysIn)[l++];
          if (l>max1) break;
      } else {
          (*keysOut)[i+min1]=(*keysIn)[r++];
          if (r>max2) break;
      }
    }
    while (l<=max1) {
        i++;
        (*keysOut)[i+min1]=(*keysIn)[l++];
    }
    while (r<=max2) {
        i++;
        (*keysOut)[i+min1]=(*keysIn)[r++];
    }
}

/* Parrallel merge function
 * Keep spliting the input two partitions A and B until reach a certain 
 * point then use serial merge
 */
void P_Merge(vector<int> *keysIn, vector<int> *keysOut, int stA, int endA, 
            int stB, int endB, int offset)
{  // Length of A and B
   int n1 = endA - stA + 1;
   int n2 = endB - stB + 1;
   // Assume lengthA is always larger then lengthB
   // If not true we exchange A and B
   if(n1 < n2){
      int temp1 = stB;
      stB = stA;
      stA = temp1;
      int temp2 = n2;
      n2 = n1;
      n1 = temp2;
      int temp3 = endB;
      endB = endA;
      endA = temp3;
   }
   // Corner case: lengthA and lengthA are both 0
   if (n1 == 0){
      return;
   }
   // Corner case: lengthB is 0, copy A to the output
   if (n2 == 0){
      copyVec(keysIn, keysOut, stA, endA, offset);
      return;
   }
   // If lengthA + lengthB is less then g, start using serial merge
   if(n1 + n2 <= cb.minN){
      // Create 3 iterators to iterate A and B and output C
      vector<int>::iterator itA = (*keysIn).begin() + stA;
      vector<int>::iterator itB = (*keysIn).begin() + stB;
      vector<int>::iterator itC = (*keysOut).begin() + offset;
      
      // Merge A and B into C
      while(true){
        // If reaches the end of A, copy the rest of B into C
         if(((*keysIn).begin() + endA) - itA + 1 == 0){
            copy(itB, (*keysIn).begin() + endB + 1, itC);
            return;
         }
         // If reaches the end of B, copy the rest of A into C
         if(((*keysIn).begin() + endB) - itB + 1 == 0){
            copy(itA, (*keysIn).begin() + endA + 1, itC);
            return;
         }
         *itC++ = (*itB < *itA)? *itB ++: *itA++;
      }
   }
   // Recursion part
   else{
      // Midpoint of A
      int q1 = (stA + endA)/2;
      // Using BS find "mid" of B
      int q2 = BS(keysIn, stB, endB, (*keysIn)[q1]);
      // New offset after merge A, B into output
      int q3 = offset + (q1 - stA) + (q2 - stB);
      // write mid of A into output
      (*keysOut)[q3] = (*keysIn)[q1];
      // Check we have spare thread to use or not
      if(checkThread()){
        // spawn new thread
         thread newThread = thread(P_Merge, ref(keysIn), ref(keysOut), 
                stA, q1-1, stB, q2-1, offset);
         P_Merge(keysIn, keysOut, q1+1, endA, q2, endB, q3+1);
         newThread.join();
         threadNum--;
      }
      // otherwise recursively call the function to split A and B in
      // single thread
      else{
         P_Merge(keysIn, keysOut, stA, q1-1, stB, q2-1, offset);
         P_Merge(keysIn, keysOut, q1+1, endA, q2, endB, q3+1);
      }
   }
}
/* Helper function using binary search to find the index i such that
 * B[i] is the smallest element in B that is larger then A[mid]
 */
int BS(vector<int> *keysIn, int stB, int endB, int target)
{
  if(endB - stB == 1){
      if ((*keysIn)[stB] >= target) return stB;
      return  endB; 
  }
  int mid = round((stB + endB)/2);
  if((*keysIn)[mid] == target){
      return mid;
  }
  if((*keysIn)[mid] > target){
      return BS(std::ref(keysIn), stB, mid, target);
  }
  else{
      return BS(std::ref(keysIn), mid, endB, target);
  }
}
