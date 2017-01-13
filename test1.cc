#include "stdio.h"
#include "vector"
#include "iostream"
#include "sstream"
using namespace std;
int main()
{
   vector<int>  a={1,4,5,65,64,22,23} ;
/*
   a.push_back(1);
   a.push_back(22);
   a.push_back(13);
   a.push_back(89);
   a.push_back(5);
*/
   for(int i = 0;i < a.size();i++)
   {
       stringstream st;
       st << "index " << i << " is:" << a[i]<<endl;
       printf("%s",st.str().c_str());
   }
    return 0;
}
