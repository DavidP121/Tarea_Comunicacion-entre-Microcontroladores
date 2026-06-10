#line 1 "C:/Users/Gabriel Salas/Documents/Proyectos Code/sistemb/Tarea_Comunicacion-entre-Microcontroladores/PIC16887/sonidoscode.c"
#line 9 "C:/Users/Gabriel Salas/Documents/Proyectos Code/sistemb/Tarea_Comunicacion-entre-Microcontroladores/PIC16887/sonidoscode.c"
void main() {
 unsigned int duracion_pulso = 0;

 ANSEL = 0x00;
 ANSELH = 0x00;
 TRISB.F0 = 1;

 Sound_Init(&PORTC, 2);

 while(1) {

 if ( PORTD.F0  == 1) {
 duracion_pulso = 0;


 while( PORTD.F0  == 1) {
 Delay_ms(1);
 duracion_pulso++;
 if(duracion_pulso > 100) break;
 }


 if (duracion_pulso >= 5 && duracion_pulso <= 20) {
 Sound_Play(261, 100); Sound_Play(329, 100);
 Sound_Play(392, 100); Sound_Play(523, 200);
 }
 else if (duracion_pulso >= 21 && duracion_pulso <= 40) {
 Sound_Play(392, 40); Sound_Play(523, 40); Sound_Play(659, 40);
 }
 else if (duracion_pulso >= 41 && duracion_pulso <= 60) {
 Sound_Play(784, 25); Sound_Play(1047, 35);
 }
 else if (duracion_pulso >= 61 && duracion_pulso <= 90) {
 Sound_Play(329, 150); Sound_Play(294, 150);
 Sound_Play(261, 200); Sound_Play(120, 350);
 }
 }
 }
}
