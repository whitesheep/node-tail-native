var tail = require("../tailnative");

var t = tail.start(__dirname + "/testfile");
    
t.on('data', function(data){
    console.log(this.file + ': ' + data);
});

t.on('error', function(){
    console.log('error');
});

t.on('end', function(){
    console.log('end');
});

setTimeout(function(){
    t.stop();
}, 2000);