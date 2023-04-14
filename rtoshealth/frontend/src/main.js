import Vue from 'vue'
import App from './App.vue'
import store from './store'
import '../node_modules/bulma/bulma.sass'
import '@fortawesome/fontawesome-free/css/all.css'
import '@fortawesome/fontawesome-free/js/all.js'
import 'bulma-slider'
import axios from 'axios'
Vue.config.productionTip = false
axios.defaults.baseURL = 'http://192.168.178.51:5000'
import ReactSpeedometer from "react-d3-speedometer"

new Vue({
  store,
  render: h => h(App)
}).$mount('#app')
