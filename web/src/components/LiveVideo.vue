<template>
  <iframe class="live-video" :src="liveVideoUrl" @load="videoLoaded" />
</template>

<script setup lang="ts">
import config from "../config";
const emit = defineEmits(["liveVideoStarted"]);
const { deviceUrl } = config;
const liveVideoUrl = `${deviceUrl}/camera_stream`;

const videoLoaded = () => {
  emit("liveVideoStarted");
};
</script>

<style scoped>
.live-video {
  width: 100%;
  height: 100%;
  border: none;
}
</style>