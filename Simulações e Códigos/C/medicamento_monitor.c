// Essas linhas são como pegar ferramentas antes de começar um projeto manual.
// Elas dizem ao computador quais funções vamos usar.
// Para mostrar mensagens no terminal (ex.: "printf").
#include <stdio.h>
// Ferramentas básicas do Pico W, como esperar um tempo.
//#include "pico/stdlib.h"      
#include "stdlib.h"        
// Biblioteca para Wi-Fi no Pico W (chip CYW43)
include "pico/cyw43_arch.h"
//include "cyw43_arch.h"
// Biblioteca para DMA no Pico W
#include "hardware/dma.h"
// Biblioteca para UART (necessária para configurar DMA com Serial)
#include "hardware/uart.h"
// Ferramentas para controlar pinos, como botões e LEDs.
#include "hardware/gpio.h"      

// -------------------------------
// ------ VARIAVEIS GLOBAIS ------
// -------------------------------

// Definição de pinos
const int pinoBOTAO_DIA = 5;    // Pino digital para o botão de controle do dia (GPIO 2)
const int pinoBOTAO_NOITE = 6;  // Pino digital para o botão de controle da noite (GPIO 8)
const int pinoLED_DIA = 12;     // Pino digital para o LED indicador de dia (GPIO 15)
const int pinoLED_NOITE = 11;   // Pino digital para o LED indicador de noite (GPIO 16)

// Variáveis globais
int dia = 1;                    // 1 = diurno, 0 = noturno
int mudancaPeriodoDia = 0;      // Flag para transição de período
volatile int inBotaoNoite = 0;  // Estado do botão noturno (volátil por interrupção)
volatile int inBotaoDia = 0;    // Estado do botão diurno (volátil por interrupção)
int estoqueDia = 10;            // Estoque inicial de comprimidos diurnos
int estoqueNoite = 10;          // Estoque inicial de comprimidos noturnos
const int LIMIAR_ESTOQUE = 3;   // Limiar para alerta de estoque baixo

// Controle de tempo (12 horas = 43.200.000 ms)
const unsigned long INTERVALO_PERIODO = 43200000UL; // 12 horas em milissegundos
unsigned long ultimoPeriodo = 0; // Última vez que o período mudou

// Variáveis Bluetooth com DMA
const int TAMANHO_BUFFER = 10;  // Tamanho máximo do comando Bluetooth (ex.: "ADD 12 R1*")
char bluetooth_buffer[TAMANHO_BUFFER]; // Buffer para armazenar dados via DMA
int dma_chan;                   // Canal DMA
String string = "";             // String para processar comandos Bluetooth
int tamanho = 0;                // Tamanho da string recebida
int cmd = 0;                    // Flag para comando Bluetooth
String comando;                 // Comando completo recebido

// Configurações Wi-Fi
const char* ssid = "SUA_REDE_WIFI";      // Substituir pelo SSID da rede
const char* password = "SUA_SENHA_WIFI"; // Substituir pela senha da rede

// --------------------------------
// ------ FUNÇÕES DE INTERRUPÇÃO ------
// --------------------------------
// Essas funções são como "alarmes" que disparam quando apertamos um botão.
/**
 * Interrupção para o botão noturno.
 * Marca que o remédio noturno foi tomado.
 */
void IRAM_ATTR botaoNoiteISR() {
    inBotaoNoite = 1;
}

/**
 * Interrupção para o botão diurno.
 * Marca que o remédio diurno foi tomado.
 */
void IRAM_ATTR botaoDiaISR() {
    inBotaoDia = 1;
}

// -------------------------
// --------- SETUP ---------
// -------------------------
/**
 * Função de configuração inicial.
 * Configura pinos, Wi-Fi, UART e DMA para leitura Bluetooth.
 */
void setup() {
    // Inicializa pinos
    pinMode(pinoBOTAO_DIA, INPUT_PULLUP);
    pinMode(pinoBOTAO_NOITE, INPUT_PULLUP);
    pinMode(pinoLED_DIA, OUTPUT);
    pinMode(pinoLED_NOITE, OUTPUT);

    // Configura interrupções dos botões
    attachInterrupt(digitalPinToInterrupt(pinoBOTAO_DIA), botaoDiaISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(pinoBOTAO_NOITE), botaoNoiteISR, FALLING);

    // Inicializa comunicação serial (UART0 por padrão no Pico W)
    Serial.begin(115200);
    delay(100);

    // Inicializa Wi-Fi com CYW43
    if (cyw43_arch_init()) {
        Serial.println("Erro ao inicializar Wi-Fi");
        return;
    }
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        Serial.println("Erro ao conectar ao Wi-Fi");
    } else {
        Serial.println("Conectado ao Wi-Fi");
    }

    // Configura DMA para leitura da UART (Bluetooth)
    dma_chan = dma_claim_unused_channel(true); // Reserva um canal DMA
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8); // 8 bits por caractere
    channel_config_set_read_increment(&cfg, false);          // Lê do registrador UART
    channel_config_set_write_increment(&cfg, true);          // Escreve incrementalmente no buffer
    channel_config_set_dreq(&cfg, DREQ_UART0_RX);            // Sincroniza com UART0 RX
    dma_channel_configure(dma_chan, &cfg, bluetooth_buffer, &uart0_hw->dr, TAMANHO_BUFFER, false);

    ultimoPeriodo = millis(); // Marca o início do primeiro período
}

// --------------------------------
// ------ FUNCOES AUXILIARES ------
// --------------------------------

/**
 * Verifica o período com base em um temporizador de 12 horas.
 * Alterna entre dia e noite a cada 12 horas.
 */
void verificaPeriodo() {
    unsigned long agora = millis();
    if (agora - ultimoPeriodo >= INTERVALO_PERIODO) {
        int periodoAtual = dia;
        dia = !dia; // Alterna entre dia (1) e noite (0)
        ultimoPeriodo = agora;
        if (periodoAtual != dia) mudancaPeriodoDia = 1; // Marca mudança de período
    }
}

/**
 * Atualiza o estado dos LEDs usando a API do CYW43.
 * @param outLedRemedio Pino do LED a ser atualizado
 * @param estadoLed "acender" ou "apagar"
 */
void atualizarEstadoLed(const int outLedRemedio, const String estadoLed) {
    cyw43_arch_gpio_put(outLedRemedio, (estadoLed == "acender") ? 1 : 0);
}

/**
 * Envia um alerta via Serial e (futuramente) Wi-Fi.
 * @param periodo "Dia" ou "Noite"
 */
void enviarAlerta(const String periodo) {
    String mensagem = "ALERTA: Remedio nao consumido do periodo: " + periodo;
    Serial.println(mensagem);
    // TODO: Implementar envio Wi-Fi via CYW43 (ex.: UDP ou TCP)
}

/**
 * Verifica o estoque de remédios e emite alertas se estiver baixo.
 */
void verificaEstoqueRemedios() {
    if (estoqueDia <= LIMIAR_ESTOQUE) {
        Serial.println("ALERTA: Estoque de remédios diurnos baixo (" + String(estoqueDia) + ")");
    }
    if (estoqueNoite <= LIMIAR_ESTOQUE) {
        Serial.println("ALERTA: Estoque de remédios noturnos baixo (" + String(estoqueNoite) + ")");
    }
}

/**
 * Reinicializa parâmetros ao mudar de período.
 * Atualiza LEDs, verifica consumo anterior e ajusta estoque.
 */
void reinicializaParametrosMudancaPeriodo(volatile int& inBotaoRemedioAtual, 
                                          const int outLedRemedioAtual, 
                                          volatile int& inBotaoRemedioAnterior,
                                          const int outLedRemedioAnterior,
                                          const String periodoAnterior) {
    inBotaoRemedioAtual = 0;
    atualizarEstadoLed(outLedRemedioAtual, "acender");
    mudancaPeriodoDia = 0;

    if (inBotaoRemedioAnterior == 0) {
        enviarAlerta(periodoAnterior);
    } else {
        if (periodoAnterior == "Dia") estoqueDia--;
        else estoqueNoite--;
    }
    atualizarEstadoLed(outLedRemedioAnterior, "apagar");
}

/**
 * Lê dados Bluetooth usando DMA.
 * Inicia a transferência DMA e processa o buffer quando completo.
 */
void leituraDadosBluetooth() {
    // Verifica se há dados disponíveis na UART e inicia DMA se necessário
    if (uart_is_readable(uart0) && !dma_channel_is_busy(dma_chan)) {
        dma_channel_start(dma_chan); // Inicia transferência DMA
    }

    // Quando DMA termina, processa o buffer
    if (!dma_channel_is_busy(dma_chan) && tamanho < TAMANHO_BUFFER) {
        for (int i = 0; i < TAMANHO_BUFFER; i++) {
            char c = bluetooth_buffer[i];
            if (c == '*') {
                cmd = 1;
                comando = string;
                string = "";
                tamanho = 0;
                break; // Para ao encontrar o terminador
            } else if (c != 0) { // Ignora bytes nulos
                string += c;
                tamanho++;
            }
        }
        // Reseta buffer após leitura
        memset(bluetooth_buffer, 0, TAMANHO_BUFFER);
    }

    // Verifica overflow
    if (tamanho >= TAMANHO_BUFFER) {
        Serial.println("ERRO: TAMANHO VALIDO EXCEDIDO");
        string = "";
        tamanho = 0;
    }
}

/**
 * Decodifica e executa comandos Bluetooth.
 * Exemplo: "ADD 12 R1*" adiciona 12 comprimidos ao remédio diurno.
 */
void decodificaComandosBluetooth() {
    if (cmd == 1) {
        if (comando.substring(0, 4) == "ADD ") {
            if (comando.length() < 5) {
                Serial.println("ERRO: PARAMETRO AUSENTE");
            } else if (isDigit(comando.charAt(4)) && isDigit(comando.charAt(5))) {
                String qtdStr = comando.substring(4, 6);
                int qtidadeComprimidos = qtdStr.toInt();
                if (qtidadeComprimidos >= 0 && qtidadeComprimidos <= 99) {
                    if (comando.substring(7) == "R1") {
                        estoqueDia += qtidadeComprimidos;
                        Serial.println("OK - Adicionados " + qtdStr + " comprimidos ao R1 (Dia)");
                    } else if (comando.substring(7) == "R2") {
                        estoqueNoite += qtidadeComprimidos;
                        Serial.println("OK - Adicionados " + qtdStr + " comprimidos ao R2 (Noite)");
                    } else {
                        Serial.println("ERRO: REMEDIO INVALIDO");
                    }
                } else {
                    Serial.println("ERRO: PARAMETRO INCORRETO");
                }
            } else {
                Serial.println("ERRO: PARAMETRO INCORRETO");
            }
        } else {
            Serial.println("ERRO: COMANDO INEXISTENTE");
        }
        cmd = 0;
    }
}

// --------------------------------
// ------- FUNCAO PRINCIPAL -------
// --------------------------------
/**
 * Loop principal.
 * Gerencia períodos, estoque, LEDs e comunicação Bluetooth.
 */
void loop() {
    cyw43_arch_poll(); // Mantém o Wi-Fi funcionando
    verificaPeriodo();
    verificaEstoqueRemedios();
    leituraDadosBluetooth();
    decodificaComandosBluetooth();

    if (dia == 1) {
        if (mudancaPeriodoDia == 1) {
            reinicializaParametrosMudancaPeriodo(inBotaoDia, pinoLED_DIA, 
                                                 inBotaoNoite, pinoLED_NOITE, "Noite");
        }
        atualizarEstadoLed(pinoLED_DIA, (inBotaoDia == 0) ? "acender" : "apagar");
    } else {
        if (mudancaPeriodoDia == 1) {
            reinicializaParametrosMudancaPeriodo(inBotaoNoite, pinoLED_NOITE, 
                                                 inBotaoDia, pinoLED_DIA, "Dia");
        }
        atualizarEstadoLed(pinoLED_NOITE, (inBotaoNoite == 0) ? "acender" : "apagar");
    }
    delay(100); // Atraso para evitar uso excessivo de CPU
}
```