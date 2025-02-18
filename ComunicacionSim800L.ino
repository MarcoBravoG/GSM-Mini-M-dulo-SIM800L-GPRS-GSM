#include <HardwareSerial.h>

HardwareSerial mySerial(1);  // Usamos el puerto serial 1 del ESP32

const char* APN = "internet";  // APN de Claro

const char* phoneNumber = "0989110444";  // Número al que enviar el mensaje

const int baudRate = 9600;  // Velocidad de baudios del SIM800L

void setup() {
  Serial.begin(115200);  // Comunicación con el PC
  mySerial.begin(baudRate, SERIAL_8N1, 16, 17);  // Configura los pines RX (16) y TX (17) del ESP32

  delay(1000);
  Serial.println("Configurando SIM800L...");
  
  // Comando AT para verificar si el módem está respondiendo
  sendATCommand("AT", "OK");

  // Configuración de APN
  sendATCommand("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\"", "OK");

  // Configurar el modo de texto para SMS
  sendATCommand("AT+CMGF=1", "OK");

  Serial.println("SIM800L listo para interactuar. Envíe comandos AT.");
}

void loop() {
  if (Serial.available()) {
    String comandoAT = Serial.readStringUntil('\n');  // Leer comando AT desde consola
    comandoAT.trim();  // Eliminar espacios al principio y al final

    if (comandoAT == "enviar") {
      // Si el comando es "enviar", envía el SMS
      sendSMS(phoneNumber, "MENSAJE DE PRUEBA");
    } else if (comandoAT.length() > 0) {
      // Si no es el comando "enviar", enviar como comando AT
      sendATCommand(comandoAT, "OK");
    }
  }
}

void sendATCommand(String command, String expectedResponse) {
  // Enviar comando AT
  Serial.print("Enviando comando AT: ");
  Serial.println(command);
  
  mySerial.println(command);  // Enviar comando al SIM800L
  delay(1000);  // Esperar respuesta
  
  String response = "";
  
  // Leer la respuesta del SIM800L
  while (mySerial.available()) {
    response += (char)mySerial.read();
  }

  // Mostrar respuesta en consola
  if (response.indexOf(expectedResponse) != -1) {
    Serial.println("Respuesta OK: " + response);
  } else {
    Serial.println("Error o respuesta inesperada: " + response);
  }
}

void sendSMS(String number, String message) {
  // Enviar comando para enviar SMS
  mySerial.print("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  
  // Enviar el mensaje
  mySerial.print(message);
  mySerial.write(26);  // Código ASCII de Ctrl+Z para finalizar el mensaje
  delay(3000);  // Esperar para asegurarse de que el mensaje fue enviado

  Serial.println("Mensaje enviado a " + number);
}


