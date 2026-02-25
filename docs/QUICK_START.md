# Quick Start Guide

## Prerequisites

- **Firmware**: ESP32, MCP2515 CAN controller, TJA1050 transceiver, NEO-6M GPS, MicroSD card
- **Backend**: Node.js 14+, MongoDB 4.4+
- **Frontend**: Node.js 14+, npm/yarn
- **Development**: VS Code, PlatformIO extension

## Firmware Setup

### 1. Hardware Assembly

```
ESP32 PIN CONNECTIONS:

CAN Bus (MCP2515):
  - CS Pin: GPIO5
  - MOSI: GPIO23 (SPI)
  - MISO: GPIO19 (SPI)
  - CLK: GPIO18 (SPI)
  - INT: GPIO4

GPS (NEO-6M):
  - RX: GPIO16 (UART2)
  - TX: GPIO17 (UART2)

SD Card:
  - CS: GPIO2
  - MOSI: GPIO23 (SPI)
  - MISO: GPIO19 (SPI)
  - CLK: GPIO18 (SPI)

Power:
  - 12V vehicle → Buck converter
  - 5V → ESP32 VCC
  - 3.3V → MCP2515, GPS module
```

### 2. Configure Firmware

```cpp
// firmware/include/config.h
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"
#define MQTT_BROKER "your-broker-ip"
#define CAN_BITRATE 500000
```

### 3. Build and Flash

```bash
cd firmware
platformio run -e esp32doit-devkit-v1 -t upload

# Monitor serial output
platformio device monitor -e esp32doit-devkit-v1
```

### 4. Verify Output

```
[00:00.00] [INF] LOGGER: Logger initialized
[00:01.50] [INF] MCP2515: CAN controller initialized successfully
[00:02.00] [INF] GPS: GPS module initialized at 9600 baud
[00:02.50] [INF] SDLOG: SD card initialized
[00:03.00] [INF] MQTT: Connecting to WiFi: your-ssid
[00:05.00] [INF] MQTT: WiFi connected. IP: 192.168.1.100
[00:05.50] [INF] MQTT: Connected to MQTT broker
```

## Backend Setup

### 1. Install Dependencies

```bash
cd backend
npm install
```

### 2. Configure Environment

```bash
cp .env.example .env
```

Edit `.env`:
```
MONGODB_URI=mongodb://localhost:27017/can-logger
JWT_SECRET=your-super-secret-key
PORT=3000
MQTT_BROKER=mqtt://localhost:1883
```

### 3. Start MongoDB

```bash
# Using Docker
docker run -d -p 27017:27017 --name mongodb mongo:latest

# Or local MongoDB server
mongod
```

### 4. Start Backend Server

```bash
npm run dev  # Development with nodemon
# or
npm start    # Production
```

### 5. Verify API

```bash
curl http://localhost:3000/health
# Response: {"status":"OK","timestamp":...}
```

## Frontend Setup

### 1. Install Dependencies

```bash
cd frontend
npm install
```

### 2. Configure API URL

```bash
# .env.local (create file)
NEXT_PUBLIC_API_URL=http://localhost:3000/api
```

### 3. Start Development Server

```bash
npm run dev
# Opens at http://localhost:3000
```

## Testing the System

### 1. Register User

```bash
curl -X POST http://localhost:3000/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "email": "test@example.com",
    "password": "testpass123"
  }'
```

### 2. Login

```bash
curl -X POST http://localhost:3000/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "password": "testpass123"
  }'
```

Copy the `token` from response.

### 3. Log CAN Data

```bash
curl -X POST http://localhost:3000/api/can \
  -H "Content-Type: application/json" \
  -d '{
    "timestamp": 1708800000000,
    "canId": "0x100",
    "dlc": 2,
    "data": [0x50, 0x00],
    "deviceId": "esp32-001"
  }'
```

### 4. Access Dashboard

- Open http://localhost:3000/login
- Login with credentials
- View real-time vehicle status

## Production Deployment

### Docker Setup

```dockerfile
# backend/Dockerfile
FROM node:18-alpine
WORKDIR /app
COPY package*.json ./
RUN npm install --production
COPY . .
CMD ["npm", "start"]
```

```bash
docker build -t can-logger-backend .
docker run -d -p 3000:3000 \
  -e MONGODB_URI=mongodb://mongo:27017/can-logger \
  -e JWT_SECRET=production-secret \
  can-logger-backend
```

### HTTPS Configuration

```nginx
server {
    listen 443 ssl http2;
    server_name yourdomain.com;

    ssl_certificate /etc/letsencrypt/live/yourdomain.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/yourdomain.com/privkey.pem;

    location / {
        proxy_pass http://localhost:3000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

### Database Backup

```bash
# Backup MongoDB
mongodump --uri "mongodb://localhost:27017/can-logger" \
          --archive=can-logger-backup.archive

# Restore
mongorestore --uri "mongodb://localhost:27017/can-logger" \
             --archive=can-logger-backup.archive
```

## Troubleshooting

### Firmware Issues

**CAN frames not received:**
- Check MCP2515 wiring
- Verify CAN transceiver power
- Check bitrate configuration
- Test with CAN analyzer

**GPS not working:**
- Verify UART connections
- Check GPS baudrate
- Ensure outdoor location
- Check satellite fix

**SD card errors:**
- Format with FAT32
- Check chip select pin
- Verify SPI connections

### Backend Issues

**MongoDB connection failed:**
```bash
# Check MongoDB status
systemctl status mongodb

# Start MongoDB
systemctl start mongodb
```

**Port 3000 already in use:**
```bash
# Kill process
lsof -ti:3000 | xargs kill -9

# Or use different port
PORT=3001 npm start
```

### Frontend Issues

**API calls failing:**
- Check CORS settings in backend
- Verify API_URL in .env.local
- Check browser console for errors

**Charts not rendering:**
```bash
npm install chart.js react-chartjs-2 --save
```

## Next Steps

1. **Customize vehicle decoding**: Modify `vehicle_state_manager.cpp` for your vehicle's CAN IDs
2. **Integrate notifications**: Implement email/SMS in `anomalyService.js`
3. **Add authentication**: Implement OAuth2 for third-party integrations
4. **Deploy to cloud**: AWS EC2, DigitalOcean, or Heroku
5. **Mobile app**: Build React Native frontend for mobile access

---

**Last Updated**: February 2026
