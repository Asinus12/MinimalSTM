/* BB2022 */


// AMBA bus and other periphery registers 
#define PERIPH_BASE  0x40000000
#define APB2_BASE    0x00010000
#define AHB_BASE     0x00020000
// GeneralPurposeInputOutput register
#define GPIOC_BASE   0x00001000
#define GPIOC        (PERIPH_BASE | APB2_BASE | GPIOC_BASE)
#define GPIOX_CRH    0x04 
#define GPIOX_BSRR   0x10
// ResetClockController registers
#define RCC_BASE    (PERIPH_BASE | AHB_BASE | GPIOC_BASE)
#define RCC_APB2ENR (RCC_BASE+0x18) // 0x18 offset - RCC_APB2ENR
#define RCC_APB1ENR (RCC_BASE+0x1C) // 0x1C offset - RCC_APB1ENR
#define RCC_CR      (RCC_BASE+0x00) // 0x00 offset - CR(control register)
#define RCC_CFGR    (RCC_BASE+0x04) // 0x04 offset - CFGR
// Flash registers 
#define FLASH_ACR   0x40022000




void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void DUMMY ( unsigned int );


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

int notmain ( void )
{
    unsigned int ra;

    /*******************************************************/
    /**************** BLINKER WITH HSE *********************/
    /*******************************************************/

    // turn on High Speed External oscillator (8Mhz xtal)
    ra = GET32(RCC_CR);                           // get register 
    ra |= 1 << 16;                                // set bit 16 (HSEON)
    PUT32(RCC_CR,ra);                             // put back modified value  
    while(1) if(GET32(RCC_CR) & (1<<17)) break;   // wait for ready flag (bit 17 HSERDY)
    
    // select HSE as system clock
    ra = GET32(RCC_CFGR);                           // get register 
    ra &= ~(0x3<<0);                                // clear last two bits (system clock switch)
    ra |= (0x1<<0);                                 // set last bit (01 - HSE selected as system clock)
    PUT32(RCC_CFGR,ra);                             // put back modified value 
    while(1) if((GET32(RCC_CFGR)&0xF)==0x5) break;  // wait for ready flag

    // enable system clock for port C (GPIOC)
    ra = GET32(RCC_APB2ENR);                        // get register
    ra |= 1<<4;                                     // set bit 4 (IOPCEN) port C clock enable 
    PUT32(RCC_APB2ENR,ra);                          // put back modified value 
        
    // set Port configuration register high
    ra = GET32(GPIOC+GPIOX_CRH);                    // get register 
    ra &= ~(3<<20);                                 // clear bits [21:20] (MODE13)
    ra |=   1<<20;                                  // select output mode 10Mhz  
    ra &= ~(3<<22);                                 // clear bits [23:22] (CNF13)    
    ra |=   0<<22;                                  // select output as PUSH-PULL    
    PUT32(GPIOC+GPIOX_CRH,ra);                      // put back modified value 

    // blink with frequency determined by HSE 
    blinker(5);


    /*******************************************************/
    /**************** Blinker with PLL**********************/
    /*******************************************************/

    // set RCC configuration regeister (RCC_CFGR offset 0x04)
    ra = 0;             // clear register 
    ra |= 0x7<<24;      // MCO:111 - pll selecte
    ra |= 0x0<<22;      // USB prescaler 
    ra |= 0x7<<18;      // PLLMUL je 0111(9) x 8Mhz = 72Mhz
    ra |= 0x0<<17;      // PLL_XTPRE: 0-HSE, 1-HSE/2
    ra |= 0x1<<16;      // PLL_SRC: 0-HSI/2, 1-HSE e
    ra |= 0x0<<14;      // ADCPRE
    ra |= 0x0<<11;      // PPRE2 // APB2 prescaler not divided 0xx
    ra |= 0x4<<8;       // PPRE1 // APB1 precaler not divided 0xx
    ra |= 0x0<<4;       // HPRE // AHB prescaer, Sys clock no divided
    PUT32(RCC_CFGR,ra); // put back modified val 

    // ResetClockController_ControlRegister 
    ra = GET32(RCC_CR);                         // get register
    ra |= 1<<24;                                // turn on PLL (bit PLLON)
    PUT32(RCC_CR,ra);                           // write back modified value
    while(1) if(GET32(RCC_CR)&(1<<25)) break;   // wait for ready flag (bit 25 PLLRDY)

    PUT32(FLASH_ACR,0x2);                       // flash memory interface 0x40022000-0x400223FF 

    ra = GET32(RCC_CFGR);                           // get register 
    ra &= ~(0x3<<0);                                // clear bits [1:0] (system clock switch)
    ra |= (0x2<<0);                                 // set bit [1] PLL selected as system clock
    PUT32(RCC_CFGR,ra);                             // put back modified value  
    while(1) if((GET32(RCC_CFGR)&0xF)==0xA) break;  // wait for 2 flags, bit [3] - PLL used as sysclk (switch status set by hw)
                                                    //                   bit [1] - PLL selected as sysclk (switch set by hw, sets HSI if HSE breaks)
    
    // blink with frequency determined by PLL
    blinker(50000);
    



    while(1);
    return(0);
}
