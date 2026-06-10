
_main:

;sonidoscode.c,9 :: 		void main() {
;sonidoscode.c,10 :: 		unsigned int duracion_pulso = 0;
	CLRF       main_duracion_pulso_L0+0
	CLRF       main_duracion_pulso_L0+1
;sonidoscode.c,12 :: 		ANSEL  = 0x00;
	CLRF       ANS7_bit
;sonidoscode.c,13 :: 		ANSELH = 0x00;
	CLRF       ANS13_bit
;sonidoscode.c,14 :: 		TRISB.F0 = 1; // RB0 (BEEP) como entrada
	BSF        TRISB0_bit, 0
;sonidoscode.c,16 :: 		Sound_Init(&PORTC, 2); // Sonido en RC2
	MOVLW      7
	MOVWF      FARG_Sound_Init_snd_port+0
	MOVLW      2
	MOVWF      FARG_Sound_Init_snd_pin+0
	CALL       _Sound_Init+0
;sonidoscode.c,18 :: 		while(1) {
L_main0:
;sonidoscode.c,20 :: 		if (SIG_BEEP == 1) {
	BTFSS      RD0_bit, 0
	GOTO       L_main2
;sonidoscode.c,21 :: 		duracion_pulso = 0;
	CLRF       main_duracion_pulso_L0+0
	CLRF       main_duracion_pulso_L0+1
;sonidoscode.c,24 :: 		while(SIG_BEEP == 1) {
L_main3:
	BTFSS      RD0_bit, 0
	GOTO       L_main4
;sonidoscode.c,25 :: 		Delay_ms(1);
	MOVLW      3
	MOVWF      R12+0
	MOVLW      151
	MOVWF      R13+0
L_main5:
	DECFSZ     R13+0, 1
	GOTO       L_main5
	DECFSZ     R12+0, 1
	GOTO       L_main5
	NOP
	NOP
;sonidoscode.c,26 :: 		duracion_pulso++;
	INCF       main_duracion_pulso_L0+0, 1
	BTFSC      RP1_bit, 2
	INCF       main_duracion_pulso_L0+1, 1
;sonidoscode.c,27 :: 		if(duracion_pulso > 100) break; // Evitar bucle infinito
	MOVF       main_duracion_pulso_L0+1, 0
	SUBLW      0
	BTFSS      RP1_bit, 2
	GOTO       L__main27
	MOVF       main_duracion_pulso_L0+0, 0
	SUBLW      100
L__main27:
	BTFSC      RP1_bit, 0
	GOTO       L_main6
	GOTO       L_main4
L_main6:
;sonidoscode.c,28 :: 		}
	GOTO       L_main3
L_main4:
;sonidoscode.c,31 :: 		if (duracion_pulso >= 5 && duracion_pulso <= 20) { // Pulso de ~10ms
	MOVLW      0
	SUBWF      main_duracion_pulso_L0+1, 0
	BTFSS      RP1_bit, 2
	GOTO       L__main28
	MOVLW      5
	SUBWF      main_duracion_pulso_L0+0, 0
L__main28:
	BTFSS      RP1_bit, 0
	GOTO       L_main9
	MOVF       main_duracion_pulso_L0+1, 0
	SUBLW      0
	BTFSS      RP1_bit, 2
	GOTO       L__main29
	MOVF       main_duracion_pulso_L0+0, 0
	SUBLW      20
L__main29:
	BTFSS      RP1_bit, 0
	GOTO       L_main9
L__main25:
;sonidoscode.c,32 :: 		Sound_Play(261, 100); Sound_Play(329, 100);
	MOVLW      5
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      100
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
	MOVLW      73
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      100
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
;sonidoscode.c,33 :: 		Sound_Play(392, 100); Sound_Play(523, 200);
	MOVLW      136
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      100
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
	MOVLW      11
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      2
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      200
	MOVWF      FARG_Sound_Play_duration_ms+0
	CLRF       FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
;sonidoscode.c,34 :: 		}
	GOTO       L_main10
L_main9:
;sonidoscode.c,35 :: 		else if (duracion_pulso >= 21 && duracion_pulso <= 40) { // Pulso de ~30ms
	MOVLW      0
	SUBWF      main_duracion_pulso_L0+1, 0
	BTFSS      RP1_bit, 2
	GOTO       L__main30
	MOVLW      21
	SUBWF      main_duracion_pulso_L0+0, 0
L__main30:
	BTFSS      RP1_bit, 0
	GOTO       L_main13
	MOVF       main_duracion_pulso_L0+1, 0
	SUBLW      0
	BTFSS      RP1_bit, 2
	GOTO       L__main31
	MOVF       main_duracion_pulso_L0+0, 0
	SUBLW      40
L__main31:
	BTFSS      RP1_bit, 0
	GOTO       L_main13
L__main24:
;sonidoscode.c,36 :: 		Sound_Play(392, 40); Sound_Play(523, 40); Sound_Play(659, 40);
	MOVLW      136
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      40
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
	MOVLW      11
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      2
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      40
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
	MOVLW      147
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      2
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      40
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
;sonidoscode.c,37 :: 		}
	GOTO       L_main14
L_main13:
;sonidoscode.c,38 :: 		else if (duracion_pulso >= 41 && duracion_pulso <= 60) { // Pulso de ~50ms
	MOVLW      0
	SUBWF      main_duracion_pulso_L0+1, 0
	BTFSS      RP1_bit, 2
	GOTO       L__main32
	MOVLW      41
	SUBWF      main_duracion_pulso_L0+0, 0
L__main32:
	BTFSS      RP1_bit, 0
	GOTO       L_main17
	MOVF       main_duracion_pulso_L0+1, 0
	SUBLW      0
	BTFSS      RP1_bit, 2
	GOTO       L__main33
	MOVF       main_duracion_pulso_L0+0, 0
	SUBLW      60
L__main33:
	BTFSS      RP1_bit, 0
	GOTO       L_main17
L__main23:
;sonidoscode.c,39 :: 		Sound_Play(784, 25); Sound_Play(1047, 35);
	MOVLW      16
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      3
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      25
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
	MOVLW      23
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      4
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      35
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
;sonidoscode.c,40 :: 		}
	GOTO       L_main18
L_main17:
;sonidoscode.c,41 :: 		else if (duracion_pulso >= 61 && duracion_pulso <= 90) { // Pulso de ~70ms
	MOVLW      0
	SUBWF      main_duracion_pulso_L0+1, 0
	BTFSS      RP1_bit, 2
	GOTO       L__main34
	MOVLW      61
	SUBWF      main_duracion_pulso_L0+0, 0
L__main34:
	BTFSS      RP1_bit, 0
	GOTO       L_main21
	MOVF       main_duracion_pulso_L0+1, 0
	SUBLW      0
	BTFSS      RP1_bit, 2
	GOTO       L__main35
	MOVF       main_duracion_pulso_L0+0, 0
	SUBLW      90
L__main35:
	BTFSS      RP1_bit, 0
	GOTO       L_main21
L__main22:
;sonidoscode.c,42 :: 		Sound_Play(329, 150); Sound_Play(294, 150);
	MOVLW      73
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      150
	MOVWF      FARG_Sound_Play_duration_ms+0
	CLRF       FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
	MOVLW      38
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      150
	MOVWF      FARG_Sound_Play_duration_ms+0
	CLRF       FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
;sonidoscode.c,43 :: 		Sound_Play(261, 200); Sound_Play(120, 350);
	MOVLW      5
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      200
	MOVWF      FARG_Sound_Play_duration_ms+0
	CLRF       FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
	MOVLW      120
	MOVWF      FARG_Sound_Play_freq_in_hz+0
	MOVLW      0
	MOVWF      FARG_Sound_Play_freq_in_hz+1
	MOVLW      94
	MOVWF      FARG_Sound_Play_duration_ms+0
	MOVLW      1
	MOVWF      FARG_Sound_Play_duration_ms+1
	CALL       _Sound_Play+0
;sonidoscode.c,44 :: 		}
L_main21:
L_main18:
L_main14:
L_main10:
;sonidoscode.c,45 :: 		}
L_main2:
;sonidoscode.c,46 :: 		}
	GOTO       L_main0
;sonidoscode.c,47 :: 		}
L_end_main:
	GOTO       $+0
; end of _main
