// Config RTOS
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// PAGINA QUE SERVIRA EL ESP32 en pagina principal
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="mul">
	<head>
		<meta
			name="viewport"
			content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no"
		/>

		<title>joystick</title>
		<style>
			body {
				display: flex;
				justify-content: center;
				align-items: center;
				flex-direction: column;
				background-color: #333;
				min-height: 75vh;
				color: white;
				touch-action: pan-x pan-y;
			}
			.j__container {
				display: flex;
				flex-wrap: wrap;
				justify-content: center;
				width: 100%;
				-webkit-touch-callout: none;
				-webkit-user-select: none;
				-khtml-user-select: none;
				-moz-user-select: none;
				-ms-user-select: none;
				user-select: none;
			}

			.j__btnA {
				background-color: green;
			}
			.j__btnB {
				background-color: red;
			}
			.j__btnC {
				background-color: gold;
			}
			.j__btnD {
				background-color: blue;
			}
			.j__btnA,
			.j__btnB,
			.j__btnC,
			.j__btnD {
				display: flex;
				justify-content: center;
				align-items: center;
				font-family: sans-serif;
				font-weight: bold;
				color: white;
				border-radius: 56px;
				width: auto;
				min-width: 56px;
				height: 56px;
				box-shadow: 0 0 0.5em black;
				font-size: 1.5em;
				margin: 1em;
			}
			.info {
				color: white;
				font-size: 2em;
				font-weight: bold;
			}
			.infoIps {
				color: white;
				font-size: 1em;
				font-weight: bold;
				margin-bottom: 1.5em;
				font-style: italic;
			}
			.infoIps span {
				color: green;
			}
		</style>
	</head>
	<body>
		<div class="infoIps">
			<div>
				IP_AP:
				<span>192.168.4.1</span>
			</div>
			<div>
				IP_STA:
				<span id="ipSta">0.0.0.0</span>
			</div>
			<hr />
			<div>
				Humedad Ambiente:&nbsp;
				<span id="humedad">...</span>
			</div>
			<br />
			<div>
				Temperatura Ambiente:&nbsp;
				<span id="temperatura">...</span>
			</div>
			<br />
			<div>
				Humedad Tierra:&nbsp;
				<span id="humedadTierra">...</span>
			</div>
			<br />
			<div>
				Luz:&nbsp;
				<span id="luz">...</span>
			</div>
			<hr />
		</div>
		<hr />

		<small>(A) Regar</small>
		<small>(B) Pronto!</small>
		<small>(C) Cerrar Ventana</small>
		<small>(D) Abrir Ventana</small>
		<hr />
		<br />
		<div class="j__container" draggable="false">
			<div id="btnA" class="j__btnA">A</div>
			<div id="btnB" class="j__btnB">B</div>
			<div id="btnC" class="j__btnC">C</div>
			<div id="btnD" class="j__btnD">D</div>
		</div>
		<script>
			let ipSta = '0.0.0.0';
			let infoIpSta = document.getElementById('ipSta');
			let infoHumedad = document.getElementById('humedad');
			let infoTemperatura = document.getElementById('temperatura');
			let infoHumedadTierra = document.getElementById('humedadTierra');
			let infoLuz = document.getElementById('luz');

			let gateway = `ws://${window.location.hostname}/ws`;
			let websocket = new WebSocket(gateway);
			websocket.onmessage = function (event) {
				console.log(event.data);
				if (event.data.includes('STA_IP')) {
					ipSta = event.data.split(';')[1].split('STA_IP:')[1];
					infoIpSta.innerHTML = ipSta;
					if (ipSta === '0.0.0.0') {
						infoIpSta.style.color = 'red';
					} else {
						infoIpSta.style.color = 'green';
					}
				} else {
					let dataSensors = event.data.split(';');
					let humAmbiente = dataSensors[0];
					let tempAmbiente = dataSensors[1];
					let humTierra = dataSensors[2];
					let luz = dataSensors[3];
					infoHumedad.innerHTML = humAmbiente;
					infoTemperatura.innerHTML = tempAmbiente + '°C';
					infoHumedadTierra.innerHTML = humTierra;
					infoLuz.innerHTML = luz;
				}
			};
			function sendMsg(msg) {
				if (!websocket.readyState) {
					return;
				}
				websocket.send(msg);
			}

			class button {
				constructor(id) {
					this.element = document.getElementById(id);
					this.active = false;
					const click = (e) => {
						this.active = true;
						e.preventDefault();
					};
					const up = (e) => {
						this.active = false;
					};
					document.addEventListener('mouseup', up);
					this.element.addEventListener('touchend', up);
					this.element.addEventListener('mousedown', click);
					this.element.addEventListener('touchstart', click);
				}
			}

			let btnA = new button('btnA');
			let btnB = new button('btnB');
			let btnC = new button('btnC');
			let btnD = new button('btnD');
			let lastBtnA = false;
			let lastBtnB = false;
			let lastBtnC = false;
			let lastBtnD = false;
			const loop = () => {
				//button and stick behavior control
				if (lastBtnA !== btnA.active) {
					lastBtnA = btnA.active;
					if (btnA.active) {
						sendMsg('A+');
						btnA.element.style = 'transform: scale(0.9);';
						//vibrate for movile devices if button is pressed
						window.navigator.vibrate(60);
					} else {
						// sendMsg('A-');
						btnA.element.style = 'transform: scale(1);';
					}
				}
				if (lastBtnB !== btnB.active) {
					lastBtnB = btnB.active;
					if (btnB.active) {
						sendMsg('B+');
						btnB.element.style = 'transform: scale(0.9);';
						window.navigator.vibrate(60);
					} else {
						// sendMsg('B-');
						btnB.element.style = 'transform: scale(1);';
					}
				}
				if (lastBtnC !== btnC.active) {
					lastBtnC = btnC.active;
					if (btnC.active) {
						sendMsg('C+');
						btnC.element.style = 'transform: scale(0.9);';
						window.navigator.vibrate(60);
					} else {
						// sendMsg('C-');
						btnC.element.style = 'transform: scale(1);';
					}
				}

				if (lastBtnD !== btnD.active) {
					lastBtnD = btnD.active;
					if (btnD.active) {
						sendMsg('D+');
						btnD.element.style = 'transform: scale(0.9);';
						window.navigator.vibrate(60);
					} else {
						// sendMsg('D-');
						btnD.element.style = 'transform: scale(1);';
					}
				}

				requestAnimationFrame(loop);
			};
			loop();
			//Evitar escalar pantalla
			document.addEventListener(
				'touchmove',
				function (event) {
					event = event.originalEvent || event;
					if (event.scale > 1) {
						event.preventDefault();
					}
				},
				false
			);
		</script>
	</body>
</html>
)rawliteral";

/***Librerias***/
// https://github.com/me-no-dev/AsyncTCP
#include <AsyncTCP.h>
// https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h>
// https://github.com/RoboticsBrno/ServoESP32
#include <Servo.h>
// esta se puede descargar desde la biblioteca de arduino
#include <DHT.h>
// buscarla en la biblioteca de arduino como EspSoftwareSerial
#include <SoftwareSerial.h>

/***DATOS PARA CONEXION DE WIFI***/

// DATOS RED ESP32 (AP)
#define SSIDAP "ESP01-01"
#define PASSAP "321321321"
// DATOS DE TU RED (STA)
#define SSID "UCN-V3"
#define PASS "a0b1c2d3e4f5"

/***Definiciones y declaraciones***/
#define SERVOPIN 5
#define AGUAPIN 16
#define HUMPIN 32
#define DHTPIN 33
#define LDRPIN 34

#define DHTTYPE DHT11

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial OpenLCD(0, 17); // RX, TX

volatile boolean btnA = false;
volatile boolean btnB = false;  
volatile boolean btnC = false;
volatile boolean btnD = false;

/***Datos Sensores procesados***/
float humAmbiente = 0;
float tempAmbiente = 0;
float humTierra = 0;
int luz = 0;

/****Utils para lcd uart****/
void setPositionLCD(byte x = 0, byte y = 0)
{
	OpenLCD.write(254);
	OpenLCD.write(128 + y * 64 + x);
}
void setContrastLCD(byte _contrast = 2)
{
	if (_contrast > 5)
	{
		_contrast = 5;
	}
	else if (_contrast < 0)
	{
		_contrast = 0;
	}
	OpenLCD.write('|'); // Put LCD into setting mode
	OpenLCD.write(24);	// Send contrast command
	OpenLCD.write(_contrast);
	delay(100);
}

void setBackLightLCD(byte _percent = 100)
{
	OpenLCD.write('|'); // Put LCD into setting mode
	OpenLCD.write(158 + round((_percent * 100) / 29));
	delay(100);
}

void clearLCD()
{
	setPositionLCD(0, 0);
	OpenLCD.print("                ");
	setPositionLCD(0, 1);
	OpenLCD.print("                ");
}
void printLCD(String msg, byte x = 0, byte y = 0)
{
	setPositionLCD(x, y);
	OpenLCD.print(msg);
}

/*****Logica de bomba de agua***/
void darAgua(int cantidad = 150, int tiempoMs = 1000)
{
	Serial.println("Dando Agua");
	if (cantidad > 255)
	{
		cantidad = 255;
	}
	else if (cantidad < 0)
	{
		cantidad = 0;
	}
	pinMode(AGUAPIN, OUTPUT);
	digitalWrite(AGUAPIN, true);
	delay(tiempoMs);
	digitalWrite(AGUAPIN, false);
}

/****control servo ventana***/
void controlVentana(bool isOpen = false, int _delay = 30)
{
	Serial.println("Ventana");
	if (isOpen)
	{
		myservo.attach(SERVOPIN);
		myservo.write(110);
		delay(2000);
		myservo.detach();
	}
	else
	{
		myservo.attach(SERVOPIN);
		myservo.write(0);
		delay(2000);
		myservo.detach();
	}
}
/******Metodos para webhook*****/
// pasa un tipo ip a String
String ip2Str(IPAddress ip)
{
	String s = "";
	for (int i = 0; i < 4; i++)
	{
		s += i ? "." + String(ip[i]) : String(ip[i]);
	}
	return s;
}
// Notifica a todos los clientes conectados a la pagina
void notifyClients(String msg)
{
	ws.textAll(msg);
}
// Controlador de mensages recibidos por los dispositivos conectados a la pagina
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
	AwsFrameInfo *info = (AwsFrameInfo *)arg;
	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
	{
		data[len] = 0;
		if (data[0] == 'A')
		{

			if (btnA == false)
			{
				btnA = true;
				darAgua();
				btnA = false;
			}
		}
		if (data[0] == 'B')
		{

			Serial.println("B: " + String(btnB));
		}
		if (data[0] == 'C')
		{

			controlVentana(true);
		}
		if (data[0] == 'D')
		{

			controlVentana(false);
		}
	}
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
			 void *arg, uint8_t *data, size_t len)
{
	switch (type)
	{
	case WS_EVT_CONNECT:
		// Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
		notifyClients("AP_IP:" + ip2Str(WiFi.softAPIP()) + ";STA_IP:" + ip2Str(WiFi.localIP()));
		break;
	case WS_EVT_DISCONNECT:
		// Serial.printf("WebSocket client #%u disconnected\n", client->id());
		break;
	case WS_EVT_DATA:
		handleWebSocketMessage(arg, data, len);
		break;
	case WS_EVT_PONG:
	case WS_EVT_ERROR:
		break;
	}
}

void initWebSocket()
{
	ws.onEvent(onEvent);
	server.addHandler(&ws);
}

String processor(const String &var)
{
	// Serial.println(var);
	return String();
}

/****RTOS UTILS****/
// Permite facilitar la creacion de una tarea en el RTOS
void NewTask(String name, void (*callback)(void *pvParameters), int priority = 2, int stackSize = 2048)
{
	xTaskCreatePinnedToCore(callback, name.c_str(), stackSize, NULL, priority, NULL, ARDUINO_RUNNING_CORE);
}
//"Delay" no detiene el tiempo de ejecucion del esp a diferencia de delay (con "d" minuscula)
void Delay(int msTime = 1000)
{
	vTaskDelay(msTime / portTICK_PERIOD_MS);
}

/*****TASKS******/

void Sensores(void *pvParameters)
{
	while (1)
	{
		humAmbiente = dht.readHumidity();
		tempAmbiente = dht.readTemperature();
		humTierra = constrain(map(analogRead(HUMPIN), 0, 2500, 0, 100), 0, 100);
		luz = constrain(map(analogRead(LDRPIN), 3050, 4095, 0, 100), 0, 100);
		Serial.println("H.A:" + String(humAmbiente));
		Serial.println("T.A:" + String(tempAmbiente));
		Serial.println("Señales analógicas");
		Serial.println("H.T:" + String(analogRead(HUMPIN)));
		Serial.println("Luz:" + String(analogRead(LDRPIN)));
		Delay(1000);
		notifyClients(String(humAmbiente) + ";" + String(tempAmbiente) + ";" + String(humTierra) + ";" + String(luz));
		Delay(3000);
	}
}

void webSocketTask(void *pvParameters)
{
	while (1)
	{
		ws.cleanupClients();
		Delay(100);
	}
}

void setup()
{
	Serial.begin(115200);
	OpenLCD.begin(9600); // Start communication with OpenLCD
	setContrastLCD();
	setBackLightLCD();
	clearLCD();
	delay(1000);
	printLCD("Estableciendo");
	printLCD("Conexiones...", 0, 1);
	pinMode(HUMPIN, INPUT);
	pinMode(LDRPIN, INPUT);
	pinMode(AGUAPIN, OUTPUT);
	pinMode(SERVOPIN, OUTPUT);

	dht.begin();
	myservo.attach(SERVOPIN);
	/*Logica Wifi*/
	WiFi.mode(WIFI_AP_STA);
	// AP
	WiFi.softAP(SSIDAP, PASSAP);
	while (!WiFi.softAP(SSIDAP, PASSAP))
		;
	{
		delay(100);
	}
	// STA
	bool conectado = true;
	WiFi.begin(SSID, PASS);
	byte intentos = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		intentos++;
		delay(1000);
		if (intentos > 5)
		{
			Serial.print("No se pudo conectar a wifi: ");
			Serial.println(SSID);
			conectado = false;
			break;
		}
	}
	clearLCD();
	if (conectado)
	{
		Serial.println("Conectado a wifi: ");
		Serial.println(SSID);
		Serial.println(WiFi.localIP());
		printLCD("SA" + ip2Str(WiFi.localIP()));
	}
	else
	{
		printLCD("STA(SA):N/A");
	}
	printLCD("AP" + ip2Str(WiFi.softAPIP()), 0, 1);
	delay(4000);

	initWebSocket();
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send_P(200, "text/html", index_html, processor); });
	server.begin();
	NewTask("sensor", &Sensores);
	NewTask("webSocket", &webSocketTask);
}
void loop()
{
}
