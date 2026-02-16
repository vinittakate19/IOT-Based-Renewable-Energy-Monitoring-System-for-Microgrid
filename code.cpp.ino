#include <WiFi.h>
#include <WebServer.h>

// Pin definitions
#define PANEL_PIN    34
#define BATTERY_PIN  35
#define CURRENT_PIN  32

// WiFi credentials
const char* ssid = "Hb";
const char* password = "12121212";

// Create WebServer on port 80
WebServer server(80);

// ---- Calibration (from your measurements) ----
const float VOLT_FACTOR = 8.0 / 1872.0;   
const int CURRENT_ZERO = 2935;
const float AMP_FACTOR = 0.5 / (2935 - 2800);

// Monitoring data structure
struct SolarData {
  float panelVoltage;
  float batteryVoltage;
  float current;
  float loadVoltage;    // Assuming same as battery voltage for load
  float solarPower;     // Panel voltage * current
  float loadPower;      // Load voltage * current
  float totalEnergy;    // Total energy consumption in Wh
  int rawPanel;
  int rawBatt;
  int rawCurrent;
};

SolarData solarData;

// Energy calculation variables
unsigned long lastUpdateTime = 0;
float totalEnergyWh = 0.0;  // Total energy in Watt-hours

void setup() {
  Serial.begin(115200);

  // Configure ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup server routes
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/style.css", handleCSS);
  server.on("/reset", handleReset);  // Add reset endpoint

  server.begin();
  Serial.println("HTTP server started");
  
  lastUpdateTime = millis();
}

void loop() {
  server.handleClient();
  updateSolarData();
  delay(1000); // Update data every second
}

void updateSolarData() {
  // Read raw ADC values
  solarData.rawPanel = analogRead(PANEL_PIN);
  solarData.rawBatt = analogRead(BATTERY_PIN);
  solarData.rawCurrent = analogRead(CURRENT_PIN);

  // Calculate voltages
  solarData.panelVoltage = solarData.rawPanel * VOLT_FACTOR;
  solarData.batteryVoltage = solarData.rawBatt * VOLT_FACTOR;
  solarData.loadVoltage = solarData.batteryVoltage; // Assuming load uses battery voltage

  // Calculate current
  int diff = CURRENT_ZERO - solarData.rawCurrent;
  solarData.current = diff * AMP_FACTOR;
  if (solarData.current < 0) solarData.current = 0;

  // Calculate power
  solarData.solarPower = solarData.panelVoltage * solarData.current;
  solarData.loadPower = solarData.loadVoltage * solarData.current;

  // Calculate energy consumption (in Watt-hours)
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastUpdateTime;
  float elapsedHours = elapsedTime / 3600000.0;  // Convert ms to hours
  
  // Add to total energy (using load power)
  totalEnergyWh += solarData.loadPower * elapsedHours;
  solarData.totalEnergy = totalEnergyWh;
  
  lastUpdateTime = currentTime;
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Solar Monitor</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="/style.css">
    <script>
        function updateData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('panelVoltage').textContent = data.panelVoltage.toFixed(2);
                    document.getElementById('batteryVoltage').textContent = data.batteryVoltage.toFixed(2);
                    document.getElementById('current').textContent = data.current.toFixed(3);
                    document.getElementById('loadVoltage').textContent = data.loadVoltage.toFixed(2);
                    document.getElementById('solarPower').textContent = data.solarPower.toFixed(2);
                    document.getElementById('loadPower').textContent = data.loadPower.toFixed(2);
                    document.getElementById('totalEnergy').textContent = data.totalEnergy.toFixed(3);
                    document.getElementById('totalEnergyKwh').textContent = (data.totalEnergy / 1000).toFixed(4);
                    document.getElementById('rawPanel').textContent = data.rawPanel;
                    document.getElementById('rawBatt').textContent = data.rawBatt;
                    document.getElementById('rawCurrent').textContent = data.rawCurrent;
                    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
                })
                .catch(error => console.error('Error:', error));
        }
        
        function resetEnergy() {
            if(confirm('Are you sure you want to reset the total energy consumption?')) {
                fetch('/reset')
                    .then(response => response.json())
                    .then(data => {
                        if(data.success) {
                            updateData();
                            alert('Energy counter reset successfully!');
                        }
                    })
                    .catch(error => console.error('Error:', error));
            }
        }
        
        // Update every 2 seconds
        setInterval(updateData, 2000);
        window.onload = updateData;
    </script>
</head>
<body>
    <div class="container">
        <h1>🌞 Solar Monitoring System</h1>
        
        <div class="grid-container">
            <div class="card solar">
                <h2>Solar Input</h2>
                <div class="value" id="panelVoltage">--</div>
                <div class="unit">Volts</div>
                <div class="power">Power: <span id="solarPower">--</span> W</div>
            </div>
            
            <div class="card battery">
                <h2>Battery</h2>
                <div class="value" id="batteryVoltage">--</div>
                <div class="unit">Volts</div>
            </div>
            
            <div class="card load">
                <h2>Load Output</h2>
                <div class="value" id="loadVoltage">--</div>
                <div class="unit">Volts</div>
                <div class="power">Power: <span id="loadPower">--</span> W</div>
            </div>
            
            <div class="card current">
                <h2>Current</h2>
                <div class="value" id="current">--</div>
                <div class="unit">Amps</div>
            </div>
        </div>

        <div class="card energy">
            <h2>Total Energy Consumption</h2>
            <div class="value" id="totalEnergy">--</div>
            <div class="unit">Watt-hours (Wh)</div>
            <div class="kwh">Kilowatt-hours: <span id="totalEnergyKwh">--</span> kWh</div>
            <button class="reset-btn" onclick="resetEnergy()">Reset Counter</button>
        </div>
        
        <div class="raw-data">
            <h3>Raw ADC Values</h3>
            <div class="raw-grid">
                <div>Panel: <span id="rawPanel">--</span></div>
                <div>Battery: <span id="rawBatt">--</span></div>
                <div>Current: <span id="rawCurrent">--</span></div>
            </div>
        </div>
        
        <div class="footer">
            Last updated: <span id="lastUpdate">--</span>
        </div>
    </div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"panelVoltage\":" + String(solarData.panelVoltage, 2) + ",";
  json += "\"batteryVoltage\":" + String(solarData.batteryVoltage, 2) + ",";
  json += "\"current\":" + String(solarData.current, 3) + ",";
  json += "\"loadVoltage\":" + String(solarData.loadVoltage, 2) + ",";
  json += "\"solarPower\":" + String(solarData.solarPower, 2) + ",";
  json += "\"loadPower\":" + String(solarData.loadPower, 2) + ",";
  json += "\"totalEnergy\":" + String(solarData.totalEnergy, 3) + ",";
  json += "\"rawPanel\":" + String(solarData.rawPanel) + ",";
  json += "\"rawBatt\":" + String(solarData.rawBatt) + ",";
  json += "\"rawCurrent\":" + String(solarData.rawCurrent);
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleReset() {
  totalEnergyWh = 0.0;
  solarData.totalEnergy = 0.0;
  
  String json = "{\"success\":true,\"message\":\"Energy counter reset\"}";
  server.send(200, "application/json", json);
}

void handleCSS() {
  String css = R"rawliteral(
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 20px;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    min-height: 100vh;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    background: white;
    border-radius: 15px;
    padding: 30px;
    box-shadow: 0 10px 30px rgba(0,0,0,0.2);
}

h1 {
    text-align: center;
    color: #333;
    margin-bottom: 30px;
    font-size: 2.5em;
}

.grid-container {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
    gap: 20px;
    margin-bottom: 30px;
}

.card {
    background: white;
    border-radius: 10px;
    padding: 20px;
    text-align: center;
    box-shadow: 0 4px 15px rgba(0,0,0,0.1);
    border-left: 5px solid;
    transition: transform 0.3s ease;
}

.card:hover {
    transform: translateY(-5px);
}

.card.solar { border-left-color: #ff6b35; }
.card.battery { border-left-color: #4ecdc4; }
.card.load { border-left-color: #45b7d1; }
.card.current { border-left-color: #96ceb4; }
.card.energy { border-left-color: #f39c12; }

.card h2 {
    margin: 0 0 15px 0;
    color: #333;
    font-size: 1.3em;
}

.value {
    font-size: 2.5em;
    font-weight: bold;
    color: #2c3e50;
    margin: 10px 0;
}

.unit {
    font-size: 1.1em;
    color: #7f8c8d;
    margin-bottom: 10px;
}

.power, .kwh {
    font-size: 1.1em;
    color: #e74c3c;
    font-weight: bold;
    margin-top: 10px;
}

.kwh {
    color: #27ae60;
    margin-bottom: 15px;
}

.reset-btn {
    background: #e74c3c;
    color: white;
    border: none;
    padding: 10px 20px;
    border-radius: 5px;
    cursor: pointer;
    font-size: 1em;
    margin-top: 10px;
    transition: background 0.3s ease;
}

.reset-btn:hover {
    background: #c0392b;
}

.raw-data {
    background: #f8f9fa;
    padding: 20px;
    border-radius: 10px;
    margin-bottom: 20px;
}

.raw-data h3 {
    margin: 0 0 15px 0;
    color: #333;
}

.raw-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 10px;
}

.raw-grid div {
    background: white;
    padding: 10px;
    border-radius: 5px;
    text-align: center;
    font-family: monospace;
}

.footer {
    text-align: center;
    color: #7f8c8d;
    font-style: italic;
    padding-top: 20px;
    border-top: 1px solid #ecf0f1;
}

@media (max-width: 768px) {
    body {
        padding: 10px;
    }
    
    .container {
        padding: 15px;
    }
    
    h1 {
        font-size: 2em;
    }
    
    .value {
        font-size: 2em;
    }
    
    .grid-container {
        grid-template-columns: 1fr;
    }
}
)rawliteral";

  server.send(200, "text/css", css);
}