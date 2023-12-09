#include <iostream>

int main(){

  int a = 0;

  do{
    for(int i=0; i<100000; i++){
      a++;
      for(int j=0; j<10000; j++){
        a++;
        if(a>=10000) break;
      }
    }
  } while(a<10000);

  std::cout << a << std::endl;
}