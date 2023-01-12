<template>
  <div class="columns is-mobile is-centered">
     <table class="table is-striped is-narrow ">
      <thead>
        <tr>
          <th class="is-size-3"> Engine </th>
          <th class="is-size-3"> Speed </th>
          <!-- <th class="is-size-3"> Punkte </th> -->
          <th class="is-size-3"> % </th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td>
            0
          </td>
          <td>
            <input class="slider is-fullwidth is-success" step="1" min="0" max="75" value="0" type="range" v-model="engine_0" @change="update_speed(engine_0,0)">
          </td>
          <td>{{engine_0}}</td>
        </tr>
        <tr>
          <td>
            1
          </td>
          <td>
            <input class="slider is-fullwidth is-success" step="1" min="0" max="75" value="0" type="range" v-model="engine_1" @change="update_speed(engine_1,1)">
          </td>
          <td>{{engine_1}}</td>
        </tr>
        <tr>
          <td>
            2
          </td>
          <td>
            <input class="slider is-fullwidth is-success" step="1" min="0" max="75" value="0" type="range" v-model="engine_2" @change="update_speed(engine_2,2)">
          </td>
          <td>{{engine_2}}</td>
        </tr>
        <tr>
          <td>
            3
          </td>
          <td>
            <input class="slider is-fullwidth is-success" step="1" min="0" max="75" value="0" type="range" v-model="engine_3" @change="update_speed(engine_3, 3)">
          </td>
          <td>{{engine_3}}</td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<script>
import axios from 'axios'
export default {
  name: 'HelloWorld',
  props: {
    msg: String
  },
  created () {
    // axios.get('/getDaily').then((res) => {
    //   this.daily = res.data
    // })
  },
  data () {
    return {
      engine_0: 0,
      engine_1: 0,
      engine_2: 0,
      engine_3: 0,
      insertMode: false,
      editMode: false,
      usrInput: '',
      daily: [
      ],
      weekly: []
    }
  },
  methods: {
    update_speed (value, engineNumber) {
      let valueAsString = value.toString()
      while (valueAsString.length < 3) valueAsString = '0' + valueAsString
      console.log(valueAsString, value, engineNumber)
      valueAsString = engineNumber.toString() + valueAsString
      console.log(valueAsString)
      axios.post(`http://192.168.178.51:5000/set_speed/${valueAsString}`).then(() => {
      })
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
</style>
