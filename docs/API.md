# API Documentation

## Base URL
```
http://localhost:3000/api
```

## Authentication

All endpoints (except `/auth/register` and `/auth/login`) require JWT token:
```
Authorization: Bearer <token>
```

## Endpoints

### Authentication

#### Register User
```http
POST /auth/register
Content-Type: application/json

{
  "username": "john_doe",
  "email": "john@example.com",
  "password": "securepassword"
}

Response 201:
{
  "user": {
    "id": "507f1f77bcf86cd799439011",
    "username": "john_doe",
    "email": "john@example.com"
  },
  "token": "eyJhbGciOiJIUzI1NiIs..."
}
```

#### Login
```http
POST /auth/login
Content-Type: application/json

{
  "username": "john_doe",
  "password": "securepassword"
}

Response 200:
{
  "user": {
    "id": "507f1f77bcf86cd799439011",
    "username": "john_doe",
    "email": "john@example.com",
    "role": "user"
  },
  "token": "eyJhbGciOiJIUzI1NiIs..."
}
```

#### Get User Devices
```http
GET /auth/me/devices
Authorization: Bearer <token>

Response 200:
[
  {
    "_id": "507f1f77bcf86cd799439012",
    "deviceId": "esp32-001",
    "name": "Vehicle Logger",
    "vehicleInfo": {
      "make": "Toyota",
      "model": "Camry",
      "year": 2020,
      "vin": "12345ABCDE67890FG",
      "licensePlate": "ABC123"
    },
    "lastSeen": "2026-02-25T10:30:00Z",
    "isActive": true
  }
]
```

### CAN Data

#### Log CAN Frame
```http
POST /can
Content-Type: application/json

{
  "timestamp": 1708800000000,
  "canId": "0x123",
  "dlc": 8,
  "data": [0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80],
  "deviceId": "esp32-001"
}

Response 201:
{
  "_id": "507f1f77bcf86cd799439013",
  "timestamp": 1708800000000,
  "canId": "0x123",
  "dlc": 8,
  "data": [0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80],
  "deviceId": "esp32-001",
  "createdAt": "2026-02-25T10:30:00Z"
}
```

#### Get Latest CAN Frames
```http
GET /can/:deviceId/latest?limit=100
Authorization: Bearer <token>

Response 200:
[
  {
    "_id": "507f1f77bcf86cd799439013",
    "timestamp": 1708800000000,
    "canId": "0x100",
    "dlc": 2,
    "data": [0x50, 0x00],
    "deviceId": "esp32-001"
  },
  ...
]
```

#### Get CAN Frames by Time Range
```http
GET /can/:deviceId/range?startTime=1708700000000&endTime=1708800000000
Authorization: Bearer <token>

Response 200:
[
  { ... }
]
```

#### Get Frames by CAN ID
```http
GET /can/:deviceId/canId/0x100?limit=50
Authorization: Bearer <token>

Response 200:
[
  { ... }
]
```

### Anomalies

#### Log Anomaly
```http
POST /anomalies
Content-Type: application/json

{
  "timestamp": 1708800000000,
  "deviceId": "esp32-001",
  "type": "RPM_SPIKE",
  "description": "RPM spike detected: 3000 RPM → 5500 RPM",
  "severity": 2,
  "canId": "0x101",
  "latitude": 40.7128,
  "longitude": -74.0060
}

Response 201:
{
  "_id": "507f1f77bcf86cd799439014",
  "timestamp": 1708800000000,
  "deviceId": "esp32-001",
  "type": "RPM_SPIKE",
  "description": "RPM spike detected: 3000 RPM → 5500 RPM",
  "severity": 2,
  "canId": "0x101",
  "acknowledged": false,
  "createdAt": "2026-02-25T10:30:00Z"
}
```

#### Get Latest Anomalies
```http
GET /anomalies/:deviceId/latest?limit=50
Authorization: Bearer <token>

Response 200:
[
  {
    "_id": "507f1f77bcf86cd799439014",
    "timestamp": 1708800000000,
    "type": "RPM_SPIKE",
    "description": "RPM spike detected",
    "severity": 2,
    "acknowledged": false
  },
  ...
]
```

#### Get Unacknowledged Anomalies
```http
GET /anomalies/:deviceId/unacknowledged
Authorization: Bearer <token>

Response 200:
[
  { ... }
]
```

#### Acknowledge Anomaly
```http
PATCH /anomalies/:anomalyId/acknowledge
Authorization: Bearer <token>
Content-Type: application/json

{
  "notes": "Reviewed and confirmed non-critical"
}

Response 200:
{
  "_id": "507f1f77bcf86cd799439014",
  "acknowledged": true,
  "acknowledgedBy": "507f1f77bcf86cd799439011",
  "acknowledgedAt": "2026-02-25T10:35:00Z",
  "notes": "Reviewed and confirmed non-critical"
}
```

#### Get Anomaly Statistics
```http
GET /anomalies/:deviceId/statistics?timeRange=86400000
Authorization: Bearer <token>

Response 200:
[
  {
    "_id": "RPM_SPIKE",
    "count": 5,
    "avgSeverity": 2.2
  },
  {
    "_id": "SPEED_EXCEED",
    "count": 3,
    "avgSeverity": 2
  },
  ...
]
```

## Error Responses

### 400 Bad Request
```json
{
  "error": "Validation error",
  "details": [
    "\"dlc\" must be between 0 and 8",
    "\"data\" must be an array"
  ]
}
```

### 401 Unauthorized
```json
{
  "error": "Invalid token"
}
```

### 403 Forbidden
```json
{
  "error": "Insufficient permissions"
}
```

### 404 Not Found
```json
{
  "error": "Route not found"
}
```

### 429 Too Many Requests
```json
{
  "error": "Too many requests, please try again later."
}
```

### 500 Internal Server Error
```json
{
  "error": "Internal server error"
}
```

## Rate Limiting

- **Limit**: 100 requests per 15 minutes per IP
- **Headers**:
  ```
  RateLimit-Limit: 100
  RateLimit-Remaining: 99
  RateLimit-Reset: 1708800900
  ```

## Data Types

### Anomaly Type
- `RPM_SPIKE`: Sudden RPM change
- `SPEED_EXCEED`: Speed above threshold
- `CAN_FREQUENCY`: Abnormal message frequency
- `MISSING_MESSAGE`: Expected message not received
- `ENGINE_INCONSISTENCY`: Engine state contradiction
- `STATISTICAL_DEVIATION`: 3-sigma statistical anomaly

### Severity Levels
- `1`: LOW - Informational
- `2`: MEDIUM - Warning
- `3`: HIGH - Critical

---

**Version**: 1.0.0  
**Last Updated**: February 2026
