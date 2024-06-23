import axios, { AxiosInstance } from "axios";

const DEVICE_ALIVE_TIMEOUT = 3000; // 3 seconds

export class DeviceApi {
    public deviceIp: string;
    private apiClient: AxiosInstance;

    constructor(deviceIp: string) {
        this.deviceIp = deviceIp;
        this.apiClient = axios.create({
            baseURL: `http://${deviceIp}`,
        });
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
}