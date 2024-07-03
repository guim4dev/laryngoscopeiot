<template>
    <div class="sensors-wrapper">
        <VerticalBarSensor :value="tongueForceSensorDefinition.value.value" :max-value="maxForceSensorNormalizedValue" :danger-threshold="dangerThreshold" :warning-threshold="warningThreashold"
            :label="tongueForceSensorDefinition.label" icon-path="/icons/pressure_colorful.png" class="vertical-sensor"/>
        <MeasurementHistoryChart :measurements="measurementHistory" :max-y="maxForceSensorNormalizedValue" :total-points="measuresHistoryMaxLength" class="measurements-history" />
    </div>
</template>

<script setup lang="ts">
import { useEventSource } from '@vueuse/core'
import { ElMessage, MessageHandler } from 'element-plus';
import { watch, ref, onMounted } from "vue";
import VerticalBarSensor from '@/components/sensors/VerticalBarSensor.vue';
import MeasurementHistoryChart from '@/components/sensors/MeasurementHistoryChart.vue';
import { useI18n } from 'vue-i18n';
import { Haptics } from '@capacitor/haptics';
import type { Measurement, TimedMeasurement } from '@/services/DeviceApi';
const { t } = useI18n();

const maxForceSensorNormalizedValue = 20 // 20N
const warningThreashold = 10
const dangerThreshold = 15
const props = defineProps<{ src: string }>();
const emit = defineEmits(["started", "lostConnection"]);

// values range from 0 to 4095
// we need to normalize it to a range from 0N to 20N of force.
const getNormalizedForceSensorValue = (value: number) => {
    return Math.round((value / 4095) * maxForceSensorNormalizedValue)
}

const measurementHistory = ref<TimedMeasurement[]>([]);

// measurements arrive every 500ms. Wanna hold the measurements for the 30 seconds
const measuresHistoryMaxLength = 60

const addMeasurementToHistory = (measurement: TimedMeasurement) => {
    measurementHistory.value.push(measurement)
    if (measurementHistory.value.length > measuresHistoryMaxLength) {
        measurementHistory.value.shift()
    }
}

const tongueForceSensor = ref(0)
const teethStartedBeingPressedAt = ref<Date>()

const tongueForceSensorDefinition = {
    label: t('sensors.tongue_force_label'),
    value: tongueForceSensor
} as const

const { status, data, close } = useEventSource(props.src, ["sensorData"], {
    autoReconnect: {
        retries: 3,
        delay: 1000,
        onFailed() {
            ElMessage({ message: t('sensors.failed_to_connect'), type: 'error' })
        },
    },
})

let lastReceivedMessageAt: Date | undefined = undefined

const handleLostConnection = () => {
    isConnected.value = false
    close()
    emit('lostConnection')
    ElMessage({ message: t('sensors.connection_lost'), type: 'error' })
}

const isConnected = ref(false)
watch(status, (newStatus) => {
    if (newStatus === 'CLOSED' && isConnected.value) {
        handleLostConnection()
    }

    if (newStatus === 'OPEN') {
        emit('started')
        isConnected.value = true
    }
})

let teethMessageHandler: MessageHandler | null = null

// put message at bottom of page, so it doesn't overlap with the sensors and the video. Space from the bottom should be around 200px
const messagePositionOffset = window.innerHeight - 150

const handleTeethBeingTouched = () => {
    if (!teethMessageHandler) {
        teethMessageHandler = ElMessage({ message: t('sensors.teeth_being_touched'), type: 'error', duration: 0, offset: messagePositionOffset })
    }
    if (!teethStartedBeingPressedAt.value) {
        teethStartedBeingPressedAt.value = new Date()
    }

    Haptics.vibrate()
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
    lastReceivedMessageAt = new Date();

    const parsedData = JSON.parse(newData) as Measurement
    const { tongueForce, teethPressed } = parsedData
    tongueForceSensor.value = getNormalizedForceSensorValue(tongueForce)
    if (teethPressed) {
        handleTeethBeingTouched()
    } else {
        handleTeethNotBeingTouched()
    }
    addMeasurementToHistory({ teethPressed, tongueForce: tongueForceSensor.value, timestamp: lastReceivedMessageAt.getTime() })
})

onMounted(() => {
    setInterval(() => {
        if (lastReceivedMessageAt && isConnected.value) {
            const now = new Date()
            const diff = now.getTime() - lastReceivedMessageAt.getTime()
            if (diff > 5000) { // 5 seconds without receiving a message
                handleLostConnection()
            }
        }
    }, 2000) // check every 2 seconds if we are still connected
})
</script>

<style scoped>
.sensors-wrapper {
    display: flex;
    gap: 1rem;
    align-items: center;
    height: 100%;
    width: 100%;
}

.vertical-sensor {
    max-width: 40%;
}

.measurements-history {
    max-width: 500px;
}
</style>