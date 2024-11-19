#include "adc.h"
#include "gpio/gpio.h"
#include "test.h"
#include "spi.h"
#include "nrf24l01.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"
#include "crc.h"
#include "exti.h"
#include "syscfg.h"
#include "am2302.h"
#include "nodeData.h"
#include "nodeConfig.h"
#include "util/timeFormats.h"
#include "rtc.h"
#include "health.h"
#include "rxSpecifics.h"
#include "globals.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))



Nrf24l01Registers_t nrf_registers;
Am2302Readings_t readings;
// NodePackage_t package;
GpioObject_t pinb;
GpioObject_t gpio_adc_a1;
GpioObject_t gpio_a3_notify;
TxConfig_t tx_config;
TxObserve_t observe;
unsigned short battery;

unsigned int tAckReceived;
char receivedAckPackage;
char outBuffer[78];
char *rx_answer;


void memcpy_custom(void *dest, const void *src, unsigned int n)
{
  char *cdest = (char *)dest;
  const char *csrc = (const char *)src;

  // Copy each byte from source to destination
  for (unsigned int i = 0; i < n; ++i)
  {
    cdest[i] = csrc[i];
  }
}


unsigned int reverse_byte_order(unsigned int num)
{
  return ((num >> 24) & 0x000000FF) |
         ((num >> 8) & 0x0000FF00) |
         ((num << 8) & 0x00FF0000) |
         ((num << 24) & 0xFF000000);
}



void  __attribute__((optimize("O0"))) tx_receive_isr()
{
  exti_acknowledge_interrupt(&pinb);
  // char rx_answer[32];
  receivedAckPackage = (char) tx_ack_receive_isr(&nrf_registers, rx_answer);
  // asm("bkpt");
  // if (receivedAckPackage)
  // {
  //   // @todo: better feed 4byte wise
  //   crc_reset();
  //   for (unsigned int i = 0; i < 13; i++)
  //   {
  //     crc_feed((unsigned int)rx_answer[i]);
  //   }

  //   unsigned int crc_expected = crc_read();
  //   unsigned int crc = 0;

  //   memcpy_custom(&crc, &rx_answer[9], sizeof(unsigned int)); // Ensure proper endianness and alignment

  //   crc = reverse_byte_order(crc);
  //   // @todo: discard package if ack gots lost
  //   // @todo: if crc==0, then no ack is required
  //   if (crc == crc_expected)
  //   {
  //     AckPackage_t *ackPackage = (AckPackage_t*) rx_answer;
  //     switch (ackPackage->typeOfAckPackage)
  //     {
  //     case PackageAcknowledge:
  //       break;
  //     case DateTimeUpdate:
  //       char *tmp = ackPackage->ackPayload;
  //       DateRepresentation_t* date = (DateRepresentation_t*) tmp;
  //       tmp += sizeof(DateRepresentation_t); 

  //       TimeRepresentation_t* time = (TimeRepresentation_t*) tmp;
  //       RTC_setTimeAndDate(time, date);
  //       break;
        
  //     default:
  //       break;
  //     }
  //   }
  // }
}

void apply_nrf_config(Nrf24l01Registers_t *nrf_registers)
{

  /************ Disable AA ***************/
#ifdef SHOCKY
  nrf_registers->en_aa = 0x3F; // ENAA_P0
  nrf_registers->setup_retr = 7;
  nrf_registers->en_rxaddr = 0x3F;
#else
  nrf_registers->en_aa = 0;
  nrf_registers->en_rxaddr = 1;

  // payload size of ack: 1 byte
  nrf_registers->rx_pw_p0 = 12;

#endif
  /************ 1Mbps data rate, 0dBm ***************/
  nrf_registers->rf_setup = 6;

  /************ 5 byte address width ***************/
  nrf_registers->setup_aw = 3;

  /************ Channel 3 ***************/
  nrf_registers->rf_ch = 3;

  nrf_registers->config = 0; 
  char tx[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
  char rx_p0[5] = {0xc8, 0xc8, 0xc8, 0xc8, 0xc8};

  for (unsigned int i = 0; i < sizeof(tx) / sizeof(char); i++)
  {
    nrf_registers->tx_addr[i] = tx[i];
  }

  for (unsigned int i = 0; i < sizeof(rx_p0) / sizeof(char); i++)
  {
    nrf_registers->rx_addr_p0[i] = rx_p0[i];
  }
}

static void init_irq()
{
  pinb.pin = 0;
  pinb.port = 'B';

  link_exti_src(tx_receive_isr, &pinb);
  syscfg_enable_clock();
  syscfg_exti_config_0_3(&pinb);
  exti_activate_ir_line(&pinb);
  exti_detect_falling_edge(&pinb);
}

char sendConfig()
{
  return 0;
#ifdef REWORK_NEEDED
  char outBuffer[32];
  for (unsigned int i = 0; i < 32; i++)
  {
    outBuffer[i] = 0;
  }
  RxConfig_t rxConfig;
  rxConfig.identifier = 0x12345678;
  rxConfig.configMask = OutputToUart;
  rxConfig.channel = 3;
  rxConfig.timeToSendAck = 0;
  rxConfig.timeToSettle = 0;
  char *payloadPtr = (char*) &rxConfig;
  for (unsigned int i = 0; i < sizeof(RxConfig_t); i++)
  {
    outBuffer[i] = payloadPtr[i];
  }
  return transmit_with_autoack(&tx_config, &receivedAckPackage, outBuffer);
#endif
}

void adc_isr()
{
    adc_acknowledge_interrupt();
    exti_acknowledge_interrupt(&gpio_adc_a1);
    battery = adc_read_regular_channel();
}

void led()
{
  while (1)
  {
    toggle_output_pin(&gpio_a3_notify);
    sleep(1000);
  }
}

void processMeasurement(NodePackage_t *package)
{
  unsigned int am2302Retries = 0;

  while (am2302Retries++ < Am2302RetryCount)
  {
    if (am2302_do_measurement(&package->readings))
    {
      break;
    }
  }
}

int __attribute((section(".main"))) __attribute__((__noipa__)) __attribute__((optimize("O0"))) main(void)
{
  NodePackage_t *package = (NodePackage_t*) outBuffer;
  NodeState_t nodeState = NodeOperation_ProcessWeatherdata;

  memset_byte((void*) outBuffer, 32, 0);
  memset_byte((void*) &tx_config, sizeof(TxConfig_t), 0);

  
  package->meta.nodeNumber = 15;
  package->header.size = sizeof(NodePackage_t) - sizeof(StructHeader_t);
  
  rx_answer = (char*) allocate(32);
  receivedAckPackage = 0;

  tx_config.autoRetransmitDelay = 170000;
  tx_config.retransmitCount = 7;
  timeToSettle = 150;

  FunctionPointer fp;
  fp.funcNoArg = read_timer;

  append_os_core_function(fp, readTimerFunctionPtr);

  fp.funcWithArg = allocate;
  append_os_core_function(fp, allocateFunctionPtr);

  fp.funcWithArgAndPtr = deallocate;
  append_os_core_function(fp, deallocateFunctionPtr);

  crc_activate();
  apply_nrf_config(&nrf_registers);
  configure_device(&nrf_registers, MASTER);
  init_irq();

  // append_os_core_function(read_timer);
  am2302_init_peripherials(0, 'A');

  gpio_adc_a1.pin = 1;
  gpio_adc_a1.port = 'A';
  adc_init(&gpio_adc_a1);
  adc_enable_interrupts();
  adc_turn_on();
  link_adc_isr(adc_isr);

  gpio_a3_notify.pin = 3;
  gpio_a3_notify.port = 'A';
  init_gpio(&gpio_a3_notify);
  set_moder(&gpio_a3_notify, GeneralPurposeOutputMode);
  set_pin_on(&gpio_a3_notify);

  char *transmitterState;

  // @multiple pid sleep must be implemented/fixed (RTOS_MultipleSleep)
  // unsigned int pid = create_task(&led, 0);
  // join_task(pid);
 
  // Uncomment as soon as HAL Test complete
  // if (!sendConfig())
  // {
  //   NodePackage_t *package = (NodePackage_t*) outBuffer;
  //   package->header.version = 99;
  //   print((char*) &outBuffer, sizeof(NodePackage_t));
  //   return 0;
  // };
  // {
  //   char dateTimeIsUpToDate = 0;
  //   DateRepresentation_t currentDate;
  //   do 
  //   {
  //     RTC_readCurrentDate(&currentDate);
  //     if (currentDate.year == 0) // year 2000 is default value
  //     {
  //       package->meta.packageType = (char) DateTimeRequest;
  //       transmit_with_autoack(&tx_config, &receivedAckPackage, outBuffer);
  //       sleep(1000);
  //     }
  //     else 
  //     {
  //       dateTimeIsUpToDate = 1;
  //     }
  //   } while (!dateTimeIsUpToDate); 
  // }

  while (1)
  {
    char *payloadPtr = outBuffer;
    unsigned int basePayloadWeight = 0;
    switch (nodeState)
    {
    case NodeOperation_ProcessWeatherdata:
      processMeasurement(package);
      // basePayloadWeight += sizeof(Am2302Readings_t);
      package->meta.packageType = (char) PackageType_WeatherData;
      break;    
    case NodeOperation_RequestLifetime:
    {
      basePayloadWeight += sizeof(OsLifetime_t);

      break;
    }
    default:
      break;
    }

    observe = get_current_tx_state();

    package->environment.retransmitsForThisPackage = observe.retransmitCount;
    // @todo : don't cast datatypes
    package->environment.signalStrength = (char) observe.signalStrength;
    // @todo : don't cast datatypes
    package->environment.totalLostPackages = (char) observe.totalLostPackages;
    // package->environment.batteryHealth = battery;
    package->environment.currentChannel = observe.currentChannel;

    // @todo: Move to own module
    // unsigned int numberOfPackages = sizeof

    for (int j = 1; j < 52; j++)
    {
      outBuffer[j] = j;
    }
    // for (int j = 26; j < 52; j++)
    // {
    //   outBuffer[j] = 0xB;
    // }
    // for (int j = 52; j < 78; j++)
    // {
    //   outBuffer[j] = 0xC;
    // }

    RxAckStatusMask_t rxAckStatus = transmit_with_autoack(&tx_config, 
                                                          &receivedAckPackage, 
                                                          outBuffer, 
                                                          rx_answer, 
                                                          27);
    
    if (rxAckStatus & RxAckReceived)
    {
      if (rxAckStatus & RxAckCRCMatch)
      {
        AckPayload_t *ackPayload = (AckPayload_t*) rx_answer;

        switch ((AckTypes_t) ackPayload->ackType)
        {
        case Ack_Default:
          nodeState = NodeOperation_ProcessWeatherdata;
          package->environment.batteryHealth = 0;
          break;
        case Ack_ContainsDate:
          RTC_setTimeAndDate(NULL, &ackPayload->specific.date.dateRepresentation);
          nodeState = NodeOperation_ProcessWeatherdata;

          break;
        case Ack_ContainsTime:
          RTC_setTimeAndDate(&ackPayload->specific.time.timeRepresentation, NULL);
          nodeState = NodeOperation_ProcessWeatherdata;
          break;
        case Ack_RequestLifetime:
          // send Lifetime, wip
          unsigned int *healthPtr = (unsigned int *) allocate(sizeof(OsLifetime_t));
          svcall_param(13, (unsigned int) healthPtr);
          OsLifetime_t *health = (OsLifetime_t *) healthPtr;
          nodeState = NodeOperation_RequestLifetime;
          break;
        case Ack_RequestErrorCode:
          // send errorcode
          package->environment.batteryHealth = 5;
          break;
        case Ack_RequestErrorLog:
          // send errorlog
          package->environment.batteryHealth = 6;
          break;
        default:
          nodeState = NodeOperation_ProcessWeatherdata;
          break;
        }
      }
      else 
      {
          package->environment.batteryHealth = 7;
      }
    }
    // memcpy_custom(&package->meta, rx_answer, 12);
    print((char*) &outBuffer, sizeof(NodePackage_t));
    // request_channel_change(&tx_config, &receivedAckPackage);
    adc_start_conv_regular();

    sleep(3000);
  }
  return 0;
}
