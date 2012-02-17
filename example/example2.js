var Tail = require("../tailnative").Tail;
var tail = new Tail();

var inst = tail.start("testfile", function(err, data) {
    if (err){
        console.log(err);
        return;
    }
    console.log("from js " + data);
});

t=setInterval(function(){
    if ( tail.stop(inst) )
        console.log("stopped");
    else
        console.log("alredy stopped");
    //clearInterval(t);
}, 5000);
