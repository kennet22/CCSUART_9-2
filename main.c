
#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#define TXLED BIT0
#define Button BIT3

long i = 0;
float n;
char standard[] = "Standard Message\r\n";
void sendData(char* tx_data);

int main(void)
{
     WDTCTL = WDTPW + WDTHOLD; // Stop WDT

    /* Use Calibration values for 1MHz Clock DCO*/
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    /* Configure Pin Muxing P1.1 RXD and P1.2 TXD */
    P1SEL = BIT1 | BIT2 ;
    P1SEL2 = BIT1 | BIT2;

    P1DIR &= ~Button;
    P1OUT |= Button;
    P1REN |= Button;

    P1DIR  |=  BIT0 + BIT4 + BIT5 + BIT6;  // P1.0 and P1.6 output
    P1OUT  &= ~(BIT0 + BIT4 + BIT5 + BIT6);  // P1.0 and P1.6 = 0

    P2DIR |= BIT0 + BIT1 + BIT2  + BIT4 + BIT5;
    P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT4 + BIT5);

    /* Place UCA0 in Reset to be configured */
    UCA0CTL1 = UCSWRST;

    /* Configure */
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0 = 104; // 1MHz 9600
    UCA0BR1 = 0; // 1MHz 9600
    UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1

    /* Take UCA0 out of reset */
    UCA0CTL1 &= ~UCSWRST;

    /* Enable USCI_A0 RX interrupt */
    IE2 |= UCA0RXIE;

    __bis_SR_register(GIE); // Enter LPM0, interrupts enabled

    while(1){
        if (!((P1IN & Button) == Button)){
            float f = 12.123;
            char buffer[sizeof(f)];       // Create a char array the size of the float value 
            sprintf(buffer, "%.2f", f);   // Converts float to C-string ; requires Project>Properties>Compiler>full printf exception
            sendData(buffer);
            _delay_cycles(100000);      // Delay in place for debounce of button press
            P1OUT ^= BIT6;
        }
    }
}

void sendData(char* tx_data){
    unsigned int i = 0;
    P1OUT ^= BIT0;
    while(tx_data[i]){     // While there is a character in the string, send it
        while((UCA0STAT & UCBUSY));
        UCA0TXBUF = tx_data[i];         // Sets a char to be transmitted
        i++;
    }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

    P1OUT ^= BIT6;

    switch (UCA0RXBUF){

    case 0x31:
        for(i = 0; i < 1000; i++){
            P1OUT ^= BIT6;
        }
        while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
        sendData("1\r\n");
    break;

    case 0x32:
       i = 0;

        P1OUT ^= BIT6;

        while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
        char c[] = {0x31, 0x69};
        while(c[i]){
            while((UCA0STAT & UCBUSY));
            UCA0TXBUF = c[i];
            i++;
        }

    break;

    case '3':

        P1OUT ^= BIT6;
        float f = 128.21;
        char buffer[sizeof(f)];
        sprintf(buffer, "%.2f", f);
        sendData(buffer);
    break;

    case 'a':
        for(i = 0; i < 1000; i++){
            P1OUT ^= BIT6;
        }
        while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
        UCA0TXBUF = 'Y';
        UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
    break;

    default:
        for(i = 0; i < 1000; i++){
            P1OUT ^= BIT0;
        }
        while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
        UCA0TXBUF = '0';
        UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
    break;

    }
}
