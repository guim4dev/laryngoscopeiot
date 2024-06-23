<template>
    <div class="dot-loader" />
</template>

<script setup lang="ts">
defineProps({
    size: {
        type: String,
        default: "35px"
    },
    color: {
        type: String,
        default: "#000"
    }
})
</script>

<style scoped>
.dot-loader {
  width: v-bind(size);
  aspect-ratio: 1;
  --_g: no-repeat radial-gradient(circle closest-side,v-bind(color) 90%,#0000);
  background: 
    var(--_g) 0    0,
    var(--_g) 0    100%,
    var(--_g) 100% 100%;
  background-size: 40% 40%;
  animation:l11 1s infinite linear;
}
@keyframes l11 {
    25% {background-position:100% 0   ,0 100%,100% 100%}
    50% {background-position:100% 0   ,0 0   ,100% 100%}
    75% {background-position:100% 0   ,0 0   ,0    100%}
    100%{background-position:100% 100%,0 0   ,0    100%}
}
</style>