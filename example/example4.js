var Tail = require("../tailnative");
var http = require("http");

var filename = process.argv[2];

if (!filename)
    console.log("Usage: node http-realtime-example.js filename");

var tail = new Tail(filename, "-");

console.log(tail);

tail.on('data', function(data){
	console.log(data);
});