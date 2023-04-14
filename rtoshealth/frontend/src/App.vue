<template>
  <div id="app" class="has-background-grey-light">
    <SpeedRegulation/>
  </div>
</template>

<script>
import SpeedRegulation from './components/SpeedRegulation.vue'

export default {
  name: 'App',
  components: {
    SpeedRegulation
  }
}
</script>

<style lang="scss">
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 20px;
}
</style>
