<template>
  <div class="live-video">camera_placeholder</div>
  <!-- <img class="live-video" :src="src" @load="videoLoaded" /> -->
</template>

<script setup lang="ts">
import { ref, onMounted } from "vue";
// import { useEventSource } from "@vueuse/core";
// import { ElMessage } from "element-plus";
// import { useI18n } from "vue-i18n";
// const { t } = useI18n();

// const props = defineProps<{ src: string }>();
const emit = defineEmits(["liveVideoStarted"]);

// TMP: testing event source implementation for camera data
// const { status, data } = useEventSource(props.src, ["cameraData"], {
//   autoReconnect: {
//     retries: 3,
//     delay: 1000,
//     onFailed() {
//       ElMessage({ message: t('camera.failed_to_connect'), type: 'error' })
//     },
//   },
// })

// watch(status, (newStatus) => {
//     if (newStatus === 'CLOSED') {
//         ElMessage({ message: t('camera.connection_closed'), type: 'error' })
//     }

//     if (newStatus === 'CONNECTING') {
//         console.log('Sensors EventSource is connecting...')
//     }

//     if (newStatus === 'OPEN') {
//         console.log('Sensors EventSource is connected')
//         emit('liveVideoStarted')
//     }
// })

// watch(data, (newData) => {
//     if (!newData || newData?.startsWith("connected")) return

//     console.log('Received data:', newData)
// })

const loaded = ref(false);
const videoLoaded = () => {
  if (loaded.value) return;

  loaded.value = true;
  emit("liveVideoStarted");
};

onMounted(() => {
  setTimeout(() => {
    videoLoaded();
  }, 3000);
});
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