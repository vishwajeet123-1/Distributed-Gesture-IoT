#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// --- CONFIGURATION ---
const char* ssid = "DUBE BROADBAND 9423351431";
const char* password = "78877532";

// The 5 Pins for your 5 LEDs
const int ledPins[] = {13, 12, 14, 27, 26}; 

AsyncWebServer server(80);

// The HTML code is stored in the next section
extern const char index_html[]; 

void setup() {
    Serial.begin(115200);

    // Initialize all 5 pins as OUTPUT
    for(int i = 0; i < 5; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nCONNECTED!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Route for the Web Page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    // Route to receive the finger count (0 to 5)
    server.on("/count", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("value")) {
            int count = request->getParam("value")->value().toInt();
            Serial.printf("Fingers detected: %d\n", count);
            
            // Logic: Turn ON 'count' number of LEDs, turn others OFF
            for(int i = 0; i < 5; i++) {
                if (i < count) {
                    digitalWrite(ledPins[i], HIGH);
                } else {
                    digitalWrite(ledPins[i], LOW);
                }
            }
        }
        request->send(200, "text/plain", "OK");
    });

    server.begin();
}

void loop() {}

// --- WEB PAGE SECTION ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>AI Finger Counter</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { margin: 0; background: #111; color: white; text-align: center; font-family: sans-serif; overflow: hidden; }
        #vid { position: absolute; top: 10px; right: 10px; width: 140px; border: 2px solid #00ffaa; transform: scaleX(-1); z-index: 10; }
        #status-container { margin-top: 20px; padding: 20px; }
        #count-display { font-size: 80px; color: #00ffaa; font-weight: bold; margin: 10px; }
        #btn { padding: 20px 40px; font-size: 20px; background: #00ffaa; border: none; border-radius: 10px; cursor: pointer; margin-top: 50px; }
        .dot-container { display: flex; justify-content: center; gap: 15px; margin-top: 20px; }
        .dot { width: 30px; height: 30px; border-radius: 50%; border: 2px solid #555; background: #222; }
        .active { background: #00ffaa; box-shadow: 0 0 15px #00ffaa; border-color: white; }
    </style>
</head>
<body>
    <div id="status-container">
        <div id="status">Ready to Start</div>
        <div id="count-display">0</div>
        <div class="dot-container" id="dots">
            <div class="dot"></div><div class="dot"></div><div class="dot"></div><div class="dot"></div><div class="dot"></div>
        </div>
    </div>

    <button id="btn">START AI CAMERA</button>
    <video id="vid" autoplay playsinline muted></video>

    <script src="https://cdn.jsdelivr.net/npm/@mediapipe/hands/hands.js" crossorigin="anonymous"></script>

    <script>
        const btn = document.getElementById('btn');
        const vid = document.getElementById('vid');
        const stat = document.getElementById('status');
        const countDisplay = document.getElementById('count-display');
        const dots = document.querySelectorAll('.dot');
        let lastCount = -1;

        btn.onclick = async () => {
            btn.style.display = "none";
            stat.innerText = "Loading AI...";
            
            try {
                const stream = await navigator.mediaDevices.getUserMedia({ video: true });
                vid.srcObject = stream;

                const hands = new Hands({
                    locateFile: (file) => `https://cdn.jsdelivr.net/npm/@mediapipe/hands/${file}`
                });

                hands.setOptions({ maxNumHands: 1, modelComplexity: 1, minDetectionConfidence: 0.6 });

                hands.onResults((results) => {
                    stat.innerText = "Show your hand to the camera";
                    let count = 0;

                    if (results.multiHandLandmarks && results.multiHandLandmarks.length > 0) {
                        const lm = results.multiHandLandmarks[0];

                        // Finger tracking (Landmarks: 8=Index, 12=Middle, 16=Ring, 20=Pinky)
                        const tips = [8, 12, 16, 20];
                        const joints = [6, 10, 14, 18];
                        
                        for (let i = 0; i < 4; i++) {
                            if (lm[tips[i]].y < lm[joints[i]].y) count++;
                        }

                        // Thumb Logic (Sideways movement check)
                        // Uses X-axis distance between thumb tip(4) and index base(5)
                        const thumbExtended = Math.abs(lm[4].x - lm[5].x) > 0.1;
                        if (thumbExtended) count++;

                        // UI Updates
                        countDisplay.innerText = count;
                        dots.forEach((dot, i) => {
                            if (i < count) dot.classList.add('active');
                            else dot.classList.remove('active');
                        });

                        // Network communication
                        if (count !== lastCount) {
                            fetch(`/count?value=${count}`).catch(err => console.log("ESP32 Offline"));
                            lastCount = count;
                        }
                    }
                });

                async function loop() {
                    if (vid.readyState >= 2) await hands.send({ image: vid });
                    requestAnimationFrame(loop);
                }
                loop();

            } catch (err) {
                stat.innerText = "Camera Error: " + err.message;
            }
        };
    </script>
</body>
</html>
)rawliteral";