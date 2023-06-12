// var http = require("http"); // 1 - Import Node.js core module
const mqtt = require("mqtt");
const client = mqtt.connect("mqtt://i87100c4.emqx.cloud", {
  username: "test",
  password: "test",
});
const mongoose = require("mongoose");
const { Double } = require("mongodb");
const uri =
  "mongodb://ritvik:ritvik@ac-riivo2z-shard-00-00.xa87djk.mongodb.net:27017,ac-riivo2z-shard-00-01.xa87djk.mongodb.net:27017,ac-riivo2z-shard-00-02.xa87djk.mongodb.net:27017/?ssl=true&replicaSet=atlas-vgcgjz-shard-0&authSource=admin&retryWrites=true&w=majority";
mongoose
  .connect(uri)
  .then(() => {
    console.log("mongo connected");
    client.on("message", function (topic, message) {
      //console.log(message.toString());
      let data = JSON.parse(message);
      console.log(data.helmetId);
      console.log(data.LAT);

      JSON.parse(message);
      const Helmet = mongoose.model("Helmet", {
        model: String,
        latitude: Number,
        longitude: Number,
        helmetId: Number,
      });
      const helmetInstance = new Helmet({
        model: "Model X",
        latitude: data.LAT,
        longitude: data.LONG,
        helmetId: data.helmetId,
      });
      helmetInstance.save().then(() => console.log("database updated"));
    });
  })
  .catch((err) => console.log(err));

client.once("connect", () => {
  console.log("Connected to broker");
});
client.on("reconnect", () => {
  console.log("reconnecting");
});
client.on("connect", function () {
  console.log("Waiting for message:");
  client.subscribe("shahrulnizam/out");
  client.publish("shahrulnizam/in", "ritvik mc");
});
