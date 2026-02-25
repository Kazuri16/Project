const Joi = require('joi');
const logger = require('../utils/logger');

const validateRequest = (schema) => {
    return (req, res, next) => {
        const { error, value } = schema.validate(req.body);

        if (error) {
            logger.warn('Validation error:', error.details);
            return res.status(400).json({
                error: 'Validation error',
                details: error.details.map(d => d.message)
            });
        }

        req.validatedData = value;
        next();
    };
};

const schemas = {
    loginUser: Joi.object({
        username: Joi.string().min(3).required(),
        password: Joi.string().min(6).required()
    }),

    registerUser: Joi.object({
        username: Joi.string().min(3).required(),
        email: Joi.string().email().required(),
        password: Joi.string().min(6).required()
    }),

    createDevice: Joi.object({
        deviceId: Joi.string().required(),
        name: Joi.string().required(),
        vehicleInfo: Joi.object({
            make: Joi.string(),
            model: Joi.string(),
            year: Joi.number().min(1900).max(2100),
            vin: Joi.string(),
            licensePlate: Joi.string()
        })
    }),

    logCANData: Joi.object({
        timestamp: Joi.number().required(),
        canId: Joi.string().required(),
        dlc: Joi.number().min(0).max(8).required(),
        data: Joi.array().max(8).items(Joi.number().min(0).max(255)).required(),
        deviceId: Joi.string().required()
    }),

    logAnomaly: Joi.object({
        timestamp: Joi.number().required(),
        type: Joi.string().valid(
            'RPM_SPIKE', 'SPEED_EXCEED', 'CAN_FREQUENCY',
            'MISSING_MESSAGE', 'ENGINE_INCONSISTENCY', 'STATISTICAL_DEVIATION'
        ).required(),
        description: Joi.string().required(),
        severity: Joi.number().valid(1, 2, 3).required(),
        deviceId: Joi.string().required()
    })
};

module.exports = {
    validateRequest,
    schemas
};
