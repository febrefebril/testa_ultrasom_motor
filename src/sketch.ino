#include "NewPing.h"

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

int DEBUG = 1; 

int MOTORES_TESTADOS = 0;

NewPing frontal(TRIG_PIN_ULTRASOM_FRONTAL, ECHO_PIN_ULTRASOM_FRONTAL, DISTANCIA_MAXIMA);
NewPing esquerdo(TRIG_PIN_ULTRASOM_ESQUERDO, ECHO_PIN_ULTRASOM_ESQUERDO, DISTANCIA_MAXIMA);
NewPing direito(TRIG_PIN_ULTRASOM_DIREITO, ECHO_PIN_ULTRASOM_DIREITO, DISTANCIA_MAXIMA);

int motor_esquerdo_ativado = 0;
int distancia_motor_esquerdo = 0;

int motor_direito_ativado = 0;
int distancia_motor_direito = 0;

int motor_frontal_ativado = 0;
int distancia_motor_frontal = 0;

void setup()
{
    Serial.begin(115200);
    pinMode(PIN_MOTOR_ESQUERDO, OUTPUT);
    pinMode(PIN_MOTOR_TRAZEIRO, OUTPUT);
    pinMode(PIN_MOTOR_DIREITO,  OUTPUT);
    pinMode(PIN_MOTOR_FRONTAL,  OUTPUT);
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

void loop()
{
    if (MOTORES_TESTADOS == 0)
    {
        if (motor_esquerdo_ativado == 0)
        {
            Serial.print("Ping esquerdo: ");
            distancia_motor_esquerdo = esquerdo.convert_cm(esquerdo.ping_median());
            Serial.print(distancia_motor_esquerdo);
            Serial.println("cm");
            if (distancia_motor_esquerdo <= 5 && distancia_motor_esquerdo != 0)
            {
                motor_esquerdo_ativado = 1;

                if (DEBUG == 1)
                {
                    Serial.println("Vibrando motor esquerdo");
                }

                alerta_vibratorio(PIN_MOTOR_ESQUERDO, 254, 2, 100);
                //analogWrite(PIN_MOTOR_ESQUERDO, 254);
                //delay(100);
                //analogWrite(PIN_MOTOR_ESQUERDO, 0);
                //delay(100);

                //analogWrite(PIN_MOTOR_ESQUERDO, 254);
                //delay(100);
                //analogWrite(PIN_MOTOR_ESQUERDO, 0);

            }
        }


        if (motor_direito_ativado == 0)
        {
            Serial.print("Ping direito: ");
            distancia_motor_direito = direito.convert_cm(direito.ping_median());
            Serial.print(distancia_motor_direito);
            Serial.println("cm");
            if (distancia_motor_direito <= 5 && distancia_motor_direito != 0)
            {
                motor_direito_ativado = 1;
                if (DEBUG == 1)
                {
                    Serial.println("Vibrando motor direito");
                }

                alerta_vibratorio(PIN_MOTOR_DIREITO, 254, 2, 100);
                //analogWrite(PIN_MOTOR_DIREITO, 254);
                //delay(100);
                //analogWrite(PIN_MOTOR_DIREITO, 0);
                //delay(100);

                //analogWrite(PIN_MOTOR_DIREITO, 254);
                //delay(100);
                //analogWrite(PIN_MOTOR_DIREITO, 0);
            }
        }

        if (motor_frontal_ativado == 0)
        {
            Serial.print("Ping frontal: ");
            distancia_motor_frontal = frontal.convert_cm(frontal.ping_median());
            Serial.print(distancia_motor_frontal);
            Serial.println("cm");
            if (distancia_motor_frontal <= 5 && distancia_motor_frontal != 0)
            {
                motor_frontal_ativado = 1;
                if (DEBUG == 1)
                {
                    Serial.println("Vibrando motor frontal");
                }

                alerta_vibratorio(PIN_MOTOR_FRONTAL, 254, 2,100);
                //analogWrite(PIN_MOTOR_FRONTAL, 254);
                //delay(100);
                //analogWrite(PIN_MOTOR_FRONTAL, 0);
                //delay(100);

                //analogWrite(PIN_MOTOR_FRONTAL, 254);
                //delay(100);
                //analogWrite(PIN_MOTOR_FRONTAL, 0);
            }
        }

        if (motor_esquerdo_ativado == 1)
        {
            if (DEBUG == 1)
            {
                Serial.println("Motor esquerdo testado");
                distancia_motor_esquerdo = esquerdo.convert_cm(esquerdo.ping_median());
                Serial.print(distancia_motor_esquerdo);
                Serial.println("cm");
            }
        }

        if (motor_direito_ativado == 1)
        {
            if (DEBUG == 1)
            {
                Serial.println("Motor direito testado");
                distancia_motor_direito = direito.convert_cm(direito.ping_median());
                Serial.print(distancia_motor_direito);
                Serial.println("cm");
            }
        }

        if (motor_frontal_ativado == 1)
        {
            if (DEBUG == 1)
            {
                Serial.println("Motor frontal testado");
                distancia_motor_frontal = frontal.convert_cm(frontal.ping_median());
                Serial.print(distancia_motor_frontal);
            }
        }
        if (motor_esquerdo_ativado == 1 && motor_direito_ativado == 1 && motor_frontal_ativado == 1)
        {// todos os motores já foram ativados fim do teste de motor
            MOTORES_TESTADOS = 1;
            if (DEBUG == 1)
            {
               Serial.println("Todos motores testados");
            }
            alerta_vibratorio(PIN_MOTOR_TRAZEIRO, 254, 2, 100);
        }


    }//fim dos testes dos motores
}
