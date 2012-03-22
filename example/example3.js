var tail = require("../tailnative");

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

test_stream.prototype.destroy = function() {
	this.emit('close');
}

ts = new test_stream();

tail.start(__dirname + "/testfile").pipe(ts);