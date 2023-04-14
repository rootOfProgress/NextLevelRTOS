<template>
  <div class="tile is-ancestor">
    <div class="tile is-vertical is-8">
      <div class="tile">
        <div class="tile is-parent is-vertical">
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 0</p>
            <p>{{ (engine_0 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <div>
              <vue-speedometer 
                :needleHeightRatio="0.7"
                :maxSegmentLabels="1"
                :segments="4"
                :minValue="0"
                :height="180"
                :maxValue="30000"
                :customSegmentStops="[0, 22000, 25000, 30000]"
                :segmentColors='["#bbffb5", "#faffc4", "#ffcbc4"]'
                :value=rpm.engine_0
                needleColor="#757474"
                textColor="#000000"
              />
            </div>
            <input class="slider is-fullwidth is-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_0" @change="update_speed(engine_0,0)">
          </article>
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 1</p>
            <p>{{ (engine_1 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <div>
              <vue-speedometer 
                :needleHeightRatio="0.7"
                :maxSegmentLabels="1"
                :segments="4"
                :minValue="0"
                :height="180"
                :maxValue="30000"
                :customSegmentStops="[0, 22000, 25000, 30000]"
                :segmentColors='["#bbffb5", "#faffc4", "#ffcbc4"]'
                :value=rpm.engine_1
                needleColor="#757474"
                textColor="#000000"
              />
            </div>
            <input class="slider is-fullwidth is-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_1" @change="update_speed(engine_1,1)">
          </article>
        </div>
        <div class="tile is-parent is-vertical ">
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 2</p>
            <p>{{ (engine_2 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <div>
              <vue-speedometer 
                :needleHeightRatio="0.7"
                :maxSegmentLabels="1"
                :segments="4"
                :minValue="0"
                :height="180"
                :maxValue="30000"
                :customSegmentStops="[0, 22000, 25000, 30000]"
                :segmentColors='["#bbffb5", "#faffc4", "#ffcbc4"]'
                :value=rpm.engine_2
                needleColor="#757474"
                textColor="#000000"
              />
            </div>
            <input class="slider is-fullwidth is-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_2" @change="update_speed(engine_2,2)">
          </article>
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 3</p>
            <p>{{ (engine_3 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <!-- <p> {{ rpm.engine_3 }} <i>rpm</i> </p> -->
            <div>
              <vue-speedometer 
                :needleHeightRatio="0.7"
                :maxSegmentLabels="1"
                :segments="4"
                :minValue="0"
                :height="180"
                :maxValue="30000"
                :customSegmentStops="[0, 22000, 25000, 30000]"
                :segmentColors='["#bbffb5", "#faffc4", "#ffcbc4"]'
                :value=rpm.engine_3
                needleColor="#757474"
                textColor="#000000"
              />
            </div>
            <input class="slider is-fullwidth has-text-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_3" @change="update_speed(engine_3,3)">
          </article>
        </div>
      </div>
      <div class="tile is-parent">
        <article class="tile is-child notification is-grey">
          <p class="title"> 
            <span @click='toggleOSPane = !toggleOSPane' class="has-text-link">
              <i class="fa fa-angle-left"  aria-hidden="true">
              </i>
            </span>
              OS Health
            <span @click='toggleOSPane = !toggleOSPane' class="has-text-link">
              <i class="fa fa-angle-right" aria-hidden="true">
              </i>
            </span>
          </p>
          <div class="content" v-if="toggleOSPane">
            <table class="table is-striped is-narrow ">
            <thead>
              <tr>
                <th>CPU Load</th>
                <th>Bytes Reserved</th>
                <th>Bytes Used</th>
                <th>RAM Utilisation</th>
                <th>Running Tasks</th>
                <th>Waiting Tasks</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td> {{ oshealth.cpu_load }} <i>%</i> </td>
                <td> {{ oshealth.total_byte_alloced }} </td>
                <td> {{ oshealth.total_byte_used  }} </td>
                <td> {{ (oshealth.total_byte_alloced / oshealth.ram_size) * 100 }} <i>%</i> </td>
                <!-- <td> {{ oshealth.ram_size }} <i>%</i> </td> -->
                <td> {{ oshealth.running_tasks }} </td>
                <td> {{ oshealth.waiting_tasks }} </td>
              </tr>
            </tbody>
          </table>
          </div>
          <div class="content" v-else>
            <table class="table is-striped is-narrow ">
            <tbody>
              <tr>
                <td> <button class="button is-link is-light is-rounded " @click=reboot()>Reboot</button> </td>
                <td>  
                  <div class="dropdown is-hoverable">
                    <div class="dropdown-trigger">
                      <button class="button" aria-haspopup="true" aria-controls="dropdown-menu4">
                        <span>Show binary packages...</span>
                        <span class="icon is-small">
                          <i class="fas fa-angle-down" aria-hidden="true"></i>
                        </span>
                      </button>
                    </div>
                    <div class="dropdown-menu is-up" id="dropdown-menu4" role="menu">
                      <div class="dropdown-content is-up">
                        <div class="dropdown-item">
                          <p v-for="p in packages" @click="selectPackage(p)"> {{ p }} </p>
                        </div>
                      </div>
                    </div>
                  </div>
                </td>
                <td> 
                  <button class="button is-link is-light is-rounded" :class="{'is-loading' : packageUploadInProgress} " @click="uploadPackage()">Upload "{{ selectedPackage }}" </button>
                </td>
                <td> 
                  <button class="button is-link is-light is-rounded is-danger" @click="uploadI2c()">Upload i2c </button>
                </td>
              </tr>
            </tbody>
          </table>
          </div>
        </article>
      </div>
    </div>
    <div class="tile is-parent">
      <article class="tile is-child notification is-grey">
        <div class="content">
          <p class="title">Sensor Readings</p>
          <p class="subtitle">Environment</p>
          <table class="table is-striped is-narrow ">
            <thead>
              <tr>
                <th>Altitude</th>
                <th>Temperature</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td> {{ sensorReadings.environment.altitude }} <i>m</i> </td>
                <td> {{ sensorReadings.environment.temp }} <i>°</i> </td>
              </tr>
            </tbody>
          </table>
          <p class="subtitle">Plane</p>
          <table class="table is-striped is-narrow ">
            <thead>
              <tr>
                <th>x</th>
                <th>y</th>
                <th>z</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td> {{ (sensorReadings.plane.x / 2.83).toFixed(3) }}  <i>°</i> </td>
                <td> {{ (sensorReadings.plane.y / 2.83).toFixed(3) }}  <i>°</i> </td>
                <td> {{ (sensorReadings.plane.z / 2.83).toFixed(3) }}  <i>°</i> </td>
              </tr>
              <tr>
                <td></td>
                <td>
                  <button class="button is-small is-success" :class="{'is-danger' : observingPosition}" @click="observingPosition = !observingPosition"> Observe Position </button>
                </td>
                <td></td>
              </tr>
            </tbody>
          </table>
        </div>
      </article>
    </div>
  </div>
</template>

<script>
import * as d3 from "d3";
import axios from 'axios'
// import VueSpeedMeter from "vue-speed-meter";
import VueSpeedometer from "vue-speedometer"
export default {
  name: 'SpeedRegulation',
  components: {
    VueSpeedometer,
  },
  props: {
    msg: String
  },
  created () {
    // axios.get('/getDaily').then((res) => {
    //   this.daily = res.data
    // })
    setInterval(() => {
      if (this.observingPosition) {
        this.observingPosition = false
        axios.get('/position').then((res) => {
          this.sensorReadings.plane.x = res.data.x
          this.sensorReadings.plane.y = res.data.y
          this.sensorReadings.plane.z = res.data.z
          this.observingPosition = true
        })
      }
      // axios.get('/rpm').then((res) => {
      //   this.rpm.engine_3 = res.data[0].engine_3
      // })
    }, 1000)
  },
  mounted() {
    axios.get('/get_packages').then((res) => {
      this.packages = res.data
    })
    
    axios.get('/lifetime').then((res) => {
      this.oshealth = res.data
    })  
  },
  data () {
    return {
      customStyle: {
        size: 200,
        needleColor: "#eb4034",
        mainBackgroundColor: "#e3e3e3",
        scaleValuesColor: "#001aff",
        scaleStep: 2000
      },
      customCurrentValue: 40,
      foo: [
        { date: "24-Apr-07", amount: 93.24 },
        { date: "25-Apr-07", amount: 95.35 },
        { date: "26-Apr-07", amount: 98.84 },
        { date: "27-Apr-07", amount: 99.92 },
        { date: "30-Apr-07", amount: 99.8 },
        { date: "1-May-07", amount: 99.47 },
        { date: "2-May-07", amount: 100.39 },
        { date: "3-May-07", amount: 100.4 },
        { date: "4-May-07", amount: 100.81 },
        { date: "7-May-07", amount: 103.92 },
        { date: "8-May-07", amount: 105.06 },
        { date: "9-May-07", amount: 106.88 },
        { date: "10-May-07", amount: 107.34 },
      ],
      observingPosition: false,
      selectedPackage: "",
      isPackageSelected: false,
      packageUploadInProgress: false,
      packages: [],
      toggleOSPane: true,
      sensorReadings: {
        environment: {
          altitude: 0,
          temp: 0
        },
        plane: {
          x: 0,
          y: 0,
          z: 0
        }
      },
      oshealth: {
        num_of_allocs: 0,
        num_of_deallocs: 0,
        ram_size: 0,
        total_byte_alloced: 0,
        total_byte_used: 0,
        os_data_end: 0,
        free_useable: 0,
        waiting_tasks: 0,
        running_tasks: 0,
        cpu_load: 0
      },
      rpm: {
        engine_0: 0,
        engine_1: 0,
        engine_2: 0,
        engine_3: 0
      },
      engine_0: 0,
      engine_1: 0,
      engine_2: 0,
      engine_3: 0,
      pwmMax: 100,
      interval: 0
    }
  },
  methods: {
    selectPackage (p) {
      this.selectedPackage = p
      this.isPackageSelected = true
    },
    getLifetimeInfo () {
      axios.get(`/lifetime`).then((response) => {
        this.oshealth = response.data
      })
    },
    fetchRPM () {
          axios.get('/rpm').then((res) => {
           console.log(res);
           this.rpm.engine_3 = res.data.engine_3
         })
      //     axios.get('/rpm').then((res) => {
        //       this.rpm.engine_3 = res.data[0].engine_3
      // this.interval = setInterval(() => axios.get('/rpm').then((res) => {
      //      console.log(res);
      //      this.rpm.engine_3 = res.data.engine_3
      //    }), 5000);
      // setInterval(() => {
      //   axios.get('/rpm').then((res) => {
      //     console.log(res);
      //     this.rpm.engine_3 = res.data.engine_3
      //   }), 1000
      //         })
    },
    uploadPackage () {
      this.packageUploadInProgress = true
      axios.post(`/upload/${this.selectedPackage}`).then(() => {
        this.packageUploadInProgress = false
        this.selectedPackage = ""
      })
    },
    reboot () {
      axios.post(`/reboot`).then(() => {
        this.getLifetimeInfo()
      })
    },
    uploadI2c () {
      axios.post(`/upload/i2c`).then(() => {
        this.packageUploadInProgress = false
        this.selectedPackage = ""
      })
    },
    update_speed (value, engineNumber) {
      let valueAsString = value.toString()
      while (valueAsString.length < 3) valueAsString = '0' + valueAsString
      valueAsString = engineNumber.toString() + valueAsString
      axios.post(`/set_speed/${valueAsString}`).then(() => {
      })
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
</style>
