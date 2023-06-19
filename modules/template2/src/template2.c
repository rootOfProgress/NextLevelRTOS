int fibbonacci(int n) {
   if(n == 0){
      return 0;
   } else if(n == 1) {
      return 1;
   } else {
      return (fibbonacci(n-1) + fibbonacci(n-2));
   }
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) template2(void)
{
    while (1)
    {
        fibbonacci(10);
    }
    
}
