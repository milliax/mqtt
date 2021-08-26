require('dotenv').config()

const mqtt = require('mqtt')
const express = require('express');
const app = express();

const opt = {
    port: process.env.NODEJS_MQTT_PORT,
    username: process.env.NODEJS_MQTT_USERNAME,
    password: process.env.NODEJS_MQTT_PASSWD,
    clientId: process.env.NODEJS_MQTT_CLINTID,
}
const client = mqtt.connect(process.env.NODEJS_MQTT_HOST,opt)

client.on('connect',()=>{
    // push connected information
    client.publish('test','Service online')
})

app.get('/open',(req,res)=>{
    for(var i = 0;i < 2;++i){
        client.publish('curtain/status','open');
    }

    res.send("Good");
})
app.get('/close',(req,res)=>{
    for(var i = 0;i < 2;++i){
        client.publish('curtain/status','close');
    }
    res.send("Good");
})


/*client.on('connect',()=>{
    console.log('connected')
    // push connected information
    client.publish('test','Service online')
    client.subscribe('test')
})

client.on('message',(topic,msg)=>{
    console.log(`Recieved: ${topic}, Message: ${msg.toString()}`)
})*/

app.listen(20000,()=>{
    console.log(`program running on 20000`)
})