var Tail = require("../tailnative");
var util = require('util');
var Stream = require('stream').Stream;

var test_stream = function(){
	this.writable = true;
  	this.readable = true;
}

util.inherits(test_stream, Stream);

test_stream.prototype.write = function (buffer) {
    console.log("Streamed " + buffer);
}

test_stream.prototype.end = function() {
  	this.emit('end');
}

test_stream.prototype.error = function() {
	this.emit('error');
}

var ts = new test_stream();
var tail = new Tail(__dirname + "/testfile");

tail.pipe(ts);