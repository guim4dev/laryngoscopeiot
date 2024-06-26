<template>
  <div class="video-wrapper">
    <Spinner v-show="resetingLiveVideo" :height="'80px'" :width="'80px'" />
    <img v-if="!resetingLiveVideo" class="live-video" :src="src" @error="handleLoadError"/>
    <ElButton v-show="!resetingLiveVideo" class="reset-button" @click="resetLiveVideo">
      <ElIcon style="vertical-align: middle">
        <ElIconRefreshRight />
      </ElIcon>
      <span>
        {{ $t("camera.reset") }}
      </span>
  </Elbutton>
  </div>
</template>

<script setup lang="ts">
import { onMounted, ref } from "vue";
import Spinner from "@/components/common/Spinner.vue";

const props = defineProps<{ cameraServer: string }>();
const src = `${props.cameraServer}/camera/stream`;

const resetingLiveVideo = ref(false);

const cameraLoadDelay = 2000; // 2 seconds of delay

const resetLiveVideo = async () => {
  resetingLiveVideo.value = true;
  // await deviceApi.resetCamera(); // TODO: finish this implementation reset camera
  setTimeout(() => {
    resetingLiveVideo.value = false;
  }, cameraLoadDelay);
}

const handleLoadError = () => {
  resetLiveVideo();
};

onMounted(() => {
  setTimeout(() => {
    if (resetingLiveVideo) {
      return
    }
  }, cameraLoadDelay);
});
</script>

<style scoped>
.reset-button {
  position: absolute;
  top: 1rem;
  right: 1rem;
  z-index: 1;
}

.video-wrapper {
  display: flex;
  align-items: center;
  justify-content: center;
  position: relative;
}

.live-video {
  width: 100%;
  height: 100%;
  object-fit: contain;
  border: 1px solid white;
  border-radius: 16px
}
</style>