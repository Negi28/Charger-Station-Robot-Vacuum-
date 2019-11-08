#include <iostm8s103f3.h>
#include <intrinsics.h>
#include <stdlib.h>

//
//  Data ready for the pulse timer ISR's to use.
//
unsigned char *_counterHighBytes = NULL;
unsigned char *_counterLowBytes = NULL;
int _numberOfPulses = 0;
int _currentPulse = 0;

unsigned int gPulseLength[55] = {9000U, 4500U, 600U, 500U, 600U, 1500U, 600U, 500U, 600U, 1500U, 600U, 500U, 600U, 500U, 600U, 1500U, 800U, 8000U, \
								 9000U, 4500U, 600U, 500U, 600U, 500U, 600U, 1500U, 600U, 500U, 600U, 1500U, 600U, 500U, 600U, 1500U, 800U, 4000U, \
								 9000U, 4500U, 600U, 500U, 600U, 1500U, 600U, 500U, 600U, 1500U, 600U, 500U, 600U, 1500U, 600U, 500U, 800U, 40000U, 53000U};

unsigned char gIRLogic[55][3] = {{0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, {0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34},{0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, {0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, {0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, {0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, {0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, {0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, {0x1A, 0x34, 0x34}, {0x34, 0x34, 0x34}, \
								{0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, {0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34},{0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, {0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, {0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, {0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, {0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, {0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, {0x34, 0x1A, 0x34}, {0x34, 0x34, 0x34}, \
								{0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34},{0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x1A}, {0x34, 0x34, 0x34}, {0x34, 0x34, 0x34}};


//--------------------------------------------------------------------------------
//
//  Prepare the data for the timer ISRs.
//
void PrepareCounterData(unsigned int *pulseDuration, unsigned int numberOfPulses)
{
    _numberOfPulses = numberOfPulses;
    if (_counterHighBytes != NULL)
    {
        free(_counterHighBytes);
        free(_counterLowBytes);
    }
    _counterHighBytes = (unsigned char *) malloc(numberOfPulses);
    _counterLowBytes = (unsigned char *) malloc(numberOfPulses);
	
    for (int index = 0; index < numberOfPulses; index++)
    {
		pulseDuration[index] -= 50;
        _counterLowBytes[index] = (unsigned char) (pulseDuration[index] & 0xff);
        _counterHighBytes[index] = (unsigned char) (((pulseDuration[index] & 0xff00) >> 8) & 0xff);
    }
    _currentPulse = 0;
}


void SetupTimer2()
{
	// Set Frequency of PWM is 38.4KHz
	TIM2_PSCR = 0;         
    TIM2_ARRH = 0x00;       
    TIM2_ARRL = 0x33;
	
	// Set Timer 2 Channel 1 with pulse 50%
   	TIM2_CCR1H = 0x00;     
    TIM2_CCR1L = 0x1A;     
    TIM2_CCER1_CC1P = 0;    //  Active high.
    TIM2_CCER1_CC1E = 1;    //  Enable compare mode for channel 1
    //TIM2_CCMR1_OC1M = 6;  //  PWM Mode 1 - active if counter < CCR1, inactive otherwise.
	TIM2_CCMR1_OC1M = 7;    //  PWM Mode 2
	
	// Set Timer 2 Channel 2 with pulse 50%
   	TIM2_CCR2H = 0x00;     
    TIM2_CCR2L = 0x1A;     
    TIM2_CCER1_CC2P = 0;    //  Active high.
    TIM2_CCER1_CC2E = 1;    //  Enable compare mode for channel 1
    //TIM2_CCMR1_OC1M = 6;  //  PWM Mode 1 - active if counter < CCR1, inactive otherwise.
	TIM2_CCMR2_OC2M = 7;    //  PWM Mode 2
	
	// Set Timer 2 Channel 3 with pulse 50%
   	TIM2_CCR3H = 0x00;     
    TIM2_CCR3L = 0x1A;     
    TIM2_CCER2_CC3P = 0;    //  Active high.
    TIM2_CCER2_CC3E = 1;    //  Enable compare mode for channel 1
    //TIM2_CCMR1_OC1M = 6;    //  PWM Mode 1 - active if counter < CCR1, inactive otherwise.
	TIM2_CCMR3_OC3M = 7;    //  PWM Mode 2
		
    TIM2_CR1_CEN = 1;       //  Finally enable the timer.
}

void SetupTimer1()
{
    TIM1_PSCRH = 0;         //  Set Timer 1 clock is 1MHz
    TIM1_PSCRL = 1;
	TIM1_IER_UIE = 1;       //  Enable the update interrupts.

	TIM1_CR1_URS = 1;
    TIM1_EGR_UG = 1;
	
    TIM1_CR1_CEN = 1;
}

//--------------------------------------------------------------------------------
//
//  Main program loop.
//
void main()
{
    PrepareCounterData(gPulseLength, 55);
    __disable_interrupt();
    SetupTimer2();
	SetupTimer1();
    __enable_interrupt();

    while (1)
    {
        __wait_for_interrupt();
    }
}

//--------------------------------------------------------------------------------
//
//  Timer 1 Overflow handler.
//
#pragma vector = TIM1_OVR_UIF_vector
__interrupt void TIM1_UPD_OVF_IRQHandler(void)
{
	_currentPulse++;
	if (_currentPulse == _numberOfPulses)
	{
		_currentPulse = 0;
	}
	
	TIM1_ARRH = _counterHighBytes[_currentPulse];
	TIM1_ARRL = _counterLowBytes[_currentPulse];
	
	// FIR 1
	TIM2_CCR1H = 0x00;     
    TIM2_CCR1L = gIRLogic[_currentPulse][0];  
	
	// FIR 2
	TIM2_CCR2H = 0x00;     
    TIM2_CCR2L = gIRLogic[_currentPulse][1]; 
	
	// FIR 3
	TIM2_CCR3H = 0x00;     
    TIM2_CCR3L = gIRLogic[_currentPulse][2]; 
	
	TIM1_CR1_URS = 1;
	TIM1_EGR_UG = 1;
	TIM1_SR1_UIF = 0;               //  Reset the interrupt otherwise it will fire again straight away.
}
