<template>
  <div class="wrapper">
    <Transition name="fade" appear>
      <h1 class="loading animated" v-show="!isDeviceAlive && initialLoading">{{ t("welcome") }}</h1>
    </Transition>
    <Transition name="fade" appear>
      <div class="app-layout animated" v-if="isDeviceAlive" v-show="!initialLoading">
        <LiveVideo key="live-video" v-if="showCamera" v-show="!initialLoading" :cameraServer="server" class="live-video" />
        <Sensors key="sensors" @started="handleSensorsStarted" :src="sensorsEndpoint" @lostConnection="validateIfDeviceIsAlive"
          class="sensors" v-if="showSensors"/>
      </div>
    </Transition>
    <ElDialog
      :title="t('device_not_alive')"
      v-model="showDeviceAliveDialog"
      width="350"
      align-center
      :showClose="false"
      :closeOnClickModal="false"
      :closeOnPressEscape="false"
    >
      <div class="body-dialog-wrapper">
        <span v-html="t('device_not_alive_description')" />
        <span v-if="showChromeDisclaimer" v-html="t('chrome_disclaimer')" />
      </div>
      <template #footer>
        <div class="alive-footer">
          <span>{{ t("device_awaiting_for_connection") }}</span>
          <DotsLoader size="20px" color="#303133" />
        </div>
      </template>
    </ElDialog>
  </div>
</template>
<script setup lang="ts">
import { computed, onMounted, ref } from "vue";
import { useRoute } from "vue-router";
import { useI18n } from "vue-i18n";
import { ElMessage } from "element-plus";
import { DeviceApi } from "@/services/DeviceApi";
import { Capacitor } from "@capacitor/core";

import LiveVideo from "@/components/LiveVideo.vue";
import Sensors from "@/components/Sensors.vue";
import DotsLoader from "@/components/common/DotsLoader.vue";

const { t } = useI18n();
const route = useRoute();

const { query } = route;
const { ip, deviceServer, noCamera, noSensors } = query as { ip?: string; deviceServer?: string, noCamera?: string, noSensors?: string};
const deviceIp = ip || "192.168.4.1" // default IP address of the device - value when in AP mode
const showCamera = noCamera !== "true";
const showSensors = noSensors !== "true";
const isDeviceAlive = ref(false);
const initialLoading = ref(true);
const validatedDeviceAliveCount = ref(0);

const deviceApi = new DeviceApi(deviceIp);

const server = deviceServer || deviceIp ? `http://${deviceIp}` : "";
const sensorsEndpoint = server ? `${server}/sensors` : "";

const sensorsStarted = ref(false);

const handleSensorsStarted = () => {
  sensorsStarted.value = true;
  ElMessage({ message: t("sensors_started"), type: "success" });
};

const showChromeDisclaimer = !Capacitor.isNativePlatform();

const INTERVAL = 2000;

type Interval = ReturnType<typeof setInterval>;
let checkDeviceAliveInterval: Interval | undefined = undefined;

let runningValidation = false; // to prevent multiple validation requests at same time
const validateIfDeviceIsAlive = async () => {
  if (runningValidation) return;
  runningValidation = true;

  isDeviceAlive.value = await deviceApi.isAlive();
  if (!isDeviceAlive.value) {
    validatedDeviceAliveCount.value += 1;

    if (!checkDeviceAliveInterval) {
      checkDeviceAliveInterval = setInterval(async () =>
        await validateIfDeviceIsAlive()
      , INTERVAL);
    }
  } else {
    if (checkDeviceAliveInterval) {
      clearInterval(checkDeviceAliveInterval);
      checkDeviceAliveInterval = undefined;
    }
    validatedDeviceAliveCount.value = 0;
  }
  runningValidation = false;
};

const showDeviceAliveDialog = computed(() => {
  return !isDeviceAlive.value && validatedDeviceAliveCount.value > 0;
});

const INITIAL_LOADING = 2000; // 2 seconds

onMounted(async () => {
  setTimeout(() => {
    initialLoading.value = false;
  }, INITIAL_LOADING);
  await validateIfDeviceIsAlive();
})
</script>

<style scoped>
.body-dialog-wrapper {
  width: 100%;
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}

.alive-footer {
  display: flex;
  width: 100%;
  align-items: flex-end;
  justify-content: flex-end;
  gap: 1rem;
  color: #606266;
  font-size: 0.7rem;
}

.alive-footer span {
  display: inline-block;
  flex-grow: 1;
}

.wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 100dvh;
  position: relative;
}

.loading {
  height: 100%;
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 1rem;
  text-align: center;
}

.animated {
  position: absolute;
  top: 0;
  left: 0;
}

.app-layout {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 100%;
  width: 100%;
  position: relative;
}

.sensors {
  height: 15%;
  top: 0;
  left: 0;
  padding: 1rem 2rem;
  position: absolute;
  pointer-events: none;
}

.live-video {
  position: absolute;
  height: 100%;
}
</style>