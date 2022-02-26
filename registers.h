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
