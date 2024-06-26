<template>
    <div class="chart-wrapper">
        <apexchart :options="chartOptions" :series="chartSeries" height="100%" width="100%"/>
    </div>
</template>

<script setup lang="ts">
import { computed} from 'vue';
import type { TimedMeasurement } from '@/services/DeviceApi';

const props = defineProps<{
    measurements: TimedMeasurement[],
    maxY: number,
    totalPoints: number
}>()

const teethSeries = computed(() => {
    return {
        data: props.measurements.map(measurement => {
            return {
                x: measurement.timestamp,
                y: measurement.teethPressed ? props.maxY : 0 // add column for teeth pressing event 
            }
        }),
        type: 'area',
        color: 'red',
    }
})

const tongueForceSeries = computed(() => {
    return {
        data: props.measurements.map(measurement => {
            return {
                x: measurement.timestamp,
                y: measurement.tongueForce
            }
        }),
        type: 'line',
        color: '#39FF14'
    }
})

const chartSeries = computed(() => {
    return [teethSeries.value, tongueForceSeries.value]
})

const chartOptions = computed(() => {
    return {
        grid: {
            show: false
        },
        legend: {
            show: false
        },
        dataLabels: {
            enabled: false
        },
        chart: {
            animations: {
                enabled: false
            },
            toolbar: {
                show: false
            }
        },
        stroke: {
            curve: 'straight',
            width: 2
        },
        xaxis: {
            type: 'datetime',
            labels: {
                show: false
            }   
        },
        yaxis: {
            max: props.maxY,
            min: 0,
            show: true,
            labels: {
                style: {
                    colors: ['white'],
                    fontSize: '8px'
                }
            }   
        },
        fill: {
            type: 'gradient',
            gradient: {
                inverseColors: false,
                shade: 'light',
                type: "vertical",
                opacityFrom: [0.7, 1],
                opacityTo: [0.3, 1], 
                stops: [0, 100, 100, 100]
          }
        }
    }
})

</script>

<style scoped>
.chart-wrapper {
    height: 100%;
    width: 100%;
}


</style>