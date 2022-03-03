/* BB2022 */

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
#define RCC_BASE    (PERIPH_BASE | AHB_BASE | GPIOC_BASE)
#define RCC_APB2ENR (RCC_BASE+0x18) // 0x18 offset - RCC_APB2ENR
#define RCC_APB1ENR (RCC_BASE+0x1C) // 0x1C offset - RCC_APB1ENR
#define RCC_CR      (RCC_BASE+0x00) // 0x00 offset - CR(control register)
#define RCC_CFGR    (RCC_BASE+0x04) // 0x04 offset - CFGR
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
void blinker ( unsigned int n )
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

int main ( void )
{    
    unsigned int ra;             

    clrbits(RCC_CFGR, 0xF8FF0000);  // Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits 
    clrbits(RCC_CR, 0xFEF6FFFF);    // Reset HSEON, CSSON and PLLON bits 
    clrbits(RCC_CR, 0xFFFBFFFF);    // Reset HSEBYP bit     
    clrbits(RCC_CR,0xFF80FFFF);     // Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits 
    setbits(RCC_CR,0x00010000);      // Enable HSE_ON bit 16  already in blinker    
    while(1) if(GET32(RCC_CR) & (0x00020000)) break;   // wait for ready flag (bit 17 HSERDY) already in blinker

    setbits(FLASH_ACR, 0x10); // Enable Prefetch Buffer 
    clrbits(FLASH_ACR,~0x03); // Flash 0 wait state 
    setbits(FLASH_ACR,0x00); // Flash latency , no effect
    setbits(RCC_CFGR, 0x00000000); // HCLK = SYSCLK, RCC_CFGR_HPRE_DIV1 = 0; no effect
    setbits(RCC_CFGR, 0x00000000); // PCLK2 = HCLK ,RCC_CFGR_PPRE2_DIV1; no effect
    setbits(RCC_CFGR, 0x00000000); // PCLK1 = HCLK, RCC_CFGR_PPRE1_DIV1;    
    clrbits(RCC_CFGR, ~(0x00000003)); // Select HSE as system clock source, ~(RCC_CFGR_SW)
    setbits(RCC_CFGR,0x00000001);   // RCC_CFGR_SW_HSE
    while(1) if(GET32(RCC_CFGR) & (0xC)) break;   // Wait till HSE is used as system clock source 
    
    // enable clocks 
    setbits(RCC_APB1ENR,0x00020000); // set bi 17, UART2_EN clock enable
    setbits(RCC_APB2ENR, 0x00000001); // RCC_APB2ENR_AFIOEN    
    setbits(RCC_APB2ENR, 0x00000004); // RCC_APB2ENR_IOPAEN
    

    /*******************************************************/
    /**************** BLINKER WITH HSE *********************/
    /*******************************************************/
    setbits(RCC_CR, (1<<16));                       // set bit 16 HSE_ON (8 Mhz xtal)
    while(1) if(GET32(RCC_CR) & (1<<17)) break;     // wait for ready flag (bit 17 HSERDY)
    
    clrbits(RCC_CFGR, ~(0x3<<0));                   // clear last two bits (system clock switch)
    setbits(RCC_CFGR, (0x1<<0));                    // set last bit (01 - HSE selected as system clock)
    while(1) if((GET32(RCC_CFGR)&0xF)==0x5) break;  // wait for ready flag

    setbits(RCC_APB2ENR, 1<<4);                     //enable clock set bit 4 (IOPCEN) port C clock enable 

    clrbits(GPIOC | GPIOX_CRH, ~(3<<20));             // clear bits [21:20] (MODE13)
    setbits(GPIOC | GPIOX_CRH, 1<<20);                // select output mode 10Mhz  
    clrbits(GPIOC | GPIOX_CRH, ~(3<<22));             // clear bits [23:22] (CNF13)    
    setbits(GPIOC | GPIOX_CRH, 0<<22);                // select output as PUSH-PULL    

    blinker(3); // blink with HSE as system clock 

    /*******************************************************/
    /**************** Blinker with PLL**********************/
    /*******************************************************/

    // // set RCC configuration regeister (RCC_CFGR offset 0x04)
    // ra = 0;             // clear register 
    // ra |= 0x7<<24;      // MCO:111 - pll selecte
    // ra |= 0x0<<22;      // USB prescaler 
    // ra |= 0x7<<18;      // PLLMUL je 0111(9) x 8Mhz = 72Mhz
    // ra |= 0x0<<17;      // PLL_XTPRE: 0-HSE, 1-HSE/2
    // ra |= 0x1<<16;      // PLL_SRC: 0-HSI/2, 1-HSE e
    // ra |= 0x0<<14;      // ADCPRE
    // ra |= 0x0<<11;      // PPRE2 // APB2 prescaler not divided 0xx
    // ra |= 0x4<<8;       // PPRE1 // APB1 precaler not divided 0xx
    // ra |= 0x0<<4;       // HPRE // AHB prescaer, Sys clock no divided
    // PUT32(RCC_CFGR,ra); // put back modified val 

    // // ResetClockController_ControlRegister 
    // ra = GET32(RCC_CR);                         // get register
    // ra |= 1<<24;                                // turn on PLL (bit PLLON)
    // PUT32(RCC_CR,ra);                           // write back modified value
    // while(1) if(GET32(RCC_CR)&(1<<25)) break;   // wait for ready flag (bit 25 PLLRDY)

    // // Flash access control register, defines latency in clock_cycles for reading flash
    // PUT32(FLASH_ACR,0x4);                       // flash memory interface 0x40022000-0x400223FF 

    // ra = GET32(RCC_CFGR);                           // get register 
    // ra &= ~(0x3<<0);                                // clear bits [1:0] (system clock switch)
    // ra |= (0x2<<0);                                 // set bit [1] PLL selected as system clock
    // PUT32(RCC_CFGR,ra);                             // put back modified value  
    // while(1) if((GET32(RCC_CFGR)&0xF)==0xA) break;  // wait for 2 flags, bit [3] - PLL used as sysclk (switch status set by hw)
    //                                                 //                   bit [1] - PLL selected as sysclk (switch set by hw, sets HSI if HSE breaks)
    
    // // blink with frequency determined by PLL
    // blinker(30000);

    
    /*******************************************************/
    /********** USART 2 CONFIG  PA2:TX, PA3:RX  ************/
    /*******************************************************/
    // pa2 tx 
    setbits(GPIOA | GPIOX_CRL, 1 << 11); // CNF2 pushpull
    setbits(GPIOA | GPIOX_CRL, 1 << 9); // mode2 mhz
    // pa3 rx 
    setbits(GPIOA | GPIOX_CRL, 1 << 14);  // CNF3 floatin (reset state)
    clrbits(GPIOA | GPIOX_CRL, 0xFFFFCFFF); // MODE3[00] input == reset state

    clrbits(USART2 | USART_CR1, 0xE9F3 ); // CR1_CLEAR_Mask;
    setbits(USART2 | USART_CR1, 0x000C); // bit 2-RXE, 3-TXE, 10[0]-NO parity, 12[0]-word8bit

    clrbits(USART2 | USART_CR2, 0xCFFF); // CR2_STOP_CLEAR_Mask;
    setbits(USART2 | USART_CR2, 0xCFFF); //STOP[13,12]: 00 - 1 stop bit 

    clrbits(USART2 | USART_CR3, 0xFCFF); //STOP[13,12]: 00 - 1 stop bit 
    clrbits(USART2 | USART_CR3, 0x0000); //bit 9-CTSE 0: disabled, 8-RTSE 0: disabled

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

    while((USART2 | USART_SR) & 0x0080); // wait forready flag, SR_TXE .. status register tx enable 
    
    setbits(USART2 | USART_DR, 0x42); // send character 'B'       
    
    


    return(0);
}


