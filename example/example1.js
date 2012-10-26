var Tail = require("../tailnative");
var tail = new Tail(__dirname + "/testfile");

    
tail.on('data', function(data){
    console.log(this.file + ': ' + data);
});

tail.on('error', function(){
    console.log('error');
});

tail.on('end', function(){
    console.log('end');
});

setTimeout(function(){
    t.close();
}, 2000);