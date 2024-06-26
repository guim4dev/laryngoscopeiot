<template>
    <div class="sensor-wrapper">
        <div class="details">
            <span class="label">{{ label }}</span>
            <div v-if="showValue" class="value">{{ value }}N</div>
        </div>

        <div class="bar" :style="{ height: `${value}%`, width: '20px', backgroundColor: color }">
        </div>
    </div>
</template>

<script setup lang="ts">
defineProps({
    value: Number,
    label: String,
    maxValue: {
        type: Number,
        default: 100
    },
    showValue: {
        type: Boolean,
        default: true
    },
    color: {
        type: String,
        default: 'red'
    }
}); // this number is a number from 0 to 100
</script>

<style scoped>
.sensor-wrapper {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 1rem;
    height: 100%;
    padding: 0.5rem;
}

.details {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 0.5rem;
    text-align: center;
}

.label {
    font-size: 0.75rem;
    word-break: keep-all;
    text-align: center;
    font-weight: italic;
}

.bar {
    border-radius: 3px;
    transition: height 0.3s;
    align-self: flex-end
}
</style>