
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// --- SETTINGS ---
// --- WIFI CREDENTIALS ---
// --- CONFIGURATION ---
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const int ledPin = 2; // Built-in LED (Change to 4 or 5 if 2 doesn't work)

AsyncWebServer server(80);

// The HTML is fixed to use the latest "Hands" initialization
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>AI Final Test</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { margin: 0; background: #111; color: white; text-align: center; font-family: sans-serif; }
        #vid { width: 160px; height: 120px; border: 2px solid #00ffaa; margin-top: 10px; transform: scaleX(-1); }
        #status { margin: 20px; padding: 10px; border-radius: 5px; background: #222; }
        #btn { padding: 20px; font-size: 18px; background: #00ffaa; border: none; border-radius: 10px; cursor: pointer; }
    </style>
</head>
<body>
    <div id="status">Status: Click button to start</div>
    <button id="btn">START SYSTEM</button><br>
    <video id="vid" autoplay playsinline muted></video>

    <script src="https://cdn.jsdelivr.net/npm/@mediapipe/hands/hands.js" crossorigin="anonymous"></script>

    <script>
        const btn = document.getElementById('btn');
        const vid = document.getElementById('vid');
        const stat = document.getElementById('status');
        let lastState = -1;

        btn.onclick = async () => {
            stat.innerText = "Step 1: Checking Camera...";
            try {
                // 1. Start Camera
                const stream = await navigator.mediaDevices.getUserMedia({ video: true });
                vid.srcObject = stream;
                stat.innerText = "Step 2: Camera OK. Loading AI Models...";

                // 2. Initialize MediaPipe
                if (typeof Hands === 'undefined') {
                    throw new Error("MediaPipe Library failed to load from Internet.");
                }

                const hands = new Hands({
                    locateFile: (file) => `https://cdn.jsdelivr.net/npm/@mediapipe/hands/${file}`
                });

                hands.setOptions({ maxNumHands: 1, modelComplexity: 1 });

                hands.onResults((results) => {
                    stat.innerText = "Step 3: AI RUNNING! Show hand.";
                    if (results.multiHandLandmarks && results.multiHandLandmarks.length > 0) {
                        const lm = results.multiHandLandmarks[0];
                        const isOpen = lm[8].y < lm[6].y;
                        stat.innerText = isOpen ? "GESTURE: OPEN" : "GESTURE: CLOSED";
                        stat.style.background = isOpen ? "green" : "red";

                        let currentState = isOpen ? 1 : 0;
                        if (currentState !== lastState) {
                            fetch(`/led?state=${currentState}`).catch(() => {});
                            lastState = currentState;
                        }
                    }
                });

                // 3. Start Processing Loop
                async function predict() {
                    if (vid.readyState >= 2) {
                        await hands.send({ image: vid });
                    }
                    requestAnimationFrame(predict);
                }
                
                vid.onloadedmetadata = () => {
                    predict();
                    btn.style.display = "none";
                };

            } catch (err) {
                stat.innerText = "CRASH: " + err.message;
                stat.style.color = "orange";
                console.error(err);
            }
        };
    </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConnected!");
    Serial.print("Go to this IP on your phone: ");
    Serial.println(WiFi.localIP());

    // Root route
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    // Combined LED route to prevent errors
    server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("state")) {
            String state = request->getParam("state")->value();
            if (state == "1") digitalWrite(ledPin, HIGH);
            else digitalWrite(ledPin, LOW);
        }
        request->send(200, "text/plain", "OK");
    });

    server.begin();
}

void loop() {}
