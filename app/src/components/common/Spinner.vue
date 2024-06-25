<template>
    <div class="spinner"/>
</template>

<script setup lang="ts">
defineProps({
    width: {
        type: String,
        default: "100%"
    },
    height: {
        type: String,
        default: "100%"
    },
    color: {
        type: String,
        default: "#fff"
    },
    secondaryColor: {
        type: String,
        default: "rgba(255,255,255,.3)"
    }
});

</script>

<style scoped>
.spinner {
  display: inline-block;
  width: v-bind(width);
  height: v-bind(height);
  border: 3px solid v-bind(secondaryColor);
  border-radius: 50%;
  border-top-color: v-bind(color);
  animation: spin 1s ease-in-out infinite;
  -webkit-animation: spin 1s ease-in-out infinite;
}

@keyframes spin {
  to { -webkit-transform: rotate(360deg); }
}
@-webkit-keyframes spin {
  to { -webkit-transform: rotate(360deg); }
}
</style>