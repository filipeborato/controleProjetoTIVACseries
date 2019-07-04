/**
 * Projeto Criado por FILIPE BORATO
 * Pinos definidos como entrandas e suas funções no controle
 * start  - A2
 * ação A - A3
 * ação B - A4
 * seta ↑ - C4
 * seta → - C5
 * seta ↓ - C6
 * seta ← - C7
 * ANG1 X - D0
 * ANG1 Y - D1
 * ANG2 X - D2
 * ANG2 y - D3
 * ação C - E1
 * ação D - E2
 * ação E - E3
 * ação F - E5
 */
#include<tm4c123gh6pm.h>
#include<inttypes.h>
#include <stdlib.h>
#define A0 0x01
#define A1 0x02
#define A2 0x04
#define A3 0x08
#define A4 0x10
#define C4 0X10
#define C5 0x20
#define C6 0x40
#define C7 0x80
#define D0 0x01
#define D1 0x02
#define D2 0x04
#define D3 0x08
#define E1 0x02
#define E2 0x04
#define E3 0x08
#define E4 0x10
//Definições dos ports para o display
//04 RS   - PA5
//05 R/W  - PA7
//06 E    - PA6
//07 DB0  - PB0
//08 DB1  - PB1
//09 DB2  - PB2
//10 DB3  - PB3
//11 DB4  - PB4
//12 DB5  - PB5
//13 DB6  - PB6
//14 DB7  - PB7
#define ENABLE GPIO_PORTA_DATA_R^=0x40
#define EEPROM_INIT_OK 0
#define EEPROM_INIT_ERROR 2

/*
 * VARIÁVEIS
 * TIMER = TEMPO DE CONTAGEM DO SYSTICK
 * COUNT = VARIÁVEL USADA COMO CONTADOR JUNTO AO SYSTICK (1 ms)
 * LETTER = VARIÁVEL QUE ARMAZENA A LETRA
 */

uint32_t TIMER = 4000; //
uint8_t bloco=1, registro=0;
unsigned char LETTER = ' '; // VARIÁVEL QUE ARMAZENA A LETRA A SER ENVIADA
int COUNT = 0;
/*
 * DECLARAÇÃO DE FUNÇÕES
 */

void delay_us(uint32_t delay);
void config(void);
void ADC_Read(void);
void UART_Send(unsigned char c, int aux);
void pulso_enable();
void escreve_LCD(char *c);
void cmd_LCD(unsigned char c, int count);
void cmd_LCD(unsigned char c, int count);
void inicializa_LCD();
void Set_memo(uint32_t word);
uint32_t EInit(void);
uint32_t EEPROM_read(uint8_t block, uint8_t offset);
void EEPROM_write(uint8_t block, uint8_t offset, uint32_t word);
void Set_memo(uint32_t word);
//void Set_blocoReg();

void main(void)
{

    config();


    while(1)
    {
        //delay_us(40);
        ADC_Read();
    }
}

void config(void)
{
    //Habilitando A,B,C,D,E

    //delay_us(40000);
    SYSCTL_RCGCGPIO_R |= 0x1F;
    //DISPLAY CONFIG
    inicializa_LCD();
    //EInit();
    /*
     * CONFIGURAMOS E HABILITAMOS A NVIC PARA UTILIZARMOS O SYSTICK COMO TIMER e INTERRUPÇÂO
     */

    //GPIO_PORTA_LOCK_R = 0x4C4F434B;
   // GPIO_PORTA_CR_R = 0xFF;

    NVIC_ST_RELOAD_R = TIMER;
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE;


    /*
        * CONFIGURAMOS UART (A0 E A1)
        * 1 - ATIVAMOS O CLOCK NA UART0
        * 2 - HABILITAMOS FUNCIONALIDADE ALTERNATIVA NAS PORTAS
        * 3 - HABILITAMOS A FUNCIONALIDADE UART NAS PORTAS
        * 3 - CONFIGURAMOS CONECTIVIDADE
        * 4 - HABILITAMOS AS PORTAS USADAS NA UART
        */

       SYSCTL_RCGCUART_R = 0x01;
       GPIO_PORTA_DEN_R = 0;

       GPIO_PORTA_AFSEL_R |= (A0 | A1);

       GPIO_PORTA_PCTL_R = 0x11;

       UART0_CTL_R &= 0x00;                                // DESABILITAMOS UART0
       UART0_IBRD_R = 8;                                   // BAUDRATE - 115200
       UART0_FBRD_R = 44;                                  // BAUDRATE
       UART0_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);  // 8 BITS SENDO ENVIADOS / SEM PARIDADE / 1 BIT DE STOP / FIFOs
       UART0_CC_R = 0x00;                                  // USAMOS SYSTEM CLOCK COMO FONTE DE CLOCK
       UART0_CTL_R |= 0x101;                               // HABILITAMOS UART0 APENAS TX LIGADO

       //

    /*
     * CONFIGURAMOS GPIOS:
     * 1 - COMO ENTRADA
     * 2 - SETAMOS 0 NO DATA
     * 3 - COLOCAMOS AS PORTAS COMO PULL-UP RESISTOR
     * 4 - HABILITAMOS AS PORTAS
     */
    GPIO_PORTA_DIR_R |= 0X00 ;

    GPIO_PORTC_DIR_R = 0X00 ;
    GPIO_PORTD_DIR_R = 0X00 ;
    GPIO_PORTE_DIR_R = 0X00 ;


    GPIO_PORTA_PUR_R = (A2 | A3 | A4 );
    GPIO_PORTC_PUR_R = (C4 | C5 | C6 | C7);
    GPIO_PORTE_PUR_R = (E1 | E2 | E3 | E4);


    /*
     * CONFIGURAMOS INTERRUPÇÕES
     * 1 - LIGAMOS AS INTERRUPÇÕES NA NVIC PARA AS PORTAS UTILIZADAS (A, C, E)
     * 2 - CONFIGURAMOS EVENTOS
     */


    GPIO_PORTA_IS_R = 0x00; // DESABILITA SENSIBILIDADE POR LEVEL E "ATIVAMOS" POR BORDA
    GPIO_PORTC_IS_R = 0x00;
    GPIO_PORTE_IS_R = 0x00;

    GPIO_PORTA_IEV_R = 0x00; // VAI ATIVAR EM FALLING EDGE (LOW - PULL-UP RESISTOR)
    GPIO_PORTC_IEV_R = 0x00;
    GPIO_PORTE_IEV_R = 0x00;

    GPIO_PORTA_IBE_R = 0x00; // DESLIGA BOTH EDGES DESSA FORMA O IEV CONTROLA
    GPIO_PORTC_IBE_R = 0x00;
    GPIO_PORTE_IBE_R = 0x00;


    GPIO_PORTA_IM_R = (A2 | A3 | A4 ); // HABILITA INTERRUPÇÃO / INTERRUPTION MASK
    GPIO_PORTC_IM_R = (C4 | C5 | C6 | C7);
    GPIO_PORTE_IM_R = (E1 | E2 | E3 | E4);

    NVIC_EN0_R = 0x15; // A, C, E

    GPIO_PORTA_DATA_R |= 0x00;
    GPIO_PORTC_DATA_R = 0x00;
    GPIO_PORTD_DATA_R = 0x00;
    GPIO_PORTE_DATA_R = 0x00;

    GPIO_PORTA_DEN_R |= (A2 | A3 | A4 );
    GPIO_PORTB_DEN_R = 0xFF;
    GPIO_PORTC_DEN_R = (C4 | C5 | C6 | C7);
    GPIO_PORTE_DEN_R = (E1 | E2 | E3 | E4);
    GPIO_PORTA_DEN_R |= (A0 | A1); //enable do uart
    GPIO_PORTA_DEN_R |= 0xE0; //enable do uart


    /*
     * CONFIGURAMOS ANALOG-DIGITAL CONVERSOR (D0 E D1)
     * 1 - HABILITAMOS OS 2 MÓDULOS (ADC0 E ADC1)
     * 2 - HABILITAMOS AS FUNCIONALIDADES ALTERNATIVAS NAS PORTAS
     * 3 - HABILITAMOS A FUNCIONALIDADE ANALÓGICA NAS PORTAS
     * 4 - CONFIGURAMOS OS MÓDULOS ADC0 E ADC1
     */

    SYSCTL_RCGCADC_R = 0x03;

    GPIO_PORTD_AFSEL_R = (D2 | D3 );//| D2 | D3);

    GPIO_PORTD_AMSEL_R = (D2 | D3 );//| D2 | D3);

    ADC0_ACTSS_R = 0x00;
    ADC0_EMUX_R = 0x0F;
    ADC0_SSMUX0_R = 0x05;
    ADC0_SSCTL0_R = 0x02;
    ADC0_ACTSS_R = 0x01;
    ADC0_PSSI_R = 0x01;

    ADC1_ACTSS_R = 0x00;
    ADC1_EMUX_R = 0x0F;
    ADC1_SSMUX0_R = 0x04;
    ADC1_SSCTL0_R = 0x02;
    ADC1_ACTSS_R = 0x01;
    ADC1_PSSI_R = 0x01;
    //GPIO_PORTD_DEN_R |= D0|D1;

    //setando ris em 0 por algum motivo desconhecido
    //GPIO_PORTC_ICR_R = C4;
    //GPIO_PORTA_ICR_R = A2;
    //GPIO_PORTA_ICR_R = A3;
    //inicializa a memoria

    EInit();



}
void delay_us(uint32_t delay){
    volatile uint32_t elapsedTime;
    uint32_t startTime = NVIC_ST_CURRENT_R;
    do{
        elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
    }
    while(elapsedTime <= delay);
}

void SysTick_INTERRUPT(void)
{
    if (COUNT > 0)
        COUNT -= 1;

    if (LETTER != ' ' && COUNT == 0)
    {
        UART0_DR_R = LETTER;
        LETTER = ' ';
        COUNT = 50; //TEMPO ENTRE ENVIOS
    }
}
/*
 * ADC_Read
 * 1 - FAZ A LEITURA DO ADC MODULOS 0 E 1
 */

void ADC_Read(void)
{
    GPIO_PORTA_DEN_R |= 0xE0; //enable display
    uint32_t x = 0, y = 0;
    uint8_t x2 = 0, y2 = 0;
    delay_us(40);
    x = ADC0_SSFIFO0_R; //LEITURA ADC0
    y = ADC1_SSFIFO0_R; //LEITURA ADC1
    delay_us(40);
    // IMPLEMENTAR CONDIÇÃO PARA NÃO MANDAR NADA
    if ((x < 1900|| x > 2800)||(y < 1900 || y > 2900))
    {
            delay_us(40);

            x2 = x >> 4; // transfere para 8 bits (LIMITAÇÃO UART)
            y2 = y >> 4; // transfere para 8 bits (LIMITAÇÃO UART)
            delay_us(40);
            UART_Send(x2, 1);
            UART_Send(y2, 1);

    }
}


/*
 *  UART_Send
 *  1 - MANDA INFORMAÇÃO
 */

void UART_Send(unsigned char c, int aux)
{

    /*
     * 1 - BOTÕES COM CHAR
     * 2 - BOTÃO START
     */
    switch(aux)
    {
    case 1:
        if (LETTER == ' ')
        LETTER = c; //


        break;
    case 2:
        UART0_DR_R = c;


        break;
    default:
        break;
    }
}

void IntPortA(void)
{
    uint32_t a;
    char c;
    if(GPIO_PORTA_RIS_R&A2)
    {
        GPIO_PORTA_ICR_R = A2;
        UART_Send('P', 2);
        escreve_LCD("P");
        c = 'P';
        a = c - '0';
        Set_memo(a);

    }
    else if(GPIO_PORTA_RIS_R&A3)
    {
        GPIO_PORTA_ICR_R = A3;
        UART_Send('A', 1);
        escreve_LCD("A");
        c = 'A';
        a = c - '0';
        Set_memo(a);
    }
    else if(GPIO_PORTA_RIS_R&A4)
    {
        GPIO_PORTA_ICR_R = A4;
        UART_Send('B', 1);
        escreve_LCD("B");
        c = 'B';
        a = c - '0';
        Set_memo(a);
    }
}
void IntPortC(void)
{
    uint32_t a;
    char c;
    if(GPIO_PORTC_RIS_R&C4)
    {
        GPIO_PORTC_ICR_R = C4;
        UART_Send('U', 1);
        escreve_LCD("U");
        c = 'U';
        a = c - '0';
        Set_memo(a);
    }
    else if (GPIO_PORTC_RIS_R&C5)
    {
        GPIO_PORTC_ICR_R = C5;
        UART_Send('R', 1);
        escreve_LCD("R");
        c = 'R';
        a = c - '0';
        Set_memo(a);
    }
    else if (GPIO_PORTC_RIS_R&C6)
    {
        GPIO_PORTC_ICR_R = C6;
        UART_Send('Q', 1);
        escreve_LCD("Q");
        c = 'Q';
        a = c - '0';
        Set_memo(a);
    }
    else if (GPIO_PORTC_RIS_R&C7)
    {
        GPIO_PORTC_ICR_R = C7;
        UART_Send('L', 1);
        escreve_LCD("L");
        c = 'L';
        a = c - '0';
        Set_memo(a);
    }
}
void IntPortE(void)
{
    uint32_t a;
    char c;
    if(GPIO_PORTE_RIS_R&E1)
    {
        GPIO_PORTE_ICR_R = E1;
        UART_Send('C', 1);
        escreve_LCD("C");
        c = 'C';
        a = c - '0';
        Set_memo(a);
    }
    else if(GPIO_PORTE_RIS_R&E2)
    {
        GPIO_PORTE_ICR_R = E2;
        UART_Send('D', 1);
        escreve_LCD("D");
        c = 'D';
        a = c - '0';
        Set_memo(a);
    }
    else if(GPIO_PORTE_RIS_R&E3)
    {
        GPIO_PORTE_ICR_R = E3;
        UART_Send('E', 1);
        escreve_LCD("E");
        c = 'E';
        a = c - '0';
        Set_memo(a);
    }
    else if(GPIO_PORTE_RIS_R&E4)
    {
        GPIO_PORTE_ICR_R = E4;
        UART_Send('F', 1);
        escreve_LCD("F");
        c = 'F';
        a = c - '0';
        Set_memo(a);
    }
}

void inicializa_LCD()
{
    int i,j;
    TIMER = 4000000;
    SYSCTL_RCGCGPIO_R |= 0x03;
    GPIO_PORTB_DATA_R = 0x00;
    GPIO_PORTA_DATA_R = 0x00;
    GPIO_PORTA_DIR_R |= 0xE0;
    GPIO_PORTB_DIR_R |= 0xFF;
    GPIO_PORTB_DEN_R |= 0xFF;
    GPIO_PORTA_DEN_R |= 0xE0;

    NVIC_ST_RELOAD_R = TIMER;           //Configura sistick pag 140, limitado 24 bits, quantidade de ciclos de clock
    NVIC_ST_CTRL_R = 0x01;

    delay_us(60500);
    GPIO_PORTB_DATA_R = 0x30;

    delay_us(100000);
    pulso_enable();

    delay_us(20000);
    pulso_enable();

    delay_us(20000);
    pulso_enable();

    GPIO_PORTB_DATA_R = 0x3C;
    pulso_enable();

    GPIO_PORTB_DATA_R = 0x08;
    pulso_enable();

    GPIO_PORTB_DATA_R = 0x01;
    pulso_enable();

    GPIO_PORTB_DATA_R = 0x07;
    pulso_enable();

    GPIO_PORTB_DATA_R = 0x0C;
    pulso_enable();

    GPIO_PORTB_DATA_R = 0x06;
    pulso_enable();

    GPIO_PORTB_DATA_R = 0x01;
    pulso_enable();

    escreve_LCD("ESPERE O BOOT  DO SISTEMA!!");
    delay_us(8000000);
    delay_us(8000000);
    escreve_LCD(".");
    delay_us(4000000);
    delay_us(8000000);
    escreve_LCD("..");
    delay_us(8000000);
    delay_us(2000000);
    escreve_LCD("...");
    delay_us(5000000);
    delay_us(8000000);
    escreve_LCD("Pronto, Sistema butado!!");
    delay_us(8000000);
    delay_us(2000000);
/*
 if(EInit()==0){
        unsigned char leta;
        escreve_LCD("Registro dos Botoes anteriores:");

        delay_us(8000000);
        delay_us(5000000);

       for(i=0;i<=EEPROM_EEBLOCK_R;i++){
           if(i > 0){
               escreve_LCD("Mais registros: ");
               delay_us(8000000);
               delay_us(5000000);
           }
            for(j=0;i<=EEPROM_EEOFFSET_R ;j++){
                leta = EEPROM_read(i, j);
                cmd_LCD(leta, j);
               delay_us(8000000);
            }
        }
     }//*/

    TIMER = 4000;

}

void cmd_LCD(unsigned char c, int count)
{
    TIMER = 4000000;
    if (count == 1)
    {
        GPIO_PORTA_DATA_R = 0x00;
        delay_us(4000);
        GPIO_PORTB_DATA_R = 0x01;
        pulso_enable();
        GPIO_PORTB_DATA_R = 0x02;
        pulso_enable();
    }
    if (count == 16)
    {
        GPIO_PORTA_DATA_R = 0x00;
        delay_us(4000);
        GPIO_PORTB_DATA_R = 0xC0;
        pulso_enable();
    }
    GPIO_PORTA_DATA_R = 0x20;
    delay_us(4000);
    GPIO_PORTB_DATA_R = c;
    pulso_enable();
    TIMER=4000;

}

void escreve_LCD(char *c)
{
    int count = 0;
    for (; *c != 0; c++)
    {
        count++;
        cmd_LCD(*c, count);

    }
}

void pulso_enable()
{
    TIMER = 4000000;
    ENABLE;
    delay_us(10000);
    ENABLE;
    TIMER=4000;
}

uint32_t EInit(void)
{
    uint8_t aux;
    SYSCTL_RCGCEEPROM_R = 0x01;
    for(aux=0;aux<2;aux++){}
    while((EEPROM_EEDONE_R & 0x01) == 0x01){}
    if(((EEPROM_EESUPP_R & 0x08) == 0x08) || ((EEPROM_EESUPP_R & 0x04) == 0x04))
    {
        return(EEPROM_INIT_ERROR);
    }

    SYSCTL_SREEPROM_R = 0x01;
    SYSCTL_SREEPROM_R = 0x00;
    for(aux=0;aux<2;aux++){}
    while((EEPROM_EEDONE_R & 0x01) == 0x01){}
    if(((EEPROM_EESUPP_R & 0x08) == 0x08) || ((EEPROM_EESUPP_R & 0x04) == 0x04))
    {
        return(EEPROM_INIT_ERROR);
    }

    return(EEPROM_INIT_OK);

}

uint32_t EEPROM_read(uint8_t block, uint8_t offset){
    while(((EEPROM_EEDONE_R&0x20)==0x20) || ((EEPROM_EEDONE_R&0x08)==0x08) || ((EEPROM_EEDONE_R&0x04)==0x04)){}
    EEPROM_EEBLOCK_R = (uint8_t)(1<<block);
    EEPROM_EEOFFSET_R = (uint8_t)offset;
    return (uint32_t)EEPROM_EERDWR_R;
}

void EEPROM_write(uint8_t block, uint8_t offset, uint32_t word) {
    while(((EEPROM_EEDONE_R&0x20)==0x20) || ((EEPROM_EEDONE_R&0x08)==0x08) || ((EEPROM_EEDONE_R&0x04)==0x04)){}
    EEPROM_EEBLOCK_R =  (uint8_t)(1<<block);
    EEPROM_EEOFFSET_R = (uint8_t)offset;
    EEPROM_EERDWR_R = (uint32_t)word;
}
void Set_memo(uint32_t word){
        if(registro > 15){
            registro=0;
            bloco++;
        }
        if(bloco > 32){
            bloco=1;
        }
        EEPROM_write(bloco, registro, word);
        registro++;
}


