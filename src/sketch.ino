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
#define DISTANCIA_RISCO                15
#define VARIACAO_ALERTA                -10

#define POTENCIA_RISCO                 254
#define DELAY_RISCO                    100
#define REPETICAO_RISCO                2

#define POTENCIA_ALERTA                150
#define DELAY_ALERTA                   100
#define REPETICAO_ALERTA               2

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
    mySerial.println(F("Hello, world?"));
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
        Serial.print(F("Ping "));
        Serial.print(posicao);
        Serial.print(F(": "));
        Serial.print(distancia);
        Serial.println(F("cm"));
    }

    if (distancia <= 5 && distancia != 0)
    {
        motor_ativado = 1;

        if (DEBUG == 1)
        {
            Serial.print(F("Vibrando motor "));
            Serial.println(posicao);
        }

        alerta_vibratorio(pin_motor, 254, 2, 100);
    }


}
/*
                LE DISTANCIA EM CM 
*/
int distancia_cm(NewPing &sonar, const char *posicao)
{
    int distancia = sonar.convert_cm(sonar.ping_median());
    if (DEBUG == 1) 
    {
        Serial.print(F("Distancia lida em cm do ultrasom: ")); 
        Serial.print(posicao);
        Serial.print(F(" "));
        Serial.println(distancia);
    }
    if (posicao == "frontal")
    {
        DISTANCIA_ATUAL_FRONTAL = distancia;
    }
    else if (posicao == "direito")
    {
        DISTANCIA_ATUAL_DIREITA = distancia;
    }
    else
    {
        DISTANCIA_ATUAL_ESQUERDA = distancia;
    }
    return distancia;
}
/*
                COMPARA COM A DISTANCIA ANTERIOR
*/
int compara_distancias(int distancia_atual, int distancia_anterior)
{
    if (distancia_atual == 0)
    {
        if (DEBUG == 1)
        {
           Serial.println(F("Distancia fora do alcance. Nao acionaremos os motores"));
        }
        return distancia_atual;
    }

    int resultado_comparacao = distancia_atual - distancia_anterior;
    if (DEBUG == 1)
    {
        Serial.print(F("Resultado da comparacao: "));
        Serial.println(resultado_comparacao);
    }
    return resultado_comparacao;
}
/*
                TRATE MOTORES
*/
void tratar_motor(int delta_d, int distancia_atual, const char *posicao)
{
    if (delta_d == 0 || distancia_atual == 0)
    {
        if (DEBUG == 1)
        {
            Serial.println(F("Motores nao serao acionados"));
            return;
        }
    }

    if (distancia_atual <= DISTANCIA_RISCO)
    {
          procedimento_risco(posicao);
          return;
    }

    if (delta_d <= VARIACAO_ALERTA && distancia_atual <= LIMITE_ALERTA)
    {
        procedimento_alerta(posicao);
        return;
    }
}

void procedimento_risco(const char *posicao)
{

    if (DEBUG == 1)
    {
        Serial.print(F("Vibrando em distancia de risco o motor "));
        Serial.println(posicao);
    }
    if (posicao == "frontal")
    {
        alerta_vibratorio(PIN_MOTOR_FRONTAL, POTENCIA_RISCO, REPETICAO_RISCO, DELAY_RISCO);
    }
    else if (posicao == "esquerdo")
    {
        alerta_vibratorio(PIN_MOTOR_ESQUERDO, POTENCIA_RISCO, REPETICAO_RISCO, DELAY_RISCO);
    }
    else {
        alerta_vibratorio(PIN_MOTOR_DIREITO, POTENCIA_RISCO, REPETICAO_RISCO, DELAY_RISCO);
    }
    return;
}

void procedimento_alerta(const char *posicao)
{
    if (DEBUG == 1)
    {
        Serial.print(F("Vibrando por variacao de distancia e distancia de alerta o motor "));
        Serial.println(posicao);
    }
    if (posicao == "frontal")
    {
        alerta_vibratorio(PIN_MOTOR_FRONTAL, POTENCIA_ALERTA, REPETICAO_ALERTA, DELAY_ALERTA);
    }
    else if (posicao == "esquerdo")
    {
        alerta_vibratorio(PIN_MOTOR_ESQUERDO, POTENCIA_ALERTA, REPETICAO_ALERTA, DELAY_ALERTA);
    }
    else {
        alerta_vibratorio(PIN_MOTOR_DIREITO, POTENCIA_ALERTA, REPETICAO_ALERTA, DELAY_ALERTA);
    }
    return;
    
}

void printa_motores_testados(NewPing &ultra_som, int &motor_testado, const char *posicao)
{
    int distancia = 0;
    if (motor_testado != 1)
        return ;

    if (DEBUG == 1)
    {
        Serial.print(F("Motor "));
        Serial.print(posicao);
        Serial.println(F(" testado"));
        distancia = ultra_som.convert_cm(ultra_som.ping_median());
        Serial.print(distancia);
        Serial.println(F("cm"));
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
            Serial.println(F("Saindo do modo debug"));
        }
        if (byte_lido == 1) {
           DEBUG = 1; 
           Serial.println(F("Entrando no modo debug"));
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
            Serial.println(F("Entrando no modo de navegacao"));
        }
        //tratar_motor(compara_distancias(distancia_cm(frontal, "frontal"), DISTANCIA_ANTERIOR_FRONTAL), DISTANCIA_ATUAL_FRONTAL, "frontal");
        //DISTANCIA_ANTERIOR_FRONTAL = DISTANCIA_ATUAL_FRONTAL;
        //tratar_motor(compara_distancias(distancia_cm(esquerdo, "esquerda"), DISTANCIA_ANTERIOR_ESQUERDA), DISTANCIA_ATUAL_ESQUERDA, "esquerda");
        //DISTANCIA_ANTERIOR_ESQUERDA = DISTANCIA_ATUAL_ESQUERDA;
        //tratar_motor(compara_distancias(distancia_cm(direito, "direita"), DISTANCIA_ANTERIOR_DIREITA), DISTANCIA_ATUAL_DIREITA, "direita");
        //DISTANCIA_ANTERIOR_DIREITA = DISTANCIA_ATUAL_DIREITA;

        int delta_d = 0;
        DISTANCIA_ATUAL_FRONTAL = distancia_cm(frontal, "frontal");
        delta_d = compara_distancias(DISTANCIA_ATUAL_FRONTAL, DISTANCIA_ANTERIOR_FRONTAL);
        tratar_motor(delta_d, DISTANCIA_ATUAL_FRONTAL, "frontal");
        DISTANCIA_ANTERIOR_FRONTAL = DISTANCIA_ATUAL_FRONTAL;


        DISTANCIA_ATUAL_ESQUERDA = distancia_cm(esquerdo, "esquerdo");
        delta_d = compara_distancias(DISTANCIA_ATUAL_ESQUERDA, DISTANCIA_ANTERIOR_ESQUERDA);
        tratar_motor(delta_d, DISTANCIA_ATUAL_ESQUERDA, "esquerdo");
        DISTANCIA_ANTERIOR_ESQUERDA = DISTANCIA_ATUAL_ESQUERDA;

        DISTANCIA_ATUAL_DIREITA = distancia_cm(direito, "direito");
        delta_d = compara_distancias(DISTANCIA_ATUAL_DIREITA, DISTANCIA_ANTERIOR_DIREITA);
        tratar_motor(delta_d, DISTANCIA_ATUAL_DIREITA, "direito");
        DISTANCIA_ANTERIOR_DIREITA = DISTANCIA_ATUAL_DIREITA;
    }
}
