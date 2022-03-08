/* BB2022 */
#include <stdio.h>


// AMBA bus and other periphery registers 
#define PERIPH_BASE  0x40000000
#define APB1_BASE    0x00000000
#define APB2_BASE    0x00010000
#define AHB_BASE     0x00020000
// GeneralPurposeInputOutput register
#define GPIOA_BASE   0x00000800 //offset
#define GPIOB_BASE   0x00000C00 //offset
#define GPIOC_BASE   0x00001000 //offset 
#define GPIOC        (PERIPH_BASE | APB2_BASE | GPIOC_BASE)
#define GPIOA        (PERIPH_BASE | APB2_BASE | GPIOA_BASE)
#define GPIOX_CRL    0x00
#define GPIOX_CRH    0x04 
#define GPIOX_BSRR   0x10
// ResetClockController registers
#define RCC_BASE     0x1000
#define RCC         (PERIPH_BASE | AHB_BASE | RCC_BASE)
#define RCC_APB2ENR (RCC + 0x18) // 0x18 offset - RCC_APB2ENR
#define RCC_APB1ENR (RCC + 0x1C) // 0x1C offset - RCC_APB1ENR
#define RCC_CR      (RCC + 0x00) // 0x00 offset - CR(control register)
#define RCC_CFGR    (RCC + 0x04) // 0x04 offset - CFGR
// USART2 
#define USART2_BASE 0x4400 //offset
#define USART2      (PERIPH_BASE | APB1_BASE | USART2_BASE)
#define USART_CR1   0x0C
#define USART_CR2   0x10
#define USART_CR3   0x14
#define USART_BRR   0x08 // baud rate register
#define USART_SR    0x00 // status register 
#define USART_DR    0x04

// FLASH 
#define FLASH_BASE  0x2000 //offset
#define FLASH       (PERIPH_BASE | AHB_BASE | FLASH_BASE) 
#define FLASH_ACR   FLASH


// Assembly functions
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void DUMMY ( unsigned int );

// C functions 
void setbits(unsigned int reg, unsigned int bits){
    unsigned int ra = GET32(reg);                         
    ra |= bits;                             
    PUT32(reg,ra);                          
}
void clrbits(unsigned int reg, unsigned int bits){
    unsigned int ra = GET32(reg);                         
    ra &= bits;                             
    PUT32(reg,ra);   
}
void blink(unsigned int n )
{
    unsigned int i,j = 0;
    while(j++ < n){
        i  = 200000;
        PUT32((GPIOC | GPIOX_BSRR), 1<<13);    // write bit 13 (BS13 bit-set) in BSRR
        while(i--) DUMMY(i);                   // dummy delay

        i = 200000;
        PUT32((GPIOC | GPIOX_BSRR), 1<<29);     // write bit 29 (BR13 bit-reset) in BSSR
        while(i--) DUMMY(i);                    // dummy delay
    }
}


void _exit(int status){

};
void* _sbrk(intptr_t increment){

};
ssize_t _write(int fd, const void *buf, size_t count){

};

void _close(void){

};

void _fstat(void){

};

void _isatty(void){

};

void _lseek(void){

};

void _read(void){

};

void _kill(void){

};
void _getpid(void){

};



int main ( void )
{    

    clrbits(RCC_CFGR, 0xF8FF0000);  // Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits 
    clrbits(RCC_CR, 0xFEF6FFFF);    // Reset HSEON, CSSON and PLLON bits 
    clrbits(RCC_CR, 0xFFFBFFFF);    // Reset HSEBYP bit     
    clrbits(RCC_CR, 0xFF80FFFF);     // Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits 
    setbits(RCC_CR, 0x00010000);     // Enable HSE_ON bit 16  already in blinker    
    while(1) if(GET32(RCC_CR) & (0x00020000)) break;   // wait for ready flag (bit 17 HSERDY) already in blinker

    setbits(FLASH_ACR, 0x10);           // Enable Prefetch Buffer 
    clrbits(FLASH_ACR,~0x03);           // Flash 0 wait state 
    setbits(FLASH_ACR,0x00);            // Flash latency , no effect
    setbits(RCC_CFGR, 0x00000000);      // HCLK = SYSCLK, RCC_CFGR_HPRE_DIV1 = 0; no effect
    setbits(RCC_CFGR, 0x00000000);      // PCLK2 = HCLK ,RCC_CFGR_PPRE2_DIV1; no effect
    setbits(RCC_CFGR, 0x00000000);      // PCLK1 = HCLK, RCC_CFGR_PPRE1_DIV1;    
    clrbits(RCC_CFGR, ~(0x00000003));   // Select HSE as system clock source, ~(RCC_CFGR_SW)
    setbits(RCC_CFGR, 0x00000001);      // RCC_CFGR_SW_HSE
    while(1) if(GET32(RCC_CFGR) & (0xC)) break;   // Wait till HSE is used as system clock source 
    
    // enable clocks 
    setbits(RCC_APB1ENR, 0x00020000);       // RCC_APB1ENR_USART2EN bit 17
    setbits(RCC_APB2ENR, 0x00000001);       // RCC_APB2ENR_AFIOEN    
    setbits(RCC_APB2ENR, 0x00000004);       // RCC_APB2ENR_IOPAEN
    setbits(RCC_APB2ENR, 1<<4);             // RCC_APB2ENR_IOPCEN port C 
    
    // pc13 
    clrbits(GPIOC | GPIOX_CRH, ~(3<<20));   // clear bits [21:20] (MODE13)
    setbits(GPIOC | GPIOX_CRH, 1<<20);      // select output mode 10Mhz  
    clrbits(GPIOC | GPIOX_CRH, ~(3<<22));   // clear bits [23:22] (CNF13)    
    setbits(GPIOC | GPIOX_CRH, 0<<22);      // select output as PUSH-PULL    

    // pa2 usart2 tx 
    setbits(GPIOA | GPIOX_CRL, 1 << 11);    // CNF2 pushpull
    setbits(GPIOA | GPIOX_CRL, 1 << 8);     // MODE2 bits [98]  00 input, 01 output 10Mhz, 10 output 2Mhz, 11 output 50Mhz

    // pa3 usart2 rx 
    setbits(GPIOA | GPIOX_CRL, 1 << 14);    // CNF3 floatin (reset state)
    clrbits(GPIOA | GPIOX_CRL, 0xFFFFCFFF); // MODE3[00] input == reset state

    clrbits(USART2 | USART_CR1, 0xE9F3 );   // CR1_CLEAR_Mask;
    setbits(USART2 | USART_CR1, 0x000C);    // [2]:(1)RXE enabled, [3]:(1)TXE enabled, [10]: 0 no parity, [12]:(0) 1S-8D-nP, (1) 1S-9D-nP

    //clrbits(USART2 | USART_CR2, 0xCFFF);    // CR2_STOP_CLEAR_Mask;
    //setbits(USART2 | USART_CR2, 0xCFFF);    // STOP[13,12]: 00 - 1 stop bit 

    clrbits(USART2 | USART_CR3, 0xFCFF);     // clears bits 9, 8, 
    clrbits(USART2 | USART_CR3, 0x0200);    // bit 9-CTSE 1: disabled, 8-RTSE 0: disabled

    // usart2 baud rate 
    unsigned int tmpreg = 0x00, apbclock = 0x00;
    unsigned int integerdivider = 0x00;
    unsigned int fractionaldivider = 0x00;

    apbclock = 8000000;
    integerdivider = ((25 * apbclock) / (4 * (9600))); // 4 normal, 2 oversampling
    tmpreg = (integerdivider / 100) << 4;
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((unsigned char)0x0F); // 0x0F normal, 0x07 oversampling

    setbits(USART2 | USART_BRR, tmpreg); // write to baudrate register
    setbits(USART2 | USART_CR1, 0x2000); // enable usart (CR1_UE_Set)

    
   
    
      
    printf("hello");
    blink(5); // blink with HSE as system clock 

    

    while (1)
    {
    while(1) if(GET32(USART2 | USART_SR) & (0x0080)) break; // SR_TXE, 1 when data moved to shift reg
    setbits(USART2 | USART_DR, (unsigned char) 'B'); 
    blink(1); // blink with HSE as system clock 
    }
    


    return(0);
}


