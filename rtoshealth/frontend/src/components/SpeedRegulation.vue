<template>
  <div class="tile is-ancestor">
    <div class="tile is-vertical is-8">
      <div class="tile">
        <div class="tile is-parent is-vertical">
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 0</p>
            <input class="slider is-fullwidth is-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_0" @change="update_speed(engine_0,0)">
            <p>{{ (engine_0 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <p> {{ rpm.engine_0 }} <i>rpm</i> </p>
          </article>
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 1</p>
            <input class="slider is-fullwidth is-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_1" @change="update_speed(engine_1,1)">
            <p>{{ (engine_1 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <p> {{ rpm.engine_1 }} <i>rpm</i> </p>
          </article>
        </div>
        <div class="tile is-parent is-vertical ">
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 2</p>
            <input class="slider is-fullwidth is-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_2" @change="update_speed(engine_2,2)">
            <p>{{ (engine_2 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <p> {{ rpm.engine_2 }} <i>rpm</i> </p>
          </article>
          <article class="tile is-child notification is-grey">
            <p class="title">Engine 3</p>
            <input class="slider is-fullwidth has-text-success" step="1" min="0" :max="pwmMax" value="0" type="range" v-model="engine_3" @change="update_speed(engine_3,3)">
            <p>{{ (engine_3 / (pwmMax / 100)).toFixed(2) }} <i>%</i> </p>
            <p> {{ rpm.engine_3 }} <i>rpm</i> </p>
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
                <th>RAM utilisation</th>
                <th>Tasks</th>
                <th>Active Tasks</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td> {{ oshealth.cpu_load }} <i>%</i> </td>
                <td> {{ oshealth.total_byte_alloced }} </td>
                <td> {{ oshealth.total_byte_used  }} </td>
                <td>RAM utilisation <i>%</i> </td>
                <td> {{ oshealth.total_scheduled_tasks }} </td>
                <td> {{ oshealth.total_scheduled_tasks - oshealth.waiting_tasks }} </td>
              </tr>
            </tbody>
          </table>
          </div>
          <div class="content" v-else>
            <table class="table is-striped is-narrow ">
            <tbody>
              <tr>
                <td> <button class="button is-link is-light is-rounded">Reboot</button> </td>
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
                    <div class="dropdown-menu" id="dropdown-menu4" role="menu">
                      <div class="dropdown-content">
                        <div class="dropdown-item">
                          <p v-for="p in packages" @click="selectPackage(p)"> {{ p }} </p>
                        </div>
                      </div>
                    </div>
                  </div>
                </td>
                <td> 
                  <button class="button is-link is-light is-rounded" :class="{'is-static' : !isPackageSelected, 'is-loading' : packageUploadInProgress} " @click="uploadPackage()">Upload "{{ selectedPackage }}" </button>
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
                <td> {{ sensorReadings.plane.x }} <i>°</i> </td>
                <td> {{ sensorReadings.plane.y }} <i>°</i> </td>
                <td> {{ sensorReadings.plane.z }} <i>°</i> </td>
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
export default {
  name: 'SpeedRegulation',
  props: {
    msg: String
  },
  created () {
    // axios.get('/getDaily').then((res) => {
    //   this.daily = res.data
    // })
  },
  mounted() {
    axios.get('/get_packages').then((res) => {
      this.packages = res.data
    })
    
    axios.get('/lifetime').then((res) => {
      this.oshealth = res.data
    })
    // const width = 300;
    // const height = 200;
    // const data = [];

    // const svg = d3.select("svg").attr("width", width).attr("height", height);
    // const g = svg.append("g");

    // //2. Parse the dates
    // const parseTime = d3.timeParse("%d-%b-%y");

    // //3. Creating the Chart Axes
    // const x = d3
    //   .scaleTime()
    //   .domain(
    //     d3.extent(this.foo, function (d) {
    //       return parseTime(d.date);
    //     })
    //   )
    //   .rangeRound([0, width]);

    // const y = d3
    //   .scaleLinear()
    //   .domain(
    //     d3.extent(this.foo, function (d) {
    //       return d.amount;
    //     })
    //   )
    //   .rangeRound([height, 0]);

    // //4. Creating a Line
    // const line = d3
    //   .line()
    //   .x(function (d) {
    //     return x(parseTime(d.date));
    //   })
    //   .y(function (d) {
    //     return y(d.amount);
    //   });

    // //5. Appending the Axes to the Chart
    // g.append("g")
    //   .attr("transform", "translate(0," + height + ")")
    //   .call(d3.axisBottom(x))
    //   .append("text")
    //   .attr("fill", "#000")
    //   .attr("y", 6)
    //   .attr("dy", "0.71em")
    //   .attr("text-anchor", "end")
    //   .text("FOO");
    // g.append("g")
    //   .call(d3.axisLeft(y))
    //   .append("text")
    //   .attr("fill", "#000")
    //   .attr("transform", "rotate(-90)")
    //   .attr("y", 6)
    //   .attr("dy", "0.71em")
    //   .attr("text-anchor", "end")
    //   .text("Price ($)");    

    // //6. Appending a path to the Chart
    // g.append("path")
    //   .datum(this.foo)
    //   .attr("fill", "none")
    //   .attr("stroke", "steelblue")
    //   .attr("stroke-width", 1.5)
    //   .attr("d", line);
  
  },
  data () {
    return {
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
        num_of_fractial_allocs: 0,
        total_byte_alloced: 0,
        total_byte_used: 0,
        os_data_end: 0,
        free_useable: 0,
        waiting_tasks: 0,
        total_scheduled_tasks: 0,
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
      pwmMax: 999,
    }
  },
  methods: {
    selectPackage (p) {
      this.selectedPackage = p
      this.isPackageSelected = true
    },
    getLifetimeInfo()
    {
      axios.get(`/lifetime`).then((response) => {
        this.oshealth = response.data
      })
    },
    uploadPackage () {
      this.packageUploadInProgress = true
      axios.post(`/upload/${this.selectedPackage}`).then(() => {
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
