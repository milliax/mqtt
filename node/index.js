require('dotenv').config()

const mqtt = require('mqtt')

const opt = {
    port: process.env.NODEJS_MQTT_PORT,
    username: process.env.NODEJS_MQTT_USERNAME,
    password: process.env.NODEJS_MQTT_PASSWD,
    clientId: process.env.NODEJS_MQTT_CLINTID,
}

const client = mqtt.connect(process.env.NODEJS_MQTT_HOST,opt)

client.on('connect',()=>{
    console.log('connected')
    // push connected information
    client.publish('test','Service online')
    client.subscribe('test')
})

client.on('message',(topic,msg)=>{
    console.log(`Recieved: ${topic}, Message: ${msg.toString()}`)
})
