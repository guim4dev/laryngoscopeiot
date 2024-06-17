<template>
  <div class="wrapper">
    <Transition name="fade" appear>
      <h1 class="loading animated" v-show="isLoading">{{ t("welcome") }}</h1>
    </Transition>
    <Transition name="fade" appear>
      <div class="app-layout animated" v-show="!isLoading"> 
        <Sensors
          key="sensors"
          @started="handleSensorsStarted"
          :src="sensorsEndpoint"
          v-if="sensorsEndpoint"
          class="sensors"
        />
        <p v-else>
          {{ t("no_sensors_src") }}
        </p>
        <LiveVideo
          key="live-video"
          v-show="!isLoading"
          @liveVideoStarted="handleVideoLoaded"
          :src="liveVideoEndpoint"
          v-if="liveVideoEndpoint"
          class="live-video"
        />
        <p v-else>
          {{ t("no_camera_src") }}
        </p>
      </div>
    </Transition>
  </div>
</template>
<script setup lang="ts">
import { onMounted, ref } from "vue";
import { useRoute } from "vue-router";
import { useI18n } from "vue-i18n";
import { ElMessage } from "element-plus";

import LiveVideo from "@/components/LiveVideo.vue";
import Sensors from "@/components/Sensors.vue";

const { t } = useI18n();
const route = useRoute();

const { query } = route;
const { ip, cameraStreamUrl, sensorsUrl } = query as { ip?: string; cameraStreamUrl?: string; sensorsUrl?: string };

const liveVideoEndpoint = cameraStreamUrl || ip ? `http://${ip}:80/camera` : "";
const sensorsEndpoint = sensorsUrl || ip ? `http://${ip}:80/sensors` : "";

const sensorsStarted = ref(false);
const videoLoaded = ref(false);

const isLoading = ref(true);
const handleVideoLoaded = () => {
  videoLoaded.value = true;
  ElMessage({ message: t("video_started"), type: "success" });
  if (sensorsStarted.value) isLoading.value = false;
};

const handleSensorsStarted = () => {
  sensorsStarted.value = true;
  ElMessage({ message: t("sensors_started"), type: "success" });
  if (videoLoaded.value) isLoading.value = false;
};

const TIMEOUT = 5000;

onMounted(() => {
  setTimeout(() => {
    if (sensorsStarted || videoLoaded) {
      console.log("Timeout reached, stopping loading animation and showing content that was able to load");
      isLoading.value = false;
    }
  }, TIMEOUT);
})
</script>
  
<style scoped>
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
  gap: 1rem;
  height: 100%;
  width: 100%;
  padding: 1rem;
}

.sensors {
  height: 15%;
}

.live-video {
  flex-grow: 1;
}
</style>
  