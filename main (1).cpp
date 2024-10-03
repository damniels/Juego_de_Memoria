#include "mbed.h"

// Alias y definiciones
#define escritura       0x40 // Comando para indicar que se va a escribir en el display
#define poner_brillo    0x88 // Comando para configurar el brillo del display
#define dir_display     0xC0 // Dirección base para escribir en los dígitos del display

// Funciones
void condicion_start(void);// Función para iniciar la comunicación con el display
void condicion_stop (void);// Función para finalizar la comunicación con el display
void send_byte (char data);// Función para enviar un byte de datos al display
void send_data(int miles, int centenas, int decenas, int unidades);// Enviar datos al display
void apagar_display(void);

//Secuencias
void sc1(void); 
void sc2(void); 
void sc3(void); 
void sc4(void); 

// Pines
DigitalOut sclk(D2); // Reloj
DigitalInOut dio(D3); //Datos
DigitalOut led(LED1);  // LED para indicar si es correcto
DigitalIn BU(D4);   // Pulsador para controlar las unidades
DigitalIn BD(D5);    // Pulsador para controlar las decenas
DigitalIn BC(D6);// Pulsador para controlar las centenas
DigitalIn BUM(D7); // Pulsador para controlar las unidades de mil
DigitalIn botonConfirmar(D8);  // Pulsador para confirmar el número ingresado

const char digitToSegment[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // Dígitos 0-9

int main()
{

// Ciclo infinito 
    while (true) {

        
        sc1();
        sc2();
        sc3();
        sc4();

    }
}

// Inicia la condición de "start" para la comunicación con el display
void condicion_start(void)
{
    sclk = 1;
    dio.output();
    dio = 1;
    wait_us(1);
    dio = 0;// Coloca el pin dio en bajo para indicar "start"
    sclk = 0;
}
// Finaliza la comunicación con el display
void condicion_stop (void)
{
    sclk = 0;
    dio.output();
    dio = 0;
    wait_us(1);
    sclk = 1;
    dio = 1; // Coloca el pin dio en alto para indicar "stop"
}

// Enviar un byte de datos al display
void send_byte(char data)
{
    dio.output();
    for (int i = 0; i < 8; i++) {
        sclk = 0;
        dio = (data & 0x01) ? 1 : 0; // Envía el bit menos significativo
        data >>= 1;
        sclk = 1;
    }
    // Esperar el ack
    dio.input();// Configura el pin dio como entrada para recibir el "ack"
    sclk = 0;
    wait_us(1);
    
    if (dio == 0) {// Si el dispositivo responde con un "ack"
        sclk = 1;
        sclk = 0;
    }
}

// Envía los dígitos de mil, centenas, decenas y unidades al display
void send_data(int miles, int centenas, int decenas, int unidades)
{
    condicion_start();
    send_byte(escritura);// Indica que se va a escribir en el display
    condicion_stop();
    condicion_start();
    send_byte(dir_display);// Indica la dirección de los datos en el display
    
    // Enviar miles, centenas, decenas y unidades al display
    send_byte(digitToSegment[miles]);    
    send_byte(digitToSegment[centenas]); 
    send_byte(digitToSegment[decenas]);  
    send_byte(digitToSegment[unidades]); 
    
    condicion_stop();
    condicion_start();
    send_byte(poner_brillo + 1);  // Configurar el brillo
    condicion_stop();
}

// Apaga el display, es decir, desactiva todos los segmentos
void apagar_display(void)
{
    condicion_start();
    send_byte(escritura);
    condicion_stop();
    condicion_start();
    send_byte(dir_display);
    
      // Envía datos para apagar todos los dígitos
    for (int i = 0; i < 4; i++) {
        send_byte(0x00); // Enviar código para apagar el segmento
    }
    
    condicion_stop();
}


//_________________________________________________________________________________

void sc1(void)
{
    int U = 0;    // Almacena las unidades ingresadas por el usuario
    int D = 0;    
    int C = 0; 
    int M = 0;  

        // Mostrar el número "9" en el display
        send_data(0, 0, 0, 9);
        ThisThread::sleep_for(250ms); // Mostrar el número "9" por 250m segundos
        
        // Poner el display en "0" para indicar que el jugador debe participar
        send_data(0,0,0,0); 
        led = 0; 

        // Esperar a que el usuario ingrese el número
        while (true) {
 {
// Si presiona el botón para incremeincrementarntar el número de unidades

    if (BU == 1) {
                U = (U + 1) % 10; // Incrementar el valor de 0 a 9
                send_data(M, C, D, U); // Mostrar eell número ingresado por el usuario
                ThisThread::sleep_for(300ms); // Evitar rebotes
            }

            // Si el usuario confirma el número
            if (botonConfirmar == 1) {
                ThisThread::sleep_for(300ms); // Evitar rebotes
                if (U == 9) {
                    led = 1; 
                } else {
                    apagar_display(); 
                    ThisThread::sleep_for(300ms); // Evitar rebotes
                    sc1(); // Se repite la secuencia
                }
                break; // Termina el ciclo una vez que se confirma el número
            }
        }

 // Espera antes de la siguiente ronda
        ThisThread::sleep_for(2000ms);  // Esperar 2 segundos antes de reiniciar el juego
       // Reiniciar las unidades ingresadas
        U = 0;  
        D = 0;   
        C = 0;
        M = 0;
        send_data(M, C, D, U);  // Mostrar 0000 en el display
        led = 0;  // Apagar el LED para la siguiente ronda
            
            }
}

void sc2(void)
{
    int U = 0;    
    int D = 0;    
    int C = 0; 
    int M = 0;   
    // Mostrar el número "34" en el display
        send_data(0, 0, 3, 4);
        ThisThread::sleep_for(250ms);  
        
        
        send_data(0, 0, 0, 0); 
        led = 0;  

        // Esperar a que el usuario ingrese el número
        while (true) {
            // Si presiona el botón para incrementar las unidades
            if (BU == 1) {
                U = (U + 1) % 10;  
                send_data(M, C, D, U);    
                ThisThread::sleep_for(300ms);      
            }

            // Si presiona el botón para incrementar las decenas
            if (BD == 1) {
                D = (D + 1) % 10;    
                send_data(M, C, D, U);    
                ThisThread::sleep_for(300ms);      
            }

            // Si el usuario confirma el número
            if (botonConfirmar == 1) {
                ThisThread::sleep_for(300ms);  
                if (U == 4 && D == 3) {
                    led = 1;  
                } else {
                    apagar_display();  
                     ThisThread::sleep_for(300ms); 
                    sc2();
                }
                break;  
            }
        }

        // Espera antes de la siguiente ronda
        ThisThread::sleep_for(2000ms);  
        U = 0;  
        D = 0;   
        C = 0;
        M = 0;
        send_data(M, C, D, U);  
        led = 0;  
    
}
void sc3(void)
{
    int U = 0;    
    int D = 0;     
    int C = 0; 
    int M = 0;   

    
        // Mostrar el número "175" en el display
        send_data(0, 1, 7, 5);
        ThisThread::sleep_for(250ms);  
        
        // Poner el display en "0000" para indicar que el jugador debe participar
        send_data(0, 0, 0, 0);  
        led = 0;  

        // Esperar a que el usuario ingrese el número
        while (true) {
            // Si presiona el botón para incrementar las unidades
            if (BU == 1) {
                U = (U + 1) % 10;  
                send_data(M, C, D, U); 
                ThisThread::sleep_for(300ms);  
            }

            // Si presiona el botón para incrementar las decenas
            if (BD == 1) {
                D = (D + 1) % 10;  
                send_data(M, C, D, U);  
                ThisThread::sleep_for(300ms);  
            }

            // Si presiona el botón para incrementar las centenas
            if (BC == 1) {
                C = (C + 1) % 10;  
                send_data(M, C, D, U);  
                ThisThread::sleep_for(300ms);  
            }

            // Si el usuario confirma el número
            if (botonConfirmar == 1) {
                ThisThread::sleep_for(300ms);  
                if (U == 5 && D == 7 && C == 1) {
                    led = 1;  
                } else {
                    apagar_display();  
                    ThisThread::sleep_for(300ms); 
                    sc3();
                }
                break;  
            }
        }

    
            // Espera antes de la siguiente ronda
        ThisThread::sleep_for(2000ms);  
        U = 0;  
        D = 0;   
        C = 0;
        M = 0;
        send_data(M, C, D, U);  
        led = 0; 
    
}

void sc4(void)
{
    int U = 0;    
    int D = 0;     
    int C = 0; 
    int M = 0;  

    
        // Mostrar el número "8162" en el display
        send_data(8, 1, 6, 2);
        ThisThread::sleep_for(250ms);  
        
        // Poner el display en "0000" para indicar que el jugador debe participar
        send_data(0, 0, 0, 0);  
        led = 0;  

        // Esperar a que el usuario ingrese el número
        while (true) {
            // Si presiona el botón para incrementar las unidades
            if (BU== 1) {
                U = (U + 1) % 10;  
                send_data(M, C, D, U);  
                ThisThread::sleep_for(300ms);  
            }

            // Si presiona el botón para incrementar las decenas
            if (BD == 1) {
                D = (D + 1) % 10;  
                send_data(M, C, D, U);  
                ThisThread::sleep_for(300ms);  
            }

            // Si presiona el botón para incrementar las centenas
            if (BC == 1) {
                C = (C + 1) % 10;  
                send_data(M, C, D, U);  
                ThisThread::sleep_for(300ms);  
            }

            // Si presiona el botón para incrementar las unidades de mil
            if (BUM == 1) {
                M = (M + 1) % 10;  
                send_data(M, C, D, U);  
                ThisThread::sleep_for(300ms);  
            }

            // Si el usuario confirma el número
            if (botonConfirmar == 1) {
                ThisThread::sleep_for(300ms);  
                if (U == 2 && D == 6 && C == 1 && M == 8) {
                    led = 1;  
                } else {
                    apagar_display();  
                    ThisThread::sleep_for(300ms); 
                    sc4();
                }
                break;  
            }
        }

        // Espera antes de la siguiente ronda
        ThisThread::sleep_for(2000ms);  
        U = 0; 
        D = 0;  
        C = 0;   
        M = 0;  
        send_data(M, C, D, U);  
        led = 0;  
    }
