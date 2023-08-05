<template>
  <h1>{{ t("welcome") }}</h1>
</template>
<script setup lang="ts">
import { onMounted } from "vue";
import { useI18n } from "vue-i18n";
const i18n = useI18n();
const { t } = i18n;

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
