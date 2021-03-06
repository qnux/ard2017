/**
 * \file
 * \brief FreeRTOS for Due and Teensy 3.0
 */
#include "FreeRTOS_ARM.h"
#include "Arduino.h"
#include "BSP.h"

//ARD map LED to blink
#define ERROR_PIN       LED_DUE_L
#define HEARTBEAT_PIN    LED_DUE_RX

//------------------------------------------------------------------------------
/** calibration factor for delayMS */
#define CAL_FACTOR (F_CPU/7000)
/** delay between led error flashes
 * \param[in] millis milliseconds to delay
 */
static void delayMS(uint32_t millis) {
  uint32_t iterations = millis * CAL_FACTOR;
  uint32_t i;
  for(i = 0; i < iterations; ++i) {
    asm volatile("nop\n\t");
  }
}

//ARD
#define UNUSED(x) ((void)(x))
__attribute__((optimize(0))) void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress ) 
{
	/* These are volatile to try and prevent the compiler/linker optimising them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];
	
	//prevent compiler warnings as variables are never used (except by the debugger)
	UNUSED(r0);
	UNUSED(r1);
	UNUSED(r2);
	UNUSED(r3);
	UNUSED(r12);
	UNUSED(lr);
	UNUSED(pc);
	UNUSED(psr);
}

//------------------------------------------------------------------------------
/** Blink error pattern
 *
 * \param[in] n  number of short pulses
 */
void errorBlink(int n) {
	noInterrupts();
		
  pinMode(13, OUTPUT);
  for (;;) {
    int i;
    for (i = 0; i < n; i++) {
      digitalWrite(ERROR_PIN, 1);
      delayMS(300);
      digitalWrite(ERROR_PIN, 0);
      delayMS(300);
    }
    delayMS(2000);
  }
}

//------------------------------------------------------------------------------
	/** vApplicationMallocFailedHook()
   Blink two short pulses if malloc fails.

	will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
void vApplicationMallocFailedHook() {
  errorBlink(2);
}
/*-----------------------------------------------------------*/
	/**  Blink three short pulses if stack overflow is detected.
	Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.
  \param[in] pxTask Task handle
  \param[in] pcTaskName Task name
  */
__attribute__((optimize(0))) void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) 
{
	UNUSED(pcTaskName);
	UNUSED(pxTask);
	errorBlink(4);
}
//------------------------------------------------------------------------------
// catch Teensy 3 and Due exceptions
/** Hard fault - blink four short flash every two seconds */
void hard_fault_isr()	{errorBlink(5);}
/** Hard fault - blink four short flash every two seconds */
__attribute__((optimize(0))) void HardFault_Handler() 	 
{	//ARD : from http://www.freertos.org/Debugging-Hard-Faults-On-Cortex-M-Microcontrollers.html
	    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
	errorBlink(1);
	}

/** Bus fault - blink five short flashes every two seconds */
void bus_fault_isr() {errorBlink(5);}
/** Bus fault - blink five short flashes every two seconds */
void BusFault_Handler() {errorBlink(5);}

/** Usage fault - blink six short flashes every two seconds */
void usage_fault_isr() {errorBlink(6);}
/** Usage fault - blink six short flashes every two seconds */
void UsageFault_Handler() {errorBlink(6);}

//ARD : see FreeRTOSConfig.h
void enterIdleCB()
{
    digitalWrite(HEARTBEAT_PIN, LOW);
}

//ARD : see FreeRTOSConfig.h
void exitIdleCB()
{
    digitalWrite(HEARTBEAT_PIN, HIGH);
}
