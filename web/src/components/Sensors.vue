<template>
    <div class="sensors-wrapper">
        <VerticalBarSensor v-for="sensor in sensors" :key="sensor.label" :value="sensor.value.value"
            :label="sensor.label" :color="sensor.color" />
    </div>
</template>

<script setup lang="ts">
import { useEventSource } from '@vueuse/core'
import { ElMessage, MessageHandler } from 'element-plus';
import { watch, ref } from "vue";
import VerticalBarSensor from '@/components/sensors/VerticalBarSensor.vue';
import { useI18n } from 'vue-i18n';
const { t } = useI18n();

const props = defineProps<{ src: string }>();
const emit = defineEmits(["started"]);

// TODO:
// values range from 0 to 4095
// we need to normalize it to a range from 0g to 100N of force. It should be treated as a log10 scale relationship

// TEMP: for now, we are just normalizing it to a 0 to 100 scale
const getNormalizedForceSensorValue = (value: number) => {
    return Math.round((value / 4095) * 100)
}

const primaryForceSensor = ref(0)
const secondaryForceSensor = ref(0)
const teethStartedBeingPressedAt = ref<Date>()

const sensors = [
    {
        label: t('sensors.primary_force_label'),
        value: primaryForceSensor,
        color: 'red'
    },
    {
        label: t('sensors.secondary_force_label'),
        value: secondaryForceSensor,
        color: 'orange'
    }
] as const

const { status, data } = useEventSource(props.src, ["sensorData"], {
    autoReconnect: {
        retries: 3,
        delay: 1000,
        onFailed() {
            ElMessage({ message: t('sensors.failed_to_connect'), type: 'error' })
        },
    },
})

watch(status, (newStatus) => {
    if (newStatus === 'CLOSED') {
        ElMessage({ message: t('sensors.connection_closed'), type: 'error' })
    }

    if (newStatus === 'CONNECTING') {
        console.log('Sensors EventSource is connecting...')
    }

    if (newStatus === 'OPEN') {
        console.log('Sensors EventSource is connected')
        emit('started')
    }
})

let teethMessageHandler: MessageHandler | null = null

const handleTeethBeingTouched = () => {
    if (!teethMessageHandler) {
        teethMessageHandler = ElMessage({ message: t('sensors.teeth_being_touched'), type: 'error', duration: 0 })
    }
    if (!teethStartedBeingPressedAt.value) {
        teethStartedBeingPressedAt.value = new Date()
    }

    if (navigator.vibrate) {
        navigator.vibrate(200)
    } else {
        console.log('Vibrating not supported')
    }
}

const handleTeethNotBeingTouched = () => {
    if (teethStartedBeingPressedAt.value) {
        teethMessageHandler?.close()
        teethMessageHandler = null
        teethStartedBeingPressedAt.value = undefined
    }
}

watch(data, (newData) => {
    if (!newData || newData?.startsWith("connected")) return

    const parsedData = JSON.parse(newData)
    const { primaryForce, secondaryForce, teethPressed } = parsedData
    primaryForceSensor.value = getNormalizedForceSensorValue(primaryForce)
    secondaryForceSensor.value = getNormalizedForceSensorValue(secondaryForce)
    if (teethPressed) {
        handleTeethBeingTouched()
    } else {
        handleTeethNotBeingTouched()
    }
})
// display grid with 1 one line and 2 columns of same size
</script>

<style scoped>
.sensors-wrapper {
    display: grid;
    grid-auto-flow: column;
    grid-auto-columns: 2fr;
    gap: 1rem;
    align-self: flex-start;
    justify-self: center;
    width: 100%
}
</style>