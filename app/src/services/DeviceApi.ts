import axios, { AxiosInstance } from "axios";

const DEVICE_ALIVE_TIMEOUT = 3000; // 3 seconds

export interface Measurement {
    teethPressed: 0 | 1,
    tongueForce: number
}

export interface TimedMeasurement extends Measurement {
    timestamp: number
}

export class DeviceApi {
    public deviceIp: string;
    private apiClient: AxiosInstance;
    static deviceApiSingleton: DeviceApi;

    constructor(deviceIp: string) {
        this.deviceIp = deviceIp;
        this.apiClient = axios.create({
            baseURL: `http://${deviceIp}`,
        });
    }

    static getInstance(deviceIp?: string) {
        if (deviceIp) {
            DeviceApi.deviceApiSingleton = new DeviceApi(deviceIp);
        }
        return DeviceApi.deviceApiSingleton;
    }

    async isAlive() {
        try {
            const { data } = await this.apiClient.get("/laringoscopiot/status", {
                responseType: "text",
                timeout: DEVICE_ALIVE_TIMEOUT
            });
            return data === "alive"
        } catch (error) {
            return false;
        }
    }

    async resetCamera() {
        try {
            await this.apiClient.get("/camera/reset");
            return true;
        } catch (error) {
            return false;
        }
    }
}