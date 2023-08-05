<template>
  <img class="live-video" :src="liveVideoUrl" @load="videoLoaded" />
</template>

<script setup lang="ts">
import config from "../config";
import { ref } from "vue";
const emit = defineEmits(["liveVideoStarted"]);
const { deviceUrl } = config;
const liveVideoUrl = `${deviceUrl}/camera_stream`;

const loaded = ref(false);
const videoLoaded = () => {
  if (loaded.value) return;

  loaded.value = true;
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