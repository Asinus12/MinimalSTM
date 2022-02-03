/* compile with 
arm-none-eabi-as --warn --fatal-warnings -mcpu=cortex-m3 flash.s -o flash.o
arm-none-eabi-gcc -Wall -O2 -ffreestanding -mcpu=cortex-m3 -mthumb -c notmain.c -o notmain.o
arm-none-eabi-ld -nostdlib -nostartfiles -T flash.ld flash.o notmain.o -o notmain.elf
arm-none-eabi-objdump -D notmain.elf > notmain.list
arm-none-eabi-objcopy -O binary notmain.elf notmain.bin
*/

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

	

#define GPIOCBASE 0x40011000
//  PERIPH_BASE = 0x40000000 +
//  APB2_BASE   = 0x00010000 +
//  GPIOC_BASE  = 0x00001000

#define RCC_BASE 0x40021000
// PERIPH_BASE = 0x40000000 +
// AHB_BASE    = 0x00020000 +
// GPIOC_BASE  = 0x00001000
#define RCC_APB2ENR (RCC_BASE+0x18) //0x18 - RCC_APB2ENR
#define RCC_APB1ENR (RCC_BASE+0x1C) //0x1C - RCC_APB1ENR
#define RCC_CR      (RCC_BASE+0x00) //0x00 - CR(control register)
#define RCC_CFGR    (RCC_BASE+0x04) //0x04 - CFGR
#define FLASH_ACR   0x40022000



void blinker ( unsigned int n )
{
    unsigned int ra;
    unsigned int rx;

    for(rx=0;rx<n;rx++)
    {
    	// BSRR
	PUT32(GPIOCBASE+0x10,1<<(13+ 0));
	for(ra=0;ra<200000;ra++) dummy(ra);
	PUT32(GPIOCBASE+0x10,1<<(13+16));
	for(ra=0;ra<200000;ra++) dummy(ra);
    }
}

int notmain ( void )
{
    unsigned int ra;

    /* Configure Clock */ 
    // RCC_CR : 0x00, ResetClockControler Control Register
    ra = GET32(RCC_CR); 
    ra |= 1 << 16; // HSE ON
    PUT32(RCC_CR,ra);
    
    while(1) if(GET32(RCC_CR)&(1<<17)) break; // wait for hse ready bit
    
    
    // HSE selected as clk source 
    ra = GET32(RCC_CFGR); 
    ra &= ~(0x3<<0);  
    ra |= (0x1<<0);  
    PUT32(RCC_CFGR,ra);

    while(1) if((GET32(RCC_CFGR)&0xF)==0x5) break;

    //GPIOC clock enable 
    ra = GET32(RCC_APB2ENR); 
    ra |= 1<<4; // set clock for GPIOC
    PUT32(RCC_APB2ENR,ra);
        
    // PC13 configuration CRH:0x04
    ra = GET32(GPIOCBASE+0x04);  
    ra &= ~(3<<20); // clear MODE13 bits
    ra |=   1<<20;  // output mode 10mhz  
    ra &= ~(3<<22); // clear FUNC13 bits    
    ra |=   0<<22;  // push pull    
    PUT32(GPIOCBASE+0x04,ra);


    blinker(5);
    
    //[RCC_CFGR 0x04]
    ra = 0; // 
    ra |= 0x7<<24; // MCO:111 - pll selecte
    ra |= 0x0<<22; // USB prescaler 
    ra |= 0x7<<18; // PLLMUL je 0111(9) x 8Mhz = 72Mhz
    ra |= 0x0<<17; // PLL_XTPRE: 0-HSE, 1-HSE/2
    ra |= 0x1<<16; // PLL_SRC: 0-HSI/2, 1-HSE e
    ra |= 0x0<<14; // ADCPRE
    ra |= 0x0<<11; // PPRE2 // APB2 prescaler not divided 0xx
    ra |= 0x4<<8;  // PPRE1 // APB1 precaler not divided 0xx
    ra |= 0x0<<4;  // HPRE // AHB prescaer, Sys clock no divided
    PUT32(RCC_CFGR,ra);


    ra = GET32(RCC_CR); 
    ra |= 1<<24; // PLLON
    PUT32(RCC_CR,ra);
    while(1) if(GET32(RCC_CR)&(1<<25)) break;

    PUT32(FLASH_ACR,0x2);

    ra=GET32(RCC_CFGR); 
    ra&=~(0x3<<0);
    ra|= (0x2<<0);
    PUT32(RCC_CFGR,ra);
    while(1) if((GET32(RCC_CFGR)&0xF)==0xA) break;
    
    blinker(50000);
    
    return(0);
}
