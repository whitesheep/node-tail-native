var Tail = require("../tailnative");
var http = require("http");

var filename = process.argv[2];

if ( typeof filename === 'undefined' ) {
    console.error("Usage: node http-realtime-example.js filename");
    process.exit();
}

var tail = new Tail(__dirname + "/testfile");

http.createServer(function(req,res){
    res.writeHead(200,{"Content-Type": "text/plain"});
    tail.on('data', function (data) {
        console.log(data);
        res.write(data + "\r\n");
    });  
}).listen(8000);

