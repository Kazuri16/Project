const mongoose = require('mongoose');

const deviceSchema = new mongoose.Schema(
    {
        deviceId: {
            type: String,
            required: true,
            unique: true,
            index: true
        },
        name: {
            type: String,
            required: true
        },
        owner: {
            type: mongoose.Schema.Types.ObjectId,
            ref: 'User',
            required: true,
            index: true
        },
        vehicleInfo: {
            make: String,
            model: String,
            year: Number,
            vin: String,
            licensePlate: String
        },
        lastSeen: {
            type: Date,
            default: Date.now
        },
        isActive: {
            type: Boolean,
            default: true
        },
        settings: {
            logInterval: { type: Number, default: 5000 },
            anomalyThresholds: {
                rpmSpikeDelta: { type: Number, default: 500 },
                maxSpeed: { type: Number, default: 200 }
            }
        }
    },
    { timestamps: true }
);

module.exports = mongoose.model('Device', deviceSchema);
