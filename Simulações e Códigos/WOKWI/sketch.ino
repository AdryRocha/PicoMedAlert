// Incluindo bibliotecas disponíveis no ambiente Arduino para RP2040
#include <Arduino.h>        // Biblioteca principal do Arduino para RP2040
#include <HardwareSerial.h> // Para comunicação UART

// Definindo os pinos GPIO que vamos usar no projeto
#define PINO_BOTAO_DIA 5    // Pino do botão A para o período diurno
#define PINO_BOTAO_NOITE 6  // Pino do botão B para o período noturno
#define PINO_LED_DIA 12     // Pino do LED que indica o período diurno
#define PINO_LED_NOITE 11   // Pino do LED que indica o período noturno

// Configurações da UART (usaremos Serial1 no Arduino)
#define UART_BAUD 115200    // Velocidade da UART (115200 baud)

// Variáveis globais para controlar o estado do sistema
volatile int periodo_dia = 1;        // 1 = dia, 0 = noite
volatile int botao_dia_pressionado = 0;    // 1 se o botão diurno foi pressionado no período
volatile int botao_noite_pressionado = 0;  // 1 se o botão noturno foi pressionado no período
int mudancas_periodo = 0;            // Conta mudanças de período desde o último reset
char mensagem_alerta[] = "ALERTA: Remédio não consumido!"; // Mensagem de alerta

static uint32_t ultimo_debug = 0;

// Função de configuração inicial (executada uma vez no início)
void setup() {
    // Inicializa a comunicação serial USB para debug
    Serial.begin(UART_BAUD);
    while (!Serial) { // Aguarda a conexão serial estar pronta
        delay(10);
    }
    Serial.println("Sistema inicializado!");

    // Configura os botões como entradas com pull-up interno
    pinMode(PINO_BOTAO_DIA, INPUT_PULLUP);   // Botão diurno (pressionado = LOW)
    pinMode(PINO_BOTAO_NOITE, INPUT_PULLUP); // Botão noturno (pressionado = LOW)

    // Configura os LEDs como saídas
    pinMode(PINO_LED_DIA, OUTPUT);   // LED diurno
    pinMode(PINO_LED_NOITE, OUTPUT); // LED noturno

    // Inicializa a UART alternativa (Serial1)
    Serial1.begin(UART_BAUD);
    Serial1.println("Sistema inicializado!");
}

// Função para enviar um alerta via UART (Serial1)
void enviar_alerta() {
    Serial1.println(mensagem_alerta); // Envia pela UART
    Serial.print("Alerta enviado via UART: ");
    Serial.println(mensagem_alerta);  // Mostra no monitor serial para debug
}

// Função principal de loop (executada continuamente)
void loop() {
    static uint32_t ultima_mudanca_periodo = 0; // Tempo da última troca de período
    uint32_t tempo_atual = millis(); // Tempo atual em milissegundos

    // Atualiza os LEDs conforme o período atual
    if (periodo_dia && !botao_dia_pressionado) { // se for dia e o botao nao tiver sido pressionado
        digitalWrite(PINO_LED_DIA, HIGH);   // Acende LED diurno
        digitalWrite(PINO_LED_NOITE, LOW);  // Apaga LED noturno
    } else if (!periodo_dia && !botao_noite_pressionado) { // se for noite e o botao nao tiver sido pressionado
        digitalWrite(PINO_LED_DIA, LOW);   // Apaga LED diurno
        digitalWrite(PINO_LED_NOITE, HIGH);  // Acende LED noturno
    } else {
        digitalWrite(PINO_LED_DIA, LOW);    // Apaga LED diurno
        digitalWrite(PINO_LED_NOITE, LOW); // Apaga LED noturno
    }

    // Verifica os botões com debounce e debug detalhado
    if (!gpio_get(PINO_BOTAO_DIA)) {
        if (!botao_dia_pressionado) { // Só registra se ainda não foi pressionado neste período
            botao_dia_pressionado = 1;
            Serial1.println("Remédio diurno confirmado! (Botão pressionado)");
        }
        delay(200); // Debounce para evitar leituras múltiplas
    }
    if (digitalRead(PINO_BOTAO_NOITE) == LOW) {
        if (!botao_noite_pressionado) { // Só registra se ainda não foi pressionado neste período
            botao_noite_pressionado = 1;
            Serial1.println("Remédio noturno confirmado! (Botão pressionado)");
        }
        delay(200); // Debounce
    }

    // Mostra o estado atual para debug a cada 1 segundo
    if (tempo_atual - ultimo_debug > 1000) {
        Serial.print("Período: ");
        Serial.print(periodo_dia ? "DIA" : "NOITE");
        Serial.print(" | Botão Dia: ");
        Serial.print(botao_dia_pressionado);
        Serial.print(" | Botão Noite: ");
        Serial.print(botao_noite_pressionado);
        Serial.print(" | Mudanças: ");
        Serial.println(mudancas_periodo);
        ultimo_debug = tempo_atual;
    }

    // Verifica se houve mudança de período e se o remédio foi consumido
    if (mudancas_periodo > 0) {
        if ((!periodo_dia && !botao_dia_pressionado) || (periodo_dia && !botao_noite_pressionado)) {
            enviar_alerta(); // Envia alerta apenas se o remédio não foi confirmado
            Serial1.println("Alerta disparado porque o remédio não foi consumido.");
        } else {
            Serial1.println("Remédio consumido corretamente neste período.");
        }
        // Reseta os estados após a verificação
        botao_dia_pressionado = 0;
        botao_noite_pressionado = 0;
        mudancas_periodo = 0;
        Serial1.println("Estados resetados após verificação.");
    }

    // Troca o período a cada 10 segundos (para simulação no Wokwi)
    if (tempo_atual - ultima_mudanca_periodo > 10000) {
        periodo_dia = !periodo_dia; // Alterna entre dia e noite
        Serial1.print("Mudança de período: agora é ");
        Serial1.println(periodo_dia ? "DIA" : "NOITE");
        ultima_mudanca_periodo = tempo_atual;
        mudancas_periodo++; // Incrementa o contador de mudanças
    }

    delay(100); // Pausa de 100ms para não sobrecarregar o loop
}