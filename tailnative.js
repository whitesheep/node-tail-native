var Stream = require('stream').Stream;
var Tailnative = require(__dirname + '/build/Release/tailnative.node').Tail;

Tailnative.prototype.__proto__ = Stream.prototype;

var Tail = function(){
    this.readable = true;
    this.writable = false;
}
Tail.prototype.__proto__ = Stream.prototype;

Tail.prototype.start = function(file, cb){
    var tn = new Tailnative();
    tn.file = file;
    tn.start(file);
    
    if ( typeof cb === 'function' )
        cb.call(this, tn);
   
    return tn;
}


module.exports = new Tail();
