var Tail = require("../tailnative").Tail;
var tail = new Tail();

var inst = tail.start("testfile", function(err, data) {
    if (err){
        console.log(err);
        return;
    }
    console.log(data);
});