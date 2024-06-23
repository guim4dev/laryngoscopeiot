<template>
  <div class="video-wrapper">
    <Spinner v-show="resetingLiveVideo" :height="'80px'" :width="'80px'" />
    <img v-if="!resetingLiveVideo" class="live-video" :src="src" @error="handleLoadError"/>
    <ElButton v-show="!resetingLiveVideo" class="reset-button" @click="resetLiveVideo">
      <ElIcon style="vertical-align: middle">
        <ElIconRefreshRight />
      </ElIcon>
  </Elbutton>
  </div>
</template>

<script setup lang="ts">
import { onMounted, ref } from "vue";
import Spinner from "@/components/common/Spinner.vue";

const props = defineProps<{ cameraServer: string }>();
const src = `${props.cameraServer}/camera/stream`;

const resetingLiveVideo = ref(false);

const cameraLoadDelay = 1000;

const resetLiveVideo = () => {
  resetingLiveVideo.value = true;
  setTimeout(() => {
    resetingLiveVideo.value = false;
  }, cameraLoadDelay);
}

const handleLoadError = (event: Event) => {
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
  top: 8px;
  right: 8px;
  z-index: 1;
}

.video-wrapper{
  background-color: black;
  display: flex;
  align-items: center;
  justify-content: center;
  border: 1px solid white;
  border-radius: 8px;
  position: relative;
}

.live-video {
  width: 100%;
  height: 100%;
  object-fit: contain;
}
</style>