import axios from 'axios';

const API_URL = process.env.NEXT_PUBLIC_API_URL;

const apiClient = axios.create({
    baseURL: API_URL,
    headers: {
        'Content-Type': 'application/json'
    }
});

// Add token to requests
apiClient.interceptors.request.use((config) => {
    const token = localStorage.getItem('token');
    if (token) {
        config.headers.Authorization = `Bearer ${token}`;
    }
    return config;
});

// Handle response errors
apiClient.interceptors.response.use(
    (response) => response,
    (error) => {
        if (error.response?.status === 401) {
            localStorage.removeItem('token');
            window.location.href = '/login';
        }
        return Promise.reject(error);
    }
);

// Auth API
export const authAPI = {
    register: (data) => apiClient.post('/auth/register', data),
    login: (data) => apiClient.post('/auth/login', data),
    getDevices: () => apiClient.get('/auth/me/devices')
};

// CAN Data API
export const canAPI = {
    getLatest: (deviceId, limit = 100) =>
        apiClient.get(`/can/${deviceId}/latest?limit=${limit}`),
    getByTimeRange: (deviceId, startTime, endTime) =>
        apiClient.get(`/can/${deviceId}/range?startTime=${startTime}&endTime=${endTime}`),
    getByCanId: (deviceId, canId, limit = 50) =>
        apiClient.get(`/can/${deviceId}/canId/${canId}?limit=${limit}`)
};

// Anomaly API
export const anomalyAPI = {
    getLatest: (deviceId, limit = 50) =>
        apiClient.get(`/anomalies/${deviceId}/latest?limit=${limit}`),
    getUnacknowledged: (deviceId) =>
        apiClient.get(`/anomalies/${deviceId}/unacknowledged`),
    acknowledge: (anomalyId, data) =>
        apiClient.patch(`/anomalies/${anomalyId}/acknowledge`, data),
    getStatistics: (deviceId, timeRange = 86400000) =>
        apiClient.get(`/anomalies/${deviceId}/statistics?timeRange=${timeRange}`)
};

export default apiClient;
