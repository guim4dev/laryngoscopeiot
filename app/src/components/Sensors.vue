<template>
    <div class="sensors-wrapper">
        <VerticalBarSensor v-for="sensor in sensors" :key="sensor.label" :value="sensor.value.value"
            :label="sensor.label" :color="sensor.color" />
    </div>
</template>

<script setup lang="ts">
import { useEventSource } from '@vueuse/core'
import { ElMessage, MessageHandler } from 'element-plus';
import { watch, ref, onMounted } from "vue";
import VerticalBarSensor from '@/components/sensors/VerticalBarSensor.vue';
import { useI18n } from 'vue-i18n';
import { Haptics } from '@capacitor/haptics';
const { t } = useI18n();

const props = defineProps<{ src: string }>();
const emit = defineEmits(["started", "lostConnection"]);

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

const handleTeethBeingTouched = () => {
    if (!teethMessageHandler) {
        teethMessageHandler = ElMessage({ message: t('sensors.teeth_being_touched'), type: 'error', duration: 0 })
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
    display: grid;
    grid-auto-flow: column;
    grid-auto-columns: 2fr;
    gap: 1rem;
    align-self: flex-start;
    justify-self: center;
    width: 100%
}
</style>