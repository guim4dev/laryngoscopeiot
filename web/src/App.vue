<template>
  <router-view />
</template>
<script setup lang="ts">
import { onMounted } from "vue";
import { useI18n } from "vue-i18n";
const i18n = useI18n();

onMounted(() => {
  const userLanguage = navigator.language;
  i18n.locale.value = userLanguage;
});
</script>
