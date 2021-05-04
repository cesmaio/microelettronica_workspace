#ifndef __BLINK845_H
#define __BLINK845_H

// clang-format off

#define R_ON {LPC_GPIO_PORT->CLR[1] = 1<<2;}//LED R on
#define G_ON {LPC_GPIO_PORT->CLR[1] = 1<<0;} //LED G on
#define B_ON {LPC_GPIO_PORT->CLR[1] = 1<<1;}//LED B on
#define LED_R {LPC_GPIO_PORT->CLR[1] = 1<<2;}//LED R on
#define LED_G {LPC_GPIO_PORT->CLR[1] = 1<<0;} //LED G on
#define LED_B {LPC_GPIO_PORT->CLR[1] = 1<<1;}//LED B on

#define R_OFF {LPC_GPIO_PORT->SET[1] = 1<<2;} //LED R off
#define G_OFF {LPC_GPIO_PORT->SET[1] = 1<<0;}//LED G off
#define B_OFF {LPC_GPIO_PORT->SET[1] = 1<<1;}//LED B off

#define ACT_R {LPC_GPIO_PORT->SET[1] = 1<<2;LPC_GPIO_PORT->DIRSET[1]= 1<<2;}
#define ACT_G {LPC_GPIO_PORT->SET[1] = 1<<0;LPC_GPIO_PORT->DIRSET[1]= 1<<0;}
#define ACT_B {LPC_GPIO_PORT->SET[1] = 1<<1;LPC_GPIO_PORT->DIRSET[1]= 1<<1;}

#define ACT_RGB {LPC_GPIO_PORT->SET[1] = 0x7;LPC_GPIO_PORT->DIRSET[1]= 0x7;}
#define RGB_OFF {LPC_GPIO_PORT->SET[1] = 0x7;}


#endif /* end __BLINK845_H */
