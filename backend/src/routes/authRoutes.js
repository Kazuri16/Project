const express = require('express');
const jwt = require('jsonwebtoken');
const User = require('../models/User');
const Device = require('../models/Device');
const { validateRequest, schemas } = require('../middleware/validation');
const logger = require('../utils/logger');

const router = express.Router();

// Register user
router.post(
    '/register',
    validateRequest(schemas.registerUser),
    async (req, res, next) => {
        try {
            const { username, email, password } = req.validatedData;

            const existingUser = await User.findOne({ $or: [{ username }, { email }] });
            if (existingUser) {
                return res.status(400).json({ error: 'User already exists' });
            }

            const user = new User({ username, email, password });
            await user.save();

            const token = jwt.sign(
                { id: user._id, role: user.role },
                process.env.JWT_SECRET,
                { expiresIn: '7d' }
            );

            logger.info(`User registered: ${username}`);
            res.status(201).json({
                user: { id: user._id, username: user.username, email: user.email },
                token
            });
        } catch (error) {
            next(error);
        }
    }
);

// Login user
router.post(
    '/login',
    validateRequest(schemas.loginUser),
    async (req, res, next) => {
        try {
            const { username, password } = req.validatedData;

            const user = await User.findOne({ username }).select('+password');
            if (!user) {
                return res.status(401).json({ error: 'Invalid credentials' });
            }

            const isPasswordValid = await user.matchPassword(password);
            if (!isPasswordValid) {
                return res.status(401).json({ error: 'Invalid credentials' });
            }

            const token = jwt.sign(
                { id: user._id, role: user.role },
                process.env.JWT_SECRET,
                { expiresIn: '7d' }
            );

            logger.info(`User logged in: ${username}`);
            res.json({
                user: { id: user._id, username: user.username, email: user.email, role: user.role },
                token
            });
        } catch (error) {
            next(error);
        }
    }
);

// Get user devices
router.get(
    '/me/devices',
    async (req, res, next) => {
        try {
            const token = req.headers.authorization?.split(' ')[1];
            if (!token) return res.status(401).json({ error: 'No token' });

            const decoded = jwt.verify(token, process.env.JWT_SECRET);
            const devices = await Device.find({ owner: decoded.id }).lean();

            res.json(devices);
        } catch (error) {
            next(error);
        }
    }
);

module.exports = router;
