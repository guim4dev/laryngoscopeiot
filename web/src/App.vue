<template>
  <transition-group appear mode="out-in" name="fade">
    <h1 key="loading-text" v-show="isLoading">{{ t("welcome") }}</h1>
    <live-video
      key="live-video"
      v-show="!isLoading"
      @liveVideoStarted="videoLoaded"
    />
  </transition-group>
</template>
<script setup lang="ts">
import { onMounted, ref } from "vue";
import LiveVideo from "./components/LiveVideo.vue";
import { useI18n } from "vue-i18n";
const i18n = useI18n();
const { t } = i18n;

const isLoading = ref(true);
const videoLoaded = () => {
  console.log("Video loaded");
  isLoading.value = false;
};

onMounted(() => {
  const userLanguage = navigator.language;
  i18n.locale.value = userLanguage;
});
</script>

<style scoped>
.logo {
  height: 6em;
  padding: 1.5em;
  will-change: filter;
  transition: filter 300ms;
}
.logo:hover {
  filter: drop-shadow(0 0 2em #646cffaa);
}
.logo.vue:hover {
  filter: drop-shadow(0 0 2em #42b883aa);
}
</style>
