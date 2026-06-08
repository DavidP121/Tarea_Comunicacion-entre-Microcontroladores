#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

// Definiciones de pines SPI para el MAX7219
#define CS_PIN   PB2
#define MOSI_PIN PB3
#define SCK_PIN  PB5

// Estados del juego
#define INICIO 0
#define NIVEL_1 1
#define NIVEL_2 2
#define NIVEL_3 3
#define VICTORIA 4
#define DERROTA 5

// Variables Globales del Juego
uint8_t estado_actual = INICIO;
uint8_t ball_x = 3, ball_y = 3;
int8_t dir_x = 1, dir_y = -1;
uint8_t paddle_x = 2; // Posición inicial izquierda de la barra
uint8_t paddle_width = 3; // Nivel 1: 3 puntos
uint8_t velocidad = 150; // Delay base para velocidad
uint8_t rebotes = 0; // Para contar cuando pasamos de nivel

// ---------------------------------------------------------
// FUNCIONES SPI Y MAX7219
// ---------------------------------------------------------
void SPI_init() {
    // Configurar MOSI, SCK y CS como salidas
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << CS_PIN);
    // Habilitar SPI, Modo Master, prescaler fosc/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void MAX7219_escribir(uint8_t direccion, uint8_t datos) {
    PORTB &= ~(1 << CS_PIN); // CS en BAJO
    SPDR = direccion;        // Enviar dirección
    while (!(SPSR & (1 << SPIF)));
    SPDR = datos;            // Enviar datos
    while (!(SPSR & (1 << SPIF)));
    PORTB |= (1 << CS_PIN);  // CS en ALTO
}

void MAX7219_init() {
    SPI_init();
    MAX7219_escribir(0x09, 0x00); // Modo decodificación: Ninguno
    MAX7219_escribir(0x0A, 0x08); // Brillo (0x00 a 0x0F)
    MAX7219_escribir(0x0B, 0x07); // Scan Limit: todas las filas (0-7)
    MAX7219_escribir(0x0C, 0x01); // Modo normal (salir de shutdown)
    MAX7219_escribir(0x0F, 0x00); // Test de display apagado
}

void limpiar_matriz() {
    for (uint8_t i = 1; i <= 8; i++) {
        MAX7219_escribir(i, 0x00);
    }
}

// ---------------------------------------------------------
// LÓGICA DE VISUALIZACIÓN Y TEXTOS
// ---------------------------------------------------------

// Muestra letras estáticas (Para L1, L2, L3 como permite la rúbrica)
void mostrar_simbolo(uint8_t nivel) {
    limpiar_matriz();
    // Ejemplo de 'L' simplificada en la izquierda y el número a la derecha
    if(nivel == 1){
        MAX7219_escribir(2, 0b01000010); // L y 1
        MAX7219_escribir(3, 0b01000110);
        MAX7219_escribir(4, 0b01000010);
        MAX7219_escribir(5, 0b01110111);
    } 
    // Aquí puedes expandir para L2 y L3, o crear la función de scroll de texto
    _delay_ms(1500); 
}

// Centralizamos el dibujado por si hay que rotar la matriz
void actualizar_matriz() {
    limpiar_matriz();
    
    // Dibujar Barra en la fila 8 (índice 7 para nosotros)
    uint8_t barra_bits = 0;
    for(uint8_t i = 0; i < paddle_width; i++) {
        barra_bits |= (1 << (7 - (paddle_x + i))); 
    }
    MAX7219_escribir(8, barra_bits); // Fila 8 es el piso
    
    // Dibujar Pelota
    MAX7219_escribir(ball_y + 1, (1 << (7 - ball_x)));
}

void delay_variable(uint8_t ms) {
    for(uint8_t i = 0; i < ms; i++) {
        _delay_ms(1);
    }
}

// ---------------------------------------------------------
// LÓGICA DEL JUEGO
// ---------------------------------------------------------

void configurar_nivel(uint8_t n) {
    ball_x = 3; ball_y = 3;
    dir_x = 1; dir_y = -1;
    rebotes = 0;
    estado_actual = n;
    
    if (n == NIVEL_1) {
        paddle_width = 3; velocidad = 120;
    } else if (n == NIVEL_2) {
        paddle_width = 2; velocidad = 80;
    } else if (n == NIVEL_3) {
        paddle_width = 1; velocidad = 50;
    }
    
    mostrar_simbolo(n); // Muestra L1, L2, L3 antes de iniciar
    // AQUÍ IRÁ EL CÓDIGO UART PARA ENVIAR SEÑAL DE SONIDO AL PIC
}

void leer_botones() {
    // Mover Izquierda (PD3)
    if (!(PIND & (1 << PD3))) {
        if (paddle_x > 0) paddle_x--;
    }
    // Mover Derecha (PD4)
    if (!(PIND & (1 << PD4))) {
        if (paddle_x < (8 - paddle_width)) paddle_x++;
    }
}

void actualizar_pelota() {
    // Mover pelota
    ball_x += dir_x;
    ball_y += dir_y;

    // Colisión con paredes laterales (X)
    if (ball_x == 0 || ball_x == 7) {
        dir_x = -dir_x; // Rebote
    }

    // Colisión con el techo (Y = 0)
    if (ball_y == 0) {
        dir_y = -dir_y; // Rebote hacia abajo
    }

    // Colisión con la barra o pérdida (Y = 6 y Y = 7)
    if (ball_y == 6 && dir_y > 0) {
        // Verificar si la pelota está en la misma columna que la barra
        if (ball_x >= paddle_x && ball_x < (paddle_x + paddle_width)) {
            dir_y = -dir_y; // Rebote exitoso
            rebotes++;
            
            // Lógica de progreso de nivel (ejemplo: 5 rebotes para pasar de nivel)
            if (rebotes >= 5) {
                if (estado_actual == NIVEL_1) configurar_nivel(NIVEL_2);
                else if (estado_actual == NIVEL_2) configurar_nivel(NIVEL_3);
                else if (estado_actual == NIVEL_3) estado_actual = VICTORIA;
            }
            // AQUÍ SE ENVIARÁ SEÑAL AL PIC DE "SONIDO REBOTE"
        }
    } else if (ball_y >= 7) {
        // La pelota tocó fondo y la barra no estaba ahí
        estado_actual = DERROTA;
        // AQUÍ SE ENVIARÁ SEÑAL AL PIC DE "SONIDO DERROTA"
    }
}

int main(void) {
    // Configurar Entradas para Botones
    DDRD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4)); 
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4); // Pull-ups activados

    MAX7219_init();

    while (1) {
        if (estado_actual == INICIO) {
            // Mostrar estático o scroll "INICIAR"
            // Por simplicidad para el loop, esperamos botón PD2 (START)
            if (!(PIND & (1 << PD2))) {
                configurar_nivel(NIVEL_1);
            }
        } 
        else if (estado_actual >= NIVEL_1 && estado_actual <= NIVEL_3) {
            leer_botones();
            actualizar_pelota();
            actualizar_matriz();
            delay_variable(velocidad); 
        } 
        else if (estado_actual == VICTORIA) {
            // Aquí iría el texto scroll "FELICIDADES"
            // Y esperar el botón de START para reiniciar
            if (!(PIND & (1 << PD2))) {
                estado_actual = INICIO;
            }
        } 
        else if (estado_actual == DERROTA) {
            // Aquí iría el texto scroll "PERDISTE"
            // Y esperar el botón de START para reiniciar
            if (!(PIND & (1 << PD2))) {
                estado_actual = INICIO;
            }
        }
    }
    return 0;
}
