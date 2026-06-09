#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

// Estados del juego
#define INICIO 0
#define NIVEL_1 1
#define NIVEL_2 2
#define NIVEL_3 3
#define VICTORIA 4
#define DERROTA 5

volatile uint8_t buffer_pantalla[8] = {0};
bool juego_en_pausa = true; // La pelota se queda quita para inicar el juego
// Variables Globales del Juego
uint8_t estado_actual = INICIO;
uint8_t ball_x = 3, ball_y = 4; // La pelota ahora empieza más abajo
int8_t dir_x = 1, dir_y = -1;
uint8_t paddle_x = 2; 
uint8_t paddle_width = 3; 

//VARIABLES PARA LOS BLOQUES 
uint8_t bloques[3] = {0, 0, 0}; // Representan las filas 0, 1 y 2 de la matriz
uint8_t bloques_restantes = 0;  // Contador para saber cuándo pasar de nivel

// Variables para controlar la velocidad de la pelota
uint16_t limite_pelota = 50; 

// Variables para la "Memoria" de los botones (Evita el efecto metralleta)
uint8_t estado_anterior_izq = 0;
uint8_t estado_anterior_der = 0;

// Símbolos para las pantallas de estado
const uint8_t SIMBOLO_PLAY[8]  = {0x00, 0x00, 0x7E, 0x3C, 0x18, 0x00, 0x00, 0x00};
const uint8_t SIMBOLO_1[8]  = {0x00, 0x44, 0x46, 0x7E, 0x7E, 0x40, 0x40, 0x00};
const uint8_t SIMBOLO_2[8]     = {0x00, 0xC4, 0xE6, 0xF6, 0xDE, 0xCC, 0x00, 0x00};
const uint8_t SIMBOLO_3[8]     = {0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x7E, 0x7E, 0x00};
const uint8_t SIMBOLO_X[8]     = {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81};
const uint8_t SIMBOLO_COPA[8]     = {0x06, 0x89, 0xCE, 0xFE, 0xFE, 0xCE, 0x89, 0x06};

// Mensaje deslizante: "INTENTALO DE NUEVO" a pantalla completa
const uint8_t MSG_DERROTA[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x82, 0xFE, 0x82, 0x00,                         // I
    0xFE, 0x08, 0x10, 0x20, 0xFE, 0x00,             // N
    0x02, 0xFE, 0x02, 0x00,                         // T
    0xFE, 0x92, 0x92, 0x82, 0x00,                   // E
    0xFE, 0x08, 0x10, 0x20, 0xFE, 0x00,             // N
    0x02, 0xFE, 0x02, 0x00,                         // T
    0xFC, 0x12, 0x12, 0xFC, 0x00,                   // A
    0xFE, 0x80, 0x80, 0x80, 0x00,                   // L
    0x7C, 0x82, 0x82, 0x7C, 0x00,                   // O
    0x00, 0x00,                                     // Espacio
    0xFE, 0x82, 0x82, 0x7C, 0x00,                   // D
    0xFE, 0x92, 0x92, 0x82, 0x00,                   // E
    0x00, 0x00,                                     // Espacio
    0xFE, 0x08, 0x10, 0x20, 0xFE, 0x00,             // N
    0x7E, 0x80, 0x80, 0x7E, 0x00,                   // U
    0xFE, 0x92, 0x92, 0x82, 0x00,                   // E
    0x3E, 0x40, 0x80, 0x40, 0x3E, 0x00,             // V
    0x7C, 0x82, 0x82, 0x7C, 0x00,                   // O
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  
};
#define LONGITUD_MSG (sizeof(MSG_DERROTA))

void inicializar_hardware() {
    DDRD = 0xFF; 
    DDRB = 0xFF; 
    DDRC &= ~((1 << PC0) | (1 << PC1) | (1 << PC5)); 
    PORTC &= ~((1 << PC0) | (1 << PC1) | (1 << PC5)); 
}

void limpiar_matriz() {
    for (uint8_t i = 0; i < 8; i++) buffer_pantalla[i] = 0;
}

void mostrar_simbolo(const uint8_t* simbolo) {
    for(uint8_t i = 0; i < 8; i++) buffer_pantalla[i] = simbolo[i];
}

void renderizar_un_frame() {
    for (uint8_t columna = 0; columna < 8; columna++) {
        PORTB = 0xFF; 
        PORTD = 0x00; 
        PORTD = buffer_pantalla[columna]; 
        PORTB = ~(1 << columna);          
        _delay_ms(0.1); 
    }
}

void actualizar_pantalla_juego() {
    limpiar_matriz();
    
    // A continuación dibujamos los bloques de las 3 primeras filas
    for (uint8_t y = 0; y < 3; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            if (bloques[y] & (1 << x)) { // Si el bit está en 1, el bloque existe
                buffer_pantalla[x] |= (1 << y);
            }
        }
    }
    
    // 2. Dibujar la barra
    for(uint8_t i = 0; i < paddle_width; i++) {
        buffer_pantalla[paddle_x + i] |= (1 << 7);
    }
    
    // 3. Dibujar la pelota
    buffer_pantalla[ball_x] |= (1 << ball_y);
}

void configurar_nivel(uint8_t n) {
    estado_actual = n;
    paddle_x = 2; // Centrada: (8 ancho matriz - 3 ancho barra) / 2 = 2.5, usamos 2.
    ball_x = 3; 
    ball_y = 6; // Pelota sobre la barra
    juego_en_pausa = true; // El juego empieza en espera
    dir_x = 0; dir_y = -1; // Dirección inicial neutra

    ball_x = 3; ball_y = 5; // Inicia cerca de la barra
    dir_x = 1; dir_y = -1;
    estado_actual = n;
    
    estado_anterior_izq = PINC & (1 << PC0);
    estado_anterior_der = PINC & (1 << PC5);
    
    // La barra ahora siempre mide 3 para que el tiro recto funcione.
    // La dificultad ahora se basa puramente en la velocidad extrema.
    paddle_width = 3; 
    
    if (n == NIVEL_1) {
        limite_pelota = 40; // Lento
        bloques[0] = 0b00000000;
        bloques[1] = 0b01111110; // Fila 1: 6 bloques
        bloques[2] = 0b00000000;
        bloques_restantes = 6;
        mostrar_simbolo(SIMBOLO_1);
    } else if (n == NIVEL_2) {
        limite_pelota = 40; // Rápido
        bloques[0] = 0b11111111; // Fila 0: 8 bloques
        bloques[1] = 0b01111110; // Fila 1: 6 bloques
        bloques[2] = 0b00000000;
        bloques_restantes = 14;
        mostrar_simbolo(SIMBOLO_2);
    } else if (n == NIVEL_3) {
        limite_pelota = 40; // Velocidad extrema
        bloques[0] = 0b11111111; // Fila 0: 8 bloques
        bloques[1] = 0b10000001; // Fila 1: 2 bloques esquineros
        bloques[2] = 0b11111111; // Fila 2: 8 bloques
        bloques_restantes = 18;
        mostrar_simbolo(SIMBOLO_3);
    }
    
    for(uint16_t i = 0; i < 200; i++) renderizar_un_frame();
    actualizar_pantalla_juego();
}

void leer_botones() {
    uint8_t estado_actual_izq = PINC & (1 << PC0);
    uint8_t estado_actual_der = PINC & (1 << PC5);
    uint8_t boton_lanzar = PINC & (1 << PC1);

    if (estado_actual_izq && !estado_anterior_izq) {
        if (paddle_x > 0) {
            paddle_x--;
            if (juego_en_pausa) ball_x = paddle_x + 1; // La pelota sigue a la barra
        }
    }
    
    if (estado_actual_der && !estado_anterior_der) {
        if (paddle_x < (8 - paddle_width)) {
            paddle_x++;
            if (juego_en_pausa) ball_x = paddle_x + 1; // La pelota sigue a la barra
        }
    }

    // Lanzar la pelota
    if (boton_lanzar && juego_en_pausa) {
        juego_en_pausa = false;
        dir_x = 0; // Sale recta
        dir_y = -1;
    }

    estado_anterior_izq = estado_actual_izq;
    estado_anterior_der = estado_actual_der;
}
void actualizar_pelota() {
    ball_x += dir_x;
    ball_y += dir_y;

    // 1. Rebote en paredes laterales (Protección de bordes)
    if (ball_x <= 0) { ball_x = 0; dir_x = 1; }
    if (ball_x >= 7) { ball_x = 7; dir_x = -1; }

    // 2. Colisión con los bloques
    bool impacto = false;
    if (ball_y <= 2) {
        if (bloques[ball_y] & (1 << ball_x)) { 
            bloques[ball_y] &= ~(1 << ball_x); // Destruye el bloque
            bloques_restantes--;
            dir_y = 1; // Rebota hacia abajo
            impacto = true;
            
            // Revisa si ya ganó el nivel
            if (bloques_restantes == 0) {
                if (estado_actual == NIVEL_1) configurar_nivel(NIVEL_2);
                else if (estado_actual == NIVEL_2) configurar_nivel(NIVEL_3);
                else if (estado_actual == NIVEL_3) estado_actual = VICTORIA;
                return; 
            }
        }
    }

    // 3. Rebote en el techo (Si no golpeó un bloque)
    if (ball_y == 0 && !impacto) {
        dir_y = 1;
    }

    // 4. Rebote con la barra (AQUÍ ESTÁ LA MAGIA)
    if (ball_y == 6 && dir_y > 0) {
        if (ball_x >= paddle_x && ball_x < (paddle_x + paddle_width)) {
            dir_y = -1; // Siempre rebota hacia arriba
            
            // Calcula dónde pegó la pelota para romper el ciclo diagonal
            if (ball_x == paddle_x) {
                dir_x = -1; // Pega a la izquierda -> Sale a la izquierda
            } else if (ball_x == paddle_x + 2) {
                dir_x = 1;  // Pega a la derecha -> Sale a la derecha
            } else {
                dir_x = 0;  // Pega en el centro -> ¡SALE RECTA HACIA ARRIBA!
            }
        }
    } 
    // 5. La pelota cayó al vacío
    else if (ball_y >= 7) {
        estado_actual = DERROTA;
    }
}
void mostrar_mensaje_deslizante() {
    for (uint16_t desplazamiento = 0; desplazamiento <= LONGITUD_MSG - 8; desplazamiento++) {
        for (uint8_t i = 0; i < 8; i++) {
            buffer_pantalla[i] = MSG_DERROTA[desplazamiento + i];
        }
        
        for (uint16_t tiempo = 0; tiempo < 20; tiempo++) { 
            renderizar_un_frame(); 
            if (PINC & (1 << PC1)) return; 
        }
    }
}

int main(void) {
    inicializar_hardware();
    mostrar_simbolo(SIMBOLO_PLAY);

    uint16_t tick_pelota = 0;

    while (1) {
        renderizar_un_frame();

        if (estado_actual == INICIO) {
            if (PINC & (1 << PC1)) {
                configurar_nivel(NIVEL_1);
            }
        } 
        else if (estado_actual >= NIVEL_1 && estado_actual <= NIVEL_3) {
           leer_botones();
           if (!juego_en_pausa) { // Solo mover si no está en pausa
              tick_pelota++;
              if (tick_pelota >= limite_pelota) {
                  actualizar_pelota();
                  tick_pelota = 0;
        }
    }
    actualizar_pantalla_juego();
}
        else if (estado_actual == VICTORIA) {
            mostrar_simbolo(SIMBOLO_COPA);
            if (PINC & (1 << PC1)) { 
                estado_actual = INICIO;
                mostrar_simbolo(SIMBOLO_PLAY);
                for(uint16_t i = 0; i < 150; i++) renderizar_un_frame(); 
            }
        }
        else if (estado_actual == DERROTA) {
            mostrar_simbolo(SIMBOLO_X);
            for(uint16_t i = 0; i < 200; i++) renderizar_un_frame(); 
            
            while (estado_actual == DERROTA) {
                mostrar_mensaje_deslizante();
                
                if (PINC & (1 << PC1)) {
                    estado_actual = INICIO;
                    mostrar_simbolo(SIMBOLO_PLAY);
                    for(uint16_t i = 0; i < 150; i++) renderizar_un_frame(); 
                }
            }
        }
    }
    return 0;
}
