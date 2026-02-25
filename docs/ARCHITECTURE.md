# CAN Bus Data Logger - Production Architecture

## Project Overview

Ultra-low-power intelligent CAN bus data logger for automotive diagnostics with passive logging, vehicle state reconstruction, embedded anomaly detection, GPS tracking, and web dashboard.

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    FIRMWARE (ESP32)                          │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ MCP2515 CAN Driver  │ GPS Module  │ SD Logger        │   │
│  │ Vehicle State Mgr   │ MQTT Client │ Anomaly Detector │   │
│  └──────────────────────────────────────────────────────┘   │
└────────────────┬───────────────────────────────────────────┘
                 │ MQTT/REST
        ┌────────┴────────┐
        │                 │
   ┌────▼─────┐    ┌──────▼──────┐
   │   WiFi   │    │  SD Card    │
   │  (Cloud) │    │  (Local)    │
   └────┬─────┘    └─────────────┘
        │
        │ REST APIs
   ┌────▼──────────────────────┐
   │  BACKEND (Node.js/Express)│
   │  ┌────────────────────┐   │
   │  │ Routes             │   │
   │  │ Services           │   │
   │  │ Middleware         │   │
   │  └────────────────────┘   │
   │         │                 │
   │         ▼                 │
   │  ┌────────────────────┐   │
   │  │  MongoDB Database  │   │
   │  │  - CAN Logs        │   │
   │  │  - Vehicle States  │   │
   │  │  - Anomalies       │   │
   │  │  - Users/Devices   │   │
   │  └────────────────────┘   │
   └────────────────────────────┘
        │
        │ REST APIs + WebSocket
   ┌────▼───────────────────────┐
   │  FRONTEND (React/Next.js)   │
   │  ┌────────────────────┐     │
   │  │ Dashboard          │     │
   │  │ - Live Gauges      │     │
   │  │ - Historical Charts│     │
   │  │ - GPS Map          │     │
   │  │ - Anomaly Timeline │     │
   │  └────────────────────┘     │
   └─────────────────────────────┘
```

## Directory Structure

```
Project/
├── firmware/                    # ESP32 firmware
│   ├── include/               # Header files
│   │   ├── config.h           # Configuration
│   │   ├── types.h            # Data structures
│   │   ├── logger.h           # Logging utility
│   │   ├── mcp2515_driver.h  # CAN driver
│   │   ├── gps_module.h       # GPS driver
│   │   ├── sd_logger.h        # SD logging
│   │   ├── vehicle_state_manager.h
│   │   ├── anomaly_detector.h
│   │   └── mqtt_client.h
│   ├── src/
│   │   ├── main.cpp           # Entry point
│   │   ├── drivers/           # Driver implementations
│   │   ├── modules/           # Module implementations
│   │   └── utils/             # Utilities
│   └── platformio.ini         # Build configuration
│
├── backend/                    # Node.js Express server
│   ├── src/
│   │   ├── index.js          # Server entry
│   │   ├── routes/           # API endpoints
│   │   │   ├── authRoutes.js
│   │   │   ├── canRoutes.js
│   │   │   └── anomalyRoutes.js
│   │   ├── models/           # MongoDB schemas
│   │   │   ├── User.js
│   │   │   ├── Device.js
│   │   │   ├── CANLog.js
│   │   │   ├── VehicleState.js
│   │   │   └── Anomaly.js
│   │   ├── services/         # Business logic
│   │   │   ├── canLogService.js
│   │   │   ├── anomalyService.js
│   │   │   └── vehicleStateService.js
│   │   ├── middleware/       # Express middleware
│   │   │   ├── auth.js
│   │   │   └── validation.js
│   │   └── utils/            # Utilities
│   │       └── logger.js
│   ├── package.json
│   └── .env.example
│
├── frontend/                   # Next.js React dashboard
│   ├── src/
│   │   ├── pages/            # Next.js pages
│   │   │   ├── _app.jsx
│   │   │   ├── index.jsx
│   │   │   ├── login.jsx
│   │   │   └── dashboard.jsx
│   │   ├── components/       # React components
│   │   │   ├── Gauges.jsx
│   │   │   ├── AnomalyComponents.jsx
│   │   │   ├── MapComponent.jsx
│   │   │   └── Charts.jsx
│   │   ├── services/         # API clients
│   │   │   └── api.js
│   │   ├── context/          # State management
│   │   │   └── store.js
│   │   └── styles/
│   │       └── globals.css
│   ├── public/               # Static assets
│   ├── package.json
│   ├── next.config.js
│   └── postcss.config.js
│
└── docs/                      # Documentation
    ├── API.md
    ├── FIRMWARE.md
    ├── DEPLOYMENT.md
    └── ARCHITECTURE.md
```

## Key Components

### 1. Firmware (ESP32)

**Modules:**
- **MCP2515 Driver**: SPI-based CAN controller communication
- **GPS Module**: NMEA parsing from NEO-6M module
- **SD Logger**: CSV logging with rolling files
- **Vehicle State Manager**: OBD-II decoding and state reconstruction
- **Anomaly Detector**: Rule-based + statistical anomaly detection
- **MQTT Client**: WiFi-enabled cloud communication

**Key Features:**
- Non-blocking asynchronous design
- Low power consumption with interrupt-driven CAN handling
- Modular architecture for easy vehicle customization
- JSON serialization for cloud transmission

### 2. Backend (Node.js/Express)

**Endpoints:**
```
POST   /api/auth/register          # User registration
POST   /api/auth/login             # User authentication
GET    /api/auth/me/devices        # List user devices

POST   /api/can/                   # Log CAN frame
GET    /api/can/:deviceId/latest   # Get latest CAN frames
GET    /api/can/:deviceId/range    # Query by time range
GET    /api/can/:deviceId/canId/:id # Query by CAN ID

POST   /api/anomalies/             # Report anomaly
GET    /api/anomalies/:deviceId/latest # Get anomalies
GET    /api/anomalies/:deviceId/unacknowledged
PATCH  /api/anomalies/:id/acknowledge
GET    /api/anomalies/:deviceId/statistics
```

**Database Schema:**
- **Users**: Authentication and permissions
- **Devices**: Device registration and metadata
- **CANLog**: Raw CAN frame records with timestamps
- **VehicleState**: Decoded vehicle state snapshots
- **Anomalies**: Detected anomalies with severity levels

### 3. Frontend (React/Next.js)

**Key Pages:**
- **Login**: JWT-based authentication
- **Dashboard**: Real-time vehicle gauges, charts, and anomaly timeline
- **Map View**: GPS location and anomaly markers
- **Analytics**: Historical data visualization

**Components:**
- Speed/RPM gauges with color-coded ranges
- Real-time line charts for trends
- Interactive Leaflet map
- Responsive anomaly cards with severity levels
- Device selector and time range filters

## Data Flow

### Real-time Flow
```
CAN Bus
  ↓
MCP2515 reads frame
  ↓
Vehicle State Manager decodes
  ↓
Anomaly Detector analyzes
  ↓
SD Card logs (local backup)
  ↓
MQTT publishes to cloud
  ↓
Backend receives via HTTP
  ↓
MongoDB stores
  ↓
WebSocket notifies frontend
  ↓
Dashboard updates gauges/charts
```

### On-Device Flow
```
CAN Frame
  ↓
Logger::log() → Serial console
SD Logger → /can_log.csv
Vehicle State Manager → Current state
Anomaly Detector
  ├─ Rule-based checks (speed, RPM, frequency)
  ├─ Statistical analysis (3-sigma)
  └─ Triggers notification on severity=HIGH
```

## API Response Examples

### Log CAN Frame
```bash
POST /api/can/
{
  "timestamp": 1708800000000,
  "canId": "0x123",
  "dlc": 8,
  "data": [0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80],
  "deviceId": "esp32-001"
}
```

### Vehicle State
```bash
GET /api/can/esp32-001/latest
[
  {
    "timestamp": 1708800000000,
    "speed": 60,
    "rpm": 3000,
    "throttle": 45,
    "gear": 3,
    "engineStatus": "ON",
    "fault": false,
    "latitude": 40.7128,
    "longitude": -74.0060
  }
]
```

### Anomaly Alert
```bash
POST /api/anomalies/
{
  "timestamp": 1708800000000,
  "deviceId": "esp32-001",
  "type": "RPM_SPIKE",
  "description": "RPM spike detected: 3000 RPM → 5500 RPM (delta: 2500)",
  "severity": 2,
  "canId": "0x101",
  "latitude": 40.7128,
  "longitude": -74.0060,
  "vehicleState": {
    "speed": 60,
    "rpm": 5500,
    "throttle": 80,
    "gear": 2,
    "engineStatus": "ON"
  }
}
```

## Security Considerations

1. **Authentication**: JWT tokens with 7-day expiration
2. **Authorization**: Role-based access (admin, user, viewer)
3. **Data Validation**: Input validation on all endpoints with Joi
4. **Rate Limiting**: 100 requests per 15 minutes per IP
5. **HTTPS**: Required in production
6. **CORS**: Whitelist allowed origins
7. **Password**: Bcrypt hashing with salt rounds=10
8. **Device Authorization**: Only owners can access device data

## Deployment Guide

### Firmware Deployment
```bash
cd firmware
platformio run -e esp32doit-devkit-v1 -t upload
```

### Backend Deployment
```bash
cd backend
npm install
cp .env.example .env  # Configure environment
npm start
```

### Frontend Deployment
```bash
cd frontend
npm install
npm run build
npm start
```

## Performance Optimization

### Firmware
- DMA-based SPI for CAN communication
- Interrupt-driven CAN handling (no polling)
- Efficient ring buffer for data buffering
- Compressed JSON for MQTT payloads

### Backend
- MongoDB indexes on deviceId and timestamp
- Pagination for large result sets
- Response caching for static queries
- Connection pooling

### Frontend
- Code splitting with Next.js
- Image optimization
- Lazy loading components
- Chart rendering optimization

## Monitoring & Logging

- **Firmware**: Serial console + SD card logs
- **Backend**: Winston logger with daily rotation
- **Frontend**: Browser console + error boundary
- **Production**: Centralized logging with ELK stack (optional)

## Troubleshooting

See [TROUBLESHOOTING.md](./TROUBLESHOOTING.md) for common issues and solutions.

---

**Version**: 1.0.0  
**Last Updated**: February 2026  
**License**: MIT
