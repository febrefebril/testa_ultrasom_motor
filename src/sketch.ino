#include "NewPing.h"
#include <SoftwareSerial.h>

#define DISTANCIA_MAXIMA               200

#define TRIG_PIN_ULTRASOM_ESQUERDO     12
#define ECHO_PIN_ULTRASOM_ESQUERDO     11

#define TRIG_PIN_ULTRASOM_DIREITO      4
#define ECHO_PIN_ULTRASOM_DIREITO      2

#define TRIG_PIN_ULTRASOM_FRONTAL      8
#define ECHO_PIN_ULTRASOM_FRONTAL      9

#define PIN_MOTOR_ESQUERDO             3
#define PIN_MOTOR_TRAZEIRO             5
#define PIN_MOTOR_DIREITO              6
#define PIN_MOTOR_FRONTAL              10

#define LIMITE_ALERTA                  90
#define DISTANCIA_RISCO                25

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

int byte_lido = 0;
SoftwareSerial mySerial(1, 0); // RX - Pino 1, TX - Pino 0

void setup()
{
    Serial.begin(9600);
    while (!Serial) 
    {
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
    if (DEBUG == 1) 
    {
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
/*
                COMPARE MOTORES FRONTAIS 
*/
int compare_distancias_frontais(NewPing &sonar)
{
    int delta_d = 0;
    DISTANCIA_ATUAL_FRONTAL = sonar.convert_cm(sonar.ping_median());
    if (DEBUG == 1)
    {
        Serial.print("Distancia atual frontal: ");      
        Serial.println(DISTANCIA_ATUAL_FRONTAL);
        Serial.print("Distancia anterior frontal: ");      
        Serial.println(DISTANCIA_ANTERIOR_FRONTAL);
    }
    if (DISTANCIA_ATUAL_FRONTAL != 0) 
    {
        delta_d = DISTANCIA_ATUAL_FRONTAL - DISTANCIA_ANTERIOR_FRONTAL;    
        if (DEBUG ==1)
        {
            Serial.print("Diferenca entre distancia atual e anterio: ");
            Serial.println(delta_d);
        }
        return delta_d;
    }
}

void trate_motores_frontais(int comparacao_distancias)
{
   if (DISTANCIA_ATUAL_FRONTAL == 0) 
   {
      return;
   }

   if (comparacao_distancias < -10 && DISTANCIA_ATUAL_FRONTAL < LIMITE_ALERTA) 
   {
       if (DEBUG == 1)
       {
            Serial.println("Distancia abaixo do limite de alerta e alterada em mais de 10 cm");
       }
       alerta_vibratorio(PIN_MOTOR_FRONTAL, 254, 2, 100);
   }
   if (DISTANCIA_ATUAL_FRONTAL <= DISTANCIA_RISCO)
   {
       if (DEBUG == 1)
       {
            Serial.println("Distancia abaixo da distancia de risco");
       }
       alerta_vibratorio(PIN_MOTOR_FRONTAL, 254, 5, 100);
   }
}
/*
                COMPARE MOTORES ESQUERDOS
*/
int compare_distancias_esquerdas(NewPing &sonar)
{
    int delta_d = 0;
    DISTANCIA_ATUAL_ESQUERDA = sonar.convert_cm(sonar.ping_median());
    if (DEBUG == 1)
    {
        Serial.print("Distancia atual esquerda: ");      
        Serial.println(DISTANCIA_ATUAL_ESQUERDA);
        Serial.print("Distancia anterior esquerda: ");      
        Serial.println(DISTANCIA_ANTERIOR_ESQUERDA);
    }
    if (DISTANCIA_ATUAL_ESQUERDA != 0) 
    {
        delta_d = DISTANCIA_ATUAL_ESQUERDA - DISTANCIA_ANTERIOR_ESQUERDA;    
        if (DEBUG ==1)
        {
            Serial.print("Diferenca entre distancia atual e anterio: ");
            Serial.println(delta_d);
        }
        return delta_d;
    }
}

void trate_motores_esquerdos(int comparacao_distancias)
{
   if (DISTANCIA_ATUAL_ESQUERDA == 0) 
   {
      return;
   }

   if (comparacao_distancias < -10 && DISTANCIA_ATUAL_ESQUERDA < LIMITE_ALERTA) 
   {
       if (DEBUG == 1)
       {
            Serial.println("Distancia esquerda abaixo do limite de alerta e alterada em mais de 10 cm");
       }
       alerta_vibratorio(PIN_MOTOR_ESQUERDO, 254, 2, 100);
   }
   if (DISTANCIA_ATUAL_ESQUERDA <= DISTANCIA_RISCO)
   {
       if (DEBUG == 1)
       {
            Serial.println("Distancia esquerda abaixo da distancia de risco");
       }
       alerta_vibratorio(PIN_MOTOR_ESQUERDO, 254, 5, 100);
   }
}
/*
                COMPARE MOTORES DIREITOS
*/
int compare_distancias_direitas(NewPing &sonar)
{
    int delta_d = 0;
    DISTANCIA_ATUAL_DIREITA = sonar.convert_cm(sonar.ping_median());
    if (DEBUG == 1)
    {
        Serial.print("Distancia atual direita: ");      
        Serial.println(DISTANCIA_ATUAL_DIREITA);
        Serial.print("Distancia anterior direita: ");      
        Serial.println(DISTANCIA_ANTERIOR_DIREITA);
    }
    if (DISTANCIA_ATUAL_DIREITA != 0) 
    {
        delta_d = DISTANCIA_ATUAL_DIREITA - DISTANCIA_ANTERIOR_DIREITA;    
        if (DEBUG ==1)
        {
            Serial.print("Diferenca entre distancia atual e anterio: ");
            Serial.println(delta_d);
        }
        return delta_d;
    }
}

void trate_motores_direitos(int comparacao_distancias)
{
   if (DISTANCIA_ATUAL_DIREITA == 0) 
   {
      return;
   }

   if (comparacao_distancias < -10 && DISTANCIA_ATUAL_DIREITA < LIMITE_ALERTA) 
   {
       if (DEBUG == 1)
       {
            Serial.println("Distancia direita abaixo do limite de alerta e alterada em mais de 10 cm");
       }
       alerta_vibratorio(PIN_MOTOR_ESQUERDO, 254, 2, 100);
   }
   if (DISTANCIA_ATUAL_DIREITA <= DISTANCIA_RISCO)
   {
       if (DEBUG == 1)
       {
            Serial.println("Distancia direita abaixo da distancia de risco");
       }
       alerta_vibratorio(PIN_MOTOR_ESQUERDO, 254, 5, 100);
   }
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
    if (Serial.available() > 0)
    {
        byte_lido = Serial.parseInt();
        if (DEBUG == 1)
        {
            //Serial.print("Foi Recebido: ");      
            //Serial.println(bytes_lidos, DEC);
            if (byte_lido == 44)
            {//espera por uma virgula
               Serial.println(); 
            }
            else
            {//printa byte_lido
                Serial.write(byte_lido); 
            }
        }        
        if (byte_lido == 0) {
            DEBUG = 0;
            Serial.println("Saindo do modo debug");
        }
        if (byte_lido == 1) {
           DEBUG = 1; 
           Serial.println("Entrando no modo debug");
        }
    }

    if (CONTROLE_EXECUCAO == 0)
    {
        testa_motor(esquerdo, motor_esquerdo_ativado, PIN_MOTOR_ESQUERDO, "esquerdo");
        testa_motor(direito, motor_direito_ativado, PIN_MOTOR_DIREITO, "direito");
        testa_motor(frontal, motor_frontal_ativado,  PIN_MOTOR_FRONTAL, "frontal");
        printa_motores_testados(esquerdo, motor_esquerdo_ativado, "esquerdo");
        printa_motores_testados(direito, motor_direito_ativado, "direito");
        printa_motores_testados(frontal, motor_frontal_ativado, "frontal");
        if (motor_esquerdo_ativado == 1 && motor_direito_ativado == 1 && motor_frontal_ativado == 1)
        {// se todos os ultrasons jah foram testados, mude de estado
            CONTROLE_EXECUCAO = 1;
        }
    }//fim dos testes dos motores
    
    if (CONTROLE_EXECUCAO == 1)
    {
        if (DEBUG == 1) 
        {
            Serial.println("Entrando no modo de navegacao");     
        }
        trate_motores_frontais(compare_distancias_frontais(frontal));
        DISTANCIA_ANTERIOR_FRONTAL = DISTANCIA_ATUAL_FRONTAL;
        trate_motores_esquerdos(compare_distancias_esquerdas(esquerdo));
        DISTANCIA_ANTERIOR_ESQUERDA = DISTANCIA_ATUAL_ESQUERDA;
        trate_motores_direitos(compare_distancias_direitas(direito));
        DISTANCIA_ANTERIOR_DIREITA = DISTANCIA_ATUAL_DIREITA;
    
    }
}
