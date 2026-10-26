#include <Arduino.h>
#include <HardwareSerial.h>
#include "driver/uart.h"

// --- Configuração dos Pinos (ESP32) ---
#define PINO_RX  16
#define PINO_TX  17
#define PINO_DIR 4

// --- CONFIGURAÇÃO DO NOVO ID ---
// Altere este valor para o ID que você deseja gravar no motor
#define NOVO_ID 1

// Função para escrever 1 byte na EEPROM
void escreverRegistrador1Byte(uint8_t id, uint8_t address, uint8_t valor) {
  uint8_t length      = 4;
  uint8_t instruction = 0x03; // WRITE
  uint32_t soma       = id + length + instruction + address + valor;
  uint8_t checksum    = (~soma) & 0xFF;

  byte pacote[] = { 0xFF, 0xFF, id, length, instruction, address, valor, checksum };

  Serial2.write(pacote, sizeof(pacote));
  Serial2.flush();
  delay(20);
}

// Função de troca de ID usando o Broadcast (0xFE fala com qualquer motor)
void mudarID(uint8_t idNovo) {
  Serial.print("Enviando comando para mudar ID para: "); 
  Serial.println(idNovo);

  // 0xFE = ID de Broadcast (funciona independente de qual ID o motor esteja agora)
  escreverRegistrador1Byte(0xFE, 55, 0);     // 1. Destrava a EEPROM
  delay(50);
  
  escreverRegistrador1Byte(0xFE, 5, idNovo); // 2. Grava o NOVO ID no endereço 5
  delay(150);                                // Tempo vital para a gravação física da EEPROM
  
  escreverRegistrador1Byte(idNovo, 55, 1);   // 3. Trava a EEPROM no novo ID
  delay(50);

  # Serial.println("====================================================");
  # Serial.println(" Processo concluído!");
  # Serial.println(" ATENÇÃO: DESLIGUE E LIGUE A FONTE DO MOTOR (Power Cycle)");
  # Serial.println(" para que ele assuma a nova configuração na memória RAM.");
  #Serial.println("====================================================");
# }

void setup() {
  Serial.begin(115200);
  delay(2000); // Tempo para abrir o Monitor Serial a tempo

  // Inicializa comunicação RS485
  Serial2.begin(1000000, SERIAL_8N1, PINO_RX, PINO_TX);
  uart_set_pin(UART_NUM_2, PINO_TX, PINO_RX, PINO_DIR, UART_PIN_NO_CHANGE);
  uart_set_mode(UART_NUM_2, UART_MODE_RS485_HALF_DUPLEX);

  Serial.println("\n--- PROGRAMA EXCLUSIVO: TROCA DE ID DE MOTOR ---");
  
  // Executa a troca apenas uma vez ao ligar
  mudarID(NOVO_ID);
}

void loop() {
  // Deixado vazio para não gravar repetidamente na EEPROM do motor
}
