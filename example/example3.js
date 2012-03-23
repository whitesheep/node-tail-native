var tail = require("../tailnative");
var Stream = require('stream').Stream;

var test_stream = function(){
	this.writable = true;
  	this.readable = true;
}

test_stream.prototype.__proto__ = Stream.prototype;

test_stream.prototype.write = function (buffer) {
    console.log("Streamed " + buffer);
}

test_stream.prototype.end = function() {
  	this.emit('end');
}

test_stream.prototype.error = function() {
	this.emit('error');
}

ts = new test_stream();

tail.start(__dirname + "/testfile").pipe(ts);