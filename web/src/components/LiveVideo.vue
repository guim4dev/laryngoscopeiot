<template>
  <div class="live-video">camera_placeholder</div>
  <!-- <img class="live-video" :src="src" @load="videoLoaded" /> -->
</template>

<script setup lang="ts">
import { ref, defineProps } from "vue";
defineProps<{ src: string }>();
const emit = defineEmits(["liveVideoStarted"]);

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
  border: 1px solid white;
  border-radius: 8px;
  padding: 8px;
  background-color: black;
  display: flex;
  align-items: center;
  justify-content: center;
}
</style>