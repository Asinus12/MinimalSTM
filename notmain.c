

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

	

#define PERIPH_BASE  0x40000000
#define APB2_BASE    0x00010000
#define GPIOC_BASE   0x00001000
#define GPIOC        0x40011000 
#define CRH          0x04 
#define GPIOX_BSRR   0x10

// ResetClockController register
#define RCC_BASE 0x40021000  // PERIPH_BASE | AHB_BASE | GPIOC_BASE 
#define RCC_APB2ENR (RCC_BASE+0x18) // 0x18 offset - RCC_APB2ENR
#define RCC_APB1ENR (RCC_BASE+0x1C) // 0x1C offset - RCC_APB1ENR
#define RCC_CR      (RCC_BASE+0x00) // 0x00 offset - CR(control register)
#define RCC_CFGR    (RCC_BASE+0x04) // 0x04 offset - CFGR

#define FLASH_ACR   0x40022000



void blinker ( unsigned int n )
{
    unsigned int i;
    unsigned int j;

    for(j=0;j<n;j++)
    {
        PUT32(GPIOC+GPIOX_BSRR, 1<<(13+ 0));    // write BS13 (bit set) in BSRR
        for(i=0;i<200000;i++) dummy(i);         // dummy delay
        
        PUT32(GPIOC+GPIOX_BSRR,1<<(13+16));     // write BR13 (bit reset) in BSSR
        for(i=0;i<200000;i++) dummy(i);         // dummy delay
    }
}

int notmain ( void )
{
    unsigned int ra;

    // turn on High Speed External oscillator 
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


    // enable system clock for port c (GPIOC)
    ra = GET32(RCC_APB2ENR);                        // get register
    ra |= 1<<4;                                     // set bit 4 (IOPCEN) port C clock enable 
    PUT32(RCC_APB2ENR,ra);                          // put back modified value 
        

    // set Port configuration register high
    ra = GET32(GPIOC+CRH);                          // get register 
    ra &= ~(3<<20);                                 // clear bits [21:20] (MODE13)
    ra |=   1<<20;                                  // select output mode 10Mhz  
    ra &= ~(3<<22);                                 // clear bits [23:22] (CNF13)    
    ra |=   0<<22;                                  // select output as PUSH-PULL    
    PUT32(GPIOC+CRH,ra);                            // put back modified value 


    // blink with frequency determined by HSE 
    blinker(5);

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



    PUT32(FLASH_ACR,0x2);


    ra = GET32(RCC_CFGR);                           // get register 
    ra &= ~(0x3<<0);                                // clear bits [1:0] (system clock switch)
    ra |= (0x2<<0);                                 // set bit [1] PLL selected as system clock
    PUT32(RCC_CFGR,ra);                             // put back modified value  
    while(1) if((GET32(RCC_CFGR)&0xF)==0xA) break;  // wait for 2 flags, bit [3] - PLL used as system clock (switch status, set by hardware)
                                                    //                   bit [1] - PLL selected as system clock (switch,set by hardware, sets HSI if HSE breaks)
    

    // blink with frequency determined by PLL
    blinker(50000);
    
    return(0);
}
