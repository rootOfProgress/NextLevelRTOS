#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")


#define SV_SET_EXT_IO_CALLBACK __asm volatile ("mov r6, 8\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")

void execute_uart_isr(unsigned int function_address)
{
  SV_SET_EXT_IO_CALLBACK;
}

int fibbonacci(int n) {
   if(n == 0){
      return 0;
   } else if(n == 1) {
      return 1;
   } else {
      return (fibbonacci(n-1) + fibbonacci(n-2));
   }
}

void myusarthandler(unsigned int uart_rx_buffer) 
{
   while (1)
   {
      /* code */
   }
   
}


void __attribute__((__noipa__))  __attribute__((optimize("O0"))) template2(void)
{
   execute_uart_isr(&myusarthandler);
   while (1)
   {
      SV_YIELD_TASK;
   }
    
}
