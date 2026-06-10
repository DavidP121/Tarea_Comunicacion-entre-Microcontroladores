/*
 * Proyecto: Esclavo de Sonido Inteligente por un solo Pin (BEEP)
 * Microcontrolador: PIC16F887
 * Entorno: mikroC for PIC
 */

#define SIG_BEEP  PORTD.F0  // Pin RB0 (Línea limpia BEEP)

void main() {
    unsigned int duracion_pulso = 0;

    ANSEL  = 0x00;
    ANSELH = 0x00;
    TRISB.F0 = 1; // RB0 (BEEP) como entrada

    Sound_Init(&PORTC, 2); // Sonido en RC2

    while(1) {
        // Esperar a que la línea BEEP suba a 1 (Inicio del pulso)
        if (SIG_BEEP == 1) {
            duracion_pulso = 0;

            // Medir cuánto tiempo se queda en alto el pulso (en milisegundos)
            while(SIG_BEEP == 1) {
                Delay_ms(1);
                duracion_pulso++;
                if(duracion_pulso > 100) break; // Evitar bucle infinito
            }

            // Decidir qué sonido tocar según el ancho del pulso recibido
            if (duracion_pulso >= 5 && duracion_pulso <= 20) { // Pulso de ~10ms
                Sound_Play(261, 100); Sound_Play(329, 100);
                Sound_Play(392, 100); Sound_Play(523, 200);
            }
            else if (duracion_pulso >= 21 && duracion_pulso <= 40) { // Pulso de ~30ms
                Sound_Play(392, 40); Sound_Play(523, 40); Sound_Play(659, 40);
            }
            else if (duracion_pulso >= 41 && duracion_pulso <= 60) { // Pulso de ~50ms
                Sound_Play(784, 25); Sound_Play(1047, 35);
            }
            else if (duracion_pulso >= 61 && duracion_pulso <= 90) { // Pulso de ~70ms
                Sound_Play(329, 150); Sound_Play(294, 150);
                Sound_Play(261, 200); Sound_Play(120, 350);
            }
        }
    }
}