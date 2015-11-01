#include "NewPing.h"
#include <SoftwareSerial.h>

#define DISTANCIA_MAXIMA                                200

#define TRIG_PIN_ULTRASOM_ESQUERDO                      12
#define ECHO_PIN_ULTRASOM_ESQUERDO                      11

#define TRIG_PIN_ULTRASOM_DIREITO                       4
#define ECHO_PIN_ULTRASOM_DIREITO                       2

#define TRIG_PIN_ULTRASOM_FRONTAL                       8
#define ECHO_PIN_ULTRASOM_FRONTAL                       9

#define PIN_MOTOR_ESQUERDO                              3
#define PIN_MOTOR_TRAZEIRO                              5
#define PIN_MOTOR_DIREITO                               6
#define PIN_MOTOR_FRONTAL                               10

#define LIMITE_ALERTA                                   90
#define DISTANCIA_RISCO                                 25
int DEBUG = 1; 

int CONTROLE_EXECUCAO = 0;
//0 : Testa os motores
//1 : Navegacao

int DISTANCIA_ATUAL_FRONTAL = 0;
int DISTANCIA_ANTERIOR_FRONTAL = 0;
int DISTANCIA_ATUAL_ESQUERDA = 0;
int DISTANCIA_ANTERIOR_ESQUERDA = 0;
int DISTANCIA_ATUAL_DIREITA = 0;
int DISTANCIA_ANTERIOR_DIREITA = 0;

NewPing frontal(TRIG_PIN_ULTRASOM_FRONTAL, ECHO_PIN_ULTRASOM_FRONTAL, DISTANCIA_MAXIMA);
NewPing esquerdo(TRIG_PIN_ULTRASOM_ESQUERDO, ECHO_PIN_ULTRASOM_ESQUERDO, DISTANCIA_MAXIMA);
NewPing direito(TRIG_PIN_ULTRASOM_DIREITO, ECHO_PIN_ULTRASOM_DIREITO, DISTANCIA_MAXIMA);

int motor_esquerdo_ativado = 0;
int distancia_motor_esquerdo = 0;

int motor_direito_ativado = 0;
int distancia_motor_direito = 0;

int motor_frontal_ativado = 0;
int distancia_motor_frontal = 0;

SoftwareSerial mySerial(1, 0); // RX, TX

void setup()
{
    Serial.begin(9600);
    Serial.begin(9600);
    while (!Serial) {

NewPing frontal(TRIG_PIN_ULTRASOM_FRONTAL, ECHO_PIN_ULTRASOM_FRONTAL, DISTANCIA_MAXIMA);
NewPing esquerdo(TRIG_PIN_ULTRASOM_ESQUERDO, ECHO_PIN_ULTRASOM_ESQUERDO, DISTANCIA_MAXIMA);
NewPing direito(TRIG_PIN_ULTRASOM_DIREITO, ECHO_PIN_ULTRASOM_DIREITO, DISTANCIA_MAXIMA);

int motor_esquerdo_ativado = 0;
int distancia_motor_esquerdo = 0;

int motor_direito_ativado = 0;
int distancia_motor_direito = 0;

int motor_frontal_ativado = 0;
int distancia_motor_frontal = 0;

SoftwareSerial mySerial(1, 0); // RX, TX

void setup()
{
    Serial.begin(9600);
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    pinMode(PIN_MOTOR_ESQUERDO, OUTPUT);
    pinMode(PIN_MOTOR_TRAZEIRO, OUTPUT);
    pinMode(PIN_MOTOR_DIREITO,  OUTPUT);
    pinMode(PIN_MOTOR_FRONTAL,  OUTPUT);
    mySerial.begin(9600);
    mySerial.println("Hello, world?");
}
void alerta_vibratorio(int pino, int potencia, int quantidade, int tempo_ligado)
{
   int i;
   for (i = 0; i < quantidade; i++)
   {
       
       analogWrite(pino, potencia);
       delay(tempo_ligado);
       analogWrite(pino, 0);
       delay(tempo_ligado);
   }
}

void testa_motor(NewPing &ultra_som, int &motor_ativado, int pin_motor, const char *posicao)
{

    int distancia = 0;
    if (motor_ativado != 0)
        return ;

    distancia = ultra_som.convert_cm(ultra_som.ping_median());
    if (DEBUG == 1) {
        Serial.print("Ping ");
        Serial.print(posicao);
        Serial.print(": ");
        Serial.print(distancia);
        Serial.println("cm");
    }

    if (distancia <= 5 && distancia != 0)
    {
        motor_ativado = 1;

        if (DEBUG == 1)
        {
            Serial.print("Vibrando motor ");
            Serial.println(posicao);
        }

        alerta_vibratorio(pin_motor, 254, 2, 100);
    }


}

int compare_distancias_frontais(NewPing &sonar)
{
    DISTANCIA_ATUAL_FRONTAL = sonar.convert_cm(ultra_som.ping_median());
    if (DISTANCIA_ATUAL_FRONTAL != 0) 
        return DISTANCIA_ATUAL_FRONTAL - DISTANCIA_ANTERIOR_FRONTAL;    
}

void trate_motores_frontais(int comparacao_distancias)
{
   if (comparacao_distancias < -10 && DISTANCIA_ATUAL_FRONTAL < LIMITE_ALERTA && DISTANCIA_ATUAL_FRONTAL != 0) 
       alerta_vibratorio(PIN_MOTOR_FRONTAL, 254, 2, 100);
   if (DISTANCIA_ATUAL_FRONTAL <= DISTANCIA_RISCO)
       alerta_vibratorio(PIN_MOTOR_FRONTAL, 254, 5, 200);
}


void printa_motores_testados(NewPing &ultra_som, int &motor_testado, const char *posicao)
{
    int distancia = 0;
    if (motor_testado != 1)
        return ;

    if (DEBUG == 1)
    {
        Serial.print("Motor ");
        Serial.print(posicao);
        Serial.println(" testado");
        distancia = ultra_som.convert_cm(ultra_som.ping_median());
        Serial.print(distancia);
        Serial.println("cm");
    }
    
}

void loop()
{
    if (CONTROLE_EXECUCAO == 0)
    {
        testa_motor(esquerdo, motor_esquerdo_ativado, PIN_MOTOR_ESQUERDO, "esquerdo");
        testa_motor(direito, motor_direito_ativado, PIN_MOTOR_DIREITO, "direito");
        testa_motor(frontal, motor_frontal_ativado,  PIN_MOTOR_FRONTAL, "frontal");
        printa_motores_testados(esquerdo, motor_esquerdo_ativado, "esquerdo");
        printa_motores_testados(direito, motor_direito_ativado, "direito");
        printa_motores_testados(frontal, motor_frontal_ativado, "frontal");

    //    if (motor_esquerdo_ativado == 1)
    //    {
    //        if (DEBUG == 1)
    //        {
    //            Serial.println("Motor esquerdo testado");
    //            distancia_motor_esquerdo = esquerdo.convert_cm(esquerdo.ping_median());
    //            Serial.print(distancia_motor_esquerdo);
    //            Serial.println("cm");
    //        }
    //    }

    //    if (motor_direito_ativado == 1)
    //    {
    //        if (DEBUG == 1)
    //        {
    //            Serial.println("Motor direito testado");
    //            distancia_motor_direito = direito.convert_cm(direito.ping_median());
    //            Serial.print(distancia_motor_direito);
    //            Serial.println("cm");
    //        }
    //    }

    //    if (motor_frontal_ativado == 1)
    //    {
    //        if (DEBUG == 1)
    //        {
    //            Serial.println("Motor frontal testado");
    //            distancia_motor_frontal = frontal.convert_cm(frontal.ping_median());
    //            Serial.print(distancia_motor_frontal);
    //            Serial.println("cm");
    //        }
    //    }

    //    if (motor_esquerdo_ativado == 1 && motor_direito_ativado == 1 && motor_frontal_ativado == 1)
    //    {// todos os motores já foram ativados fim do teste de motor
    //        CONTROLE_EXECUCAO = 1;
    //        if (DEBUG == 1)
    //        {
    //            Serial.println("Todos motores testados");
    //        }
    //        //alerta_vibratorio(PIN_MOTOR_TRAZEIRO, 254, 3, 200);
    //    }


    }//fim dos testes dos motores
    
    if (CONTROLE_EXECUCAO == 1)
    {
        if (DEBUG == 1) 
        {
            Serial.println("Entrando no modo de navegacao");     
        }
        trate_motores_frontais(compare_distancias_frontais(frontal));
    
    }
}
