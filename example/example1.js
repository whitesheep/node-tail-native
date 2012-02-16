var Tail = require("../tail").Tail;
var tail = new Tail();

tail.start("testfile", function(err, data) {
    if (err){
        console.log(err);
        return;
    }
    console.log(data);
});