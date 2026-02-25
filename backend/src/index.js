require('dotenv').config();
const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const mongoose = require('mongoose');
const morgan = require('morgan');

const logger = require('./utils/logger');
const { errorHandler, rateLimiter } = require('./middleware/auth');

const authRoutes = require('./routes/authRoutes');
const canRoutes = require('./routes/canRoutes');
const anomalyRoutes = require('./routes/anomalyRoutes');

const app = express();

// ===== MIDDLEWARE =====
app.use(helmet());
app.use(cors({
    origin: process.env.ALLOWED_ORIGINS || ['http://localhost:3000', 'http://localhost:3001'],
    credentials: true
}));
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ limit: '10mb', extended: true }));
app.use(morgan('combined', { stream: { write: msg => logger.info(msg.trim()) } }));

// ===== DATABASE CONNECTION =====
mongoose.connect(process.env.MONGODB_URI, {
    useNewUrlParser: true,
    useUnifiedTopology: true
})
.then(() => logger.info('Connected to MongoDB'))
.catch(err => {
    logger.error('MongoDB connection error:', err);
    process.exit(1);
});

// ===== ROUTES =====
app.use('/api/auth', rateLimiter, authRoutes);
app.use('/api/can', rateLimiter, canRoutes);
app.use('/api/anomalies', rateLimiter, anomalyRoutes);

// Health check endpoint
app.get('/health', (req, res) => {
    res.json({ status: 'OK', timestamp: Date.now() });
});

// API documentation
app.get('/api', (req, res) => {
    res.json({
        name: 'CAN Logger Backend API',
        version: '1.0.0',
        endpoints: {
            auth: '/api/auth',
            can: '/api/can',
            anomalies: '/api/anomalies'
        }
    });
});

// 404 handler
app.use((req, res) => {
    res.status(404).json({ error: 'Route not found' });
});

// Error handler
app.use(errorHandler);

// ===== SERVER START =====
const PORT = process.env.PORT || 3000;
const server = app.listen(PORT, () => {
    logger.info(`CAN Logger Backend running on port ${PORT}`);
});

// Graceful shutdown
process.on('SIGTERM', () => {
    logger.info('SIGTERM signal received: closing HTTP server');
    server.close(() => {
        logger.info('HTTP server closed');
        mongoose.connection.close(false, () => {
            logger.info('MongoDB connection closed');
            process.exit(0);
        });
    });
});

module.exports = app;
