
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <strings_en.h>
#include <WiFiManager.h>
#include <UniversalTelegramBot.h>    
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <sstream>

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  
// Inicializa el bot
#define BOTtoken "5054316668:AAGeVhv--JgA2kL5Cy2AaqnUxFt4bnzBrvg"    //Se obtiene de Botfather
int long chats[] = {1621818993,0,0,0,0};

#define RELE1 4
#define RELE2 0
#define RELE3 2
#define RELE4 15
#define DISCRE1 32
#define DISCRE2 33
#define DISCRE3 25
#define DISCRE4 26

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Checkea mensajes cada 0.1 s
int botRequestDelay = 100;
unsigned long lastTimeBotRan;

//Inicializa las banderas y los pines ADC
const int ADC1 = 36;
const int ADC2 = 39;
const int ADC3 = 34;
const int ADC4 = 35;
//Por defecto mide temperatura
String uni = "°C";
String uni2 = "°C";
String uni3 = "°C";
String uni4 = "°C";
//Valor del ADC
float sens=0;
float sens2=0;
float sens3=0;
float sens4=0;
//Maximo valor de entrada analogica
float maxi=10;
float maxi2=10;
float maxi3=10;
float maxi4=10;
//Minimo valor de entrada analogica
float mini=0;
float mini2=0;
float mini3=0;
float mini4=0;
//Umbral entrada analogica
float umbral=5;
float umbral2=5;
float umbral3=5;
float umbral4=5;
//Flags para un aviso
bool flag = 0;
bool flag2 = 0; 
bool flag3 = 0;
bool flag4 = 0;
bool flag5 = 0;
bool flag6 = 0;
bool flag7 = 0;
bool flag8 = 0;
//Llave de pase
bool pass = 0;
bool pass1 = 0;
bool pass2 = 0;
bool pass3 = 0;
bool pass4 = 0;


//Funcion encargada de enviar el estado del sistema
void stateSystem(String chat_id){
    String state = "ESTADO DEL SISTEMA: \n \n";
    if (!digitalRead(RELE1)){
        state+= "-Rele 1 ON \n";
    }else{
        state+= "-Rele 1 OFF \n";
    }
    if (!digitalRead(RELE2)){
        state+= "-Rele 2 ON \n";
    }else{
        state+= "-Rele 2 OFF \n";
        } 
    if (!digitalRead(RELE3)){
        state+= "-Rele 3 ON \n";
    }else{
        state+= "-Rele 3 OFF \n";
        }
    if (!digitalRead(RELE4)){
        state+= "-Rele 4 ON \n";
    }else{
        state+= "-Rele 4 OFF \n";
        }
    state+="-----------------------\n"; 
    if (digitalRead(DISCRE1)){
        state+= "-Digital 1 ON \n";
    }else{
        state+= "-Digital 1 OFF \n";
        } 
    if (digitalRead(DISCRE2)){
        state+= "-Digital 2 ON \n";
    }else{
        state+= "-Digital 2 OFF \n";
        } 
    if (digitalRead(DISCRE3)){
        state+= "-Digital 3 ON \n";
    }else{
        state+= "-Digital 3 OFF \n";
        } 
    if (digitalRead(DISCRE4)){
        state+= "-Digital 4 ON \n";
    }else{
        state+= "-Digital 4 OFF \n";
        }
    state+="-----------------------\n"; 
    state+="-Sensor 1 " ;
    state+=String(sens);state+=" ";
    state+=uni;

    state+="\n-Sensor 2 " ;
    state+=String(sens2);state+=" ";
    state+=uni2;
    
    state+="\n-Sensor 3 " ;
    state+=String(sens3);state+=" ";
    state+=uni3;
    
    state+="\n-Sensor 4 " ;
    state+=String(sens4);state+=" ";
    state+=uni4;
    
    bot.sendMessage(chat_id,state);

}
//Funcion encargarda de Setear los sensores
void setSensor(String chat_id, String text){
    float MAX=0, MIN=0,UMB=0;
    String UNI;
    int cont=0,op;
    char textochar[text.length()+1];
    strcpy(textochar, text.c_str());
    char *separador =strtok(textochar,"/");
    if(separador != NULL){
        while(separador != NULL){
            if (cont==0){
                op=atoi(separador);
            }
            if (cont==1){
                MAX=atof(separador);
            }
            if (cont==2){
                MIN=atof(separador);
            }
            if (cont==3){
                UMB=atof(separador);
            }
            if (cont==4){
                UNI=String(separador);
            }
        // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
            separador = strtok(NULL, "/");
            cont++;
            Serial.print(MAX);
        }
        switch(op){

            case 1:
                    maxi=MAX;
                    mini=MIN;
                    umbral=UMB;
                    uni = UNI;
                    bot.sendMessage(chat_id, "Sensor 1 Configurado :)");
            break;
            case 2:
                    maxi2=MAX;
                    mini2=MIN;
                    umbral2=UMB;
                    uni2 = UNI;
                    bot.sendMessage(chat_id, "Sensor 2 Configurado :)");
            break;
            case 3:
                    maxi3=MAX;
                    mini3=MIN;
                    umbral3=UMB;
                    uni3 = UNI;
                    bot.sendMessage(chat_id, "Sensor 3 Configurado :)");
            break;
            case 4:
                    maxi4=MAX;
                    mini4=MIN;
                    umbral4=UMB;
                    uni4 = UNI;
                    bot.sendMessage(chat_id, "Sensor 4 Configurado :)");
            break;

            default:

                bot.sendMessage(chat_id, "Sensor Incorrecto ;(");
        }


}
pass4=0;

}
//Funcion encargada de mensaje de seteo
String Mensaje(){
   String seteo = "Por favor envie un mensaje con el siguiente formato \n";
        seteo+= "SENS/MAX/MIN/UMB/UNID \n";
        seteo+= "-----------------------\n";
        seteo+="Siendo:\n";
        seteo+="SENS = N° de Sensor a Configurar \n";
        seteo+="MAX = Tensión Máxima de sensor \n";
        seteo+="MIN = Tensión Mínima de sensor \n";
        seteo+="UMB = Umbral de Aviso \n";
        seteo+="UNID = Unidad de Medición \n";
        seteo+= "-----------------------\n";
        seteo+="Ejemplo:\n";
        seteo+="1/100/0/50/°C";
        return seteo;
}
//Funcion encargada de conexion de WiFi
void Wifiestatus(){
    if(WiFi.status() == WL_CONNECTED){
    
        lcd.setCursor(0, 1);
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("WIFI CONECTADO");
        delay(500);
        Serial.println(WiFi.localIP());
    }else{
        lcd.setCursor(0, 1);
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("WIFI NO CONECTADO :(");
      }
}
// Funcion encargada de administrar los mensajes
void handleNewMessages(int numNewMessages) {
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));
    bool aceptado = 0;
    int opc=0;
   //Identifica si es amigo del bot 
    for (int i=0; i<numNewMessages; i++){
        String chat_id = String(bot.messages[i].chat_id);
        for (int j=0; j<5; j++){
            if (chat_id == String(chats[j])){
                aceptado=1;
                break;
            }
        }
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
    
        if(pass4==1){
            setSensor(chat_id,text);
            opc=1;
        }
        if(pass3==1){ //Pass3 varible dedicada a Eliminar amigo
            if(text.toInt()>=0 and text.toInt()<5){
            chats[text.toInt()]=0;
            bot.sendMessage(chat_id, "Amigo Eliminado :(");
            }else{
            bot.sendMessage(chat_id, "Amigo incorrecto :(");
        }
                opc=1;
                pass3=0;
        }
        if(pass2==1){ //Pass2 variable dedicada a verificar contraseña para eliminar amigo
            int cont=0;
            if(text=="Futurama"){
                for(int i=0;i<5;i++){
                    if (chats[i]==0){
                        cont++;
                    }
                }
                if(cont<4){
                    String admins = "Que amigo queres que elimine?";
                    admins+="\n 0 - " + String(chats[0]);
                    admins+="\n 1 - " + String(chats[1]);
                    admins+="\n 2 - " + String(chats[2]);
                    admins+="\n 3 - " + String(chats[3]);
                    admins+="\n 4 - " + String(chats[4]);
                    bot.sendMessage(chat_id, admins);
                    pass3=1;
                }else{
                bot.sendMessage(chat_id,"Tu eres mi unico amigo, eliminarte sería una catástrofe");
                }
            }else{
                bot.sendMessage(chat_id,"Contraseña incorrecta :(");
            }
            pass2=0;
            opc=1;
        }else{
            if(pass1==1){ //Pass1 variable dedicada a agregar amigo
                Serial.print("Amigo nuevo!");
                bot.sendMessage(chat_id,"Ya tengo un nuevo amigo :)");
                for (int j=0; j<5; j++){
                    if(chats[j]==0){ 
                        chats[j]=text.toInt();
                        Serial.print(chats[j]);
                        j=4;
                    }
                }
                pass1=0;
                pass=0;
                opc=1;
            }
            if (pass==1){ //Pass1 variable dedicada a verificar contraseña a agregar amigo
                int cont1=0;
                if (text=="Futurama"){
                    bot.sendMessage(chat_id, "Ingrese ID de mi nuevo amigo");
                    pass1=1;
                
                }else{
                    pass=0;
                    bot.sendMessage(chat_id,"Contraseña incorrecta :/");
                    }
            }else{
                Serial.println(text);
                if(aceptado==1 and pass==0 and pass1==0){
                    if (text == "/start") {
                        String welcome = "Hola " + from_name + " soy Bender, En que te puedo ayudar?.\n";
                        bot.sendMessage(chat_id, mensajeStart(welcome), "");
                        opc=1;
                    }
                    //Setea sensores
                    if (text=="/Set"){
                        bot.sendMessage(chat_id, Mensaje());
                        pass4=1;
                        opc=1;
                        }
                    //Agrega amigo de bender
                    if (text=="/Add"){
                        if(chats[4]==0){
                            pass=1;
                            bot.sendMessage(chat_id, "Ingrese contraseña :/");
                        }
                        else{
                            bot.sendMessage(chat_id, "Tengo demasiados amigos, elimine uno :(");
                        }
                        opc=1;
                        }

                    //Elimina amigo de bender
                    if(text=="/Del"){
                        opc=1;
                        pass2=1 ;
                        bot.sendMessage(chat_id, "Ingrese contraseña :/");
                    }
                        
                    if (text=="/State"){
                        stateSystem(chat_id);
                        opc=1;
                    }
                        //Enciende el rele1
                    if(text=="/Rele1ON"){
                        bot.sendMessage(chat_id, "Rele 1 Activado", "");
                        digitalWrite(RELE1, LOW);
                        opc=1;
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE1 ON");
                    }

                    //Apaga el rele1
                    if(text=="/Rele1OFF"){
                        bot.sendMessage(chat_id, "Rele 1 Apagado", "");
                        digitalWrite(RELE1, HIGH);
                        opc=1;
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE1 OFF");
                    }
                    //Enciende el rele2
                    if(text=="/Rele2ON"){
                        bot.sendMessage(chat_id, "Rele 2 Activado", "");
                        digitalWrite(RELE2, LOW);
                        opc=1;
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE2 ON");
                    }
                    //Apaga el rele2
                    if(text=="/Rele2OFF"){
                        bot.sendMessage(chat_id, "Rele 2 Apagado", "");
                        digitalWrite(RELE2, HIGH);
                        opc=1;
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE2 OFF");
                    }
                    //Enciende el rele3
                    if(text=="/Rele3ON"){
                        bot.sendMessage(chat_id, "Rele 3 Activado", "");
                        digitalWrite(RELE3, LOW);
                        opc=1;
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE3 ON");
                    }
                    //Apaga el rele3
                    if(text=="/Rele3OFF"){
                        bot.sendMessage(chat_id, "Rele 3 Apagado", "");
                        digitalWrite(RELE3, HIGH);
                        opc=1;
                        
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE3 OFF");
                    }
                    //Enciende el rele4
                    if(text=="/Rele4ON"){
                        bot.sendMessage(chat_id, "Rele 4 Activado", "");
                        digitalWrite(RELE4, LOW);
                        opc=1;
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE4 ON");
                    }

                    //Apaga el rele3
                    if(text=="/Rele4OFF"){
                        bot.sendMessage(chat_id, "Rele 4 Apagado", "");
                        digitalWrite(RELE4, HIGH);
                        opc=1;
                        lcd.setCursor(0, 1);
                        lcd.print("                 ");
                        lcd.setCursor(0, 1);
                        lcd.print("RELE4 OFF");
                    }
                    
                    if(opc==0){
                    bot.sendMessage(chat_id, "Opcion Incorrecta! :(", "");
                    }
                    }else{ 
                        bot.sendMessage(chat_id, "Perdon pero no hablo con extraños", "");
                    }
                } 
            }
        }
}
// Funcion encargada de testear los sensores
void sensores(){
    //Primer Sensor
    if(sens>=umbral and flag5==0){
        for(int i=0;i<5;i++){
            if(chats[i]!=0){
                bot.sendMessage(String(chats[i]), "Sensor 1 Superó Umbral");
            }
        }
    flag5=1;
    }
    if(sens<umbral){
        flag5=0;
    } 
   //Segundo Sensor
    if(sens2>=umbral2 and flag6==0){
        for(int i=0;i<5;i++){
            if(chats[i]!=0){
                bot.sendMessage(String(chats[i]), "Sensor 2 Superó Umbral");
            }
        }
    flag6=1;
    }
    if(sens2<umbral2){
    flag6=0;
    } 

    //Tercer Sensor
    if(sens3>=umbral3 and flag7==0){
        for(int i=0;i<5;i++){
            if(chats[i]!=0){
                bot.sendMessage(String(chats[i]), "Sensor 3 Superó Umbral");
            }
        }
    flag7=1;
    }
    if(sens3<umbral3){
        flag7=0;
    } 
   //Cuarto Sensor
    if(sens4>=umbral4 and flag8==0){
        for(int i=0;i<5;i++){
            if(chats[i]!=0){
                bot.sendMessage(String(chats[i]), "Sensor 4 Superó Umbral");
            }
        }
    flag8=1;
    }
    if(sens4<umbral4){
         flag8=0;
    } 
}
// Funcion encargada de testear las entradas digitales
void digitales(){  
    if (digitalRead(DISCRE1) and flag==0){
        for(int i=0;i<5;i++){
            if(chats[i]!=0){
                bot.sendMessage(String(chats[i]), "Entrada Digital 1 ON");
            }
        }   
        lcd.setCursor(0, 1);
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("DIGITAL 1 ON");
        flag=1;
        }

    if (!digitalRead(DISCRE1) and flag==1){
        flag=0;
        for(int i=0;i<5;i++){
            if(chats[i]!=0){
                bot.sendMessage(String(chats[i]), "Entrada Digital 1 OFF");
            }
        }
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("DIGITAL 1 OFF");
            }

        if (digitalRead(DISCRE2) and flag2==0){
            for(int i=0;i<5;i++){
                if(chats[i]!=0){
                    bot.sendMessage(String(chats[i]), "Entrada Digital 2 ON");
                }
            }
            lcd.setCursor(0, 1);
            lcd.print("                 ");
            lcd.setCursor(0, 1);
            lcd.print("DIGITAL 2 ON");
            flag2=1;
        }
        if (!digitalRead(DISCRE2) and flag2==1){
            for(int i=0;i<5;i++){
                if(chats[i]!=0){
                    bot.sendMessage(String(chats[i]), "Entrada Digital 2 OFF");
                }
            }
        flag2=0;
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("DIGITAL 2 OFF");
    
        }

        if (digitalRead(DISCRE3) and flag3==0){
            for(int i=0;i<5;i++){
                if(chats[i]!=0){
                    bot.sendMessage(String(chats[i]), "Entrada Digital 3 ON");
                }
            }
            lcd.setCursor(0, 1);
            lcd.print("                 ");
            lcd.setCursor(0, 1);
            lcd.print("DIGITAL 3 ON");
            flag3=1;
        }
        if (!digitalRead(DISCRE3) and flag3==1){
            for(int i=0;i<5;i++){
                if(chats[i]!=0){
                    bot.sendMessage(String(chats[i]), "Entrada Digital 3 OFF");
                }
            }
        flag3=0;
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("DIGITAL 3 OFF");
    
        }

        if (digitalRead(DISCRE4) and flag4==0){
            for(int i=0;i<5;i++){
                if(chats[i]!=0){
                    bot.sendMessage(String(chats[i]), "Entrada Digital 4 ON");
                }
            }
        lcd.setCursor(0, 1);
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("DIGITAL 4 ON");
        flag4=1;
        }   
        if (!digitalRead(DISCRE4) and flag4==1){
            for(int i=0;i<5;i++){
                if(chats[i]!=0){
                    bot.sendMessage(String(chats[i]), "Entrada Digital 4 OFF");
                }
            }
        flag4=0;
        lcd.print("                 ");
        lcd.setCursor(0, 1);
        lcd.print("DIGITAL 4 OFF");
    }
}
// Mensajes con las funciones de bender
String mensajeStart(String welcome){
      welcome += "----------------------------------\n";
      welcome += "/Add Agregar Amigo.\n";
      welcome += "----------------------------------\n";
      welcome += "/Del Eliminar Amigo.\n";
      welcome += "----------------------------------\n";
      welcome += "/Rele1ON Para Activar Rele 1. \n";
      welcome += "----------------------------------\n";
      welcome += "/Rele1OFF Para Desactivar Rele 1. \n";
      welcome += "----------------------------------\n";
      welcome += "/Rele2ON Para Activar Rele 2. \n";
      welcome += "----------------------------------\n";
      welcome += "/Rele2OFF Para Desactivar Rele 2.\n"; 
      welcome += "----------------------------------\n";
      welcome += "/Rele3ON Para Activar Rele 3. \n";
      welcome += "----------------------------------\n";
      welcome += "/Rele3OFF Para Desactivar Rele 3.\n"; 
      welcome += "----------------------------------\n";
      welcome += "/Rele4ON Para Activar Rele 4. \n";
      welcome += "----------------------------------\n";
      welcome += "/Rele4OFF Para Desactivar Rele 4.\n"; 
      welcome += "----------------------------------\n";
      welcome += "/Set Configura Entradas Analógicas.\n"; 
      welcome += "----------------------------------\n";
      welcome += "/State Para ver el estado de E/S. \n";
      return welcome;
}

void setup() {
    Serial.begin(9600);
// Inicio el LCD
    lcd.init();
// Enciendo blacklight                      
    lcd.backlight();
//Inicializo cada pin
    pinMode(RELE1, OUTPUT);
    pinMode(RELE2, OUTPUT);
    pinMode(RELE3, OUTPUT);
    pinMode(RELE4, OUTPUT);
    pinMode(DISCRE1, INPUT);
    pinMode(DISCRE2, INPUT);
    pinMode(DISCRE3, INPUT);
    pinMode(DISCRE4, INPUT);
    digitalWrite(RELE1, HIGH);
    digitalWrite(RELE2, HIGH);
    digitalWrite(RELE3, HIGH);
    digitalWrite(RELE4, HIGH);
    pinMode(ADC1, INPUT);
    pinMode(ADC2, INPUT);
    pinMode(ADC3, INPUT);
    pinMode(ADC4, INPUT);


    lcd.clear();
    lcd.setCursor(5, 0);
    // print message
    lcd.print("BENDER");
    lcd.setCursor(0, 1);
    lcd.print("CONECTANDO WIFI");
    //Trata de conectarse a la ultima red Wi-Fi
    WiFiManager wifiManager;
    wifiManager.autoConnect("Bender");
    
    Wifiestatus();
    #ifdef ESP32
        client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
    #endif
    
}
void loop() {
    lcd.setCursor(5, 0);
    // print message
    lcd.print("BENDER");

    sens=(((maxi-mini)/4095)*(analogRead(ADC1)))+mini;
    sens2=(((maxi2-mini2)/4095)*(analogRead(ADC2)))+mini2;
    sens3=(((maxi3-mini3)/4095)*(analogRead(ADC3)))+mini3;
    sens4=(((maxi4-mini4)/4095)*(analogRead(ADC4)))+mini4;

    Serial.println((analogRead(ADC1)));
    sensores();
    digitales(); 
    lcd.setCursor(0, 1);
    lcd.print("                 ");
    lcd.setCursor(0, 1);
    lcd.print("WIFI CONECTADO");
    delay(500); 

    if (millis() > lastTimeBotRan + botRequestDelay)  {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        while(numNewMessages) {
        Serial.println("got response");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}