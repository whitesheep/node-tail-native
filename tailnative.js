var Stream = require('stream').Stream;
var Tailnative = require(__dirname + '/build/Release/tailnative.node').Tail;
var util = require('util');

util.inherits(Tailnative, Stream);

module.exports = Tailnative;
