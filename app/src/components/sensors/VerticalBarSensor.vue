<template>
    <div class="sensor-wrapper">
        <div class="details">
            <span class="label">{{ label }}</span>
            <div v-if="showValue" class="value">{{ `${value}${reachedMax ? '+' : ''}` }} N</div>
        </div>

        <div class="bar" :style="{ height: `${visibleHeight}%`, width: '20px', backgroundColor: color }" />
        <!-- <img v-if="iconPath" :src="iconPath" alt="Force sensor icon" class="icon" /> -->
    </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';

const props = defineProps({
    value: {
        type: Number,
        default: 0,
        required: true
    },
    iconPath: {
        type: String,
    },
    label: String,
    maxValue: {
        type: Number,
        default: 100
    },
    showValue: {
        type: Boolean,
        default: true
    },
    warningThreshold: {
        type: Number,
        default: 60
    },
    dangerThreshold: {
        type: Number,
        default: 80
    },
    safeColor: {
        type: String,
        default: '#39FF14'
    },
    warningColor: {
        type: String,
        default: 'yellow'
    },
    dangerColor: {
        type: String,
        default: 'red'
    }
}); // this number is a number from 0 to 100

const visibleHeight = computed(() => (Math.max(props.value, 0.05*props.maxValue)/props.maxValue)*100) // height needs to be at least 5%
const reachedMax = computed(() => props.value >= props.maxValue);

const color = computed(() => {
    if (props.value < props.warningThreshold) {
        return props.safeColor;
    }

    if (props.value < props.dangerThreshold) {
        return props.warningColor;
    }

    return props.dangerColor;
});
</script>

<style scoped>
.sensor-wrapper {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 1rem;
    height: 100%;
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
    font-size: 0.6rem;
    max-width: 150px;
    text-align: center;
    font-weight: 500;
}

.bar {
    border-radius: 4px;
    transition: height 0.3s, background-color 0.3s;
    align-self: flex-end
}

.icon {
    width: 30px;
    aspect-ratio: 1;
    object-fit: contain;
    align-self: flex-end;
}
</style>